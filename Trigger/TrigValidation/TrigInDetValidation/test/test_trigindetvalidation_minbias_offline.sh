#!/bin/bash
# art-description: art job for minBias_offline
# art-type: grid
# art-output: HLTL2-plots-lowpt
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
# art-input-nfiles: 25



export RTTJOBNAME=TrigInDetValidation_minBias_offline


fileList="['${ArtInFile//,/', '}']"
echo "List of files = $fileList"

get_files -jo             TrigInDetValidation/TrigInDetValidation_RTT_topOptions_MonitorSlice.py
athena.py  -c 'ARTConfig=$fileList;EventMax=2000;doIDNewTracking=True'             TrigInDetValidation/TrigInDetValidation_RTT_topOptions_MonitorSlice.py
echo "art-result: $? athena_0"

get_files -data data-FTK_mu_ttbar-reference.root
  data-muon-FTK.root data-FTK_mu_ttbar-reference.root HLT_mu6_idperf_InDetTrigTrackingxAODCnv_Muon_FTF  HLT_mu6_FTK_idperf_InDetTrigTrackingxAODCnv_Muon_FTK HLT_mu6_FTKRefit_idperf_InDetTrigTrackingxAODCnv_Muon_FTKRefit   -d HLTL2-plots-lowpt
echo "art-result: $? _1"

TIDArun-art.sh data-tau-FTK.root data-FTK_tau_tautau-reference.root HLT_tau25_idperf_track_InDetTrigTrackingxAODCnv_Tau_IDTrig  HLT_tau25_idperf_FTK:InDetTrigTrackingxAODCnv_Tau_FTK_IDTrig HLT_tau25_idperf_FTKRefit:InDetTrigTrackingxAODCnv_Tau_FTKRefit_IDTrig  -d HLTEF-plots
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

