# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

from AthenaCommon import CfgMgr

def getTestBoundariesUserActionTool(name='G4UA::iGeant4::TestBoundariesUserActionTool', **kwargs):
    from AthenaCommon import Logging
    from AthenaCommon.ConcurrencyFlags import jobproperties as concurrencyProps
    if concurrencyProps.ConcurrencyFlags.NumThreads() >1:
        log=Logging.logging.getLogger(name)
        log.fatal('Attempt to run '+name+' with more than one thread, which is not supported')
        #from AthenaCommon.AppMgr import theApp
        #theApp.exit(1)
        return False
    return CfgMgr.G4UA__iGeant4__TestBoundariesUserActionTool(name,**kwargs)
