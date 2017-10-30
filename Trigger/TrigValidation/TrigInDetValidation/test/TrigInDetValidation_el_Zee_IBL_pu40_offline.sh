#!/bin/bash
# art-description: art job for el_Zee_IBL_pu40_offline
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

export RTTJOBNAME=TrigInDetValidation_el_Zee_IBL_pu40_offline

get_files -jo            TrigInDetValidation/TrigInDetValidation_RTT_topOptions_ElectronSlice.py
athena.py  -c 'ARTConfig=["/eos/atlas/atlascerngroupdisk/proj-sit/trigindet/mc15_13TeV.361106.PowhegPythia8EvtGen_AZNLOCTEQ6L1_Zee.recon.RDO.e3601_s2665_s2183_r7191/RDO.06753544._000001.pool.root.1"];           EventMax=2500;runMergedChain=True;doIDNewTracking=True;PdgId=11'            TrigInDetValidation/TrigInDetValidation_RTT_topOptions_ElectronSlice.py
get_files -data TIDAdata11-rtt-offline.dat
get_files -data TIDAdata_cuts.dat
get_files -data TIDAdata_chains.dat
get_files -data TIDAbeam.dat
get_files -data Test_bin.dat
TIDArdict.exe TIDAdata11-rtt-offline.dat -f data-electron-merge.root -b Test_bin.dat

TIDArun-art.sh data-electron-merge.root data-el_Zee_IBL_pu40_offline-reference.root HLT_e24_medium_idperf_InDetTrigTrackingxAODCnv_Electron_FTF  HLT_e24_medium_idperf_InDetTrigTrackingxAODCnv_Electron_IDTrig   -d HLTEF-plots

TIDArun-art.sh data-electron-merge.root data-el_Zee_IBL_pu40_offline-reference.root HLT_e24_medium_idperf_InDetTrigTrackingxAODCnv_Electron_FTF  -d HLTL2-plots

TIDArun-art.sh data-electron-merge.root data-el_Zee_IBL_pu40_offline-reference.root  HLT_e5_loose_idperf_InDetTrigTrackingxAODCnv_Electron_FTF   HLT_e5_loose_idperf_InDetTrigTrackingxAODCnv_Electron_IDTrig   -d HLTEF-plots-lowpt

TIDArun-art.sh data-electron-merge.root data-el_Zee_IBL_pu40_offline-reference.root  HLT_e5_loose_idperf_InDetTrigTrackingxAODCnv_Electron_FTF   -d HLTL2-plots-lowpt

TIDArun-art.sh expert-monitoring.root  expert-monitoring*-ref.root --auto -o times

TIDArun-art.sh expert-monitoring.root  expert-monitoring*-ref.root --auto -p FastTrack -o times-FTF

RunTrigCostD3PD.exe -f trig_cost.root  --outputTagFromAthena --costMode --linkOutputDir

TIDAcpucost.exe costMon/TrigCostRoot_Results.root costMon/TrigCostRoot_Results.root -o cost-perCall --auto -d "/Algorithm" -p "_Time_perCall"

TIDAcpucost.exe costMon/TrigCostRoot_Results.root costMon/TrigCostRoot_Results.root -o cost-perEvent --auto -d "/Algorithm" -p "_Time_perEvent"

TIDAcpucost.exe costMon/TrigCostRoot_Results.root costMon/TrigCostRoot_Results.root -o cost-perCall-chain --auto -d "/Chain_Algorithm" -p "_Time_perCall"

TIDAcpucost.exe costMon/TrigCostRoot_Results.root costMon/TrigCostRoot_Results.root -o cost-perEvent-chain --auto -d "/Chain_Algorithm" -p "_Time_perEvent"

