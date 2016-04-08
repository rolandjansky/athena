#!/bin/bash
#
# script to run monitoring on phase 1 commissioning files.
#
# 30. Jan. 06   R. Seuster adopt it for online farm
# 10. Jan. 06   R. Seuster make it run on kit and at CERN
# 06. Jan. 06   R. Seuster, K. Voss
#
# After setting variables depending on the installation of
# the atlas software, the enviroment and variables steering
# the job are set in the first part.
#
# Next, a short python file with all variables for this job is
# created.
#
# The job is run in its own run directory in the third part.
#

if [ $# -gt 4 ] ; then
   echo "Script does not accept more than three parameters!"
   echo ""
   echo "usage: ec_mon.sh [runnumber [evtno [ gain [jobOptions.py]]]]"
   echo "       runnumber     of the file to process (default 18720)"
   echo "       gain          of the file to process (default HIGH)"
   echo "       jobOptions.py of the algorithms to run (default RecExCommission_LAr_EC_INSTALLATION_jobOptions.py)"
   return
   exit
fi

##############################################################
#          first part, general installation setup            #
#                variables like runnumber                    #
##############################################################

# this is where your athena release is installed
#export BASEPATH="/atlas/slc3"
# at CERN this would be
export BASEPATH="/afs/cern.ch/atlas/software"
# P1:
# export BASEPATH="/space/sw/offline_test"

# which release to run
ATLAS_RELEASE="11.2.0"

# this is the run directory of the job, please give the absolute path !
# RUNPATH=${HOME}/Athena/11.2.0-MON
RUNPATH=${PWD}

# the runnumber to process
RUNNUMBER="1020"

# the location of the datafiles disc or castor
DATPATH="/castor/cern.ch/atlas/LargFec/EC_Installation"

#
EVTNO="10000000"

# the gain of the noisefile
GAIN=HIGH

# the main joboptions file
JOBOPTIONFILE=RecExCommission_LAr_EC_INSTALLATION_jobOptions.py

# overwrite, if given on command line
if [ $# -ge 1 ]; then
 RUNNUMBER="$1"
 shift
fi

# overwrite, if given on command line
if [ $# -ge 1 ]; then
 EVTNO="$1"
 shift
fi

# overwrite, if given on command line
if [ $# -ge 1 ]; then
 GAIN="$1"
 shift
fi

# overwrite, if given on command line
if [ $# -eq 1 ]; then
 JOBOPTIONFILE=$1
 shift
fi

# construct string containing runnumber including leading 0's
RUNSTR="`expr substr 0000000 1 \( 7 - length $RUNNUMBER \)`$RUNNUMBER"

# name of logfile
LOGFILE=logfile.$RUNSTR

# do some basic cross checks, existance of directories and files
if [ ! -d $RUNPATH ]; then
 echo Your run directory does not exist !!
 echo Please create one with
 echo \"mkdir $RUNPATH \"
 return
 exit
fi


# FILEPATH="${DATPATH}daq_EB-C_${GAIN}_InstPed_${RUNSTR}_file01.data"

# if [ ! -e $FILEPATH ]; then
#  echo Your datafile does not exist !!
#  echo Looking for $FILEPATH
# # return
# # exit
# fi

# print out the setup:
echo "Setting up for"
echo "Atlas Release : $ATLAS_RELEASE"
echo " installed in : $BASEPATH"
echo ""
echo "The run directory is    : $RUNPATH"
echo "The data directory is   : $DATPATH"
echo "The run number is       : $RUNSTR"
echo "The number of events    : $EVTNO"
echo "The gain of this run is : $GAIN"
echo "The main jobOptions are : $JOBOPTIONFILE"
echo ""
echo "The output will be in   : $LOGFILE"

# setup the athena enviroment

#if [ ! -e $BASEPATH/$ATLAS_RELEASE/setup.sh ]; then
#    echo "This AtlasRelease does not exist in this configuration !"
#    echo $BASEPATH/$ATLAS_RELEASE
##    return
##    exit
#fi

# running at CERN ?
if [ $BASEPATH = "/afs/cern.ch/atlas/software" ]; then
 source ~/Athena/cmthome/setup.sh -tag=$ATLAS_RELEASE,groupArea
 source ${BASEPATH}/dist/${ATLAS_RELEASE}/Control/AthenaRunTime/*/cmt/setup.sh
else
 # basic setup and distribution dependent variables
 # source ${BASEPATH}/${ATLAS_RELEASE}/setup.sh
 source ${BASEPATH}/${ATLAS_RELEASE}-MON/setup.sh
 export CMTPATH=${RUNPATH}:${CMTPATH}
 # multiple releases in installation directory ?
 # source ${BASEPATH}/${ATLAS_RELEASE}/dist/${ATLAS_RELEASE}/Control/AthenaRunTime/*/cmt/setup.sh
 source ${BASEPATH}/${ATLAS_RELEASE}-MON/dist/${ATLAS_RELEASE}/Control/AthenaRunTime/*/cmt/setup.sh
 # source ${BASEPATH}/kits/dist/${ATLAS_RELEASE}/Control/AthenaRunTime/*/cmt/setup.sh
fi

# variables to access the CVS repository at CERN (via ssh)
# P1:
# export CVSROOT=:ext:atlas-sw.cern.ch:/atlascvs
# export CVS_RSH=ssh



if [ `basename $0` = "ec_mon.sh" ]; then

 if [ ! -e $JOBOPTIONFILE ]; then
  echo No $JOBOPTIONFILE yet !
  echo Getting it for you
  get_files $JOBOPTIONFILE
 else
  echo Using $JOBOPTIONFILE in your run directory !
 fi
 
 MONDIR=~/scratch0/monitor.run.$RUNSTR
 TOPOPT=myTopOptions.py
 HEREDIR=`pwd` 


 mkdir -p $MONDIR
 cd $MONDIR

cat <<EOF > $TOPOPT

RunNumber = $RUNNUMBER
LArDigitKey = "$GAIN"
EvtNo=$EVTNO
InputDir = "$DATPATH"

EOF

 # now run athena
 echo starting athena
 athena.py -bs $TOPOPT $HEREDIR/$JOBOPTIONFILE >& $LOGFILE
else
 echo just setting up the enviroment
fi

