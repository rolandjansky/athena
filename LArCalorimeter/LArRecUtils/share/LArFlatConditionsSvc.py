# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

#JobO fragment to set up LArFlatConditionsSvc

if 'LARFLATCONDITIONSVC_HAS_BEEN_INCLUDED' not in dir():
    from LArRecUtils.LArRecUtilsConf import LArFlatConditionSvc
    theLArCondSvc=LArFlatConditionSvc(OutputLevel=DEBUG,DoSuperCells=False)
    svcMgr+=theLArCondSvc
    svcMgr.ProxyProviderSvc.ProviderNames += [ "LArFlatConditionSvc" ]
    pass

