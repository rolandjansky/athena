#--------------------------------------------------------------------
# Overrides for run,lumi - dependent MC
#--------------------------------------------------------------------
include.block("Digitization/LumiBlockOverrides.py")
from Digitization.DigitizationFlags import digitizationFlags
if digitizationFlags.RunAndLumiOverrideList.get_Value():
    digitizationFlags.RunAndLumiOverrideList.Print()
    digitizationFlags.RunAndLumiOverrideList.lock()

    #adjust EvtMax
    from AthenaCommon.AppMgr import ServiceMgr as svcMgr
    athenaCommonFlags.EvtMax.unlock() #don't run off end of job!
    athenaCommonFlags.EvtMax= digitizationFlags.RunAndLumiOverrideList.getEvtsMax()
    digilog.info('now evtmax is: %s' % athenaCommonFlags.EvtMax.get_Value())

    #fix pileup
    scalefrom = digitizationFlags.numberOfCollisions.get_Value()
    digilog.info('Your pileup configuration file determined the scalefactor: %g' % scalefrom)
    digitizationFlags.RunAndLumiOverrideList.ScaleNumberOfCollisions(scaleFrom=scalefrom)

    #fix event id
    from AthenaCommon.CfgGetter import getService
    getService("EvtIdModifierSvc")
    from AthenaCommon.DetFlags import DetFlags
    from AthenaCommon.ConcurrencyFlags import jobproperties as concurrencyProps
    if concurrencyProps.ConcurrencyFlags.NumThreads() > 0:
        if not hasattr(svcMgr, 'AthenaHiveEventLoopMgr'):
            from AthenaServices.AthenaServicesConf import AthenaHiveEventLoopMgr
            svcMgr += AthenaHiveEventLoopMgr()
        svcMgr.AthenaHiveEventLoopMgr.EvtIdModifierSvc = "EvtIdModifierSvc"  
    elif DetFlags.pileup.any_on():
        # already configured in PileUpEventLoopMgr
        pass
    else:
        if not hasattr(svcMgr, 'AthenaEventLoopMgr'):
            from AthenaServices.AthenaServicesConf import AthenaEventLoopMgr
            svcMgr += AthenaEventLoopMgr()
        svcMgr.AthenaEventLoopMgr.EvtIdModifierSvc = "EvtIdModifierSvc"

    #fix iov metadata
    if not hasattr(svcMgr.ToolSvc, 'IOVDbMetaDataTool'): svcMgr.ToolSvc += CfgMgr.IOVDbMetaDataTool()
    svcMgr.ToolSvc.IOVDbMetaDataTool.MinMaxRunNumbers = list(digitizationFlags.RunAndLumiOverrideList.getMinMaxRunNumbers())
