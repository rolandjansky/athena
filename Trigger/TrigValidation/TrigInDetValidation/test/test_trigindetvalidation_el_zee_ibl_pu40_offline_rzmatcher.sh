#!/bin/bash
# art-description: art job for el_Zee_IBL_pu40_offline_rzMatcher
# art-type: grid
# art-output: HLTEF-plots
# art-output: HLTL2-plots
# art-output: HLTEF-plots-lowpt
# art-output: HLTL2-plots-lowpt
# art-output: times
# art-output: times-FTF
# art-output: cost-perCall
# art-output: cost-perEvent
# art-output: cost-perCall-chain
# art-output: cost-perEvent-chain
# art-input:  mc15_13TeV.361106.PowhegPythia8EvtGen_AZNLOCTEQ6L1_Zee.recon.RDO.e3601_s2665_s2183_r7191
# art-output: *.dat 
# art-output: *.root
# art-output: *.log
# art-input-nfiles: 25



export RTTJOBNAME=TrigInDetValidation_el_Zee_IBL_pu40_offline_rzMatcher


fileList="['${ArtInFile//,/', '}']"
echo "List of files = $fileList"

get_files -jo            TrigInDetValidation/TrigInDetValidation_RTT_topOptions_ElectronSlice.py
athena.py  -c "ARTConfig=$fileList;EventMax=5000;doIDNewTracking=True;PdgId=11"            TrigInDetValidation/TrigInDetValidation_RTT_topOptions_ElectronSlice.py
echo "art-result: $? athena_0"

get_files -data TIDAdata11-rtt-offline-rzMatcher.dat
get_files -data TIDAdata_cuts.dat
get_files -data TIDAdata_chains.dat
get_files -data TIDAbeam.dat
get_files -data Test_bin.dat
TIDArdict.exe TIDAdata11-rtt-offline-rzMatcher.dat -f data-electron-merge.root -b Test_bin.dat
echo "art-result: $? TIDArdict_1"

TIDArun-art.sh data-electron-merge.root data-el_Zee_IBL_pu40_offline_rzMatcher-reference.root HLT_e24_medium_idperf_InDetTrigTrackingxAODCnv_Electron_FTF  HLT_e24_medium_idperf_InDetTrigTrackingxAODCnv_Electron_IDTrig   -d HLTEF-plots
echo "art-result: $? TIDArun_2"

TIDArun-art.sh data-electron-merge.root data-el_Zee_IBL_pu40_offline_rzMatcher-reference.root HLT_e24_medium_idperf_InDetTrigTrackingxAODCnv_Electron_FTF  -d HLTL2-plots
echo "art-result: $? TIDArun_3"

TIDArun-art.sh data-electron-merge.root data-el_Zee_IBL_pu40_offline_rzMatcher-reference.root  HLT_e5_loose_idperf_InDetTrigTrackingxAODCnv_Electron_FTF  HLT_e5_loose_idperf_InDetTrigTrackingxAODCnv_Electron_IDTrig   -d HLTEF-plots-lowpt
echo "art-result: $? TIDArun_4"

TIDArun-art.sh data-electron-merge.root data-el_Zee_IBL_pu40_offline_rzMatcher-reference.root HLT_e5_loose_idperf_InDetTrigTrackingxAODCnv_Electron_FTF  -d HLTL2-plots-lowpt
echo "art-result: $? TIDArun_5"

TIDArun-art.sh expert-monitoring.root  expert-monitoring*-ref.root --auto -o times
echo "art-result: $? TIDArun_6"

TIDArun-art.sh expert-monitoring.root  expert-monitoring*-ref.root --auto -p FastTrack -o times-FTF
echo "art-result: $? TIDArun_7"

RunTrigCostD3PD.exe -f trig_cost.root  --outputTagFromAthena --costMode --linkOutputDir
echo "art-result: $? RunTrigCostD3PD_8"

TIDAcpucost.exe costMon/TrigCostRoot_Results.root costMon/TrigCostRoot_Results.root -o cost-perCall --auto -d "/Algorithm" -p "_Time_perCall"
echo "art-result: $? TIDAcpucost_9"

TIDAcpucost.exe costMon/TrigCostRoot_Results.root costMon/TrigCostRoot_Results.root -o cost-perEvent --auto -d "/Algorithm" -p "_Time_perEvent"
echo "art-result: $? TIDAcpucost_10"

TIDAcpucost.exe costMon/TrigCostRoot_Results.root costMon/TrigCostRoot_Results.root -o cost-perCall-chain --auto -d "/Chain_Algorithm" -p "_Time_perCall"
echo "art-result: $? TIDAcpucost_11"

TIDAcpucost.exe costMon/TrigCostRoot_Results.root costMon/TrigCostRoot_Results.root -o cost-perEvent-chain --auto -d "/Chain_Algorithm" -p "_Time_perEvent"
echo "art-result: $? TIDAcpucost_12"

