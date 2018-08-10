#
# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
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

#
# --- load vertex fitter
#
from TrkVKalVrtFitter.TrkVKalVrtFitterConf import Trk__TrkVKalVrtFitter
InDetSecVxFitterTool = Trk__TrkVKalVrtFitter(name                = "InclusiveVxFitter",
                                             Extrapolator        = ToolSvc.AtlasExtrapolator,
                                             IterationNumber     = 30,
                                             AtlasMagFieldSvc    = "AtlasFieldSvc" )


                                             #FirstMeasuredPoint         = VertexCuts.Fitter_FirstMeasuredPoint(),
                                             #Robustness                 = VertexCuts.Fitter_Robustness(),
                                             #InputParticleMasses        = VertexCuts.Fitter_InputParticleMasses(),
                                             #VertexForConstraint        = VertexCuts.Fitter_VertexForConstraint(),
                                             #CovVrtForConstraint        = VertexCuts.Fitter_CovVrtForConstraint(),
                                             #FirstMeasuredPointLimit    = VertexCuts.FirstMeasuredPointLimit(),
                                             #usePhiCnst                 = VertexCuts.usePhiCnst(),
                                             #useThetaCnst               = VertexCuts.useThetaCnst())

ToolSvc += InDetSecVxFitterTool

#
# Add driving algorithm
#
from VrtSecDecay.VrtSecDecayConf import VKalVrtAthena__VrtSecDecay
VrtSecDecayFinder = VKalVrtAthena__VrtSecDecay(name                 = "VrtSecDecayFinder",
                                               VertexFitterTool     = InDetSecVxFitterTool )

# Add to top sequence!
topSequence += VrtSecDecayFinder 

#topSequence.StreamESD.ItemList+=["xAOD::VertexContainer#VrtSecDecay"]
#topSequence.StreamESD.ItemList+=["xAOD::VertexAuxContainer#VrtSecDecayAux."]

# TODO: These are only here for now... move eventually
#StreamESD.ItemList+=["xAOD::VertexContainer#VrtSecDecay"]
#StreamESD.ItemList+=["xAOD::VertexAuxContainer#VrtSecDecayAux."]
#InDetESDList+=["xAOD::VertexContainer*"]
#InDetESDList+=["xAOD::VertexAuxContainer*"]

# DONE!
