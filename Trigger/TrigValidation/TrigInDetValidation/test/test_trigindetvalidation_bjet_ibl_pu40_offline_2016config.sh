#!/bin/bash
# art-description: art job for bjet_IBL_pu40_offline_2016config
# art-type: grid
# art-output: HLTEF-plots
# art-output: HLTL2-plots
# art-output: times
# art-output: times-FTF
# art-output: cost-perEvent
# art-output: cost-perCall-chain
# art-input:  mc15_13TeV.410000.PowhegPythiaEvtGen_P2012_ttbar_hdamp172p5_nonallhad.recon.RDO.e3698_s2608_s2183_r7193
# art-output: *.dat 
# art-output: *.root
# art-output: *.log
# art-input-nfiles: 25



export RTTJOBNAME=TrigInDetValidation_bjet_IBL_pu40_offline_2016config


fileList="['${ArtInFile//,/', '}']"
echo "List of files = $fileList"

get_files -jo             TrigInDetValidation/TrigInDetValidation_RTT_topOptions_BjetSlice.py
athena.py  -c 'ARTConfig=$fileList;EventMax=1800;doIDNewTracking=True;minVtxTrackpT=1000;bjetEtaHalfWidth=0.2;bjetPhiHalfWidth=0.2;splitZHalfWidth=20;globalTag="OFLCOND-RUN12-SDR-17"'             TrigInDetValidation/TrigInDetValidation_RTT_topOptions_BjetSlice.py
echo "art-result: $? athena_0"

get_files -data TIDAdata11-rtt.dat
get_files -data TIDAdata_cuts.dat
get_files -data TIDAdata_chains.dat
get_files -data TIDAbeam.dat
get_files -data Test_bin.dat
TIDArdict.exe TIDAdata11-rtt.dat -f data-tau-IBL.root -b Test_bin.dat
echo "art-result: $? TIDArdict_1"

TIDArun-art.sh data-muon.root data-mu_single_mu_larged0-reference.root HLT_mu6_idperf_InDetTrigTrackingxAODCnv_Muon_FTF  HLT_mu6_idperf_InDetTrigTrackingxAODCnv_Muon_IDTrig -d HLTEF-plots
echo "art-result: $? TIDArun_2"

TIDArun-art.sh data-muon.root data-mu_single_mu_larged0_PU-reference.root HLT_mu6_idperf_InDetTrigTrackingxAODCnv_Muon_FTF -d HLTL2-plots
echo "art-result: $? TIDArun_3"

TIDArun-art.sh expert-monitoring.root  expert-monitoring*-ref.root --auto -o times
echo "art-result: $? TIDArun_4"

TIDArun-art.sh expert-monitoring.root  expert-monitoring*-ref.root --auto -p FastTrack -o times-FTF
echo "art-result: $? TIDArun_5"

RunTrigCostD3PD.exe -f trig_cost.root  --outputTagFromAthena --costMode --linkOutputDir
echo "art-result: $? RunTrigCostD3PD_6"

TIDAcpucost.exe 
echo "art-result: $? TIDAcpucost_7"

TIDAcpucost.exe costMon/TrigCostRoot_Results.root costMon/TrigCostRoot_Results.root -o cost-perEvent --auto -d "/Algorithm" -p "_Time_perEvent"
echo "art-result: $? TIDAcpucost_8"

TIDAcpucost.exe costMon/TrigCostRoot_Results.root costMon/TrigCostRoot_Results.root -o cost-perCall-chain --auto -d "/Chain_Algorithm" -p "_Time_perCall"
echo "art-result: $? TIDAcpucost_9"

TIDAcpucost.exe 
echo "art-result: $? TIDAcpucost_10"

