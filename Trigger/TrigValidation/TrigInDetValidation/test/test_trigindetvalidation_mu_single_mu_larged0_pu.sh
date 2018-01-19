#!/bin/bash
# art-description: art job for mu_single_mu_larged0_PU
# art-type: grid
# art-input:  mc15_13TeV.107237.ParticleGenerator_mu_Pt4to100_vertxy20.recon.RDO.e3603_s2726_r7772
# art-output: *.dat 
# art-output: *.root
# art-output: *.log
# art-input-nfiles: 25



export RTTJOBNAME=TrigInDetValidation_mu_single_mu_larged0_PU


fileList="['${ArtInFile//,/', '}']"
echo "List of files = $fileList"

get_files -jo             TrigInDetValidation/TrigInDetValidation_RTT_topOptions_MuonSlice.py
athena.py  -c 'ARTConfig=$fileList;EventMax=30000'             TrigInDetValidation/TrigInDetValidation_RTT_topOptions_MuonSlice.py
echo "art-result: $? athena_0"

