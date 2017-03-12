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

    RELEASE=$(TIDAreader.exe -ro TrkNtuple-0000.root | grep release | awk '{print $2}' | head -1)

    REFDIR=$BASEREFDIR/share

    echo "RELEASE: $RELEASE"

    if [ -d $BASEREFDIR/$RELEASE ]; then 
        REFDIR=$BASEREFDIR/$RELEASE
    fi

    echo "fetching reference files from $REFDIR"
        
    get_files TIDAhistos-vtx.dat

    EXPERT=$(grep expert $2) 

    # if expert timing histos
    if [ "x$EXPERT" != "x" ]; then 
      if [ "x$RTTJOBNAME" != "x" ]; then 
        EXPERTREF="expert-monitoring-$(echo $RTTJOBNAME | sed 's|TrigInDetValidation_||g')-ref.root"
        REFFILE=$REFDIR/expert/$EXPERTREF
      else
        REFFILE=$REFDIR/expert/$2
      fi      
    else
      REFFILE=$REFDIR/$2
    fi

    if [ -e  $REFFILE ]; then 
      echo "copying $REFFILE" 

      LOCALFILE=$(basename $REFFILE)

      # don;t bother to copy it if it already exists in this directory
      if [ ! -e $LOCALFILE ]; then 
         cp $REFFILE .
      fi

      ls -l

      if [ "x$EXPERT" == "x" ]; then 

        echo
        echo "running comparitor " `date`
        echo

        for arg in $*; do 
           if $(echo "$arg" | grep -q "TIDA.*.dat"); then get_files -data "$arg"; fi 
           if $(echo "$arg" | grep -q "Test.*.dat"); then get_files -data "$arg"; fi 
        done

        TIDAcomparitor.exe $*

      else

        echo
        echo "running cpucost " `date`
        echo

        for arg in $*; do 
           if $(echo "$arg" | grep -q "TIDA.*.dat"); then get_files -data "$arg"; fi 
           if $(echo "$arg" | grep -q "Test.*.dat"); then get_files -data "$arg"; fi 
        done

        TIDAcpucost.exe $*

      fi

    else
      echo "no reference file $REFFILE" 
    fi

    echo "finished postprocessing"
    ls -l


fi
