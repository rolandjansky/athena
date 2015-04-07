
#update the run/event info for each event
if not hasattr(svcMgr,'EvtIdModifierSvc'):
    import AthenaServices.Configurables as asc
    svcMgr +=asc.EvtIdModifierSvc(EvtStoreName="StoreGateSvc") #OriginalEvent_SG
    from AthenaCommon.AppMgr import theApp
    theApp.CreateSvc += ["EvtIdModifierSvc"]
else:
    print 'g4runnumber.py: Will override the settings of the EvtIdModifierSvc that was previously set up!'
    ## remove any existing settings
    svcMgr.EvtIdModifierSvc.Modifiers = []
svcMgr.EvtIdModifierSvc.OutputLevel=DEBUG

include("events.txt")
#svcMgr.EvtIdModifierSvc.add_modifier(run_nbr=167776, evt_nbr=18, time_stamp=1269948350, lbk_nbr=124, nevts=1)


#set the max number of events
fname = "events.txt"
num_lines = 0
with open(fname, 'r') as f:
    for line in f:
        num_lines += 1
        print line,
print "Number of lines in events.txt is "+str(num_lines)
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
athenaCommonFlags.EvtMax.unlock()
athenaCommonFlags.EvtMax.set_Value_and_Lock(num_lines)

#fix iov metadata
if not hasattr(ServiceMgr.ToolSvc, 'IOVDbMetaDataTool'):
    from AthenaCommon import CfgMgr
    svcMgr.ToolSvc += CfgMgr.IOVDbMetaDataTool()
    svcMgr.ToolSvc.IOVDbMetaDataTool.MinMaxRunNumbers = [svcMgr.EvtIdModifierSvc.Modifiers[0], 2147483647]
    ## FIXME need to use maxRunNumber = 2147483647 for now to keep overlay working but in the future this should be set properly.

#use conditions from this run number and timestamp
svcMgr.EventSelector.RunNumber = svcMgr.EvtIdModifierSvc.Modifiers[0]
svcMgr.EventSelector.OverrideRunNumber = True
svcMgr.EventSelector.OverrideEventNumber = True
svcMgr.EventSelector.InitialTimeStamp = svcMgr.EvtIdModifierSvc.Modifiers[2]
print svcMgr.EventSelector
svcMgr.TagInfoMgr.OutputLevel=DEBUG

