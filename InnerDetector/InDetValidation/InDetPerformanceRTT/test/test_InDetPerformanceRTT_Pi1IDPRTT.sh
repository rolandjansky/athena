#!/bin/bash
# art-description: art job for InDetPerformanceRTT, Single Pion 1GeV
# art-type: grid
# art-input: mc15_13TeV.422014.ParticleGun_single_pion_Pt1.recon.RDO.e4459_s2726_r7059_tid07275532_00
# art-input-nfiles: 10
# art-output: InDetStandardPlots.root
# art-output: comparison.root
# art-output: comparison.ps
# art-output: *.root
# art-output: *.xml

fileList="['${ArtInFile//,/', '}']"
echo "List of files = ", $fileList

joboptions="InDetPerformanceRTT/InDetPerformanceRTT_jobOptions.py"
get_files -jo $joboptions
athena.py  -c "from AthenaCommon.AthenaCommonFlags import athenaCommonFlags; athenaCommonFlags.FilesInput=$fileList; EventMax=20000;" $joboptions

refhist="/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/InDetPerformanceRTT/ReferenceHistograms/InDetStandardPlots-SinglePi1000.root"
rootcomp.py -o comparison -c $refhist InDetStandardPlots.root
