#!/bin/sh

# art-description: SUSYTools ART test - share/minimalExampleJobOptions_mc.py
# art-type: build
# art-ci: 21.2

echo "Running SUSYTools test: \'share/minimalExampleJobOptions_mc.py\'"
athena /afs/cern.ch/user/s/szambito/GIT/athena/PhysicsAnalysis/SUSYPhys/SUSYTools/share/minimalExampleJobOptions_mc.py

result=$?
echo "Done. Exit code is "$result
exit $result
