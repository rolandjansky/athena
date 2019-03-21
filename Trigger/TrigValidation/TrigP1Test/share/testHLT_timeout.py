#
#  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#

# This file is based on MTCalibPeb.py

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
               "HLT_MTCalibPeb2": "L1_RD0_FILLED"}

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

# Chain 1 - high accept rate, sleeps for up to 2 seconds
hypoTool1 = MTCalibPebHypoTool("HLT_MTCalibPeb1")
hypoTool1.RandomAcceptRate = 0.75
hypoTool1.BurnTimePerCycleMillisec = 200
hypoTool1.NumBurnCycles = 10

# Chain 2 - lower accept rate, sleeps for up to 3 seconds
hypoTool2 = MTCalibPebHypoTool("HLT_MTCalibPeb2")
hypoTool2.RandomAcceptRate = 0.25
hypoTool2.BurnTimePerCycleMillisec = 600
hypoTool2.NumBurnCycles = 5

# Add the hypo tools to the algorithm
hypo.HypoTools = [hypoTool1, hypoTool2]

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

# StreamTag definitions
streamPhysicsMain = ['Main', 'physics', "True", "True"]

# Tool adding stream tags to HLT result
stmaker = StreamTagMakerTool()
stmaker.ChainDecisions = "HLTSummary"
stmaker.PEBDecisionKeys = [hypo.HypoOutputDecisions]
stmaker.ChainToStream = {}
stmaker.ChainToStream["HLT_MTCalibPeb1"] = streamPhysicsMain
stmaker.ChainToStream["HLT_MTCalibPeb2"] = streamPhysicsMain

# Tool adding HLT bits to HLT result
bitsmaker = TriggerBitsMakerTool()
bitsmaker.ChainDecisions = "HLTSummary"
bitsmaker.ChainToBit = {}
bitsmaker.ChainToBit["HLT_MTCalibPeb1"] = 4
bitsmaker.ChainToBit["HLT_MTCalibPeb2"] = 32

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
