# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
"""
Tools configurations for ISF
KG Tan, 17/06/2012
"""

from ISF_Config.ISF_jobProperties import ISF_Flags
import sys

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
            if (i.FullGeant4 is False) :
                print ("asking for FullGeant4 but already configured Geant4 without specifying Full Geant4! Must specify FullGeant4 everywhere (or nowhere)!!!")
                sys.exit()
    else:
        from iGeant4 import iGeant4
        i = iGeant4(**kwargs)
        cache['iGeant4'] = i
    return i

def getGeant4SimSvc(name="ISF_Geant4SimSvc", **kwargs):
    kwargs.setdefault('FullGeant4'            , False                        )
    kwargs.setdefault('SimulationTool'        , 'ISFG4TransportTool'         )
    kwargs.setdefault('Identifier'            , "Geant4"                     )
    kwargs.setdefault('PrintTimingInfo'       , ISF_Flags.DoTimeMonitoring() )
    return getIGeant4(**kwargs).getSimSvc()

def getFullGeant4SimSvc(name="ISF_Geant4SimSvc", **kwargs):
    kwargs.setdefault('FullGeant4'            , True                         )
    kwargs.setdefault('SimulationTool'        , 'FullG4TransportTool'        )
    return getGeant4SimSvc(name, **kwargs)

def getPassBackGeant4SimSvc(name="ISF_PassBackGeant4SimSvc", **kwargs):
    kwargs.setdefault('FullGeant4'            , False                        )
    kwargs.setdefault('SimulationTool'        , 'PassBackG4TransportTool'    )
    return getGeant4SimSvc(name, **kwargs)
