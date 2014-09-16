###############################################################
#
# Set Flags for Running the Trigger in readBS Mode
# includes Trigger_topOptions.py to do the real work.
# Reads from file RawEvent.re (file must already exits
# run athena with TrigWriteBS_Flags.py to produce it)
# 
# use:
# athena.py -bs testIDreadBS_Flags.py
#==============================================================

# file produced by re-running LVL1 using offline release 12.
from RecExConfig.RecFlags import rec
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags as acf

acf.BSRDOInput=["/afs/cern.ch/atlas/project/trigger/pesa-sw/releases/data/top_CSC-01-02-00_RDO_extract_EventStorage.data"]

onlyID=True
doTrigger=True
# Set detector description
DetDescrVersion = "ATLAS-CSC-01-02-00"

rec.doCBNT=False
rec.doWriteAOD=False
rec.doWriteESD=False
rec.doWriteTAG=False
rec.doAOD=False # make AOD objects
rec.doESD.set_Value_and_Lock(False) # make ESD objects
doTAG=False

useROOTNtuple=False # False : hbook ntuple

# Set Truth flags
rec.doTruth=False    # needs to be true if creating Fake RoI 

include ("TriggerRelease/TriggerFlags.py")
# only do test for Egamma Slice
TriggerFlags.TauSlice.unsetAll()
TriggerFlags.MuonSlice.unsetAll()

TriggerFlags.doHLTpersistency=False
TriggerFlags.fakeLVL1=False
TriggerFlags.doLVL1=False
TriggerFlags.readLVL1Calo=True
TriggerFlags.doNtuple=True
TriggerFlags.useOfflineSpacePoints=False
#TriggerFlags.doEF=False
TriggerFlags.readLVL1configFromXML=True
TriggerFlags.inputLVL1menuFile = "LVL1triggermenuCSC-02.xml"
TriggerFlags.inputLVL1thresholdFile = "LVL1triggerthresholdsCSC-02.xml"
TriggerFlags.readHLTconfigFromXML=False

TriggerFlags.EgammaSlice.setFlag("doTRTxK")

#-------------------------------------------------------------
# End of setting flags
#-------------------------------------------------------------

include( "TriggerRelease/TrigReadBS_topOptions.py" )


    
MessageSvc.OutputLevel = INFO
#NovaCnvSvc.OutputLevel = INFO
MessageSvc.debugLimit = 10000000

# Number of events to be processed 
theApp.EvtMax = 10

Algorithm("ToolSvc.OnlineSpacePointProviderTool").OutputLevel=DEBUG 
Algorithm("Lvl1Conversion_L2").OutputLevel=DEBUG
Algorithm("FakeLvl1RoIfromKine_L2").OutputLevel=DEBUG
Algorithm("StepController_L2").OutputLevel=DEBUG
Algorithm("StepSequencer_L2").OutputLevel=DEBUG
Algorithm("StepDecision_L2").OutputLevel=DEBUG

if (TriggerFlags.EgammaSlice.doL2()):
    if TriggerFlags.EgammaSlice.testFlag("doIDSCAN"):
        Algorithm(idscanEgamma.instanceName()).OutputLevel=DEBUG
    if TriggerFlags.EgammaSlice.testFlag("doSiTrack"):
        Algorithm(sitrackEgamma.instanceName()).OutputLevel=DEBUG
    if TriggerFlags.EgammaSlice.testFlag("doTRTxK"):
        Algorithm(trtxkEgammaTRTxK.instanceName()).OutputLevel=DEBUG

#set EF ID algos OutputLevel:
if TriggerFlags.EgammaSlice.doEF():
    Algorithm(TrigSCT_ClusteringEG.instanceName()).OutputLevel=DEBUG
    Algorithm(TrigPixelClusteringEG.instanceName()).OutputLevel=DEBUG
    Algorithm(TrigTRT_DriftCircleMakerEG.instanceName()).OutputLevel=DEBUG
    Algorithm(TrigSiSpacePointFinderEG.instanceName()).OutputLevel=DEBUG
    Algorithm(TrigSiTrackFinderEG.instanceName()).OutputLevel=DEBUG
    Algorithm(TrigInDetAmbiguitySolverEG.instanceName()).OutputLevel=DEBUG
    Algorithm(TrigTRT_TrackExtensionEG.instanceName()).OutputLevel=DEBUG
    Algorithm(TrigInDetExtensProcessorEG.instanceName()).OutputLevel=DEBUG
    Algorithm(TrigInDetVxPrimaryEG.instanceName()).OutputLevel=DEBUG
    Algorithm(TrigInDetParticleCreatorEG.instanceName()).OutputLevel=DEBUG

# For BS files produced with DetDescrVersion "ATLAS-DC3-01" or "ATLAS-DC3-02"
# uncomment the following line (but don't do that if using a file produced
# by re-running LVL1 with an offline release 12 or higher).
# Algorithm("Lvl1Conversion_L2").useOldRoIWordFormat=True # fix to read data with old RoiWord format
