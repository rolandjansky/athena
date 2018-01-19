#!/bin/bash
# art-description: art job for FTK_beamspot_ttbar
# art-type: grid
# art-output: HLTL2-plots-lowpt
# art-output: HLTEF-plots
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



export RTTJOBNAME=TrigInDetValidation_FTK_beamspot_ttbar


fileList="['${ArtInFile//,/', '}']"
echo "List of files = $fileList"

get_files -jo             TrigInDetValidation/TrigInDetValidation_RTT_topOptions_BeamspotSlice.py
athena.py  -c 'ARTConfig=$fileList;EventMax=2000;doFTK=True;rec.doFloatingPointException.set_Value_and_Lock(False)'             TrigInDetValidation/TrigInDetValidation_RTT_topOptions_BeamspotSlice.py
echo "art-result: $? athena_0"

get_files -data data-FTK_mu_ttbar_offline-reference.root
TIDArdict.exe data-muon-FTK.root data-FTK_mu_ttbar_offline-reference.root HLT_mu6_idperf_InDetTrigTrackingxAODCnv_Muon_FTF  HLT_mu6_FTK_idperf_InDetTrigTrackingxAODCnv_Muon_FTK HLT_mu6_FTKRefit_idperf_InDetTrigTrackingxAODCnv_Muon_FTKRefit   -d HLTL2-plots-lowpt
echo "art-result: $? TIDArdict_1"

TIDArun-art.sh data-bjet-FTK.root data-FTK_bjet_ttbar_offline-reference.root HLT_j55_boffperf_split_InDetTrigTrackingxAODCnv_Bjet_IDTrig_forID HLT_j55_boffperf_split_FTKVtx_InDetTrigTrackingxAODCnv_Bjet_IDTrig HLT_j55_boffperf_split_FTK_InDetTrigTrackingxAODCnv_Bjet_FTK_IDTrig  HLT_j55_boffperf_split_FTKRefit_InDetTrigTrackingxAODCnv_Bjet_FTKRefit_IDTrig -d HLTEF-plots
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

