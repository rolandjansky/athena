"""ComponentAccumulator configuration for Monte Carlo Truth simulation algorithms

Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
"""
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory

# Note: various experimentalDigi uses not migrated

def GenericMergeMcEventCollToolCfg(flags, name="MergeMcEventCollTool", **kwargs):
    acc = ComponentAccumulator()
    kwargs.setdefault("TruthCollOutputKey", "TruthEvent")
    kwargs.setdefault("TruthCollInputKey", "TruthEvent")
    kwargs.setdefault("LowTimeToKeep", -50.5)
    kwargs.setdefault("HighTimeToKeep", 50.5)
    kwargs.setdefault("KeepUnstable", False)
    kwargs.setdefault("AbsEtaMax", 5.0)
    kwargs.setdefault("OutOfTimeAbsEtaMax", 3.0)
    kwargs.setdefault("rRange", 20.0)
    kwargs.setdefault("zRange", 200.0)
    kwargs.setdefault("SaveCavernBackground", True)
    kwargs.setdefault("SaveInTimeMinBias", True)
    kwargs.setdefault("SaveOutOfTimeMinBias", True)
    kwargs.setdefault("SaveRestOfMinBias", False)
    kwargs.setdefault("AddBackgroundCollisionVertices", True)
    kwargs.setdefault("CompressOutputCollection", False)
    acc.setPrivateTools(CompFactory.MergeMcEventCollTool(name, **kwargs))
    return acc


def MergeMcEventCollToolCfg(flags, name="MergeMcEventCollTool", **kwargs):
    acc = ComponentAccumulator()
    if flags.Digitization.DoXingByXingPileUp: # PileUpTool approach
        kwargs.setdefault("FirstXing", -30000)
        kwargs.setdefault("LastXing",   30000)
    kwargs.setdefault("DoSlimming", False)
    kwargs.setdefault("OnlySaveSignalTruth", False)
    tool = acc.popToolsAndMerge(GenericMergeMcEventCollToolCfg(flags, name, **kwargs))
    acc.setPrivateTools(tool)
    return acc


def SignalOnlyMcEventCollToolCfg(flags, name="SignalOnlyMcEventCollTool", **kwargs):
    acc = ComponentAccumulator()
    if flags.Digitization.DoXingByXingPileUp: # PileUpTool approach
        kwargs.setdefault("FirstXing", 0)
        kwargs.setdefault("LastXing",  0)
    kwargs.setdefault("OnlySaveSignalTruth", True)
    tool = acc.popToolsAndMerge(GenericMergeMcEventCollToolCfg(flags, name, **kwargs))
    acc.setPrivateTools(tool)
    return acc


def InTimeOnlyMcEventCollToolCfg(flags, name="InTimeOnlyMcEventCollTool", **kwargs):
    acc = ComponentAccumulator()
    if flags.Digitization.DoXingByXingPileUp: # PileUpTool approach
        kwargs.setdefault("FirstXing", 0)
        kwargs.setdefault("LastXing",  0)
    kwargs.setdefault("DoSlimming", False)
    kwargs.setdefault("OnlySaveSignalTruth", False)
    tool = acc.popToolsAndMerge(GenericMergeMcEventCollToolCfg(flags, name, **kwargs))
    acc.setPrivateTools(tool)
    return acc


# The earliest bunch crossing time for which interactions will be sent
# to the Truth jet merging code.
def TruthJet_FirstXing():
    return -500


# The latest bunch crossing time for which interactions will be sent
# to the Truth jet merging code.
def TruthJet_LastXing():
    return 100


def TruthJetRangeCfg(flags, name="TruthJetRange", **kwargs):
    """Return a Truth-Jet configured PileUpXingFolder tool"""
    #this is the time of the xing in ns
    kwargs.setdefault("FirstXing", TruthJet_FirstXing())
    kwargs.setdefault("LastXing",  TruthJet_LastXing())
    kwargs.setdefault("ItemList", ["JetCollection#InTimeAntiKt4TruthJets",
                                   "JetCollection#OutOfTimeAntiKt4TruthJets"])
    return CompFactory.PileUpXingFolder(name, **kwargs)


