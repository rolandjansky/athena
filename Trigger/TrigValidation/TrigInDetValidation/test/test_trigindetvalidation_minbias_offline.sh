#!/bin/bash
# art-description: art job for minBias_offline
# art-type: grid
# art-output: HLTEF-plots
# art-output: times
# art-output: times-FTF
# art-output: cost-perCall
# art-output: cost-perEvent
# art-output: cost-perCall-chain
# art-output: cost-perEvent-chain
# art-input:  mc15_13TeV.361036.Pythia8_A2MSTW2008LO_minbias_inelastic.recon.RDO.e3580_s2726_r7011
# art-output: *.dat 
# art-output: *.root
# art-output: *.log

export RTTJOBNAME=TrigInDetValidation_minBias_offline

get_files -jo             TrigInDetValidation/TrigInDetValidation_RTT_topOptions_MonitorSlice.py
athena.py  -c 'ARTConfig=["/eos/atlas/atlascerngroupdisk/proj-sit/trigindet/mc15_13TeV.361036.Pythia8_A2MSTW2008LO_minbias_inelastic.recon.RDO.e3580_s2726_r7011/RDO.07344311._000057.pool.root.1"];	    EventMax=1000;doIDNewTracking=True'             TrigInDetValidation/TrigInDetValidation_RTT_topOptions_MonitorSlice.py
JOBSTATUS_0=$?

get_files -data TIDAdata11-rtt-offline.dat
get_files -data TIDAdata_cuts.dat
get_files -data TIDAdata_chains.dat
get_files -data TIDAbeam.dat
get_files -data Test_bin.dat
TIDArdict.exe TIDAdata11-rtt-offline.dat -f data-minBias.root -b Test_bin.dat
JOBSTATUS_1=$?

TIDArun-art.sh data-minBias.root data-minBias_offline-reference.root HLT_mb_idperf_L1MBTS_2_InDetTrigTrackingxAODCnv_minBias_EFID -d HLTEF-plots
JOBSTATUS_2=$?

TIDArun-art.sh expert-monitoring.root  expert-monitoring*-ref.root --auto -o times
JOBSTATUS_3=$?

TIDArun-art.sh expert-monitoring.root  expert-monitoring*-ref.root --auto -p FastTrack -o times-FTF
JOBSTATUS_4=$?

RunTrigCostD3PD.exe -f trig_cost.root  --outputTagFromAthena --costMode --linkOutputDir
JOBSTATUS_5=$?

TIDAcpucost.exe costMon/TrigCostRoot_Results.root costMon/TrigCostRoot_Results.root -o cost-perCall --auto -d "/Algorithm" -p "_Time_perCall"
JOBSTATUS_6=$?

TIDAcpucost.exe costMon/TrigCostRoot_Results.root costMon/TrigCostRoot_Results.root -o cost-perEvent --auto -d "/Algorithm" -p "_Time_perEvent"
JOBSTATUS_7=$?

TIDAcpucost.exe costMon/TrigCostRoot_Results.root costMon/TrigCostRoot_Results.root -o cost-perCall-chain --auto -d "/Chain_Algorithm" -p "_Time_perCall"
JOBSTATUS_8=$?

TIDAcpucost.exe costMon/TrigCostRoot_Results.root costMon/TrigCostRoot_Results.root -o cost-perEvent-chain --auto -d "/Chain_Algorithm" -p "_Time_perEvent"
JOBSTATUS_9=$?

