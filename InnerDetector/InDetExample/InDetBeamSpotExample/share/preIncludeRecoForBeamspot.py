#Correctly configure ID reconstruction
from InDetRecExample.InDetJobProperties import InDetFlags
InDetFlags.doVtxBeamSpot.set_Value_and_Lock(True)
InDetFlags.doTrackSegmentsDisappearing.set_Value_and_Lock(False)
InDetFlags.doCaloSeededAmbi.set_Value_and_Lock(False)


#Turn off all useless parts of the detector
from AthenaCommon.DetFlags import DetFlags
DetFlags.Calo_setOff()
DetFlags.Muon_setOff()
DetFlags.detdescr.BField_setOn()
DetFlags.ID_setOn()
DetFlags.TRT_setOff()
DetFlags.detdescr.TRT_setOn()
DetFlags.makeRIO.TRT_setOff()
DetFlags.pileup.ID_setOff()

#Disable useless parts of the reconstruction
from RecExConfig.RecFlags import rec
#Trigger needs to stay on or some tools crash
rec.doTrigger.set_Value_and_Lock(True)
rec.doCalo.set_Value_and_Lock(False)
rec.doMuon.set_Value_and_Lock(False)
rec.doJetMissingETTag.set_Value_and_Lock(False)
rec.doMuonCombined.set_Value_and_Lock(False)
rec.doEgamma.set_Value_and_Lock(False)
rec.doTau.set_Value_and_Lock(False)
rec.doWriteTAG.set_Value_and_Lock(False)
rec.doTagRawSummary.set_Value_and_Lock(False)
rec.doExpressProcessing.set_Value_and_Lock(True)
rec.doHIP.set_Value_and_Lock(False)

#Diable some additional algs which throw warnings
from RecExConfig.RecAlgsFlags import recAlgs
recAlgs.doTrackParticleCellAssociation.set_Value_and_Lock(False)

from ParticleBuilderOptions.AODFlags import AODFlags 
AODFlags.ThinInDetForwardTrackParticles.set_Value_and_Lock(False)

from AthenaMonitoring.DQMonFlags import DQMonFlags
DQMonFlags.enableLumiAccess=False


