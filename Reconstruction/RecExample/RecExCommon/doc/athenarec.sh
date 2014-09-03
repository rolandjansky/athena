#!/usr/local/bin/zsh
####################################################################
# script to run athena reconstruction in batch                     
# David Rousseau, Dec 2002                                         
# distilled from examples by Jorgen Beck Hansen,Elzbieta Richter-Was, Samira Hassani.                           
#     Modifications : 
#       DR May 2006 : Adapt to project build
#       DR January 2005 : do not build anymore RecExCommon package
#       DR April 2005 : cannot read zebra data anymore
# look for MUSTBEMODIFIED for mandatory modification
# This run the standard RecExCommon package, taken either directly 
#   from a release or from a user area (if USERAREA is defined).
# The code which is run is by default taken directly from the release 
#   but modified packages can be used if properly recompiled in 
#   a USERAREA
# New packages can also be used if USERAREA is defined.
# The data processed are either DC1 files taken directly from castor, 
# only for geant3 !something similar to be done for geant4 (volunteer?)
#   or any other data file if soft links are constructed by hand.
#   It is possible to run on several files within the same job.
#
# Be careful that the script is loaded ONLY when the job starts, not 
#   when the job is submitted, so avoid to modify the script 
#   before the job starts.
#
# Note on timing: the time allocation for the queues are normalised
# to a 500Mhz machine (for more powerful machine it is scaled down 
# by 500/<frequency>) (the frequency appear in the output)
# The time indicated at the beginning of the output:
#Resource usage summary:
#
#    CPU time   :    239.54 sec.
# is *NOT* normalized.
#
#The time indicated at the end of the output:
#CERN statistics: This job used 0:06:22 NCU hours (382 NCU seconds)
# *IS* normalised.
#The time indicated in athena output is *NOT* normalised
# 
#                    
################################################################### 
# Arguments : 1 string which will be append to ntuple and log file
#             2 string specifying where to take the data simul/lumi02/lumi10
#             3,4,5 data set number, first and last file to process
####################################################################
# Examples : 
# run RecExCommon out of the box
# bsub -q 8nm -o athenarec.log -J athenarec athenarec.sh test1
# run on DC1 data set 001003 files number 5 to 10  
#bsub -q 8nh -o athenarec.log -J athenarec athenarec.sh prod1 lumi02 001003 5 10 
# run on files specified in script (by default ZEBRA_HEEMM
#bsub -q 8nm -o athenarec.log -J athenarec athenarec.sh prod1 
# one can also use the subm.sh script:
# subm.sh 8nh athenarec prod1 lumi02 001003 1  10
####################################################################
#
# ------------------------------------------------------------ #
#set -x 


unset USERAREA
unset VALGRIND
unset BUGFIX
unset VAL
#this line should be modified to point to the user area
export HOMEDIR=${HOME}

#if USERAREA is set,  packages checked out and possibly modified 
#   by the user will be used directly from batch (without recompilation). 
# (They should have 
# been compiled against the release being used and should not be 
# modified when the job is running)
# export USERAREA=$HOMEDIR/maxi/test3

#output directory for ntuples, this can be castor
#export OUTPUTHIS=/castor/cern.ch/user/j/joe
#MUSTBEMODIFIED
export OUTPUTHIS=$HOMEDIR/scratch0/output/his
#output directory for log file, this can be castor
#MUSTBEMODIFIED
export OUTPUTLOG=$HOMEDIR/scratch0/output/log
# specify  "project" or "replica" in castor (only for DC1 data)
#export PROREP=project
#export PROREP=replica
#specify release to be used

#CANBEMODIFIED
# specify release 11.0.42 or 11.5.0 etc.... (automatically adapt to project build)
# can also be rel_0 rel_1 etc for nightly build
export RELEASE=13.0.30
# uncomment if use production cache or pcache nightly
export PCACHE=13.0.30.1
# uncomment for X.0.Y bugfix nightlies
#export BUGFIX=bugfix
# uncomment for X.0.Y bugfix nightlies
#export VAL=val



