#!/bin/bash

FILE=instances/simple-synthetic.su
A_INI=0.000000198
A_FIN=0.00000177
A_INC=101
B_INI=0.000000198
B_FIN=0.00000177
B_INC=101
C_INI=0.000000198
C_FIN=0.00000177
C_INC=101
MD=150
WIND=0.002 
APH=600

make clean

make
if [ "$2" == "valgrind" ]; then
	valgrind ./bin/cmp $FILE $A_INI $A_FIN $A_INC $B_INI $B_FIN $B_INC $C_INI $C_FIN $C_INC $MD $WIND $APH
else   
	./bin/cmp $FILE $A_INI $A_FIN $A_INC $B_INI $B_FIN $B_INC $C_INI $C_FIN $C_INC $MD $WIND $APH
fi

