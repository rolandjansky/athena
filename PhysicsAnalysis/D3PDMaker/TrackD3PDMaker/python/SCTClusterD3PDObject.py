# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import TrackD3PDMaker
import D3PDMakerCoreComps
from D3PDMakerCoreComps.D3PDObject import D3PDObject
from D3PDMakerCoreComps.IndexMultiAssociation import IndexMultiAssociation
from D3PDMakerCoreComps.IndexAssociation import IndexAssociation
from D3PDMakerConfig.D3PDMakerFlags import D3PDMakerFlags


from TrackD3PDMaker.TrackD3PDMakerConf import D3PD__SCTClusterFillerTool as SCTClusterFiller
def makeSCTClusterD3PDObject (name, prefix, object_name, getter = None,
                              sgkey = 'SCT_Clusters',
                              typename = 'InDet::SCT_ClusterContainer',
                              label = 'SCTClus'):
    if not getter:
        from TrackD3PDMaker.TrackD3PDMakerConf import D3PD__SCTClusterCollectionGetterTool
        getter = D3PD__SCTClusterCollectionGetterTool (name + '_Getter',
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

#SCTRDOAssoc = IndexMultiAssociation(SCTClusterD3PDObject,TrackD3PDMaker.SCTClusterSCTRDOAssociationTool,'SCTRdo','rdoAssoc_',level = 1)

#TrackAssoc = IndexMultiAssociation(SCTClusterD3PDObject,TrackD3PDMaker.SCTClusterToTrackParticleAssociationTool,'trk','trkAssoc_',level = 1, TrackParticleContainerSGKey = D3PDMakerFlags.TrackSGKey())


SCTClusterD3PDObject.defineBlock (0, 'SCT_Cluster',  SCTClusterFiller)
#SCTClusterD3PDObject.defineBlock (1, 'SCT_Clster_RDOIndex',  SCTRDOAssoc)

