##########################################
# Menu and CF construction
##########################################

# import flags
include("TrigUpgradeTest/testHLT_MT.py")

from TriggerJobOpts.TriggerFlags import TriggerFlags
TriggerFlags.triggerMenuSetup = "LS2_v1"

from TriggerMenuMT.HLTMenuConfig.Menu.GenerateMenuMT import GenerateMenuMT
g = GenerateMenuMT()



# set some flags
g.doEgammaChains=True

g.generateMT()

#allChainConfigs = g.getChainsFromMenu()
#EnabledChainNamesToCTP = dict([ (c[0], c[1])  for c in allChainConfigs])
#topSequence.L1DecoderTest.ChainToCTPMapping = EnabledChainNamesToCTP
 


##########################################  
# Some debug
##########################################  
from AthenaCommon.AlgSequence import dumpSequence
dumpSequence(topSequence)

