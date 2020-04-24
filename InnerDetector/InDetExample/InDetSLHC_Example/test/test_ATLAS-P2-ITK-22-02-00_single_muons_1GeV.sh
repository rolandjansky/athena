#!/bin/bash
# art-description: InclinedDuals sim+reco
# art-type: grid
# art-include: 21.9/Athena
# art-output: *.root
# art-output: *.xml
# art-output: dcube*
#
#
# Running art test for single muon, geometry ATLAS-P2-ITK-22-02-00
#


file=test_ITk_single_particle.sh

echo "Executing script ${file}"
echo " "
"$file" muons 1GeV  ATLAS-P2-ITK-22-02-00



