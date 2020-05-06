#!/bin/bash
# art-description: InclinedDuals sim+reco
# art-type: grid
# art-include: 21.9/Athena
# art-output: *.root
# art-output: *.xml
# art-output: dcube*
#
#
# Running art test for single muon, geometry ATLAS-P2-ITK-17-06-00
#
# resolve the script directory:
# https://stackoverflow.com/questions/59895/how-to-get-the-source-directory-of-a-bash-script-from-within-the-script-itself
# and https://acode-browser2.usatlas.bnl.gov/lxr/source/AthAna/atlas/PhysicsAnalysis/TauID/TauCorrUncert/setup.sh
#


SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )/../share"  >/dev/null 2>&1 && pwd )"



file=${SCRIPT_DIR}/test_InclinedAlternative_single.sh
script="`basename \"$0\"`"


if [ -f "$file" ]
then
	echo "Executing script $file"
        echo " "
        "$file" muons 1GeV  ATLAS-P2-ITK-17-06-00 ${script}
else
	echo "ERROR: script $file not found."
fi


