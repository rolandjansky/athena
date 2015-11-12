#!/bin/sh

get_files -jo TrigInDetValidation_RTT_topOptions_CosmicSlice.py
athena.py  -c 'XMLDataSet="TrigInDetValidation_cosmic";EventMax=5000;' TrigInDetValidation_RTT_topOptions_CosmicSlice.py 


get_files -data TIDAcosmic-rtt.dat
get_files -data Test_bin.dat
TIDArdict.exe TIDAcosmic-rtt.dat -f data-cosmic.root -b Test_bin.dat

get_files -data data-cosmic-reference.root
TIDAcomparitor.exe data-cosmic.root data-cosmic-reference.root HLT_id_cosmic_InDetTrigTrackingxAODCnvIOTRT_CosmicsN_EFID -d HLTEF-plots

get_files -data expert-monitoring-cosmic-ref.root
TIDAcpucost.exe expert-monitoring.root expert-monitoring-cosmic-ref.root --auto -o times

RunTrigCostD3PD.exe -f trig_cost.root --outputTagFromAthena --monitorAllChainSeqAlg --monitorROI --linkOutputDir

TIDAcpucost.exe costMon/TrigCostRoot_Results.root costMon/TrigCostRoot_Results.root -o cost-perCall --auto -d "/Algorithm" -p "_Time_perCall"

TIDAcpucost.exe costMon/TrigCostRoot_Results.root costMon/TrigCostRoot_Results.root -o cost-perEvent --auto -d "/Algorithm" -p "_Time_perEvent"

TIDAcpucost.exe costMon/TrigCostRoot_Results.root costMon/TrigCostRoot_Results.root -o cost-perCall-chain --auto -d "/Chain_Algorithm" -p "_Time_perCall"

TIDAcpucost.exe costMon/TrigCostRoot_Results.root costMon/TrigCostRoot_Results.root -o cost-perEvent-chain --auto -d "/Chain_Algorithm" -p "_Time_perEvent" check_log.pl --conf checklogTriggerTest.conf %JOBLOG


