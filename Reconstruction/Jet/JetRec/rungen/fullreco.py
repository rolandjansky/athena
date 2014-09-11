# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
athenaCommonFlags.FilesInput=["/afs/cern.ch/atlas/offline/test/mc11_valid.105200.T1_McAtNlo_Jimmy.digit.RDO.e835_s1310_s1300_d622.RDO.10evts.pool.root"]

from RecExConfig.RecFlags import rec
rec.LoadGeometry = True
rec.doDetStatus = False
rec.doAODCaloCells = False
rec.doAODall = False
rec.doInDet = False
rec.doLArg = False
rec.doTile = False
rec.doCalo = False
rec.doMuon = False
rec.doLucid = False
rec.doZdc = False
rec.doJetMissingETTag = True
rec.doEgamma = False
rec.doMuonCombined = False
rec.doTau = False
rec.doTrigger = False

from ParticleBuilderOptions.AODFlags import AODFlags
AODFlags.TrackParticleSlimmer=False 
AODFlags.TrackParticleLastHitAndPerigeeSlimmer=False

from JetRec.JetRecFlags import jetFlags
jetFlags.useTruth = True
jetFlags.useTopo = False
jetFlags.useTracks = False
jetFlags.usePflow = False
jetFlags.useMuonSegments = False 

from InDetRecExample.InDetJobProperties import InDetFlags
InDetFlags.doSecVertexFinder.set_Value_and_Lock(False)

# main jobOption
include ("RecExCommon/RecExCommon_topOptions.py")
include("RecExCommon/CleanItemList_Rel19.py")

