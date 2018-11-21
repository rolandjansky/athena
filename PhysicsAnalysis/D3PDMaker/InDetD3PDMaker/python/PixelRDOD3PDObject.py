# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration

## Pixel RDOs
import D3PDMakerCoreComps


from D3PDMakerCoreComps.D3PDObject     import D3PDObject
from InDetD3PDMaker import PixelRDOFillerTool
def makePixelRDOD3PDObject (name, prefix, object_name, getter = None,
                          sgkey = 'PixelRDOs',
                          typename = 'PixelRDO_Container', #PixelRDO_Container',
                          label = 'PixelRdo'):
    if not getter:
        from InDetD3PDMaker import PixelRDOCollectionGetterTool
        getter = PixelRDOCollectionGetterTool (name + '_Getter',
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


PixelRDOD3PDObject.defineBlock (0, 'PixelRdo',  PixelRDOFillerTool)

