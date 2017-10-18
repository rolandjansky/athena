#!/bin/bash
# art-description: art job for all_ttbar_pu80
# art-type: grid
# art-output: HLTEF-plots-electron
# art-output: HLTL2-plots-electron
# art-output: HLTEF-plots-electron-lowpt
# art-output: HLTL2-plots-electron-lowpt
# art-output: HLTEF-plots-muon
# art-output: HLTL2-plots-muon
# art-output: HLTEF-plots-tau
# art-output: HLTEF-plots-bjet
# art-output: HLTL2-plots-bjet
# art-output: times
# art-output: times-FTF
# art-output: cost-perCall
# art-output: cost-perEvent
# art-output: cost-perCall-chain
# art-output: cost-perEvent-chain
# art-input:  mc15_13TeV.410000.PowhegPythiaEvtGen_P2012_ttbar_hdamp172p5_nonallhad.recon.RDO.e3698_s2608_s2183_r7195
# art-output: *.dat 
# art-output: *.root
# art-output: *.log

export RTTJOBNAME=TrigInDetValidation_all_ttbar_pu80

get_files -jo             TrigInDetValidation/TrigInDetValidation_RTT_topOptions_AllSlices.py
athena.py  -c 'ARTConfig=["/eos/atlas/atlascerngroupdisk/proj-sit/trigindet/mc15_13TeV.410000.PowhegPythiaEvtGen_P2012_ttbar_hdamp172p5_nonallhad.recon.RDO.e3698_s2608_s2183_r7195/RDO.06752780._000001.pool.root.1"];	    EventMax=1000;runMergedChain=True;doIDNewTracking=True;rec.doFloatingPointException.set_Value_and_Lock(False)'             TrigInDetValidation/TrigInDetValidation_RTT_topOptions_AllSlices.py
get_files -data TIDAdata11-rtt.dat
get_files -data TIDAdata_cuts.dat
get_files -data TIDAdata_chains.dat
get_files -data TIDAbeam.dat
get_files -data Test_bin.dat
TIDArdict.exe TIDAdata11-rtt.dat -r Offline -f data-all.root -b Test_bin.dat

TIDArun-art.sh data-all.root data-all_ttbar_pu80-reference.root HLT_e24_medium_idperf_InDetTrigTrackingxAODCnv_Electron_FTF  HLT_e24_medium_idperf_InDetTrigTrackingxAODCnv_Electron_IDTrig  -d HLTEF-plots-electron

TIDArun-art.sh data-all.root data-all_ttbar_pu80-reference.root HLT_e24_medium_idperf_InDetTrigTrackingxAODCnv_Electron_FTF -d HLTL2-plots-electron

TIDArun-art.sh data-all.root data-all_ttbar_pu80-reference.root HLT_e5_loose_idperf_InDetTrigTrackingxAODCnv_Electron_FTF  HLT_e5_loose_idperf_InDetTrigTrackingxAODCnv_Electron_IDTrig   -d HLTEF-plots-electron-lowpt

TIDArun-art.sh data-all.root data-all_ttbar_pu80-reference.root HLT_e5_loose_idperf_InDetTrigTrackingxAODCnv_Electron_FTF  -d HLTL2-plots-electron-lowpt

TIDArun-art.sh data-all.root data-all_ttbar_pu80-reference.root HLT_mu24_idperf_InDetTrigTrackingxAODCnv_Muon_FTF   HLT_mu24_idperf_InDetTrigTrackingxAODCnv_Muon_IDTrig -d HLTEF-plots-muon

TIDArun-art.sh data-all.root data-all_ttbar_pu80-reference.root  HLT_mu24_idperf_InDetTrigTrackingxAODCnv_Muon_FTF  -d HLTL2-plots-muon

TIDArun-art.sh data-all.root data-all_ttbar_pu80-reference.root HLT_tau25_idperf_track_InDetTrigTrackingxAODCnv_Tau_FTF HLT_tau25_idperf_track_InDetTrigTrackingxAODCnv_Tau_IDTrig HLT_tau25_idperf_tracktwo_InDetTrigTrackingxAODCnv_TauIso_FTF_forID3 HLT_tau25_idperf_tracktwo_InDetTrigTrackingxAODCnv_Tau_IDTrig_forID3  -d HLTEF-plots-tau

TIDArun-art.sh data-all.root data-all_ttbar_pu80-reference.root  HLT_tau25_idperf_track_InDetTrigTrackingxAODCnv_Tau_FTF HLT_tau25_idperf_tracktwo_InDetTrigTrackingxAODCnv_TauCore_FTF_forID1 HLT_tau25_idperf_tracktwo_InDetTrigTrackingxAODCnv_TauIso_FTF_forID3-d HLTL2-plots-tau

TIDArun-art.sh data-all.root data-all_ttbar_pu80-reference.root   HLT_j55_boffperf_InDetTrigTrackingxAODCnv_Bjet_IDTrig HLT_j55_boffperf_split_InDetTrigTrackingxAODCnv_Bjet_IDTrig_forID HLT_j55_boffperf_split_InDetTrigTrackingxAODCnv_Bjet_FTF_forID -d HLTEF-plots-bjet

TIDArun-art.sh data-all.root data-all_ttbar_pu80-reference.root   HLT_j55_boffperf_split_InDetTrigTrackingxAODCnv_Bjet_FTF_forID HLT_j55_boffperf_InDetTrigTrackingxAODCnv_Bjet_FTF_forID HLT_j55_boffperf_split_InDetTrigTrackingxAODCnv_BjetPrmVtx_FTF_SuperRoi -d HLTL2-plots-bjet

TIDArun-art.sh expert-monitoring.root  expert-monitoring*-ref.root --auto -o times

TIDArun-art.sh expert-monitoring.root  expert-monitoring*-ref.root --auto -p FastTrack -o times-FTF

RunTrigCostD3PD.exe -f trig_cost.root  --outputTagFromAthena --costMode --linkOutputDir

TIDAcpucost.exe costMon/TrigCostRoot_Results.root costMon/TrigCostRoot_Results.root -o cost-perCall --auto -d "/Algorithm" -p "_Time_perCall"

TIDAcpucost.exe costMon/TrigCostRoot_Results.root costMon/TrigCostRoot_Results.root -o cost-perEvent --auto -d "/Algorithm" -p "_Time_perEvent"

TIDAcpucost.exe costMon/TrigCostRoot_Results.root costMon/TrigCostRoot_Results.root -o cost-perCall-chain --auto -d "/Chain_Algorithm" -p "_Time_perCall"

TIDAcpucost.exe costMon/TrigCostRoot_Results.root costMon/TrigCostRoot_Results.root -o cost-perEvent-chain --auto -d "/Chain_Algorithm" -p "_Time_perEvent"

