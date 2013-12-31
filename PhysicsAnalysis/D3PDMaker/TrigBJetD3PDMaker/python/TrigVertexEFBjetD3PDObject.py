# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from D3PDMakerCoreComps.D3PDObject import make_SGDataVector_D3PDObject
import TrigInDetD3PDMaker

TrigVertexContainer_EF_sgkey = 'HLT_EFHistoPrmVtx'
TrigVertexEFBjetD3PDObject = make_SGDataVector_D3PDObject( 'TrigVertexCollection',
                                                         'HLT_EFHistoPrmVtx',
                                                         'trig_EF_bjet_vertex_',
                                                         'TrigVertexEFBjetD3PDObject',
                                                         default_allowMissing = True )

#
# Information about the TrigVertex itself:
#
TrigVertexEFBjetD3PDObject.defineBlock( 0, 'BasicInfo',
                                  TrigInDetD3PDMaker.TrigVertexFillerTool,
                                  WriteBasicInfo = True )

TrigVertexEFBjetD3PDObject.defineBlock( 1, 'HypoInfo',
                                  TrigInDetD3PDMaker.TrigVertexFillerTool,
                                  WriteHypoInfo = True )

TrigVertexEFBjetD3PDObject.defineBlock( 2, 'DetailedInfo',
                                  TrigInDetD3PDMaker.TrigVertexFillerTool,
                                  WriteDetailedInfo = True )

