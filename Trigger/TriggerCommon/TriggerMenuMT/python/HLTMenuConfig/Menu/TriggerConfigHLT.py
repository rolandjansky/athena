# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

#########################################################################################
#
# TriggerConfigHLT class, providing basic functionality for assembling the menu
#
#########################################################################################

import sys
from six import itervalues
from AthenaCommon.Logging import logging
log = logging.getLogger(__name__)

class TriggerConfigHLT(object):
    """
    Repository of all configured chains. Contains chain dictionaries and Menu Chain objects
    """
    from collections import OrderedDict
    __allChainConfigs   = OrderedDict()
    __allChainDicts     = OrderedDict()

    @classmethod
    def registerChain(cls, chainDict, chainConfig):
        """ 
        Register chain for future use
        
        Name reuse and inconsistency is reported
        """
        assert chainDict['chainName'] not in cls.__allChainDicts, 'Chain dictionary {} already registered'.format(chainDict['chainName'])
        assert chainConfig.name not in cls.__allChainConfigs, 'Chain configuration {} already registered'.format(chainConfig.name)
        assert chainDict['chainName'] == chainConfig.name, 'Registering chain dictionary and config that have differnet names: in dictionary {}, in config {}'.format(chainDict['chainName'], chainConfig.name)

        cls.__allChainConfigs[chainConfig.name] = chainConfig
        cls.__allChainDicts[chainDict['chainName']] = chainDict
        log.debug("Registered chain %s", chainConfig.name ) 

    @classmethod
    def isChainRegistered(cls, chainName):
        if chainName in cls.__allChainDicts:
            return True
        return False

    @classmethod
    def dicts(cls):
        return cls.__allChainDicts

    @classmethod
    def configs(cls):
        return cls.__allChainConfigs

    @classmethod
    def dictsList(cls):

        return list(itervalues(cls.__allChainDicts))

    @classmethod
    def configsList(cls):
        return list(itervalues(cls.__allChainConfigs))

    @classmethod
    def getChainDictFromChainName(cls, chainName):
        assert chainName in cls.__allChainDicts, "Chain of name {} not registered, all registered {}".format(chainName, cls.__allChainDicts.keys())
        return cls.__allChainDicts[chainName]




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
        log.error("getConfFromChainName: Wrong chain name given: found %s", chainName)
        sys.exit("ERROR, in getConfFromChainName: Wrong chain configuration") 
        return [chainName]
    else:
        log.info("getConfFromChainName: Called chain %s and hypoTool conf %s", chainName, chainPartNames)
        return chainPartNames



if __name__ == "__main__": # selftest
    log.info('Self testing')
    class ChainClassGoodForThisTest:
        def __init__(self, n):
            self.name = n

    TriggerConfigHLT.registerChain({'chainName':'HLT_bla1'}, ChainClassGoodForThisTest('HLT_bla1'))
    TriggerConfigHLT.registerChain({'chainName':'HLT_bla2'}, ChainClassGoodForThisTest('HLT_bla2'))
    TriggerConfigHLT.getChainDictFromChainName('HLT_bla1') # if missing will assert
    TriggerConfigHLT.getChainDictFromChainName('HLT_bla2') # if missing will assert
    log.info("ok, registration works")
    try:
        TriggerConfigHLT.getChainDictFromChainName('HLT_blabla')
    except Exception as e:
        if isinstance(e, AssertionError):
            log.info("ok, unregistered chain handling works")
        else:
            log.error("unhandled missing chain")
            
