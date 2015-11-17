#!/bin/sh

get_files -jo TrigInDetValidation_RTT_topOptions_BjetSlice.py
athena.py  -c 'XMLDataSet="TrigInDetValidation_bjet_pu46";EventMax=2000;runMergedChain=True' TrigInDetValidation_RTT_topOptions_BjetSlice.py 


get_files -data TIDAdata11-rtt.dat
get_files -data Test_bin.dat
TIDArdict.exe TIDAdata11-rtt.dat -f data-bjet-pileup-merging.root -b Test_bin.dat

get_files -data data-bjet_pu46-reference.root
TIDAcomparitor.exe data-bjet-pileup-merging.root data-bjet_pu46-reference.root HLT_j55_bperf_InDetTrigTrackingxAODCnv_Bjet_IDTrig HLT_j55_bperf_split_InDetTrigTrackingxAODCnv_Bjet_IDTrig_forID HLT_j55_bperf_split_InDetTrigTrackingxAODCnv_Bjet_FTF_forID -d HLTEF-plots

get_files -data data-bjet_pu46-reference.root
TIDAcomparitor.exe data-bjet-pileup-merging.root data-bjet_pu46-reference.root HLT_j55_bperf_split:TrigFastTrackFinder_Jet_forID HLT_j55_bperf_split_InDetTrigTrackingxAODCnv_Bjet_FTF_forID HLT_j55_bperf_InDetTrigTrackingxAODCnv_Bjet_FTF_forID HLT_j55_bperf_split_InDetTrigTrackingxAODCnv_BjetPrmVtx_FTF_SuperRoi -d HLTL2-plots

get_files -data expert-monitoring-bjet_pu46-ref.root
TIDAcpucost.exe expert-monitoring.root expert-monitoring-bjet_pu46-ref.root --auto -o times

RunTrigCostD3PD.exe -f trig_cost.root --outputTagFromAthena --monitorAllChainSeqAlg --monitorROI --linkOutputDir

TIDAcpucost.exe costMon/TrigCostRoot_Results.root costMon/TrigCostRoot_Results.root -o cost-perCall --auto -d "/Algorithm" -p "_Time_perCall"

TIDAcpucost.exe costMon/TrigCostRoot_Results.root costMon/TrigCostRoot_Results.root -o cost-perEvent --auto -d "/Algorithm" -p "_Time_perEvent"

TIDAcpucost.exe costMon/TrigCostRoot_Results.root costMon/TrigCostRoot_Results.root -o cost-perCall-chain --auto -d "/Chain_Algorithm" -p "_Time_perCall"

TIDAcpucost.exe costMon/TrigCostRoot_Results.root costMon/TrigCostRoot_Results.root -o cost-perEvent-chain --auto -d "/Chain_Algorithm" -p "_Time_perEvent" check_log.pl --conf checklogTriggerTest.conf %JOBLOG


