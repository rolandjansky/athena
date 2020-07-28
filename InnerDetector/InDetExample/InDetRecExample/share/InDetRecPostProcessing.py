# ------------------------------------------------------------
#
# ----------- now we do post-processing
#
# ------------------------------------------------------------

if InDetFlags.doTRTPhaseCalculation() and jobproperties.Beam.beamType()=="collisions":
  #
  # --- now run the phase calculation
  #
  include ("InDetRecExample/ConfiguredInDetTRTPhase.py")
  InDetRecTRTPhase = ConfiguredInDetTRTPhase ([InDetKeys.UnslimmedTracks()],
                                              InDetKeys.TRT_Segments())

# ------------------------------------------------------------
#
# --- do primary vertexing and particle creation for default tracking
#
# ------------------------------------------------------------

# fix 
if not InDetFlags.doTruth():
  InputTrackCollectionTruth = None

include ("InDetRecExample/ConfiguredPriVtxAndPartCreation.py")
InDetPriVtxPartCreation = ConfiguredPriVtxAndPartCreation (extension                 = "",
                                                           #InputTrackCollection      = InputTrackCollection,
                                                           InputTrackCollection      = InDetKeys.xAODTrackParticleContainer(),
                                                           InputTrackCollectionTruth = InputTrackCollectionTruth,
                                                           VxCandidates              = InDetKeys.xAODVertexContainer(),
                                                           TrackParticles            = InDetKeys.TrackParticles(),
                                                           TrackParticlesTruth       = InDetKeys.TrackParticlesTruth())

# ------------------------------------------------------------
#
# --- do particle creation for default forward tracking
#
# ------------------------------------------------------------

if InDetFlags.doForwardTracks():
  include ("InDetRecExample/ConfiguredPriVtxAndPartCreation.py")
  InDetPriVtxPartCreation = ConfiguredPriVtxAndPartCreation (extension                 = "ForwardTracks",
                                                             InputTrackCollection      = InDetKeys.ResolvedForwardTracks(),
                                                             InputTrackCollectionTruth = InDetKeys.ResolvedForwardTracksTruth(),
                                                             VxCandidates              = InDetKeys.xAODVertexContainer(),
                                                             TrackParticles            = InDetKeys.ResolvedForwardTrackParticles(),
                                                             TrackParticlesTruth       = InDetKeys.ResolvedForwardTrackParticlesTruth())


# ------------------------------------------------------------
#
#  --- now load primary vertex setup with no beamconstraint and in split mode for enhanced monitoring
#      this should be done after trackparticle creation because it always runs on trackparticles (as opposed to the default primary vertexing)
# ------------------------------------------------------------

if InDetFlags.doVertexFindingForMonitoring():

  # also load finder tools here (easier for container naming)
  #
  # PV with NO beam constraint
  #
  from InDetPriVxFinderTool.InDetPriVxFinderToolConf import InDet__InDetIterativePriVxFinderTool
  InDetPriVxFinderToolNoBeamConstraint = InDet__InDetIterativePriVxFinderTool(name                     = "InDetPriVxFinderToolNoBeamConstraint",
                                                                              VertexFitterTool         = InDetVxFitterTool,
                                                                              TrackSelector            = InDetTrackSelectorTool,
                                                                              SeedFinder               = InDetVtxSeedFinder,
                                                                              ImpactPoint3dEstimator   = InDetImpactPoint3dEstimator,
                                                                              LinearizedTrackFactory   = InDetLinFactory,
                                                                              useBeamConstraint        = False,
                                                                              significanceCutSeeding   = 12,
                                                                              maximumChi2cutForSeeding = 49,
                                                                              maxVertices              = 200,
                                                                              createSplitVertices      = False,
                                                                              doMaxTracksCut           = InDetPrimaryVertexingCuts.doMaxTracksCut(),
                                                                              MaxTracks                = InDetPrimaryVertexingCuts.MaxTracks()  )
  ToolSvc += InDetPriVxFinderToolNoBeamConstraint
  if (InDetFlags.doPrintConfigurables()):
    printfunc (InDetPriVxFinderToolNoBeamConstraint)

  from InDetPriVxFinder.InDetPriVxFinderConf import InDet__InDetPriVxFinder
  InDetPriVxFinderNoBeamConstraint = InDet__InDetPriVxFinder(name                        = "InDetPriVxFinderNoBeamConstraint",
                                                             VertexFinderTool            = InDetPriVxFinderToolNoBeamConstraint,
                                                             TracksName                  = InDetKeys.xAODTrackParticleContainer(),
                                                             VxCandidatesOutputName      = InDetKeys.PrimaryVerticesWithoutBeamConstraint(),
                                                             VertexCollectionSortingTool = VertexCollectionSortingTool,
                                                             doVertexSorting             = True)

  topSequence += InDetPriVxFinderNoBeamConstraint
  if InDetFlags.doPrintConfigurables():
    printfunc (InDetPriVxFinderNoBeamConstraint)

