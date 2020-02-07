#!/bin/sh
#
# art-description: Truth derivation validation
#
# art-type: grid
# art-include: 21.2/AnalysisBase
# art-include: 21.2/AthAnalysis
# art-output: test_histograms.root

art.py download --nightly-release 21.2 --project AthDerivation --platform x86_64-slc6-gcc62-opt DerivationFrameworkMCTruthART test_mc15TRUTH3.sh
inputFile=./ref-*/DAOD_TRUTH3.art.pool.root

TruthDerivationTester --input ${inputFile} --output test_histograms.root --nevents -1
echo "art-result: $?"
