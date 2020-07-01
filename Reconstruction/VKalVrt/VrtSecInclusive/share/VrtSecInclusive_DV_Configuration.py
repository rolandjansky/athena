###  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# File for feeding Reco_tf.py vertexing options for the searches 
# looking for displaced vertices in the SUSY and Exotics groups.
# The options here are needed both when running on RAW and ESD inputs.
# Must run after the large-radius tracking to use large-d0 tracks.

## get a handle on the top sequence of algorithms -import AlgSequence
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

# instantiate the vertexing alg
from VrtSecInclusive.VrtSecInclusive import VrtSecInclusive
VrtSecInclusive_InDet = VrtSecInclusive("VrtSecInclusive_InDet")
VrtSecInclusive_leptons  = VrtSecInclusive("VrtSecInclusive_leptons")

try:
    end_idx = [_.getName() for _ in topSequence].index('StreamESD')
except ValueError:
    try:
        end_idx = [_.getName() for _ in topSequence].index('StreamAOD')
    except ValueError:
        local_logger = logging.getLogger('VrtSecInclusive_DV_Configuration')
        local_logger.warning('Neither StreamESD nor StreamAOD found, VrtSecInclusive algs will be scheduled one before end of topSequence. Probably wrong!')
        end_idx = -1
        del local_logger
topSequence.insert(end_idx, VrtSecInclusive_InDet)
topSequence.insert(end_idx, VrtSecInclusive_leptons)
del end_idx

# set options for vertexing
VrtSecInclusive_InDet.do_PVvetoCut                           = True
VrtSecInclusive_InDet.do_d0Cut                               = False
VrtSecInclusive_InDet.do_z0Cut                               = False
VrtSecInclusive_InDet.do_d0errCut                            = False
VrtSecInclusive_InDet.do_z0errCut                            = False
VrtSecInclusive_InDet.do_d0signifCut                         = False
VrtSecInclusive_InDet.do_z0signifCut                         = False
VrtSecInclusive_InDet.doTRTPixCut                            = True
VrtSecInclusive_InDet.DoSAloneTRT                            = False
VrtSecInclusive_InDet.ImpactWrtBL                            = True
VrtSecInclusive_InDet.DoPVcompatibility                      = True
VrtSecInclusive_InDet.RemoveFake2TrkVrt                      = True
VrtSecInclusive_InDet.CheckHitPatternStrategy                = 'ExtrapolationAssist' # Either 'Classical', 'Extrapolation' or 'ExtrapolationAssist'
VrtSecInclusive_InDet.doReassembleVertices                   = True
VrtSecInclusive_InDet.doMergeByShuffling                     = True
VrtSecInclusive_InDet.doMergeFinalVerticesDistance           = True
VrtSecInclusive_InDet.doAssociateNonSelectedTracks           = True
VrtSecInclusive_InDet.doFinalImproveChi2                     = False
VrtSecInclusive_InDet.DoTruth                                = (globalflags.DataSource == 'geant4' and globalflags.InputFormat == "pool")
VrtSecInclusive_InDet.FillHist                               = True
VrtSecInclusive_InDet.FillIntermediateVertices               = False
VrtSecInclusive_InDet.CutPixelHits                           = 0
VrtSecInclusive_InDet.CutSctHits                             = 2
VrtSecInclusive_InDet.TrkA0ErrCut                            = 200000
VrtSecInclusive_InDet.TrkZErrCut                             = 200000
VrtSecInclusive_InDet.a0TrkPVDstMinCut                       = 2.0    # track d0 min
VrtSecInclusive_InDet.a0TrkPVDstMaxCut                       = 300.0  # track d0 max: default is 1000.0
VrtSecInclusive_InDet.zTrkPVDstMinCut                        = 0.0    # track z0 min: default is 0.0, just for clarification
VrtSecInclusive_InDet.zTrkPVDstMaxCut                        = 1500.0 # track z0 max: default is 1000.0
VrtSecInclusive_InDet.twoTrkVtxFormingD0Cut                  = 2.0
VrtSecInclusive_InDet.TrkPtCut                               = 1000
VrtSecInclusive_InDet.SelVrtChi2Cut                          = 5.
VrtSecInclusive_InDet.CutSharedHits                          = 2
VrtSecInclusive_InDet.TrkChi2Cut                             = 50
VrtSecInclusive_InDet.TruthTrkLen                            = 1
VrtSecInclusive_InDet.SelTrkMaxCutoff                        = 2000
VrtSecInclusive_InDet.mergeByShufflingAllowance              = 10.
VrtSecInclusive_InDet.associatePtCut                         = 1000.
VrtSecInclusive_InDet.associateMinDistanceToPV               = 2.
VrtSecInclusive_InDet.associateMaxD0Signif                   = 5.
VrtSecInclusive_InDet.associateMaxZ0Signif                   = 5.
VrtSecInclusive_InDet.VertexMergeFinalDistCut                = 1.
VrtSecInclusive_InDet.VertexMergeFinalDistScaling            = 0.
VrtSecInclusive_InDet.improveChi2ProbThreshold               = 0.0001
VrtSecInclusive_InDet.doAugmentDVimpactParametersToMuons     = True
VrtSecInclusive_InDet.doAugmentDVimpactParametersToElectrons = True


