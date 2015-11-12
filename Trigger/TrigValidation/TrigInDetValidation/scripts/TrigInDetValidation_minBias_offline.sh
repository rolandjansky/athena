#!/bin/sh

get_files -jo TrigInDetValidation_RTT_topOptions_MonitorSlice.py
athena.py  -c 'XMLDataSet="TrigInDetValidation_minBias_offline";EventMax=1000;doIDNewTracking=True' TrigInDetValidation_RTT_topOptions_MonitorSlice.py 


get_files -data TIDAdata11-rtt-offline.dat
get_files -data Test_bin.dat
TIDArdict.exe TIDAdata11-rtt-offline.dat -f data-minBias.root -b Test_bin.dat

TIDAcomparitor.exe data-minBias.root data-minBias.root HLT_mb_idperf_L1MBTS_2_InDetTrigTrackingxAODCnv_minBias_EFID -d HLTEF-plots

get_files -data expert-monitoring-minBias-ref.root
TIDAcpucost.exe expert-monitoring.root expert-monitoring-minBias-ref.root --auto -o times

RunTrigCostD3PD.exe -f trig_cost.root --outputTagFromAthena --monitorAllChainSeqAlg --monitorROI --linkOutputDir

TIDAcpucost.exe costMon/TrigCostRoot_Results.root costMon/TrigCostRoot_Results.root -o cost-perCall --auto -d "/Algorithm" -p "_Time_perCall"

TIDAcpucost.exe costMon/TrigCostRoot_Results.root costMon/TrigCostRoot_Results.root -o cost-perEvent --auto -d "/Algorithm" -p "_Time_perEvent"

TIDAcpucost.exe costMon/TrigCostRoot_Results.root costMon/TrigCostRoot_Results.root -o cost-perCall-chain --auto -d "/Chain_Algorithm" -p "_Time_perCall"

TIDAcpucost.exe costMon/TrigCostRoot_Results.root costMon/TrigCostRoot_Results.root -o cost-perEvent-chain --auto -d "/Chain_Algorithm" -p "_Time_perEvent" check_log.pl --conf checklogTriggerTest.conf %JOBLOG


