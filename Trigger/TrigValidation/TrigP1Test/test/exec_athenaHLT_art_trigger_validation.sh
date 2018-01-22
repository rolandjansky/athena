#!/bin/bash

echo  "Execute AthenaHLT"

### DEFAULTS

if [ -z ${MENU} ]; then
  export MENU="Physics_pp_v7_unprescaled_only_physics_prescale"
fi

if [ -z ${EVENTS} ]; then
  export EVENTS="1000"
fi

if [ -z ${JOBOPTION} ]; then
  export JOBOPTION="TriggerTest/testCommonSliceAthenaTrigRDO.py"
fi

if [ -z ${JOB_LOG} ]; then
  export JOB_LOG="athena.log"
fi

if [ -z ${FILE} ]; then
  export FILE="/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/TrigP1Test/data17_13TeV.00339070.physics_EnhancedBias.merge.RAW._lb0101._SFO-1._0001.1"
fi

if [ -z ${TESTOPTION} ]; then
  export TESTOPTION="setMenu='Physics_pp_v7_unprescaled_only_physics_prescale';setLVL1XML='TriggerMenuXML/LVL1config_Physics_pp_v7.xml';setL1TopoXML=setLVL1XML.replace('/LVL1','/L1Topo');rerunLVL1=True;enableCostD3PD=True;enableCostForCAF=True"
fi

if [ -z ${ATHENAHLTOPT} ]; then
export ATHENAHLTOPT=""
fi

if [ -z ${ATHENAHLTCMD} ]; then
export ATHENAHLTCMD="athenaHLT.py"
fi

######################################

${ATHENAHLTCMD} ${ATHENAHLTOPT} \
-n ${EVENTS} \
-f "${FILE}" \
-c "${TESTOPTION}" \
${JOBOPTION} | tee ${JOB_LOG}

######################################

COMMAND=$PREVIOUS_COMMAND 
ATH_RETURN=${PIPESTATUS[0]}
echo "Command to reproduce: ${COMMAND}"
echo "art-result: ${ATH_RETURN} ${JOB_LOG%%.*}"




