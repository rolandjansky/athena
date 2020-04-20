# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

if 'LARAUTOCORRTOTALTOOL_HAS_BEEN_INCLUDED' not in dir():
  LARAUTOCORRTOTALTOOL_HAS_BEEN_INCLUDED =1

printfunc (" WARNING obsolete jobOption. Please use python class to configure LArAutoCorrTotalTool")

from AthenaCommon.AppMgr import ToolSvc
from LArRecUtils.LArRecUtilsConf import LArAutoCorrTotalTool
LArAutoCorrTotalTool = LArAutoCorrTotalTool("LArAutoCorrTotalTool")
ToolSvc += LArAutoCorrTotalTool

# Set data type (needed to decide wether to retrieve LArNoise or LArPedestal for the noise)
from AthenaCommon.GlobalFlags import globalflags
if globalflags.DataSource()=='data':
    LArAutoCorrTotalTool.IsMC=False
