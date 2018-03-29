# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

##########################################################################################
##########################################################################################
__doc__=""
__version__="Implementation of heavy ion signatures"

from AthenaCommon.Logging import logging
logging.getLogger().info("Importing %s",__name__)


from TriggerMenu.heavyion.HeavyIonDef import L2EFChain_HI
from TriggerMenu.menu.MenuUtils import splitChainDict,mergeChainDefs


HeavyIon = []

##########################################################################################
##########################################################################################


def generateChainDefs(chainDict):

    print 'generateHeavyIonChainDefs, input dict', chainDict

    listOfChainDicts = splitChainDict(chainDict)
    listOfChainDefs = []
    
    for subChainDict in listOfChainDicts:      
        HeavyIon = L2EFChain_HI(subChainDict)
        
        listOfChainDefs += [HeavyIon.generateHLTChainDef()]
        
    if len(listOfChainDefs)>1:
        theChainDef = mergeChainDefs(listOfChainDefs)
    else:
        theChainDef = listOfChainDefs[0]        
    
    return theChainDef
