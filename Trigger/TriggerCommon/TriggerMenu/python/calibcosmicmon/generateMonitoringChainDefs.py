# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

##########################################################################################
##########################################################################################
__doc__=""
__version__="Implementation of Streaming Chain Def Generations"

from AthenaCommon.Logging import logging
logging.getLogger().info("Importing %s",__name__)
log = logging.getLogger(__name__)

from TriggerMenu.calibcosmicmon.MonitorDef import L2EFChain_Monitoring
from TriggerMenu.menu.MenuUtils import splitChainDict,mergeChainDefs

##########################################################################################
##########################################################################################

def generateChainDefs(chainDict):
    
    listOfChainDicts = splitChainDict(chainDict)
    listOfChainDefs = []

    for subChainDict in listOfChainDicts:      
        
        Monitoring = L2EFChain_Monitoring(subChainDict)
        listOfChainDefs += [Monitoring.generateHLTChainDef()]
    
    if len(listOfChainDefs)>1:
        theChainDef = mergeChainDefs(listOfChainDefs)
    else:
        theChainDef = listOfChainDefs[0]

    return theChainDef

