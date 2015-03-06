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
    kwargs.setdefault('SimulationTool'       , 'AFII_G4TransportTool')
    from ISF_Geant4Config.ISF_iGeant4Config import getPassBackGeant4SimSvc
    return getPassBackGeant4SimSvc(name, **kwargs)

def getLongLivedGeant4SimSvc(name="ISF_LongLivedGeant4SimSvc", **kwargs):
    kwargs.setdefault('SimulationTool'       , 'QuasiStableG4TransportTool')
    from ISF_Geant4Config.ISF_iGeant4Config import getFullGeant4SimSvc
    return getFullGeant4SimSvc(name, **kwargs)
