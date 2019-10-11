#
#  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#

from TrigPartialEventBuilding.TrigPartialEventBuildingConf import StaticPEBInfoWriterTool, RoIPEBInfoWriterTool
from TriggerMenuMT.HLTMenuConfig.Menu.EventBuildingInfo import getFullHLTResultID
from libpyeformat_helper import SourceIdentifier, SubDetector


class StaticPEBInfoWriterToolCfg(StaticPEBInfoWriterTool):
    def addROBs(self, robs):
        self.ROBList.extend(robs)

    def addSubDets(self, dets):
        self.SubDetList.extend(dets)

    def addHLTResultToROBList(self, moduleId=getFullHLTResultID()):
        hltResultSID = SourceIdentifier(SubDetector.TDAQ_HLT, moduleId)
        self.addROBs([hltResultSID.code()])

    def addCTPResultToROBList(self, moduleId=0):
        ctpResultSID = SourceIdentifier(SubDetector.TDAQ_CTP, moduleId)
        self.addROBs([ctpResultSID.code()])


class RoIPEBInfoWriterToolCfg(RoIPEBInfoWriterTool):
    def addROBs(self, robs):
        self.ExtraROBs.extend(robs)

    def addSubDets(self, dets):
        self.ExtraSubDets.extend(dets)

    def addHLTResultToROBList(self, moduleId=getFullHLTResultID()):
        hltResultSID = SourceIdentifier(SubDetector.TDAQ_HLT, moduleId)
        self.addROBs([hltResultSID.code()])

    def addCTPResultToROBList(self, moduleId=0):
        ctpResultSID = SourceIdentifier(SubDetector.TDAQ_CTP, moduleId)
        self.addROBs([ctpResultSID.code()])
