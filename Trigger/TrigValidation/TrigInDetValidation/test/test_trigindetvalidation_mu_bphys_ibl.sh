#!/bin/bash
# art-description: art job for mu_bphys_IBL
# art-type: grid
# art-output: HLTEF-plots-lowpt
# art-output: times
# art-output: times-FTF
# art-output: cost-perCall
# art-output: cost-perEvent
# art-output: cost-perCall-chain
# art-output: cost-perEvent-chain
# art-input:  mc15_13TeV.300401.Pythia8BPhotospp_A14_CTEQ6L1_Bs_Jpsimu3p5mu3p5_phi.recon.RDO.e4397_s2608_r6869
# art-output: *.dat 
# art-output: *.root
# art-output: *.log
# art-input-nfiles: 25



export RTTJOBNAME=TrigInDetValidation_mu_bphys_IBL


fileList="['${ArtInFile//,/', '}']"
echo "List of files = $fileList"

get_files -jo             TrigInDetValidation/TrigInDetValidation_RTT_topOptions_MuonSlice.py
athena.py  -c 'ARTConfig=$fileList;EventMax=10000'             TrigInDetValidation/TrigInDetValidation_RTT_topOptions_MuonSlice.py
echo "art-result: $? athena_0"

 
echo "art-result: $? _1"

TIDArdict.exe
echo "art-result: $? TIDArdict_2"

get_files -data TIDAdata11-rtt.dat
get_files -data TIDAdata_cuts.dat
get_files -data TIDAdata_chains.dat
get_files -data TIDAbeam.dat
get_files -data Test_bin.dat
TIDArdict.exe TIDAdata11-rtt.dat -f data-monitor.root -b Test_bin.dat
echo "art-result: $? TIDArdict_3"

TIDArun-art.sh data-muon-FTK.root data-FTK_mu_ttbar-reference.root HLT_mu6_idperf_InDetTrigTrackingxAODCnv_Muon_IDTrig  HLT_mu6_FTK_idperf_InDetTrigTrackingxAODCnv_Muon_FTK_IDTrig HLT_mu6_FTKRefit_idperf_InDetTrigTrackingxAODCnv_Muon_FTKRefit_IDTrig    -d HLTEF-plots-lowpt
echo "art-result: $? TIDArun_4"

TIDArun-art.sh expert-monitoring.root  expert-monitoring*-ref.root --auto -o times
echo "art-result: $? TIDArun_5"

TIDArun-art.sh expert-monitoring.root  expert-monitoring*-ref.root --auto -p FastTrack -o times-FTF
echo "art-result: $? TIDArun_6"

RunTrigCostD3PD.exe -f trig_cost.root  --outputTagFromAthena --costMode --linkOutputDir
echo "art-result: $? RunTrigCostD3PD_7"

TIDAcpucost.exe costMon/TrigCostRoot_Results.root costMon/TrigCostRoot_Results.root -o cost-perCall --auto -d "/Algorithm" -p "_Time_perCall"
echo "art-result: $? TIDAcpucost_8"

TIDAcpucost.exe costMon/TrigCostRoot_Results.root costMon/TrigCostRoot_Results.root -o cost-perEvent --auto -d "/Algorithm" -p "_Time_perEvent"
echo "art-result: $? TIDAcpucost_9"

TIDAcpucost.exe costMon/TrigCostRoot_Results.root costMon/TrigCostRoot_Results.root -o cost-perCall-chain --auto -d "/Chain_Algorithm" -p "_Time_perCall"
echo "art-result: $? TIDAcpucost_10"

TIDAcpucost.exe costMon/TrigCostRoot_Results.root costMon/TrigCostRoot_Results.root -o cost-perEvent-chain --auto -d "/Chain_Algorithm" -p "_Time_perEvent"
echo "art-result: $? TIDAcpucost_11"

