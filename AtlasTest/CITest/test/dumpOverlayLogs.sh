#!/usr/bin/env bash

# Dump relevant overlay log files:
find . -name log.Overlay | xargs grep -wi "warning\|error\|fatal\|diff\|sync"
find . -name 'diff-root-*.RDO.log' | xargs grep -wi "warning\|error\|fatal\|diff\|sync"

exit ${ATLAS_CTEST_TESTSTATUS}
