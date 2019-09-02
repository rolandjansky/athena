#
#  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#

from AthenaCommon.Logging import logging
__log = logging.getLogger('full_menu')

from TriggerJobOpts.TriggerFlags import TriggerFlags
TriggerFlags.triggerMenuSetup = "LS2_v1"

include("TrigUpgradeTest/testHLT_MT.py")



##################################################################
# Generate the menu
##################################################################


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


##########################################
# Output configuration
##########################################

# Configure BS result only in the following three cases
# 1) Running in partition
# 2) Running with athenaHLT
# 3) Running with athena and saving BS output
import os
onlineWriteBS = os.getenv('TDAQ_PARTITION') is not None  # athenaHLT also sets this
offlineWriteBS = False # TODO: cover configuration of BS writing in athena
configureBSResult = onlineWriteBS or offlineWriteBS

if onlineWriteBS and opt.doWriteESD:
    __log.error("ESD writing configured in online mode")

from AthenaCommon.CFElements import findAlgorithm,findSubSequence
if opt.doWriteESD:
  from RecExConfig.RecFlags  import rec
  rec.doESD=True
  rec.doWriteESD=True
  import DecisionHandling
  for a in findSubSequence(topSequence, "HLTAllSteps").getChildren():
      if isinstance(a, DecisionHandling.DecisionHandlingConf.TriggerSummaryAlg):
          a.OutputLevel = DEBUG


# this part uses parts from the NewJO configuration, it is very hacky for the moment

from TriggerJobOpts.TriggerConfig import collectHypos, collectFilters, collectDecisionObjects, triggerOutputStreamCfg
hypos = collectHypos(findSubSequence(topSequence, "HLTAllSteps"))
filters = collectFilters(findSubSequence(topSequence, "HLTAllSteps"))

# try to find L1Decoder
l1decoder = findAlgorithm(topSequence,'L1Decoder')
if not l1decoder:
    l1decoder = findAlgorithm(topSequence,'L1EmulationTest')

if l1decoder:
    decObj = collectDecisionObjects( hypos, filters, l1decoder )
    __log.debug("Decision Objects to export to ESD [hack method - should be replaced with triggerRunCfg()]")
    __log.debug(decObj)

    from TrigEDMConfig.TriggerEDMRun3 import TriggerHLTListRun3
    ItemList  = [ 'xAOD::TrigCompositeContainer#{}'.format(d) for d in decObj ]
    ItemList += [ 'xAOD::TrigCompositeAuxContainer#{}Aux.'.format(d) for d in decObj ]
    ItemList += [ 'xAOD::TrigCompositeAuxContainer#{}Aux.'.format(k[0].split("#")[1]) for k in TriggerHLTListRun3 if 'ESD' in k[1] and "TrigComposite" in k[0] ]
    if not configureBSResult:
        # enable for BS when ATR-20228 is resolved:
        ItemList += [ k[0] for k in TriggerHLTListRun3 if 'ESD' in k[1] and "TrigComposite" not in k[0] ] 
        ItemList += [ k[0] for k in TriggerHLTListRun3 if 'ESD' in k[1] and "TrigComposite" in k[0] ]
        # don't enable for BS:
        ItemList += [ "xAOD::EventInfo#EventInfo", "xAOD::EventAuxInfo#EventInfoAux."]
    ItemList = list(set(ItemList))

else:
    ItemList = []


if opt.doWriteESD:
  import AthenaPoolCnvSvc.WriteAthenaPool
  from OutputStreamAthenaPool.OutputStreamAthenaPool import  createOutputStream
  StreamESD=createOutputStream("StreamESD","myESD.pool.root",True)
  StreamESD.ItemList = ItemList

##########################################
# HLT result configuration
# TODO: Move this to TriggerJobOpts and call from here
##########################################

if configureBSResult:
    from TrigOutputHandling.TrigOutputHandlingConfig import TriggerEDMSerialiserToolCfg
    from TrigOutputHandling.TrigOutputHandlingConf import StreamTagMakerTool # TODO: TriggerBitsMakerTool
    from TrigEDMConfig.TriggerEDMRun3 import persistent

    # Tool serialising EDM objects to fill the HLT result
    serialiser = TriggerEDMSerialiserToolCfg('Serialiser')
    for item in ItemList:
        split_name = item.split('#')
        split_name[0] = persistent(split_name[0])
        pers_name = '#'.join(split_name)
        __log.debug('adding to serialiser list: %s' % pers_name)
        serialiser.addCollectionToMainResult(pers_name)

    # Tool adding stream tags to HLT result
    stmaker = StreamTagMakerTool()
    stmaker.ChainDecisions = 'HLTNav_Summary'
    stmaker.HLTmenuFile = TriggerFlags.outputHLTmenuJsonFile()

    # Configure the HLT result maker to use the above tools
    from AthenaCommon.AppMgr import ServiceMgr as svcMgr
    hltResultMaker = svcMgr.HltEventLoopMgr.ResultMaker
    hltResultMaker.MakerTools = [stmaker, serialiser] # TODO: add bits maker
