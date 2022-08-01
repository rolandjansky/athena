# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration


from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory


def BPHY_TrkVKalVrtFitterCfg(flags, BPHYDerivationName):
    acc = ComponentAccumulator()
    from TrkConfig.AtlasExtrapolatorConfig import InDetExtrapolatorCfg
    extrap = acc.popToolsAndMerge(InDetExtrapolatorCfg(flags))
    tool = CompFactory.Trk.TrkVKalVrtFitter( name       = BPHYDerivationName+"_VKalVrtFitter",
                                                 Extrapolator        = extrap,
                                                 FirstMeasuredPoint  = False,
                                                 MakeExtendedVertex  = True)
    acc.setPrivateTools(tool)
    return acc



def BPHY_V0ToolCfg(flags, BPHYDerivationName):
    acc = ComponentAccumulator()
    tool =CompFactory.Trk.V0Tools(name = BPHYDerivationName + "_V0Tools",Extrapolator = None, DisableExtrapolator = True)
    acc.setPrivateTools(tool)
    return acc

def BPHY_VertexPointEstimatorCfg(flags, BPHYDerivationName):
    acc = ComponentAccumulator()
    tool = CompFactory.InDet.VertexPointEstimator(name = BPHYDerivationName+"_VtxPointEstimator",
                                        MinDeltaR              = [-10000.,-10000.,-10000.],
                                        MaxDeltaR              = [10000.,10000.,10000.],
                                        MaxPhi                 = [10000., 10000., 10000.],
                                        MaxChi2OfVtxEstimation = 2000.) #NOTE MaxChi2OfVtxEstimation differs from tracking default
    acc.setPrivateTools(tool)
    return acc

def BPHY_InDetDetailedTrackSelectorToolCfg(flags,BPHYDerivationName, **kwargs):
    acc = ComponentAccumulator()
    from TrkConfig.AtlasExtrapolatorConfig import InDetExtrapolatorCfg
    extrap = acc.popToolsAndMerge(InDetExtrapolatorCfg(flags))
    InDetTrackSelectorTool = CompFactory.InDet.InDetDetailedTrackSelectorTool(name = BPHYDerivationName+"_InDetDetailedTrackSelectorTool",
                                pTMin                = 400.0,
                                IPd0Max              = 10000.0,
                                IPz0Max              = 10000.0,
                                z0Max                = 10000.0,
                                sigIPd0Max           = 10000.0,
                                sigIPz0Max           = 10000.0,
                                d0significanceMax    = -1.,
                                z0significanceMax    = -1.,
                                etaMax               = 9999.,
                                useTrackSummaryInfo  = True,
                                nHitBLayer           = 0,
                                nHitPix              = 1,
                                nHitBLayerPlusPix    = 1,
                                nHitSct              = 2,
                                nHitSi               = 3,
                                nHitTrt              = 0,
                                nHitTrtHighEFractionMax = 10000.0,
                                useSharedHitInfo     = False,
                                useTrackQualityInfo  = True,
                                fitChi2OnNdfMax      = 10000.0,
                                TrtMaxEtaAcceptance  = 1.9,
                                UseEventInfoBS       = True,
                                TrackSummaryTool     = None,
                                Extrapolator         = extrap
                               )
    acc.setPrivateTools(InDetTrackSelectorTool)
    print("UseEventInfoBS       = True,")
    return acc


def BPHY_TrkV0VertexFitterCfg(flags) :
    acc = ComponentAccumulator()
    from TrkConfig.AtlasExtrapolatorConfig import InDetExtrapolatorCfg
    extrap = acc.popToolsAndMerge(InDetExtrapolatorCfg(flags))
    tool = CompFactory.Trk.TrkV0VertexFitter("TrkV0VertexFitter", MaxIterations=10, Use_deltaR=False, Extrapolator=extrap )
    acc.setPrivateTools(tool)
    return acc
