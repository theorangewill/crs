#!/bin/bash

FILE=instances/simple-synthetic.su
FILE=instances/simple-windowed.su
A_INI=-0.0007
A_FIN=0.0007
A_INC=5
B_INI=-0.0000001
B_FIN=0.0000001
B_INC=5
C_INI=0.000000198
C_FIN=0.00000177
C_INC=5
MD=150
WIND=0.002 
APH=600

make clean

make
if [ "$1" == "valgrind" ]; then
	valgrind ./bin/crs $FILE $A_INI $A_FIN $A_INC $B_INI $B_FIN $B_INC $C_INI $C_FIN $C_INC $MD $WIND $APH
else   
	./bin/crs $FILE $A_INI $A_FIN $A_INC $B_INI $B_FIN $B_INC $C_INI $C_FIN $C_INC $MD $WIND $APH
fi

