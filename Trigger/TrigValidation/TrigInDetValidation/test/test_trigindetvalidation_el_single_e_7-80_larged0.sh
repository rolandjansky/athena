#!/bin/bash
# art-description: art job for el_single_e_7-80_larged0
# art-type: grid
# art-output: HLTEF-plots
# art-output: HLTEF-plots-lowpt
# art-output: HLTL2-plots
# art-output: HLTL2-plots-lowpt
# art-output: times
# art-output: times-FTF
# art-output: cost-perCall
# art-output: cost-perEvent
# art-output: cost-perCall-chain
# art-output: cost-perEvent-chain
# art-input:  mc15_13TeV.159053.ParticleGenerator_e_Et7to80_vertxy20.recon.RDO.e3603_s2726_r7728
# art-output: *.dat 
# art-output: *.root
# art-output: *.log

export RTTJOBNAME=TrigInDetValidation_el_single_e_7-80_larged0

get_files -jo             TrigInDetValidation/TrigInDetValidation_RTT_topOptions_ElectronSlice.py
athena.py  -c 'ARTConfig=["/eos/atlas/atlascerngroupdisk/proj-sit/trigindet/mc15_13TeV.159053.ParticleGenerator_e_Et7to80_vertxy20.recon.RDO.e3603_s2726_r7728/RDO.10240167._000001.pool.root.1"];	    EventMax=15000;runMergedChain=True'             TrigInDetValidation/TrigInDetValidation_RTT_topOptions_ElectronSlice.py
JOBSTATUS_0=$?

get_files -data TIDAdata11-rtt-larged0-el.dat
get_files -data TIDAdata_cuts.dat
get_files -data TIDAdata_chains.dat
get_files -data TIDAbeam.dat
get_files -data Test_bin_larged0.dat
TIDArdict.exe TIDAdata11-rtt-larged0-el.dat -f data-electron-IBL.root -p 11 -b Test_bin_larged0.dat
JOBSTATUS_1=$?

TIDArun-art.sh data-electron-IBL.root data-el_single_e_7-80_larged0-reference.root HLT_e24_medium_idperf_InDetTrigTrackingxAODCnv_Electron_FTF  HLT_e24_medium_idperf_InDetTrigTrackingxAODCnv_Electron_IDTrig   -d HLTEF-plots
JOBSTATUS_2=$?

TIDArun-art.sh data-electron-IBL.root data-el_single_e_7-80_larged0-reference.root HLT_e5_loose_idperf_InDetTrigTrackingxAODCnv_Electron_FTF  HLT_e5_loose_idperf_InDetTrigTrackingxAODCnv_Electron_IDTrig  -d HLTEF-plots-lowpt
JOBSTATUS_3=$?

TIDArun-art.sh data-electron-IBL.root data-el_single_e_7-80_larged0-reference.root HLT_e24_medium_idperf_InDetTrigTrackingxAODCnv_Electron_FTF  -d HLTL2-plots
JOBSTATUS_4=$?

TIDArun-art.sh data-electron-IBL.root data-el_single_e_7-80_larged0-reference.root HLT_e5_loose_idperf_InDetTrigTrackingxAODCnv_Electron_FTF  -d HLTL2-plots-lowpt
JOBSTATUS_5=$?

TIDArun-art.sh expert-monitoring.root  expert-monitoring*-ref.root --auto -o times
JOBSTATUS_6=$?

TIDArun-art.sh expert-monitoring.root  expert-monitoring*-ref.root --auto -p FastTrack -o times-FTF
JOBSTATUS_7=$?

RunTrigCostD3PD.exe -f trig_cost.root  --outputTagFromAthena --costMode --linkOutputDir
JOBSTATUS_8=$?

TIDAcpucost.exe costMon/TrigCostRoot_Results.root costMon/TrigCostRoot_Results.root -o cost-perCall --auto -d "/Algorithm" -p "_Time_perCall"
JOBSTATUS_9=$?

TIDAcpucost.exe costMon/TrigCostRoot_Results.root costMon/TrigCostRoot_Results.root -o cost-perEvent --auto -d "/Algorithm" -p "_Time_perEvent"
JOBSTATUS_10=$?

TIDAcpucost.exe costMon/TrigCostRoot_Results.root costMon/TrigCostRoot_Results.root -o cost-perCall-chain --auto -d "/Chain_Algorithm" -p "_Time_perCall"
JOBSTATUS_11=$?

TIDAcpucost.exe costMon/TrigCostRoot_Results.root costMon/TrigCostRoot_Results.root -o cost-perEvent-chain --auto -d "/Chain_Algorithm" -p "_Time_perEvent"
JOBSTATUS_12=$?

