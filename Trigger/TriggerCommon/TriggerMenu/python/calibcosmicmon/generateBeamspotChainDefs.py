# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

##########################################################################################
##########################################################################################
__doc__=""
__version__="Implementation of Beamspot Chain Def Generations"

from AthenaCommon.Logging import logging
logging.getLogger().info("Importing %s",__name__)
mlog = logging.getLogger("TriggerMenu.calibcosmic.generateBeamspotChainDefs")

from TriggerMenu.calibcosmicmon.BeamspotDef import L2EFChain_Beamspot
from TriggerMenu.menu.MenuUtils import splitChainDict,mergeChainDefs

##########################################################################################
##########################################################################################

def generateChainDefs(chainDict):
    
    listOfChainDicts = splitChainDict(chainDict)
    listOfChainDefs = []

    for subChainDict in listOfChainDicts:      
        
        Beamspot = L2EFChain_Beamspot(subChainDict)
        listOfChainDefs += [Beamspot.generateHLTChainDef()]
    
    if len(listOfChainDefs)>1:
        theChainDef = mergeChainDefs(listOfChainDefs)
    else:
        theChainDef = listOfChainDefs[0]

    return theChainDef

