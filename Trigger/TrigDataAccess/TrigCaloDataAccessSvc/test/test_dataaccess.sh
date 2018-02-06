#!/bin/sh
# art-type: build
# art-ci: master

athena.py --concurrent-events=1 --threads=1 --evtMax=10 --filesInput="/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/TrigP1Test/data17_13TeV.00327265.physics_EnhancedBias.merge.RAW._lb0100._SFO-1._0001.1" -c 'enableViews=False;doMuon=False;doID=False' TrigCaloDataAccessSvc/testDataAccessService.py
