from AthenaCommon.AppMgr import ToolSvc

from TrkExTools.AtlasExtrapolator import AtlasExtrapolator
JetFitterExtrapolator = AtlasExtrapolator()

ToolSvc += JetFitterExtrapolator
if BTaggingFlags.OutputLevel < 3:
  print JetFitterExtrapolator

# each tagger needs own instance, can't be shared! (
from InDetTrackSelectorTool.InDetTrackSelectorToolConf import InDet__InDetDetailedTrackSelectorTool
InDetJetFitterTrackSelectorTool = InDet__InDetDetailedTrackSelectorTool(name = "InDetJetFitterTrackSelectorTool",
                                                                        pTMin = 769.2,
                                                                        IPd0Max = 3.5,
                                                                        IPz0Max = 5,
                                                                        sigIPd0Max = 0.35,
                                                                        sigIPz0Max = 2.5,
                                                                        etaMax = 9999.,
                                                                        useTrackSummaryInfo = True,
                                                                        nHitBLayer = 0,
                                                                        nHitPix = 1,
                                                                        nHitSct = 4,
                                                                        nHitSi = 7,
                                                                        nHitTrt = 0,
                                                                        useSharedHitInfo = False,
                                                                        useTrackQualityInfo = True,
                                                                        fitChi2OnNdfMax = 3.5,
                                                                        TrackSummaryTool = None,
                                                                        Extrapolator = JetFitterExtrapolator)
                                                               
ToolSvc +=InDetJetFitterTrackSelectorTool
if BTaggingFlags.OutputLevel < 3:
  print InDetJetFitterTrackSelectorTool    



from TrkVertexFitterUtils.TrkVertexFitterUtilsConf import Trk__FullLinearizedTrackFactory
JetFitterLinFactory = Trk__FullLinearizedTrackFactory(name              = "JetFitterFullLinearizedTrackFactory",
                                                      Extrapolator      = JetFitterExtrapolator)
ToolSvc += JetFitterLinFactory
if BTaggingFlags.OutputLevel < 3:
  print JetFitterLinFactory

from TrkJetVxFitter.TrkJetVxFitterConf import Trk__JetFitterInitializationHelper
JetFitterInitializationHelper = Trk__JetFitterInitializationHelper( name = "JetFitterInitializationHelper",
                                                                    LinearizedTrackFactory = JetFitterLinFactory,
                                                                    OutputLevel = BTaggingFlags.OutputLevel)
ToolSvc += JetFitterInitializationHelper
if BTaggingFlags.OutputLevel < 3:
  print JetFitterInitializationHelper

from TrkJetVxFitter.TrkJetVxFitterConf import Trk__JetFitterRoutines
jetFitterRoutines = Trk__JetFitterRoutines( name = "JetFitterRoutines",
                                            JetFitterInitializationHelper = JetFitterInitializationHelper,
                                            BeFast = False,
                                            maxDRshift = 0.,
                                            noPrimaryVertexRefit = False,
                                            OutputLevel = BTaggingFlags.OutputLevel)
ToolSvc += jetFitterRoutines
if BTaggingFlags.OutputLevel < 3:
  print jetFitterRoutines

from InDetSecVxFinderTool.InDetSecVxFinderToolConf import InDet__InDetJetFitterVxFinder
JetFitterVxFinder = InDet__InDetJetFitterVxFinder( name = "JetFitterVxFinder",
                                                   JetFitterRoutines = jetFitterRoutines,
                                                   OutputLevel = BTaggingFlags.OutputLevel,
                                                   MaxNumDeleteIterations = 30,
                                                   VertexProbCut = 0.01,
                                                   MaxClusteringIterations = 30,
                                                   VertexClusteringProbabilityCut = 0.01,
					           JetFitterInitializationHelper = JetFitterInitializationHelper,
                                                   TrackSelector = InDetJetFitterTrackSelectorTool)
ToolSvc += JetFitterVxFinder
if BTaggingFlags.OutputLevel < 3:
  print JetFitterVxFinder

#JetFitterVxFinder.MaxTracksToFitAtOnce = 10 to be added in bugfix version