def MergeTruthJetsToolCfg(flags, name="MergeTruthJetsTool", **kwargs):
    acc = ComponentAccumulator()
    if flags.Digitization.DoXingByXingPileUp: # PileUpTool approach
        kwargs.setdefault("FirstXing", TruthJet_FirstXing())
        kwargs.setdefault("LastXing",  TruthJet_LastXing())
    kwargs.setdefault("InTimeOutputTruthJetCollKey", "InTimeAntiKt4TruthJets")
    kwargs.setdefault("OutOfTimeTruthJetCollKey", "OutOfTimeAntiKt4TruthJets")
    acc.setPrivateTools(CompFactory.MergeTruthJetsTool(name, **kwargs))
    return acc


def MergeTruthJetsFilterToolCfg(flags, name="MergeTruthJetsFilterTool", **kwargs):
    acc = ComponentAccumulator()
    kwargs.setdefault("ActivateFilter", True)
    tool = acc.popToolsAndMerge(MergeTruthJetsToolCfg(flags, name, **kwargs))
    acc.setPrivateTools(tool)
    return acc


# The earliest bunch crossing time for which interactions will be sent
# to the TrackRecordCollection merging code.
def TrackRecord_FirstXing():
    return -1


# The latest bunch crossing time for which interactions will be sent
# to the TrackRecordCollection merging code.
def TrackRecord_LastXing():
    return 1


def TrackRangeCfg(flags, name="TrackRange" , **kwargs):
    """Return a Track configured PileUpXingFolder tool"""
    # this is the time of the xing in ns
    kwargs.setdefault("FirstXing", TrackRecord_FirstXing())
    kwargs.setdefault("LastXing",  TrackRecord_LastXing())
    kwargs.setdefault("ItemList", ["TrackRecordCollection#MuonExitLayer"])
    return CompFactory.PileUpXingFolder(name, **kwargs)


def MergeTrackRecordCollToolCfg(flags, name="MergeTrackRecordCollTool", **kwargs):
    acc = ComponentAccumulator()
    if flags.Digitization.DoXingByXingPileUp: # PileUpTool approach
        kwargs.setdefault("FirstXing", TrackRecord_FirstXing())
        kwargs.setdefault("LastXing",  TrackRecord_LastXing())
    acc.setPrivateTools(CompFactory.MergeTrackRecordCollTool(name, **kwargs))
    return acc


def MergeCaloEntryLayerToolCfg(flags, name="MergeCaloEntryLayerTool", **kwargs):
    acc = ComponentAccumulator()
    kwargs.setdefault("TrackRecordCollKey", "CaloEntryLayer")
    kwargs.setdefault("TrackRecordCollOutputKey", "CaloEntryLayer")
    tool = acc.popToolsAndMerge(MergeTrackRecordCollToolCfg(flags, name, **kwargs))
    acc.setPrivateTools(tool)
    return acc


def MergeMuonEntryLayerToolCfg(flags, name="MergeMuonEntryLayerTool", **kwargs):
    acc = ComponentAccumulator()
    kwargs.setdefault("TrackRecordCollKey", "MuonEntryLayer")
    kwargs.setdefault("TrackRecordCollOutputKey", "MuonEntryLayer")
    tool = acc.popToolsAndMerge(MergeTrackRecordCollToolCfg(flags, name, **kwargs))
    acc.setPrivateTools(tool)
    return acc


def MergeMuonExitLayerToolCfg(flags, name="MergeMuonExitLayerTool", **kwargs):
    acc = ComponentAccumulator()
    kwargs.setdefault("TrackRecordCollKey", "MuonExitLayer")
    kwargs.setdefault("TrackRecordCollOutputKey", "MuonExitLayer")
    tool = acc.popToolsAndMerge(MergeTrackRecordCollToolCfg(flags, name, **kwargs))
    acc.setPrivateTools(tool)
    return acc


def MergeHijingParsToolCfg(flags, name="MergeHijingParsTool", **kwargs):
    acc = ComponentAccumulator()
    if flags.Digitization.DoXingByXingPileUp: # PileUpTool approach
        kwargs.setdefault("FirstXing", -1)
        kwargs.setdefault("LastXing",  +1)
    acc.setPrivateTools(CompFactory.MergeHijingParsTool(name, **kwargs))
    return acc


# The earliest bunch crossing time for which interactions will be sent
# to the CaloCalibrationHitContainer merging code.
def CalibHit_FirstXing():
    return -1

# The latest bunch crossing time for which interactions will be sent
# to the CaloCalibrationHitContainer merging code.
def CalibHit_LastXing():
    return 1


