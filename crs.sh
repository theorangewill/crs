#!/bin/bash

#FILE=instances/simple-synthetic.su
FILE=instances/simple-windowed.su
#FILE=instances/simple-windowed2.su
A_ANG=60
A_V0=2000
A_INT=50
B_PCTG=1.0
B_INT=100
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
	valgrind ./bin/crs $FILE $A_ANG $A_V0 $A_INT $B_PCTG $B_INT $V_INI $V_FIN $V_INT $MD $WIND $APH $AZIMUTH  ../referencia/scripts/crs.a.su ../referencia/scripts/crs.b.su ../referencia/scripts/crs.vel.su ../referencia/scripts/crs.coher.su
else
	./bin/crs $FILE $A_ANG $A_V0 $A_INT $B_PCTG $B_INT $V_INI $V_FIN $V_INT $MD $WIND $APH $AZIMUTH ../referencia/scripts/crs.a.su ../referencia/scripts/crs.b.su ../referencia/scripts/crs.vel.su ../referencia/scripts/crs.coher.su
fi
