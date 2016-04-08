# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


include.block("TruthUtils/HforTool_jobOptions.py")

from AthenaCommon.AppMgr import ToolSvc
from TruthUtils.TruthUtilsConf import HforTool
ToolSvc += HforTool("hforTool")

# Name of removal schema
ToolSvc.hforTool.RemovalSchema = 'jetbased'
ToolSvc.hforTool.OutputLevel = INFO 
ToolSvc.hforTool.SampleType = 'isLightFlavor' 


