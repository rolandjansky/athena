# -- try DisplacedVertices by loosening the PriVtx
# -- Mar 3, 2016, Lianyou SHAN@bjIHEP

#myPrint = VERBOSE
from InDetRecExample import TrackingCommon
myPrint = INFO
IncSecVtxMultiSeed = False 

## UNcomment below 9 lines IF define INCLUSIVESECVTX_DEBUG 
## is also UNcomment in InDetIterativeSecVtxFinderTool

if not InDetFlags.doMonitoring()  :
    if  not IncSecVtxMultiSeed :
        if not hasattr(ServiceMgr, 'THistSvc'):
            from GaudiSvc.GaudiSvcConf import THistSvc
            ServiceMgr += THistSvc()

        ServiceMgr.THistSvc.Output += [ "AANT  DATAFILE='IteraSecVtxFinderTool.root' OPT='RECREATE'" ]
    else :
        printfunc (' IncSecVtx : can not fill Xcheck upon MultiSeed mode ')
## use together with INCLUSIVESECVTX_DEBUG

## trackSelector
from InDetTrackSelectionTool.InDetTrackSelectionToolConf import InDet__InDetTrackSelectionTool
InDetTrackSelectorToolRelax = InDet__InDetTrackSelectionTool(name = "DetailedTrackSelectToolRelax",
                                  CutLevel = "NoCut",
                                  minPt = 1000.,
                                  maxD0 = 500.0,
                                  maxZ0 = 1500.,
                                  maxSigmaD0 = -1.0,
                                  maxSigmaZ0SinTheta = -1.0,
                                  maxChiSqperNdf = 5.0, 
                                  maxAbsEta = InDetPrimaryVertexingCuts.etaMax(),
                                  minNInnermostLayerHits = 0,
                                  minNPixelHits = 0,
                                  maxNPixelHoles = 1,
                                  minNSctHits = 2,
                                  minNTrtHits = 0,
                                  minNSiHits = 0,
                                  maxNSiSharedHits = 6,
                                  TrackSummaryTool = InDetTrackSummaryTool,
#                                  OutputLevel              = myPrint,
                                  Extrapolator = InDetExtrapolator   )

ToolSvc += InDetTrackSelectorToolRelax
if (InDetFlags.doPrintConfigurables()):
    printfunc (' InDetTrackSelectorToolRelax : ', InDetTrackSelectorToolRelax)


from InDetSecVtxTrackSelectionTool.InDetSecVtxTrackSelectionToolConf import InDet__InDetSecVtxTrackSelectionTool
InDetSecVtxTrackSelectorTool = InDet__InDetSecVtxTrackSelectionTool( name = "SecVtxTrackSelector" ,
                                              OutputLevel              = myPrint,
                                              minNPixelHitsAtZeroTRT = 2,
                                              minTotalHits = 0,
                                              minD0 = 2.0   )
ToolSvc += InDetSecVtxTrackSelectorTool
if (InDetFlags.doPrintConfigurables()):
    printfunc (' InDetSecVtxTrackSelectorTool : ', InDetSecVtxTrackSelectorTool)

def getFullLinearizedTrackFactoryIncSecVtx() :
    return TrackingCommon.getInDetFullLinearizedTrackFactory('FullLinearizedTrackFactoryIncSecVtx')

## two options for seedFinder
if IncSecVtxMultiSeed :

#    from TrkVertexFitterUtils.TrkVertexFitterUtilsConf import Trk__FullLinearizedTrackFactory
#    InDetLinFactory = Trk__FullLinearizedTrackFactory( name        = "FullLinearizedTrackFactoryIncSecVtx",
#                                                       Extrapolator      = InDetExtrapolator )
#    ToolSvc += InDetLinFactory
#    if (InDetFlags.doPrintConfigurables()):
#      print InDetLinFactory


    if ( not ( InDetFlags.primaryVertexSetup() == 'MedImgMultiFinding' ) ) :

        from TrkVertexSeedFinderUtils.TrkVertexSeedFinderUtilsConf import Trk__LocalMax1DClusterFinder
        InDetMedImgClusterFinder = Trk__LocalMax1DClusterFinder( name            = "InDetMedImgClusterFinder",
                                                                 weightThreshold = 1500.0, 	
                                                                 mergeParameter  = 0.95,
                                                                 clusterWindowXY = 0.34,
                                                                 refineZ         = True,
                                                                 gaussianWindow  = True)
        ToolSvc += InDetMedImgClusterFinder
        if (InDetFlags.doPrintConfigurables()):
            printfunc (' InDetMedImgClusterFinder ' , InDetMedImgClusterFinder)
