#!/bin/sh

get_files -jo TrigInDetValidation_RTT_topOptions_MuonSlice.py
athena.py  -c 'XMLDataSet="TrigInDetValidation_FTK_mu_tautau";EventMax=5000;runMergedChain=True;doFTK=True' TrigInDetValidation_RTT_topOptions_MuonSlice.py 


get_files -data TIDAdata11-rtt.dat
get_files -data Test_bin.dat
TIDArdict.exe TIDAdata11-rtt.dat -f data-muon-FTK.root -p 13 -b Test_bin.dat

get_files -data data-FTK_mu_tautau-reference.root
TIDAcomparitor.exe data-muon-FTK.root data-FTK_mu_tautau-reference.root HLT_mu6_idperf_InDetTrigTrackingxAODCnv_Muon_FTF HLT_mu24_idperf_InDetTrigTrackingxAODCnv_Muon_FTF HLT_mu6_FTK_idperf_InDetTrigTrackingxAODCnv_Muon_FTK HLT_mu24_FTK_idperf_InDetTrigTrackingxAODCnv_Muon_FTK -d HLTL2-plots

get_files -data data-FTK_mu_tautau-reference.root
TIDAcomparitor.exe data-muon-FTK.root data-FTK_mu_tautau-reference.root HLT_mu6_idperf_InDetTrigTrackingxAODCnv_Muon_IDTrig HLT_mu24_idperf_InDetTrigTrackingxAODCnv_Muon_IDTrig HLT_mu6_FTLJ_idperf_InDetTrigTrackingxAODCnv_Muon_IDTrig HLT_mu24_FTK_idperf_InDetTrigTrackingxAODCnv_Muon_IDTrig -d HLTEF-plots

get_files -data expert-monitoring-FTK_mu_tautau-ref.root
TIDAcpucost.exe expert-monitoring.root expert-monitoring-FTK_mu_tautau-ref.root --auto -o times

RunTrigCostD3PD.exe -f trig_cost.root --outputTagFromAthena --monitorAllChainSeqAlg --monitorROI --linkOutputDir

TIDAcpucost.exe costMon/TrigCostRoot_Results.root costMon/TrigCostRoot_Results.root -o cost-perCall --auto -d "/Algorithm" -p "_Time_perCall"

TIDAcpucost.exe costMon/TrigCostRoot_Results.root costMon/TrigCostRoot_Results.root -o cost-perEvent --auto -d "/Algorithm" -p "_Time_perEvent"

TIDAcpucost.exe costMon/TrigCostRoot_Results.root costMon/TrigCostRoot_Results.root -o cost-perCall-chain --auto -d "/Chain_Algorithm" -p "_Time_perCall"

TIDAcpucost.exe costMon/TrigCostRoot_Results.root costMon/TrigCostRoot_Results.root -o cost-perEvent-chain --auto -d "/Chain_Algorithm" -p "_Time_perEvent" check_log.pl --conf checklogTriggerTest.conf %JOBLOG