if InDetFlags.doSplitVertexFindingForMonitoring():
  #
  # Split primary vertexing
  #
  from InDetPriVxFinderTool.InDetPriVxFinderToolConf import InDet__InDetIterativePriVxFinderTool
  InDetPriVxFinderToolSplit = InDet__InDetIterativePriVxFinderTool( name                     = "InDetIterativePriVxFinderToolSplit",
                                                                    VertexFitterTool         = InDetVxFitterTool,
                                                                    TrackSelector            = InDetTrackSelectorTool,
                                                                    SeedFinder               = InDetVtxSeedFinder,
                                                                    ImpactPoint3dEstimator   = InDetImpactPoint3dEstimator,
                                                                    LinearizedTrackFactory   = InDetLinFactory,
                                                                    useBeamConstraint        = False,
                                                                    significanceCutSeeding   = 12,
                                                                    maximumChi2cutForSeeding = 49,
                                                                    maxVertices              = 25,
                                                                    createSplitVertices      = True,
                                                                    doMaxTracksCut           = InDetPrimaryVertexingCuts.doMaxTracksCut(),
                                                                    MaxTracks                = InDetPrimaryVertexingCuts.MaxTracks())
  ToolSvc += InDetPriVxFinderToolSplit
  if (InDetFlags.doPrintConfigurables()):
    printfunc (InDetPriVxFinderToolSplit)

  from InDetPriVxFinder.InDetPriVxFinderConf import InDet__InDetPriVxFinder
  InDetPriVxFinderSplit= InDet__InDetPriVxFinder(name                        = "InDetPriVxFinderSplit",
                                                 VertexFinderTool            = InDetPriVxFinderToolSplit,
                                                 TracksName                  = InDetKeys.xAODTrackParticleContainer(),
                                                 VxCandidatesOutputName      = InDetKeys.PrimaryVerticesSplitStream(),
                                                 VertexCollectionSortingTool = VertexCollectionSortingTool,
                                                 doVertexSorting             = False)
  topSequence += InDetPriVxFinderSplit
  if InDetFlags.doPrintConfigurables():
    printfunc (InDetPriVxFinderSplit)

# ---------------------------------------------------------------------------- 
#
# --- TRT low beta finder (sources unslimmed tracks via TrackParticle objects) 
#
# ----------------------------------------------------------------------------     

if InDetFlags.doLowBetaFinder():
  include ("InDetRecExample/ConfiguredLowBetaFinder.py")
  from AthenaCommon.GlobalFlags import globalflags
  InDetLowBetaTrkAlgorithm = ConfiguredLowBetaFinder(InDetKeys.xAODTrackParticleContainer(),
                                                     True if (globalflags.DataSource != "data") else False,
                                                     InDetKeys.UnslimmedTracks())

# -------------------------------------------------------------------------
#
# ----------- V0 Finder Algorithm
#
# -------------------------------------------------------------------------

