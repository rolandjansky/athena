#!/bin/sh

# art-description: MC+data Overlay with MT support, running sequentially, new config
# art-type: grid
# art-include: master/Athena

# art-output: dataOverlayRDO.pool.root
# art-output: log.*
# art-output: mem.summary.*
# art-output: mem.full.*
# art-output: runargs.*
# art-output: *.pkl
# art-output: *Config.txt

set -o pipefail

OverlayTest.py -d -n 10 -t 0 2>&1 | tee log.OverlayTest

rc=$?
echo "art-result: $rc overlay"

rc2=-9999
if [ $rc -eq 0 ]
then
    ArtPackage=$1
    ArtJobName=$2
    art.py compare grid --entries 10 ${ArtPackage} ${ArtJobName} --mode=semi-detailed --order-trees
    rc2=$?
fi
echo  "art-result: $rc2 regression"
