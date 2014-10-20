# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

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

    FullGeant4 = kwargs['FullGeant4']

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
        i = iGeant4(ParticleService        = kwargs['ParticleService'],
                    TruthService           = kwargs['TruthService'],
                    SimHitService          = kwargs['SimHitService'],
                    GeoIDSvc               = kwargs["GeoIDSvc"],
                    UseNewConfiguration    = ISF_Flags.UseNewG4Config(),
                    PrintTimingInfo        = ISF_Flags.DoTimeMonitoring(),
                    FullGeant4             = FullGeant4)
        cache['iGeant4'] = i
    return i

def getGeant4SimSvc(name="ISF_Geant4SimSvc", **kwargs):
    kwargs.setdefault('FullGeant4'            , False                               )
    kwargs.setdefault('ParticleService'       , getService('ISF_ParticleBrokerSvc') )
    kwargs.setdefault('TruthService'          , ISF_Flags.TruthService.get_Value()  )
    kwargs.setdefault('SimHitService'         , getService('ISF_SimHitService')     )
    kwargs.setdefault('GeoIDSvc'              , getService('ISF_GeoIDSvc')          )
    return getIGeant4(**kwargs).getSimSvc()

def getFullGeant4SimSvc(name="ISF_Geant4SimSvc", **kwargs):
    kwargs.setdefault('FullGeant4'            , True                                          )
    kwargs.setdefault('ParticleService'       , getService('ISF_ParticleBrokerSvcNoOrdering') )
    return getGeant4SimSvc(name, **kwargs)

