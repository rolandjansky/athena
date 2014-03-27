#!/bin/sh

## Check arguments
if [ $# -lt 1 ] ; then
   echo "Usage: ort_runAll.sh <xmlfile>"
   exit 1 
fi

## Intercept dryrun option if given
DRYRUN=0 
if [ "$1" = "-d" ] ; then
  DRYRUN=1
  shift
fi

## set configuration parameters
XMLFILE=$1
if [ ! -f $XMLFILE ]; then 
  echo "Input xmlfile ${XMLFILE} not found. Exit."
  exit 1
fi

RUNALLCMDS="runallcmds.sh"
if [ -f ${RUNALLCMDS} ]; then
  rm -f ${RUNALLCMDS}
fi

echo "#!/bin/sh" > ${RUNALLCMDS}
tct_readxmlcmds.py $XMLFILE >> ${RUNALLCMDS}
chmod +x ${RUNALLCMDS}

## run all commands
RUNCMD=${RUNALLCMDS}
echo
echo ">> Now running command:"
echo ">> ===================="
echo ${RUNCMD}
echo 
if [ $DRYRUN -ne 1 ]; then
  mkdir -p run; cd run
  ../$RUNCMD
  cd ..
fi
echo 
echo ">> Finished command:"
echo ">> ===================="
echo ${RUNCMD}
echo 

