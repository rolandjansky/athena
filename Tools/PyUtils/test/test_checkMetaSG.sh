#!/bin/sh

# art-description: Test checkMetaSG command
# art-type: build
# art-ci: 21.2

checkMetaSG $ASG_TEST_FILE_MC

result=$?

echo "Exit code was "$result

exit $result
