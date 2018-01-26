#!/bin/bash
# art-description: art job for InDetPerformanceRTT, ttbar
# art-type: grid
# art-input:  mc15_13TeV.410000.PowhegPythiaEvtGen_P2012_ttbar_hdamp172p5_nonallhad.recon.RDO.e3698_s2608_s2183_r7728_tid08543088_00
# art-input-nfiles: 1

fileList="['${ArtInFile//,/', '}']"
echo "List of files = ", $fileList


get_files -jo InDetPerformanceRTT/InDetPerformanceRTT_jobOptions_For20.7Digi.py
athena.py  -c "from AthenaCommon.AthenaCommonFlags import athenaCommonFlags; athenaCommonFlags.FilesInput=$fileList; EventMax=500;" InDetPerformanceRTT/InDetPerformanceRTT_jobOptions_For20.7Digi.py
