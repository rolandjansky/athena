"""ComponentAccumulator configuration for Monte Carlo Truth simulation algorithms

Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
"""
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from Digitization.PileUpToolsConfig import PileUpToolsCfg
from Digitization.PileUpMergeSvcConfigNew import PileUpMergeSvcCfg, PileUpXingFolderCfg

# Note: various experimentalDigi uses not migrated

def GenericMergeMcEventCollCfg(flags, name="MergeMcEventCollTool", **kwargs):
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
    tool = CompFactory.MergeMcEventCollTool(name, **kwargs)
    acc.merge(PileUpToolsCfg(flags, PileUpTools=tool))
    return acc


def MergeMcEventCollCfg(flags, name="MergeMcEventCollTool", **kwargs):
    acc = ComponentAccumulator()
    if flags.Digitization.DoXingByXingPileUp: # PileUpTool approach
        kwargs.setdefault("FirstXing", -30000)
        kwargs.setdefault("LastXing",   30000)
    kwargs.setdefault("DoSlimming", False)
    kwargs.setdefault("OnlySaveSignalTruth", False)
    acc.merge(GenericMergeMcEventCollCfg(flags, name, **kwargs))
    return acc


def SignalOnlyMcEventCollCfg(flags, name="SignalOnlyMcEventCollTool", **kwargs):
    acc = ComponentAccumulator()
    if flags.Digitization.DoXingByXingPileUp: # PileUpTool approach
        kwargs.setdefault("FirstXing", 0)
        kwargs.setdefault("LastXing",  0)
    kwargs.setdefault("OnlySaveSignalTruth", True)
    acc.merge(GenericMergeMcEventCollCfg(flags, name, **kwargs))
    return acc


def InTimeOnlyMcEventCollCfg(flags, name="InTimeOnlyMcEventCollTool", **kwargs):
    acc = ComponentAccumulator()
    if flags.Digitization.DoXingByXingPileUp: # PileUpTool approach
        kwargs.setdefault("FirstXing", 0)
        kwargs.setdefault("LastXing",  0)
    kwargs.setdefault("DoSlimming", False)
    kwargs.setdefault("OnlySaveSignalTruth", False)
    acc.merge(GenericMergeMcEventCollCfg(flags, name, **kwargs))
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
    return PileUpXingFolderCfg(flags, name, **kwargs)


def MergeTruthJetsCfg(flags, name="MergeTruthJetsTool", **kwargs):
    acc = ComponentAccumulator()
    rangetool = acc.popToolsAndMerge(TruthJetRangeCfg(flags))
    acc.merge(PileUpMergeSvcCfg(flags, Intervals=rangetool))
    if flags.Digitization.DoXingByXingPileUp: # PileUpTool approach
        kwargs.setdefault("FirstXing", TruthJet_FirstXing())
        kwargs.setdefault("LastXing",  TruthJet_LastXing())
    kwargs.setdefault("InTimeOutputTruthJetCollKey", "InTimeAntiKt4TruthJets")
    kwargs.setdefault("OutOfTimeTruthJetCollKey", "OutOfTimeAntiKt4TruthJets")
    tool = CompFactory.MergeTruthJetsTool(name, **kwargs)
    acc.merge(PileUpToolsCfg(flags, PileUpTools=tool))
    return acc


def MergeTruthJetsFilterCfg(flags, name="MergeTruthJetsFilterTool", **kwargs):
    acc = ComponentAccumulator()
    kwargs.setdefault("ActivateFilter", True)
    acc.merge(MergeTruthJetsCfg(flags, name, **kwargs))
    return acc


# The earliest bunch crossing time for which interactions will be sent
# to the TrackRecordCollection merging code.
def TrackRecord_FirstXing():
    return -1


# The latest bunch crossing time for which interactions will be sent
# to the TrackRecordCollection merging code.
def TrackRecord_LastXing():
    return 1


def TrackRangeCfg(flags, name="TrackRange", **kwargs):
    """Return a Track configured PileUpXingFolder tool"""
    # this is the time of the xing in ns
    kwargs.setdefault("FirstXing", TrackRecord_FirstXing())
    kwargs.setdefault("LastXing",  TrackRecord_LastXing())
    kwargs.setdefault("ItemList", ["TrackRecordCollection#MuonExitLayer"])
    return PileUpXingFolderCfg(flags, name, **kwargs)


def MergeTrackRecordCollCfg(flags, name="MergeTrackRecordCollTool", **kwargs):
    acc = ComponentAccumulator()
    rangetool = acc.popToolsAndMerge(TrackRangeCfg(flags))
    acc.merge(PileUpMergeSvcCfg(flags, Intervals=rangetool))
    if flags.Digitization.DoXingByXingPileUp: # PileUpTool approach
        kwargs.setdefault("FirstXing", TrackRecord_FirstXing())
        kwargs.setdefault("LastXing",  TrackRecord_LastXing())
    tool = CompFactory.MergeTrackRecordCollTool(name, **kwargs)
    acc.merge(PileUpToolsCfg(flags, PileUpTools=tool))
    return acc


def MergeCaloEntryLayerCfg(flags, name="MergeCaloEntryLayerTool", **kwargs):
    acc = ComponentAccumulator()
    kwargs.setdefault("TrackRecordCollKey", "CaloEntryLayer")
    kwargs.setdefault("TrackRecordCollOutputKey", "CaloEntryLayer")
    acc.merge(MergeTrackRecordCollCfg(flags, name, **kwargs))
    return acc


