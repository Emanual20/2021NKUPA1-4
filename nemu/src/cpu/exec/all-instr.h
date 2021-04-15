#include "cpu/exec.h"

make_EHelper(mov);

make_EHelper(operand_size);

make_EHelper(inv);
make_EHelper(nemu_trap);

// arith.c
make_EHelper(sub);

// logic.c
make_EHelper(xor);

// control.c
make_EHelper(call);
make_EHelper(jmp);
make_EHelper(ret);

make_EHelper(push);