#!/bin/bash
# art-description: art job for InDetPerformanceRTT, Single Pion 1GeV
# art-type: grid
# art-input: mc15_13TeV.422014.ParticleGun_single_pion_Pt1.recon.RDO.e4459_s2726_r7059_tid07275532_00
# art-input-nfiles: 10
# art-include: 21.0/Athena
# art-include: master/Athena
# art-output: InDetStandardPlots.root
# art-output: comparison.root
# art-output: comparison.ps
# art-output: *.root
# art-output: *.xml

fileList="['${ArtInFile//,/', '}']"
echo "List of files = ", $fileList

joboptions="InDetPerformanceRTT/InDetPerformanceRTT_jobOptions.py"
set -x 

get_files -jo $joboptions
ls -lR
athena.py  -c "from AthenaCommon.AthenaCommonFlags import athenaCommonFlags; athenaCommonFlags.FilesInput=$fileList; EventMax=20000;" $joboptions
echo  "art-result: $? reco"

bash /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/InDetPerformanceRTT/dcube/bin/art-dcube "Pi1" "InDetStandardPlots-SinglePi1000.root"
echo  "art-result: $? plot"

