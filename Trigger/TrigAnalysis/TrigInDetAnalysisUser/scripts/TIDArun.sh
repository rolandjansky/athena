#!/usr/bin/env bash

pwd

echo "args: $*"

BASEDIR=/afs/cern.ch/user/s/sutt/public


# copy basic web page information 

for FILE in style.css index.php; do
  echo "checking file $FILE"
  if [ ! -e $FILE ]; then 
    cp  $BASEDIR/TIDAWeb/TIDAV/$FILE .
  fi
done

echo 
echo "copy reference file and run comparitor"
echo
 
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

      LOCALFILE=$(basename $REFFILE)

      # don;t bother to copy it if it already exists in this directory
      if [ ! -e $LOCALFILE ]; then 
         cp $REFFILE .
      fi

      ls -l

      echo
      echo "running comparitor " `date`
      echo

      TIDAcomparitor.exe $*

    else
      echo "no reference file $REFFILE" 
    fi


fi
