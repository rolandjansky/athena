#!/bin/bash

echo  $(date "+%FT%H:%M %Z")"     Execute Athena test ${NAME}"

### DEFAULTS

if [ -z ${MENU} ]; then
  export MENU="MC_pp_v8"
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

if [ -z "${EXTRA}" ]; then
  export EXTRA="extraPython=False;"
fi

if [ -z ${COST_MONITORING} ]; then
  export COST_MONITORING="True"
fi

###

if [[ $INPUT == "pbpb" ]]; then
  export DS='["/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/TriggerTest/mc15_5TeV.420000.Hijing_PbPb_5p02TeV_MinBias_Flow_JJFV6.recon.RDO.e3754_s2633_r7161/RDO.06677682._000002.pool.root.1","/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/TriggerTest/mc15_5TeV.420000.Hijing_PbPb_5p02TeV_MinBias_Flow_JJFV6.recon.RDO.e3754_s2633_r7161/RDO.06677682._000003.pool.root.1"]'

elif [[ $INPUT == "bphysics" ]]; then
  export DS='["/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/TriggerTest/mc15_RTT_1000_Bphysics_toAOD_r6889/Bphysics_RDO_r6889_mixed.pool.root"]'

elif [[ $INPUT == "mubphysics" ]]; then
  export DS='["/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/TriggerTest/mc16_13TeV.180593.Pythia8_AUMSTW2008LO_Wtaunu_3mu_noPhotos.merge.RDO.e3802_s3126_r11478/RDO.18537785._000009.pool.root.1","/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/TriggerTest/mc16_13TeV.180593.Pythia8_AUMSTW2008LO_Wtaunu_3mu_noPhotos.merge.RDO.e3802_s3126_r11478/RDO.18537785._000010.pool.root.1"]'

elif [[ $INPUT == "minbias" ]]; then
  export DS='["/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/TriggerTest/mc15_13TeV.361203.Pythia8_A2_MSTW2008LO_ND_minbias.recon.RDO.e3639_s2606_s2174_r7661_tid07858100_00/RDO.07858100._000087.pool.root.1"]'

elif [[ $INPUT == 'data' ]]; then
  export DS='["/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/TrigP1Test/data18_13TeV.00360026.physics_EnhancedBias.recon.RAW.r10978/RAW.15789997._000435.pool.root.1","/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/TrigP1Test/data18_13TeV.00360026.physics_EnhancedBias.recon.RAW.r10978/RAW.15789997._001626.pool.root.1","/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/TrigP1Test/data18_13TeV.00360026.physics_EnhancedBias.recon.RAW.r10978/RAW.15789997._002546.pool.root.1","/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/TrigP1Test/data18_13TeV.00360026.physics_EnhancedBias.recon.RAW.r10978/RAW.15789997._005117.pool.root.1"]'

elif [[ $INPUT == 'run3_l1calo' ]]; then
  export DS='["/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/TriggerTest/valid1.410000.PowhegPythiaEvtGen_P2012_ttbar_hdamp172p5_nonallhad.merge.RDO.e4993_s3214_d1536_tid18795273_00/RDO.18795273._000063.pool.root.1"]'

else 
  # Default - ttbar
  export DS='["/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/TriggerTest/valid1.410000.PowhegPythiaEvtGen_P2012_ttbar_hdamp172p5_nonallhad.digit.RDO.e4993_s3214_d1540_tid19539348_00/RDO.19539348._000084.pool.root.1","/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/TriggerTest/valid1.410000.PowhegPythiaEvtGen_P2012_ttbar_hdamp172p5_nonallhad.digit.RDO.e4993_s3214_d1540_tid19539348_00/RDO.19539348._000041.pool.root.1"]'
fi

######################################

# Generate empty PoolFileCatalog.xml - this prevents incorrect handling of crashes on the grid
# Generate empty PoolFileCatalog.xml - this prevents incorrect handling of crashes on the grid
ART_AVAILABLE=`which art.py >/dev/null 2>&1; echo $?`
if [[ $ART_AVAILABLE == "0" ]]; then
  echo "Executing art.py createpoolfile"
  art.py createpoolfile
fi

######################################

echo "Running athena command:"
if [[ $INPUT == 'data' ]]; then
  (set -x
  athena.py --preloadlib=$ATLASMKLLIBDIR_PRELOAD/libintlc.so.5:$ATLASMKLLIBDIR_PRELOAD/libimf.so -b -c \
  "setMenu=\"${MENU}\";\
  BSRDOInput=${DS};\
  EvtMax=${EVENTS};\
  ${EXTRA}\
  LVL1OutputLevel=WARNING;\
  HLTOutputLevel=WARNING;" \
  ${JOBOPTION} >${JOB_LOG} 2>&1
  ) 2>&1
else
  (set -x
  athena.py --preloadlib=$ATLASMKLLIBDIR_PRELOAD/libintlc.so.5:$ATLASMKLLIBDIR_PRELOAD/libimf.so -b -c \
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
  ${JOBOPTION} >${JOB_LOG} 2>&1
  ) 2>&1
fi

######################################

export ATH_RETURN=$?
echo "art-result: ${ATH_RETURN} ${JOB_LOG%%.*}"
echo  $(date "+%FT%H:%M %Z")"     Done executing Athena test ${NAME}"

