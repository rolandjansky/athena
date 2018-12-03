# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

##########################################################################################
##########################################################################################
__doc__=""
__version__="Implementation of Calibration Chain Def generation"

from AthenaCommon.Logging import logging
logging.getLogger().info("Importing %s",__name__)
mlog = logging.getLogger("TriggerMenu.calibcosmic.generateCalibChainDefs")

from TriggerMenu.calibcosmicmon.CalibDef import L2EFChain_CalibTemplate
from TriggerMenu.menu.MenuUtils import splitChainDict,mergeChainDefs


##########################################################################################
##########################################################################################

def generateChainDefs(chainDict):
    
    listOfChainDicts = splitChainDict(chainDict)
    listOfChainDefs = []

    for subChainDict in listOfChainDicts:      
        
        Calib = L2EFChain_CalibTemplate(subChainDict)

        listOfChainDefs += [Calib.generateHLTChainDef()]
    
    if len(listOfChainDefs)>1:
        theChainDef = mergeChainDefs(listOfChainDefs)
    else:
        theChainDef = listOfChainDefs[0]

    return theChainDef

