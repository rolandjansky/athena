#!/bin/bash
# art-description: art job for InDetPerformanceRTT, ttbar
# art-type: grid
# art-input:  mc15_13TeV.410000.PowhegPythiaEvtGen_P2012_ttbar_hdamp172p5_nonallhad.recon.RDO.e3698_s2608_s2183_r7728_tid08543088_00
# art-input-nfiles: 1
# art-include: 21.0/Athena
# art-include: master/Athena
# art-output: InDetStandardPlots.root
# art-output: comparison.root
# art-output: comparison.ps
# art-output: *.root
# art-output: *.xml

fileList="['${ArtInFile//,/', '}']"
echo "List of files = ", $fileList
joboptions="InDetPerformanceRTT/InDetPerformanceRTT_jobOptions_For20.7Digi.py"
set -x

get_files -jo $joboptions
ls -lR
athena.py  -c "from AthenaCommon.AthenaCommonFlags import athenaCommonFlags; athenaCommonFlags.FilesInput=$fileList; EventMax=500;" $joboptions
echo  "art-result: $? reco"

bash /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/InDetPerformanceRTT/dcube/bin/art-dcube "Ttbar" "InDetStandardPlots-Ttbar.root"
echo  "art-result: $? plot"

