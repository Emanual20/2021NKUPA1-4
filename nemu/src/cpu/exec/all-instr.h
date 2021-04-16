#include "cpu/exec.h"

make_EHelper(mov);

make_EHelper(operand_size);

make_EHelper(inv);
make_EHelper(nemu_trap);

// arith.c
make_EHelper(sub);

// logic.c
make_EHelper(xor);
make_EHelper(and);
make_EHelper(sar);

// control.c
make_EHelper(call);
make_EHelper(jmp);
make_EHelper(ret);

// data-mov.c
make_EHelper(push);
make_EHelper(lea);