def CalibRangeCfg(flags, name="CalibRange", **kwargs):
    """Return a Calibration configured PileUpXingFolder tool"""
    # bunch crossing range in ns
    kwargs.setdefault("FirstXing", CalibHit_FirstXing())
    kwargs.setdefault("LastXing",  CalibHit_LastXing())
    ItemList = [
        "CaloCalibrationHitContainer#LArCalibrationHitActive",
        "CaloCalibrationHitContainer#LArCalibrationHitDeadMaterial",
        "CaloCalibrationHitContainer#LArCalibrationHitInactive",
        "CaloCalibrationHitContainer#TileCalibHitActiveCell",
        "CaloCalibrationHitContainer#TileCalibHitInactiveCell",
        "CaloCalibrationHitContainer#TileCalibHitDeadMaterial"
    ]
    kwargs.setdefault("ItemList", ItemList)
    return CompFactory.PileUpXingFolder(name, **kwargs)


def MergeCalibHitsToolCfg(flags, name="MergeCalibHitsTool", **kwargs):
    acc = ComponentAccumulator()
    if flags.Digitization.DoXingByXingPileUp: # PileUpTool approach
        kwargs.setdefault("FirstXing", CalibHit_FirstXing())
        kwargs.setdefault("LastXing",  CalibHit_LastXing())
    acc.setPrivateTools(CompFactory.MergeCalibHitsTool(name, **kwargs))
    return acc


# The earliest bunch crossing time for which interactions will be sent
# to the RecoTimingObj merging code.
def TimingObj_FirstXing():
    return -1


# The latest bunch crossing time for which interactions will be sent
# to the RecoTimingObj merging code.
def TimingObj_LastXing():
    return 1


def TimingObjRangeCfg(flags, name="TimingObjRange" , **kwargs):
    """Return a RecoTiming configured PileUpXingFolder tool"""
    #this is the time of the xing in ns
    kwargs.setdefault("FirstXing", TimingObj_FirstXing())
    kwargs.setdefault("LastXing",  TimingObj_LastXing())
    kwargs.setdefault("ItemList", ["RecoTimingObj#EVNTtoHITS_timings"])
    return CompFactory.PileUpXingFolder(name, **kwargs)


def MergeRecoTimingObjToolCfg(flags, name="MergeRecoTimingObjTool", **kwargs):
    acc = ComponentAccumulator()
    if flags.Digitization.DoXingByXingPileUp: # PileUpTool approach
        kwargs.setdefault("FirstXing", TimingObj_FirstXing())
        kwargs.setdefault("LastXing",  TimingObj_LastXing())

    kwargs.setdefault("RecoTimingObjInputKey", "EVNTtoHITS_timings")
    kwargs.setdefault("RecoTimingObjOutputKey", "EVNTtoHITS_timings")
    acc.setPrivateTools(CompFactory.MergeRecoTimingObjTool(name, **kwargs))
    return acc


def MergeGenericMuonSimHitCollToolCfg(flags, name="MergeGenericMuonSimHitCollTool", **kwargs):
    acc = ComponentAccumulator()
    acc.setPrivateTools(CompFactory.MergeGenericMuonSimHitCollTool(name, **kwargs))
    return acc


def MergeMicromegasSimHitCollToolCfg(flags, name="MergeMicromegasSimHitCollTool", **kwargs):
    acc = ComponentAccumulator()
    kwargs.setdefault("SimHitContainerNames", ["MicromegasSensitiveDetector"])
    if flags.Digitization.DoXingByXingPileUp: # PileUpTool approach
        kwargs.setdefault("FirstXing", -250)
        kwargs.setdefault("LastXing",   200)
    acc.setPrivateTools(CompFactory.MergeGenericMuonSimHitCollTool(name, **kwargs))
    return acc


def MergeSTGCSimHitCollToolCfg(flags, name="MergeSTGCSimHitCollTool", **kwargs):
    acc = ComponentAccumulator()
    kwargs.setdefault("SimHitContainerNames", ["sTGCSensitiveDetector"])
    if flags.Digitization.DoXingByXingPileUp: # PileUpTool approach
        kwargs.setdefault("FirstXing", -50)
        kwargs.setdefault("LastXing",   75)
    acc.setPrivateTools(CompFactory.MergeGenericMuonSimHitCollTool(name, **kwargs))
    return acc

