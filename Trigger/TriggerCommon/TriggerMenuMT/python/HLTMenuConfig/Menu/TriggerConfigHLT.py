# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

#########################################################################################
#
# TriggerConfigHLT class, providing basic functionality for assembling the menu
#
#########################################################################################

import sys

from AthenaCommon.Logging import logging
log = logging.getLogger( 'TriggerConfigHLT' )

class TriggerConfigHLT:

    sCurrentTriggerConfig = None
    def currentTriggerConfig():
        return TriggerConfigHLT.sCurrentTriggerConfig
    currentTriggerConfig = staticmethod(currentTriggerConfig)

    def __init__(self, hltfile=None):
        self.menuName = 'TestMenu'
        self.__HLTFile = hltfile
        
        self.allChainConfigs   = []
        self.allChainDicts     = []
 
        self.allThresholds     = {}
        self.allItems          = {}
        self.allChains         = {}
        self.allChainSteps     = {}

        self.theHLTChains      = []
        self.theSeqLists       = []
        self.theSeqDict        = {} # dict by Seq output TE

        TriggerConfigHLT.sCurrentTriggerConfig = self


    def registerHLTChain(self, chainName, chainConfig):
        """ Adds chain to the set of self.allChains which are registered for further use """
        if logical_name in self.allChains:
            self.allChains[chainName].append( chainConfig )
        else:
            self.allChains[chainName] = [ chainConfig ]

    def getHLTConfigFile(self):
        return self.__HLTFile


    def writeHLTConfigFile(self,smkPskName):
        """ Writes HLT config file"""
        if self.__HLTFile is None:
            return

        log.info("Writing of config files needs to be implemented")

__chainsDict = {}
def getChainDictFromChainName(chainName, allChainDicts = None):
    if __chainsDict == {}:
        __chainsDict.update( [ (c['chainName'], c) for c in allChainDicts ] )
    return __chainsDict[chainName]

##############################
# this function was supposed to be part of the class but doesn't work for now
# hope to be able to integrate it at a later stage
##############################
def getConfFromChainName(chainName, allChainDicts = None):  
    # expects something like this: 'HLT_mu8_e8'       : ['HLT_mu8','HLT_e8'],        
    chainPartNames = []
    
    # This should be not needed once all TrigUpgrade code is transferred to TriggerMenuMT
    dictToUse = []        
    if allChainDicts:
        dictToUse =  allChainDicts
    else:
        log.error("This option is not working right now, need the allChainDicts to be passed for now")
        #dictToUse = self.allChainDicts
        sys.exit("ERROR when calling getConfFromChainName, didn't pass dictionary") 
        
        
    for cDict in dictToUse:
        if chainName == cDict["chainName"]:
            for cPart in cDict["chainParts"]:
                cPName = cPart['chainPartName']
                if "HLT_" in cPName:
                    chainPartNames.append(cPName)
                else:
                    chainPartNames.append('HLT_'+cPName)
                    
    if len(chainPartNames) == 0:
        log.error("MenuChains.getConfFromChainName: Wrong chain name given: found %s",name)
        sys.exit("ERROR, in getConfFromChainName: Wrong chain configuration") 
        return [chainName]
    else:
        print "TriggerConfigHLT.getConfFromChainName: Called chain " + chainName + " and hypoTool conf "+ str(chainPartNames)
        return chainPartNames

