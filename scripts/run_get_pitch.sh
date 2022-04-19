#!/bin/bash
if [ $# -ne 2 ]; then
    um="0.45"
    u1="0.95"
    #echo "Usage: $0 alpha1 alpha2"
    #exit -1
else
    um=$1
    u1=$2
#    u1=$3
#    pot=$4
fi
#u1="0.95"
ds="5"
pot="-20"
ex="0"
# Put here the program (maybe with path)
#GETF0="get_pitch --umaxnorm=$um --u1norm=$u1 --upot=$pot --uext=$ex"
#GETF0="get_pitch --umaxnorm=$um --upot=$pot --uext=$ex"
#GETF0="get_pitch --cclipping --umaxnorm=$um --uext=$ex"
GETF0="get_pitch --cclipping=0.009 --umaxnorm=$um --u1norm=$u1 --upot=$pot --uext=$ex --downsampl=$ds --lpfwindow=1"

for fwav in pitch_db/train/*.wav; do
    ff0=${fwav/.wav/.f0}
    echo "$GETF0 $fwav $ff0 ----"
    $GETF0 $fwav $ff0 > /dev/null || (echo "Error in $GETF0 $fwav $ff0"; exit 1)
done
#pitch_evaluate pitch_db/train/*f0ref | fgrep ===
pitch_evaluate pitch_db/train/*f0ref

exit 0
