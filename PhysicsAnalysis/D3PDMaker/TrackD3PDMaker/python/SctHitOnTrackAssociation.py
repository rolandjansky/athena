# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import TrackD3PDMaker
from TrackD3PDMaker.TrackD3PDMakerConf import D3PD__ClusterAssocFillerTool as ClusterAssocFillerTool
from D3PDMakerCoreComps.SimpleAssociation import SimpleAssociation
from D3PDMakerCoreComps.ContainedVectorMultiAssociation import ContainedVectorMultiAssociation

def SctHitOnTrackAssociation (parent, associator, _prefix = '',
                              _blocknameprefix = '', _level = 0):
    assoc = ContainedVectorMultiAssociation\
            (parent, associator,
             prefix = _prefix, level = _level,
             blockname = _blocknameprefix+'clAssoc')
    assoc.defineBlock(0, _blocknameprefix+'clAssoc',
                      TrackD3PDMaker.ClusterAssocFillerTool)

    return assoc
    
