#====================================================================
# Collection of tools required by V0Finder
#====================================================================

class BPHYV0FinderTools:
 
    def __init__(self, derivation = ""):

        if derivation == "":
            print('--------> FATAL: BPHYV0FinderTools, "derivation" string not set!')
            import sys
            sys.exit()

        from AthenaCommon.AppMgr import ToolSvc

        # set up extrapolator
        from TrkExTools.AtlasExtrapolator import AtlasExtrapolator
        self.InDetExtrapolator = AtlasExtrapolator(name             = derivation+"_AtlasExtrapolator")
        ToolSvc += self.InDetExtrapolator
        print(self.InDetExtrapolator)

        # Vertex point estimator
        from InDetConversionFinderTools.InDetConversionFinderToolsConf import InDet__VertexPointEstimator
        self.V0VtxPointEstimator = InDet__VertexPointEstimator(name        = derivation+"_VtxPointEstimator",
                                                    MaxTrkXYDiffAtVtx      = [20.,20.,20.],
                                                    MaxTrkZDiffAtVtx       = [100.,100.,100.],
                                                    MaxTrkXYValue          = [400.,400.,400.],
                                                    MinArcLength           = [-800.,-800.,-800.],
                                                    MaxArcLength           = [800.,800.,800.],
                                                    MinDeltaR              = [-10000.,-10000.,-10000.],
                                                    MaxDeltaR              = [10000.,10000.,10000.],
                                                    MaxPhi                 = [10000., 10000., 10000.],
                                                    MaxChi2OfVtxEstimation = 2000.)
        ToolSvc += self.V0VtxPointEstimator
        print(self.V0VtxPointEstimator)

        from InDetRecExample.InDetKeys import InDetKeys

        from InDetAssociationTools.InDetAssociationToolsConf import InDet__InDetPRD_AssociationToolGangedPixels
        self.V0PrdAssociationTool = InDet__InDetPRD_AssociationToolGangedPixels(name                     = derivation+"_V0PrdAssociationTool",
                                                                          PixelClusterAmbiguitiesMapName = InDetKeys.GangedPixelMap())
        ToolSvc += self.V0PrdAssociationTool
        print(self.V0PrdAssociationTool)

        from RecExConfig.RecFlags import rec
        CountDeadModulesAfterLastHit=False
        #rec.Commissioning=False

        from InDetTrackHoleSearch.InDetTrackHoleSearchConf import InDet__InDetTrackHoleSearchTool
        self.V0HoleSearchTool = InDet__InDetTrackHoleSearchTool(name = derivation+"_V0HoleSearchTool",
                                                          Extrapolator = self.InDetExtrapolator,
                                                          usePixel      = DetFlags.haveRIO.pixel_on(),
                                                          useSCT        = DetFlags.haveRIO.SCT_on(),
                                                          #Commissioning = rec.Commissioning())
                                                      CountDeadModulesAfterLastHit = CountDeadModulesAfterLastHit)
        ToolSvc += self.V0HoleSearchTool
        print(self.V0HoleSearchTool)

        from InDetTrackSummaryHelperTool.InDetTrackSummaryHelperToolConf import InDet__InDetTrackSummaryHelperTool
        self.V0TrackSummaryHelperTool = InDet__InDetTrackSummaryHelperTool(name         = derivation+"_InDetSummaryHelper",
                                                                     AssoTool     = self.V0PrdAssociationTool,
                                                                     DoSharedHits = False,
                                                                     HoleSearch   = self.V0HoleSearchTool,
                                                                     usePixel      = DetFlags.haveRIO.pixel_on(),
                                                                     useSCT        = DetFlags.haveRIO.SCT_on(),
                                                                     useTRT        = DetFlags.haveRIO.TRT_on())
        ToolSvc += self.V0TrackSummaryHelperTool
        print(self.V0TrackSummaryHelperTool)

        from TrkTrackSummaryTool.TrkTrackSummaryToolConf import Trk__TrackSummaryTool
        self.V0TrackSummaryTool = Trk__TrackSummaryTool(name = derivation+"_V0TrackSummaryTool",
                                                  InDetSummaryHelperTool = self.V0TrackSummaryHelperTool,
                                                  doSharedHits           = False,
                                                  InDetHoleSearchTool    = self.V0HoleSearchTool)
        ToolSvc += self.V0TrackSummaryTool
        print(self.V0TrackSummaryTool)



        from InDetTrackSelectorTool.InDetTrackSelectorToolConf import InDet__InDetConversionTrackSelectorTool
        self.InDetV0VxTrackSelector = InDet__InDetConversionTrackSelectorTool(name                = derivation+"InDetV0VxTrackSelector",
                                                                              TrackSummaryTool    = self.V0TrackSummaryTool,
                                                                              Extrapolator        = self.InDetExtrapolator,
             #                                                                 Extrapolator        = "Trk::Extrapolator/InDetExtrapolator",
                                                                              maxTrtD0            = 50.,
                                                                              maxSiZ0             = 250.,
                                                                              significanceD0_Si   = 1.,
                                                                              significanceD0_Trt  = 1.,
                                                                              significanceZ0_Trt  = 3.,
                                                                              minPt               = 400.0,
                                                                              IsConversion        = False)
        ToolSvc += self.InDetV0VxTrackSelector
        print(self.InDetV0VxTrackSelector)


        # configure vertex fitters

        from TrkV0Fitter.TrkV0FitterConf import Trk__TrkV0VertexFitter
        self.BPhysV0Fitter = Trk__TrkV0VertexFitter(name              = derivation+'_BPhysV0Fitter',
                                                    MaxIterations     = 10,
                                                    Use_deltaR        = False,
                                                    FirstMeasuredPoint  = True,
                                                    Extrapolator      = self.InDetExtrapolator)
        ToolSvc += self.BPhysV0Fitter
        print(self.BPhysV0Fitter)
