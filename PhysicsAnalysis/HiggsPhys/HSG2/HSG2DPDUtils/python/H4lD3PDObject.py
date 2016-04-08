# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import HSG2DPDUtils
import D3PDMakerCoreComps
from D3PDMakerCoreComps.D3PDObject import D3PDObject
from D3PDMakerConfig.D3PDMakerFlags import D3PDMakerFlags
 
 
def makeH4lD3PDObject (name, prefix, getter = None,
                        sgkey = None,
                        label = None):
    if sgkey == None: sgkey = 'H4lD3PD'
    if label == None: label = prefix
    if not getter:
        getter = D3PDMakerCoreComps.SGObjGetterTool \
                 (name + '_Getter',
                  TypeName = 'H4lNtuple',
                  SGKey = sgkey)
        
    return D3PDMakerCoreComps.ObjFillerTool (name,
                                             Prefix = prefix,
                                             Getter = getter)

H4lD3PDObject = D3PDObject (makeH4lD3PDObject, 'h4l_')

# The final kinematic block
H4lD3PDObject.defineBlock (0, 'Kinematics',
                           HSG2DPDUtils.H4lKinematicFillerTool)

# The preselection variables block
H4lD3PDObject.defineBlock (1, 'Preselection',
                           HSG2DPDUtils.H4lPreselectionFillerTool)











