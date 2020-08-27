
### Temporally disabled to avoid a crash or an ERROR message (start) ###
### Checked with devval rel_6 as of 2014-12-07

rec.doTrigger=False # Otherwise, you will get "Py:TriggerMenu.menu.GenerateMenu   ERROR GenerateMenu: Problems when importing EgammaDef.py or PhotonDef.py, disabling egamma chains."

### Temporally disabled to avoid a crash or an ERROR message (end) ###

# Magnet is on
from AthenaCommon.DetFlags import DetFlags
DetFlags.BField_setOn()
DetFlags.overlay.BField_setOff()

DetFlags.pileup.all_setOff()

include("InDetSLHC_Example/SLHC_Setup_Reco_TrackingGeometry_GMX.py")

from InDetRecExample.InDetJobProperties import InDetFlags
InDetFlags.doSLHC=True
if not InDetFlags.useEtaDependentCuts(): # because InDetEtaDependentCutsTool covers whole eta region
    InDetFlags.doForwardTracks=True

InDetFlags.doSlimming=False
InDetFlags.doBeamHalo=False
InDetFlags.doStatistics=True
InDetFlags.useDCS=False

InDetFlags.doSGDeletion=False # This should be False. 
#InDetFlags.doPixelClusterSplitting=False  # Christoph Rauchegger: Set to true (default) for run2 ambi solving  # Previously: This should be False.
InDetFlags.doLowBetaFinder=False # This should be False. 

# InDetFlags.doStandardPlots=True 
# InDetFlags.doPrintConfigurables = True

if DetFlags.detdescr.Calo_on() and DetFlags.detdescr.SCT_on():
  InDetFlags.doROIConv = True   

from BTagging.BTaggingFlags import BTaggingFlags
BTaggingFlags.CalibrationTag="BTagCalibITk-50_50-02-00"

if rec.OutputFileNameForRecoStep() == 'RAWtoESD':
    InDetFlags.writeRDOs = True

# if rec.OutputFileNameForRecoStep() == 'ESDtoAOD': 
#     InDetFlags.doNewTracking=False
#     InDetFlags.doVertexFinding=False
#     InDetFlags.doPRDFormation=False
#     InDetFlags.doParticleCreation=False
#     InDetFlags.disableInDetReco=False
#     InDetFlags.doStatistics=False
#     InDetFlags.disableTracking=False
#     InDetFlags.doForwardTracks=False
#     InDetFlags.doSGDeletion=True

if rec.OutputFileNameForRecoStep() == 'ESDtoDPD': 
    rec.doDPD = True
    from InDetPrepRawDataToxAOD.InDetDxAODJobProperties import InDetDxAODFlags
    InDetDxAODFlags.DumpUnassociatedHits = False
    InDetDxAODFlags.DumpPixelInfo = True
    InDetDxAODFlags.DumpPixelRdoInfo = True
    InDetDxAODFlags.DumpSctInfo = True
    InDetDxAODFlags.DumpSctRdoInfo = True
    InDetDxAODFlags.ThinHitsOnTrack = False
    InDetDxAODFlags.ThinTrackSelection = "InDetTrackParticles.pt > 0.0*GeV"
    InDetDxAODFlags.DumpTriggerInfo = False
    InDetDxAODFlags.DumpLArCollisionTime = False
