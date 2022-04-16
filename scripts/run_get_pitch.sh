#!/bin/bash
if [ $# -ne 3 ]; then
    um="0.44"
    u1="1"
    pot="0"
    #echo "Usage: $0 alpha1 alpha2"
    #exit -1
else
    um=$1
    u1=$2
    pot=$3
fi
# Put here the program (maybe with path)
GETF0="get_pitch --umaxnorm=$um --u1norm=$u1 --upot=$pot"

for fwav in pitch_db/train/*.wav; do
    ff0=${fwav/.wav/.f0}
    #echo "$GETF0 $fwav $ff0 ----"
    $GETF0 $fwav $ff0 > /dev/null || (echo "Error in $GETF0 $fwav $ff0"; exit 1)
done
#pitch_evaluate pitch_db/train/*f0ref | fgrep ===
pitch_evaluate pitch_db/train/*f0ref

exit 0
