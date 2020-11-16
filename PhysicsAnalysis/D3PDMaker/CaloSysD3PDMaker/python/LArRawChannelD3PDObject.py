# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

import CaloSysD3PDMaker
import D3PDMakerCoreComps
from D3PDMakerCoreComps.D3PDObject import D3PDObject


LArRawChannelSGKey='LArRawChannels'

def makeLArRawChannelD3PDObject (name, prefix, object_name='LArRawChannelD3PDObject', getter = None,
                           sgkey = None,
                           label = None):
    if sgkey is None: sgkey = LArRawChannelSGKey
    if label is None: label = prefix

    if not getter:
        getter = CaloSysD3PDMaker.LArRawChannelContainerGetterTool(name + '_Getter',
                  TypeName = 'LArRawChannelContainer',
                  SGKey = LArRawChannelSGKey)

    # create the selected cells
    return D3PDMakerCoreComps.VectorFillerTool (name,
                                                Prefix = prefix,
                                                Getter = getter,
                                                ObjectName = object_name)

LArRawChannelD3PDObject = D3PDObject (makeLArRawChannelD3PDObject, 'larrawchannel_', 'LArRawChannelD3PDObject')

LArRawChannelD3PDObject.defineBlock (0, 'Digits',
                                CaloSysD3PDMaker.LArRawChannelFillerTool)
