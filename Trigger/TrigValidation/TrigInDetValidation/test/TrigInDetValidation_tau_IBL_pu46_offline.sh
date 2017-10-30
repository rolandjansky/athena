#!/bin/bash
# art-description: art job for tau_IBL_pu46_offline
# art-type: grid
# art-output: HLTEF-plots
# art-output: HLTL2-plots
# art-output: times
# art-output: times-FTF
# art-output: cost-perCall
# art-output: cost-perEvent
# art-output: cost-perCall-chain
# art-output: cost-perEvent-chain
# art-input:  mc15_13TeV.361108.PowhegPythia8EvtGen_AZNLOCTEQ6L1_Ztautau.recon.RDO.e3601_s2757_r7245
# art-output: *.dat 
# art-output: *.root
# art-output: *.log

export RTTJOBNAME=TrigInDetValidation_tau_IBL_pu46_offline

get_files -jo            TrigInDetValidation/TrigInDetValidation_RTT_topOptions_TauSlice.py
athena.py  -c 'ARTConfig=["/eos/atlas/atlascerngroupdisk/proj-sit/trigindet/mc15_13TeV.361108.PowhegPythia8EvtGen_AZNLOCTEQ6L1_Ztautau.recon.RDO.e3601_s2757_r7245/RDO.06885566._001556.pool.root.1"];           EventMax=2000;runMergedChain=True;doIDNewTracking=True;PdgId=15'            TrigInDetValidation/TrigInDetValidation_RTT_topOptions_TauSlice.py
get_files -data TIDAdata11-rtt-offline.dat
get_files -data TIDAdata_cuts.dat
get_files -data TIDAdata_chains.dat
get_files -data TIDAbeam.dat
get_files -data Test_bin.dat
TIDArdict.exe TIDAdata11-rtt-offline.dat -f data-tau-IBL.root -b Test_bin.dat

TIDArun-art.sh data-tau-IBL.root data-tau_IBL_pu46_offline-reference.root HLT_tau25_idperf_track_InDetTrigTrackingxAODCnv_Tau_FTF HLT_tau25_idperf_track_InDetTrigTrackingxAODCnv_Tau_IDTrig   HLT_tau25_idperf_tracktwo_InDetTrigTrackingxAODCnv_TauIso_FTF_forID3 HLT_tau25_idperf_tracktwo_InDetTrigTrackingxAODCnv_Tau_IDTrig_forID3  -d HLTEF-plots

TIDArun-art.sh data-tau-IBL.root data-tau_IBL_pu46_offline-reference.root HLT_tau25_idperf_track_InDetTrigTrackingxAODCnv_Tau_FTF HLT_tau25_idperf_tracktwo_InDetTrigTrackingxAODCnv_TauCore_FTF_forID1 HLT_tau25_idperf_tracktwo_InDetTrigTrackingxAODCnv_TauIso_FTF_forID3 -d HLTL2-plots

TIDArun-art.sh expert-monitoring.root  expert-monitoring*-ref.root --auto -o times

TIDArun-art.sh expert-monitoring.root  expert-monitoring*-ref.root --auto -p FastTrack -o times-FTF

RunTrigCostD3PD.exe -f trig_cost.root  --outputTagFromAthena --costMode --linkOutputDir

TIDAcpucost.exe costMon/TrigCostRoot_Results.root costMon/TrigCostRoot_Results.root -o cost-perCall --auto -d "/Algorithm" -p "_Time_perCall"

TIDAcpucost.exe costMon/TrigCostRoot_Results.root costMon/TrigCostRoot_Results.root -o cost-perEvent --auto -d "/Algorithm" -p "_Time_perEvent"

TIDAcpucost.exe costMon/TrigCostRoot_Results.root costMon/TrigCostRoot_Results.root -o cost-perCall-chain --auto -d "/Chain_Algorithm" -p "_Time_perCall"

TIDAcpucost.exe costMon/TrigCostRoot_Results.root costMon/TrigCostRoot_Results.root -o cost-perEvent-chain --auto -d "/Chain_Algorithm" -p "_Time_perEvent"

