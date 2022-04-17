#!/bin/bash
if [ $# -ne 1 ]; then
    #um="0.49"
    #u1="1"
    #pot="0"
    #ex="21"
    #echo "Usage: $0 alpha1 alpha2"
    #exit -1
    c=""
else
    c="--cclipping"
    #um=$1
    #u1=$2
    #pot=$3
    #ex=$2
fi
um="0.49"
u1="1"
pot="0"
ex="21"
# Put here the program (maybe with path)
#GETF0="get_pitch --umaxnorm=$um --u1norm=$u1 --upot=$pot --uext=$ex"
#GETF0="get_pitch --umaxnorm=$um --upot=$pot --uext=$ex"
#GETF0="get_pitch --cclipping --umaxnorm=$um --uext=$ex"
GETF0="get_pitch $c --umaxnorm=$um --u1norm=$u1 --upot=$pot --uext=$ex"

for fwav in pitch_db/train/*.wav; do
    ff0=${fwav/.wav/.f0}
    echo "$GETF0 $fwav $ff0 ----"
    $GETF0 $fwav $ff0 > /dev/null || (echo "Error in $GETF0 $fwav $ff0"; exit 1)
done
#pitch_evaluate pitch_db/train/*f0ref | fgrep ===
pitch_evaluate pitch_db/train/*f0ref

exit 0
