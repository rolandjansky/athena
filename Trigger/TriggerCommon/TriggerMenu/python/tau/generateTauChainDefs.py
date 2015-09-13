# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

##########################################################################################
##########################################################################################
__doc__=""
__version__="Implementation of Tau Slice"

from AthenaCommon.Logging import logging
logging.getLogger().info("Importing %s",__name__)


from TriggerMenu.tau.TauDef import L2EFChain_tau as L2EFChain_tau
try:
    from TriggerMenu.tau.TauDefIdTest import L2EFChain_tau as L2EFChain_tau_IdTest
except:
    log.error('generateTauChainDefs: Problems when importing TauDefIdTest.')
    log.info(traceback.print_exc())

from TriggerJobOpts.TriggerFlags import TriggerFlags
from TriggerMenu.menu.MenuUtils import *

# Try new HypoProvider
from TriggerMenu.tau.TauHypoProvider import TauHypoProvider


Taus = []

##########################################################################################
##########################################################################################



def generateChainDefs(chainDict):          

    chainParts = chainDict['chainParts']
    
    listOfChainDicts = splitChainDict(chainDict)
    listOfChainDefs = []

    myProvider = TauHypoProvider()

    

    for subChainDict in listOfChainDicts:
        if "IdTest" in subChainDict["chainParts"]["addInfo"]:
            Tau = L2EFChain_tau_IdTest(subChainDict, myProvider)
        else:
            Tau = L2EFChain_tau(subChainDict, myProvider)
 
        listOfChainDefs += [Tau.generateHLTChainDef()]

    if len(listOfChainDefs)>1:
        theChainDef = mergeChainDefs(listOfChainDefs)
    else:
        theChainDef = listOfChainDefs[0]
        
    return theChainDef