if InDetFlags.doV0Finder():
  #
  if InDetFlags.useV0Fitter():
    from TrkV0Fitter.TrkV0FitterConf import Trk__TrkV0VertexFitter
    InDetV0Fitter = Trk__TrkV0VertexFitter(name              = 'InDetV0Fitter',
                                           MaxIterations     = 10,
                                           Use_deltaR        = False,
                                           Extrapolator      = "Trk::Extrapolator/InDetExtrapolator")
    ToolSvc += InDetV0Fitter
    #InDetV0Fitter.OutputLevel = DEBUG
    if (InDetFlags.doPrintConfigurables()):
      printfunc (     InDetV0Fitter)
    #
    InDetVKVertexFitter  = None
    InDetKshortFitter    = None
    InDetLambdaFitter    = None
    InDetLambdabarFitter = None
  else:
    InDetV0Fitter   = None
    #    
    from TrkVKalVrtFitter.TrkVKalVrtFitterConf import Trk__TrkVKalVrtFitter
    InDetVKVertexFitter = Trk__TrkVKalVrtFitter(name                = "InDetVKVFitter",
                                                Extrapolator        = "Trk::Extrapolator/InDetExtrapolator",
                                                IterationNumber     = 30,
                                                MakeExtendedVertex  = True,
                                                FirstMeasuredPoint  = True)
    ToolSvc += InDetVKVertexFitter
    #InDetVKVertexFitter.OutputLevel = DEBUG
    if (InDetFlags.doPrintConfigurables()):
      printfunc (InDetVKVertexFitter)
      #
    from TrkVKalVrtFitter.TrkVKalVrtFitterConf import Trk__TrkVKalVrtFitter
    InDetKshortFitter = Trk__TrkVKalVrtFitter(name                = "InDetVKKVFitter",
                                              Extrapolator        = "Trk::Extrapolator/InDetExtrapolator",
                                              IterationNumber     = 30,
                                              MakeExtendedVertex  = True,
                                              FirstMeasuredPoint  = True,
                                              InputParticleMasses = [139.57,139.57],
                                              MassForConstraint   = 497.672)
    ToolSvc += InDetKshortFitter
    if (InDetFlags.doPrintConfigurables()):
      printfunc (InDetKshortFitter)
    #
    from TrkVKalVrtFitter.TrkVKalVrtFitterConf import Trk__TrkVKalVrtFitter
    InDetLambdaFitter = Trk__TrkVKalVrtFitter(name                = "InDetVKLFitter",
                                              Extrapolator        = "Trk::Extrapolator/InDetExtrapolator",
                                              IterationNumber     = 30,
                                              MakeExtendedVertex  = True,
                                              FirstMeasuredPoint  = True,
                                              InputParticleMasses = [938.272,139.57],
                                              MassForConstraint   = 1115.68)
    ToolSvc += InDetLambdaFitter
    if (InDetFlags.doPrintConfigurables()):
      printfunc (InDetLambdaFitter)
    #
    from TrkVKalVrtFitter.TrkVKalVrtFitterConf import Trk__TrkVKalVrtFitter
    InDetLambdabarFitter = Trk__TrkVKalVrtFitter(name                = "InDetVKLbFitter",
                                                 Extrapolator        = "Trk::Extrapolator/InDetExtrapolator",
                                                 IterationNumber     = 30,
                                                 MakeExtendedVertex  = True,
                                                 FirstMeasuredPoint  = True,
                                                 InputParticleMasses = [139.57,938.272],
                                                 MassForConstraint   = 1115.68)
    ToolSvc += InDetLambdabarFitter
    if (InDetFlags.doPrintConfigurables()):
      printfunc (InDetLambdabarFitter)
  #
  # --- we always need the gamma tool
  #
  from TrkVKalVrtFitter.TrkVKalVrtFitterConf import Trk__TrkVKalVrtFitter
  InDetGammaFitter = Trk__TrkVKalVrtFitter(name                = "InDetVKGFitter",
                                           Extrapolator        = "Trk::Extrapolator/InDetExtrapolator",
                                           IterationNumber     = 30,
                                           Robustness          = 6,
                                           MakeExtendedVertex  = True,
                                           FirstMeasuredPoint  = True,
                                           usePhiCnst          = True,
                                           useThetaCnst        = True,
                                           InputParticleMasses = [0.511,0.511])
  ToolSvc += InDetGammaFitter
  if (InDetFlags.doPrintConfigurables()):
    printfunc (InDetGammaFitter)

  # Track selector tool
  #
  if InDetFlags.doV0Finder() :  
    if InDetFlags.doSimpleV0Finder() :  
      from InDetTrackSelectorTool.InDetTrackSelectorToolConf import InDet__InDetConversionTrackSelectorTool
      InDetV0VxTrackSelector = InDet__InDetConversionTrackSelectorTool(name                = "InDetV0VxTrackSelector",
                                                                       TrackSummaryTool    = InDetTrackSummaryTool,
                                                                       Extrapolator        = "Trk::Extrapolator/InDetExtrapolator",
                                                                       maxTrtD0            = 50.,
                                                                       maxSiZ0             = 250.,
                                                                       significanceD0_Si   = 1.,
                                                                       significanceD0_Trt  = 1.,
                                                                       significanceZ0_Trt  = 3.,
                                                                       minPt               = 50.0,
                                                                       IsConversion        = False)
    else:
      from InDetTrackSelectorTool.InDetTrackSelectorToolConf import InDet__InDetConversionTrackSelectorTool
      InDetV0VxTrackSelector = InDet__InDetConversionTrackSelectorTool(name                = "InDetV0VxTrackSelector",
                                                                       TrackSummaryTool    = InDetTrackSummaryTool,
                                                                       Extrapolator        = "Trk::Extrapolator/InDetExtrapolator",
                                                                       maxTrtD0            = 50.,
                                                                       maxSiZ0             = 250.,
                                                                       significanceD0_Si   = 1.,
                                                                       significanceD0_Trt  = 1.,
                                                                       significanceZ0_Trt  = 3.,
                                                                       minPt               = 400.0,
                                                                       IsConversion        = False)
    ToolSvc += InDetV0VxTrackSelector
    if (InDetFlags.doPrintConfigurables()):
      printfunc (InDetV0VxTrackSelector)
      pass
    pass
  
  #
  # Vertex point estimator
  #
  from InDetConversionFinderTools.InDetConversionFinderToolsConf import InDet__VertexPointEstimator
  InDetV0VtxPointEstimator = InDet__VertexPointEstimator(name                   = "InDetV0VtxPointEstimator",
                                                         MaxTrkXYDiffAtVtx      = [20.,20.,20.],
                                                         MaxTrkZDiffAtVtx       = [100.,100.,100.],
                                                         MaxTrkXYValue          = [400.,400.,400.],
                                                         MinArcLength           = [-800.,-800.,-800.],
                                                         MaxArcLength           = [800.,800.,800.],
                                                         MinDeltaR              = [-10000.,-10000.,-10000.],
                                                         MaxDeltaR              = [10000.,10000.,10000.],
                                                         MaxPhi                 = [10000., 10000., 10000.],
                                                         MaxChi2OfVtxEstimation = 2000.)
  ToolSvc += InDetV0VtxPointEstimator
  if (InDetFlags.doPrintConfigurables()):
    printfunc (InDetV0VtxPointEstimator)

  #
  # InDetV0FinderTool
  #
  if InDetFlags.doSimpleV0Finder() :  
    from InDetV0Finder.InDetV0FinderConf import InDet__InDetV0FinderTool
    V0FinderTool = InDet__InDetV0FinderTool(name                    = 'InDetV0FinderTool',
                                            TrackParticleCollection = InDetKeys.xAODTrackParticleContainer(),
                                            useV0Fitter             = InDetFlags.useV0Fitter(),
                                            VertexFitterTool        = InDetV0Fitter,
                                            VKVertexFitterTool      = InDetVKVertexFitter,
                                            KshortFitterTool        = InDetKshortFitter,
                                            LambdaFitterTool        = InDetLambdaFitter,
                                            LambdabarFitterTool     = InDetLambdabarFitter,
                                            GammaFitterTool         = InDetGammaFitter,
                                            TrackSelectorTool       = InDetV0VxTrackSelector,
                                            VertexPointEstimator    = InDetV0VtxPointEstimator,
                                            doSimpleV0              = True,
                                            #useorigin               = False,
                                            #useTRTplusTRT           = True,
                                            #useTRTplusSi            = True,
                                            uksmin                  = 0.,
                                            uksmax                  = 2000.,
                                            ulamin                  = 0.,
                                            ulamax                  = 2000.,
                                            ksmin                   = 350,
                                            ksmax                   = 650,
                                            lamin                   = 900, 
                                            lamax                   = 1300,    
                                            d0_cut                  = 0.,
                                            Extrapolator            = "Trk::Extrapolator/InDetExtrapolator")
  else:
    from InDetV0Finder.InDetV0FinderConf import InDet__InDetV0FinderTool
    V0FinderTool = InDet__InDetV0FinderTool(name                    = 'InDetV0FinderTool',
                                            TrackParticleCollection = InDetKeys.xAODTrackParticleContainer(),
                                            useV0Fitter             = InDetFlags.useV0Fitter(),
                                            VertexFitterTool        = InDetV0Fitter,
                                            VKVertexFitterTool      = InDetVKVertexFitter,
                                            KshortFitterTool        = InDetKshortFitter,
                                            LambdaFitterTool        = InDetLambdaFitter,
                                            LambdabarFitterTool     = InDetLambdabarFitter,
                                            GammaFitterTool         = InDetGammaFitter,
                                            TrackSelectorTool       = InDetV0VxTrackSelector,
                                            VertexPointEstimator    = InDetV0VtxPointEstimator,
                                            doSimpleV0              = False,
                                            #useorigin               = False,
                                            #useTRTplusTRT           = True,
                                            #useTRTplusSi            = True,
                                            useVertexCollection     = True,
                                            #trkSelPV                = True,
                                            Extrapolator            = "Trk::Extrapolator/InDetExtrapolator")
  ToolSvc += V0FinderTool
  if (InDetFlags.doPrintConfigurables()):
    printfunc (V0FinderTool)

  #
  # --- now configure the algorithm 
  # 
  if InDetFlags.doV0Finder() :  
    from InDetV0Finder.InDetV0FinderConf import InDet__InDetV0Finder
    InDetV0Finder = InDet__InDetV0Finder(name                    = 'InDetV0Finder',
                                         #decorateV0              = False,
                                         InDetV0FinderToolName   = V0FinderTool,
                                         V0ContainerName         = InDetKeys.xAODV0VertexContainer(),
                                         KshortContainerName     = InDetKeys.xAODKshortVertexContainer(),
                                         LambdaContainerName     = InDetKeys.xAODLambdaVertexContainer(),
                                         LambdabarContainerName  = InDetKeys.xAODLambdabarVertexContainer())
    
    topSequence += InDetV0Finder
    if (InDetFlags.doPrintConfigurables()):
      printfunc (InDetV0Finder)
      pass
    pass
  
  pass # end if (doV0Finder)

