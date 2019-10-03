from RecExConfig.RecFlags import rec
from AthenaCommon import CfgMgr

from MuonCombinedRecExample.MuonCombinedRecFlags import muonCombinedRecFlags
from MuonCombinedRecExample.MuonCombinedKeys import MuonCombinedKeys as MuonCbKeys
from MuonRecExample.MuonRecFlags import muonRecFlags

muonCombinedRecFlags.setDefaults()

from MuonTrackPerformance.MuonTrackPerformanceConf import MuonTrackPerformanceAlg

if muonCombinedRecFlags.doxAOD():  
  topSequence += MuonTrackPerformanceAlg("ExtrapolatedMuonPerformanceAlg",
                                         TrackInputLocation = "",
                                         DoSummary = muonCombinedRecFlags.TrackPerfSummaryLevel(),
                                         DoTruth = rec.doTruth(),
                                         DoTrackDebug = muonCombinedRecFlags.TrackPerfDebugLevel(),
                                         TrackType = 4,
                                         IsCombined = False )
  topSequence += MuonTrackPerformanceAlg("CombinedMuonPerformanceAlg",
                                         TrackInputLocation = "",
                                         DoSummary = muonCombinedRecFlags.TrackPerfSummaryLevel(),
                                         DoTruth = rec.doTruth(),
                                         DoTrackDebug = muonCombinedRecFlags.TrackPerfDebugLevel(),
                                         TrackType = 3,
                                         IsCombined = True )
  topSequence += MuonTrackPerformanceAlg("MSOnlyExtrapolatedMuonPerformanceAlg",
                                         TrackInputLocation = "",
                                         DoSummary = muonCombinedRecFlags.TrackPerfSummaryLevel(),
                                         DoTruth = rec.doTruth(),
                                         DoTrackDebug = muonCombinedRecFlags.TrackPerfDebugLevel(),
                                         TrackType = 5,
                                         IsCombined = False )

  if muonCombinedRecFlags.doMuGirlLowBeta():
    topSequence += MuonTrackPerformanceAlg("CombinedStauPerformanceAlg",
                                           TrackInputLocation = "",
                                           DoSummary = muonCombinedRecFlags.TrackPerfSummaryLevel(),
                                           DoTruth = rec.doTruth(),
                                           DoTrackDebug = muonCombinedRecFlags.TrackPerfDebugLevel(),
                                           TrackType = 3,
                                           IsCombined = True,
                                           doStau = True)
    #no MuGirlStau extrapolated tracks at the moment
    #topSequence += MuonTrackPerformanceAlg("ExtrapolatedStauPerformanceAlg",
                                           #TrackInputLocation = "ExtrapolatedStauTracks",
                                           #DoSummary = muonCombinedRecFlags.TrackPerfSummaryLevel(),
                                           #DoTruth = rec.doTruth(),
                                           #DoTrackDebug = muonCombinedRecFlags.TrackPerfDebugLevel(),
                                           #IsCombined = False )

  from MuonTrackPerformance.MuonTrackPerformanceConf import MuonSegmentPerformanceAlg
  topSequence += MuonSegmentPerformanceAlg(name="MuonSegmentPerformanceAlg")

  from MuonTrackPerformance.MuonTrackPerformanceConf import MuonPerformanceAlg
  topSequence += MuonPerformanceAlg(name="MuonPerformanceAlg")

