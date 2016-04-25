#!/bin/sh

get_files -jo TrigInDetValidation_RTT_topOptions_MonitorSlice.py
athena.py  -c 'XMLDataSet="TrigInDetValidation_minBias_mergeDataPrep";EventMax=1000;doMergedDataPrep=True' TrigInDetValidation_RTT_topOptions_MonitorSlice.py 


get_files -data TIDAdata11-rtt.dat
get_files -data Test_bin.dat
TIDArdict.exe TIDAdata11-rtt.dat -f data-monitor-merge.root -b Test_bin.dat

get_files -data data-minBias_mergeDataPrep-reference.root
TIDAcomparitor.exe data-monitor-merge.root data-minBias_mergeDataPrep-reference.root EF_InDetMonMergeDP_FS_InDetTrigParticleCreation_FullScan_EFID -d EF-plots

get_files -data data-minBias_mergeDataPrep-reference.root
TIDAcomparitor.exe data-monitor-merge.root data-minBias_mergeDataPrep-reference.root L2_InDetMonMergeDP_FS_TrigL2SiTrackFinder_FullScan_0 L2_InDetMonMergeDP_FS_TrigL2SiTrackFinder_FullScan_1 L2_InDetMonMergeDP_FS_TrigL2SiTrackFinder_FullScan_2 L2_InDetMonMergeDP_FS_TrigL2SiTrackFinder_FullScan_3 -d L2-plots

get_files -data expert-monitoring-minBias_mergeDataPrep-ref.root
TIDAcpucost.exe expert-monitoring.root expert-monitoring-minBias_mergeDataPrep-ref.root --auto -o times

RunTrigCostD3PD.exe -f trig_cost.root --outputTagFromAthena --monitorAllChainSeqAlg --monitorROI --linkOutputDir

TIDAcpucost.exe costMon/TrigCostRoot_Results.root costMon/TrigCostRoot_Results.root -o cost-perCall --auto -d "/Algorithm" -p "_Time_perCall"

TIDAcpucost.exe costMon/TrigCostRoot_Results.root costMon/TrigCostRoot_Results.root -o cost-perEvent --auto -d "/Algorithm" -p "_Time_perEvent"

TIDAcpucost.exe costMon/TrigCostRoot_Results.root costMon/TrigCostRoot_Results.root -o cost-perCall-chain --auto -d "/Chain_Algorithm" -p "_Time_perCall"

TIDAcpucost.exe costMon/TrigCostRoot_Results.root costMon/TrigCostRoot_Results.root -o cost-perEvent-chain --auto -d "/Chain_Algorithm" -p "_Time_perEvent" check_log.pl --conf checklogTriggerTest.conf %JOBLOG


