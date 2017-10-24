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
# art-input:  mc14_cos.108865.CosSimPixVolSolOnTorOn.recon.RDO.s2111_r6258_tid05313290_00
# art-output: *.dat 
# art-output: *.root
# art-output: *.log

export RTTJOBNAME=TrigInDetValidation_cosmic

get_files -jo             TrigInDetValidation/TrigInDetValidation_RTT_topOptions_CosmicSlice.py
athena.py  -c 'ARTConfig=["/eos/atlas/atlascerngroupdisk/proj-sit/trigindet/mc14_cos.108865.CosSimPixVolSolOnTorOn.recon.RDO.s2111_r6258_tid05313290_00/RDO.05313290._000892.pool.root.1"];	    EventMax=5000;'             TrigInDetValidation/TrigInDetValidation_RTT_topOptions_CosmicSlice.py
get_files -data TIDAcosmic-rtt.dat
get_files -data TIDAbeam.dat
get_files -data Test_bin.dat
TIDArdict.exe TIDAcosmic-rtt.dat -f data-cosmic.root -b Test_bin.dat

TIDArun-art.sh data-cosmic.root data-cosmic-reference.root HLT_id_cosmic_InDetTrigTrackingxAODCnvIOTRT_CosmicsN_EFID  -d HLTEF-plots

TIDArun-art.sh expert-monitoring.root  expert-monitoring*-ref.root --auto -o times

TIDArun-art.sh expert-monitoring.root  expert-monitoring*-ref.root --auto -p FastTrack -o times-FTF

RunTrigCostD3PD.exe -f trig_cost.root  --outputTagFromAthena --costMode --linkOutputDir

TIDAcpucost.exe costMon/TrigCostRoot_Results.root costMon/TrigCostRoot_Results.root -o cost-perCall --auto -d "/Algorithm" -p "_Time_perCall"

TIDAcpucost.exe costMon/TrigCostRoot_Results.root costMon/TrigCostRoot_Results.root -o cost-perEvent --auto -d "/Algorithm" -p "_Time_perEvent"  costMon/TrigCostRoot_Results.root costMon/TrigCostRoot_Results.root -o cost-perCall-chain --auto -d "/Chain_Algorithm" -p "_Time_perCall"

TIDAcpucost.exe costMon/TrigCostRoot_Results.root costMon/TrigCostRoot_Results.root -o cost-perEvent-chain --auto -d "/Chain_Algorithm" -p "_Time_perEvent"

