from RecExConfig.RecFlags import rec
from MuonRecExample.MuonRecFlags import muonRecFlags
muonRecFlags.setDefaults()

from MuonTrackPerformance.MuonTrackPerformanceConf import MuonTrackPerformanceAlg
from AthenaCommon.CfgGetter import getPublicTool

getPublicTool("MuonTrackTruthTool")

if muonRecFlags.doMoore:
  topSequence += MuonTrackPerformanceAlg("MoorePerformanceAlg",
                                         TrackInputLocation = "MooreTracks",
                                         DoSummary = muonRecFlags.TrackPerfSummaryLevel(),
                                         DoTruth = rec.doTruth(),
                                         DoTrackDebug = muonRecFlags.TrackPerfDebugLevel() )


if muonRecFlags.doMuonboy:
  topSequence += MuonTrackPerformanceAlg("MuonboyPerformanceAlg",
                                         TrackInputLocation = "ConvertedMBoyMuonSpectroOnlyTracks",
                                         DoSummary = muonRecFlags.TrackPerfSummaryLevel(),
                                         DoTruth = rec.doTruth(),
                                         DoTrackDebug = muonRecFlags.TrackPerfDebugLevel() )


if muonRecFlags.doStandalone:
  topSequence += MuonTrackPerformanceAlg("MuonStandalonePerformanceAlg",
                                         TrackInputLocation = "MuonSpectrometerTracks",
                                         DoSummary = muonRecFlags.TrackPerfSummaryLevel(),
                                         DoTruth = rec.doTruth(),
                                         DoTrackDebug = muonRecFlags.TrackPerfDebugLevel() )

