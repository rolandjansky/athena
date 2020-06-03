# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

"""
Tools configurations for ISF
KG Tan, 17/06/2012
"""

from AthenaCommon.CfgGetter import getPrivateTool,getPrivateToolClone,getPublicTool,getPublicToolClone,\
        getService,getServiceClone,getAlgorithm,getAlgorithmClone

from AthenaCommon.Constants import *  # FATAL,ERROR etc.
from AthenaCommon.SystemOfUnits import *
from AthenaCommon.DetFlags import DetFlags

from ISF_Config.ISF_jobProperties import ISF_Flags # IMPORTANT: Flags must be set before tools are retrieved


cache = {}

def usingGeant4():
    global cache
    return bool(cache)

def getIGeant4(**kwargs):
    global cache
    FullGeant4 = kwargs.pop('FullGeant4',False)
    if 'iGeant4' in cache:
        i = cache['iGeant4']
        # if FullGeant4 specified, make sure returning cached version with FullGeant4 already in use
        if FullGeant4:
            if (i.FullGeant4 == False) :
                print "asking for FullGeant4 but already configured Geant4 without specifying Full Geant4! Must specify FullGeant4 everywhere (or nowhere)!!!"
                sys.exit()
    else:
        from ISF_Config.ISF_jobProperties import ISF_Flags
        from iGeant4 import iGeant4
        i = iGeant4(**kwargs)
        cache['iGeant4'] = i
    return i

def getGeant4SimSvc(name="ISF_Geant4SimSvc", **kwargs):
    from ISF_Config.ISF_jobProperties import ISF_Flags
    kwargs.setdefault('FullGeant4'            , False                        )
    kwargs.setdefault('SimulationTool'        , 'ISFG4TransportTool'         )
    kwargs.setdefault('Identifier'            , "Geant4"                     )
    kwargs.setdefault('PrintTimingInfo'       , ISF_Flags.DoTimeMonitoring() )
    return getIGeant4(**kwargs).getSimSvc()

def getFullGeant4SimSvc(name="ISF_Geant4SimSvc", **kwargs):
    kwargs.setdefault('FullGeant4'            , True                         )
    kwargs.setdefault('SimulationTool'        , 'FullG4TransportTool'        )
    return getGeant4SimSvc(name, **kwargs)

def getLongLivedGeant4SimSvc(name="ISF_LongLivedGeant4SimSvc", **kwargs):
    kwargs.setdefault('SimulationTool'       , 'QuasiStableG4TransportTool')
    return getFullGeant4SimSvc(name, **kwargs)

def getPassBackGeant4SimSvc(name="ISF_PassBackGeant4SimSvc", **kwargs):
    kwargs.setdefault('FullGeant4'            , False                        )
    kwargs.setdefault('SimulationTool'        , 'PassBackG4TransportTool'    )
    return getGeant4SimSvc(name, **kwargs)

def getAFIIGeant4SimSvc(name="ISF_AFIIGeant4SimSvc", **kwargs):
    kwargs.setdefault('SimulationTool'       , 'AFII_G4TransportTool')
    return getPassBackGeant4SimSvc(name, **kwargs)

def getAFII_QS_Geant4SimSvc(name="ISF_AFII_QS_Geant4SimSvc", **kwargs):
    kwargs.setdefault('SimulationTool'       , 'AFII_QS_G4TransportTool')
    return getPassBackGeant4SimSvc(name, **kwargs)
