;test for eq uality

ldd 10 ;load 10 INTO a , and b
ldbd 10

sub ;subtracts
jz %equals ; JUMP IF zero flag set (a-b = 0)
halt

equals: out ;wil print vallue of A (in, this case ITs 0A becuase: addres of CONDITional jump gets in stored in A)
halt
