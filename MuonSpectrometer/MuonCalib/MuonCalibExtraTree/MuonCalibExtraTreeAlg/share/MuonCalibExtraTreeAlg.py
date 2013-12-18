from AthenaCommon.AlgSequence import AlgSequence
from AthenaCommon.ConfigurableDb import getConfigurable
from AthenaCommon.AppMgr import ToolSvc

topSequence = AlgSequence()

MuonCalibExtraTreeAlg = getConfigurable('MuonCalib__MuonCalibExtraTreeAlg')()

MuonCalibExtraTreeAlg.NtupleName = "PatternNtupleMaker"

if doMoore:
    MuonCalibExtraTreeAlg.doPhi = True
    MuonCalibExtraTreeAlg.doTracks = True
    MuonCalibExtraTreeAlg.PatternLocation = "COMBINED_PATTERNS"
    MuonCalibExtraTreeAlg.TrackLocation = "ConvertedMooreTracks"
if doMuonboy:
    MuonCalibExtraTreeAlg.doPhi = False
    MuonCalibExtraTreeAlg.doTracks = True
    MuonCalibExtraTreeAlg.PatternLocation = "????"
#    MuonCalibExtraTreeAlg.TrackLocation = "ConvertedMBoyTracks"
    MuonCalibExtraTreeAlg.TrackLocation = "ConvertedMBoyMuonSpectroOnlyTracks"

if doMoMu:
    MuonCalibExtraTreeAlg.doPhi = True
    MuonCalibExtraTreeAlg.doTracks = False
    MuonCalibExtraTreeAlg.PatternLocation = "COMBINED_PATTERNS"
    MuonCalibExtraTreeAlg.TrackLocation = "ConvertedMooreTracks"

ToolSvc.MuonCalib_PatternNtupleMaker.DelayFinish = True

include("MuonTrackThroughCalo/ConfMuonTrackThroughCalo.py");

topSequence += MuonCalibExtraTreeAlg
print MuonCalibExtraTreeAlg

