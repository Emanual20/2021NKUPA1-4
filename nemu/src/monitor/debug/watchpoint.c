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

void free_wp(WP *wp){
	WP* temp_ptr = head;
	WP* temp_back_ptr = NULL;
	WP* aim_ptr = NULL;
	while(temp_ptr){
		if(temp_ptr->NO == wp->NO){
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