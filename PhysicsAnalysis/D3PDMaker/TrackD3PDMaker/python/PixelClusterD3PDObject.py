# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import TrackD3PDMaker
import D3PDMakerCoreComps
from D3PDMakerCoreComps.D3PDObject import D3PDObject
from D3PDMakerCoreComps.IndexMultiAssociation import IndexMultiAssociation
from D3PDMakerCoreComps.IndexAssociation import IndexAssociation
from D3PDMakerConfig.D3PDMakerFlags import D3PDMakerFlags


from TrackD3PDMaker.TrackD3PDMakerConf import D3PD__PixelClusterFillerTool as PixelClusterFiller
def makePixelClusterD3PDObject (name, prefix, object_name, getter = None,
                              sgkey = 'PixelClusters',
                              typename = 'InDet::PixelClusterContainer',
                              label = 'PixelClus',
			      blayerOnly = False):
    if not getter:
        from TrackD3PDMaker.TrackD3PDMakerConf import D3PD__PixelClusterCollectionGetterTool
        getter = D3PD__PixelClusterCollectionGetterTool (name + '_Getter',
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

#PixelRDOAssoc = IndexMultiAssociation(PixelClusterD3PDObject,TrackD3PDMaker.PixelClusterPixelRDOAssociationTool,'PixelRdo','rdoAssoc_',level = 1)

#TrackAssoc = IndexMultiAssociation(PixelClusterD3PDObject,TrackD3PDMaker.PixelClusterToTrackParticleAssociationTool,'trk','trkAssoc_',level = 1, TrackParticleContainerSGKey = D3PDMakerFlags.TrackSGKey())


PixelClusterD3PDObject.defineBlock (0, 'PixelCluster',  PixelClusterFiller)
#PixelClusterD3PDObject.defineBlock (1, 'Pixel_Clster_RDOIndex',  PixelRDOAssoc)

