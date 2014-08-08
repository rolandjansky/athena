if (not BTaggingFlags.JetFitterTag) and (not BTaggingFlags.JetFitterNN):

  from AthenaCommon.AppMgr import ToolSvc
  from TrkExTools.AtlasExtrapolator import AtlasExtrapolator
  JetFitterExtrapolator = AtlasExtrapolator()
  
  ToolSvc += JetFitterExtrapolator
  if BTaggingFlags.OutputLevel < 3:
    print JetFitterExtrapolator
    
  # Track Selector for JetFitter
  from InDetTrackSelectorTool.InDetTrackSelectorToolConf import InDet__InDetDetailedTrackSelectorTool
  NewInDetJetFitterTrackSelectorTool = InDet__InDetDetailedTrackSelectorTool(name = "InDetImprovedJetFitterTrackSelectorTool",
                                                                             pTMin = 500.,
                                                                             IPd0Max = 7.,
                                                                             IPz0Max = 10.,
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
  
  ToolSvc += NewInDetJetFitterTrackSelectorTool
  if BTaggingFlags.OutputLevel < 3:
    print NewInDetJetFitterTrackSelectorTool    
      
  #LOAD CONVENTIONAL VERTEXING TOOLS

  from TrkVertexFitterUtils.TrkVertexFitterUtilsConf import Trk__FullLinearizedTrackFactory
  JetFitterLinFactory = Trk__FullLinearizedTrackFactory(name              = "JetFitterFullLinearizedTrackFactory",
                                                      Extrapolator      = JetFitterExtrapolator)
  ToolSvc += JetFitterLinFactory
  if BTaggingFlags.OutputLevel < 3:
    print      JetFitterLinFactory

  from TrkVertexSeedFinderUtils.TrkVertexSeedFinderUtilsConf import Trk__Mode3dTo1dFinder
  mode3dfinder = Trk__Mode3dTo1dFinder(name = "JetFitterMode3dTo1dFinder")
  
  ToolSvc += mode3dfinder
  if BTaggingFlags.OutputLevel < 3:
    print mode3dfinder

  from TrkVertexFitters.TrkVertexFittersConf import Trk__SequentialVertexSmoother
  vertexsmoother = Trk__SequentialVertexSmoother(name = "JetFitterSequentialVertexSmoother")
  
  ToolSvc += vertexsmoother
  if BTaggingFlags.OutputLevel < 3:
    print vertexsmoother

  from TrkVertexFitters.TrkVertexFittersConf import Trk__SequentialVertexFitter
  sequentialVtxFitter = Trk__SequentialVertexFitter(name = "JetFitterSequentialVertexFitter",
                                                    VertexSmoother= vertexsmoother,
                                                    LinearizedTrackFactory=JetFitterLinFactory)

  ToolSvc += sequentialVtxFitter
  if BTaggingFlags.OutputLevel < 3:
    print sequentialVtxFitter


  #LOAD JETFITTER TOOLS

  from TrkJetVxFitter.TrkJetVxFitterConf import Trk__JetFitterInitializationHelper
  NewJetFitterInitializationHelper = Trk__JetFitterInitializationHelper( name = "ImprovedJetFitterInitializationHelper",
                                                                         LinearizedTrackFactory = JetFitterLinFactory,
                                                                         OutputLevel = BTaggingFlags.OutputLevel)
  #                                                                    errphiJetAxis = 0.15,
  #                                                                    erretaJetAxis = 0.15)

  ToolSvc += NewJetFitterInitializationHelper
  if BTaggingFlags.OutputLevel < 3:
    print NewJetFitterInitializationHelper

    from TrkJetVxFitter.TrkJetVxFitterConf import Trk__TrkDistanceFinderNeutralNeutral
    TrkDistanceFinderNeutralNeutral=Trk__TrkDistanceFinderNeutralNeutral("TrkDistanceFinderNeutralNeutral",
                                                                     OutputLevel=BTaggingFlags.OutputLevel)

    ToolSvc+=TrkDistanceFinderNeutralNeutral
    if BTaggingFlags.OutputLevel < 3:
      print TrkDistanceFinderNeutralNeutral

    from TrkJetVxFitter.TrkJetVxFitterConf import Trk__TrkDistanceFinderNeutralCharged
    TrkDistanceFinderNeutralCharged=Trk__TrkDistanceFinderNeutralCharged("TrkDistanceFinderNeutralCharged",
                                                                         OutputLevel=BTaggingFlags.OutputLevel)

    ToolSvc+=TrkDistanceFinderNeutralCharged
    if BTaggingFlags.OutputLevel < 3:
      print TrkDistanceFinderNeutralCharged

    from TrkJetVxFitter.TrkJetVxFitterConf import Trk__JetFitterRoutines
    NewJetFitterRoutines = Trk__JetFitterRoutines( name = "ImprovedJetFitterRoutines",
                                                   JetFitterInitializationHelper = NewJetFitterInitializationHelper,
                                                   BeFast = False,
                                                   maxDRshift = 0.,
                                                   noPrimaryVertexRefit = False,
                                                   JetFitterMinimumDistanceFinder=TrkDistanceFinderNeutralCharged,
                                                   JetFitterMinimumDistanceFinderNeutral=TrkDistanceFinderNeutralNeutral,
                                                   OutputLevel = BTaggingFlags.OutputLevel)
    ToolSvc += NewJetFitterRoutines
    if BTaggingFlags.OutputLevel < 3:
      print NewJetFitterRoutines

#LOAD JETFITTER VERTEX FINDER

    from InDetSecVxFinderTool.InDetSecVxFinderToolConf import InDet__InDetJetFitterUtils
    NewInDetJetFitterUtils = InDet__InDetJetFitterUtils(name = "InDetJetFitterUtils",
                                                        LinearizedTrackFactory = JetFitterLinFactory,
                                                        Extrapolator = JetFitterExtrapolator,
                                                        OutputLevel =BTaggingFlags.OutputLevel)

    ToolSvc += NewInDetJetFitterUtils
    if BTaggingFlags.OutputLevel < 3:
      print NewInDetJetFitterUtils

    if not 'InDetKeys' in dir():
      from InDetRecExample.InDetKeys import InDetKeys
#??    InDetKeys.lock_JobProperties()

from InDetSecVxFinderTool.InDetSecVxFinderToolConf import InDet__InDetImprovedJetFitterVxFinder
NewJetFitterVxFinderFlip = InDet__InDetImprovedJetFitterVxFinder( name = "NewJetFitterVxFinderFlip",
                                                                 JetFitterInitializationHelper = NewJetFitterInitializationHelper,
                                                                 JetFitterRoutines = NewJetFitterRoutines,
                                                                 InDetJetFitterUtils=NewInDetJetFitterUtils,
                                                                 Extrapolator=JetFitterExtrapolator,
                                                                 TrackSelector = NewInDetJetFitterTrackSelectorTool,
                                                                 Mode3dFinder=mode3dfinder,
                                                                 SequentialVertexFitter=sequentialVtxFitter,
                                                                 #VxPrimaryContainer="VxPrimaryCandidateAOD",
                                                                 VxPrimaryContainer=InDetKeys.PrimaryVertices(),
                                                                 OutputLevel = BTaggingFlags.OutputLevel,
                                                                 MaxNumDeleteIterations = 30,
                                                                 VertexProbCut = 0.001,
                                                                 MaxClusteringIterations = 30,
                                                                 VertexClusteringProbabilityCut = 0.005,
                                                                 revertFromPositiveToNegativeTags=True)



ToolSvc += NewJetFitterVxFinderFlip
if BTaggingFlags.OutputLevel < 3:
  print NewJetFitterVxFinderFlip

#JetFitterVxFinder.MaxTracksToFitAtOnce = 3000
#to be added in bugfix version

