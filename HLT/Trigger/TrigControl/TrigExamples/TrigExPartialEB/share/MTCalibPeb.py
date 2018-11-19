#
#  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
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

# Initialise L1 decoding tools
from L1Decoder.L1DecoderConf import CTPUnpackingTool
ctpUnpacker = CTPUnpackingTool(ForceEnableAllChains = True)
# Can add other tools here if needed

# Define the "menu"
chainCTPMap = {"HLT_MTCalibPeb1": "L1_NIML1A",
               "HLT_MTCalibPeb2": "L1_NIML1A"}

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

hypoTool1 = MTCalibPebHypoTool("HLT_MTCalibPeb1")
hypoTool1.RandomAcceptRate = 0.75

hypoTool2 = MTCalibPebHypoTool("HLT_MTCalibPeb2")
hypoTool2.RandomAcceptRate = 0.25

hypo.HypoTools = [hypoTool1, hypoTool2]

################################################################################
# HLT result maker configuration
################################################################################

from TrigOutputHandling.TrigOutputHandlingConf import TriggerEDMSerialiserTool, StreamTagMakerTool, TriggerBitsMakerTool

# Tool serialising EDM objects to fill the HLT result
serialiser = TriggerEDMSerialiserTool()
serialiser.CollectionsToSerialize = [] # nothing at the moment

# Tool adding stream tags to HLT result
stmaker = StreamTagMakerTool()
stmaker.ChainDecisions = "HLTFinalDecisions"
stmaker.ChainToStream = {}
stmaker.ChainToStream["HLT_MTCalibPeb1"] = "DataScouting_05_Jets"
stmaker.ChainToStream["HLT_MTCalibPeb2"] = "Main"

# Tool adding HLT bits to HLT result
bitsmaker = TriggerBitsMakerTool()
bitsmaker.ChainDecisions = "HLTFinalDecisions"
bitsmaker.ChainToBit = {}
bitsmaker.ChainToBit["HLT_MTCalibPeb1"] = 3
bitsmaker.ChainToBit["HLT_MTCalibPeb2"] = 50

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
