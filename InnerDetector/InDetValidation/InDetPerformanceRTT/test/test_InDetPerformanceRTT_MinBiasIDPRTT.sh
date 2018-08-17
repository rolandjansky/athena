#!/bin/bash
# art-description: art job for InDetPerformanceRTT, Min Bias, low pT tracking
# art-type: grid
# art-input: mc15_13TeV.119994.Pythia8_A2MSTW2008LO_minbias_inelastic.recon.RDO.e3802_s2608_s2183_r7509_tid07497163_00
# art-input-nfiles: 3
# art-include: 21.0/Athena
# art-include: master/Athena
# art-output: *.root
# art-output: *.xml
# art-output: dcube
# art-output: *.png

fileList="['${ArtInFile//,/', '}']"
echo "List of files = ", $fileList

joboptions="InDetPerformanceRTT/InDetPerformanceRTT_jobOptions_LowPtTracking.py"
set -x

get_files -jo $joboptions
ls -lR
athena.py  -c "from AthenaCommon.AthenaCommonFlags import athenaCommonFlags; athenaCommonFlags.FilesInput=$fileList; EventMax=1500;" $joboptions
echo  "art-result: $? reco"

dcubeName="MinBias"
dcubeXml="/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/InDetPerformanceRTT/dcube/config/MinBias_DCubeConfig.xml"
dcubeRef="/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/InDetPerformanceRTT/ReferenceHistograms/InDetStandardPlots-MinBias.root"

bash /cvmfs/atlas.cern.ch/repo/sw/art/dcube/bin/art-dcube $dcubeName InDetStandardPlots.root $dcubeXml $dcubeRef
echo  "art-result: $? plot"

plotmacro="/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/InDetPerformanceRTT/scripts/makeplots.C"
root -b -q $plotmacro
echo "art-result: $? root"
