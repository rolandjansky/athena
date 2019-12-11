#
#  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#

##################################################################
# Pre-setup
##################################################################
from AthenaCommon.Logging import logging
__log = logging.getLogger('full_menu')

##################################################################
# Include the common configuration
##################################################################
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
# Debugging for view cross-dependencies
##########################################
if opt.reverseViews or opt.filterViews:
    from TriggerJobOpts.TriggerConfig import collectViewMakers
    viewMakers = collectViewMakers( topSequence )
    theFilter = []
    if opt.filterViews:
        theFilter = [ "Cache", "EventInfo", "HLT_EFHistoPrmVtx" ]
    for alg in viewMakers:
        alg.ReverseViewsDebug = opt.reverseViews
        alg.FallThroughFilter = theFilter

##########################################
# Configure trigger output using parts of the NewJO configuration
# in a somewhat hacky way
##########################################
from TriggerJobOpts.TriggerConfig import collectHypos, collectFilters, collectDecisionObjects, triggerOutputCfg, collectHypoDecisionObjects
from AthenaCommon.CFElements import findAlgorithm,findSubSequence
hypos = collectHypos(findSubSequence(topSequence, "HLTAllSteps"))
filters = collectFilters(findSubSequence(topSequence, "HLTAllSteps"))

# find DecisionSummaryMakerAlg
summaryMakerAlg = findAlgorithm(topSequence, "DecisionSummaryMakerAlg")
if not summaryMakerAlg:
    __log.warning("Failed to find DecisionSummaryMakerAlg")

# try to find L1Decoder
l1decoder = findAlgorithm(topSequence,'L1Decoder')
if not l1decoder:
    l1decoder = findAlgorithm(topSequence,'L1EmulationTest')
if l1decoder and summaryMakerAlg:
    decObj = collectDecisionObjects( hypos, filters, l1decoder, summaryMakerAlg )
    decObjHypoOut = collectHypoDecisionObjects(hypos, inputs=False, outputs=True)
    __log.debug("Decision Objects to write to output [hack method - should be replaced with triggerRunCfg()]")
    __log.debug(decObj)
else:
    __log.warning("Failed to find L1Decoder or DecisionSummaryMakerAlg, cannot determine Decision names for output configuration")
    decObj = []
    decObjHypoOut = []


from AthenaConfiguration.AllConfigFlags import ConfigFlags
from AthenaCommon.Configurable import Configurable
Configurable.configurableRun3Behavior+=1
acc, edmSet = triggerOutputCfg(ConfigFlags, decObj, decObjHypoOut, summaryMakerAlg)
Configurable.configurableRun3Behavior-=1
acc.appendToGlobals()

##########################################
# Print top sequence for debugging
##########################################
from AthenaCommon.AlgSequence import dumpSequence, AthSequencer
dumpSequence(topSequence)
