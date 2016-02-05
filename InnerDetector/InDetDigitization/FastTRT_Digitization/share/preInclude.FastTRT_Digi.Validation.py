##
## preInclude.FastTRT_Digi.py
##

from Digitization.DigitizationFlags import digitizationFlags
digitizationFlags.experimentalDigi += ['doFastTRT_Digi']

from TRT_ConditionsServices.TRT_ConditionsServicesConf import TRT_StrawStatusSummarySvc
InDetTRTStrawStatusSummarySvc = TRT_StrawStatusSummarySvc( name = "InDetTRTStrawStatusSummarySvc" )
ServiceMgr += InDetTRTStrawStatusSummarySvc

from TRT_ElectronPidTools.TRT_ElectronPidToolsConf import InDet__TRT_LocalOccupancy
InDetTRT_LocalOccupancy = InDet__TRT_LocalOccupancy( name = "InDet_TRT_LocalOccupancy",
                                                     isTrigger = False )
ToolSvc += InDetTRT_LocalOccupancy

from TRT_ElectronPidTools.TRT_ElectronPidToolsConf import InDet__TRT_ElectronPidToolRun2
InDetTRT_ElectronPidTool = InDet__TRT_ElectronPidToolRun2( name = "InDetTRT_ElectronPidTool",
                                                           TRT_LocalOccupancyTool = InDetTRT_LocalOccupancy,
                                                           isData = (globalflags.DataSource == 'data') )
ToolSvc += InDetTRT_ElectronPidTool

##EOF
