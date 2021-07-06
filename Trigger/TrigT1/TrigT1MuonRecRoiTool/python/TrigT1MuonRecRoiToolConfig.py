# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
from AthenaConfiguration.ComponentFactory import CompFactory

def getRun3RPCRecRoiTool(name = "RPCRecRoiTool", useRun3Config=True):
    tool = CompFactory.getComp("LVL1::TrigT1RPCRecRoiTool")(name)
    tool.UseRun3Config=useRun3Config
    return tool

def getRun3TGCRecRoiTool(name = "TGCRecRoiTool", useRun3Config=True):
    tool = CompFactory.getComp("LVL1::TrigT1TGCRecRoiTool")(name)
    tool.UseRun3Config=useRun3Config
    return tool
