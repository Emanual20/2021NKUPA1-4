#include "proc.h"
#include "memory.h"

static void *pf = NULL;

void* new_page(void) {
  assert(pf < (void *)_heap.end);
  void *p = pf;
  pf += PGSIZE;
  return p;
}

void free_page(void *p) {
  panic("not implement yet");
}

/* The brk() system call handler. */
int mm_brk(uint32_t new_brk) {
  if(current->cur_brk == 0){
    current->cur_brk = current -> max_brk = new_brk;
  }
  else{
    if(new_brk > current -> max_brk){
      // TODO: map memory region [current->max_brk, new_brk)
      // into address space current->as
      uintptr_t vaddr = (((uint32_t)(current->max_brk) + 0xfff) & 0xfffff000);
      while(vaddr < new_brk){
        _map(&current->as, (void*)vaddr, (void*)new_page());
        vaddr += PGSIZE;
      }
      
      current->max_brk = new_brk;
    }

    current->max_brk = new_brk;
  }
  return 0;
}

void init_mm() {
  pf = (void *)PGROUNDUP((uintptr_t)_heap.start);
  Log("free physical pages starting from %p", pf);

  _pte_init(new_page, free_page);
  Log("finish pte_init..");
}
