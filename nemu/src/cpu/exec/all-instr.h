#include "cpu/exec.h"

make_EHelper(mov);

make_EHelper(operand_size);

make_EHelper(inv);
make_EHelper(nemu_trap);

// arith.c
make_EHelper(sub);
make_EHelper(xor);

make_EHelper(call);
make_EHelper(jmp);

make_EHelper(push);