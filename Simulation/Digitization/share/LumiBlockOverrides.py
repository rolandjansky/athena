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

    #fix iov metadata
    if not hasattr(svcMgr.ToolSvc, 'IOVDbMetaDataTool'): svcMgr.ToolSvc += CfgMgr.IOVDbMetaDataTool()
    svcMgr.ToolSvc.IOVDbMetaDataTool.MinMaxRunNumbers = list(digitizationFlags.RunAndLumiOverrideList.getMinMaxRunNumbers())
