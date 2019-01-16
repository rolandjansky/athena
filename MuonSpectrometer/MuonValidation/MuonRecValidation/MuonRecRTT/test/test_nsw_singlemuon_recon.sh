#!/bin/sh
#
# art-description: Test the reconstruction of muon events of New Small Wheel.
#
# art-type: grid
# 
# art-include: 21.3/Athena

set -x

Reco_tf.py \
        --preExec 'all:from MuonRecExample.MuonRecFlags import muonRecFlags,muonStandaloneFlags;muonRecFlags.doFastDigitization=False; muonRecFlags.useLooseErrorTuning.set_Value_and_Lock(True);muonRecFlags.doCSCs=False;muonRecFlags.doNSWNewThirdChain=True;muonRecFlags.dosTGCs=True;muonRecFlags.doMicromegas=True;muonStandaloneFlags.printSummary=True;muonRecFlags.doTrackPerformance=True;muonRecFlags.TrackPerfSummaryLevel=2;muonRecFlags.TrackPerfDebugLevel=5;rec.doTrigger=False;rec.doInDet=True;rec.doCalo=True;rec.doLArg=True;rec.doEgamma=False;rec.doTile=True;rec.doLucid=False;rec.doZdc=False;rec.doJetMissingETTag=False;from MuonCombinedRecExample.MuonCombinedRecFlags import muonCombinedRecFlags;muonCombinedRecFlags.doCaloTrkMuId=True;muonCombinedRecFlags.doMuGirlLowBeta=False;muonCombinedRecFlags.doMuGirl=True;muonCombinedRecFlags.doCombinedFit=True;muonCombinedRecFlags.doMuonSegmentTagger=True;svcMgr.MessageSvc.defaultLimit = 1000;svcMgr.MessageSvc.debugLimit = 1000;svcMgr.MessageSvc.verboseLimit = 1000' \
        --ignorePatterns 'ToolSvc.MM_PrepDataProviderTool.+.ERROR.' \
        --ignoreErrors FALSE  \
        --outputESDFile nsw.ESD.pool.root \
        --inputRDOFile  /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/MuonRecRTT/NSW/recon/*.root \
        --autoConfiguration everything \
        --maxEvents -1 --skipEvents 0

echo "art-result: $?"
