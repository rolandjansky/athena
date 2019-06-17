#
#  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#

from AthenaCommon.Logging import logging
__log = logging.getLogger('full_menu')

# import flags
from RecExConfig.RecFlags  import rec
rec.doESD=True
rec.doWriteESD=True

include("TrigUpgradeTest/testHLT_MT.py")



##################################################################
# Generate the menu
##################################################################

from TriggerJobOpts.TriggerFlags import TriggerFlags
TriggerFlags.triggerMenuSetup = "LS2_v1"

from TriggerMenuMT.HLTMenuConfig.Menu.GenerateMenuMT import GenerateMenuMT
menu = GenerateMenuMT()


def signaturesToGenerate():
    TriggerFlags.Slices_all_setOff()
    for sig in opt.enabledSignatures:
        eval(sig)    
        
menu.overwriteSignaturesWith(signaturesToGenerate)
allChainConfigs = menu.generateMT()


##########################################
# Some debug
##########################################
from AthenaCommon.AlgSequence import dumpSequence, AthSequencer
dumpSequence(topSequence)


import DecisionHandling
for a in AthSequencer("HLTAllSteps").getChildren():
    if isinstance(a, DecisionHandling.DecisionHandlingConf.TriggerSummaryAlg):
        a.OutputLevel = DEBUG


# this part uses parts from the NewJO configuration, it is very hacky for the moment

from TriggerJobOpts.TriggerConfig import collectHypos, collectFilters, collectDecisionObjects, triggerOutputStreamCfg
hypos = collectHypos(AthSequencer("HLTAllSteps"))
filters = collectFilters(AthSequencer("HLTAllSteps"))

# try to find L1Decoder
from AthenaCommon.CFElements import findAlgorithm,findSubSequence
l1decoder = findAlgorithm(topSequence,'L1Decoder')
if not l1decoder:
    l1decoder = findAlgorithm(topSequence,'L1EmulationTest')

if l1decoder:
    decObj = collectDecisionObjects( hypos, filters, l1decoder )
    __log.debug("Decision Objects to export to ESD [hack method - should be replaced with triggerRunCfg()]")
    __log.debug(decObj)

    from TrigEDMConfig.TriggerEDMRun3 import TriggerHLTList
    ItemList  = [ 'xAOD::TrigCompositeContainer#{}'.format(d) for d in decObj ]
    ItemList += [ 'xAOD::TrigCompositeAuxContainer#{}Aux.'.format(d) for d in decObj ]
    ItemList += [ k[0] for k in TriggerHLTList if 'ESD' in k[1] and "TrigComposite" not in k[0] ]
    ItemList += [ k[0] for k in TriggerHLTList if 'ESD' in k[1] and "TrigComposite" in k[0] ]
    ItemList += [ 'xAOD::TrigCompositeAuxContainer#{}Aux.'.format(k[0].split("#")[1]) for k in TriggerHLTList if 'ESD' in k[1] and "TrigComposite" in k[0] ]
    ItemList += [ "xAOD::EventInfo#EventInfo" ]

    ItemList = list(set(ItemList))

else:
    ItemList = []


import AthenaPoolCnvSvc.WriteAthenaPool
from OutputStreamAthenaPool.OutputStreamAthenaPool import  createOutputStream
StreamESD=createOutputStream("StreamESD","myESD.pool.root",True)
StreamESD.ItemList = ItemList


HLTTop = findSubSequence(topSequence, "HLTTop")
