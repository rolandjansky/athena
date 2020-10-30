#
#  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#

from TrigPartialEventBuilding.TrigPartialEventBuildingConf import StaticPEBInfoWriterTool, RoIPEBInfoWriterTool
from TrigEDMConfig.DataScoutingInfo import getFullHLTResultID
from libpyeformat_helper import SourceIdentifier, SubDetector
from RegionSelector import RegSelToolConfig


class StaticPEBInfoWriterToolCfg(StaticPEBInfoWriterTool):
    def addROBs(self, robs):
        self.ROBList.extend(robs)

    def addSubDets(self, dets):
        self.SubDetList.extend([int(detid) for detid in dets])

    def addHLTResultToROBList(self, moduleId=getFullHLTResultID()):
        hltResultSID = SourceIdentifier(SubDetector.TDAQ_HLT, moduleId)
        self.addROBs([hltResultSID.code()])

    def addCTPResultToROBList(self, moduleId=0):
        ctpResultSID = SourceIdentifier(SubDetector.TDAQ_CTP, moduleId)
        self.addROBs([ctpResultSID.code()])


class RoIPEBInfoWriterToolCfg(RoIPEBInfoWriterTool):
    def addRegSelDets(self, detNames):
        '''
        Add RegionSelector tools for given detector look-up tables to build PEB list of ROBs
        in these detectors that intersect with the RoI. Special value 'All' can be also given
        in the detNames list to include all detectors available in RegionSelector.
        '''
        if 'All' in detNames:
            detNames = [
                'Pixel', 'SCT', 'TRT',  # ID
                'MDT', 'RPC', 'TGC', 'CSC', 'MM', 'sTGC',  # Muon
                'TTEM', 'TTHEC', 'FCALEM', 'FCALHAD', 'TILE']  # Calo
        for det in detNames:
            funcName = 'makeRegSelTool_' + det
            if not hasattr(RegSelToolConfig, funcName):
                raise RuntimeError('Cannot add detector "' + det + '", RegSelToolConfig does not have a function ' + funcName)
            func = getattr(RegSelToolConfig, funcName)
            if not callable(func):
                raise RuntimeError('Cannot add detector "' + det + '", RegSelToolConfig.' + funcName + ' is not callable')
            self.RegionSelectorTools += [func()]

    def addROBs(self, robs):
        '''Add extra fixed list of ROBs independent of RoI'''
        self.ExtraROBs.extend(robs)

    def addSubDets(self, dets):
        '''Add extra fixed list of SubDets independent of RoI'''
        self.ExtraSubDets.extend(dets)

    def addHLTResultToROBList(self, moduleId=getFullHLTResultID()):
        hltResultSID = SourceIdentifier(SubDetector.TDAQ_HLT, moduleId)
        self.addROBs([hltResultSID.code()])

    def addCTPResultToROBList(self, moduleId=0):
        ctpResultSID = SourceIdentifier(SubDetector.TDAQ_CTP, moduleId)
        self.addROBs([ctpResultSID.code()])
