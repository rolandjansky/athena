#!/bin/bash
# art-description: ITk layout with split Strip sensoes sim+reco
# art-type: grid
# art-include: 21.9/Athena
# art-output: *.root
# art-output: *.xml
# art-output: dcube*
#
#
# Running art test for single muon, 100GeV, geometry ATLAS-P2-ITK-23-00-99
#


file=test_ITk_single_particle.sh
script="`basename \"$0\"`"

echo "Executing script ${file}"
echo " "
"$file" muons 100GeV  ATLAS-P2-ITK-23-00-99 ${script} analogue



