# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

##########################################################################################
##########################################################################################
__doc__=""
__version__="Implementation of min bias signatures"

from AthenaCommon.Logging import logging
logging.getLogger().info("Importing %s",__name__)


from TriggerMenu.minbias.MinBiasDef import L2EFChain_MB
from TriggerMenu.menu.MenuUtils import splitChainDict,mergeChainDefs


MinBias = []

##########################################################################################
##########################################################################################


def generateChainDefs(chainDict):          

    listOfChainDicts = splitChainDict(chainDict)
    listOfChainDefs = []
    
    for subChainDict in listOfChainDicts:      
        MinBias = L2EFChain_MB(subChainDict)
        
        listOfChainDefs += [MinBias.generateHLTChainDef()]
        
    if len(listOfChainDefs)>1:
        theChainDef = mergeChainDefs(listOfChainDefs)
    else:
        theChainDef = listOfChainDefs[0]        
    
    return theChainDef
