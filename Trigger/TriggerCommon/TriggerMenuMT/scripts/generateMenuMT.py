#!/bin/env python

# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration



# import flags
include("TrigUpgradeTest/testHLT_MT.py")

#################################
# Configure L1Decoder
#################################

# provide a minimal menu information
if globalflags.InputFormat.is_bytestream():
   topSequence.L1DecoderTest.ctpUnpacker.OutputLevel=DEBUG
   topSequence.L1DecoderTest.roiUnpackers[0].OutputLevel=DEBUG

# map L1 decisions for menu
for unpack in topSequence.L1DecoderTest.roiUnpackers:
    if unpack.name() is "EMRoIsUnpackingTool":
        unpack.Decisions="L1EM"
        emUnpacker=unpack
    if unpack.name() is "MURoIsUnpackingTool":
        unpack.Decisions="L1MU"
        
for unpack in topSequence.L1DecoderTest.rerunRoiUnpackers:
    if unpack.name() is "EMRerunRoIsUnpackingTool":
        unpack.Decisions="RerunL1EM"
        unpack.SourceDecisions="L1EM"

for unpack in topSequence.L1DecoderTest.rerunRoiUnpackers:
    if unpack.name() is "EMRerunRoIsUnpackingTool":
        unpack.SourceDecisions="L1EM"
    if unpack.name() is "MURerunRoIsUnpackingTool":
        unpack.SourceDecisions="L1MU"



# this is a temporary hack to include new test chains
# very hacky indeed, the L1 part should be in in the GenerateMenuMT part

from TriggerJobOpts.TriggerFlags import TriggerFlags
TriggerFlags.triggerMenuSetup = "LS2_v1"

from TriggerMenuMT.HLTMenuConfig.Menu.GenerateMenuMT import GenerateMenuMT
g = GenerateMenuMT()
#allChainConfigs = g.generateChainConfigs()
allChainConfigs = g.getChainsFromMenu()

EnabledChainNamesToCTP = dict([ (c[0], c[1])  for c in allChainConfigs])
topSequence.L1DecoderTest.ChainToCTPMapping = EnabledChainNamesToCTP



##########################################
# Menu and CF construction
##########################################
g.generateMT()




