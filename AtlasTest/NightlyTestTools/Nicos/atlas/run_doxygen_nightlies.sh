#!/usr/local/bin/zsh
# ==================================================================== #
#                                                                      #
#    run_doxygen_nightlies [Options]                                   #
#                                                                      #
#    Unix script to build the dbg or opt release.                      #
#                                                                      #
#    Options:                                                          #
#                                                                      #
#      -r RELEASE   Atlas Release (e.g. atlrel_2)                      #
#      -b PACKAGE   Begin at package PACKAGE                           #
#      -s PACKAGE   Select only package PACKAGE                        #
#      -h           Print usage                                        #
#                                                                      #
#    Requirements:                                                     #
#                                                                      #
# ==================================================================== #

# set -x

# --- Default parameters

pass="1"
begin=""
select=""
distDir="${NICOS_COPY_HOME}"
###if [ ${NICOS_PROJECT_HOME} != "" ]; then
###distDir="${NICOS_PROJECT_HOME}"
###fi 
cmtHome=${HOME}/cmthome
platform="i686-rh73-gcc32"
usageMsg="Usage: $(basename $0) [Options]
  Options:
    -r RELEASE   Atlas Release (e.g. atlrel_2)
    -b PACKAGE   Begin at package PACKAGE
    -s PACKAGE   Select only package PACKAGE
    -h           Print usage
"

# --- Input parameters

while [ $# -ge 1 ]; do
    if   [ "$1" = "-r" ]; then
        shift
        relNum=$1
        shift
    elif [ "$1" = "-b" ]; then
        shift
        begin="$1"
        shift
    elif [ "$1" = "-begin" ]; then
        shift
        begin="$1"
        shift
    elif [ "$1" = "-s" ]; then
        shift
        select="$1"
        shift
    elif [ "$1" = "-select" ]; then
        shift
        select="$1"
        shift
    elif [ "$1" = "-h" ]; then
        echo ${usageMsg}
        return
    elif [ "$1" = "--help" ]; then
        echo ${usageMsg}
        return
    else
        echo ${usageMsg}
        return
    fi
done

# --- Check input parameters

if [ "${relNum}" = "" ]; then
   echo "run_doxygen_nightlies: please specify release name with -r option."
   exit 1 
fi

# --- Release parameters

relDir=${distDir}/${relNum}
buildDir=${relDir}/${NICOS_COPY_TESTS_DIR}

# --- Set up log files

pass=1
globalLog="nicos_doxygen_${pass}.log"
while [ -e ${globalLog} ]; do
    let pass=${pass}+1
    globalLog="nicos_doxygen_${pass}.log"
done
startStamp="nicos_doxygen_${pass}.`hostname`.started"
finishStamp="nicos_doxygen_${pass}.`hostname`.finished"
#NICOS_TESTDIR=`dirname ${NICOS_TESTLOG}`
NICOS_TESTDIR="/build/atnight"
localLog="${NICOS_TESTDIR}/${globalLog}"
errorLog="${NICOS_TESTDIR}/nicos_errorsInDoxygen_${pass}.log"

# --- Set up environment
 
echo "Setting up environment for release ${relNum}..."
eval cd ${buildDir}
export CMTPATH="$relDir"
echo `pwd`
echo $CMTPATH
#source setup.sh -no_cleanup
source setup.sh 

# --- Build doxygen

beginFlag=""
echo "Building doxygen for release ${relNum}..."
if [ -n "${begin}" ]; then
    echo "Beginning at package ${begin}..."
    beginFlag="-begin=${begin}"
fi
selectFlag=""
if [ -n "${select}" ]; then
    echo "Selecting package ${select}..."
    selectFlag="-select=${select}"
fi

if [ -e "${localLog}" ]; then rm -f ${localLog}; fi
if [ -e "${globalLog}" ]; then rm -f ${globalLog}; fi
touch ${localLog}
touch ${globalLog}
touch ${startStamp}
echo "nohup cmt broadcast ${beginFlag} ${selectFlag} - make doxygen |& tee -a ${localLog} >>! ${globalLog}"
echo "Monitor build with \"tail -f ${globalLog}\""
nohup cmt broadcast ${beginFlag} ${selectFlag} - make doxygen |& tee -a ${localLog} >>! ${globalLog}
touch ${finishStamp}
grep "Error 1" ${globalLog} >& ${errorLog}
#
cd $distDir
if [ -e latest ]; then rm -f latest; fi
#ln -sf $NICOS_PROJECT_RELNAME latest
if [ -e latest_doxygen ]; then rm -f latest_doxygen; fi
ln -sf $NICOS_PROJECT_RELNAME latest_doxygen
if [ "$NICOS_PROJECT_RELNAME_COPY" != "" ]; then
rm -f latest_doxygen
ln -sf $NICOS_PROJECT_RELNAME_COPY latest_doxygen
fi