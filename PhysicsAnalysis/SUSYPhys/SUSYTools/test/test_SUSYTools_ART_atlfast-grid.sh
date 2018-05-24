#!/bin/sh

# art-description: SUSYTools ART test - share/minimalExampleJobOptions_atlfast.py
# art-type: grid
# art-include: 21.2/AthAnalysis
# art-output: monitoring.atlfast.root

echo "Running SUSYTools test: \'share/minimalExampleJobOptions_atlfast.py\'"
athena SUSYTools/minimalExampleJobOptions_atlfast.py --evtMax=-1

result=$?
echo "Done. Exit code is "$result
exit $result
