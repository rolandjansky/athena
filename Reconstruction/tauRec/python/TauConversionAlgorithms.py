# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

################################################################################
##
#@file TauConversionsAlgorithms.py
#
#@brief Algorithms for new ConversionFinder are configured here.
#
# This file contains two functions:
#     1) getTauConversionFinderTool():
#             Returns the TauTool which assigns the conversion tracks to taujets
#     2) getPhotonConversionTool():
#             Returns the TauTool which finds conversion vertices
# Not a standalaone - meant to be used in other job options file
#
#
#@author KG Tan
#@date May 2011
################################################################################


from tauRec.tauRecFlags import jobproperties

def getTauConversionFinderTool():
    if jobproperties.tauRecFlags.useVertexBasedConvFinder():
        inputConversionContainerName = "ConversionsVertex_Container"
        minEProb = 0.04
    else:
        inputConversionContainerName = "ConversionsPID_Container"
        minEProb = 0.9
    from tauRec.tauRecConf import TauConversionFinder
    TauConversionFinder = TauConversionFinder(
                              ConversionCandidatesName  = inputConversionContainerName,
                              TrackContainerName        = "InDetTrackParticles",
                              DoNormalTracks            = True,
                              #DoLooseTracks             = False,
                              MinElectronProbability    = minEProb,
                              AdjustTauCharge           = False
                          )
    return TauConversionFinder

