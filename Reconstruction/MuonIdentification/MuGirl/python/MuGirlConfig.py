# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from MuGirl.MuGirlConf import MuGirl, MuGirlNS__MuGirlRecoTool
from MuGirl.MuGirlRecoConfig import MuGirlRecoConfig
from AthenaCommon.Configurable import *
from AthenaCommon.AppMgr import ToolSvc

class MuGirlConfig(MuGirl):
    __slots__ = {}

    def __init__(self, name = Configurable.DefaultName):
        super(MuGirlConfig, self).__init__(name)
        global ToolSvc

        RecoTool = MuGirlRecoConfig(name="MuGirlRecoTool")
        ToolSvc += RecoTool
        self.MuGirlReconstruction = RecoTool

MuGirl = MuGirlConfig()
