#include "cpu/exec.h"

make_EHelper(mov) {
  operand_write(id_dest, &id_src->val);
  print_asm_template2(mov);
}

make_EHelper(push) {
  rtl_sext(&t0,&id_dest->val,id_dest->width);
  rtl_push(&t0);
  print_asm_template1(push);
}

make_EHelper(pop) {
  rtl_pop(&t0);
  operand_write(id_dest, &t0);

  print_asm_template1(pop);
}

make_EHelper(pusha) {
  TODO();

  print_asm("pusha");
}

make_EHelper(popa) {
  TODO();

  print_asm("popa");
}

make_EHelper(leave) {
  rtl_sr(4, 4, &cpu.ebp);
  rtl_pop(&cpu.ebp);

  print_asm("leave");
}

make_EHelper(cltd) {
  if (decoding.is_operand_size_16) {
    // rtl_sext(&t0, &cpu.eax, 2);
    // rtl_shri(&cpu.edx, &t0, 16);
    rtl_msb(&t0, &cpu.eax, 2);
    if(t0 == 1) cpu.edx = cpu.edx | 0xffff;
    else cpu.edx = 0;
  }
  else {
    // rtl_sari(&cpu.edx, &cpu.eax, 31);
    rtl_msb(&t0, &cpu.eax, 4);
    if(t0 == 1) cpu.edx = cpu.edx | 0xffffffff;
    else cpu.edx = 0;
  }

  print_asm(decoding.is_operand_size_16 ? "cwtl" : "cltd");
}

make_EHelper(cwtl) {
  if (decoding.is_operand_size_16) {
    rtl_shli(&cpu.eax, &cpu.eax, 24);
    rtl_sari(&cpu.eax, &cpu.eax, 8);
    rtl_shri(&cpu.eax, &cpu.eax, 16);
  }
  else {
    rtl_sext(&cpu.eax, &cpu.eax, 2);
  }

  print_asm(decoding.is_operand_size_16 ? "cbtw" : "cwtl");
}

make_EHelper(movsx) {
  id_dest->width = decoding.is_operand_size_16 ? 2 : 4;
  rtl_sext(&t2, &id_src->val, id_src->width);
  operand_write(id_dest, &t2);
  print_asm_template2(movsx);
}

make_EHelper(movsxy) {
  id_dest->type = OP_TYPE_MEM;
  id_dest->addr = cpu.gpr[R_EDI]._32;
  id_dest->width = decoding.is_operand_size_16 ? 2 : 4;
  t0 = cpu.gpr[R_ESI]._32;
  rtl_lm(&t1, &t0, id_dest->width);
  operand_write(id_dest, &t1);
  cpu.gpr[R_ESI]._32 += (cpu.DF ? -id_dest->width : id_dest->width);
  cpu.gpr[R_EDI]._32 += (cpu.DF ? -id_dest->width : id_dest->width);

#ifdef DEBUG
  sprintf(id_src->str, "%s%#x", ((int)t0 < 0 ? "-" : ""), ((int)t0 < 0 ? (int)-t0 : (int)t0));
  sprintf(id_dest->str, "%%es:(%%edi)");
#endif

  print_asm_template2(movs);
}

make_EHelper(movzx) {
  id_dest->width = decoding.is_operand_size_16 ? 2 : 4;
  operand_write(id_dest, &id_src->val);
  print_asm_template2(movzx);
}

make_EHelper(lea) {
  rtl_li(&t2, id_src->addr);
  operand_write(id_dest, &t2);
  print_asm_template2(lea);
}
