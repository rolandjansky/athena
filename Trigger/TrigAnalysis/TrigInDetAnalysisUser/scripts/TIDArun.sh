#!/usr/bin/env bash

pwd

echo "args: $*"

BASEDIR=/afs/cern.ch/user/s/sutt/public

for FILE in $BASEDIR/TIDAWeb/TIDAV/{style.css,index.php}; do
  if [ ! -e $FILE ]; then 
      cp  $FILE .
  fi
done


BASEREFDIR=$BASEDIR/TrigInDetValidationReference

if [ -e TrkNtuple-0000.root ]; then 

    RELEASE=$(TIDAreader.exe -ro data*.root | grep release | awk '{print $2}' | head -1)

    REFDIR=$BASEREFDIR/share

    if [ -d $BASEREFDIR/$RELEASE ]; then 
        REFDIR=$BASEREFDIR/$RELEASE
    fi

    echo "fetching reference files from $REFDIR"
     
    REFFILE=$REFDIR/$2

    if [ -e  $REFFILE ]; then 
      echo "copying $REFFILE" 
      cp $REFFILE .

      ls -l

      echo
      echo "running comparitor " `date`
      echo

      TIDAcomparitor.exe $*

    else
      echo "no reference file $REFFILE" 
    fi


fi
