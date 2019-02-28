#!/bin/bash

echo  $(date "+%FT%H:%M %Z")"     Execute Athena test ${NAME}"

### DEFAULTS

if [ -z ${MENU} ]; then
  export MENU="MC_pp_v7_TriggerValidation_mc_prescale"
fi

if [ -z ${EVENTS} ]; then
  export EVENTS="1000"
fi

if [ -z ${JOBOPTION} ]; then
  if [[ $INPUT == "data" ]]; then
    export JOBOPTION="TriggerRelease/runHLT_standalone.py"
  else
    export JOBOPTION="TriggerTest/testCommonSliceAthenaTrigRDO.py"
  fi
fi

if [ -z ${JOB_LOG} ]; then
  export JOB_LOG="athena.log"
fi

if [ -z ${EXTRA} ]; then
  export EXTRA="extraPython=False;"
fi

if [ -z ${COST_MONITORING} ]; then
  export COST_MONITORING="True"
fi

###

if [[ $INPUT == "pbpb" ]]; then
  export DS='["/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/TriggerTest/mc15_5TeV.420000.Hijing_PbPb_5p02TeV_MinBias_Flow_JJFV6.recon.RDO.e3754_s2633_r7161/RDO.06677682._000002.pool.root.1","/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/TriggerTest/mc15_5TeV.420000.Hijing_PbPb_5p02TeV_MinBias_Flow_JJFV6.recon.RDO.e3754_s2633_r7161/RDO.06677682._000003.pool.root.1"]'

elif [[ $INPUT == "ftk" ]]; then
  export DS='["/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/TriggerTest/mc16_13TeV.410470.PhPy8EG_A14_ttbar_hdamp258p75_nonallhad.digit.RDO_FTK.e6337_e5984_s3126_d1480_d1471_tid15265974_00/RDO_FTK.15265974._004440.pool.root.1"]'

elif [[ $INPUT == "bphysics" ]]; then
  export DS='["/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/TriggerTest/RDO_FTK.Bphysics_1000.pool.root.1"]'

elif [[ $INPUT == "mubphysics" ]]; then
  export DS='["/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/TriggerTest/mc16_13TeV.180593.Pythia8_AUMSTW2008LO_Wtaunu_3mu_noPhotos.digit.RDO_FTK.e3802_s3126_r11234_d1505/RDO_FTK.17121384._000012.pool.root.1","/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/TriggerTest/mc16_13TeV.180593.Pythia8_AUMSTW2008LO_Wtaunu_3mu_noPhotos.digit.RDO_FTK.e3802_s3126_r11234_d1505/RDO_FTK.17121384._000028.pool.root.1"]'

elif [[ $INPUT == "minbias" ]]; then
  export DS='["/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/TriggerTest/RDO_FTK.minbias_1000.pool.root.1"]'

elif [[ $INPUT == 'data' ]]; then
  export DS='["/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/TrigP1Test/data18_13TeV.00360026.physics_EnhancedBias.recon.RAW.r10978/RAW.15789997._002546.pool.root.1"]'

else 
  # Default - ttbar
  export DS='["/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/TriggerTest/valid1.410000.PowhegPythiaEvtGen_P2012_ttbar_hdamp172p5_nonallhad.digit.RDO_FTK.e4993_s3214_r11234_d1505/RDO_FTK.17071950._000065.pool.root.1","/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/TriggerTest/valid1.410000.PowhegPythiaEvtGen_P2012_ttbar_hdamp172p5_nonallhad.digit.RDO_FTK.e4993_s3214_r11234_d1505/RDO_FTK.17071950._000235.pool.root.1"]'
fi

trap 'PREVIOUS_COMMAND=$THIS_COMMAND; THIS_COMMAND=$BASH_COMMAND' DEBUG

######################################

if [[ $INPUT == 'data' ]]; then
  athena.py -b -c \
  "setMenu=\"${MENU}\";\
  BSRDOInput=${DS};\
  EvtMax=${EVENTS};\
  ${EXTRA}\
  LVL1OutputLevel=WARNING;\
  HLTOutputLevel=WARNING;" \
  ${JOBOPTION} &> ${JOB_LOG}
else
  athena.py -b -c \
  "enableCostMonitoring=${COST_MONITORING};\
  RunningRTT=True;\
  menu=\"${MENU}\";\
  sliceName=\"${SLICE}\";\
  jp.AthenaCommonFlags.FilesInput=${DS};\
  jp.AthenaCommonFlags.EvtMax.set_Value_and_Lock(${EVENTS});\
  jp.Rec.OutputLevel=WARNING;\
  ${EXTRA}\
  LVL1OutputLevel=WARNING;\
  HLTOutputLevel=WARNING;" \
  ${JOBOPTION} &> ${JOB_LOG}
fi

######################################

COMMAND=$PREVIOUS_COMMAND ATH_RETURN=$?
echo ${COMMAND} > command.txt
echo "Command to reproduce:"
envsubst < command.txt
echo "art-result: ${ATH_RETURN} ${JOB_LOG%%.*}"
echo  $(date "+%FT%H:%M %Z")"     Done executing Athena test ${NAME}"
