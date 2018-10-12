#!/usr/bin/env sh
#
# Post-processing script to be used with atlas_add_test if no
# log file post-processing is required (as done in post.sh)

if [ -z "$testStatus" ]; then
   echo "nopost.sh> Error: athena exit status is not available (\$testStatus is not set)"
   exit 1
fi

exit $testStatus
