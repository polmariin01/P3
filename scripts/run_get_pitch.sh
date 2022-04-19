#!/bin/bash
#if [ $# -ne 1 ]; then
#    um="0.45"
#    pot="-20"
#    cc="0.009"
    #echo "Usage: $0 alpha1 alpha2"
    #exit -1
#else
#    cc=$1
#    um=$1
#    pot=$2
#    u1=$3
#    pot=$4
#fi
cc="0.0265"
um="0.44"
u1="1"
ds="4"
pot="0"
ex="0"
# Put here the program (maybe with path)
GETF0="get_pitch --cclipping=$cc --umaxnorm=$um --u1norm=$u1 --upot=$pot --uext=$ex --downsampl=$ds --lpfwindow=1"

for fwav in pitch_db/train/*.wav; do
    ff0=${fwav/.wav/.f0}
    echo "$GETF0 $fwav $ff0 ----"
    $GETF0 $fwav $ff0 > /dev/null || (echo "Error in $GETF0 $fwav $ff0"; exit 1)
done
pitch_evaluate pitch_db/train/*f0ref

exit 0
