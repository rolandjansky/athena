#!/bin/bash
#
# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#
# Trigger ART test execution script based on TriggerTest/test/exec_athena_art_trigger_validation.sh
#
echo $(date "+%FT%H:%M %Z")"     Execute Athena test ${NAME}"

### DEFAULTS

if [ -z ${EVENTS} ]; then
  export EVENTS="1000"
fi

if [ -z ${SKIPEVENTS} ]; then
  export SKIPEVENTS="0"
fi

if [ -z ${JOBOPTION} ]; then
  export JOBOPTION="TriggerRelease/runHLT_standalone.py"
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

###

if [[ $INPUT == "run2data" ]]; then
  export DS="/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/TrigP1Test/data17_13TeV.00327265.physics_EnhancedBias.merge.RAW._lb0100._SFO-1._0001.1"
elif [[ $INPUT == "run2mc_ttbar" ]]; then
  export DS="/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/TriggerTest/valid1.110401.PowhegPythia_P2012_ttbar_nonallhad.recon.RDO.e3099_s2578_r7572_tid07644622_00/RDO.07644622._000001.pool.root.1,/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/TriggerTest/valid1.110401.PowhegPythia_P2012_ttbar_nonallhad.recon.RDO.e3099_s2578_r7572_tid07644622_00/RDO.07644622._000002.pool.root.1"
else
  echo "ERROR: input not defined, cannot run"
  echo "art-result: 1"
  exit 1
fi

trap 'PREVIOUS_COMMAND=$THIS_COMMAND; THIS_COMMAND=$BASH_COMMAND' DEBUG

######################################

if [[ ${FROMPICKLE} == "1" ]]; then
  echo "Running athena from pickle file ${JOBOPTION}"
  athena.py \
  ${JOBOPTION} &> ${JOB_LOG}
else
  athena.py \
  --threads ${THREADS} \
  --concurrent-events ${SLOTS} \
  --filesInput ${DS} \
  --evtMax ${EVENTS} \
  --skipEvents ${SKIPEVENTS} \
  -c "${EXTRA}" \
  ${JOBOPTION} &> ${JOB_LOG}
fi

######################################

COMMAND=$PREVIOUS_COMMAND ATH_RETURN=$?
echo ${COMMAND} > command.txt
echo "Command to reproduce:"
envsubst < command.txt
echo "art-result: ${ATH_RETURN} ${JOB_LOG%%.*}"
echo  $(date "+%FT%H:%M %Z")"     Done executing Athena test ${NAME} with result ${ATH_RETURN}"
if [ "${ATH_RETURN}" -ne "0" ]; then
  exit ${ATH_RETURN}
fi