# ----------------------------------------------------------------------
#
# ----------- Use Conversion Finder to find V0s ?
#
# ----------------------------------------------------------------------

if InDetFlags.doSecVertexFinder():
  #
  # --- setup of cut values for  Secondary vertexing  
  #
  if (not 'InDetSecondaryVertexCuts' in dir()):
    printfunc ("InDetRecPostProcessing: InDetSecondaryVertexCuts not set before - import them now")
    from InDetRecExample.ConfiguredSecondaryVertexCuts import ConfiguredSecondaryVertexCuts
    InDetSecondaryVertexCuts      = ConfiguredSecondaryVertexCuts(InDetFlags.secondaryVertexCutSetup(),
                                                                  InDetNewTrackingCuts.minSecondaryPt())
    if (InDetFlags.doPrintConfigurables()):
      printfunc (InDetSecondaryVertexCuts.printInfo()         )
    pass
  pass
  #
  # --- configure the secondary vertex finder
  #
  from InDetRecExample.ConfiguredSecVertexFinding import ConfiguredSecVertexFinding
  InDetSecVertexFinding = ConfiguredSecVertexFinding (prefix           = "InDetSecVx",
                                                      VertexCuts       = InDetSecondaryVertexCuts,
                                                      TrackParticles   = InDetKeys.TrackParticles(),
                                                      SecVertices      = InDetKeys.SecVertices(),
                                                      Extrapolator     = InDetExtrapolator,
                                                      TrackSummaryTool = InDetTrackSummaryTool,
                                                      printConfig      = InDetFlags.doPrintConfigurables())
  # --- we need the driving algorithm
  InDetSecVertexFinding.addAlgorithm()

