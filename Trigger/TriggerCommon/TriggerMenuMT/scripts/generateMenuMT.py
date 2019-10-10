#!/bin/env python

# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration



# import flags
include("TrigUpgradeTest/testHLT_MT.py")

#################################
# Configure L1Decoder
#################################

# provide a minimal menu information
if globalflags.InputFormat.is_bytestream():
   topSequence.L1Decoder.ctpUnpacker.OutputLevel=DEBUG



# this is a temporary hack to include new test chains
# very hacky indeed, the L1 part should be in in the GenerateMenuMT part

from TriggerJobOpts.TriggerFlags import TriggerFlags
#TriggerFlags.triggerMenuSetup = "LS2_v1"
TriggerFlags.triggerMenuSetup = "Dev_pp_run3_v1"

from TriggerMenuMT.HLTMenuConfig.Menu.GenerateMenuMT import GenerateMenuMT
g = GenerateMenuMT()
#allChainConfigs = g.generateChainConfigs()
allChainConfigs = g.getChainsFromMenu()

EnabledChainNamesToCTP = dict([ (c[0], c[1])  for c in allChainConfigs])
topSequence.L1Decoder.ChainToCTPMapping = EnabledChainNamesToCTP



##########################################
# Menu and CF construction
##########################################
g.generateMT()




