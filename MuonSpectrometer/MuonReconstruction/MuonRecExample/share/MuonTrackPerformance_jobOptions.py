from RecExConfig.RecFlags import rec
from MuonRecExample.MuonRecFlags import muonRecFlags
muonRecFlags.setDefaults()

from MuonTrackPerformance.MuonTrackPerformanceConf import MuonTrackPerformanceAlg
from AthenaCommon.CfgGetter import getPublicTool



if muonRecFlags.doStandalone:
  getPublicTool("MuonTrackTruthTool")
  topSequence += MuonTrackPerformanceAlg("MuonStandalonePerformanceAlg",
                                         TrackInputLocation = "MuonSpectrometerTracks",
                                         MuonLocation="",
                                         DoSummary = muonRecFlags.TrackPerfSummaryLevel(),
                                         DoTruth = rec.doTruth(),
                                         DoTrackDebug = muonRecFlags.TrackPerfDebugLevel() )