#maximum number of event processed in the job (default is 10)
export EVTMAX=-1
#export EVTMAX=5


# on geant4 a non-tunable, mechanism is used to stop the job: 
# if time left is less
# than 3 times the average time, the job is stopped

# 1 if data file defined in this script 
export SPECIALDATA=0

#if valgrind to be used (not recently tested) 
#  valgrind is a tool to find memory leaks and other run time error.
#  it is rather slow so if switched on, it is suggested to run on a few events
#  valgrind cannot run on castor, so it needs to be run on a file on disk
#  so when valgrind is switched on : subm.sh 8nh athenarec test1
#export VALGRIND=valgrind ; export EVTMAX=2 ; export SPECIALDATA=0
#
#release directory
export DISTREL=/afs/cern.ch/atlas/software/dist
#cmt version 

export CMTVERSION=v1r20p20070720


#########################################################################
#  end of main modifiable area                                          #
# look for "CANBEMODIFIED" for further possible modification             #  
#########################################################################



if [ $# -eq 0 ] ; then
  echo Input data files are specified directly in athena.sh
  export JOBNAME=0
elif [ $# -eq 1 ] ; then
  echo Input data files are specified directly in athena.sh
  export JOBNAME=$1
elif [ $# -eq 5 ] ; then
  export JOBNAME=$1_$2_$3_$4_$5
  unset DATASETNUM 
  export CASTORLOC=$2
  export DATASETNUM=$3
  export IFIRSTFILE=$4
  export ILASTFILE=$5

#directory where to take the data (obsolete)
   echo CASTORLOC = $CASTORLOC
  if [ $CASTORLOC = "simul" ] ; then 
     echo simul...
     export INPUTDATA=/castor/cern.ch/atlas/$PROREP/dc1/simul/data/$DATASETNUM
  elif [ $CASTORLOC = "lumi02" ] ; then 
     echo lumi02...
     export INPUTDATA=/castor/cern.ch/atlas/$PROREP/dc1/lumi02/$DATASETNUM/data
  elif [ $CASTORLOC = "lumi10" ] ; then 
     echo lumi10...
     export INPUTDATA=/castor/cern.ch/atlas/$PROREP/dc1/lumi10/$DATASETNUM/data
  else
    echo Invalid value of CASTORLOC $CASTORLOC Stop.
#    exit
  fi


  echo use dataset $INPUTDATA file $IFIRSTFILE to $ILASTFILE



else
 echo Invalid number of arguments. Stop.
 exit
fi


# nightly build are project build

PROJECTBUILD=1
RELCUT=`echo $RELEASE | cut -c1-3`

if [[ $RELCUT = "rel" ]] ; then
# nightly build are project build
    NIGHTLY=1
else
    NIGHTLY=0
fi



#

if [ $NIGHTLY -eq 0 ] ; then
    TOPDIR=/afs/cern.ch/atlas/software/builds
else
    TOPDIR=/afs/cern.ch/atlas/software/builds/nightlies/dev/
fi	

FULLREL=$TOPDIR/AtlasOffline/$RELEASE


if [ -d "${FULLREL}"  ] ; then
    echo $FULLREL exists
else
     echo $FULLREL does not exist. Stop.
    exit
fi


#
echo "##working directory is:"
pwd

#power of the processor
grep MHz /var/log/dmesg 
#increase default limit for core dump
#limit coredumpsize 10M
#create clean working directory
if [ -d myjob ] ; then
  echo "##delete already existing myjob directory"
  rm -fR myjob
fi

mkdir myjob
cd myjob

# export WORKAREA=`pwd`

mkdir cmtdir
cd cmtdir


rm -f requirements ; touch requirements

echo "##set up cmt with requirements file" 


    cat <<EOF >|requirements
set   CMTSITE  CERN
set   SITEROOT /afs/cern.ch
set ATLAS_DIST_AREA "${DISTREL}"
# do not append release number to test area
apply_tag oneTest
#
#
apply_tag  setupCMT
# optimised by default
apply_tag  opt
apply_tag 32
apply_tag ${RELEASE} 
use AtlasLogin AtlasLogin-* "${DISTREL}"
EOF

if [ -n "$USERAREA" ] ; then
 echo "## use user area $USERAREA"
 echo 'macro ATLAS_TEST_AREA "${WORKAREA}"' >> thereq
else
  echo "## do not use userarea"
fi

if  [ -n "$BUGFIX" ] ; then
   echo "apply_tag $BUGFIX " >> thereq
fi

# if pcache nightly
if  [[ -n "$PCACHE" && $NIGHTLY -eq 1 ]] ; then
   echo "apply_tag pcache " >> thereq
fi

# if not using productin cache setup is done through requirements
if  [[ -n "$PCACHE" && $NIGHTLY -eq 0 ]] ; then
else
   echo "apply_tag setup " >> thereq
fi

# if pcache nightly
if  [ -n "$PCACHE" && $NIGHTLY -eq  ]] ; then
   echo "apply_tag pcache " >> thereq
fi

if  [ -n "$VAL" ] ; then
   echo "apply_tag $VAL " >> thereq
fi

echo "## main requirements file"
cat requirements



CMTROOT=/afs/cern.ch/sw/contrib/CMT/$CMTVERSION; export CMTROOT
. ${CMTROOT}/mgr/setup.sh
which cmt
cmt config

#intial cmt setup
#valgrind error message are more useful in debug mode
#otherwise use optimised (typically 4 times faster)
if [ ${(c)#VALGRIND} -eq 0 ] ; then
    echo "## optimised mode"
    source setup.sh -tag_add=opt
else
    source setup.sh -tag_add=dbg
fi

# set up to run on production cache
if  [[ -n "$PCACHE" && $NIGHTLY -eq 0 ]] ; then
   echo "## use production cache $PCACHE " 
   export CMTPATH=/afs/cern.ch/atlas/software/releases/$RELEASE/AtlasProduction/$PCACHE 
   if [ -n "$USERAREA" ] ; then
       export CMTPATH=$USERAREA:$CMTPATH
   fi
   source /afs/cern.ch/atlas/software/releases/$RELEASE/AtlasProduction/$PCACHE/AtlasProductionRunTime/cmt/setup.sh
fi


echo "## CMTCONFIG=$CMTCONFIG"
echo "## CMTPATH=$CMTPATH"
cmt show path

#back to top dir
cd ..



#now fill run directory

source RecExCommon_links.sh


## tune the joboptions.file
rm -f myTopOptions_tmp.py ; touch myTopOptions_tmp.py


## set the number of events by changing EvtMax
if [ ${(c)#EVTMAX} -ne 0 ] ; then
  echo "##set EvtMax to $EVTMAX"
  echo "EvtMax=$EVTMAX" >> myTopOptions_tmp.py
fi


## switch on LSFTimeKeeperSvc 
echo "##set doTimeLimit to True"
echo "doTimeLimit=True" >> myTopOptions_tmp.py


#CANBEMODIFIED : change 0 to 1 if take existing joboption
if [ 0 -eq 1 ] ; then
  # take existing topoption  
  cat ${HOMEDIR}/maxi/test2/Reconstruction/RecExample/RecExCommon/RecExCommon-00-02-62/run/nothing.py >> myTopOptions_tmp.py
else
#build joboption in the script
cat >> myTopOptions_tmp.py << EOF
# loop to give list of file number 
filehint=[(1,4),(2,6),(3,4),(4,1),(5,13),(6,1),(7,4),(8,6),(9,12),(10,9)]
PoolRDOInput=[]
for t in filehint:
  string0=str(t[0])
  while (len(string0)<5):
    string0="0"+string0
    string1=str(t[1])
  filename="rfio:/castor/cern.ch/user/r/rtt/RTTdata/calib1_csc11.005200.T1_McAtNlo_Jimmy.digit.RDO.v12000301/calib1_csc11.005200.T1_McAtNlo_Jimmy.digit.RDO.v12000301_tid003138._%s.pool.root.%s" % (string0,string1)
  PoolRDOInput+=["%s" % filename]

#PoolRDOInput=[ "castor:/castor/cern.ch/user/r/rtt/RTTdata/calib1_csc11.005200.T1_McAtNlo_Jimmy.digit.RDO.v12000301/calib1_csc11.005200.T1_McAtNlo_Jimmy.digit.RDO.v12000301_tid003138._00001.pool.root.4" ]  # input file

#doTruth=False
#doESD=False
#doTrigger=False
#doWriteESD=False
doAOD=False
doWriteAOD=False
doWriteTAG=False
#include ("RecExCommon/RecExCommon_flags.py")
#DetFlags.Muon_setOff()
#DetFlags.ID_setOff()
#DetFlags.Calo_setOff()
include ("RecExCommon/RecExCommon_topOptions.py")
EOF
fi
## cat myTopOptions.py >> myTopOptions_tmp.py
mv -f myTopOptions_tmp.py myTopOptions.py 

#CANBEMODIFIED 
#this is the place where run time files can be overwritten 
# take my own topoption file
# rm -f myTopOptions.py
# 
# the other option is to build directly myTopOptions.py as done below


echo "## myTopOptions.py:"
cat myTopOptions.py



#echo "##use the following athena.py script"
#which athena.py
#echo "##PATH="$PATH
echo "##LD_LIBRARY_PATH="$LD_LIBRARY_PATH

#echo "##list of shared libraries"
#ls -l ../$CMTCONFIG

echo " " 
echo "##now running"
echo " " 

if [ ${(c)#VALGRIND} -eq 0 ] ; then
  # time athena.py -s   <&- >& athena.out
  time athena.py    <&- >& athena.out
else
    # magic line to fix oracle problem 
    source /afs/cern.ch/project/oracle/script/setoraenv.sh -s 10102gcc323
    time valgrind -v --leak-check=yes --leak-resolution=high --show-reachable=yes --num-callers=8 athena.py -s   <&- >&! athena.out
fi

#from John Baines allow to have traceback with gdb
#CORE=`ls core.*`
#if [ ! -z $CORE ]; then
#  echo analysing core file $CORE
#  export COMMAND="where
#  info f
#  bt 
#  q"
#  gdb -q -n athena.exe $CORE <<<$COMMAND
# fi




echo "##list of files in run directory"
ls -l 

echo "##now copy back some files"

if [ -z `echo $OUTPUTHIS | awk /castor/` ]; then
  if [ ! -d $OUTPUTHIS ]; then
    mkdir -p $OUTPUTHIS
  fi
else
  rfdir $OUTPUTHIS
  if [ $? -ne 0 ]; then
    rfmkdir -p $OUTPUTHIS
  fi
fi

if [ -z `echo $OUTPUTLOG | awk /castor/` ]; then
  if [ ! -d $OUTPUTLOG ]; then
    mkdir -p $OUTPUTLOG
  fi
else
  rfdir $OUTPUTLOG
  if [ $? -ne 0 ]; then
    rfmkdir -p $OUTPUTLOG
  fi
fi

echo $OUTPUTHIS/ntuple_$JOBNAME.root
rfcp ntuple.root $OUTPUTHIS/ntuple_$JOBNAME.root
echo $OUTPUTLOG/athena_$JOBNAME.log
rfcp athena.out $OUTPUTLOG/athena_$JOBNAME.log



echo "## remove all files"
#rm -fR *

#
#exit
