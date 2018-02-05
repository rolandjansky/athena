#!/bin/sh
# art-type: build
# art-ci: master

rm -rf test_RegSel_mt
mkdir test_RegSel_mt
cd test_RegSel_mt
athena --threads=4 --concurrent-events=1 --evtMax=10 --filesInput="/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/TrigP1Test/data17_13TeV.00327265.physics_EnhancedBias.merge.RAW._lb0100._SFO-1._0001.1" -c 'enableViews=True;TestType="mt"' RegSelSvcTest/testRegSelSvc.py