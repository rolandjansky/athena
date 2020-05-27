# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration

#********************************************************************

# SoftBtagCommon.py

#********************************************************************

import AthenaCommon.Constants as Lvl
from AthenaCommon import Logging
ftaglog = Logging.logging.getLogger('SoftBtagCommon')

from DerivationFrameworkCore.DerivationFrameworkMaster import *
from AthenaCommon.GlobalFlags import globalflags

def applySoftBtagging(algname,sequence):

    from VrtSecInclusive.SoftBtagTrackSelector import SoftBtagTrackSelector
    SoftBtagTrackSelector = SoftBtagTrackSelector()
    SoftBtagTrackSelector.Jetcontainer         = "AntiKt4EMTopoJets"
    SoftBtagTrackSelector.InputTrackContainer  = "InDetTrackParticles"
    SoftBtagTrackSelector.OutputTrackContainer = "SoftBtagSeedTrackParticles"

    sequence +=  SoftBtagTrackSelector
    from AthenaCommon.AppMgr import ToolSvc

    from VrtSecInclusive.VrtSecInclusive import VrtSecInclusive
    VrtSecInclusive_SoftB = VrtSecInclusive("VrtSecInclusive_SoftB")

    sequence += VrtSecInclusive_SoftB

   # set options for vertexing
    VrtSecInclusive_SoftB.TrackLocation                          = "SoftBtagSeedTrackParticles"
    VrtSecInclusive_SoftB.SecondaryVerticesContainerName         = "SoftBtagCandidateVertices"
    VrtSecInclusive_SoftB.TruthParticleFilter                    = "Bhadron"
    VrtSecInclusive_SoftB.DoTwoTrSoftBtag                        = True
    VrtSecInclusive_SoftB.TwoTrVrtAngleCut                       = 0.7
    VrtSecInclusive_SoftB.twoTrVrtMinDistFromPV                  = 0.2
    VrtSecInclusive_SoftB.PassThroughTrackSelection              = True
    VrtSecInclusive_SoftB.ImpactWrtBL                            = False
    VrtSecInclusive_SoftB.doPVcompatibilityCut                   = False
    VrtSecInclusive_SoftB.RemoveFake2TrkVrt                      = True
    VrtSecInclusive_SoftB.doReassembleVertices                   = False
    VrtSecInclusive_SoftB.doMergeByShuffling                     = False
    VrtSecInclusive_SoftB.doMergeFinalVerticesDistance           = False
    VrtSecInclusive_SoftB.doAssociateNonSelectedTracks           = False
    VrtSecInclusive_SoftB.doFinalImproveChi2                     = False
    VrtSecInclusive_SoftB.DoTruth                                = False
    VrtSecInclusive_SoftB.SelVrtChi2Cut                          = 5.
    VrtSecInclusive_SoftB.SelTrkMaxCutoff                        = 30
    VrtSecInclusive_SoftB.twoTrkVtxFormingD0Cut                  = 0.

    from TrkVKalVrtFitter.TrkVKalVrtFitterConf import Trk__TrkVKalVrtFitter
    InclusiveVxFitterTool = Trk__TrkVKalVrtFitter(name                = "InclusiveVxFitter",
                                                  Extrapolator        = ToolSvc.AtlasExtrapolator,
                                                  IterationNumber     = 30
                                                  )
    ToolSvc +=  InclusiveVxFitterTool;

    VrtSecInclusive_SoftB.VertexFitterTool=InclusiveVxFitterTool
    VrtSecInclusive_SoftB.Extrapolator = ToolSvc.AtlasExtrapolator

