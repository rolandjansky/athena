# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

##########################################################################################
##########################################################################################
__doc__=""
__version__="Implementation of Test Slice signatures"

from AthenaCommon.Logging import logging
logging.getLogger().info("Importing %s",__name__)


from TriggerMenu.test.TestDef import L2EFChain_test as L2EFChain_test

from TriggerMenu.menu.MenuUtils import splitChainDict
TestChains = []

##########################################################################################
##########################################################################################

def generateChainDefs(chainDict):
   
    listOfChainDicts = splitChainDict(chainDict)

    subChainDict = listOfChainDicts[0] # take only the first
    TestChain = L2EFChain_test(subChainDict)
            
    theChainDef = TestChain.generateHLTChainDef()

    return theChainDef


