#!/bin/bash
# art-description: art job for InDetPerformanceRTT, Min Bias, low pT tracking
# art-type: grid
# art-input: mc15_13TeV.119994.Pythia8_A2MSTW2008LO_minbias_inelastic.recon.RDO.e3802_s2608_s2183_r7509_tid07497163_00
# art-input-nfiles: 3
# art-output: InDetStandardPlots.root
# art-output: comparison.root
# art-output: comparison.ps
# art-output: *.root
# art-output: *.xml

fileList="['${ArtInFile//,/', '}']"
echo "List of files = ", $fileList

joboptions="InDetPerformanceRTT/InDetPerformanceRTT_jobOptions_LowPtTracking.py"
get_files -jo $joboptions
athena.py  -c "from AthenaCommon.AthenaCommonFlags import athenaCommonFlags; athenaCommonFlags.FilesInput=$fileList; EventMax=1500;" $joboptions

refhist="/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/InDetPerformanceRTT/ReferenceHistograms/InDetStandardPlots-MinBias.root"
rootcomp.py -o comparison -c $refhist InDetStandardPlots.root
