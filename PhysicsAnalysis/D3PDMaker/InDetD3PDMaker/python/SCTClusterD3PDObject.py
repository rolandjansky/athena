# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration

import D3PDMakerCoreComps
from D3PDMakerCoreComps.D3PDObject import D3PDObject
from D3PDMakerCoreComps.IndexMultiAssociation import IndexMultiAssociation
from D3PDMakerCoreComps.IndexAssociation import IndexAssociation
from D3PDMakerConfig.D3PDMakerFlags import D3PDMakerFlags


from InDetD3PDMaker import SCTClusterFillerTool
def makeSCTClusterD3PDObject (name, prefix, object_name, getter = None,
                              sgkey = 'SCT_Clusters',
                              typename = 'InDet::SCT_ClusterContainer',
                              label = 'SCTClus'):
    if not getter:
        from InDetD3PDMaker import SCTClusterCollectionGetterTool
        getter = SCTClusterCollectionGetterTool (name + '_Getter',
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
SCTClusterD3PDObject = D3PDObject (makeSCTClusterD3PDObject, 'sctClus_',
                                   'SCTClusterD3PDObject')

SCTClusterD3PDObject.defineBlock (0, 'SCT_Cluster',  SCTClusterFillerTool)

