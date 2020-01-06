#!/bin/sh
#
# art-description: Truth derivation validation
#
# art-type: grid
# art-include: 21.2/AnalysisBase
# art-include: 21.2/AthAnalysis
# art-output: test_histograms.root
# art-input: mc15_13TeV:mc15_13TeV.410470.PhPy8EG_A14_ttbar_hdamp258p75_nonallhad.deriv.DAOD_TRUTH3.e6337_e5984_p3655
# art-input-nfiles: 1

TruthDerivationTester --input ${ArtInFile} --output test_histograms.root --nevents -1
echo "art-result: $?"
