# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from __future__ import print_function


# ------------------------------------------------------------------------------------
#
# ----------- Configures secondary vertex finding for V0 or conversions
#
# ------------------------------------------------------------------------------------

class ConfiguredSecVertexFinding:

    def __init__(self,
                 prefix="",
                 VertexCuts=None,
                 TrackParticles=None,
                 SecVertices=None,
                 Extrapolator=None,
                 MagFieldSvc=None,
                 printConfig=False):

        # get ToolSvc and topSequence
        from AthenaCommon.AppMgr import ToolSvc
        from AthenaCommon.AlgSequence import AlgSequence
        topSequence = AlgSequence()

        #
        # --- load vertex fitter
        #
        from TrkVKalVrtFitter.TrkVKalVrtFitterConf import Trk__TrkVKalVrtFitter
        InDetSecVxFitterTool = Trk__TrkVKalVrtFitter(
            name=prefix+"Fitter",
            Extrapolator=Extrapolator,
            IterationNumber=VertexCuts.Fitter_IterationNumber(),
            MakeExtendedVertex=VertexCuts.Fitter_MakeExtendedVertex(),
            FirstMeasuredPoint=VertexCuts.Fitter_FirstMeasuredPoint(),
            Robustness=VertexCuts.Fitter_Robustness(),
            InputParticleMasses=VertexCuts.Fitter_InputParticleMasses(),
            VertexForConstraint=VertexCuts.Fitter_VertexForConstraint(),
            CovVrtForConstraint=VertexCuts.Fitter_CovVrtForConstraint(),
            FirstMeasuredPointLimit=VertexCuts.FirstMeasuredPointLimit(),
            usePhiCnst=VertexCuts.usePhiCnst(),
            useThetaCnst=VertexCuts.useThetaCnst())
        if (printConfig):
            print(InDetSecVxFitterTool)
        #
        # --- Distance of minimum approach utility
        #
        from TrkVertexSeedFinderUtils.TrkVertexSeedFinderUtilsConf import (
            Trk__SeedNewtonTrkDistanceFinder)
        InDetSecVxTrkDistanceFinder = Trk__SeedNewtonTrkDistanceFinder(
            name=prefix+'TrkDistanceFinder')
        ToolSvc += InDetSecVxTrkDistanceFinder
        if (printConfig):
            print(InDetSecVxTrkDistanceFinder)
        #
        # --- Helper Tool
        #
        from InDetConversionFinderTools.InDetConversionFinderToolsConf import (
            InDet__ConversionFinderUtils)
        InDetSecVxHelper = InDet__ConversionFinderUtils(
            name=prefix+"FinderUtils")
        if (printConfig):
            print(InDetSecVxHelper)
        #
        # --- Track selector tool
        #
        from InDetTrackSelectorTool.InDetTrackSelectorToolConf import (
            InDet__InDetConversionTrackSelectorTool)
        InDetSecVxTrackSelector = InDet__InDetConversionTrackSelectorTool(
            name=prefix+"TrackSelector",
            Extrapolator=Extrapolator,
            maxSiD0=VertexCuts.TrkSel_maxSiD0(),
            maxTrtD0=VertexCuts.TrkSel_maxTrtD0(),
            maxSiZ0=VertexCuts.TrkSel_maxSiZ0(),
            maxTrtZ0=VertexCuts.TrkSel_maxTrtZ0(),
            minPt=VertexCuts.TrkSel_minPt(),
            RatioCut1=VertexCuts.TrkSel_RatioCut1(),
            RatioCut2=VertexCuts.TrkSel_RatioCut2(),
            RatioCut3=VertexCuts.TrkSel_RatioCut3(),
            RatioTRT=VertexCuts.TrkSel_RatioTRT(),
            IsConversion=VertexCuts.TrkSel_IsConversion(),
            significanceD0_Si=VertexCuts.TrkSel_significanceD0_Si(),
            RatioV0=VertexCuts.TrkSel_RatioV0())

        if (printConfig):
            print(InDetSecVxTrackSelector)
        #
        # Track pairs selector
        #
        from InDetConversionFinderTools.InDetConversionFinderToolsConf import (
            InDet__TrackPairsSelector)
        InDetSecVxTrackPairsSelector = InDet__TrackPairsSelector(
            name=prefix+"TrackPairsSelector",
            ConversionFinderHelperTool=InDetSecVxHelper,
            DistanceTool=InDetSecVxTrkDistanceFinder,
            MaxFirstHitRadius=VertexCuts.TrkPairSel_MaxFirstHitRadius(),
            MaxDistBetweenTracks=VertexCuts.TrkPairSel_MaxDistBetweenTracks(),
            MaxEta=VertexCuts.TrkPairSel_MaxEta(),
            MaxInitDistance=VertexCuts.TrkPairSel_MaxInitDistance(),
            MinTrackAngle=VertexCuts.TrkPairSel_MinTrackAngle())
        if (printConfig):
            print(InDetSecVxTrackPairsSelector)
        #
        # Vertex point estimator
        #
        from InDetConversionFinderTools.InDetConversionFinderToolsConf import (
            InDet__VertexPointEstimator)
        InDetSecVtxPointEstimator = InDet__VertexPointEstimator(
            name=prefix+"PointEstimator",
            MinDeltaR=VertexCuts.VtxPt_MinDeltaR(),
            MaxDeltaR=VertexCuts.VtxPt_MaxDeltaR(),
            MaxPhi=VertexCuts.VtxPt_MaxPhi())
        if (printConfig):
            print(InDetSecVtxPointEstimator)
        #
        # Secondary Vertex post selector
        #
        from InDetConversionFinderTools.InDetConversionFinderToolsConf import (
            InDet__ConversionPostSelector)
        InDetSecVtxPostSelector = InDet__ConversionPostSelector(
            name=prefix+"PostSelector",
            MaxChi2Vtx=VertexCuts.SecVtxPost_MaxChi2Vtx(),
            MaxInvariantMass=VertexCuts.SecVtxPost_MaxInvariantMass(),
            MinFitMomentum=VertexCuts.SecVtxPost_MinFitMomentum(),
            MinRadius=VertexCuts.SecVtxPost_MinRadius(),
            MinPt=VertexCuts.SecVtxPost_MinPt(),
            MaxdR=VertexCuts.SecVtxPost_MaxdR(),
            MaxPhiVtxTrk=VertexCuts.SecVtxPost_MaxPhiVtxTrk())
        if (printConfig):
            print(InDetSecVtxPostSelector)
        #
        # Single track secondary vertex tool
        #
        from InDetConversionFinderTools.InDetConversionFinderToolsConf import (
            InDet__SingleTrackConversionTool)
        InDetSingleTrackSecVtx = InDet__SingleTrackConversionTool(
            name=prefix+"SingleTrackTool",
            ConversionFinderHelperTool=InDetSecVxHelper,
            Extrapolator=Extrapolator,
            MaxBLayerHits=VertexCuts.SingleTrk_MaxBLayerHits(),
            MinInitialHitRadius=VertexCuts.SingleTrk_MinInitialHitRadius(),
            MinInitialHitRadius_noBlay=VertexCuts.SingleTrk_MinInitialHitRadius_noBlay(),
            MinRatioOfHLhits=VertexCuts.SingleTrk_MinRatioOfHLhits())
        if (printConfig):
            print(InDetSingleTrackSecVtx)
        #
        # conversion finder tool
        #
        from InDetConversionFinderTools.InDetConversionFinderToolsConf import (
            InDet__InDetConversionFinderTools)
        InDetSecVtxFinderTool = InDet__InDetConversionFinderTools(
            name=prefix+"FinderTool",
            VertexFitterTool=InDetSecVxFitterTool,
            TrackSelectorTool=InDetSecVxTrackSelector,
            TrackPairsSelector=InDetSecVxTrackPairsSelector,
            VertexPointEstimator=InDetSecVtxPointEstimator,
            PostSelector=InDetSecVtxPostSelector,
            SingleTrackConversionTool=InDetSingleTrackSecVtx,
            Extrapolator=Extrapolator,
            TrackParticleCollection=TrackParticles,
            RemoveTrtTracks=VertexCuts.Finder_RemoveTrtTracks(),
            IsConversion=VertexCuts.Finder_IsConversion(),
            MinDistVtxHit=VertexCuts.Finder_MinDistVtxHit(),
            MaxDistVtxHit=VertexCuts.Finder_MaxDistVtxHit(),
            MinFlightAngle=VertexCuts.Finder_MinFlightAngle(),
            MinInitVtxR=VertexCuts.Finder_MinInitVtxR())
        if (printConfig):
            print(InDetSecVtxFinderTool)

        # --- remember instances
        self.__prefix = prefix
        self.__InDetSecVtxFinderTool = InDetSecVtxFinderTool
        self.__TrackParticles = TrackParticles
        self.__SecVertices = SecVertices
        self.__printConfig = printConfig
        self.__topSequence = topSequence

    def toolInstance(self):
        #
        # --- return tool for usage in egamma
        #
        return self.__InDetSecVtxFinderTool

    def addAlgorithm(self):
        #
        # --- add driving algorithm of usage in InDet
        #
        from InDetConversionFinder.InDetConversionFinderConf import (
            InDet__ConversionFinder)
        InDetSecVtxFinder = InDet__ConversionFinder(
            name=self.__prefix+"Finder",
            VertexFinderTool=self.__InDetSecVtxFinderTool,
            TracksName=self.__TrackParticles,
            InDetConversionOutputName=self.__SecVertices)
        self.__topSequence += InDetSecVtxFinder
        if (self.__printConfig):
            print(InDetSecVtxFinder)
