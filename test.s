;TEST: hjutst prints even numbers,

jmp %start

;varables
x: .db 0 ;counter

;entry point
start: nop
;INCREMENT x
ldi %x
ldbd 1
add
sta %x

ldi %x ;x -> a
ldbd 1 
and ;A = x & 1

sub ;A = A-1
jz %start ;JUMPs if A&1 is 1 (means odd)
ldi %x
out

jmp %start
