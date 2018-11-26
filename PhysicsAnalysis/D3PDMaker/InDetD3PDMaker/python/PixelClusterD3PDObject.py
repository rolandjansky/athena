# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration

import D3PDMakerCoreComps
from D3PDMakerCoreComps.D3PDObject import D3PDObject
from D3PDMakerCoreComps.IndexMultiAssociation import IndexMultiAssociation
from D3PDMakerCoreComps.IndexAssociation import IndexAssociation
from D3PDMakerConfig.D3PDMakerFlags import D3PDMakerFlags


from InDetD3PDMaker import PixelClusterFillerTool
def makePixelClusterD3PDObject (name, prefix, object_name, getter = None,
                              sgkey = 'PixelClusters',
                              typename = 'InDet::PixelClusterContainer',
                              label = 'PixelClus',
			      blayerOnly = False):
    if not getter:
        from InDetD3PDMaker import PixelClusterCollectionGetterTool
        getter = PixelClusterCollectionGetterTool (name + '_Getter',
                                                   SGKey = sgkey,
                                                   TypeName = typename,
                                                   Label = label,
						   BLayerOnly = blayerOnly)

    from D3PDMakerConfig.D3PDMakerFlags import D3PDMakerFlags
    return  D3PDMakerCoreComps.VectorFillerTool (name, Prefix = prefix,
                                                 Getter = getter,
                                                 ObjectName = object_name,
                                                 SaveMetadata = \
                                                 D3PDMakerFlags.SaveObjectMetadata())

##---
PixelClusterD3PDObject = D3PDObject (makePixelClusterD3PDObject, 'pixelClus_',
                                   'PixelClusterD3PDObject')

PixelClusterD3PDObject.defineBlock (0, 'PixelCluster',  PixelClusterFillerTool)

