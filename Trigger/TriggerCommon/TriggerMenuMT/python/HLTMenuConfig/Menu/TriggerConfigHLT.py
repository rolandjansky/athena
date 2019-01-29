#########################################################################################
#
# TriggerConfigHLT class, providing basic functionality for assembling the menu
#
# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
#
#########################################################################################

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



