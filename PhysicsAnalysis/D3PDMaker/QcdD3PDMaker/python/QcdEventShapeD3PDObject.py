# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from D3PDMakerCoreComps.D3PDObject import make_Void_D3PDObject
from D3PDMakerConfig.D3PDMakerFlags import D3PDMakerFlags
import QcdD3PDMaker

from EventShapeTools.EventEtClusterDensityConfig import *

# -- common tool settings
commonDict = dict(getDefaultEtDensityClusterDict())
commonDict['Algorithm']             = "kt"
commonDict['JetAreaMethod']         = "VoronoiArea"
commonDict['VoronoiEffectiveRfact'] = 0.9
commonDict['InclusivePtMin']        = 0.*GeV
commonDict['EtaMin']                = -2.
commonDict['EtaMax']                =  2.
commonDict['UseAreaFourMomentum']   = True
del commonDict['SignalState']

def suffix (radius, calibrated):
    suff = 'Kt%d' % int(radius*10 + 0.5)
    if calibrated:
        suff = suff + 'LC'
    else:
        suff = suff + 'EM'
    return suff
    
    
def areaDensity (radius, calibrated):
    suff = suffix (radius, calibrated)
    ss = 'CALIBRATED' if calibrated else 'UNCALIBRATED'
    return configureJetAreaDensity ('ClusterRho' + suff,
                                    Radius = radius,
                                    SignalState = ss,
                                    **commonDict)
    
def rhoBlock (obj, radius, calibrated):
    suff = suffix (radius, calibrated)
    obj.defineBlock (0, 'ClusterRho' + suff,
                     QcdD3PDMaker.EventEtDensityFillerTool,
                     VariableSuffix = suff,
                     EventEtDensityTool = areaDensity (radius, calibrated))
    return
    
EventShapeD3PDObject = make_Void_D3PDObject( 'Eventshape_', "EventShapeD3PDObject" )
rhoBlock (EventShapeD3PDObject, 0.3, False)
rhoBlock (EventShapeD3PDObject, 0.4, False)
rhoBlock (EventShapeD3PDObject, 0.3, True)
rhoBlock (EventShapeD3PDObject, 0.4, True)
