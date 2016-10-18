#!/bin/sh

get_files -jo TrigInDetValidation/TrigInDetValidation_RTT_topOptions_BjetSlice.py
athena.py  -c 'XMLDataSet="TrigInDetValidation_bjet_IBL_pu40_offline";EventMax=900;doIDNewTracking=True;runMergedChain=True;globalTag="OFLCOND-RUN12-SDR-17"' TrigInDetValidation/TrigInDetValidation_RTT_topOptions_BjetSlice.py 


get_files -data TIDAdata11-rtt-offline.dat
get_files -data Test_bin.dat
TIDArdict.exe TIDAdata11-rtt-offline.dat -f data-bjet-pileup.root -b Test_bin.dat

TIDAcomparitor.exe data-bjet-pileup.root data-bjet-pileup.root HLT_j55_bperf_InDetTrigTrackingxAODCnv_Bjet_IDTrig HLT_j55_bperf_split_InDetTrigTrackingxAODCnv_Bjet_IDTrig_forID HLT_j55_bperf_split_InDetTrigTrackingxAODCnv_Bjet_FTF_forID -d HLTEF-plots

TIDAcomparitor.exe data-bjet-pileup.root data-bjet-pileup.root HLT_j55_bperf_split:TrigFastTrackFinder_Jet_forID HLT_j55_bperf_split_InDetTrigTrackingxAODCnv_Bjet_FTF_forID HLT_j55_bperf_InDetTrigTrackingxAODCnv_Bjet_FTF_forID HLT_j55_bperf_split_InDetTrigTrackingxAODCnv_BjetPrmVtx_FTF_SuperRoi -d HLTL2-plots

get_files -data TIDAvertex.dat
TIDAcomparitor.exe data-bjet-pileup.root data-bjet-pileup.root HLT_j55_bperf_split_InDetTrigTrackingxAODCnv_BjetPrmVtx_FTF_SuperRoi_xPrimVx/xPrimVx HLT_j55_bperf_split_InDetTrigTrackingxAODCnv_BjetPrmVtx_FTF_SuperRoi_EFHistoVtx/EFHistoVtx -c TIDAvertex.dat -d HLTL2-vtx

get_files -data expert-monitoring-bjet_IBL_pu40-ref.root
TIDAcpucost.exe expert-monitoring.root expert-monitoring-bjet_IBL_pu40-ref.root --auto -o times

RunTrigCostD3PD.exe -f trig_cost.root --outputTagFromAthena --monitorAllChainSeqAlg --monitorROI --linkOutputDir --monitorGlobals

TIDAcpucost.exe costMon/TrigCostRoot_Results.root costMon/TrigCostRoot_Results.root -o cost-perCall --auto -d "/Algorithm" -p "_Time_perCall"

TIDAcpucost.exe costMon/TrigCostRoot_Results.root costMon/TrigCostRoot_Results.root -o cost-perEvent --auto -d "/Algorithm" -p "_Time_perEvent"

TIDAcpucost.exe costMon/TrigCostRoot_Results.root costMon/TrigCostRoot_Results.root -o cost-perCall-chain --auto -d "/Chain_Algorithm" -p "_Time_perCall"

TIDAcpucost.exe costMon/TrigCostRoot_Results.root costMon/TrigCostRoot_Results.root -o cost-perEvent-chain --auto -d "/Chain_Algorithm" -p "_Time_perEvent" check_log.pl --conf checklogTriggerTest.conf %JOBLOG


