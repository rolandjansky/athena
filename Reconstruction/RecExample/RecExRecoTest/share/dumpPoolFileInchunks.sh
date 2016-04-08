#!/bin/bash

TD=/tmp/$USER/dumprootfile.tmp.$EUID.$$
if [ ! -z "$TMPDIR" ]
then
    TD=$TMPDIR/dumprootfile.tmp.$EUID.$$
fi

mkdir -p $TD
echo "using as tempdir $TD"

Nev=`acmd.py chk-file $1 2>&1 | grep "Nbr Events:" | gawk '{print $3 }'`

Ident=`basename $1`

echo "file has $Nev events"
RLAST=49
RFIRST=0
RANGE="$RFIRST:$RLAST"

while [ ! -z $RANGE ]
do
    echo dumping range of events of $RANGE
    acmd.py dump-root --entries $RANGE $1 | gzip - > dumprootfile.$Ident.range-$RFIRST-$RLAST.log.gz
    
    RANGE=""
    RFIRST=$(( $RFIRST + 50 ))
    RLAST=$(( $RLAST + 50 ))
    if [ $RFIRST -lt $Nev ]
    then
        RANGE="$RFIRST:$RLAST"
    fi
done

rm -fr $TD
