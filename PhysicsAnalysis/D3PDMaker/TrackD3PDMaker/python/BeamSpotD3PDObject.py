# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import TrackD3PDMaker
import D3PDMakerCoreComps
from D3PDMakerCoreComps.D3PDObject import D3PDObject

def makeBeamSpotD3PDObject (name, prefix, object_name,
                            getter = None):
    if not getter:
        from TrackD3PDMaker.TrackD3PDMakerConf import D3PD__BeamSpotGetterTool
        getter = D3PD__BeamSpotGetterTool ( name + '_Getter' )

    from D3PDMakerConfig.D3PDMakerFlags import D3PDMakerFlags
    return D3PDMakerCoreComps.ObjFillerTool (name,
                                             Prefix = prefix,
                                             Getter = getter,
                                             ObjectName = object_name,
                                             AllowMissing=True,
                                             SaveMetadata = \
                                             D3PDMakerFlags.SaveObjectMetadata())
    
BeamSpotD3PDObject = D3PDObject (makeBeamSpotD3PDObject, 'beamSpot_', 'BeamSpotD3PDObject')

from TrackD3PDMaker.TrackD3PDMakerConf import D3PD__BeamSpotFillerTool as BeamSpotFiller
BeamSpotD3PDObject.defineBlock (0, 'BeamSpot',  BeamSpotFiller)
