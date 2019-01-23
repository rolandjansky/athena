#!/bin/sh
# art-type: build
# art-include: master/Athena

# clear BS from previous runs
rm -rf  data_test.*.data

athena --dump-configuration=cfg.txt  --threads=1 --skipEvents=10 --evtMax=20 --filesInput="/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/TrigP1Test/data17_13TeV.00327265.physics_EnhancedBias.merge.RAW._lb0100._SFO-1._0001.1" TrigUpgradeTest/egamma.withViews.py #&&
checkxAOD.py myESD.pool.root &&
athena TrigUpgradeTest/checkESD.py
