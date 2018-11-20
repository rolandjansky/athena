# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from InD3PDMaker import ClusterAssocFillerTool
from D3PDMakerCoreComps.SimpleAssociation import SimpleAssociation
from D3PDMakerCoreComps.ContainedVectorMultiAssociation import ContainedVectorMultiAssociation

def SctHitOnTrackAssociation (parent, associator, _prefix = '',
                              _blocknameprefix = '', _level = 0):
    assoc = ContainedVectorMultiAssociation\
            (parent, associator,
             prefix = _prefix, level = _level,
             blockname = _blocknameprefix+'clAssoc')
    assoc.defineBlock(0, _blocknameprefix+'clAssoc',
                      ClusterAssocFillerTool)

    return assoc
    
