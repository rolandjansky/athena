##
## preInclude.FastTRT_Digi.py
##

from Digitization.DigitizationFlags import digitizationFlags
digitizationFlags.experimentalDigi += ['doFastTRT_Digi']

# The TRT Services are replaced by private tools
from TRT_ConditionsServices.TRT_ConditionsServicesConf import TRT_StrawStatusSummaryTool
InDetTRTStrawStatusSummaryTool = TRT_StrawStatusSummaryTool( name = "InDetTRTStrawStatusSummaryTool",
                                                             isGEANT4=!(globalflags.DataSource == 'data') )
#ServiceMgr += InDetTRTStrawStatusSummarySvc

from TRT_ConditionsServices.TRT_ConditionsServicesConf import TRT_CalDbTool
                                            InDetTRTCalDbTool = TRT_CalDbTool(name = "TRT_CalDbTool")

from TRT_ElectronPidTools.TRT_ElectronPidToolsConf import InDet__TRT_LocalOccupancy
InDetTRT_LocalOccupancy = InDet__TRT_LocalOccupancy( name = "InDet_TRT_LocalOccupancy",
                                                     isTrigger = False,
                                                     TRTCalDbTool = InDetTRTCalDbTool)
ToolSvc += InDetTRT_LocalOccupancy

from TRT_ElectronPidTools.TRT_ElectronPidToolsConf import TRT_ToT_dEdx
InDetTRT_ToT_dEdx = TRT_ToT_dEdx( name = "InDet_TRT_ToT_dEdx",
                                  TRTStrawSummaryTool = InDetTRTStrawStatusSummaryTool,
                                  TRT_LocalOccupancyTool = InDetTRT_LocalOccupancy )                                      
ToolSvc += InDetTRT_ToT_dEdx

from TRT_ElectronPidTools.TRT_ElectronPidToolsConf import InDet__TRT_ElectronPidToolRun2
InDetTRT_ElectronPidTool = InDet__TRT_ElectronPidToolRun2( name = "InDetTRT_ElectronPidTool",
                                                           TRT_LocalOccupancyTool = InDetTRT_LocalOccupancy,
                                                           TRTStrawSummaryTool       = InDetTRTStrawStatusSummaryTool,
                                                           TRT_ToT_dEdx_Tool = InDetTrigTRT_ToT_dEdx,
                                                           isData = (globalflags.DataSource == 'data') )
ToolSvc += InDetTRT_ElectronPidTool

##EOF
