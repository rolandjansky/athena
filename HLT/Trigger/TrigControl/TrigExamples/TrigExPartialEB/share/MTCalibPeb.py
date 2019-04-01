#
#  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#

################################################################################
# General set up
################################################################################

# The top algo sequence
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

# SGInputLoader takes care of unmet input dependencies (e.g. triggering conversion from BS)
from AthenaCommon.AlgScheduler import AlgScheduler
AlgScheduler.setDataLoaderAlg ('SGInputLoader')

# EventInfoCnvAlg produces xAOD::EventInfo from the plain non-xAOD EventInfo
from xAODEventInfoCnv.xAODEventInfoCreator import xAODMaker__EventInfoCnvAlg
topSequence += xAODMaker__EventInfoCnvAlg()

################################################################################
# L1 result decoding
################################################################################

# Configure BS converter and address provider for RoIBResult decoding
from AthenaCommon.AppMgr import ServiceMgr as svcMgr
svcMgr.ByteStreamCnvSvc.GetDetectorMask = True
svcMgr.ByteStreamCnvSvc.InitCnvs += [ "ROIB::RoIBResult" ]
svcMgr.ByteStreamAddressProviderSvc.TypeNames += [ "ROIB::RoIBResult/RoIBResult" ]

# Ensure LVL1ConfigSvc is initialised before L1Decoder handles BeginRun incident
# This should be done by the L1Decoder configuration in new-style job options (with component accumulator)
from TrigConfigSvc.TrigConfigSvcConfig import LVL1ConfigSvc, findFileInXMLPATH
svcMgr += LVL1ConfigSvc()

# Set the LVL1 menu (needed for initialising LVL1ConfigSvc)
from TriggerJobOpts.TriggerFlags import TriggerFlags
svcMgr.LVL1ConfigSvc.XMLMenuFile = findFileInXMLPATH(TriggerFlags.inputLVL1configFile())

# Initialise L1 decoding tools
from L1Decoder.L1DecoderConf import CTPUnpackingTool
ctpUnpacker = CTPUnpackingTool(ForceEnableAllChains = True)
# Can add other tools here if needed

# Define the "menu" - L1 items do not matter if we set ForceEnableAllChains = True,
# but they have to be defined in the L1 menu xml
chainCTPMap = {"HLT_MTCalibPeb1": "L1_RD0_FILLED",
               "HLT_MTCalibPeb2": "L1_RD0_FILLED",
               "HLT_MTCalibPeb3": "L1_RD0_FILLED"}

# Schedule the L1Decoder algo with the above tools
from L1Decoder.L1DecoderConf import L1Decoder
l1decoder = L1Decoder()
l1decoder.ctpUnpacker = ctpUnpacker
l1decoder.ChainToCTPMapping = chainCTPMap
topSequence += l1decoder

################################################################################
# HLT configuration
################################################################################

from TrigExPartialEB.TrigExPartialEBConf import MTCalibPebHypoAlg,MTCalibPebHypoTool
hypo = MTCalibPebHypoAlg()
hypo.HypoInputDecisions = "HLTChains"
hypo.HypoOutputDecisions = "MTCalibPebDecisions"

# Chain 1 - high rate, writes PEB info
hypoTool1 = MTCalibPebHypoTool("HLT_MTCalibPeb1")
hypoTool1.RandomAcceptRate = 0.75
hypoTool1.BurnTimePerCycleMillisec = 10
hypoTool1.NumBurnCycles = 3
hypoTool1.PEBROBList = [0x42002a, 0x42002b] # example LAr EMBC ROBs
hypoTool1.PEBSubDetList = [0x65, 0x66] # RPC A and C side

# Chain 2 - lower rate, makes ROB requests
hypoTool2 = MTCalibPebHypoTool("HLT_MTCalibPeb2")
hypoTool2.RandomAcceptRate = 0.25
hypoTool2.BurnTimePerCycleMillisec = 20
hypoTool2.NumBurnCycles = 10
hypoTool2.TimeBetweenROBReqMillisec = 50
exampleROBList = [0x420024, 0x420025, 0x420026, 0x420027, 0x420034, 0x420035, 0x420036, 0x420037,
                  0x42005c, 0x42005d, 0x42005e, 0x42005f, 0x42006c, 0x42006d, 0x42006e, 0x42006f] # ROS-LAR-EMBC-02
hypoTool2.ROBAccessDict = {
 "01 :ADD:       Preload ":  [ 0x420024, 0x420025 ], # robs for 1st preload
 "02 :ADD:       Preload ":  [ 0x420026, 0x420027 ], # robs for 2nd preload
 "03 :GET:       Retrieve ": [ 0x420025, 0x420026 ], # robs for 1st retrieval (prefetched)
 "04 :GET:       Retrieve ": [ 0x420034 ],           # robs for 2nd retrieval (not prefetched)
 "05 :ADD:       Preload ":  exampleROBList,         # robs for 3rd preload (the full list)
 "05 :GET:RND5:  Retrieve ": exampleROBList,         # robs for 3rd retrieval (5 random from the list)
 "06 :GET:RND10: Retrieve ": exampleROBList,         # robs for 4th retrieval (10 random from the list)
 "07 :COL:       Ev.Build ": []                      # event building
} # This is just an example with a few ROBs (LAr in this case) for testing the ROBDataProvider

