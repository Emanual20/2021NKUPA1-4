#include "cpu/exec.h"

make_EHelper(mov);

make_EHelper(operand_size);

make_EHelper(inv);
make_EHelper(nemu_trap);

// arith.c
make_EHelper(add);
make_EHelper(sub);
make_EHelper(cmp);
make_EHelper(dec);

// logic.c
make_EHelper(test);
make_EHelper(xor);
make_EHelper(and);
make_EHelper(sar);
make_EHelper(shl);
make_EHelper(setcc);

// control.c
make_EHelper(call);
make_EHelper(jmp);
make_EHelper(jcc);
make_EHelper(ret);

// data-mov.c
make_EHelper(push);
make_EHelper(pop);
make_EHelper(movsx);
make_EHelper(movzx);
make_EHelper(lea);