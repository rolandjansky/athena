#!/bin/sh

# art-description: SUSYTools ART test - share/minimalExampleJobOptions_atlfast.py
# art-type: build
# art-ci: 21.2

echo "Running SUSYTools test: \'share/minimalExampleJobOptions_data.py\'"
athena SUSYTools/minimalExampleJobOptions_atlfast.py

result=$?
echo "Done. Exit code is "$result
exit $result