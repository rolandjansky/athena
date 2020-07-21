###  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

import AthenaCommon.Constants as Lvl

from AthenaCommon.GlobalFlags import globalflags

def setupVSI( suffix ):

    from VrtSecInclusiveConf import VKalVrtAthena__VrtSecInclusive 
    vsiInstance = VKalVrtAthena__VrtSecInclusive("VrtSecInclusive_"+suffix)

    vsiInstance.AugmentingVersionString = "_"+suffix
    vsiInstance.OutputLevel                            = Lvl.INFO
    vsiInstance.do_PVvetoCut                           = True
    vsiInstance.do_d0Cut                               = False
    vsiInstance.do_z0Cut                               = False
    vsiInstance.do_d0errCut                            = False
    vsiInstance.do_z0errCut                            = False
    vsiInstance.do_d0signifCut                         = False
    vsiInstance.do_z0signifCut                         = False
    vsiInstance.doTRTPixCut                            = True
    vsiInstance.DoSAloneTRT                            = False
    vsiInstance.ImpactWrtBL                            = True
    vsiInstance.DoPVcompatibility                      = True
    vsiInstance.RemoveFake2TrkVrt                      = True
    vsiInstance.CheckHitPatternStrategy                = 'ExtrapolationAssist' # Either 'Classical', 'Extrapolation' or 'ExtrapolationAssist'
    vsiInstance.doReassembleVertices                   = True
    vsiInstance.doMergeByShuffling                     = True
    vsiInstance.doMergeFinalVerticesDistance           = True
    vsiInstance.doAssociateNonSelectedTracks           = True
    vsiInstance.doFinalImproveChi2                     = False
    vsiInstance.DoTruth                                = (globalflags.DataSource == 'geant4' and globalflags.InputFormat == "pool")
    vsiInstance.FillHist                               = True
    vsiInstance.FillIntermediateVertices               = False
    vsiInstance.CutPixelHits                           = 0
    vsiInstance.CutSctHits                             = 2
    vsiInstance.TrkA0ErrCut                            = 200000
    vsiInstance.TrkZErrCut                             = 200000
    vsiInstance.a0TrkPVDstMinCut                       = 2.0    # track d0 min
    vsiInstance.a0TrkPVDstMaxCut                       = 300.0  # track d0 max: default is 1000.0
    vsiInstance.zTrkPVDstMinCut                        = 0.0    # track z0 min: default is 0.0, just for clarification
    vsiInstance.zTrkPVDstMaxCut                        = 1500.0 # track z0 max: default is 1000.0
    vsiInstance.twoTrkVtxFormingD0Cut                  = 2.0
    vsiInstance.TrkPtCut                               = 1000
    vsiInstance.SelVrtChi2Cut                          = 5.
    vsiInstance.CutSharedHits                          = 2
    vsiInstance.TrkChi2Cut                             = 50
    vsiInstance.TruthTrkLen                            = 1
    vsiInstance.SelTrkMaxCutoff                        = 2000
    vsiInstance.mergeByShufflingAllowance              = 10.
    vsiInstance.associatePtCut                         = 1000.
    vsiInstance.associateMinDistanceToPV               = 2.
    vsiInstance.associateMaxD0Signif                   = 5.
    vsiInstance.associateMaxZ0Signif                   = 5.
    vsiInstance.VertexMergeFinalDistCut                = 1.
    vsiInstance.VertexMergeFinalDistScaling            = 0.
    vsiInstance.improveChi2ProbThreshold               = 0.0001
    vsiInstance.doAugmentDVimpactParametersToMuons     = True
    vsiInstance.doAugmentDVimpactParametersToElectrons = True

    return vsiInstance

