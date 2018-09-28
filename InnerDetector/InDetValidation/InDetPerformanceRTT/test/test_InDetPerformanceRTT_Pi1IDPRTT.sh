#!/bin/bash
# art-description: art job for InDetPerformanceRTT, Single Pion 1GeV
# art-type: grid
# art-input: mc15_13TeV.422014.ParticleGun_single_pion_Pt1.recon.RDO.e4459_s2726_r7059_tid07275532_00
# art-input-nfiles: 10
# art-include: 21.0/Athena
# art-include: master/Athena
# art-output: *.root
# art-output: *.xml
# art-output: dcube
# art-output: *.png

fileList="['${ArtInFile//,/', '}']"
echo "List of files = ", $fileList

joboptions="InDetPerformanceRTT/InDetPerformanceRTT_jobOptions.py"
set -x 

get_files -jo $joboptions
ls -lR
athena.py  -c "from AthenaCommon.AthenaCommonFlags import athenaCommonFlags; athenaCommonFlags.FilesInput=$fileList; EventMax=20000;" $joboptions
echo  "art-result: $? reco"

dcubeName="Pi1"
dcubeXml="/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/InDetPerformanceRTT/dcube/config/Pi1_DCubeConfig.xml"
dcubeRef="/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/InDetPerformanceRTT/ReferenceHistograms/InDetStandardPlots-SinglePi1000.root"

bash /cvmfs/atlas.cern.ch/repo/sw/art/dcube/bin/art-dcube $dcubeName InDetStandardPlots.root $dcubeXml $dcubeRef
bash /cvmfs/atlas.cern.ch/repo/sw/art/dcube/bin/art-dcube "Pi1" "InDetStandardPlots-SinglePi1000.root"
echo  "art-result: $? plot"

plotmacro="/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/InDetPerformanceRTT/scripts/makeplots.C"
root -b -q $plotmacro
echo "art-result: $? root"
