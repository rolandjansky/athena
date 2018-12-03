# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from Digitization.DigitizationFlags import digitizationFlags
from AthenaCommon import CfgMgr

############################################################################

def genericMergeMcEventCollTool(name="MergeMcEventCollTool", **kwargs):
    kwargs.setdefault("TruthCollKey", "TruthEvent")
    kwargs.setdefault("LowTimeToKeep", -50.5)
    kwargs.setdefault("HighTimeToKeep", 50.5)
    kwargs.setdefault("KeepUnstable", False)
    kwargs.setdefault("AbsEtaMax", 5.0)
    kwargs.setdefault("OutOfTimeAbsEtaMax", 3.0)
    kwargs.setdefault("rRange", 20.0)
    kwargs.setdefault("zRange", 200.0)
    #kwargs.setdefault("ptMin", 0.4*GeV)
    #kwargs.setdefault("EKinMin", 1.0*MeV)
    kwargs.setdefault("SaveCavernBackground", True)
    kwargs.setdefault("SaveInTimeMinBias", True)
    kwargs.setdefault("SaveOutOfTimeMinBias", True)
    kwargs.setdefault("SaveRestOfMinBias", False)
    kwargs.setdefault("AddBackgroundCollisionVertices", True)
    kwargs.setdefault("CompressOutputCollection", False)

    return CfgMgr.MergeMcEventCollTool(name, **kwargs)

def MergeMcEventCollTool(name="MergeMcEventCollTool", **kwargs):
    if digitizationFlags.doXingByXingPileUp(): # PileUpTool approach
        kwargs.setdefault("FirstXing", -30000)
        kwargs.setdefault("LastXing",   30000)
    kwargs.setdefault("DoSlimming", True)
    kwargs.setdefault("OnlySaveSignalTruth", False)
    return genericMergeMcEventCollTool(name, **kwargs)

def SignalOnlyMcEventCollTool(name="SignalOnlyMcEventCollTool", **kwargs):
    if digitizationFlags.doXingByXingPileUp(): # PileUpTool approach
        kwargs.setdefault("FirstXing", 0)
        kwargs.setdefault("LastXing",  0)
    kwargs.setdefault("OnlySaveSignalTruth", True)
    return genericMergeMcEventCollTool(name, **kwargs)

def InTimeOnlyMcEventCollTool(name="InTimeOnlyMcEventCollTool", **kwargs):
    if digitizationFlags.doXingByXingPileUp(): # PileUpTool approach
        kwargs.setdefault("FirstXing", 0)
        kwargs.setdefault("LastXing",  0)
    kwargs.setdefault("DoSlimming", False)
    kwargs.setdefault("OnlySaveSignalTruth", False)
    return genericMergeMcEventCollTool(name, **kwargs)

############################################################################

# The earliest bunch crossing time for which interactions will be sent
# to the Truth jet merging code.
def TruthJet_FirstXing():
    return -500

# The latest bunch crossing time for which interactions will be sent
# to the Truth jet merging code.
def TruthJet_LastXing():
    return 100

def getTruthJetRange(name="TruthJetRange", **kwargs):
    #this is the time of the xing in ns
    kwargs.setdefault('FirstXing', TruthJet_FirstXing() )
    kwargs.setdefault('LastXing',  TruthJet_LastXing() )
    kwargs.setdefault('ItemList', ["JetCollection#InTimeAntiKt4TruthJets",
                                   "JetCollection#OutOfTimeAntiKt4TruthJets"] )
    return CfgMgr.PileUpXingFolder(name, **kwargs)


def getMergeTruthJetsTool(name="MergeTruthJetsTool", **kwargs):
    if digitizationFlags.doXingByXingPileUp(): # PileUpTool approach
        kwargs.setdefault("FirstXing", TruthJet_FirstXing() )
        kwargs.setdefault("LastXing",  TruthJet_LastXing() )
    return CfgMgr.MergeTruthJetsTool(name, **kwargs)

def getMergeTruthJetsFilterTool(name="MergeTruthJetsFilterTool", **kwargs):
    kwargs.setdefault("ActivateFilter", True )
    return getMergeTruthJetsTool(name, **kwargs)


############################################################################

# The earliest bunch crossing time for which interactions will be sent
# to the TrackRecordCollection merging code.
def TrackRecord_FirstXing():
    return -1

# The latest bunch crossing time for which interactions will be sent
# to the Truth jet merging code.
def TrackRecord_LastXing():
    return 1

def getTrackRange(name="TrackRange" , **kwargs):
    #this is the time of the xing in ns
    kwargs.setdefault('FirstXing', TrackRecord_FirstXing() )
    kwargs.setdefault('LastXing',  TrackRecord_LastXing() )
    kwargs.setdefault('ItemList', ["TrackRecordCollection#MuonExitLayer"] )
    #"TrackRecordCollection#MuonEntryLayer",
    #"TrackRecordCollection#CaloEntryLayer"
    return CfgMgr.PileUpXingFolder(name, **kwargs)


def MergeTrackRecordCollTool(name="MergeTrackRecordCollTool", **kwargs):
    if digitizationFlags.doXingByXingPileUp(): # PileUpTool approach
        kwargs.setdefault("FirstXing", TrackRecord_FirstXing() )
        kwargs.setdefault("LastXing",  TrackRecord_LastXing() )

    return CfgMgr.MergeTrackRecordCollTool(name, **kwargs)

