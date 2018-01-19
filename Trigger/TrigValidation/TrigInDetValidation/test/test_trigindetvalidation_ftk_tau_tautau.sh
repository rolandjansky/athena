#!/bin/bash
# art-description: art job for FTK_tau_tautau
# art-type: grid
# art-output: HLTL2-plots-lowpt
# art-output: HLTEF-plots
# art-output: times
# art-output: times-FTF
# art-output: cost-perCall
# art-output: cost-perEvent
# art-output: cost-perCall-chain
# art-output: cost-perEvent-chain
# art-input:  user.jahreda.valid1.147408.PowhegPythia8_AZNLO_Ztautau.recon.RDO.e3099_s2578_r7611.beamspotJohn.v6_EXT2
# art-output: *.dat 
# art-output: *.root
# art-output: *.log
# art-input-nfiles: 25



export RTTJOBNAME=TrigInDetValidation_FTK_tau_tautau


fileList="['${ArtInFile//,/', '}']"
echo "List of files = $fileList"

get_files -jo             TrigInDetValidation/TrigInDetValidation_RTT_topOptions_TauSlice.py
athena.py  -c 'ARTConfig=$fileList;EventMax=10000;doFTK=True'             TrigInDetValidation/TrigInDetValidation_RTT_topOptions_TauSlice.py
echo "art-result: $? athena_0"

get_files -data TIDAdata11-rtt.dat
get_files -data TIDAdata_cuts.dat
get_files -data TIDAdata_chains.dat
get_files -data TIDAbeam.dat
get_files -data Test_bin.dat
TIDArdict.exe TIDAdata11-rtt.dat -f data-bjet-FTK.root -b Test_bin.dat
echo "art-result: $? TIDArdict_1"

get_files -data data-FTK_mu_ttbar_offline-reference.root
TIDArdict.exe data-muon-FTK.root data-FTK_mu_ttbar_offline-reference.root HLT_mu6_idperf_InDetTrigTrackingxAODCnv_Muon_FTF  HLT_mu6_FTK_idperf_InDetTrigTrackingxAODCnv_Muon_FTK HLT_mu6_FTKRefit_idperf_InDetTrigTrackingxAODCnv_Muon_FTKRefit   -d HLTL2-plots-lowpt
echo "art-result: $? TIDArdict_2"

TIDArun-art.sh data-bjet-FTK.root data-FTK_bjet_ttbar_offline-reference.root HLT_j55_boffperf_split_InDetTrigTrackingxAODCnv_Bjet_IDTrig_forID HLT_j55_boffperf_split_FTKVtx_InDetTrigTrackingxAODCnv_Bjet_IDTrig HLT_j55_boffperf_split_FTK_InDetTrigTrackingxAODCnv_Bjet_FTK_IDTrig  HLT_j55_boffperf_split_FTKRefit_InDetTrigTrackingxAODCnv_Bjet_FTKRefit_IDTrig -d HLTEF-plots
echo "art-result: $? TIDArun_3"

TIDArun-art.sh expert-monitoring.root  expert-monitoring*-ref.root --auto -o times
echo "art-result: $? TIDArun_4"

TIDArun-art.sh expert-monitoring.root  expert-monitoring*-ref.root --auto -p FastTrack -o times-FTF
echo "art-result: $? TIDArun_5"

RunTrigCostD3PD.exe -f trig_cost.root  --outputTagFromAthena --costMode --linkOutputDir
echo "art-result: $? RunTrigCostD3PD_6"

TIDAcpucost.exe costMon/TrigCostRoot_Results.root costMon/TrigCostRoot_Results.root -o cost-perCall --auto -d "/Algorithm" -p "_Time_perCall"
echo "art-result: $? TIDAcpucost_7"

TIDAcpucost.exe costMon/TrigCostRoot_Results.root costMon/TrigCostRoot_Results.root -o cost-perEvent --auto -d "/Algorithm" -p "_Time_perEvent"
echo "art-result: $? TIDAcpucost_8"

TIDAcpucost.exe costMon/TrigCostRoot_Results.root costMon/TrigCostRoot_Results.root -o cost-perCall-chain --auto -d "/Chain_Algorithm" -p "_Time_perCall"
echo "art-result: $? TIDAcpucost_9"

TIDAcpucost.exe costMon/TrigCostRoot_Results.root costMon/TrigCostRoot_Results.root -o cost-perEvent-chain --auto -d "/Chain_Algorithm" -p "_Time_perEvent"
echo "art-result: $? TIDAcpucost_10"

