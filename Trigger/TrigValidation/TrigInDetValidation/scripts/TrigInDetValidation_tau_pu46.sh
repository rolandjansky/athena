#!/bin/sh

get_files -jo TrigInDetValidation_RTT_topOptions_TauSlice.py
athena.py  -c 'XMLDataSet="TrigInDetValidation_tau_pu46";EventMax=4000;runMergedChain=True' TrigInDetValidation_RTT_topOptions_TauSlice.py 


get_files -data TIDAdata11-rtt.dat
get_files -data Test_bin.dat
TIDArdict.exe TIDAdata11-rtt.dat -f data-tau-merge.root -b Test_bin.dat

get_files -data data-tau_pu46-reference.root
TIDAcomparitor.exe data-tau-merge.root data-tau_pu46-reference.root HLT_tau25_idperf_track:InDetTrigTrackingxAODCnv_Tau_FTF HLT_tau25_idperf_track_InDetTrigTrackingxAODCnv_Tau_IDTrig HLT_tau25_idperf_tracktwo_InDetTrigTrackingxAODCnv_TauIso_FTF_forID3 HLT_tau25_idperf_tracktwo_InDetTrigTrackingxAODCnv_Tau_IDTrig_forID3 -d HLTEF-plots

get_files -data data-tau_pu46-reference.root
TIDAcomparitor.exe data-tau-merge.root data-tau_pu46-reference.root HLT_tau25_idperf_track_InDetTrigTrackingxAODCnv_Tau_FTF HLT_tau25_idperf_tracktwo_InDetTrigTrackingxAODCnv_TauCore_FTF_forID1 HLT_tau25_idperf_tracktwo_InDetTrigTrackingxAODCnv_TauIso_FTF_forID3 -d HLTL2-plots

get_files -data data-tau_pu46-reference.root
TIDAcomparitor.exe data-tau-merge.root data-tau_pu46-reference.root HLT_tau29_idperf_InDetTrigTrackingxAODCnv_Tau_EFID HLT_tau20_r1_idperf_InDetTrigTrackingxAODCnv_Tau_EFID --tag EFID -d HLTEF-plots

get_files -data expert-monitoring-tau_pu46-ref.root
TIDAcpucost.exe expert-monitoring.root expert-monitoring-tau_pu46-ref.root --auto -o times

RunTrigCostD3PD.exe -f trig_cost.root --outputTagFromAthena --monitorAllChainSeqAlg --monitorROI --linkOutputDir

TIDAcpucost.exe costMon/TrigCostRoot_Results.root costMon/TrigCostRoot_Results.root -o cost-perCall --auto -d "/Algorithm" -p "_Time_perCall"

TIDAcpucost.exe costMon/TrigCostRoot_Results.root costMon/TrigCostRoot_Results.root -o cost-perEvent --auto -d "/Algorithm" -p "_Time_perEvent"

TIDAcpucost.exe costMon/TrigCostRoot_Results.root costMon/TrigCostRoot_Results.root -o cost-perCall-chain --auto -d "/Chain_Algorithm" -p "_Time_perCall"

TIDAcpucost.exe costMon/TrigCostRoot_Results.root costMon/TrigCostRoot_Results.root -o cost-perEvent-chain --auto -d "/Chain_Algorithm" -p "_Time_perEvent" check_log.pl --conf checklogTriggerTest.conf %JOBLOG


