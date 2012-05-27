# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#
# D3PD object saving information about the LVL2 vertices
#

from D3PDMakerCoreComps.D3PDObject import make_SGDataVector_D3PDObject
from D3PDMakerCoreComps.SimpleAssociation import SimpleAssociation
import TrigInDetD3PDMaker

TrigVertexContainer_sgkey = 'HLT_TrigBeamSpotVertex'
TrigVertexD3PDObject = make_SGDataVector_D3PDObject( 'TrigVertexCollection',
                                                     TrigVertexContainer_sgkey,
                                                     'trig_L2_vertex_',
                                                     'TrigVertexD3PDObject' )

#
# Information about the TrigVertex itself:
#
TrigVertexD3PDObject.defineBlock( 0, 'BasicInfo',
                                  TrigInDetD3PDMaker.TrigVertexFillerTool,
                                  WriteBasicInfo = True )

TrigVertexD3PDObject.defineBlock( 1, 'HypoInfo',
                                  TrigInDetD3PDMaker.TrigVertexFillerTool,
                                  WriteHypoInfo = True )

TrigVertexD3PDObject.defineBlock( 2, 'DetailedInfo',
                                  TrigInDetD3PDMaker.TrigVertexFillerTool,
                                  WriteDetailedInfo = True )

#
# "Associate" TrigInDetTrackFitPar object to the TrigVertex. Then,
# save the properties of the fit parameter object using the dedicated
# filler tool.
#
FitParAssocToVertex = SimpleAssociation( TrigVertexD3PDObject,
                                         TrigInDetD3PDMaker.TrigInDetTrackFitParAtVertexAssociationTool,
                                         matched = "", blockname = "MotherParticleParameters",
                                         prefix = "motherTrack_" )
FitParAssocToVertex.defineBlock( 0, "FitParBasicInfo",
                                 TrigInDetD3PDMaker.TrigInDetTrackFitParFillerTool,
                                 WriteBasicInfo = True )
FitParAssocToVertex.defineBlock( 2, "FitParDetailedInfo",
                                 TrigInDetD3PDMaker.TrigInDetTrackFitParFillerTool,
                                 WriteDetailedInfo = True )
