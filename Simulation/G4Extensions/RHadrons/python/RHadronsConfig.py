# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon import CfgMgr,Logging
from G4AtlasServices import G4AtlasServicesConfig

def getSG_StepNtupleTool(name="G4UA::SG_StepNtupleTool", **kwargs):
    from AthenaCommon.ConcurrencyFlags import jobproperties as concurrencyProps
    if concurrencyProps.ConcurrencyFlags.NumThreads() >1:
        log=Logging.logging.getLogger(name)
        log.fatal(' Attempt to run '+name+' with more than one thread, which is not supported')
        #from AthenaCommon.AppMgr import theApp
        #theApp.exit(1)
        return False
    from RHadrons.RHadronsConf import G4UA__SG_StepNtupleTool
    return G4UA__SG_StepNtupleTool(name, **kwargs)

def getRHadronsPythiaPhysicsTool(name='RHadronsPythiaPhysicsTool', **kwargs):
    kwargs.setdefault('UsePythia8',False)
    return CfgMgr.RHadronsPhysicsTool(name,**kwargs)

def getRHadronsPythia8PhysicsTool(name='RHadronsPythia8PhysicsTool', **kwargs):
    kwargs.setdefault('UsePythia8',True)
    return CfgMgr.RHadronsPhysicsTool(name,**kwargs)
