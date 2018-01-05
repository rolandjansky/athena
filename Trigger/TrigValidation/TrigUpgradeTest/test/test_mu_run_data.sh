#!/bin/sh
# art-type: build
# art-ci: master

### BSfile ###
athena --threads=1 -c 'EvtMax=50;FilesInput="/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/TrigP1Test/data17_13TeV.00327265.physics_EnhancedBias.merge.RAW._lb0100._SFO-1._0001.1"' TrigUpgradeTest/mu.withViews.py
### RDOfile ###
#athena --threads=1 -c 'EvtMax=50;FilesInput="/afs/cern.ch/user/s/shhayash/workspace/public/RDOfile/ttbar/RDO_TRIG.root"' TrigUpgradeTest/mu.withViews.py
