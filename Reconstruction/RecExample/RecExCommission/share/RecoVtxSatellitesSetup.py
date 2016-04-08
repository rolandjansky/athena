## preInlcude for Reco_trf.py for satellite bunches reconstruction
## Runs extended |z| tracking with low-pT threshold
## This is the porting to 17.2 of what was used in ATLAS-CONF-2011-049
## Example usage (see also VtxSatellitesD3PD_trf_v1.txt):
## Reco_trf.py maxEvents=-1 autoConfiguration=everything preInclude=PUStudies/VtxSatellitesD3PD_preInclude_v1.py inputBSFile=/eliza18/atlas/spagan/GBStudies/RAW/data12_8TeV.00201351.calibration_VdM.daq.RAW/data12_8TeV.00201351.calibration_VdM.daq.RAW._lb0000._SFO-3._2333.data outputESDFile=myESD.pool.root outputAODFile=myAOD.pool.root outputNTUP_IDVTXLUMIFile=myVTXD3PD.root

#Set general reco settings
from InDetRecExample.InDetJobProperties import InDetFlags
InDetFlags.useBeamConstraint.set_Value_and_Lock(False)
InDetFlags.doLowMuRunSetup.set_Value_and_Lock(True)
InDetFlags.doTrackSegmentsPixel.set_Value_and_Lock(False)
InDetFlags.doTrackSegmentsSCT.set_Value_and_Lock(False)
InDetFlags.doTrackSegmentsTRT.set_Value_and_Lock(False)
InDetFlags.selectSCTIntimeHits=False


#Set algorithms to be run
AODFlags.TrackParticleSlimmer=True

from AthenaCommon.DetFlags import DetFlags
DetFlags.Calo_setOff()
DetFlags.Muon_setOff()
DetFlags.ID_setOn()
DetFlags.TRT_setOff()
DetFlags.detdescr.TRT_setOn()
DetFlags.makeRIO.TRT_setOff()

rec.doTrigger.set_Value_and_Lock(False)
rec.doCalo.set_Value_and_Lock(False)
rec.doMuon.set_Value_and_Lock(False)
rec.doJetMissingETTag.set_Value_and_Lock(False)
rec.doMuonCombined.set_Value_and_Lock(False)
rec.doEgamma.set_Value_and_Lock(False)
rec.doTau.set_Value_and_Lock(False)

from RecExConfig.RecAlgsFlags import recAlgs
recAlgs.doEgammaBremReco.set_Value_and_Lock  (False)

from TrackD3PDMaker.VertexD3PDAnalysisFlags import VertexD3PDAnalysisFlags
VertexD3PDAnalysisFlags.useBeamspot.set_Value_and_Lock(True)

#Change tracking reconstruction cuts (Increase maximum z of tracking accpetance)
from InDetRecExample.ConfiguredNewTrackingCuts import ConfiguredNewTrackingCuts
InDetNewTrackingCuts      = ConfiguredNewTrackingCuts("Offline")
InDetNewTrackingCuts._ConfiguredNewTrackingCuts__maxZImpact = 1700.
InDetNewTrackingCuts.printInfo()

InDetNewTrackingCutsLowPt = ConfiguredNewTrackingCuts("LowPt")
InDetNewTrackingCutsLowPt._ConfiguredNewTrackingCuts__maxZImpact = 1700.
InDetNewTrackingCutsLowPt.printInfo()

#Change vertex reconstruction cuts (Increase maximum z of tracking accpetance)
from InDetRecExample.ConfiguredVertexingCuts import ConfiguredVertexingCuts
InDetPrimaryVertexingCuts = ConfiguredVertexingCuts("LowPt")
InDetPrimaryVertexingCuts._ConfiguredVertexingCuts__z0Max = 1700.
InDetPrimaryVertexingCuts._ConfiguredVertexingCuts__IPz0Max = 1700.
InDetPrimaryVertexingCuts._ConfiguredVertexingCuts__sigIPd0Max = 5.0
InDetPrimaryVertexingCuts.printInfo()

#Setup D3PD making options
InDetFlags.doVertexFindingForMonitoring.set_Value_and_Lock(False)
InDetFlags.doSplitVertexFindingForMonitoring.set_Value_and_Lock(False)

from TrackD3PDMaker.VertexD3PDAnalysisFlags import VertexD3PDAnalysisFlags
VertexD3PDAnalysisFlags.useAllVertexCollections.set_Value_and_Lock(False)
VertexD3PDAnalysisFlags.useBeamspot.set_Value_and_Lock(True)
# Disable tracking info to have small D3PDs
#VertexD3PDAnalysisFlags.useTracks.set_Value_and_Lock(True)
# The following is only available after TrackD3PDMaker-01-03-05
#VertexD3PDAnalysisFlags.filterTightTruth.set_Value_and_Lock(True)
