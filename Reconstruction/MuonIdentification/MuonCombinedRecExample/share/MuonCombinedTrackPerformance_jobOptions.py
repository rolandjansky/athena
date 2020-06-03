from RecExConfig.RecFlags import rec
from AthenaCommon import CfgMgr

from MuonCombinedRecExample.MuonCombinedRecFlags import muonCombinedRecFlags
from MuonCombinedRecExample.MuonCombinedKeys import MuonCombinedKeys as MuonCbKeys

muonCombinedRecFlags.setDefaults()

from MuonTrackPerformance.MuonTrackPerformanceConf import MuonTrackPerformanceAlg

if muonCombinedRecFlags.doxAOD():  
  topSequence += MuonTrackPerformanceAlg("ExtrapolatedMuonPerformanceAlg",
                                         TrackInputLocation = "ExtrapolatedMuonTracks",
                                         DoSummary = muonCombinedRecFlags.TrackPerfSummaryLevel(),
                                         DoTruth = rec.doTruth(),
                                         DoTrackDebug = muonCombinedRecFlags.TrackPerfDebugLevel(),
                                         IsCombined = False )
  topSequence += MuonTrackPerformanceAlg("CombinedMuonPerformanceAlg",
                                         TrackInputLocation = "CombinedMuonTracks",
                                         DoSummary = muonCombinedRecFlags.TrackPerfSummaryLevel(),
                                         DoTruth = rec.doTruth(),
                                         DoTrackDebug = muonCombinedRecFlags.TrackPerfDebugLevel(),
                                         IsCombined = True )
  topSequence += MuonTrackPerformanceAlg("MSOnlyExtrapolatedMuonPerformanceAlg",
                                         TrackInputLocation = "MSOnlyExtrapolatedMuonTracks",
                                         DoSummary = muonCombinedRecFlags.TrackPerfSummaryLevel(),
                                         DoTruth = rec.doTruth(),
                                         DoTrackDebug = muonCombinedRecFlags.TrackPerfDebugLevel(),
                                         IsCombined = False )

  if muonCombinedRecFlags.doMuGirlLowBeta():
    topSequence += MuonTrackPerformanceAlg("CombinedStauPerformanceAlg",
                                           TrackInputLocation = "CombinedStauTracks",
                                           DoSummary = muonCombinedRecFlags.TrackPerfSummaryLevel(),
                                           DoTruth = rec.doTruth(),
                                           DoTrackDebug = muonCombinedRecFlags.TrackPerfDebugLevel(),
                                           IsCombined = True )
    topSequence += MuonTrackPerformanceAlg("ExtrapolatedStauPerformanceAlg",
                                           TrackInputLocation = "ExtrapolatedStauTracks",
                                           DoSummary = muonCombinedRecFlags.TrackPerfSummaryLevel(),
                                           DoTruth = rec.doTruth(),
                                           DoTrackDebug = muonCombinedRecFlags.TrackPerfDebugLevel(),
                                           IsCombined = False )


  from MuonTrackPerformance.MuonTrackPerformanceConf import MuonSegmentPerformanceAlg
  topSequence += MuonSegmentPerformanceAlg(name="MuonSegmentPerformanceAlg")

  from MuonTrackPerformance.MuonTrackPerformanceConf import MuonPerformanceAlg
  topSequence += MuonPerformanceAlg(name="MuonPerformanceAlg")