def MergeCaloEntryLayerTool(name="MergeCaloEntryLayerTool", **kwargs):
    kwargs.setdefault("TrackRecordCollKey", "CaloEntryLayer" )
    return MergeTrackRecordCollTool(name, **kwargs)

def MergeMuonEntryLayerTool(name="MergeMuonEntryLayerTool", **kwargs):
    kwargs.setdefault("TrackRecordCollKey", "MuonEntryLayer" )
    return MergeTrackRecordCollTool(name, **kwargs)

def MergeMuonExitLayerTool(name="MergeMuonExitLayerTool", **kwargs):
    kwargs.setdefault("TrackRecordCollKey", "MuonExitLayer" )
    return MergeTrackRecordCollTool(name, **kwargs)


############################################################################
def MergeHijingParsTool(name="MergeHijingParsTool", **kwargs):
    if digitizationFlags.doXingByXingPileUp(): # PileUpTool approach
        kwargs.setdefault("FirstXing", -1)
        kwargs.setdefault("LastXing",  +1)

    return CfgMgr.MergeHijingParsTool(name, **kwargs)


############################################################################

# The earliest bunch crossing time for which interactions will be sent
# to the CaloCalibrationHitContainer merging code.
def CalibHit_FirstXing():
    return -1

# The latest bunch crossing time for which interactions will be sent
# to the Truth jet merging code.
def CalibHit_LastXing():
    return 1

def getCalibRange(name="CalibRange", **kwargs):
    # bunch crossing range in ns
    kwargs.setdefault('FirstXing', CalibHit_FirstXing() )
    kwargs.setdefault('LastXing',  CalibHit_LastXing() )
    ItemList = []
    ItemList += ["CaloCalibrationHitContainer#LArCalibrationHitActive",
                 "CaloCalibrationHitContainer#LArCalibrationHitDeadMaterial",
                 "CaloCalibrationHitContainer#LArCalibrationHitInactive" ]
    from Digitization.DigitizationFlags import digitizationFlags
    if 'OldTileCalibHitContainers' in digitizationFlags.experimentalDigi():
        ItemList += ["CaloCalibrationHitContainer#TileCalibrationCellHitCnt",
                     "CaloCalibrationHitContainer#TileCalibrationDMHitCnt" ]
    else:
        ItemList += ["CaloCalibrationHitContainer#TileCalibHitActiveCell",
                     "CaloCalibrationHitContainer#TileCalibHitInactiveCell",
                     "CaloCalibrationHitContainer#TileCalibHitDeadMaterial" ]
    kwargs.setdefault('ItemList', ItemList)
    return CfgMgr.PileUpXingFolder(name, **kwargs)

def MergeCalibHitsTool(name="MergeCalibHitsTool", **kwargs):
    if digitizationFlags.doXingByXingPileUp(): # PileUpTool approach
        kwargs.setdefault("FirstXing", CalibHit_FirstXing() )
        kwargs.setdefault("LastXing",  CalibHit_LastXing() )
    if 'OldTileCalibHitContainers' in digitizationFlags.experimentalDigi():
        kwargs.setdefault("OldFormat", True)

    return CfgMgr.MergeCalibHitsTool(name, **kwargs)


############################################################################

# The earliest bunch crossing time for which interactions will be sent
# to the RecoTimingObj merging code.
def TimingObj_FirstXing():
    return -1

# The latest bunch crossing time for which interactions will be sent
# to the RecoTimingObj merging code.
def TimingObj_LastXing():
    return 1

def getTimingObjRange(name="TimingObjRange" , **kwargs):
    #this is the time of the xing in ns
    kwargs.setdefault('FirstXing', TimingObj_FirstXing() )
    kwargs.setdefault('LastXing',  TimingObj_LastXing() )
    kwargs.setdefault('ItemList', ["RecoTimingObj#EVNTtoHITS_timings"] )
    return CfgMgr.PileUpXingFolder(name, **kwargs)


def MergeRecoTimingObjTool(name="MergeRecoTimingObjTool", **kwargs):
    if digitizationFlags.doXingByXingPileUp(): # PileUpTool approach
        kwargs.setdefault("FirstXing", TimingObj_FirstXing() )
        kwargs.setdefault("LastXing",  TimingObj_LastXing() )

    return CfgMgr.MergeRecoTimingObjTool(name, **kwargs)


############################################################################
def MergeGenericMuonSimHitCollTool(name="MergeGenericMuonSimHitCollTool", **kwargs):
    return CfgMgr.MergeGenericMuonSimHitCollTool(name, **kwargs)

def MergeMicromegasSimHitCollTool(name="MergeMicromegasSimHitCollTool", **kwargs):
    kwargs.setdefault("SimHitContainerNames", ["MicromegasSensitiveDetector"])
    if digitizationFlags.doXingByXingPileUp(): # PileUpTool approach
        kwargs.setdefault("FirstXing", -250)
        kwargs.setdefault("LastXing",   200)
    return CfgMgr.MergeGenericMuonSimHitCollTool(name, **kwargs)

def MergeSTGCSimHitCollTool(name="MergeSTGCSimHitCollTool", **kwargs):
    kwargs.setdefault("SimHitContainerNames", ["sTGCSensitiveDetector"])
    if digitizationFlags.doXingByXingPileUp(): # PileUpTool approach
        kwargs.setdefault("FirstXing", -50)
        kwargs.setdefault("LastXing",   75)
    return CfgMgr.MergeGenericMuonSimHitCollTool(name, **kwargs)

############################################################################
