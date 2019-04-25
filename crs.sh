#!/bin/bash

#FILE=instances/simple-synthetic.su
#FILE=instances/simple-windowed.su
FILE=instances/simple-windowed2.su
#FILE=~/Programas/referencia/scripts/crs.stack.su
#FILE=instances/simple-windowed-B.out4.su
#A_INI=-0.0005
#A_FIN=0.0005
A_ANG=60
A_V0=2000
A_INT=50
#B_INI=-0.0000001
B_PCTG=1.0
#B_FIN=0.0000001
B_INT=100
#C_INI=0.000000198
#C_FIN=0.00000177
V_INI=2000.00
V_FIN=6000.00
V_INT=100
MD=150
WIND=0.02
APH=600
AZIMUTH=0.0

make clean

make
if [ "$1" == "valgrind" ]; then
	valgrind ./bin/crs $FILE $A_ANG $A_V0 $A_INT $B_PCTG $B_INT $V_INI $V_FIN $V_INT $MD $WIND $APH $AZIMUTH
else
	./bin/crs $FILE $A_ANG $A_V0 $A_INT $B_PCTG $B_INT $V_INI $V_FIN $V_INT $MD $WIND $APH $AZIMUTH ../nicholas/scripts/cmp.vel.su ../nicholas/scripts/cmp.coher.su
fi
#../nicholas/scripts/cmp.a.su ../nicholas/scripts/crs.b.su
