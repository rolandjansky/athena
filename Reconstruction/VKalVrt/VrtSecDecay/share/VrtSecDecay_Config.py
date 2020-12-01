#
# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#

# --------------------------------------------------------------------------------------------- #
# Configuration of secondary vertex finding for charged tracks orginating from the primary      #
# vertex and decaying in inner detector (optmized for decays beyond the 3rd pixel layer         #
# Requires two input track containers                                                           #
# --------------------------------------------------------------------------------------------- #

# Get ToolSvc and topSequence
from AthenaCommon.AppMgr import ToolSvc
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

from InDetRecExample.InDetKeys import InDetKeys

#
# --- load vertex fitter
#
from TrkVKalVrtFitter.TrkVKalVrtFitterConf import Trk__TrkVKalVrtFitter
InDetSecVxFitterTool = Trk__TrkVKalVrtFitter(name                = "InclusiveVxFitter",
                                             Extrapolator        = ToolSvc.AtlasExtrapolator,
                                             IterationNumber     = 30,
                                             AtlasMagFieldSvc    = "AtlasFieldSvc" )

ToolSvc += InDetSecVxFitterTool

#
# Add driving algorithm
#
from VrtSecDecay.VrtSecDecayConf import VKalVrtAthena__VrtSecDecay
VrtSecDecayFinder = VKalVrtAthena__VrtSecDecay(name                         = "VrtSecDecayFinder",
                                               VertexFitterTool             = InDetSecVxFitterTool,
                                               OutputVtxContainer           = InDetKeys.xAODSecVertexDecayContainer(),
                                               ParentxAODContainer          = InDetKeys.xAODPixelPrdAssociationTrackParticleContainer(),
                                               ChildxAODContainer           = InDetKeys.xAODTrackParticleContainer(),
                                               DoHitPatternMatching         = False,
                                               ParentChildMaxDr             = 1.5,
                                               MaxVtxPerEvent               = 200,
                                               VtxQuality                   = 10.0,
                                               VtxMinRadius                 = 85.0,
                                               VtxMaxRadius                 = 300.0,
                                               ParentPt                     = 20000.0,
                                               ParentMinEta                 = 0.1,
                                               ParentMaxEta                 = 20.0,
                                               ChildMinPt                   = 100.0,
                                               ChildMaxPt                   = 150000000.0,
                                               ChildBLayerHits              = 0,
                                               ChildNextToInPixLayerHits    = 0,
                                               ChildPixHits                 = 1,
                                               ChildSCTHits                 = 6 )

# Add to top sequence
topSequence += VrtSecDecayFinder 
