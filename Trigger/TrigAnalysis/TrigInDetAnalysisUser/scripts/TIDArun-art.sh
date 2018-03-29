#!/usr/bin/env bash

pwd

echo "args: $*"

# BASEDIR=/eos/atlas/atlascerngroupdisk/proj-sit/trigindet
BASEDIR=/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art

echo 
echo "copy reference file and run comparitor"
echo
 
BASEREFDIR=$BASEDIR/TrigInDetValidationReference

if [ -e TrkNtuple-0000.root ]; then 

    RELEASE=$(TIDAreader.exe -ro TrkNtuple-0000.root | grep release | awk '{print $2}' | head -1)

    REFDIR=$BASEREFDIR/share

    echo "RELEASE: $RELEASE"

    if [ -d $BASEREFDIR/share/$RELEASE ]; then 
        REFDIR=$BASEREFDIR/share/$RELEASE
    fi

    echo "fetching reference files from $REFDIR"
        
    EXPERT=$(echo $1 $2 | grep expert) 

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

    NOREF=

    if [ ! -e $REFFILE ]; then 

      echo "no reference file $REFFILE - will process without"

      NOREF=--noref

    else

      echo "copying $REFFILE" 

      LOCALFILE=$(basename $REFFILE)

      # don't bother to copy it if it already exists in this directory
      [ -e $LOCALFILE ] || cp $REFFILE .
  
    fi

    ls -l

    for arg in $*; do 
       if $(echo "$arg" | grep -q "TIDA.*.dat"); then get_files -data "$arg"; fi 
       if $(echo "$arg" | grep -q "Test.*.dat"); then get_files -data "$arg"; fi 
    done

    if [ "x$EXPERT" == "x" ]; then 
        get_files -data TIDAhistos-vtx.dat
        echo -e "\nrunning comparitor " $(date) "\n"
        TIDAcomparitor.exe $* $NOREF
    else
        echo -e "\nrunning cpucost " $(date) "\n"
        TIDAcpucost.exe $* $NOREF
    fi

    echo "finished postprocessing"

    ls -l

fi
