# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from D3PDMakerCoreComps.D3PDObject import make_Void_D3PDObject
from D3PDMakerConfig.D3PDMakerFlags import D3PDMakerFlags
import QcdD3PDMaker

from EventShapeTools.EventEtClusterDensityConfig import *
from EventShapeRec.EventEtDensityConfig import areaDensityTool, suffix

# previous configuration here has been factorized into 
# EventShapeRec.EventEtDensityConfig
    
def rhoBlock (obj, radius, calibrated):
    suff = suffix (radius, calibrated)
    obj.defineBlock (0, 'ClusterRho' + suff,
                     QcdD3PDMaker.EventEtDensityFillerTool,
                     VariableSuffix = suff,
                     EventEtDensityTool = areaDensityTool (radius, calibrated))
    return
    
EventShapeD3PDObject = make_Void_D3PDObject( 'Eventshape_', "EventShapeD3PDObject" )
rhoBlock (EventShapeD3PDObject, 0.3, False)
rhoBlock (EventShapeD3PDObject, 0.4, False)
rhoBlock (EventShapeD3PDObject, 0.6, False)
rhoBlock (EventShapeD3PDObject, 0.3, True)
rhoBlock (EventShapeD3PDObject, 0.4, True)
rhoBlock (EventShapeD3PDObject, 0.6, True)
