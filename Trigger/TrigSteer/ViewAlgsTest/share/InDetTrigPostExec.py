from RegionSelector.RegSelSvcDefault import RegSelSvcDefault
RegSelSvc = RegSelSvcDefault()

#RegSelSvc.DisableFromConditions = False
#RegSelSvc.DisablePixelFromConditions = False
#RegSelSvc.DisableTRTFromConditions = False
#RegSelSvc.DisableSCTFromConditions = False
#from InDetTrigRecExample.InDetTrigConfigConditions import \
#         SCT_ConditionsSetup, TRT_ConditionsSetup
#RegSelSvc.PixConditionsSvc = ""      #empty as it does not work
#RegSelSvc.SCTConditionsSvc = "SCT_ConditionsSummarySvc/"+SCT_ConditionsSetup.instanceName("InDetSCT_ConditionsSummarySvc")
#RegSelSvc.TRTConditionsSvc = "TRT_ConditionsSummarySvc/"+TRT_ConditionsSetup.instanceName("InDetTRTConditionsSummaryService")

RegSelSvc.enablePixel = True
RegSelSvc.enableSCT   = True

from AthenaCommon.AppMgr import ServiceMgr as svcMgr
svcMgr += RegSelSvc

topSequence.InDetPixelClusterization.isRoI_Seeded = True
topSequence.InDetPixelClusterization.RoIs = "OutputRoIs"

from AthenaCommon.DetFlags import DetFlags
DetFlags.Print()
#DetFlags.ID_setOn()

