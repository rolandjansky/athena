#!/bin/bash
# art-description: art job for cosmic
# art-type: grid
# art-output: HLTL2-vtx
# art-output: times-FTF
# art-output: cost-perCall
# art-output: cost-perEvent
# art-output: cost-perEvent-chain
# art-input:  mc14_cos.108865.CosSimPixVolSolOnTorOn.recon.RDO.s2111_r6258_tid05313290_00
# art-output: *.dat 
# art-output: *.root
# art-output: *.log
# art-input-nfiles: 25



export RTTJOBNAME=TrigInDetValidation_cosmic


fileList="['${ArtInFile//,/', '}']"
echo "List of files = $fileList"

get_files -jo             TrigInDetValidation/TrigInDetValidation_RTT_topOptions_CosmicSlice.py
athena.py  -c 'ARTConfig=$fileList;EventMax=10000;'             TrigInDetValidation/TrigInDetValidation_RTT_topOptions_CosmicSlice.py
echo "art-result: $? athena_0"

get_files -data TIDAcosmic-rtt.dat
get_files -data TIDAbeam.dat
get_files -data Test_bin.dat
TIDArdict.exe TIDAcosmic-rtt.dat -f data-cosmic.root -b Test_bin.dat
echo "art-result: $? TIDArdict_1"

TIDArun-art.sh data-bjet-pileup.root data-bjet_IBL_pu40_offline-reference.root  HLT_j55_boffperf_split_InDetTrigTrackingxAODCnv_BjetPrmVtx_FTF_SuperRoi_xPrimVx/xPrimVx HLT_j55_boffperf_split_InDetTrigTrackingxAODCnv_BjetPrmVtx_FTF_SuperRoi_EFHistoVtx/EFHistoVtx -c TIDAvertex.dat -d HLTL2-vtx
echo "art-result: $? TIDArun_2"

TIDArun.sh 
echo "art-result: $? TIDArun_3"

TIDArun-art.sh expert-monitoring.root  expert-monitoring*-ref.root --auto -p FastTrack -o times-FTF
echo "art-result: $? TIDArun_4"

RunTrigCostD3PD.exe -f trig_cost.root  --outputTagFromAthena --costMode --linkOutputDir
echo "art-result: $? RunTrigCostD3PD_5"

TIDAcpucost.exe costMon/TrigCostRoot_Results.root costMon/TrigCostRoot_Results.root -o cost-perCall --auto -d "/Algorithm" -p "_Time_perCall"
echo "art-result: $? TIDAcpucost_6"

TIDAcpucost.exe costMon/TrigCostRoot_Results.root costMon/TrigCostRoot_Results.root -o cost-perEvent --auto -d "/Algorithm" -p "_Time_perEvent"
echo "art-result: $? TIDAcpucost_7"

TIDAcpucost.exe 
echo "art-result: $? TIDAcpucost_8"

TIDAcpucost.exe costMon/TrigCostRoot_Results.root costMon/TrigCostRoot_Results.root -o cost-perEvent-chain --auto -d "/Chain_Algorithm" -p "_Time_perEvent"
echo "art-result: $? TIDAcpucost_9"

