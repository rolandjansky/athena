#!/bin/sh

# art-description: SUSYTools ART test - share/minimalExampleJobOptions_mc.py
# art-type: grid
# art-include: 21.2/AthAnalysis
# art-output: monitoring.mc.root

echo "Running SUSYTools test: \'share/minimalExampleJobOptions_mc.py\'"
athena SUSYTools/minimalExampleJobOptions_mc.py --evtMax=-1

result=$?
echo "Done. Exit code is "$result
exit $result
