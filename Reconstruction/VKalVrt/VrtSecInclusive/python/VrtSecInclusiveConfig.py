"""Define method to configure and test SCT_ConditionsSummaryTestAlg

Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
"""

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
import AthenaCommon.Constants as Lvl
from AthenaCommon.GlobalFlags import globalflags

def VrtSecInclusiveCfg(flags, name="VrtSecInclusive", **kwargs):

    """Return a configured VrtSecInclusive algorithm instance"""
    acc = ComponentAccumulator()

    from TrkConfig.AtlasExtrapolatorConfig import AtlasExtrapolatorCfg
    from TrkConfig.TrkVertexFitterUtilsConfig import TrackToVertexIPEstimatorCfg
    from InDetConfig.TrackRecoConfig import TrackToVertexCfg
    from TrkConfig.TrkVKalVrtFitterConfig import TrkVKalVrtFitterCfg
    from PixelConditionsTools.PixelConditionsSummaryConfig import PixelConditionsSummaryCfg


    kwargs.setdefault("Extrapolator"                 , acc.popToolsAndMerge(AtlasExtrapolatorCfg(flags)))
    kwargs.setdefault("VertexFitterTool"             , acc.popToolsAndMerge(TrkVKalVrtFitterCfg(flags)))# , IterationNumber = 30)))
    kwargs.setdefault("PixelConditionsSummaryTool"   , acc.popToolsAndMerge(PixelConditionsSummaryCfg(flags, UseByteStreamFEI4 = False, UseByteStreamFEI3 = False)))

    TrackToVertexTool = acc.popToolsAndMerge(TrackToVertexCfg(flags))
    acc.addPublicTool(TrackToVertexTool)
    kwargs.setdefault("TrackToVertexTool"            , TrackToVertexTool)

    TrackToVertexIPEstimatorTool = acc.popToolsAndMerge(TrackToVertexIPEstimatorCfg(flags))
    acc.addPublicTool(TrackToVertexIPEstimatorTool)
    kwargs.setdefault("TrackToVertexIPEstimatorTool" , TrackToVertexIPEstimatorTool)

    kwargs.setdefault("AugmentingVersionString"                , "")
    kwargs.setdefault("TrackLocation"                          , "InDetTrackParticles")
    kwargs.setdefault("OutputLevel"                            , Lvl.INFO)
    kwargs.setdefault("do_PVvetoCut"                           , True)
    kwargs.setdefault("do_d0Cut"                               , False)
    kwargs.setdefault("do_z0Cut"                               , False)
    kwargs.setdefault("do_d0errCut"                            , False)
    kwargs.setdefault("do_z0errCut"                            , False)
    kwargs.setdefault("do_d0signifCut"                         , False)
    kwargs.setdefault("do_z0signifCut"                         , False)
    kwargs.setdefault("doTRTPixCut"                            , True)
    kwargs.setdefault("DoSAloneTRT"                            , False)
    kwargs.setdefault("ImpactWrtBL"                            , True)
    kwargs.setdefault("DoPVcompatibility"                      , True)
    kwargs.setdefault("RemoveFake2TrkVrt"                      , True)
    kwargs.setdefault("CheckHitPatternStrategy"                , 'ExtrapolationAssist') # Either 'Classical', 'Extrapolation' or 'ExtrapolationAssist'
    kwargs.setdefault("doReassembleVertices"                   , True)
    kwargs.setdefault("doMergeByShuffling"                     , True)
    kwargs.setdefault("doMergeFinalVerticesDistance"           , True)
    kwargs.setdefault("doAssociateNonSelectedTracks"           , True)
    kwargs.setdefault("doFinalImproveChi2"                     , False)
    kwargs.setdefault("DoTruth"                                , (globalflags.DataSource == 'geant4' and globalflags.InputFormat == "pool"))
    kwargs.setdefault("FillHist"                               , True)
    kwargs.setdefault("FillNtuple"                             , False)
    kwargs.setdefault("TruthParticleFilter"                    , "Higgs")
    kwargs.setdefault("FillIntermediateVertices"               , False)
    kwargs.setdefault("CutPixelHits"                           , 0)
    kwargs.setdefault("CutSctHits"                             , 2)
    kwargs.setdefault("TrkA0ErrCut"                            , 200000)
    kwargs.setdefault("TrkZErrCut"                             , 200000)
    kwargs.setdefault("a0TrkPVDstMinCut"                       , 2.0)    # track d0 min
    kwargs.setdefault("a0TrkPVDstMaxCut"                       , 300.0)  # track d0 max: default is 1000.0
    kwargs.setdefault("zTrkPVDstMinCut"                        , 0.0)    # track z0 min: default is 0.0, just for clarification
    kwargs.setdefault("zTrkPVDstMaxCut"                        , 1500.0) # track z0 max: default is 1000.0
    kwargs.setdefault("twoTrkVtxFormingD0Cut"                  , 2.0)
    kwargs.setdefault("TrkPtCut"                               , 1000)
    kwargs.setdefault("SelVrtChi2Cut"                          , 5.)
    kwargs.setdefault("CutSharedHits"                          , 2)
    kwargs.setdefault("TrkChi2Cut"                             , 50)
    kwargs.setdefault("TruthTrkLen"                            , 1)
    kwargs.setdefault("SelTrkMaxCutoff"                        , 2000)
    kwargs.setdefault("mergeByShufflingAllowance"              , 10.)
    kwargs.setdefault("associatePtCut"                         , 1000.)
    kwargs.setdefault("associateMinDistanceToPV"               , 2.)
    kwargs.setdefault("associateMaxD0Signif"                   , 5.)
    kwargs.setdefault("associateMaxZ0Signif"                   , 5.)
    kwargs.setdefault("VertexMergeFinalDistCut"                , 1.)
    kwargs.setdefault("VertexMergeFinalDistScaling"            , 0.)
    kwargs.setdefault("improveChi2ProbThreshold"               , 0.0001)
    kwargs.setdefault("doAugmentDVimpactParametersToMuons"     , False)
    kwargs.setdefault("doAugmentDVimpactParametersToElectrons" , False)

    acc.addEventAlgo(CompFactory.VKalVrtAthena.VrtSecInclusive(name, **kwargs))
    return acc

