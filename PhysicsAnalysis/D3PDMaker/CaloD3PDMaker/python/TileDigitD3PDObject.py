# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id: TileDigitD3PDObject.py 470396 2011-11-24 16:42:13Z krasznaa $

import CaloD3PDMaker
import D3PDMakerCoreComps
from D3PDMakerCoreComps.D3PDObject import D3PDObject

TileDigitSGKey='TileDigitsFlt'

def makeTileDigitD3PDObject (name, prefix, object_name='TileDigitD3PDObject', getter = None,
                           sgkey = None,
                           label = None):
    if sgkey == None: sgkey = TileDigitSGKey
    if label == None: label = prefix

    if not getter:
        getter = CaloD3PDMaker.SGTileDigitsGetterTool \
                 (name + '_Getter',
                  TypeName = 'TileDigitsContainer',
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
0: digits
1: digit+section+side+tower

"""

TileDigitD3PDObject = D3PDObject (makeTileDigitD3PDObject, 'tiledigit_', 'TileDigitD3PDObject')

TileDigitD3PDObject.defineBlock (0, 'Digits',
                                CaloD3PDMaker.TileDigitFillerTool,
                                SaveOfflineInfo= False,
                                SaveHardwareInfo=True,
                                )



TileDigitD3PDObject.defineBlock (1, 'SST',
                                CaloD3PDMaker.TileDigitFillerTool,
                                SaveOfflineInfo= True,
                                SaveHardwareInfo=False,
                                )




