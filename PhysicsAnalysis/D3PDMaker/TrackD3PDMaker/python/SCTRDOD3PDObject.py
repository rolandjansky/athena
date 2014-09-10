# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## SCT RDOs
import D3PDMakerCoreComps


from D3PDMakerCoreComps.D3PDObject     import D3PDObject
from TrackD3PDMaker.TrackD3PDMakerConf import D3PD__SCTRDOFillerTool as SCTRDOFiller
def makeSCTRDOD3PDObject (name, prefix, object_name, getter = None,
                          sgkey = 'SCT_RDOs',
                          typename = 'SCT_RDO_Container', #SCT_RDO_Container',
                          label = 'SCTRdo'):
    if not getter:
        from TrackD3PDMaker.TrackD3PDMakerConf import D3PD__SCTRDOCollectionGetterTool
        getter = D3PD__SCTRDOCollectionGetterTool (name + '_Getter',
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
SCTRDOD3PDObject = D3PDObject (makeSCTRDOD3PDObject, 'sctRdo_',
                               'SCTRDOD3PDObject')

#clasterAssoc = IndexAssociation(SCTRDOD3PDObject,TrackD3PDMaker.SCTRDOClusterAssociationTool,'SCTClus','clAssoc_',level = 1, ClusterConteinerKey = 'SCT_Clusters')


SCTRDOD3PDObject.defineBlock (0, 'SCT_Rdo',  SCTRDOFiller)
#
