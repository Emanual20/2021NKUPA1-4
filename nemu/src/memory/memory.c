#include "nemu.h"

#define PMEM_SIZE (128 * 1024 * 1024)

#define pmem_rw(addr, type) *(type *)({\
    Assert(addr < PMEM_SIZE, "physical address(0x%08x) is out of bound", addr); \
    guest_to_host(addr); \
    })

uint8_t pmem[PMEM_SIZE];

/* Memory accessing interfaces */

uint32_t paddr_read(paddr_t addr, int len) {
  if(addr%0x10000==0)
    printf("paddr:0x%x\n", addr);
  int map_NO = is_mmio(addr);
  if(map_NO == -1)
    return pmem_rw(addr, uint32_t) & (~0u >> ((4 - len) << 3));
  else
    return mmio_read(addr, len, map_NO);
}

void paddr_write(paddr_t addr, int len, uint32_t data) {
  int map_NO = is_mmio(addr);
  if(map_NO == -1)
    memcpy(guest_to_host(addr), &data, len);
  else
    mmio_write(addr, len, data, map_NO);
}

#define PDX(va) (((uint32_t)(va) >> 22) & 0x03ff)
#define PTX(va) (((uint32_t)(va) >> 12) & 0x03ff)
#define OFFSET(va) (((uint32_t)(va) >> 0) & 0x03ff)
#define REMOVE_OFFSET(pte) ((uint32_t)(pte) & 0xfffffc00) 

bool IfDataCrossBoundary(vaddr_t addr, int len){
  return ((addr + len - 1) & ~PAGE_MASK) != (addr & ~ PAGE_MASK);
}

paddr_t page_translate(vaddr_t vaddr, bool flag){
  PDE pde;
  PDE *pgdir;
  PTE pte;
  PTE *ptdir;
  // printf("%x", cpu.cr0);
  if(cpu.cr0.protect_enable && cpu.cr0.paging){
    pgdir = (PDE*)(REMOVE_OFFSET(cpu.cr3.val));
    pde.val = paddr_read((paddr_t)&pgdir[PDX(vaddr)], 4);
    assert(pde.present);
    pde.accessed = true;

    ptdir = (PTE*)(REMOVE_OFFSET(pde.val));
    pte.val = paddr_read((paddr_t)&ptdir[PTX(vaddr)], 4);
    assert(pte.present);
    pte.accessed = true;
    pte.dirty = flag ? 1 : pte.dirty;

    return REMOVE_OFFSET(pte.val) | OFFSET(vaddr);
  }
  else{
    return vaddr;
  }
}

uint32_t vaddr_read(vaddr_t vaddr, int len) {
  if(IfDataCrossBoundary(vaddr, len)){
    assert(0);
  }
  else{
    paddr_t paddr = page_translate(vaddr, false);
    return paddr_read(paddr, len);
  }
}

void vaddr_write(vaddr_t vaddr, int len, uint32_t data) {
  if(IfDataCrossBoundary(vaddr, len)){
    assert(0);
  }
  else{
    paddr_t paddr = page_translate(vaddr, true);
    paddr_write(paddr, len, data);
  }
}
