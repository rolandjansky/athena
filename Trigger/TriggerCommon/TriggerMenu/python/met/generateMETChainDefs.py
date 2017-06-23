# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

##########################################################################################
##########################################################################################
__doc__=""
__version__="Implementation of Electron Slice single electron signatures"

from AthenaCommon.Logging import logging
logging.getLogger().info("Importing %s",__name__)


from TriggerMenu.met.MissingETDef import L2EFChain_met

from TriggerJobOpts.TriggerFlags import TriggerFlags
from TriggerMenu.menu.MenuUtils import splitChainDict,mergeChainDefs



##########################################################################################
##########################################################################################



def generateChainDefs(chainDict):          

    listOfChainDicts = splitChainDict(chainDict)
    listOfChainDefs = []
    
    for subChainDict in listOfChainDicts:      
        
        MissingET = L2EFChain_met(subChainDict)
        
        listOfChainDefs += [MissingET.generateHLTChainDef()]


    if "v6" in TriggerFlags.triggerMenuSetup() or "v5" in TriggerFlags.triggerMenuSetup():       
        if len(listOfChainDefs)>1:
            listOfChainDefs = [mergeChainDefs(listOfChainDefs)]
        else:
            listOfChainDefs = [listOfChainDefs[0]]
    
    listOfChainDefs.reverse()
    
    return listOfChainDefs


##########################################################


    
