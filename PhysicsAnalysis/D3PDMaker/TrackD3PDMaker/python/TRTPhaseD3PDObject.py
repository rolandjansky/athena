# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#TRT_Phase  PUT IN SEPARATE PYTHON FILE
#
# We operate on a single object from StoreGate.
#
import D3PDMakerCoreComps

from D3PDMakerCoreComps.D3PDObject     import D3PDObject
from TrackD3PDMaker.TrackD3PDMakerConf import D3PD__TRTPhaseFillerTool as TRTPhaseFiller
def makeTRTPhaseD3PDObject (name, prefix, object_name, getter = None, sgkey = 'TRT_Phase'):
     if not getter:
         getter = D3PDMakerCoreComps.SGObjGetterTool (name + '_Getter',
                                                      SGKey = sgkey,
                                                      TypeName = 'ComTime')

     from D3PDMakerConfig.D3PDMakerFlags import D3PDMakerFlags 
     return D3PDMakerCoreComps.ObjFillerTool (name,
                                              Prefix = prefix,
                                              Getter = getter,
                                              ObjectName = object_name,
                                              SaveMetadata = \
                                              D3PDMakerFlags.SaveObjectMetadata())
 
# Create the object type.
TRTPhaseD3PDObject = D3PDObject (makeTRTPhaseD3PDObject, 'TRT_', 'TRTPhaseD3PDObject') 
# Define blocks.
TRTPhaseD3PDObject.defineBlock (0, 'TRTPhase',TRTPhaseFiller)

