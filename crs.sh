#!/bin/bash

#FILE=instances/simple-synthetic.su
FILE=instances/simple-windowed.su
#FILE=~/Programas/referencia/scripts/crs.stack.su
#FILE=instances/simple-windowed-B.out4.su
#A_INI=-0.0005
#A_FIN=0.0005
A_ANG=60
A_V0=2000
A_INT=7
#B_INI=-0.0000001
B_PCTG=0.1
#B_FIN=0.0000001
B_INT=5
#C_INI=0.000000198
#C_FIN=0.00000177
V_FIN=4000.00
V_INI=2000.00
V_INT=100
MD=150
WIND=0.002
APH=600

make clean

make
if [ "$1" == "valgrind" ]; then
	valgrind ./bin/crs $FILE $A_ANG $A_V0 $A_INT $B_PCTG $B_INT $V_INI $V_FIN $V_INT $MD $WIND $APH
else
	./bin/crs $FILE $A_ANG $A_V0 $A_INT $B_PCTG $B_INT $V_INI $V_FIN $V_INT $MD $WIND $APH
fi
