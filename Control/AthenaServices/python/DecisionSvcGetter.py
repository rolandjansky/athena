# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# @purpose: bootstrap the decision svc

def _init():
    """function executed -once- at module import
    """
    import AthenaCommon.CfgMgr as CfgMgr
    from AthenaCommon.AppMgr import ServiceMgr as svcMgr
    if not hasattr(svcMgr, 'DecisionSvc'):
        svcMgr += CfgMgr.DecisionSvc()
    return

# side-effect of importing that module
_init()
# clean-up
del _init

