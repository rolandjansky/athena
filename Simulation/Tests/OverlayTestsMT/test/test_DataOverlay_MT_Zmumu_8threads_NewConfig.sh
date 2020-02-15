#!/bin/sh

# art-description: MC+data Overlay with MT support, running with 8 threads, new config
# art-type: grid
# art-athena-mt: 8
# art-include: master/Athena

# art-output: dataOverlayRDO.pool.root
# art-output: log.*
# art-output: mem.summary.*
# art-output: mem.full.*
# art-output: runargs.*

OverlayTest.py -d -n 100 -t 8 2>&1 | tee log.OverlayTest

rc=$?
echo "art-result: $rc overlay"

# Regression disabled as many changes are planned
# rc2=-9999
# if [ $rc -eq 0 ]
# then
#     ArtPackage=$1
#     ArtJobName=$2
#     art.py compare grid --entries 10 ${ArtPackage} ${ArtJobName} --error-mode resilient --mode=semi-detailed --order-trees
#     rc2=$?
# fi
# echo  "art-result: $rc2 regression"
