from InDetRecExample.InDetJobProperties import InDetFlags
InDetFlags.useBeamConstraint.set_Value_and_Lock(False)
InDetFlags.selectSCTIntimeHits=False
InDetFlags.doVtxLumi.set_Value_and_Lock(True)

AODFlags.TrackParticleSlimmer=True

from AthenaCommon.DetFlags import DetFlags
DetFlags.Calo_setOff()
DetFlags.Muon_setOff()
DetFlags.ID_setOn()
DetFlags.TRT_setOff()
DetFlags.detdescr.TRT_setOn()
DetFlags.makeRIO.TRT_setOff()

rec.doTrigger.set_Value_and_Lock(True)
rec.doCalo.set_Value_and_Lock(False)
rec.doMuon.set_Value_and_Lock(False)
rec.doJetMissingETTag.set_Value_and_Lock(False)
rec.doMuonCombined.set_Value_and_Lock(False)
rec.doEgamma.set_Value_and_Lock(False)
rec.doTau.set_Value_and_Lock(False)

from TrackD3PDMaker.VertexD3PDAnalysisFlags import VertexD3PDAnalysisFlags
VertexD3PDAnalysisFlags.useBeamspot.set_Value_and_Lock(True)
VertexD3PDAnalysisFlags.useTrigger.set_Value_and_Lock(False)
VertexD3PDAnalysisFlags.useTriggerRaw.set_Value_and_Lock(True)

