# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#import a configured instance of TRT_DriftCircleTool

from InDetTrigRecExample.InDetTrigCommonTools import InDetTrigTRT_DriftCircleTool,InDetTrigTRTRodDecoder
from AthenaCommon.AppMgr import ToolSvc

from TrigOfflineDriftCircleTool.TrigOfflineDriftCircleToolConf import TrigTRT_DCCollByteStreamTool, TrigTRT_DriftCircleProviderTool
ConfiguredTrigTRT_DCCollByteStreamTool = TrigTRT_DCCollByteStreamTool(name="TrigTRT_DCCollByteStreamTool",
                                                                      TRT_DriftCircleToolName=InDetTrigTRT_DriftCircleTool,
                                                                      TRTRodDecoder=InDetTrigTRTRodDecoder)

ToolSvc += ConfiguredTrigTRT_DCCollByteStreamTool

ConfiguredTrigTRT_DriftCircleProviderTool = TrigTRT_DriftCircleProviderTool(name="TRT_DriftCircleProviderTool",
                                                                            TRT_DCCollBSToolName=ConfiguredTrigTRT_DCCollByteStreamTool)
ToolSvc += ConfiguredTrigTRT_DriftCircleProviderTool




