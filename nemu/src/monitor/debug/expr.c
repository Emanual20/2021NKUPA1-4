#include "nemu.h"

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <sys/types.h>
#include <regex.h>

enum {
  TK_NOTYPE = 256, 
  TK_OR, TK_AND,
  TK_EQ, TK_NEQ, TK_LOGAND, TK_LOGOR, TK_NOT,
  NUM_10, NUM_16

  /* TODO: Add more token types */

};

static struct rule {
  char *regex;
  int token_type;
} rules[] = {

  /* TODO: Add more rules.
   * Pay attention to the precedence level of different rules.
   */

  {" +", TK_NOTYPE},     // spaces
  {"!", TK_NOT},         // log-not
  {"\\*", '*'},          // multi
  {"\\/", '/'},          // div
  {"\\+", '+'},          // plus
  {"\\-", '-'},          // minus
  {"\\|", TK_OR},        // calc-or
  {"&", TK_AND},         // calc-and
  {"\\|\\|", TK_LOGOR},      // log-or
  {"&&", TK_LOGAND},         // log-and
  {"==", TK_EQ},         // equal
  {"!=", TK_NEQ},        // not-equal
  {"[1-9][0-9]*", NUM_10}, // num with radix 10
  {"0[xX][a-fA-F0-9]+", NUM_16}, // num with radix 16
  {"\\(", '('},          // l-parentheses
  {"\\)", ')'},          // r-patentheses
};

#define NR_REGEX (sizeof(rules) / sizeof(rules[0]) )

static regex_t re[NR_REGEX];

/* Rules are used for many times.
 * Therefore we compile them only once before any usage.
 */
void init_regex() {
  int i;
  char error_msg[128];
  int ret;

  for (i = 0; i < NR_REGEX; i ++) {
    ret = regcomp(&re[i], rules[i].regex, REG_EXTENDED);
    if (ret != 0) {
      regerror(ret, &re[i], error_msg, 128);
      panic("regex compilation failed: %s\n%s", error_msg, rules[i].regex);
    }
  }
}

typedef struct token {
  int type;
  char str[32];
} Token;

Token tokens[32];
int nr_token;

static bool make_token(char *e) {
  int position = 0;
  int i;
  regmatch_t pmatch;

  nr_token = 0;

  while (e[position] != '\0') {
    /* Try all rules one by one. */
    for (i = 0; i < NR_REGEX; i ++) {
      if (regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0) {
        char *substr_start = e + position;
        int substr_len = pmatch.rm_eo;

        Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s",
            i, rules[i].regex, position, substr_len, substr_len, substr_start);
        position += substr_len;

        /* TODO: Now a new token is recognized with rules[i]. Add codes
         * to record the token in the array `tokens'. For certain types
         * of tokens, some extra actions should be performed.
         */

        switch (rules[i].token_type) {
          case TK_NOTYPE: break;
          case NUM_10:
          case NUM_16:{
            for(int i=0;i<substr_len;i++){
              tokens[nr_token].str[i] = substr_start[i];
            }
            tokens[nr_token].str[substr_len] = '\0';
          }
          default: {
            tokens[nr_token].type = rules[i].token_type;
            nr_token++;
          }
        }
        break;
      }
    }

    if (i == NR_REGEX) {
      printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
      return false;
    }
  }

  return true;
}

bool check_pre_valid(int l, int r){
  int tot = 0;
  for(int i=l;i<=r;i++){
    if(tokens[i].type=='('){
      tot+=1;
    }
    else if(tokens[i].type==')'){
      tot-=1;
    }
    if(tot<0){
      return false;
    }
  }
  return tot==0;
}

bool check_parentheses(int l, int r, bool *success){
  if(!check_pre_valid(l,r)){
    *success=false;
    return *success;
  }
  if(tokens[l].type=='('&&tokens[r].type==')'){
    if(check_pre_valid(l+1,r-1)){
      return true;
    }
  }
  *success=true;
  return false;
}

