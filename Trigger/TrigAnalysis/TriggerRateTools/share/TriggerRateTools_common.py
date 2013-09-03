TriggerFlags.Lvl1.RemapThresholdsAsListed = True

# Add "always" chains to menu
def addAlwaysAcceptChain():
    TriggerFlags.MinBiasSlice.enableSignature("always")
from TriggerMenuPython.GenerateMenu import GenerateMenu
GenerateMenu.overwriteSignaturesWith(addAlwaysAcceptChain)

from TriggerJobOpts.TriggerFlags import TriggerFlags
TriggerFlags.configurationSourceList=['ds']

from TrigDecisionMaker.TrigDecisionMakerConfig import WriteTrigDecision
trigDecWriter = WriteTrigDecision()

# Default TriggerRateTools output & config

from AthenaCommon.AlgSequence import AlgSequence
TopAlg = AlgSequence()
from TriggerRateTools.TriggerRateToolsConf import Trig__TriggerRateTools
triggerRateTools = Trig__TriggerRateTools("TriggerRateTools")

# TriggerRateTools options
#ServiceMgr.THistSvc.Output += ["TriggerRateTools DATAFILE='TriggerRates.root' OPT='RECREATE'"]
triggerRateTools.doTextOutput         = False
triggerRateTools.doVeto               = False
triggerRateTools.doRawTD              = False
triggerRateTools.OutputLevel          = 3
triggerRateTools.xSection             = 1.0 #barn^-1 s^-1
triggerRateTools.Luminosity           = 1.0 #barn
triggerRateTools.collsPerEvent	      = 1.0 	
triggerRateTools.IgnoreList           = ["L2_always","EF_always"]
triggerRateTools.CplxAndList          = []
triggerRateTools.CplxOrList           = []
triggerRateTools.PrescaleOverrideList = [["L1_RD0_FILLED","512"]]
triggerRateTools.CumulateList         = []
triggerRateTools.MenusList            = []
triggerRateTools.StreamsList          = []
triggerRateTools.doSlicesInfo         = False
triggerRateTools.ignorePassThrough    = False
triggerRateTools.doRetrieveItems      = False

TopAlg += triggerRateTools

# Steering changes

if TriggerFlags.doLVL1:
    TopAlg.TrigSteer_L2.LvlConverterTool.Lvl1ResultAccessTool.ignorePrescales=True

if TriggerFlags.doLVL2:
    TopAlg.TrigSteer_L2.LvlConverterTool.ignorePrescales=True   
 
if TriggerFlags.doEF:
    TopAlg.TrigSteer_EF.LvlConverterTool.ignorePrescales=True

