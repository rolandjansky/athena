# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from D3PDMakerCoreComps.D3PDObject import make_SGDataVector_D3PDObject
import TrigInDetD3PDMaker

TrigVertexContainer_sgkey = 'HLT_T2HistoPrmVtx'
TrigVertexBjetD3PDObject = make_SGDataVector_D3PDObject( 'TrigVertexCollection',
                                                         'HLT_T2HistoPrmVtx',
                                                         'trig_L2_bjet_vertex_',
                                                         'TrigVertexBjetD3PDObject',
                                                         default_allowMissing = True )

#
# Information about the TrigVertex itself:
#
TrigVertexBjetD3PDObject.defineBlock( 0, 'BasicInfo',
                                  TrigInDetD3PDMaker.TrigVertexFillerTool,
                                  WriteBasicInfo = True )

TrigVertexBjetD3PDObject.defineBlock( 1, 'HypoInfo',
                                  TrigInDetD3PDMaker.TrigVertexFillerTool,
                                  WriteHypoInfo = True )

TrigVertexBjetD3PDObject.defineBlock( 2, 'DetailedInfo',
                                  TrigInDetD3PDMaker.TrigVertexFillerTool,
                                  WriteDetailedInfo = True )