## MedImgClusterFinder has been set as ImagingSeedFinder if primaryVertexSetup() == 'MedImgMultiFinding'

    from TrkVertexSeedFinderUtils.TrkVertexSeedFinderUtilsConf import Trk__VertexImageMaker
    SecVtxInDetMedImgMaker = Trk__VertexImageMaker( name                           = "InDetMedImgMaker",
                                              xbins                          = 32*4,
                                              ybins                          = 32*4,
                                              zbins                          = 2048*2,
                                              xrange                         = 500.0,
                                              yrange                         = 500.0,
                                              zrange                         = 800.0,
                                              cutoffFreqDenominator_xy       = 2,
                                              cutoffFreqDenominator_z        = 1,
                                              angularCutoffParameter         = 0.75)
    ToolSvc += SecVtxInDetMedImgMaker
    if (InDetFlags.doPrintConfigurables()):
      printfunc (' SecVtxInDetMedImgMaker ', SecVtxInDetMedImgMaker)

    from TrkVertexSeedFinderTools.TrkVertexSeedFinderToolsConf import Trk__ImagingSeedFinder
    InDetIncSecVtxSeedFinder = Trk__ImagingSeedFinder( name             = "InDetSecVtxMedImgSeedFinder",
                                                     VertexCluster    = InDetMedImgClusterFinder,
                                                     VertexImageMaker = SecVtxInDetMedImgMaker )

    if (InDetFlags.doPrintConfigurables()):
      printfunc ('InDetIncSecVtxSeedFinder', InDetIncSecVtxSeedFinder)

## InDetVtxSeedFinder has been set as ImagingSeedFinder if primaryVertexSetup() == 'MedImgMultiFinding'
else :
  from TrkVertexSeedFinderUtils.TrkVertexSeedFinderUtilsConf import Trk__Mode3dFromFsmw1dFinder
  Mode3dFromFsmw1dFinder = Trk__Mode3dFromFsmw1dFinder ( name = "Mode3dFromFsmw1dFinder", 
                                                         OutputLevel              = myPrint,
#                                                         OutputLevel              = DEBUG,
                                                         Fraction                 = 0.5 ,
                                                         MinimalDistanceFromZtoXY = 0.25,
                                                         MinimalRadiusFromBeam = 1.5 ,
                                                         UseBroadenModes  = True
                                                       ) 
  ToolSvc+=Mode3dFromFsmw1dFinder
  if (InDetFlags.doPrintConfigurables()):
      printfunc ( ' Mode3dFromFsmw1dFinder ', Mode3dFromFsmw1dFinder )

  from TrkVertexSeedFinderUtils.TrkVertexSeedFinderUtilsConf import Trk__SeedNewtonTrkDistanceFinder
  NewtonTrk2DDistanceFinder = Trk__SeedNewtonTrkDistanceFinder(name  = "NewtonTrk2DDistanceFinder" )
  ToolSvc+=NewtonTrk2DDistanceFinder
  if (InDetFlags.doPrintConfigurables()):
      printfunc (' NewtonTrk2DDistanceFinder ', NewtonTrk2DDistanceFinder)

  from TrkVertexSeedFinderTools.TrkVertexSeedFinderToolsConf import Trk__IndexedCrossDistancesSeedFinder
  InDetIncSecVtxSeedFinder = Trk__IndexedCrossDistancesSeedFinder(name   = "IncSecVtxCrossDistancesSeedFinder",
                                                     OutputLevel              = myPrint,
                                                     Mode3dFinder             = Mode3dFromFsmw1dFinder ,
                                                     TrkDistanceFinder   = NewtonTrk2DDistanceFinder,
                                                     trackdistcutoff     = 0.01 ,
                                                     trackdistexppower   = 2,
                                                     maximumTracksNoCut  = 30,
                                                     maximumDistanceCut  = 7.5,
                                                     useweights          = True
                                                         )

## Now turn to the vertex fitter
ToolSvc += InDetIncSecVtxSeedFinder
if (InDetFlags.doPrintConfigurables()):
    printfunc (InDetIncSecVtxSeedFinder)
          
from TrkVertexFitterUtils.TrkVertexFitterUtilsConf import Trk__KalmanVertexUpdator
IncSecVertexUpdator = Trk__KalmanVertexUpdator()
ToolSvc += IncSecVertexUpdator
if (InDetFlags.doPrintConfigurables()):
  printfunc (' IncSecVertexUpdator ', IncSecVertexUpdator)

if rec.readAOD():
    from TrkVertexFitterUtils.TrkVertexFitterUtilsConf import Trk__ImpactPoint3dEstimator
    InDetImpactPoint3dEstimator = Trk__ImpactPoint3dEstimator(name              = "InDetImpactPoint3dEstimator",
                                                               Extrapolator      = InDetExtrapolator)
    ToolSvc += InDetImpactPoint3dEstimator
    if (InDetFlags.doPrintConfigurables()):
        printfunc (InDetImpactPoint3dEstimator)


    from TrkVertexFitterUtils.TrkVertexFitterUtilsConf import Trk__DetAnnealingMaker
    InDetAnnealingMaker = Trk__DetAnnealingMaker(name = "InDetAnnealingMaker",
                                                 SetOfTemperatures = [64.,16.,4.,2.,1.5,1.]) # not default
    ToolSvc += InDetAnnealingMaker
    if (InDetFlags.doPrintConfigurables()):
       printfunc (InDetAnnealingMaker)

    from TrkVertexFitters.TrkVertexFittersConf import Trk__SequentialVertexSmoother
    InDetVertexSmoother = Trk__SequentialVertexSmoother(name = "InDetSequentialVertexSmoother")
    ToolSvc += InDetVertexSmoother
    if (InDetFlags.doPrintConfigurables()):
       printfunc (InDetVertexSmoother)


