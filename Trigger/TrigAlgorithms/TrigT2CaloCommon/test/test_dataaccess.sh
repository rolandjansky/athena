#!/bin/sh

athena.py --concurrent-events=1 --threads=1 --evtMax=10 --filesInput="/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/TrigP1Test/data17_13TeV.00327265.physics_EnhancedBias.merge.RAW._lb0100._SFO-1._0001.1" -c 'isOnline=True;enableViews=False;doMuon=False;doID=False;enableL1MuonPhase1=False;' TrigT2CaloCommon/testDataAccessService.py