VrtSecInclusive_leptons.doSelectTracksFromMuons                = True
VrtSecInclusive_leptons.doSelectTracksFromElectrons            = True
VrtSecInclusive_leptons.AugmentingVersionString                = "_Leptons"
VrtSecInclusive_leptons.do_PVvetoCut                           = True
VrtSecInclusive_leptons.do_d0Cut                               = True
VrtSecInclusive_leptons.do_z0Cut                               = False
VrtSecInclusive_leptons.do_d0errCut                            = False
VrtSecInclusive_leptons.do_z0errCut                            = False
VrtSecInclusive_leptons.do_d0signifCut                         = False
VrtSecInclusive_leptons.do_z0signifCut                         = False
VrtSecInclusive_leptons.doTRTPixCut                            = False
VrtSecInclusive_leptons.DoSAloneTRT                            = False
VrtSecInclusive_leptons.ImpactWrtBL                            = False
VrtSecInclusive_leptons.DoPVcompatibility                      = True
VrtSecInclusive_leptons.RemoveFake2TrkVrt                      = True
VrtSecInclusive_leptons.CheckHitPatternStrategy                = 'ExtrapolationAssist' # Either 'Classical', 'Extrapolation' or 'ExtrapolationAssist'
VrtSecInclusive_leptons.doReassembleVertices                   = True
VrtSecInclusive_leptons.doMergeByShuffling                     = True
VrtSecInclusive_leptons.doMergeFinalVerticesDistance           = True
VrtSecInclusive_leptons.doAssociateNonSelectedTracks           = False
VrtSecInclusive_leptons.doFinalImproveChi2                     = False
VrtSecInclusive_leptons.DoTruth                                = (globalflags.DataSource == 'geant4' and globalflags.InputFormat == "pool")
VrtSecInclusive_leptons.FillHist                               = True
VrtSecInclusive_leptons.FillIntermediateVertices               = False
VrtSecInclusive_leptons.CutPixelHits                           = 0
VrtSecInclusive_leptons.CutSctHits                             = 2
VrtSecInclusive_leptons.TrkA0ErrCut                            = 200000
VrtSecInclusive_leptons.TrkZErrCut                             = 200000
VrtSecInclusive_leptons.a0TrkPVDstMinCut                       = 0.0    # track d0 min
VrtSecInclusive_leptons.a0TrkPVDstMaxCut                       = 300.0  # track d0 max: default is 1000.0
VrtSecInclusive_leptons.zTrkPVDstMinCut                        = 0.0    # track z0 min: default is 0.0, just for clarification
VrtSecInclusive_leptons.zTrkPVDstMaxCut                        = 1500.0 # track z0 max: default is 1000.0
VrtSecInclusive_leptons.twoTrkVtxFormingD0Cut                  = 0.0
VrtSecInclusive_leptons.TrkPtCut                               = 1000
VrtSecInclusive_leptons.SelVrtChi2Cut                          = 5.
VrtSecInclusive_leptons.CutSharedHits                          = 2
VrtSecInclusive_leptons.TrkChi2Cut                             = 50
VrtSecInclusive_leptons.TruthTrkLen                            = 1
VrtSecInclusive_leptons.SelTrkMaxCutoff                        = 2000
VrtSecInclusive_leptons.mergeByShufflingAllowance              = 5.
VrtSecInclusive_leptons.associatePtCut                         = 1000.
VrtSecInclusive_leptons.VertexMergeFinalDistCut                = 1.
VrtSecInclusive_leptons.VertexMergeFinalDistScaling            = 0.
VrtSecInclusive_leptons.improveChi2ProbThreshold               = 0.0001


# set options related to the vertex fitter
from TrkVKalVrtFitter.TrkVKalVrtFitterConf import Trk__TrkVKalVrtFitter
InclusiveVxFitterTool = Trk__TrkVKalVrtFitter(name                = "InclusiveVxFitter",
                                              Extrapolator        = ToolSvc.AtlasExtrapolator,
                                              IterationNumber     = 30,
                                              AtlasMagFieldSvc    = "AtlasFieldSvc"
                                             )
ToolSvc +=  InclusiveVxFitterTool;
InclusiveVxFitterTool.OutputLevel = INFO

VrtSecInclusive_InDet.VertexFitterTool=InclusiveVxFitterTool
VrtSecInclusive_InDet.Extrapolator = ToolSvc.AtlasExtrapolator

VrtSecInclusive_leptons.VertexFitterTool=InclusiveVxFitterTool
VrtSecInclusive_leptons.Extrapolator = ToolSvc.AtlasExtrapolator

# tell VrtSecInclusive the interface name for Trk::IVertexMapper
from TrkDetDescrTestTools.TrkDetDescrTestToolsConf import Trk__VertexMapper
HadronicVertexMapper = Trk__VertexMapper("HadronicVertexMapper")
ToolSvc += HadronicVertexMapper

VrtSecInclusive_InDet.VertexMapper    = HadronicVertexMapper
VrtSecInclusive_leptons.VertexMapper  = HadronicVertexMapper


include("VrtSecInclusive/VrtSecInclusive_DV_postInclude.py")