# -------------------------------------------------------------------------
#
# ----------- Conversion Finder
#
# -------------------------------------------------------------------------

if InDetFlags.doConversions():
  #
  # --- setup of cut values for  Conversion vertexing  
  #
  if (not 'InDetConversionVertexCuts' in dir()):
    printfunc ("InDetRecPostProcessing: InDetConversionVertexCuts not set before - import them now")
    from InDetRecExample.ConfiguredSecondaryVertexCuts import ConfiguredSecondaryVertexCuts
    InDetConversionVertexCuts      = ConfiguredSecondaryVertexCuts(InDetFlags.conversionVertexCutSetup(),
                                                                   InDetNewTrackingCuts.minPT())
    if (InDetFlags.doPrintConfigurables()):
      printfunc (InDetConversionVertexCuts.printInfo())
    pass
  pass
  #
  # --- configure the conversion finder
  #

  # Utility for conversion tool
  from TrkVertexFitterUtils.TrkVertexFitterUtilsConf import Trk__NeutralParticleParameterCalculator
  convUtils = Trk__NeutralParticleParameterCalculator(
      name                   = "convUtils",
      LinearizedTrackFactory = None)
  ToolSvc+= convUtils 

  from InDetRecExample.ConfiguredSecVertexFinding import ConfiguredSecVertexFinding
  InDetConversionFinding = ConfiguredSecVertexFinding (prefix           = "InDetConversion",
                                                       VertexCuts       = InDetConversionVertexCuts,
                                                       TrackParticles   = InDetKeys.xAODTrackParticleContainer(),
                                                       SecVertices      = InDetKeys.Conversions(),
                                                       Extrapolator     = InDetExtrapolator,
                                                       TrackSummaryTool = InDetTrackSummaryTool,
                                                       printConfig      = InDetFlags.doPrintConfigurables())
  # --- we need the driving algorithm
  InDetConversionFinding.addAlgorithm()

  from egammaTrackTools.egammaTrackToolsFactories import EMExtrapolationTools
  topSequence.InDetConversionFinder.ExtrapolationTool  =  EMExtrapolationTools.copyPublic('egammaExtrapolationTool')()
  topSequence.InDetConversionFinder.doExtrapolation = True


