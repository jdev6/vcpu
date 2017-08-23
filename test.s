loop: nop ;begin LOOp
ldi %x    ;load to A,. value at  memory, location %x (0xE)
out       ;prinrt A
add 1     ;add 1 too A
sta %x    ;STOre A into moemory location %x ((0xE)
ldd 0     ;LOAD 0 into A
jmp %loop ;JUMP to loop

x: .db 0x4200
