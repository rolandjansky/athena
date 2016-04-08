# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## Unassociated hits

import D3PDMakerCoreComps
from D3PDMakerCoreComps.D3PDObject import D3PDObject
from MinBiasD3PDMaker.MinBiasD3PDMakerConf import D3PD__UnassociatedHitsFillerTool as UnassociatedHitsFiller

def makeUnassociatedHitsD3PDObject( name, prefix, object_name, getter = None,
                                    sgkey = 'MinBiasD3PD::PRDAssociation'):
    if not getter:
        from MinBiasD3PDMaker.MinBiasD3PDMakerConf import D3PD__UnassociatedHitsGetterTool 
        getter = D3PD__UnassociatedHitsGetterTool (name + '_Getter',
                                                   TrackCollection = "Tracks",
                                                   PixelClusters = "PixelClusters",
                                                   SCTClusterContainer = "SCT_Clusters",
                                                   TRTDriftCircleContainer = "TRT_DriftCircles")
        pass

    from D3PDMakerConfig.D3PDMakerFlags import D3PDMakerFlags
    return  D3PDMakerCoreComps.ObjFillerTool( name, Prefix = prefix,
                                              Getter = getter,
                                              ObjectName = object_name,
                                              SaveMetadata = \
                                              D3PDMakerFlags.SaveObjectMetadata() )

UnassociatedHitsD3PDObject = D3PDObject( makeUnassociatedHitsD3PDObject, 'unassocHits_',
                                         "UnassociatedHitsD3PDObject" )

UnassociatedHitsD3PDObject.defineBlock (0, 'Unassociated_Hits',  UnassociatedHitsFiller)
