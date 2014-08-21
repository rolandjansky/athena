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
    
def getIGeant4(FullGeant4 = False):
    global cache
    if 'iGeant4' in cache:
        i = cache['iGeant4']

        # if FullGeant4 specified, make sure returning cached version with FullGeant4 already in use
        if FullGeant4:
            if (i.FullGeant4 == False) :
                print "asking for FullGeant4 but already configured Geant4 without specifying Full Geant4! Must specify FullGeant4 everywhere (or nowhere)!!!"
                sys.exit()

    else:
        
        if FullGeant4:
            ParticleServiceName ="ISF_ParticleBrokerSvcNoOrdering"
        else:
            ParticleServiceName ="ISF_ParticleBrokerSvc"
            
        from ISF_Config.ISF_jobProperties import ISF_Flags
        from iGeant4 import iGeant4
        i = iGeant4(ParticleService        = getService(ParticleServiceName),
                    TruthService           = getService('ISF_TruthService'),
                    SimHitService          = getService('ISF_SimHitService'),
                    UseNewConfiguration    = ISF_Flags.UseNewG4Config(),
                    PrintTimingInfo        = ISF_Flags.DoTimeMonitoring(),
                    FullGeant4             = FullGeant4)
        cache['iGeant4'] = i
    return i

def getGeant4SimSvc(name="ISF_Geant4SimSvc", **kwargs):
    return getIGeant4().getSimSvc()

def getFullGeant4SimSvc(name="ISF_Geant4SimSvc", **kwargs):
    return getIGeant4(True).getSimSvc()
