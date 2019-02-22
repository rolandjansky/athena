#!/bin/sh
# art-type: build
# art-include: master/Athena

athena --threads=1 --skipEvents=10 --evtMax=20 --filesInput="/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/TrigP1Test/data17_13TeV.00327265.physics_EnhancedBias.merge.RAW._lb0100._SFO-1._0001.1" TrigUpgradeTest/dataScoutingTest.py
