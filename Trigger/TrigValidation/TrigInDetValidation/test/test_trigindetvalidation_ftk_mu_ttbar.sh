#!/bin/bash
# art-description: art job for FTK_mu_ttbar
# art-type: grid
# art-output: HLTL2-plots
# art-output: HLTEF-plots
# art-output: HLTL2-plots-lowpt
# art-output: HLTEF-plots-lowpt
# art-output: times
# art-output: times-FTF
# art-output: cost-perCall
# art-output: cost-perEvent
# art-output: cost-perCall-chain
# art-output: cost-perEvent-chain
# art-input:  valid1.410000.PowhegPythiaEvtGen_P2012_ttbar_hdamp172p5_nonallhad.digit.RDO_FTK.e4993_s2887_r8937_r9119
# art-output: *.dat 
# art-output: *.root
# art-output: *.log
# art-input-nfiles: 25



export RTTJOBNAME=TrigInDetValidation_FTK_mu_ttbar


fileList="['${ArtInFile//,/', '}']"
echo "List of files = $fileList"

get_files -jo             TrigInDetValidation/TrigInDetValidation_RTT_topOptions_MuonSlice.py
athena.py  -c 'ARTConfig=$fileList;EventMax=10000;doFTK=True'             TrigInDetValidation/TrigInDetValidation_RTT_topOptions_MuonSlice.py
echo "art-result: $? athena_0"

get_files -data TIDAdata11-rtt.dat
get_files -data TIDAdata_cuts.dat
get_files -data TIDAdata_chains.dat
get_files -data TIDAbeam.dat
get_files -data Test_bin.dat
TIDArdict.exe TIDAdata11-rtt.dat -f data-muon-FTK.root -p 13  -b Test_bin.dat
echo "art-result: $? TIDArdict_1"

TIDArun-art.sh data-muon-FTK.root data-FTK_mu_ttbar-reference.root  HLT_mu24_idperf_InDetTrigTrackingxAODCnv_Muon_FTF  HLT_mu24_FTK_idperf_InDetTrigTrackingxAODCnv_Muon_FTK  HLT_mu24_FTKRefit_idperf_InDetTrigTrackingxAODCnv_Muon_FTKRefit  -d HLTL2-plots
echo "art-result: $? TIDArun_2"

TIDArun-art.sh data-muon-FTK.root data-FTK_mu_ttbar-reference.root  HLT_mu24_idperf_InDetTrigTrackingxAODCnv_Muon_IDTrig  HLT_mu24_FTK_idperf_InDetTrigTrackingxAODCnv_Muon_FTK_IDTrig HLT_mu24_FTKRefit_idperf_InDetTrigTrackingxAODCnv_Muon_FTKRefit_IDTrig   -d HLTEF-plots
echo "art-result: $? TIDArun_3"

TIDArun-art.sh data-muon-FTK.root data-FTK_mu_ttbar-reference.root HLT_mu6_idperf_InDetTrigTrackingxAODCnv_Muon_FTF  HLT_mu6_FTK_idperf_InDetTrigTrackingxAODCnv_Muon_FTK HLT_mu6_FTKRefit_idperf_InDetTrigTrackingxAODCnv_Muon_FTKRefit   -d HLTL2-plots-lowpt
echo "art-result: $? TIDArun_4"

TIDArun-art.sh data-muon-FTK.root data-FTK_mu_ttbar-reference.root HLT_mu6_idperf_InDetTrigTrackingxAODCnv_Muon_IDTrig  HLT_mu6_FTK_idperf_InDetTrigTrackingxAODCnv_Muon_FTK_IDTrig HLT_mu6_FTKRefit_idperf_InDetTrigTrackingxAODCnv_Muon_FTKRefit_IDTrig    -d HLTEF-plots-lowpt
echo "art-result: $? TIDArun_5"

TIDArun-art.sh expert-monitoring.root  expert-monitoring*-ref.root --auto -o times
echo "art-result: $? TIDArun_6"

TIDArun-art.sh expert-monitoring.root  expert-monitoring*-ref.root --auto -p FastTrack -o times-FTF
echo "art-result: $? TIDArun_7"

RunTrigCostD3PD.exe -f trig_cost.root  --outputTagFromAthena --costMode --linkOutputDir
echo "art-result: $? RunTrigCostD3PD_8"

TIDAcpucost.exe costMon/TrigCostRoot_Results.root costMon/TrigCostRoot_Results.root -o cost-perCall --auto -d "/Algorithm" -p "_Time_perCall"
echo "art-result: $? TIDAcpucost_9"

TIDAcpucost.exe costMon/TrigCostRoot_Results.root costMon/TrigCostRoot_Results.root -o cost-perEvent --auto -d "/Algorithm" -p "_Time_perEvent"
echo "art-result: $? TIDAcpucost_10"

TIDAcpucost.exe costMon/TrigCostRoot_Results.root costMon/TrigCostRoot_Results.root -o cost-perCall-chain --auto -d "/Chain_Algorithm" -p "_Time_perCall"
echo "art-result: $? TIDAcpucost_11"

TIDAcpucost.exe costMon/TrigCostRoot_Results.root costMon/TrigCostRoot_Results.root -o cost-perEvent-chain --auto -d "/Chain_Algorithm" -p "_Time_perEvent"
echo "art-result: $? TIDAcpucost_12"

