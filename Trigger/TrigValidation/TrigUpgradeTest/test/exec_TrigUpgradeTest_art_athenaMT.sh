#!/bin/bash
#
# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#
# Trigger ART test execution script based on TriggerTest/test/exec_athena_art_trigger_validation.sh
#

### DEFAULTS

if [ -z "${NAME}" ]; then
  testname=$(basename $0)
  testname=${testname#test_}
  testname=${testname%.sh}
  export NAME=${testname}
fi
echo $(date "+%FT%H:%M %Z")"     Execute Athena test ${NAME}"

if [ -z ${EVENTS} ]; then
  export EVENTS="1000"
fi

if [ -z ${SKIPEVENTS} ]; then
  export SKIPEVENTS="0"
fi

if [ -z ${JOBOPTION} ]; then
  export JOBOPTION="TriggerJobOpts/runHLT_standalone.py"
fi

if [ -z ${JOB_LOG} ]; then
  export JOB_LOG="athena.log"
fi

if [ -z ${INPUT} ]; then
  export INPUT="run2data"
fi

if [ -z ${THREADS} ]; then
  export THREADS="1"
fi

if [ -z ${SLOTS} ]; then
  export SLOTS="1"
fi

# Run with PerfMon by default
if [ -z ${DOPERFMON} ] || [ ${DOPERFMON} -ne 0 ]; then
  export PERFMONFLAG="--perfmon"
else
  export PERFMONFLAG=""
fi

if [ -z ${STDCMATH} ] || [ ${STDCMATH} -eq 0 ]; then
  if [ -f ${ATLASMKLLIBDIR_PRELOAD}/libimf.so ]; then
    export MATHLIBOPT="--imf"
  else
    echo "!!! WARNING !!! Intel math library is not available, using standard cmath library instead. This may lead to differences in test results."
    export MATHLIBOPT="--stdcmath"
  fi
else
  export MATHLIBOPT="--stdcmath"
fi

###

if [[ $INPUT == "run2data" ]]; then
  export DS="/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/TrigP1Test/data17_13TeV.00327265.physics_EnhancedBias.merge.RAW._lb0100._SFO-1._0001.1"
elif [[ $INPUT == "run2dataFTK" ]]; then
  export DS="/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/TrigP1Test/data18_13TeV.00360026.physics_EnhancedBias.MissingTowers._lb0151._SFO-6._0001.1,/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/TrigP1Test/data18_13TeV.00360026.physics_EnhancedBias.MissingTowers._lb0151._SFO-6._0002.1,/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/TrigP1Test/data18_13TeV.00360026.physics_EnhancedBias.MissingTowers._lb0151._SFO-6._0003.1,/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/TrigP1Test/data18_13TeV.00360026.physics_EnhancedBias.MissingTowers._lb0151._SFO-6._0004.1"
elif [[ $INPUT == "run2mc_ttbar" ]]; then
  # ttbar RDO_FTK produced in 21.3
  export DS="/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/TriggerTest/valid1.410000.PowhegPythiaEvtGen_P2012_ttbar_hdamp172p5_nonallhad.digit.RDO_FTK.e4993_s3214_r11234_d1505/RDO_FTK.17071950._000065.pool.root.1,/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/TriggerTest/valid1.410000.PowhegPythiaEvtGen_P2012_ttbar_hdamp172p5_nonallhad.digit.RDO_FTK.e4993_s3214_r11234_d1505/RDO_FTK.17071950._000235.pool.root.1"
elif [[ $INPUT == "ttbar" ]]; then
  # ttbar RDO produced with serial athena job in master r2019-03-20
  export DS="/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/TriggerTest/valid1.410000.PowhegPythiaEvtGen_P2012_ttbar_hdamp172p5_nonallhad.merge.RDO.e4993_s3214_r11315/RDO.17533168._000001.pool.root.1,/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/TriggerTest/valid1.410000.PowhegPythiaEvtGen_P2012_ttbar_hdamp172p5_nonallhad.merge.RDO.e4993_s3214_r11315/RDO.17533168._000002.pool.root.1"
elif [[ $INPUT == "none" ]]; then
  export DS=""
elif [[ -f $INPUT ]]; then
  export DS=${INPUT}
else
  echo "ERROR: input not defined, cannot run"
  echo "art-result: 1"
  exit 1
fi

######################################

# Generate empty PoolFileCatalog.xml - this prevents incorrect handling of crashes on the grid
ART_AVAILABLE=`which art.py >/dev/null 2>&1; echo $?`
if [[ $ART_AVAILABLE == "0" ]]; then
  echo "Executing art.py createpoolfile"
  art.py createpoolfile
fi

######################################

if [[ ${FROMPICKLE} == "1" ]]; then
  echo "Running athena from pickle file ${JOBOPTION} with the command:"
  (set -x
  athena.py \
  ${MATHLIBOPT} \
  ${PERFMONFLAG} \
  ${ATHENAOPTS} \
  ${JOBOPTION} >${JOB_LOG} 2>&1
  ) 2>&1
else
  echo "Running athena command:"
  (set -x
  athena.py \
  ${MATHLIBOPT} \
  ${PERFMONFLAG} \
  ${ATHENAOPTS} \
  --threads ${THREADS} \
  --concurrent-events ${SLOTS} \
  --filesInput "${DS}" \
  --evtMax ${EVENTS} \
  --skipEvents ${SKIPEVENTS} \
  -c "${EXTRA}" \
  ${JOBOPTION} >${JOB_LOG} 2>&1
  ) 2>&1
fi

######################################

export ATH_RETURN=$?
echo "art-result: ${ATH_RETURN} ${JOB_LOG%.*}"
echo  $(date "+%FT%H:%M %Z")"     Done executing Athena test ${NAME} with result ${ATH_RETURN}"

