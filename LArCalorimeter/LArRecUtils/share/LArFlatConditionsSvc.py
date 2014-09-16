
#JobO fragment to set up LArFlatConditionsSvc

if not 'LARFLATCONDITIONSVC_HAS_BEEN_INCLUDED' in dir():
    from LArRecUtils.LArRecUtilsConf import LArFlatConditionSvc
    theLArCondSvc=LArFlatConditionSvc(OutputLevel=DEBUG,DoSuperCells=False)
    svcMgr+=theLArCondSvc
    svcMgr.ProxyProviderSvc.ProviderNames += [ "LArFlatConditionSvc" ]
    pass

