#include "monitor/monitor.h"
#include "monitor/expr.h"
#include "monitor/watchpoint.h"
#include "nemu.h"

#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>

void cpu_exec(uint64_t);
WP* new_wp();
void free_wpn(int n);
void free_wp(WP* wp);
void print_watchpointsinfo();
bool check_watchpointsvalue();

/* We use the `readline' library to provide more flexibility to read from stdin. */
char* rl_gets() {
  static char *line_read = NULL;

  if (line_read) {
    free(line_read);
    line_read = NULL;
  }

  line_read = readline("(nemu) ");

  if (line_read && *line_read) {
    add_history(line_read);
  }

  return line_read;
}

static int cmd_c(char *args) {
  cpu_exec(-1);
  return 0;
}

static int cmd_q(char *args) {
  return -1;
}

static int cmd_help(char *args);

// Commands to implement in pa1
static int cmd_si(char *args);
static int cmd_info(char *args);
static int cmd_p(char *args);
static int cmd_x(char *args);
static int cmd_w(char *args);
static int cmd_d(char *args);

static struct {
  char *name;
  char *description;
  int (*handler) (char *);
} cmd_table [] = {
  { "help", "Display informations about all supported commands", cmd_help },
  { "c", "Continue the execution of the program", cmd_c },
  { "q", "Exit NEMU", cmd_q },

  /* TODO: Add more commands */
  { "si", "Let the program execute N instructions step by step", cmd_si },
  { "info", "Print registers' status for r, checkpoint informations for w", cmd_info },
  { "p", "Calculate the expression's value", cmd_p },
  { "x", "Scan the consecutive 4N bytes from Address EXPR", cmd_x },
  { "w", "Set watchpoint i.e. pause the program until the EXPR's value changes", cmd_w },
  { "d", "Delete the watchpoint which number is N", cmd_q }
};

#define NR_CMD (sizeof(cmd_table) / sizeof(cmd_table[0]))

static int cmd_help(char *args) {
  /* extract the first argument */
  char *arg = strtok(NULL, " ");
  int i;

  if (arg == NULL) {
    /* no argument given */
    for (i = 0; i < NR_CMD; i ++) {
      printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
    }
  }
  else {
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(arg, cmd_table[i].name) == 0) {
        printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
        return 0;
      }
    }
    printf("Unknown command '%s'\n", arg);
  }
  return 0;
}

static int cmd_si(char *args){
  char *arg = strtok(NULL, " ");
  if(arg!=NULL){
    cpu_exec(atoi(arg));
  }
  else{
    cpu_exec(1);
  }
  return 0;
}

static int cmd_info(char *args){
  char *arg = strtok(NULL, " ");
  if(arg==NULL){
    printf("please input subcmd for info..!\n");
    return 0;
  }
  
  if(strcmp(arg,"r")==0){
    for(int i=0;i<8;i++){
      printf("%s:0x%x\n", reg_name(i,4), reg_l(i));
    }
  }
  else if(strcmp(arg,"w")==0){
    print_watchpointsinfo();
  }
  else{
    printf("undefined subcmd for info..\n");
  }
  return 0;
}

static int cmd_x(char *args){
  char *arg1 = strtok(NULL, " ");
  if(arg1==NULL){
    printf("u shall input the parameter N to specify the consecutive N..\n");
    return 0;
  }
  int i_arg1 = atoi(arg1);
  char *arg2 = strtok(NULL, " ");
  /* TODO: now i just implement the function given accurate number, must fix it in 1-2 or 1-3*/
  if(arg2==NULL){
    printf("u shall input the parameter EXPR must generate from keyboard input..!\n");
    return 0;
  }
  uint32_t addr_begin = strtoul(arg2,NULL,16);
  for(int i=0;i<i_arg1;i++){
    printf("0x%x ", vaddr_read(addr_begin,1));
    addr_begin+=1;
  }
  printf("\n");
  return 0;
}

static int cmd_p(char *args){
  char *arg = strtok(NULL," ");
  if(arg==NULL){
    printf("please input the expression u wanna calculate..!\n");
    return 0;
  }
  bool is_finish=true;
  uint32_t ans = expr(arg,&is_finish);
  if(!is_finish){
    printf("please check your expression's format..!\n");
  }
  else{
    printf("%d\n", ans);
  }
  return 0;
}

static int cmd_w(char *args){
  char *arg = strtok(NULL, " ");
  if(arg == NULL){
    printf("u must clain the expression of watchpoint..!\n");
    return 0;
  }
  bool success;
  uint32_t res = expr(arg, &success);
  // printf("hello1\n");
  if(!success){
    printf("u must check the format of your expression..!\n");
    return 0;
  }
  // printf("hello2\n");
  WP *mywp = new_wp();
  // printf("hello3,%d\n",res);
  // printf("out new_wp():%d\n",mywp);
  // printf("out new wp() NO:%d\n", mywp->NO);
  mywp->value = res;
  // printf("hello4\n");
  // printf("%d\n", strlen(arg));
  for(int i=0;arg[i];i++){
    mywp->expr[i] = arg[i];
  }
  mywp->expr[strlen(arg)] = '\0';
  return 0;
}

static int cmd_d(char *args){
  char *arg = strtok(NULL, " ");
  if(arg == NULL){
    printf("u must claim the number of watchpoint to delete..!\n");
    return 0;
  }
  int aim = atoi(arg);
  free_wpn(aim);
  return 0;
}

void ui_mainloop(int is_batch_mode) {
  if (is_batch_mode) {
    cmd_c(NULL);
    return;
  }

  while (1) {
    char *str = rl_gets();
    char *str_end = str + strlen(str);

    /* extract the first token as the command */
    char *cmd = strtok(str, " ");
    if (cmd == NULL) { continue; }

    /* treat the remaining string as the arguments,
     * which may need further parsing
     */
    char *args = cmd + strlen(cmd) + 1;
    if (args >= str_end) {
      args = NULL;
    }

#ifdef HAS_IOE
    extern void sdl_clear_event_queue(void);
    sdl_clear_event_queue();
#endif

    int i;
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(cmd, cmd_table[i].name) == 0) {
        if (cmd_table[i].handler(args) < 0) { return; }
        break;
      }
    }

    if (i == NR_CMD) { printf("Unknown command '%s'\n", cmd); }
  }
}
