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
    # Get the PDG IDs for RHadrons
    from RHadronMasses import offset_options
    kwargs.setdefault('RHadronPDGIDList',offset_options.keys())
    if name in simFlags.UserActionConfig.get_Value().keys():
        for prop,value in simFlags.UserActionConfig.get_Value()[name].iteritems():
            kwargs.setdefault(prop,value)
    from RHadrons.RHadronsConf import G4UA__SG_StepNtupleTool
    return G4UA__SG_StepNtupleTool(name, **kwargs)

def getRHadronsPhysicsTool(name='RHadronsPhysicsTool', **kwargs):
    return CfgMgr.RHadronsPhysicsTool(name,**kwargs)
