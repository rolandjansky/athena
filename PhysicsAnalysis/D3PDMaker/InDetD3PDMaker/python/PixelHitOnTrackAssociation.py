# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration

import InDetD3PDMaker
from D3PDMakerCoreComps.SimpleAssociation import SimpleAssociation
from D3PDMakerCoreComps.ContainedVectorMultiAssociation import ContainedVectorMultiAssociation

def PixelHitOnTrackAssociation (parent, associator, _prefix = '',
                              _blocknameprefix = '', _level = 0):
    assoc = ContainedVectorMultiAssociation\
            (parent, associator,
             prefix = _prefix, level = _level,
             blockname = _blocknameprefix+'clAssoc')
    assoc.defineBlock(0, _blocknameprefix+'clAssoc',
                      InDetD3PDMaker.ClusterAssocPixelFillerTool)

    return assoc
    
