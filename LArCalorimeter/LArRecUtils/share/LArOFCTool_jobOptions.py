# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

include.block("LArRecUtils/LArOFCTool_jobOptions.py")

printfunc (" WARNING obsolete jobOption. Please use python class to configure LArOFCTool")

from AthenaCommon.AppMgr import ToolSvc
from LArRecUtils.LArRecUtilsConf import LArOFCTool
theLArOFCTool = LArOFCTool("LArOFCTool")

# Set data type (needed to decide wether to retrieve LArNoise or LArPedestal for the noise)
from AthenaCommon.GlobalFlags import globalflags
if globalflags.DataSource()=='data':
    theLArOFCTool.IsMC=False

ToolSvc+=theLArOFCTool
