#!/bin/bash

echo  "Execute Athena"

### DEFAULTS

if [ -z ${MENU} ]; then
  export MENU="MC_pp_v7_tightperf_mc_prescale"
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

if [ -z ${EXTRA} ]; then
  export EXTRA="extraPython=False;"
fi

if [ -z ${COST_MONITORING} ]; then
  export COST_MONITORING="True"
fi

###

if [[ $INPUT == "pbpb" ]]; then
  export DS='["/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/TriggerTest/mc15_5TeV.420000.Hijing_PbPb_5p02TeV_MinBias_Flow_JJFV6.recon.RDO.e3754_s2633_r7161/RDO.06677682._000001.pool.root.1","/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/TriggerTest/mc15_5TeV.420000.Hijing_PbPb_5p02TeV_MinBias_Flow_JJFV6.recon.RDO.e3754_s2633_r7161/RDO.06677682._000002.pool.root.1","/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/TriggerTest/mc15_5TeV.420000.Hijing_PbPb_5p02TeV_MinBias_Flow_JJFV6.recon.RDO.e3754_s2633_r7161/RDO.06677682._000003.pool.root.1"]'

elif [[ $INPUT == "ftk" ]]; then
  export DS='["/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/TriggerTest/RDO_FTK.10733709._000019.pool.root.1"]'

elif [[ $INPUT == "bphysics" ]]; then
  export DS='["/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/TriggerTest/mc15_RTT_1000_Bphysics_toAOD_r6889/Bphysics_RDO_r6889_mixed.pool.root"]'

elif [[ $INPUT == "mubphysics" ]]; then
  export DS='["/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/TriggerTest/mc15_13TeV.180593.Pythia8_AUMSTW2008LO_Wtaunu_3mu_noPhotos.recon.RDO.e3802_s2608_s2183_r6790/RDO.05594910._000008.pool.root.1"]'

elif [[ $INPUT == "minbias" ]]; then
  export DS='["/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/TriggerTest/mc15_13TeV.361203.Pythia8_A2_MSTW2008LO_ND_minbias.recon.RDO.e3639_s2606_s2174_r7661_tid07858100_00/RDO.07858100._000087.pool.root.1"]'

else 
  # Default - ttbar
  export DS='["/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/TriggerTest/valid1.110401.PowhegPythia_P2012_ttbar_nonallhad.recon.RDO.e3099_s2578_r7572_tid07644622_00/RDO.07644622._000001.pool.root.1","/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/TriggerTest/valid1.110401.PowhegPythia_P2012_ttbar_nonallhad.recon.RDO.e3099_s2578_r7572_tid07644622_00/RDO.07644622._000002.pool.root.1"]'
fi

trap 'PREVIOUS_COMMAND=$THIS_COMMAND; THIS_COMMAND=$BASH_COMMAND' DEBUG

######################################

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
${JOBOPTION} | tee ${JOB_LOG}

######################################

COMMAND=$PREVIOUS_COMMAND ATH_RETURN=${PIPESTATUS[0]}
echo "Command to reproduce: ${COMMAND}"
echo "art-result: ${ATH_RETURN} ${JOB_LOG%%.*}"
