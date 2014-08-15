# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id$

import CaloD3PDMaker
import D3PDMakerCoreComps
from D3PDMakerCoreComps.D3PDObject import D3PDObject


LArRawChannelSGKey='LArRawChannels'

def makeLArRawChannelD3PDObject (name, prefix, object_name='LArRawChannelD3PDObject', getter = None,
                           sgkey = None,
                           label = None):
    if sgkey == None: sgkey = LArRawChannelSGKey
    if label == None: label = prefix

    if not getter:
        getter = CaloD3PDMaker.LArRawChannelContainerGetterTool(name + '_Getter',
                  TypeName = 'LArRawChannelContainer',
                  SGKey = LArRawChannelSGKey)

    # create the selected cells
    from D3PDMakerConfig.D3PDMakerFlags import D3PDMakerFlags
    return D3PDMakerCoreComps.VectorFillerTool (name,
                                                Prefix = prefix,
                                                Getter = getter,
                                                ObjectName = object_name)

LArRawChannelD3PDObject = D3PDObject (makeLArRawChannelD3PDObject, 'larrawchannel_', 'LArRawChannelD3PDObject')

LArRawChannelD3PDObject.defineBlock (0, 'Digits',
                                CaloD3PDMaker.LArRawChannelFillerTool)
