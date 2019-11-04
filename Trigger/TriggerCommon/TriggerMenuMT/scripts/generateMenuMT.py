# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

# import flags
include("TrigUpgradeTest/testHLT_MT.py")  # noqa: F821

#################################
# Configure L1Decoder
#################################

# provide a minimal menu information
from AthenaCommon.GlobalFlags import globalflags
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()
if globalflags.InputFormat.is_bytestream():
   from AthenaCommon.Constants import DEBUG
   topSequence.L1Decoder.ctpUnpacker.OutputLevel=DEBUG  # noqa: ATL900



# this is a temporary hack to include new test chains
# very hacky indeed, the L1 part should be in in the GenerateMenuMT part

from TriggerJobOpts.TriggerFlags import TriggerFlags
TriggerFlags.triggerMenuSetup = "LS2_v1"

from TriggerMenuMT.HLTMenuConfig.Menu.GenerateMenuMT import GenerateMenuMT
g = GenerateMenuMT()





##########################################
# Menu and CF construction
##########################################
g.generateMT()