from TrkVertexFitters.TrkVertexFittersConf import Trk__AdaptiveVertexFitter
InDetIncSecVtxFitterTool = Trk__AdaptiveVertexFitter(name                = "AdaptiveVxFitterToolIncSecVtx",
                                              SeedFinder                   = InDetIncSecVtxSeedFinder,
                                              MaxIterations                = 8000,
                                              MaxDistToLinPoint            = 0.2,
                                              InitialError                 = 0.2,
                                              LinearizedTrackFactory       = getFullLinearizedTrackFactoryIncSecVtx(),
                                              ImpactPoint3dEstimator       = InDetImpactPoint3dEstimator,
                                              VertexUpdator                = IncSecVertexUpdator, 
                                              AnnealingMaker               = InDetAnnealingMaker,
                                              DoSmoothing                  = True,
                                              VertexSmoother               = InDetVertexSmoother,
                                              OutputLevel                  = myPrint)               
ToolSvc += InDetIncSecVtxFitterTool
if (InDetFlags.doPrintConfigurables()):
    printfunc (InDetIncSecVtxFitterTool                              )

from InDetIncSecVxFinderTool.InDetIncSecVxFinderToolConf import InDet__InDetIterativeSecVtxFinderTool
InDetSecVtxFinderTool = InDet__InDetIterativeSecVtxFinderTool(name  = "InDetSecVtxFinderTool",
                                    VertexFitterTool         = InDetIncSecVtxFitterTool,
                                    BaseTrackSelector            = InDetTrackSelectorToolRelax,
                                    SecVtxTrackSelector            = InDetSecVtxTrackSelectorTool,
                                    SeedFinder               = InDetIncSecVtxSeedFinder,
                                    ImpactPoint3dEstimator   = InDetImpactPoint3dEstimator,
                                    LinearizedTrackFactory   = getFullLinearizedTrackFactoryIncSecVtx(),
                                    useBeamConstraint        = False,
#                                    significanceCutSeeding   = 15.,
                                    significanceCutSeeding   = 9.,
# need some tuning ...
                                    maxCompatibilityCutSeeding = 18.,
                                    minTrackWeightAtVtx        = 0.02 ,
                                    maxVertices              = 20,
                                    TrackInnerOuterFraction  = 0.95 ,
                                    doMaxTracksCut           = InDetPrimaryVertexingCuts.doMaxTracksCut(),
                                    VertexFilterLevel        = 5 ,
#                                    VertexFilterLevel        = 0 ,
                                    MomentumProjectionOnDirection = -999.9 ,
                                    SeedsMinimumDistance     = 0.1 ,
                                    OutputLevel              = myPrint,                        
#                                    OutputLevel              = DEBUG,                        
                                    MaxTracks                = InDetPrimaryVertexingCuts.MaxTracks()  )

ToolSvc += InDetSecVtxFinderTool

if (InDetFlags.doPrintConfigurables()):
  printfunc (' InDetSecVtxFinderTool : ', InDetSecVtxFinderTool )

from TrkVertexTools.TrkVertexToolsConf import Trk__SecVertexMergingTool
SecVertexMergingTool = Trk__SecVertexMergingTool( name   = "IncSecMergeTool",
                                                  VertexFitterTool = InDetIncSecVtxFitterTool,
#                                                  OutputLevel              = DEBUG,
                                                  MininumDistance = 5. ,
                                                  CompatibilityDimension = 2 
                                                )
ToolSvc += SecVertexMergingTool
if (InDetFlags.doPrintConfigurables()):
  printfunc (' SecVertexMergingTool : ', SecVertexMergingTool)

from InDetInclusiveSecVtx.InDetInclusiveSecVtxConf import InDet__InDetSecVtxFinder
InDetIncSecVtxFinder = InDet__InDetSecVtxFinder(name             = "InDetIncSecVtxFinder",
                                    VertexFinderTool            = InDetSecVtxFinderTool,
                                    TracksName                  = InDetKeys.xAODTrackParticleContainer(),
                                    VxCandidatesOutputName      = "InclusiveSecVtx",
                                    doVertexMerging             = True,
                                    VertexMergingTool           = SecVertexMergingTool,
                                    OutputLevel                 = myPrint)

topSequence += InDetIncSecVtxFinder
if InDetFlags.doPrintConfigurables():
    printfunc (' InDetIncSecVtxFinder : ' , InDetIncSecVtxFinder)