# Chain 3 - medium rate, produces random data, writes PEB info for data scouting
hypoTool3 = MTCalibPebHypoTool("HLT_MTCalibPeb3")
hypoTool3.RandomAcceptRate = 0.50
hypoTool3.BurnTimePerCycleMillisec = 20
hypoTool3.NumBurnCycles = 5
hypoTool3.CreateRandomData = {
  "ExampleCollection1": 5,
  "ExampleCollection2": 3,
}
hypoTool3.PEBROBList = [0x42002e, 0x42002f] # example LAr EMBC ROBs
hypoTool3.PEBROBList.extend([0x7c0001]) # extra HLT result for data scouting

# Add the hypo tools to the algorithm
hypo.HypoTools = [hypoTool1, hypoTool2, hypoTool3]

################################################################################
# HLT result maker configuration
################################################################################
from TrigOutputHandling.TrigOutputHandlingConf import HLTResultMTMakerAlg, StreamTagMakerTool, TriggerBitsMakerTool
from TrigOutputHandling.TrigOutputHandlingConfig import TriggerEDMSerialiserToolCfg, HLTResultMTMakerCfg

# Tool serialising EDM objects to fill the HLT result
serialiser = TriggerEDMSerialiserToolCfg("Serialiser")
serialiser.addCollectionListToMainResult([
  "xAOD::TrigCompositeContainer_v1#"+hypo.HypoOutputDecisions,
  "xAOD::TrigCompositeAuxContainer_v2#"+hypo.HypoOutputDecisions+"Aux.",
])
# Data scouting example
resultList = [serialiser.fullResultID(), 1]
serialiser.addCollectionListToResults([
  "xAOD::TrigCompositeContainer_v1#ExampleCollection1",
  "xAOD::TrigCompositeAuxContainer_v2#ExampleCollection1Aux.floatVec_1.floatVec_2.floatVec_3.floatVec_4.floatVec_5",
  "xAOD::TrigCompositeContainer_v1#ExampleCollection2",
  "xAOD::TrigCompositeAuxContainer_v2#ExampleCollection2Aux.floatVec_1.floatVec_2.floatVec_3",
], resultList)

# StreamTag definitions
streamPhysicsMain = ['Main', 'physics', "True", "True"]
streamExamplePEB = ['ExamplePEB', 'calibration', "True", "False"]
streamExampleDataScoutingPEB = ['ExampleDataScoutingPEB', 'physics', "True", "False"]

# Tool adding stream tags to HLT result
stmaker = StreamTagMakerTool()
stmaker.ChainDecisions = "HLTSummary"
stmaker.PEBDecisionKeys = [hypo.HypoOutputDecisions]
stmaker.ChainToStream = {}
stmaker.ChainToStream["HLT_MTCalibPeb1"] = streamExamplePEB
stmaker.ChainToStream["HLT_MTCalibPeb2"] = streamPhysicsMain
stmaker.ChainToStream["HLT_MTCalibPeb3"] = streamExampleDataScoutingPEB

# Tool adding HLT bits to HLT result
bitsmaker = TriggerBitsMakerTool()
bitsmaker.ChainDecisions = "HLTSummary"
bitsmaker.ChainToBit = {}
bitsmaker.ChainToBit["HLT_MTCalibPeb1"] = 3
bitsmaker.ChainToBit["HLT_MTCalibPeb2"] = 50
bitsmaker.ChainToBit["HLT_MTCalibPeb3"] = 11

# Configure the HLT result maker to use the above tools
hltResultMaker = svcMgr.HltEventLoopMgr.ResultMaker
hltResultMaker.MakerTools = [ stmaker, bitsmaker, serialiser ]

################################################################################
# Control Flow construction
################################################################################

from DecisionHandling.DecisionHandlingConf import TriggerSummaryAlg
summary = TriggerSummaryAlg( "TriggerSummaryAlg" )
summary.InputDecision = "HLTChains"
summary.FinalDecisions = [ hypo.HypoOutputDecisions ]

from TrigOutputHandling.TrigOutputHandlingConf import DecisionSummaryMakerAlg
summMaker = DecisionSummaryMakerAlg()
summMaker.FinalDecisionKeys = [ hypo.HypoOutputDecisions ]
summMaker.FinalStepDecisions =  dict( [ ( tool.getName(), hypo.HypoOutputDecisions ) for tool in hypo.HypoTools ] )
print summMaker

# Create a top-level algorithm as a sequence
from AthenaCommon.CFElements import seqOR
hltTop = seqOR( "hltTop", [ hypo, summary, summMaker ] )
topSequence += hltTop

# Print configuration for debugging
print("Dump of topSequence")
from AthenaCommon.AlgSequence import dumpSequence
dumpSequence(topSequence)
print("Dump of serviceMgr")
dumpSequence(ServiceMgr)