#
        from TrkVKalVrtFitter.TrkVKalVrtFitterConf import Trk__TrkVKalVrtFitter
        self.BPhysVKVertexFitter = Trk__TrkVKalVrtFitter(name           = derivation+"_BPhysVKVFitter",
                                                         Extrapolator        = self.InDetExtrapolator,
                                                         IterationNumber     = 30,
                                                         FirstMeasuredPoint  = True,
                                                         MakeExtendedVertex  = True)
        ToolSvc += self.BPhysVKVertexFitter
        print(self.BPhysVKVertexFitter)

#
        from TrkVKalVrtFitter.TrkVKalVrtFitterConf import Trk__TrkVKalVrtFitter
        self.BPhysKshortFitter = Trk__TrkVKalVrtFitter(name                = derivation+"_BPhysVKKVFitter",
                                                       Extrapolator        = self.InDetExtrapolator,
                                                       IterationNumber     = 30,
                                                       FirstMeasuredPoint  = True,
                                                       MakeExtendedVertex  = True,
                                                       InputParticleMasses = [139.57,139.57],
                                                       MassForConstraint   = 497.672)
        ToolSvc += self.BPhysKshortFitter
        print(self.BPhysKshortFitter)
#
        from TrkVKalVrtFitter.TrkVKalVrtFitterConf import Trk__TrkVKalVrtFitter
        self.BPhysLambdaFitter = Trk__TrkVKalVrtFitter(name                = derivation+"_BPhysVKLFitter",
                                                       Extrapolator        = self.InDetExtrapolator,
                                                       IterationNumber     = 30,
                                                       FirstMeasuredPoint  = True,
                                                       MakeExtendedVertex  = True,
                                                       InputParticleMasses = [938.272,139.57],
                                                       MassForConstraint   = 1115.68)
        ToolSvc += self.BPhysLambdaFitter
        print(self.BPhysLambdaFitter)
#
        from TrkVKalVrtFitter.TrkVKalVrtFitterConf import Trk__TrkVKalVrtFitter
        self.BPhysLambdabarFitter = Trk__TrkVKalVrtFitter(name                = derivation+"_BPhysVKLbFitter",
                                                          Extrapolator        = self.InDetExtrapolator,
                                                          IterationNumber     = 30,
                                                          FirstMeasuredPoint  = True,
                                                          MakeExtendedVertex  = True,
                                                          InputParticleMasses = [139.57,938.272],
                                                          MassForConstraint   = 1115.68)
        ToolSvc += self.BPhysLambdabarFitter
        print(self.BPhysLambdabarFitter)
#
        from TrkVKalVrtFitter.TrkVKalVrtFitterConf import Trk__TrkVKalVrtFitter
        self.BPhysGammaFitter = Trk__TrkVKalVrtFitter(name                = derivation+"_BPhysVKGFitter",
                                                      Extrapolator        = self.InDetExtrapolator,
                                                      IterationNumber     = 30,
                                                      Robustness          = 6,
                                                      FirstMeasuredPoint  = True,
                                                      MakeExtendedVertex  = True,
                                                      usePhiCnst          = True,
                                                      useThetaCnst        = True,
                                                      InputParticleMasses = [0.511,0.511])
        ToolSvc += self.BPhysGammaFitter
        print(self.BPhysGammaFitter)

##--------------------------------------------
## Setup V0Finder
##--------------------------------------------
        from InDetV0Finder.InDetV0FinderConf import InDet__InDetV0FinderTool
        self.V0FinderTool = InDet__InDetV0FinderTool(name                    = derivation+'_InDetV0FinderTool',
                                                     TrackParticleCollection = InDetKeys.xAODTrackParticleContainer(),
                                                     #TrackParticleCollection = "InDetTrackParticles",
                                                     useV0Fitter             = True,
                                                     VertexFitterTool        = self.BPhysV0Fitter,
                                                     VKVertexFitterTool      = self.BPhysVKVertexFitter,
                                                     KshortFitterTool        = self.BPhysKshortFitter,
                                                     LambdaFitterTool        = self.BPhysLambdaFitter,
                                                     LambdabarFitterTool     = self.BPhysLambdabarFitter,
                                                     GammaFitterTool         = self.BPhysGammaFitter,
                                                     TrackSelectorTool       = self.InDetV0VxTrackSelector,
                                                     VertexPointEstimator    = self.V0VtxPointEstimator,
                                                     doSimpleV0              = False,
                                                     #doSimpleV0              = True,
                                                     #useorigin               = False,
                                                     #useTRTplusTRT           = True,
                                                     #useTRTplusSi            = True,
                                                     useVertexCollection     = True,
                                                     #trkSelPV                = True,
                                                     Extrapolator            = self.InDetExtrapolator)
                                                     #Extrapolator            = "Trk::Extrapolator/InDetExtrapolator")
        ToolSvc += self.V0FinderTool
        print(self.V0FinderTool)

        #from InDetV0Finder.InDetV0FinderConf import InDet__InDetV0Finder
        #self.InDetV0Finder = InDet__InDetV0Finder(name                    = derivation+'InDetV0Finder',
        #                                          #decorateV0              = False,
        #                                          InDetV0FinderToolName   = V0FinderTool,
        #                                          V0ContainerName         = InDetKeys.xAODV0VertexContainer(),
        #                                          KshortContainerName     = InDetKeys.xAODKshortVertexContainer(),
        #                                          LambdaContainerName     = InDetKeys.xAODLambdaVertexContainer(),
        #                                          LambdabarContainerName  = InDetKeys.xAODLambdabarVertexContainer())
        #topSequence += self.InDetV0Finder
        #print      self.InDetV0Finder


