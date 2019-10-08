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
    __log.debug("Decision Objects to write to output [hack method - should be replaced with triggerRunCfg()]")
    __log.debug(decObj)
else:
    __log.warning("Failed to find L1Decoder, cannot determine Decision names for output configuration")
    decObj = []

# Get the EDM list
from TrigEDMConfig.TriggerEDM import getTriggerEDMList
from TrigEDMConfig.TriggerEDMRun3 import persistent
TriggerFlags.EDMDecodingVersion = 3
TriggerFlags.ESDEDMSet = 'BS' if configureBSResult else 'ESD'
edmList = getTriggerEDMList(TriggerFlags.ESDEDMSet(), TriggerFlags.EDMDecodingVersion())

# Build the output ItemList
ItemList = []
for edmType, edmKeys in edmList.iteritems():
    edmType = persistent(edmType) if configureBSResult else edmType
    for key in edmKeys:
        ItemList.append(edmType+'#'+key)

if not configureBSResult:
    # Do not serialise EventInfo into BS, as it is already represented by the BS event header
    ItemList.append("xAOD::EventInfo#EventInfo")
    ItemList.append("xAOD::EventAuxInfo#EventInfoAux.")

for item in decObj:
    typeName = 'xAOD::TrigCompositeContainer#{}'.format(item)
    typeNameAux = 'xAOD::TrigCompositeAuxContainer#{}Aux.'.format(item)
    ItemList.append(typeName)
    # Temporary workaround for the case when item from decObj is explicitly in TriggerEDM list
    # to avoid adding it twice - with and without dynamic variables
    if typeNameAux+'-' not in ItemList:
        ItemList.append(typeNameAux)

ItemList = list(set(ItemList))

# Configure ESD writing
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
    from TrigOutputHandling.TrigOutputHandlingConf import StreamTagMakerTool, TriggerBitsMakerTool

    # Tool serialising EDM objects to fill the HLT result
    serialiser = TriggerEDMSerialiserToolCfg('Serialiser')
    for item in ItemList:
        __log.debug('adding to serialiser list: %s' % item)
        serialiser.addCollectionToMainResult(item)

    # Tool adding stream tags to HLT result
    stmaker = StreamTagMakerTool()

    # Map decisions producing PEBInfo from DecisionSummaryMakerAlg.FinalStepDecisions to StreamTagMakerTool.PEBDecisionKeys
    import AthenaCommon.AlgSequence as acas
    summaryMakerAlg = [s for s in acas.iter_algseq(topSequence) if s.getName() == "DecisionSummaryMakerAlg"][0]
    chainToDecisionKeyDict = summaryMakerAlg.getProperties()['FinalStepDecisions']
    stmaker.PEBDecisionKeys = []
    for chain, decisionKey in chainToDecisionKeyDict.iteritems():
        if 'PEBInfoWriter' in decisionKey:
            __log.debug('Chain %s produces decision %s with PEBInfo', chain, decisionKey)
            stmaker.PEBDecisionKeys.append(decisionKey)

    bitsmaker = TriggerBitsMakerTool()

    # Configure the HLT result maker to use the above tools
    from AthenaCommon.AppMgr import ServiceMgr as svcMgr
    hltResultMaker = svcMgr.HltEventLoopMgr.ResultMaker
    hltResultMaker.MakerTools = [stmaker, serialiser, bitsmaker]

# Debugging for view cross-dependencies
if opt.reverseViews:
    from TriggerJobOpts.TriggerConfig import collectViewMakers
    viewMakers = collectViewMakers( topSequence )
    for alg in viewMakers:
        alg.ReverseViewsDebug = True
