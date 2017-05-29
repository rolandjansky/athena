# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

##########################################################################################
##########################################################################################
__doc__=""
__version__="Implementation of EnhancedBias Chain Def Generations"

from AthenaCommon.Logging import logging
logging.getLogger().info("Importing %s",__name__)
mlog = logging.getLogger("TriggerMenu.calibcosmic.generateEnhancedBiasChainDefs")

from TriggerMenu.calibcosmicmon.EnhancedBiasDefs import L2EFChain_EnhancedBiasTemplate
from TriggerMenu.menu.MenuUtils import splitChainDict,mergeChainDefs
##########################################################################################
##########################################################################################

def generateChainDefs(chainDict):
    
    listOfChainDicts = splitChainDict(chainDict)
    listOfChainDefs = []

    for subChainDict in listOfChainDicts:      
        
        EnhancedBias = L2EFChain_EnhancedBiasTemplate(subChainDict)

        listOfChainDefs += [EnhancedBias.generateHLTChainDef()]
    
    if len(listOfChainDefs)>1:
        theChainDef = mergeChainDefs(listOfChainDefs)
    else:
        theChainDef = listOfChainDefs[0]

    return theChainDef

