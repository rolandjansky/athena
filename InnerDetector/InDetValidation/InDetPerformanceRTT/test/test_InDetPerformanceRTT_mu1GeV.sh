#!/bin/bash
# art-description: art job for InDetPerformanceRTT, Single Muon 1GeV
# art-type: grid
# art-input: mc15_13TeV.422010.ParticleGun_single_mu_Pt1.recon.RDO.e4459_s2726_r7059_tid07275515_00 
# art-input-nfiles: 10

fileList="['${ArtInFile//,/', '}']"
echo "List of files = ", $fileList

joboptions="InDetPerformanceRTT/InDetPerformanceRTT_jobOptions.py"
get_files -jo $joboptions
athena.py  -c "from AthenaCommon.AthenaCommonFlags import athenaCommonFlags; athenaCommonFlags.FilesInput=$fileList; EventMax=20000;" $joboptions
