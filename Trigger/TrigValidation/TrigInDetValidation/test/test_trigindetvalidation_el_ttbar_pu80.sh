#!/bin/bash
# art-description: art job for el_ttbar_pu80
# art-type: grid
# art-input:  mc15_13TeV.410000.PowhegPythiaEvtGen_P2012_ttbar_hdamp172p5_nonallhad.recon.RDO.e3698_s2608_s2183_r7195
# art-output: *.dat 
# art-output: *.root
# art-output: *.log
# art-input-nfiles: 25



export RTTJOBNAME=TrigInDetValidation_el_ttbar_pu80


fileList="['${ArtInFile//,/', '}']"
echo "List of files = $fileList"

get_files -jo            TrigInDetValidation/TrigInDetValidation_RTT_topOptions_ElectronSlice.py
athena.py  -c 'ARTConfig=$fileList;EventMax=8000'            TrigInDetValidation/TrigInDetValidation_RTT_topOptions_ElectronSlice.py
echo "art-result: $? athena_0"

get_files -data TIDAdata11-rtt.dat
get_files -data TIDAdata_cuts.dat
get_files -data TIDAdata_chains.dat
get_files -data TIDAbeam.dat
get_files -data Test_bin.dat
TIDArdict.exe TIDAdata11-rtt.dat -f data-electron-merge.root -p 11 -b Test_bin.dat
echo "art-result: $? TIDArdict_1"

TIDArun-art.sh data-electron-merge.root data-el_ttbar_pu80-reference.root HLT_e24_medium_idperf_InDetTrigTrackingxAODCnv_Electron_FTF  HLT_e24_medium_idperf_InDetTrigTrackingxAODCnv_Electron_IDTrig  -d HLTEF-plots
echo "art-result: $? TIDArun_2"

TIDArun-art.sh data-electron-merge.root data-el_ttbar_pu80-reference.root HLT_e24_medium_idperf_InDetTrigTrackingxAODCnv_Electron_FTF -d HLTL2-plots
echo "art-result: $? TIDArun_3"

TIDArun-art.sh data-electron-merge.root data-el_ttbar_pu80-reference.root  HLT_e5_loose_idperf_InDetTrigTrackingxAODCnv_Electron_FTF  HLT_e5_loose_idperf_InDetTrigTrackingxAODCnv_Electron_IDTrig   -d HLTEF-plots-lowpt
echo "art-result: $? TIDArun_4"

