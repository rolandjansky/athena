# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

"""
Common Geant4 configurations for ISF
Elmar Ritsch, 04/09/2014
"""

from AthenaCommon.CfgGetter import getPrivateTool,getPrivateToolClone,getPublicTool,getPublicToolClone,\
        getService,getServiceClone,getAlgorithm,getAlgorithmClone

from AthenaCommon.Constants import *  # FATAL,ERROR etc.
from AthenaCommon.SystemOfUnits import *
from AthenaCommon.DetFlags import DetFlags

def getAFIIGeant4SimSvc(name="ISF_AFIIGeant4SimSvc", **kwargs):
    from ISF_Geant4Config.ISF_iGeant4Config import getGeant4SimSvc
    kwargs.setdefault('ParticleService'       , getService('ISF_AFIIParticleBrokerSvc') )
    kwargs.setdefault('GeoIDSvc'              , getService('ISF_AFIIGeoIDSvc')          )
    return getGeant4SimSvc(name, **kwargs)