static uint32_t operator2priority(Token tk){
  switch(tk.type){
    case TK_NOT:{
      return 2;
    }
    case '*':
    case '/':{
      return 3;
    }
    case '+':
    case '-':{
      return 4;
    }
    case TK_EQ:
    case TK_NEQ:{
      return 7;
    }
    case TK_AND:{
      return 8;
    }
    case TK_OR:{
      return 10;
    }
    case TK_LOGAND:{
      return 11;
    }
    case TK_LOGOR:{
      return 12;
    }
    default:{
      return 16;
    }
  }
}

static bool is_operator(Token tk){
  return operator2priority(tk)<=15;
}

static int find_operator(int l, int r, bool *success){
  printf("before find op, is_success:%d\n", *success);
  int now_max_power = -1, now_max_index = -1;
  int tot_parentheses = 0;
  for(int i=l;i<=r;i++){
    /*make sure the operator is not between lp and rp*/
    if(tokens[i].type=='('){
      tot_parentheses+=1;
      continue;
    }
    else if(tokens[i].type==')'){
      tot_parentheses-=1;
      continue;
    }

    /* NOTE: short circuit in C language!*/
    if(tot_parentheses!=0) continue;
    if(!is_operator(tokens[i])) continue;

    // printf("now_index:%d\n",i);

    int now_power = operator2priority(tokens[i]);
    if(now_power>now_max_power){
      now_max_power=now_power;
      now_max_index=i;
    }
  }
  printf("after find op, is_success:%d\n", *success);
  return now_max_index;
}

static uint32_t eval(int l, int r, bool *success){
  if(l>r){
    *success=false;
    return 0;
  }
  if(l==r){
    *success=true;
    switch(tokens[l].type){
      case NUM_10:
      case NUM_16:{
        return strtoul(tokens[l].str,NULL,0);
      }
      default:{
        /* TODO: fix further for regs..*/
        return 0;
      }
    }
  }
  else if(check_parentheses(l, r, success)){
    // printf("%d\n",*success);
    return eval(l+1,r-1,success);
  }
  else{
    // printf("%d\n",*success);
    int opIndex = find_operator(l,r,success);
    printf("opIndex:%d\n",opIndex);
    uint32_t val1 = 0;
    if(tokens[opIndex].type==TK_NOT){
      val1=eval(l,opIndex-1,success);
      if(*success){
        return !val1;
      }
      else{
        return 0;
      }
    }
    uint32_t val2 = 0;
    val2 = eval(opIndex+1,r,success);
    if(!*success){
      return 0;
    }
    switch(tokens[opIndex].type){
      printf("%d %d\n", val1, val2);
      case '+':{
        return val1+val2;
      }
      case '-':{
        if(val1>val2) return val1-val2;
        else{
          printf("cuz this is a unsigned number,so the result is wrong literally..!\n");
          return val1-val2;
        }
      }
      case '*':{
        return val1*val2;
      }
      case '/':{
        if(val2==0){
          printf("divide 0 error..!\n");
          *success=false;
          return 0;
        }
        return val1/val2;
      }
      case TK_LOGAND:{
        return val1&&val2;
      }
      case TK_LOGOR:{
        return val1||val2;
      }
      case TK_EQ:{
        return val1==val2;
      }
      case TK_NEQ:{
        return val1!=val2;
      }
      case TK_AND:{
        return val1&val2;
      }
      case TK_OR:{
        return val1|val2;
      }
      default:{
        assert(0);
      }
    }
  }
}

uint32_t expr(char *e, bool *success) {
  if (!make_token(e)) {
    *success = false;
    return 0;
  }

  /* TODO: Insert codes to evaluate the expression. */
  printf("before calc expr is_success:%d\n", *success);
  printf("l:%d r:%d\n", 0, nr_token-1);
  return eval(0,nr_token-1,success);
}
