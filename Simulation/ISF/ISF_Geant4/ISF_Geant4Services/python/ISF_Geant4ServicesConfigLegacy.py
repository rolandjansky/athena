# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from __future__ import print_function

"""
Tools configurations for ISF
KG Tan, 17/06/2012
"""

import sys

cache = {}

def usingGeant4():
    global cache
    return bool(cache)

def getIGeant4(name, **kwargs):
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
        from .iGeant4 import iGeant4
        i = iGeant4(name, **kwargs)
        cache['iGeant4'] = i
    return i

def getGeant4SimSvc(name="ISF_Geant4SimSvc", **kwargs):
    kwargs.setdefault('SimulatorTool'        , 'ISF_Geant4Tool'         )
    kwargs.setdefault('Identifier'            , "Geant4"                     )
    kwargs.setdefault('FullGeant4'            , False                        )
    return getIGeant4(name, **kwargs).getSimSvc()

def getFullGeant4SimSvc(name="ISF_FullGeant4SimSvc", **kwargs):
    kwargs.setdefault('SimulatorTool'        , 'ISF_FullGeant4Tool'         )
    kwargs.setdefault('FullGeant4'            , True                         )
    return getGeant4SimSvc(name, **kwargs)

def getLongLivedGeant4SimSvc(name="ISF_LongLivedGeant4SimSvc", **kwargs):
    kwargs.setdefault('SimulatorTool'       , 'ISF_LongLivedGeant4Tool')
    return getFullGeant4SimSvc(name, **kwargs)

def getPassBackGeant4SimSvc(name="ISF_PassBackGeant4SimSvc", **kwargs):
    kwargs.setdefault('FullGeant4'            , False                        )
    kwargs.setdefault('SimulatorTool'        , 'ISF_PassBackGeant4Tool'    )
    return getGeant4SimSvc(name, **kwargs)

def getAFIIGeant4SimSvc(name="ISF_AFIIGeant4SimSvc", **kwargs):
    kwargs.setdefault('SimulatorTool'       , 'ISF_AFIIGeant4Tool')
    return getPassBackGeant4SimSvc(name, **kwargs)

def getAFII_QS_Geant4SimSvc(name="ISF_AFII_QS_Geant4SimSvc", **kwargs):
    kwargs.setdefault('SimulatorTool'       , 'AFII_QS_Geant4Tool')
    return getPassBackGeant4SimSvc(name, **kwargs)
