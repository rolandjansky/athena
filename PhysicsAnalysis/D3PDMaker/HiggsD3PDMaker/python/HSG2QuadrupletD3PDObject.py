# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id: HSG2QuadrupletD3PDObject.py 520030 2012-10-03 10:11:57Z krasznaa $

# D3PDMaker import(s):
from D3PDMakerCoreComps.D3PDObject import make_SGDataVector_D3PDObject
from D3PDMakerCoreComps.IndexAssociation import IndexAssociation
from D3PDMakerCoreComps.IndexMultiAssociation import IndexMultiAssociation
from D3PDMakerCoreComps.SimpleAssociation import SimpleAssociation
import HiggsD3PDMaker
import EventCommonD3PDMaker

# Hook setting up the vertex reconstruction in the pre-sequence:
def __vertexReco( c, **kw ):
    from D3PDMakerConfig.D3PDMakerFlags import D3PDMakerFlags
    from AthenaCommon.AlgSequence import AlgSequence
    from HSG2VertexCreation.HSG2VertexReconstruction import HSG2VertexReconstruction
    HSG2VertexReconstruction( "HSG2VertexReconstruction",
                              AlgSequence( D3PDMakerFlags.PreD3PDAlgSeqName() ) )
    return

# Create the D3PDObject:
HSG2QuadrupletD3PDObject = make_SGDataVector_D3PDObject( "QuadrupletContainer",
                                                         "QuadrupletCandidates",
                                                         "quad_",
                                                         "HSG2QuadrupletD3PDObject" )

# Define the vertex reconstruction hook:
HSG2QuadrupletD3PDObject.defineHook( __vertexReco )

# The basic information:
HSG2QuadrupletD3PDObject.defineBlock( 0, "BaseInfo",
                                      HiggsD3PDMaker.QuadrupletFillerTool )

# Associate the reconstructed vertices:
VtxAssoc = IndexAssociation( HSG2QuadrupletD3PDObject,
                             HiggsD3PDMaker.QuadrupletVertexAssociationTool,
                             target = "quad_vertex_",
                             blockname = "VertexIndex",
                             prefix = "vtx_",
                             level = 1 )

# Associate the CombinedParticles:
CombAssoc = SimpleAssociation( HSG2QuadrupletD3PDObject,
                               HiggsD3PDMaker.QuadrupletCompositeParticleAssociationTool,
                               blockname = "CombPartMatch",
                               prefix = "cbpart_",
                               level = 1 )

# Save the 4-momentum of the combined particles:
CombAssoc.defineBlock( 1, "ComPart4Mom",
                       EventCommonD3PDMaker.FourMomFillerTool )

# Save the indices of all the electrons that took part in the combination:
ElAssoc = IndexMultiAssociation( CombAssoc,
                                 EventCommonD3PDMaker.CompositeParticleAssociationTool,
                                 target = "el_",
                                 blockname = "ElectronIndex",
                                 prefix = "el_",
                                 level = 2 )

# Save the indices of all the MuID muons that took part in the combination:
MuidAssoc = IndexMultiAssociation( CombAssoc,
                                   EventCommonD3PDMaker.CompositeParticleAssociationTool,
                                   target = "mu_muid_",
                                   blockname = "MuonMuidIndex",
                                   prefix = "mu_muid_",
                                   level = 2 )

# Save the indices of all the Staco muons that took part in the combination:
StacoAssoc = IndexMultiAssociation( CombAssoc,
                                    EventCommonD3PDMaker.CompositeParticleAssociationTool,
                                    target = "mu_staco_",
                                    blockname = "MuonStacoIndex",
                                    prefix = "mu_staco_",
                                    level = 2 )

# Save the indices of all the Calo muons that took part in the combination:
CaloAssoc = IndexMultiAssociation( CombAssoc,
                                   EventCommonD3PDMaker.CompositeParticleAssociationTool,
                                   target = "mu_calo_",
                                   blockname = "MuonCaloIndex",
                                   prefix = "mu_calo_",
                                   level = 2 )

# Save the indices of all the third chain muons that took part in the combination:
MuonAssoc = IndexMultiAssociation( CombAssoc,
                                   EventCommonD3PDMaker.CompositeParticleAssociationTool,
                                   target = "mu_muon_",
                                   blockname = "MuonIndex",
                                   prefix = "mu_muon_",
                                   level = 2 )
