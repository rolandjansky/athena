#!/bin/bash
# art-description: art job for FTK_mu_ttbar
# art-type: grid
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

