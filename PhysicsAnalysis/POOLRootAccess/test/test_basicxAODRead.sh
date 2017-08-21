#!/bin/sh

# art-description: Test basic xAOD reading in python
# art-type: build
# art-ci: 21.2

echo "Running test ut_basicxAODRead_test ... "

ut_basicxAODRead_test

result=$?

echo "Exit code is "$result

exit $result

