#!/bin/sh

get_files -jo TrigInDetValidation/TrigInDetValidation_RTT_topOptions_AllSlices.py
athena.py  -c 'XMLDataSet="TrigInDetValidation_all_ttbar_pu80";EventMax=1000;runMergedChain=True;doIDNewTracking=True;rec.doFloatingPointException.set_Value_and_Lock(False)' TrigInDetValidation/TrigInDetValidation_RTT_topOptions_AllSlices.py 


get_files -data TIDAdata11-rtt.dat
get_files -data Test_bin.dat
TIDArdict.exe TIDAdata11-rtt.dat -r Offline -f data-all.root -b Test_bin.dat

get_files -data data-all_ttbar_pu46-reference.root
TIDAcomparitor.exe data-all.root data-all_ttbar_pu46-reference.root HLT_e24_medium_idperf_InDetTrigTrackingxAODCnv_Electron_FTF HLT_e24_medium_idperf_InDetTrigTrackingxAODCnv_Electron_IDTrig -d HLTEF-plots-electron

get_files -data data-all_ttbar_pu46-reference.root
TIDAcomparitor.exe data-all.root data-all_ttbar_pu46-reference.root HLT_e24_medium_idperf_InDetTrigTrackingxAODCnv_Electron_FTF -d HLTL2-plots-electron

get_files -data data-all_ttbar_pu46-reference.root
TIDAcomparitor.exe data-all.root data-all_ttbar_pu46-reference.root HLT_e5_loose_idperf_InDetTrigTrackingxAODCnv_Electron_FTF HLT_e5_loose_idperf_InDetTrigTrackingxAODCnv_Electron_IDTrig -d HLTEF-plots-electron-lowpt

get_files -data data-all_ttbar_pu46-reference.root
TIDAcomparitor.exe data-all.root data-all_ttbar_pu46-reference.root HLT_e5_loose_idperf_InDetTrigTrackingxAODCnv_Electron_FTF -d HLTL2-plots-electron-lowpt

get_files -data data-all_ttbar_pu46-reference.root
TIDAcomparitor.exe data-all.root data-all_ttbar_pu46-reference.root HLT_mu24_idperf_InDetTrigTrackingxAODCnv_Muon_FTF HLT_mu24_idperf_InDetTrigTrackingxAODCnv_Muon_IDTrig -d HLTEF-plots-muon

get_files -data data-all_ttbar_pu46-reference.root
TIDAcomparitor.exe data-all.root data-all_ttbar_pu46-reference.root HLT_mu24_idperf_InDetTrigTrackingxAODCnv_Muon_FTF -d HLTL2-plots-muon

get_files -data data-all_ttbar_pu46-reference.root
TIDAcomparitor.exe data-all.root data-all_ttbar_pu46-reference.root HLT_tau25_idperf_track_InDetTrigTrackingxAODCnv_Tau_IDTrig HLT_tau25_idperf_tracktwo_InDetTrigTrackingxAODCnv_Tau_IDTrig_forID3 HLT_tau20_r1_idperf_InDetTrigTrackingxAODCnv_Tau_EFID -d HLTEF-plots-tau

get_files -data data-all_ttbar_pu46-reference.root
TIDAcomparitor.exe data-all.root data-all_ttbar_pu46-reference.root HLT_tau25_idperf_track_InDetTrigTrackingxAODCnv_Tau_FTF HLT_tau25_idperf_tracktwo_InDetTrigTrackingxAODCnv_TauCore_FTF_forID1 HLT_tau25_idperf_tracktwo_InDetTrigTrackingxAODCnv_TauIso_FTF_forID3 HLT_tau20_r1_idperf_TrigL2SiTrackFinder_Tau_2 -d HLTL2-plots-tau

