#!/bin/bash
# art-description: art job for cosmic
# art-type: grid
# art-output: HLTEF-plots
# art-output: times
# art-output: times-FTF
# art-output: cost-perCall
# art-output: cost-perEvent
# art-output: cost-perCall-chain
# art-output: cost-perEvent-chain
# art-input:  mc14_cos.108865.CosSimPixVolSolOnTorOn.recon.RDO.s2111_r6258
# art-output: *.dat 
# art-output: *.root
# art-output: *.log
# art-input-nfiles: 10


export RTTJOBNAME=TrigInDetValidation_cosmic


fileList="['${ArtInFile//,/', '}']"
echo "List of files = $fileList"

get_files -jo             TrigInDetValidation/TrigInDetValidation_RTT_topOptions_CosmicSlice.py
athena.py  -c "ARTConfig=$fileList;EventMax=10000;"             TrigInDetValidation/TrigInDetValidation_RTT_topOptions_CosmicSlice.py
echo "art-result: $? athena_0"

get_files -data TIDAcosmic-rtt.dat
get_files -data TIDAbeam.dat
get_files -data Test_bin.dat
TIDArdict.exe TIDAcosmic-rtt.dat -f data-cosmic.root -b Test_bin.dat
echo "art-result: $? TIDArdict_1"

TIDArun-art.sh data-cosmic.root data-cosmic-reference.root HLT_id_cosmic_InDetTrigTrackingxAODCnvIOTRT_CosmicsN_EFID  -d HLTEF-plots
echo "art-result: $? TIDArun_2"

TIDArun-art.sh expert-monitoring.root  expert-monitoring*-ref.root --auto -o times
echo "art-result: $? TIDArun_3"

TIDArun-art.sh expert-monitoring.root  expert-monitoring*-ref.root --auto -p FastTrack -o times-FTF
echo "art-result: $? TIDArun_4"

RunTrigCostD3PD.exe -f trig_cost.root  --outputTagFromAthena --costMode --linkOutputDir
echo "art-result: $? RunTrigCostD3PD_5"

TIDAcpucost.exe costMon/TrigCostRoot_Results.root costMon/TrigCostRoot_Results.root -o cost-perCall --auto -d "/Algorithm" -p "_Time_perCall"
echo "art-result: $? TIDAcpucost_6"

TIDAcpucost.exe costMon/TrigCostRoot_Results.root costMon/TrigCostRoot_Results.root -o cost-perEvent --auto -d "/Algorithm" -p "_Time_perEvent"
echo "art-result: $? TIDAcpucost_7"

TIDAcpucost.exe costMon/TrigCostRoot_Results.root costMon/TrigCostRoot_Results.root -o cost-perCall-chain --auto -d "/Chain_Algorithm" -p "_Time_perCall"
echo "art-result: $? TIDAcpucost_8"

TIDAcpucost.exe costMon/TrigCostRoot_Results.root costMon/TrigCostRoot_Results.root -o cost-perEvent-chain --auto -d "/Chain_Algorithm" -p "_Time_perEvent"
echo "art-result: $? TIDAcpucost_9"

