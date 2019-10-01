# import flags
from TriggerJobOpts.TriggerFlags import TriggerFlags
TriggerFlags.triggerMenuSetup = "LS2_v1"

include("TrigUpgradeTest/testHLT_MT.py")

#################################
# Configure L1Decoder
#################################

# provide a minimal menu information
if globalflags.InputFormat.is_bytestream():
   topSequence.L1Decoder.ctpUnpacker.OutputLevel=DEBUG


##########################################
# Menu and CF construction
##########################################





from TriggerMenuMT.HLTMenuConfig.Menu.GenerateMenuMT import GenerateMenuMT
g = GenerateMenuMT()
g.generateMT()





