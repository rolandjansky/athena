##########################################
# Menu and CF construction
##########################################

# import flags
include("TrigUpgradeTest/testHLT_MT.py")

from AthenaCommon.Constants import VERBOSE,INFO,DEBUG
from TriggerJobOpts.TriggerFlags import TriggerFlags
TriggerFlags.triggerMenuSetup = "LS2_v1"

from TriggerMenuMT.HLTMenuConfig.Menu.GenerateMenuMT import GenerateMenuMT
menu = GenerateMenuMT(DEBUG)

def signaturesToGenerate():
    TriggerFlags.Slices_all_setOff()
    TriggerFlags.EgammaSlice.setAll()
    TriggerFlags.MuonSlice.setAll()


# generate the Chains from the Menu Dictionary
menu.overwriteSignaturesWith(signaturesToGenerate)
allChainConfigs = menu.generateMT()


##########################################  
# Some debug
##########################################  
from AthenaCommon.AlgSequence import dumpSequence
dumpSequence(topSequence)

