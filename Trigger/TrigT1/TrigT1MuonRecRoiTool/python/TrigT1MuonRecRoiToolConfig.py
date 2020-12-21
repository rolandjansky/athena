# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration


# Local (generated) configurable(s):
from TrigT1MuonRecRoiTool.TrigT1MuonRecRoiToolConf import LVL1__TrigT1RPCRecRoiTool
from TrigT1MuonRecRoiTool.TrigT1MuonRecRoiToolConf import LVL1__TrigT1TGCRecRoiTool

def getRun3RPCRecRoiTool(name = "RPCRecRoiTool"):
    tool = LVL1__TrigT1RPCRecRoiTool(name)
    tool.UseRun3Config=True
    return tool

def getRun3TGCRecRoiTool(name = "TGCRecRoiTool"):
    tool = LVL1__TrigT1TGCRecRoiTool(name)
    tool.UseRun3Config=True
    return tool
