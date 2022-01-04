#
#  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
#

## This file runs runHLT_standalone with external menus
# 3 tests of the CF to cover different menu generation frameworks and data inputs:

#   - menuManual: chains are generated manually in generateCFChains, and run on input data file
#   - emuMenuTest: chains are generated in the menu framework as HLT_TestChain**, and run on emulated data 
#   - emuManual: chains are genrated manually and run on emulated data

class testopt:
    menuType  = 'menuManual' # use either menu or manual chain building

from AthenaCommon.Logging import logging
log = logging.getLogger('test_menu_cf')


log.info('Setup options:')
defaultOptions = [a for a in dir(testopt) if not a.startswith('__')]
for option in defaultOptions:
    if option in globals():
        setattr(testopt, option, globals()[option])
        log.info(' %20s = %s' , option, getattr(testopt, option))
    else:        
        log.info(' %20s = (Default) %s' , option, getattr(testopt, option))



from AthenaConfiguration.AllConfigFlags import ConfigFlags
ConfigFlags.Trigger.generateMenuDiagnostics = True
ConfigFlags.Trigger.triggerMenuSetup = "Dev_pp_run3_v1"

createHLTMenuExternally=True # menu will be build up explicitly here 
doWriteRDOTrigger = False
doWriteBS = False
forceEnableAllChains=True

# load all configuration as the real HLT
include("TriggerJobOpts/runHLT_standalone.py")


# make menu manually here:
from TriggerMenuMT.HLTMenuConfig.Menu.HLTCFConfig import makeHLTTree
from TriggerMenuMT.HLTMenuConfig.Menu.TriggerConfigHLT import TriggerConfigHLT
from TriggerMenuMT.CFtest.generateCFChains import generateCFChains
from TriggerMenuMT.CFtest.EmuStepProcessingConfig import generateHLTSeedingAndChainsManually, generateHLTSeedingAndChainsByMenu

topSequence = AlgSequence()

if testopt.menuType == 'menuManual':
    generateCFChains(opt)
    from TriggerMenuMT.HLTMenuConfig.Menu.CheckL1HLTConsistency import checkL1HLTConsistency
    checkL1HLTConsistency()
elif testopt.menuType == 'emuMenuTest':
    # HLT_TestChain
    generateHLTSeedingAndChainsByMenu(topSequence)
elif testopt.menuType == 'emuManual':
    generateHLTSeedingAndChainsManually(topSequence)
else:
    log.error("Input parameter %s not accepted",testopt.menuType)


# set DEBUG flag on the control-flow builder (before building)
import TriggerMenuMT.HLTMenuConfig.Menu.HLTCFConfig
TriggerMenuMT.HLTMenuConfig.Menu.HLTCFConfig.log.setLevel(DEBUG)

# from here generate the ControlFlow and the Dataflow
# doing the same as menu.generateMT()
makeHLTTree( triggerConfigHLT=TriggerConfigHLT )

       
from TriggerMenuMT.HLTMenuConfig.Menu.HLTMenuJSON import generateJSON
generateJSON()

from TriggerMenuMT.HLTMenuConfig.Menu.HLTPrescaleJSON import generateJSON as generatePrescaleJSON
generatePrescaleJSON()
   
from TriggerMenuMT.HLTMenuConfig.Menu.HLTMonitoringJSON import generateDefaultMonitoringJSON
generateDefaultMonitoringJSON()

# now some debug
print ("EmuStepProcessing: dump top Sequence after CF/DF Tree build")
from AthenaCommon.AlgSequence import dumpSequence
dumpSequence( topSequence )

from TriggerJobOpts.TriggerConfig import collectHypos, collectFilters
from AthenaCommon.CFElements import findSubSequence
hypos   = collectHypos(findSubSequence(topSequence, "HLTAllSteps"))
filters = collectFilters(findSubSequence(topSequence, "HLTAllSteps"))

nfilters = sum(len(v) for v in filters.values())
nhypos = sum(len(v) for v in hypos.values())
log.info( "Algorithms counting: Number of Filter algorithms: %d  -  Number of Hypo algorithms: %d", nfilters , nhypos) 


# switch on DEBUG on the trigger monitoring
topSequence.HLTTop.HLTEndSeq.TrigSignatureMoni.OutputLevel = DEBUG

from AthenaCommon.CFElements import getSequenceChildren, isSequence
for alg in getSequenceChildren( topSequence.HLTTop.HLTAllSteps ):
         if isSequence( alg ):
             continue
 
         if "TriggerSummary" in alg.getName():
             alg.OutputLevel = DEBUG

