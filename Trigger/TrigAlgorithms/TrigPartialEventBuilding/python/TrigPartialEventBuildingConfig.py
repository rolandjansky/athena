#
#  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#

from AthenaConfiguration.ComponentFactory import CompFactory
from TrigEDMConfig.DataScoutingInfo import getFullHLTResultID
from libpyeformat_helper import SourceIdentifier, SubDetector
from RegionSelector import RegSelToolConfig


def StaticPEBInfoWriterToolCfg(name='StaticPEBInfoWriterTool'):
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

    CompFactory.StaticPEBInfoWriterTool.addROBs = addROBs
    CompFactory.StaticPEBInfoWriterTool.addSubDets = addSubDets
    CompFactory.StaticPEBInfoWriterTool.addHLTResultToROBList = addHLTResultToROBList
    CompFactory.StaticPEBInfoWriterTool.addCTPResultToROBList = addCTPResultToROBList

    tool = CompFactory.StaticPEBInfoWriterTool(name)

    return tool


def RoIPEBInfoWriterToolCfg(name='RoIPEBInfoWriterTool'):
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
        self.ExtraSubDets.extend([int(detid) for detid in dets])

    def addHLTResultToROBList(self, moduleId=getFullHLTResultID()):
        hltResultSID = SourceIdentifier(SubDetector.TDAQ_HLT, moduleId)
        self.addROBs([hltResultSID.code()])

    def addCTPResultToROBList(self, moduleId=0):
        ctpResultSID = SourceIdentifier(SubDetector.TDAQ_CTP, moduleId)
        self.addROBs([ctpResultSID.code()])

    CompFactory.RoIPEBInfoWriterTool.addRegSelDets = addRegSelDets
    CompFactory.RoIPEBInfoWriterTool.addROBs = addROBs
    CompFactory.RoIPEBInfoWriterTool.addSubDets = addSubDets
    CompFactory.RoIPEBInfoWriterTool.addHLTResultToROBList = addHLTResultToROBList
    CompFactory.RoIPEBInfoWriterTool.addCTPResultToROBList = addCTPResultToROBList

    tool = CompFactory.RoIPEBInfoWriterTool(name)

    return tool