# [XXX JDC: problems with the association tool (Tracks already in SG)
# @TODO needs to be fixed.
if InDetFlags.doParticleCreation() and not InDetFlags.useExistingTracksAsInput():
 trackToVertexTool = None
 if InDetFlags.perigeeExpression() == 'Vertex' :
     if hasattr(ToolSvc,'TrackToVertex') :
        trackToVertexTool = ToolSvc.TrackToVertex
     else :
       from TrkExTools.AtlasExtrapolator import AtlasExtrapolator
       atlas_extrapolator = AtlasExtrapolator()

       from TrackToVertex.TrackToVertexConf import Reco__TrackToVertex
       trackToVertexTool = Reco__TrackToVertex('TrackToVertex',
                                               Extrapolator = atlas_extrapolator)
       ToolSvc += trackToVertexTool

if rec.doPhysicsValidationAugmentation() :
  try:
    import InDetPhysValMonitoring.InDetPhysValDecoration
    InDetPhysValMonitoring.InDetPhysValDecoration.addDecoratorIfNeeded()
  except ImportError:
    from AthenaCommon.Logging import logging
    log = logging.getLogger('InDetRecExample/InDetRecPostProcessing.py' )
    log.info('Package InDetPhysValMonitoring.InDetPhysValDecoration is missing.'\
               ' So, will not run the decoration. Module should become available in future'\
               ' versions of InDetPhysValMonitoring')
    import traceback
    log.info (traceback.format_exc())
    import sys 
    sys.exit(1)
    pass

