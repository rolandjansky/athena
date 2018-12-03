# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

##########################################################################################
##########################################################################################
__doc__=""
__version__="Implementation of AFP Slice signatures"

from AthenaCommon.Logging import logging
logging.getLogger().info("Importing %s",__name__)


from TriggerMenu.afp.AFPDef import L2EFChain_afp

#from TriggerJobOpts.TriggerFlags import TriggerFlags
from TriggerMenu.menu.MenuUtils import splitChainDict,mergeChainDefs

AFP = []

##########################################################################################
##########################################################################################



def generateChainDefs(chainDict):          

    listOfChainDicts = splitChainDict(chainDict)
    listOfChainDefs = []
    
    for subChainDict in listOfChainDicts:      
        AFP = L2EFChain_afp(subChainDict)
        
        listOfChainDefs += [AFP.generateHLTChainDef()]


        
    if len(listOfChainDefs)>1:
        listOfChainDefs = [mergeChainDefs(listOfChainDefs)]
    else:
        listOfChainDefs = [listOfChainDefs[0]]
    
    listOfChainDefs.reverse()
    
    return listOfChainDefs


##########################################################


    
