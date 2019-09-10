#
#  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#

from DecisionHandling.DecisionHandlingConf import InputMakerForRoI
from AthenaCommon.CFElements import seqAND
from TrigPartialEventBuilding.TrigPartialEventBuildingConf import PEBInfoWriterAlg,StaticPEBInfoWriterTool,RoIPEBInfoWriterTool
from TriggerMenuMT.HLTMenuConfig.Menu.MenuComponents import MenuSequence
from libpyeformat_helper import SourceIdentifier,SubDetector

import sys
from AthenaCommon.Logging import logging
log = logging.getLogger('pebMenuDefs')

##################################################################
# PEB Info Writer step
##################################################################
def pebInputMaker(name):
    maker = InputMakerForRoI("IMpeb_"+name, mergeOutputs=False)
    maker.RoIs="pebInputRoI_"+name
    return maker

def pebSequence(inputMaker):
    return seqAND("pebSequence_"+inputMaker.name(), [inputMaker])

def addHLTResultToROBList(robList, moduleId=0):
    hltResultSID = SourceIdentifier(SubDetector.TDAQ_HLT,moduleId)
    robList.extend([hltResultSID.code()])

def pebInfoWriterToolFromDict(chainDict):
    name = chainDict['chainName']
    # pebtestone is an example which saves a few detector ROBs and the full HLT result (typically saved in physics streams)
    if "pebtestone" in name:
        tool = StaticPEBInfoWriterTool(name)
        tool.ROBList = [0x42002e, 0x420060, 0x420064] # a few example LAr ROBs
        addHLTResultToROBList(tool.ROBList) # add the main (full) HLT result to the list
        return tool
    # pebtesttwo is an example which saves some detector data, but no HLT result (not needed in detector calibration streams)
    if "pebtesttwo" in name:
        tool = StaticPEBInfoWriterTool(name)
        tool.SubDetList = [
            int(SubDetector.MUON_RPC_BARREL_A_SIDE),
            int(SubDetector.MUON_RPC_BARREL_C_SIDE)] # example: RPC side A and C
        return tool
    # pebtestthree is same as pebtestone, but does not write any HLT result (in DS+PEB example the DS writer adds the HLT result to the list)
    if "pebtestthree" in name:
        tool = StaticPEBInfoWriterTool(name)
        tool.ROBList = [0x42002e, 0x420060, 0x420064] # a few example LAr ROBs
        return tool
    # pebtestfour is the first example using RoIPEBInfoWriterTool which writes all ROBs within a given RoI, and also the main (full) HLT result
    if "pebtestfour" in name:
        tool = RoIPEBInfoWriterTool(name)
        tool.EtaEdge = 5.0
        tool.EtaWidth = 0.1
        tool.PhiWidth = 0.1
        tool.DetNames = ["All"]
        tool.ExtraROBs = []
        tool.ExtraSubDets = []
        addHLTResultToROBList(tool.ExtraROBs) # add the main (full) HLT result to the list
        return tool
    # pebtestfive is similar to pebtestfour, but saves only muon detector ROBs within a larger RoI plus the HLT result
    if "pebtestfive" in name:
        tool = RoIPEBInfoWriterTool(name)
        tool.EtaWidth = 0.5
        tool.PhiWidth = 0.5
        tool.DetNames = ["MDT", "CSC", "RPC", "TGC", "MM", "STGC"] # all muon detectors
        tool.ExtraROBs = []
        addHLTResultToROBList(tool.ExtraROBs) # add the main (full) HLT result to the list
        return tool
    else:
        log.error("Unknown name %s passed to pebInfoWriterToolFromDict" % name)
        sys.exit("Configuration error")

def pebInfoWriterSequence(name,toolGenerator=pebInfoWriterToolFromDict):
    """Creates a MenuSequence for PEBInfo writer. The algorithm and tools are given unique names derived from
    the name parameter. This is required to avoid execution stall from having the same algorithm instance configured
    in different steps awaiting different inputs."""
    inputMaker = pebInputMaker(name)
    return MenuSequence(Sequence    = pebSequence(inputMaker),
                        Maker       = inputMaker,
                        Hypo        = PEBInfoWriterAlg("PEBInfoWriterAlg_"+name),
                        HypoToolGen = toolGenerator)

##################################################################
# Special case for data scouting
##################################################################

# This mapping has to be fixed somewhere in the menu/EDM config. Once an ID is assigned to a name,
# it should not change throughout Run 3. The ID will be used for encoding/decoding the data scouting result.
def dataScoutingResultIDFromName(name):
    if "dataScoutingElectronTest" in name:
      return 3 # just an example
    else:
      log.error("Unknown name %s, cannot assign result ID" % name)
      sys.exit("Configuration error")

def dataScoutingInfoWriter(chainDict):
    name = chainDict['chainName']
    '''Creates a StaticPEBInfoWriterTool, which adds the data scouting HLT result to the PEBInfo'''
    tool = StaticPEBInfoWriterTool(name)
    moduleId = dataScoutingResultIDFromName(name)
    addHLTResultToROBList(tool.ROBList, moduleId)
    tool.SubDetList = []
    return tool

def dataScoutingSequence(name):
    return pebInfoWriterSequence(name,dataScoutingInfoWriter)
