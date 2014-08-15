# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id: TileRawChannelD3PDObject.py 470396 2011-11-24 16:42:13Z krasznaa $

import CaloD3PDMaker
import D3PDMakerCoreComps
from D3PDMakerCoreComps.D3PDObject import D3PDObject

TileRawChannelSGKey='TileRawChannelFlt'

def makeTileRawChannelD3PDObject (name, prefix, object_name='TileRawChannelD3PDObject', getter = None,
                           sgkey = None,
                           label = None):
    if sgkey == None: sgkey = TileRawChannelSGKey
    if label == None: label = prefix

    if not getter:
        getter = CaloD3PDMaker.SGTileRawChannelGetterTool \
                 (name + '_Getter',
                  TypeName = 'TileRawChannelContainer',
                  SGKey = sgkey,
                  Label = label)

    # create the selected cells
    from D3PDMakerConfig.D3PDMakerFlags import D3PDMakerFlags
    return D3PDMakerCoreComps.VectorFillerTool (name,
                                                Prefix = prefix,
                                                Getter = getter,
                                                ObjectName = object_name,
                                                SaveMetadata = \
                                                D3PDMakerFlags.SaveObjectMetadata())



""" level of details:
0: amp, time, quality and pedestal
1: hardware id + amp, time, quality and pedestal

"""

TileRawChannelD3PDObject = D3PDObject( makeTileRawChannelD3PDObject, 'tileraw_',
                                       'TileRawChannelD3PDObject' )

TileRawChannelD3PDObject.defineBlock (0, 'RawChannel',
                                CaloD3PDMaker.TileRawChannelFillerTool,
                                SaveHardwareInfo=False,
                                SaveRawChannel= True,
                                )



TileRawChannelD3PDObject.defineBlock (1, 'Hardware',
                                CaloD3PDMaker.TileRawChannelFillerTool,
                                SaveHardwareInfo=True,
                                SaveRawChannel= False,
                                )




