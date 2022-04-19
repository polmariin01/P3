#!/bin/bash
if [ $# -ne 1 ]; then

    f="1"
    #echo "Usage: $0 alpha1 alpha2"
    #exit -1
else
#    ex=$1
#    um=$2
#    u1=$3
#    pot=$4
    f=$1
fi
ds="4"
um="0.45"
u1="0.95"
pot="-20"
ex="0"
# Put here the program (maybe with path)
#GETF0="get_pitch --umaxnorm=$um --u1norm=$u1 --upot=$pot --uext=$ex"
#GETF0="get_pitch --umaxnorm=$um --upot=$pot --uext=$ex"
#GETF0="get_pitch --cclipping --umaxnorm=$um --uext=$ex"
GETF0="get_pitch --cclipping=0.009 --umaxnorm=$um --u1norm=$u1 --upot=$pot --uext=$ex --downsampl=$ds --lpfwindow=$f"

for fwav in pitch_db/train/*.wav; do
    ff0=${fwav/.wav/.f0}
    echo "$GETF0 $fwav $ff0 ----"
    $GETF0 $fwav $ff0 > /dev/null || (echo "Error in $GETF0 $fwav $ff0"; exit 1)
done
#pitch_evaluate pitch_db/train/*f0ref | fgrep ===
pitch_evaluate pitch_db/train/*f0ref

exit 0
