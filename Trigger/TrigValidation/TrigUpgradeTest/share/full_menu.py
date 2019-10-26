#
#  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#

##################################################################
# Pre-setup
##################################################################
from AthenaCommon.Logging import logging
__log = logging.getLogger('full_menu')

# Set the menu name
from TriggerJobOpts.TriggerFlags import TriggerFlags
TriggerFlags.triggerMenuSetup = "LS2_v1"

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
if opt.reverseViews:
    from TriggerJobOpts.TriggerConfig import collectViewMakers
    viewMakers = collectViewMakers( topSequence )
    for alg in viewMakers:
        alg.ReverseViewsDebug = True

##########################################
# Configure trigger output using parts of the NewJO configuration
# in a somewhat hacky way
##########################################
from TriggerJobOpts.TriggerConfig import collectHypos, collectFilters, collectDecisionObjects, triggerOutputCfg
from AthenaCommon.CFElements import findAlgorithm,findSubSequence
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

# find DecisionSummaryMakerAlg
summaryMakerAlg = findAlgorithm(topSequence, "DecisionSummaryMakerAlg")
if not summaryMakerAlg:
    __log.warning("Failed to find DecisionSummaryMakerAlg")

from AthenaConfiguration.AllConfigFlags import ConfigFlags
from AthenaCommon.Configurable import Configurable
Configurable.configurableRun3Behavior+=1
acc, edmSet = triggerOutputCfg(ConfigFlags, decObj, summaryMakerAlg)
Configurable.configurableRun3Behavior-=1
acc.appendToGlobals()

##########################################
# Print top sequence for debugging
##########################################
from AthenaCommon.AlgSequence import dumpSequence, AthSequencer
dumpSequence(topSequence)
