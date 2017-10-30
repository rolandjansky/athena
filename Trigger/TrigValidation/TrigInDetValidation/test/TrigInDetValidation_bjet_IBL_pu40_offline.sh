#!/bin/bash
# art-description: art job for bjet_IBL_pu40_offline
# art-type: grid
# art-output: HLTEF-plots
# art-output: HLTL2-plots
# art-output: HLTL2-vtx
# art-output: times
# art-output: times-FTF
# art-output: cost-perCall
# art-output: cost-perEvent
# art-output: cost-perCall-chain
# art-output: cost-perEvent-chain
# art-input:  mc15_13TeV.410000.PowhegPythiaEvtGen_P2012_ttbar_hdamp172p5_nonallhad.recon.RDO.e3698_s2608_s2183_r7193
# art-output: *.dat 
# art-output: *.root
# art-output: *.log

export RTTJOBNAME=TrigInDetValidation_bjet_IBL_pu40_offline

get_files -jo             TrigInDetValidation/TrigInDetValidation_RTT_topOptions_BjetSlice.py
athena.py  -c 'ARTConfig=["/eos/atlas/atlascerngroupdisk/proj-sit/trigindet/mc15_13TeV.410000.PowhegPythiaEvtGen_P2012_ttbar_hdamp172p5_nonallhad.recon.RDO.e3698_s2608_s2183_r7193/RDO.06752771._000001.pool.root.1"];	    EventMax=900;doIDNewTracking=True;runMergedChain=True;globalTag="OFLCOND-RUN12-SDR-17"'             TrigInDetValidation/TrigInDetValidation_RTT_topOptions_BjetSlice.py
get_files -data TIDAdata11-rtt-offline.dat
get_files -data TIDAdata_cuts.dat
get_files -data TIDAdata_chains.dat
get_files -data TIDAbeam.dat
get_files -data Test_bin.dat
TIDArdict.exe TIDAdata11-rtt-offline.dat -f data-bjet-pileup.root -b Test_bin.dat

TIDArun-art.sh data-bjet-pileup.root data-bjet_IBL_pu40_offline-reference.root  HLT_j55_boffperf_InDetTrigTrackingxAODCnv_Bjet_IDTrig HLT_j55_boffperf_split_InDetTrigTrackingxAODCnv_Bjet_IDTrig_forID HLT_j55_boffperf_split_InDetTrigTrackingxAODCnv_Bjet_FTF_forID  -d HLTEF-plots

TIDArun-art.sh data-bjet-pileup.root data-bjet_IBL_pu40_offline-reference.root   HLT_j55_boffperf_split_InDetTrigTrackingxAODCnv_Bjet_FTF_forID HLT_j55_boffperf_InDetTrigTrackingxAODCnv_Bjet_FTF_forID HLT_j55_boffperf_split_InDetTrigTrackingxAODCnv_BjetPrmVtx_FTF_SuperRoi_xPrimVx -d HLTL2-plots

TIDArun-art.sh data-bjet-pileup.root data-bjet_IBL_pu40_offline-reference.root  HLT_j55_boffperf_split_InDetTrigTrackingxAODCnv_BjetPrmVtx_FTF_SuperRoi_xPrimVx/xPrimVx HLT_j55_boffperf_split_InDetTrigTrackingxAODCnv_BjetPrmVtx_FTF_SuperRoi_EFHistoVtx/EFHistoVtx -c TIDAvertex.dat -d HLTL2-vtx

TIDArun-art.sh expert-monitoring.root  expert-monitoring*-ref.root --auto -o times

TIDArun-art.sh expert-monitoring.root  expert-monitoring*-ref.root --auto -p FastTrack -o times-FTF

RunTrigCostD3PD.exe -f trig_cost.root  --outputTagFromAthena --costMode --linkOutputDir

TIDAcpucost.exe costMon/TrigCostRoot_Results.root costMon/TrigCostRoot_Results.root -o cost-perCall --auto -d "/Algorithm" -p "_Time_perCall"

TIDAcpucost.exe costMon/TrigCostRoot_Results.root costMon/TrigCostRoot_Results.root -o cost-perEvent --auto -d "/Algorithm" -p "_Time_perEvent"

TIDAcpucost.exe costMon/TrigCostRoot_Results.root costMon/TrigCostRoot_Results.root -o cost-perCall-chain --auto -d "/Chain_Algorithm" -p "_Time_perCall"

TIDAcpucost.exe costMon/TrigCostRoot_Results.root costMon/TrigCostRoot_Results.root -o cost-perEvent-chain --auto -d "/Chain_Algorithm" -p "_Time_perEvent"

