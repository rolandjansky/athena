#!/bin/sh
#
# art-description: Truth derivation validation
#
# art-type: grid
# art-include: master
# art-output: test_histograms.root
# art-output: dcube
# art-html: dcube

art.py download --nightly-release master --project Athena --platform x86_64-slc6-gcc62-opt DerivationFrameworkMCTruthART test_mc15TRUTH3.sh
inputFile=./ref-*/DAOD_TRUTH3.art.pool.root
echo "art-result: $? Inputs"

TruthDerivationTester --input ${inputFile} --output test_histograms.root --nevents -1
echo "art-result: $? Analysis"

dcubeName="LHE"
dcubeXml="/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/DerivationFrameworkAnalysisTests/Analysis_DCubeConfig.xml"
dcubeRef="/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/DerivationFrameworkAnalysisTests/reference_2020_02_14_test_histograms.root"

bash /cvmfs/atlas.cern.ch/repo/sw/art/dcube/bin/art-dcube $dcubeName test_histograms.root $dcubeXml $dcubeRef
echo  "art-result: $? DCube"