def MergeMuonEntryLayerCfg(flags, name="MergeMuonEntryLayerTool", **kwargs):
    acc = ComponentAccumulator()
    kwargs.setdefault("TrackRecordCollKey", "MuonEntryLayer")
    kwargs.setdefault("TrackRecordCollOutputKey", "MuonEntryLayer")
    acc.merge(MergeTrackRecordCollCfg(flags, name, **kwargs))
    return acc


def MergeMuonExitLayerCfg(flags, name="MergeMuonExitLayerTool", **kwargs):
    acc = ComponentAccumulator()
    kwargs.setdefault("TrackRecordCollKey", "MuonExitLayer")
    kwargs.setdefault("TrackRecordCollOutputKey", "MuonExitLayer")
    acc.merge(MergeTrackRecordCollCfg(flags, name, **kwargs))
    return acc


def MergeHijingParsCfg(flags, name="MergeHijingParsTool", **kwargs):
    acc = ComponentAccumulator()
    if flags.Digitization.DoXingByXingPileUp: # PileUpTool approach
        kwargs.setdefault("FirstXing", -1)
        kwargs.setdefault("LastXing",  +1)
    tool = CompFactory.MergeHijingParsTool(name, **kwargs)
    acc.merge(PileUpToolsCfg(flags, PileUpTools=tool))
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
    return PileUpXingFolderCfg(flags, name, **kwargs)


def MergeCalibHitsCfg(flags, name="MergeCalibHitsTool", **kwargs):
    acc = ComponentAccumulator()
    rangetool = acc.popToolsAndMerge(CalibRangeCfg(flags))
    acc.merge(PileUpMergeSvcCfg(flags, Intervals=rangetool))
    if flags.Digitization.DoXingByXingPileUp: # PileUpTool approach
        kwargs.setdefault("FirstXing", CalibHit_FirstXing())
        kwargs.setdefault("LastXing",  CalibHit_LastXing())
    tool = CompFactory.MergeCalibHitsTool(name, **kwargs)
    acc.merge(PileUpToolsCfg(flags, PileUpTools=tool))
    return acc


# The earliest bunch crossing time for which interactions will be sent
# to the RecoTimingObj merging code.
def TimingObj_FirstXing():
    return -1


# The latest bunch crossing time for which interactions will be sent
# to the RecoTimingObj merging code.
def TimingObj_LastXing():
    return 1


def TimingObjRangeCfg(flags, name="TimingObjRange", **kwargs):
    """Return a RecoTiming configured PileUpXingFolder tool"""
    #this is the time of the xing in ns
    kwargs.setdefault("FirstXing", TimingObj_FirstXing())
    kwargs.setdefault("LastXing",  TimingObj_LastXing())
    kwargs.setdefault("ItemList", ["RecoTimingObj#EVNTtoHITS_timings"])
    return PileUpXingFolderCfg(flags, name, **kwargs)


def MergeRecoTimingObjCfg(flags, name="MergeRecoTimingObjTool", **kwargs):
    acc = ComponentAccumulator()
    rangetool = acc.popToolsAndMerge(TimingObjRangeCfg(flags))
    acc.merge(PileUpMergeSvcCfg(flags, Intervals=rangetool))
    if flags.Digitization.DoXingByXingPileUp: # PileUpTool approach
        kwargs.setdefault("FirstXing", TimingObj_FirstXing())
        kwargs.setdefault("LastXing",  TimingObj_LastXing())
    kwargs.setdefault("RecoTimingObjInputKey", "EVNTtoHITS_timings")
    kwargs.setdefault("RecoTimingObjOutputKey", "EVNTtoHITS_timings")
    acc.setPrivateTools(CompFactory.MergeRecoTimingObjTool(name, **kwargs))
    return acc


def MergeGenericMuonSimHitCollCfg(flags, name="MergeGenericMuonSimHitCollTool", **kwargs):
    acc = ComponentAccumulator()
    tool = CompFactory.MergeGenericMuonSimHitCollTool(name, **kwargs)
    acc.merge(PileUpToolsCfg(flags, PileUpTools=tool))
    return acc


def MergeMicromegasSimHitCollCfg(flags, name="MergeMicromegasSimHitCollTool", **kwargs):
    acc = ComponentAccumulator()
    kwargs.setdefault("SimHitContainerNames", ["MicromegasSensitiveDetector"])
    if flags.Digitization.DoXingByXingPileUp: # PileUpTool approach
        kwargs.setdefault("FirstXing", -250)
        kwargs.setdefault("LastXing",   200)
    tool = CompFactory.MergeGenericMuonSimHitCollTool(name, **kwargs)
    acc.merge(PileUpToolsCfg(flags, PileUpTools=tool))
    return acc


def MergeSTGCSimHitCollCfg(flags, name="MergeSTGCSimHitCollTool", **kwargs):
    acc = ComponentAccumulator()
    kwargs.setdefault("SimHitContainerNames", ["sTGCSensitiveDetector"])
    if flags.Digitization.DoXingByXingPileUp: # PileUpTool approach
        kwargs.setdefault("FirstXing", -50)
        kwargs.setdefault("LastXing",   75)
    tool = CompFactory.MergeGenericMuonSimHitCollTool(name, **kwargs)
    acc.merge(PileUpToolsCfg(flags, PileUpTools=tool))
    return acc
