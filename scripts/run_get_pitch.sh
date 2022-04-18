#!/bin/bash
if [ $# -ne 6 ]; then
    c="0.009"
    um="0.4"
    u1="0.9"
    pot="-20"
    ex="0"
    ds="1"
    #echo "Usage: $0 alpha1 alpha2"
    #exit -1
else
    c=$1
    um=$2
    u1=$3
    pot=$4
    ex=$5
    ds=$6
fi
# Put here the program (maybe with path)
#GETF0="get_pitch --umaxnorm=$um --u1norm=$u1 --upot=$pot --uext=$ex"
#GETF0="get_pitch --umaxnorm=$um --upot=$pot --uext=$ex"
#GETF0="get_pitch --cclipping --umaxnorm=$um --uext=$ex"
GETF0="get_pitch --cclipping=0.001 --umaxnorm=$um --u1norm=$u1 --upot=$pot --uext=$ex --downsampl=$ds --lpfwindow=1"

for fwav in pitch_db/train/*.wav; do
    ff0=${fwav/.wav/.f0}
    echo "$GETF0 $fwav $ff0 ----"
    $GETF0 $fwav $ff0 > /dev/null || (echo "Error in $GETF0 $fwav $ff0"; exit 1)
done
#pitch_evaluate pitch_db/train/*f0ref | fgrep ===
pitch_evaluate pitch_db/train/*f0ref

exit 0
