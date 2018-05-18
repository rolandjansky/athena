#!/bin/sh

# art-description: SUSYTools ART test - share/minimalExampleJobOptions_data.py
# art-type: grid
# art-include: 21.2/AthAnalysis
# art-output: monitoring.data.root

echo "Running SUSYTools test: \'share/minimalExampleJobOptions_data.py\'"
athena SUSYTools/minimalExampleJobOptions_data.py --evtMax=-1

result=$?
echo "Done. Exit code is "$result
exit $result