def getPhotonConversionTool():
    
    if jobproperties.tauRecFlags.useVertexBasedConvFinder():
        from AthenaCommon.AppMgr import ToolSvc
        
        name = "_PhotonConversionVertex"

        #Configure the extrapolator
        from TrkExTools.AtlasExtrapolator import AtlasExtrapolator
        theAtlasExtrapolator=AtlasExtrapolator(name = 'theAtlasExtrapolator'+name)
        theAtlasExtrapolator.DoCaloDynamic = False # this turns off dynamic
        ToolSvc += theAtlasExtrapolator

        from TrkMagFieldTools.TrkMagFieldToolsConf import Trk__MagneticFieldTool
        InDetMagField = Trk__MagneticFieldTool('InDetMagField'+name)
        ToolSvc += InDetMagField

        #
        # Setup track summary tool
        #
        from TrkTrackSummaryTool.AtlasTrackSummaryTool import AtlasTrackSummaryTool
        MyInDetTrackSummaryTool = AtlasTrackSummaryTool(name = "MyInDetTrackSummaryTool"+name)
        ToolSvc += MyInDetTrackSummaryTool

        from TrkVKalVrtFitter.TrkVKalVrtFitterConf import Trk__TrkVKalVrtFitter
        InDetConversionVxFitterTool = Trk__TrkVKalVrtFitter(name                = "InDetConversionVxFitter"+name,
                                                            Extrapolator        = theAtlasExtrapolator,
                                                            IterationNumber     = 30,
                                                            MakeExtendedVertex  = True,
                                                            FirstMeasuredPoint  = True,
                                                            MagFieldSvc         = InDetMagField,
                                                            Robustness          = 6,
                                                            usePhiCnst          = False,
                                                            useThetaCnst        = False,
                                                            FirstMeasuredPointLimit = True, 
                                                            InputParticleMasses = [0.511,0.511],
                                                            VertexForConstraint = [0.,0.,0.],
                                                            CovVrtForConstraint = [0.015*0.015,0.,0.015*0.015,0.,0.,10000.*10000.])
        ToolSvc += InDetConversionVxFitterTool 
        print      InDetConversionVxFitterTool

        # Distance of minimum approach utility
        #
        from TrkVertexSeedFinderUtils.TrkVertexSeedFinderUtilsConf import Trk__SeedNewtonTrkDistanceFinder
        InDetConversionTrkDistanceFinder = Trk__SeedNewtonTrkDistanceFinder(name = 'InDetConversionTrkDistanceFinder'+name)
        ToolSvc += InDetConversionTrkDistanceFinder 
        print      InDetConversionTrkDistanceFinder

        # Straight line propagator needed to clean-up single track conversions
        #
        from TrkExSlPropagator.TrkExSlPropagatorConf import Trk__StraightLinePropagator as Propagator
        InDetConversionPropagator = Propagator(name = 'InDetConversionPropagator'+name)
        ToolSvc += InDetConversionPropagator
        print      InDetConversionPropagator

        # Helper Tool
        #
        from InDetConversionFinderTools.InDetConversionFinderToolsConf import InDet__ConversionFinderUtils
        InDetConversionHelper = InDet__ConversionFinderUtils(name = "InDetConversionFinderUtils"+name)
        ToolSvc += InDetConversionHelper
        print      InDetConversionHelper

        # Track selector tool
        #
        from InDetTrackSelectorTool.InDetTrackSelectorToolConf import InDet__InDetConversionTrackSelectorTool
        InDetConversionTrackSelector = InDet__InDetConversionTrackSelectorTool(name          = "InDetConversionTrackSelector"+name,
                                                                            TrackSummaryTool = MyInDetTrackSummaryTool,
                                                                            Extrapolator     = theAtlasExtrapolator, 
                                                                            maxSiD0          = 10000.,  #50.0,
                                                                            maxTrtD0         = 10000.,  #100.,
                                                                            maxSiZ0          = 10000.,  #350.0,
                                                                            maxTrtZ0         = 10000.,  #1400.,
                                                                            minPt            = 300, #InDetNewTrackingCuts.minSecondaryPt()
                                                                            RatioCut1        = 0.0,     #0.5,
                                                                            RatioCut2        = 0.0,
                                                                            RatioCut3        = 0.0)

        ToolSvc += InDetConversionTrackSelector
        print      InDetConversionTrackSelector


        # Track pairs selector
        #
        from InDetConversionFinderTools.InDetConversionFinderToolsConf import InDet__TrackPairsSelector
        InDetConversionTrackPairsSelector = InDet__TrackPairsSelector(name                     = "InDetConversionTrackPairsSelector"+name,
                                                                    ConversionFinderHelperTool = InDetConversionHelper,
                                                                    DistanceTool               = InDetConversionTrkDistanceFinder,
                                                                    MaxFirstHitRadius          = 10000.,
                                                                    MaxEta                     = [0.12,10000.,10000.],      #[0.5,1.0,0.5])
                                                                    MaxDistBetweenTracks       = [6.6,10000.,10000.],     #[6.,80.,30.]
                                                                    MaxInitDistance            = [10000.,10000.,10000.],
                                                                    MinTrackAngle              = 0.)
        ToolSvc += InDetConversionTrackPairsSelector
        print      InDetConversionTrackPairsSelector

        # Vertex point estimator
        #
        from InDetConversionFinderTools.InDetConversionFinderToolsConf import InDet__VertexPointEstimator
        InDetConversionVtxPointEstimator = InDet__VertexPointEstimator(name                = "InDetConversionVtxPointEstimator"+name,
                                                                    MaxTrkXYDiffAtVtx      = [10000.,10000.,10000.],
                                                                    MaxTrkZDiffAtVtx       = [10000.,10000.,10000.],
                                                                    MaxTrkXYValue          = [10000.,10000.,10000.],
                                                                    MinArcLength           = [-10000., -10000., -10000.],
                                                                    MaxArcLength           = [10000., 10000., 10000.],
                                                                    MinDeltaR              = [-10000.,-10000.,-10000.],
                                                                    MaxDeltaR              = [10000.,10000.,10000.],
                                                                    MaxHl                  = [10000.,10000.,10000.],
                                                                    MaxPhi                 = [0.034, 10000., 10000.]) #[0.05, 0.1, 0.1])
        ToolSvc += InDetConversionVtxPointEstimator
        print      InDetConversionVtxPointEstimator

        # Conversion post selector
        #
        from InDetConversionFinderTools.InDetConversionFinderToolsConf import InDet__ConversionPostSelector
        InDetConversionPostSelector = InDet__ConversionPostSelector(name             = "InDetConversionPostSelector"+name,
                                                                    MaxChi2Vtx       = [10000.,10000.,10000.],              #[40.,100.,80.],
                                                                    MaxInvariantMass = [45.,25000.,25000.],     #[60.,60.,30.],
                                                                    MinFitMomentum   = [0.,0.,0.],                 #[2000.,2000.,2000.],
                                                                    MinRadius        = [23.4,-10000.,-10000.],  #[30.,35.,250.],
                                                                    MinPt            = 0.,
                                                                    MaxdR            = 10000.,                    #-250.,
                                                                    MaxPhiVtxTrk     = 0.046)  #0.2)
        ToolSvc += InDetConversionPostSelector
        print      InDetConversionPostSelector

        # Single track conversion tool
        #
        from InDetConversionFinderTools.InDetConversionFinderToolsConf import InDet__SingleTrackConversionTool
        InDetSingleTrackConversion = InDet__SingleTrackConversionTool(name                     = "InDetSingleTrackConversionTool"+name,
                                                                    ConversionFinderHelperTool = InDetConversionHelper,
                                                                    TrackSummaryTool           = MyInDetTrackSummaryTool,
                                                                    Extrapolator               = theAtlasExtrapolator,
                                                                    MinInitialHitRadius        = 70.,
                                                                    MinRatioOfHLhits           = 0.95)
        ToolSvc += InDetSingleTrackConversion
        print      InDetSingleTrackConversion

        from InDetConversionFinderTools.InDetConversionFinderToolsConf import InDet__InDetConversionFinderTools
        MyInDetConversionFinderTools = InDet__InDetConversionFinderTools(name                  = "InDetConversionFinderTools"+name,
                                                                    VertexFitterTool           = InDetConversionVxFitterTool,
                                                                    TrackSelectorTool          = InDetConversionTrackSelector,
                                                                    TrackPairsSelector         = InDetConversionTrackPairsSelector,
                                                                    ConversionFinderHelperTool = InDetConversionHelper,
                                                                    VertexPointEstimator       = InDetConversionVtxPointEstimator,
                                                                    PostSelector               = InDetConversionPostSelector,
                                                                    SingleTrackConversionTool  = InDetSingleTrackConversion,
                                                                    Extrapolator               = theAtlasExtrapolator,
                                                                    TrackParticleCollection    = "InDetTrackParticles",
                                                                    RemoveTrtTracks            = False,
                                                                    IsConversion               = True)
        ToolSvc += MyInDetConversionFinderTools
        print      MyInDetConversionFinderTools

        from tauRec.tauRecConf import PhotonConversionVertex
        photonConv = PhotonConversionVertex(name                        = "PhotonConversionVertex",
                                    TauRecContainer                     = "TauRecContainer",
                                    TrackParticleContainer              = "InDetTrackParticles",
                                    OutputConversionVertexContainerName = "ConversionsVertex_Container",
                                    MaxTauJetDr                         = 0.5,
                                    ConversionFinderTool                = MyInDetConversionFinderTools)
        
        return photonConv

    else:
        from tauRec.tauRecConf import PhotonConversionPID
        photonConv = PhotonConversionPID(ConversionCandidatesName = "ConversionCandidate", 
                                         ConversionOutputName = "ConversionsPID_Container",
                                         ElectronProbability = 0.9)
        return photonConv

def getTauConversionTaggerTool():
    if jobproperties.tauRecFlags.useNewPIDBasedConvFinder():
        #Anything we need to do to run the tool
        #Handled in TauRecBuilder.py
        pass
    else:
        #Anything we need to do to avoid running the tool goes here
        #Handled in TauRecBuilder.py
        pass
    from tauRec.tauRecConf import TauConversionTagger
    TauConversionTagger = TauConversionTagger(ConversionTaggerVersion = 1)
    return TauConversionTagger
