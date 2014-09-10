# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## Pixel RDOs
import D3PDMakerCoreComps


from D3PDMakerCoreComps.D3PDObject     import D3PDObject
from TrackD3PDMaker.TrackD3PDMakerConf import D3PD__PixelRDOFillerTool as PixelRDOFiller
def makePixelRDOD3PDObject (name, prefix, object_name, getter = None,
                          sgkey = 'PixelRDOs',
                          typename = 'PixelRDO_Container', #PixelRDO_Container',
                          label = 'PixelRdo'):
    if not getter:
        from TrackD3PDMaker.TrackD3PDMakerConf import D3PD__PixelRDOCollectionGetterTool
        getter = D3PD__PixelRDOCollectionGetterTool (name + '_Getter',
                                                   SGKey = sgkey,
                                                   TypeName = typename,
                                                   Label = label)

    from D3PDMakerConfig.D3PDMakerFlags import D3PDMakerFlags
    return  D3PDMakerCoreComps.VectorFillerTool (name, Prefix = prefix,
                                                 Getter = getter,
                                                 ObjectName = object_name,
                                                 SaveMetadata = \
                                                 D3PDMakerFlags.SaveObjectMetadata())

##---
PixelRDOD3PDObject = D3PDObject (makePixelRDOD3PDObject, 'pixelRdo_',
                               'PixelRDOD3PDObject')

#clasterAssoc = IndexAssociation(PixelRDOD3PDObject,TrackD3PDMaker.PixelRDOClusterAssociationTool,'PixelClus','clAssoc_',level = 1, ClusterConteinerKey = 'PixelClusters')


PixelRDOD3PDObject.defineBlock (0, 'PixelRdo',  PixelRDOFiller)
#
