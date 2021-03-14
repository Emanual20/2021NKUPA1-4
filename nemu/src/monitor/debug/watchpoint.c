#include "monitor/watchpoint.h"
#include "monitor/expr.h"

#define NR_WP 32

static WP wp_pool[NR_WP];
static WP *head, *free_;

void init_wp_pool() {
  int i;
  for (i = 0; i < NR_WP; i ++) {
    wp_pool[i].NO = i;
    wp_pool[i].next = &wp_pool[i + 1];
  }
  wp_pool[NR_WP - 1].next = NULL;

  head = NULL;
  free_ = wp_pool;
}

/* TODO: Implement the functionality of watchpoint */

WP* new_wp(){
	if(free_==NULL){
		printf("There is no free watchpoint..!\n");
		assert(0);
	}
	WP* ret = free_;
	free_ = free_ -> next;
	ret -> next = head;
	head = ret;
	return ret;
}

void free_wp(int n){
	WP* temp_ptr = head;
	WP* temp_back_ptr = NULL;
	WP* aim_ptr = NULL;
	while(temp_ptr){
		if(temp_ptr->NO == n){
			aim_ptr = temp_ptr;
			break;
		}

		temp_back_ptr = temp_ptr;
		temp_ptr = temp_ptr -> next;
	}
	if(aim_ptr == NULL){
		printf("There is no corressponding watchpoint..!\n");
		assert(0);
	}

	if(temp_back_ptr){
		temp_back_ptr -> next = temp_ptr -> next;
	}
	else{
		free_ = free_ -> next;
	}

	aim_ptr -> next = free_;
	free_ =  aim_ptr;
}

void free_wpn(WP *wp){
	return free_wp(wp->NO);
}

void print_watchpointsinfo(){
	WP* temp_ptr = head;
	if(!temp_ptr){
		printf("no watchpoint is in use..!\n");
		return;
	}
	while(temp_ptr){
		printf("%d\t,%s\t,%d\t\n", temp_ptr->NO, temp_ptr->expr, temp_ptr->value);
		temp_ptr = temp_ptr -> next;
	}
}