get_files -data data-all_ttbar_pu46-reference.root
get_files -data data-all_ttbar_pu46-reference.root
get_files -data data-all_ttbar_pu46-reference.root
get_files -data data-all_ttbar_pu46-reference.root
get_files -data data-all_ttbar_pu46-reference.root
get_files -data data-all_ttbar_pu46-reference.root
get_files -data data-all_ttbar_pu46-reference.root
get_files -data data-all_ttbar_pu46-reference.root
get_files -data data-all_ttbar_pu46-reference.root
get_files -data data-all_ttbar_pu46-reference.root
TIDAcomparitor.exe data-all.root data-all_ttbar_pu46-reference.root HLT_j55_bperf_InDetTrigTrackingxAODCnv_Bjet_FTF_forID HLT_j55_bperf_InDetTrigTrackingxAODCnv_Bjet_IDTrig -d HLTEF-plots-bjet TIDArun.sh data-all.root data-all_ttbar_pu46-reference.root HLT_e24_medium_idperf_InDetTrigTrackingxAODCnv_Electron_FTF HLT_e24_medium_idperf_InDetTrigTrackingxAODCnv_Electron_IDTrig -d HLTEF-test-plots-electron TIDArun.sh data-all.root data-all_ttbar_pu46-reference.root HLT_e24_medium_idperf_InDetTrigTrackingxAODCnv_Electron_FTF -d HLTL2-test-plots-electron TIDArun.sh data-all.root data-all_ttbar_pu46-reference.root HLT_e5_loose_idperf_InDetTrigTrackingxAODCnv_Electron_FTF HLT_e5_loose_idperf_InDetTrigTrackingxAODCnv_Electron_IDTrig -d HLTEF-test-plots-electron-lowpt TIDArun.sh data-all.root data-all_ttbar_pu46-reference.root HLT_e5_loose_idperf_InDetTrigTrackingxAODCnv_Electron_FTF -d HLTL2-test-plots-electron-lowpt TIDArun.sh data-all.root data-all_ttbar_pu46-reference.root HLT_mu24_idperf_InDetTrigTrackingxAODCnv_Muon_FTF HLT_mu24_idperf_InDetTrigTrackingxAODCnv_Muon_IDTrig -d HLTEF-test-plots-muon TIDArun.sh data-all.root data-all_ttbar_pu46-reference.root HLT_mu24_idperf_InDetTrigTrackingxAODCnv_Muon_FTF -d HLTL2-test-plots-muon TIDArun.sh data-all.root data-all_ttbar_pu46-reference.root HLT_tau25_idperf_track_InDetTrigTrackingxAODCnv_Tau_IDTrig HLT_tau25_idperf_tracktwo_InDetTrigTrackingxAODCnv_Tau_IDTrig_forID3 HLT_tau20_r1_idperf_InDetTrigTrackingxAODCnv_Tau_EFID -d HLTEF-test-plots-tau TIDArun.sh data-all.root data-all_ttbar_pu46-reference.root HLT_tau25_idperf_track_InDetTrigTrackingxAODCnv_Tau_FTF HLT_tau25_idperf_tracktwo_InDetTrigTrackingxAODCnv_TauCore_FTF_forID1 HLT_tau25_idperf_tracktwo_InDetTrigTrackingxAODCnv_TauIso_FTF_forID3 HLT_tau20_r1_idperf_TrigL2SiTrackFinder_Tau_2 -d HLTL2-test-plots-tau TIDArun.sh data-all.root data-all_ttbar_pu46-reference.root HLT_j55_bperf_InDetTrigTrackingxAODCnv_Bjet_FTF_forID HLT_j55_bperf_InDetTrigTrackingxAODCnv_Bjet_IDTrig -d HLTEF-test-plots-bjet

get_files -data expert-monitoring-all_ttbar_pu46-ref.root
TIDAcpucost.exe expert-monitoring.root expert-monitoring-all_ttbar_pu46-ref.root --auto -o times

RunTrigCostD3PD.exe -f trig_cost.root --outputTagFromAthena --monitorAllChainSeqAlg --monitorROI --linkOutputDir --monitorGlobals

TIDAcpucost.exe costMon/TrigCostRoot_Results.root costMon/TrigCostRoot_Results.root -o cost-perCall --auto -d "/Algorithm" -p "_Time_perCall"

TIDAcpucost.exe costMon/TrigCostRoot_Results.root costMon/TrigCostRoot_Results.root -o cost-perEvent --auto -d "/Algorithm" -p "_Time_perEvent"

TIDAcpucost.exe costMon/TrigCostRoot_Results.root costMon/TrigCostRoot_Results.root -o cost-perCall-chain --auto -d "/Chain_Algorithm" -p "_Time_perCall"

TIDAcpucost.exe costMon/TrigCostRoot_Results.root costMon/TrigCostRoot_Results.root -o cost-perEvent-chain --auto -d "/Chain_Algorithm" -p "_Time_perEvent" check_log.pl --conf checklogTriggerTest.conf %JOBLOG


