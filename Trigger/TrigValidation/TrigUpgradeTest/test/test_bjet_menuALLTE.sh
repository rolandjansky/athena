#!/bin/sh
# art-type: build
# art-include: master/Athena

athena --imf --threads=1 --skipEvents=0 --evtMax=100 --filesInput="/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/TrigP1Test/data17_13TeV.00327265.physics_EnhancedBias.merge.RAW._lb0100._SFO-1._0001.1" TrigUpgradeTest/bjet.menu.ALLTE.py
