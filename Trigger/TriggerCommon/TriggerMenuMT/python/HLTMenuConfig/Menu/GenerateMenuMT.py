# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from TriggerJobOpts.TriggerFlags              import TriggerFlags
from TriggerJobOpts.MuonSliceFlags            import MuonSliceFlags
from TriggerJobOpts.EgammaSliceFlags          import EgammaSliceFlags
from TriggerJobOpts.JetSliceFlags             import JetSliceFlags
from TriggerJobOpts.CombinedSliceFlags        import CombinedSliceFlags
from TriggerJobOpts.TestSliceFlags            import TestSliceFlags



# Configure the scheduler
from AthenaCommon.AlgScheduler import AlgScheduler
AlgScheduler.ShowControlFlow( True )
AlgScheduler.ShowDataFlow( True )

from AthenaCommon.CFElements import parOR, seqAND, stepSeq
from AthenaCommon.AlgSequence import AlgSequence, AthSequencer
from AthenaCommon.Constants import VERBOSE,INFO,DEBUG

from TriggerMenuMT.HLTMenuConfig.Menu.TriggerConfigHLT  import TriggerConfigHLT
from TriggerMenuMT.HLTMenuConfig.Menu.HLTCFConfig import *
from TriggerMenuMT.HLTMenuConfig.Menu.MenuComponents import *
from TriggerMenuMT.HLTMenuConfig.Menu import DictFromChainName
from TriggerMenuMT.HLTMenuConfig.Menu.ChainDictTools import splitInterSignatureChainDict
from TriggerMenuMT.HLTMenuConfig.Menu.MenuPrescaleConfig import MenuPrescaleConfig

from TriggerMenuMT.HLTMenuConfig.Menu.HLTCFConfig import makeHLTTree

import os, traceback, operator, commands, time

from AthenaCommon.Logging import logging
log = logging.getLogger( 'TriggerMenuMT.HLTMenuConfig.Menu.GenerateMenuMT' )


_func_to_modify_the_menu = None
_func_to_modify_signatures = None

class GenerateMenuMT:   
    
    def __init__(self, logLevel=DEBUG):
        log.setLevel(logLevel)
        self.triggerConfigHLT = None
        self.chains = []
        self.chainDefs = []
        self.listOfErrorChainDefs = []
        self.signaturesOverwritten = False
        self.L1Prescales = None
        self.HLTPrescales = None

        # flags
        self.doEgammaChains      = True
        self.doJetChains         = True
        self.doMuonChains        = True
        self.doCombinedChains    = True
        self.doTestChains   = True

        
    def setTriggerConfigHLT(self):
        # setting the hlt menu configuration
        (HLTPrescales) = self.setupMenu()
        self.triggerConfigHLT = TriggerConfigHLT(TriggerFlags.outputHLTconfigFile())
        self.triggerConfigHLT.menuName = TriggerFlags.triggerMenuSetup()
        log.debug("Working with menu: %s", self.triggerConfigHLT.menuName)

        
    def generateChainConfig(self, chainDicts):
        """
        == Assembles the chain configuration and returns a chain object with (name, L1see and list of ChainSteps)
        """
        if self.doEgammaChains:
            try:
                import TriggerMenuMT.HLTMenuConfig.Egamma.generateElectronChainDefs                
            except:
                log.exception('Problems when importing generateElectronChainDefs, disabling Egamma chains.')
                self.doEgammaChains = False

        if self.doTestChains:
            try:
                import TriggerMenuMT.HLTMenuConfig.Egamma.generateTestChainDefs  
            except:
                log.exception('Problems when importing generateTestChainDefs, disabling Test chains.')
                self.doTestChains = False
              

        listOfChainConfigs = []
        chainDicts = splitInterSignatureChainDict(chainDicts)  
      
        if log.isEnabledFor(logging.DEBUG):
            import pprint
            pp = pprint.PrettyPrinter(indent=4, depth=8)
            log.debug('dictionary is: %s', pp.pformat(chainDicts))

        # Loop over all chainDicts and sending them off to their respective assembly code
        for chainDict in chainDicts:
            chainConfigs = None
            log.debug('Checking chainDict for chain %s' , chainDict['chainName'])
            
            if chainDict["signature"] == "Electron" and self.doEgammaChains:
                try:
                    log.debug("Try to get chain config")
                    chainConfigs = TriggerMenuMT.HLTMenuConfig.Egamma.generateElectronChainDefs.generateChainConfigs(chainDict)                    
                except:
                    log.exception( 'Problems creating ChainDef for chain\n %s ' % (chainDict['chainName']) ) 
                    continue

            elif chainDict["signature"] == "Test" and self.doTestChains:
                try:
                    chainDefs = TriggerMenuMT.HLTMenuConfig.Test.generateTestChainDefs.generateChainConfigs(chainDict)
                except:
                    log.exception('Problems creating ChainDef for chain %s ' % (chainDict['chainName']))
                    continue

            else:                
                log.error('Chain %s ignored - either trigger signature is turned off or chain dictionary cannot be read.' %(chainDict['chainName']))
                log.debug('Chain dictionary of failed chain is %s.', chainDict)
            
            log.debug('ChainConfigs  %s ' % chainConfigs)

            log.debug('type of chainconfigs %s' % (type(chainConfigs))) 
            #for chainConfig in chainConfigs:
            listOfChainConfigs.append(chainConfigs)


        if len(listOfChainConfigs) == 0:  
            log.error('No Chain Configuration found ')
            return False
        
        elif len(listOfChainConfigs)>1:
            if ("mergingStrategy" in chainDicts[0].keys()):
                log.warning("Need to define merging strategy, returning only first chain part configuration")
                theChainConfig = listOfChainConfigs[0]
            else:
                log.error("No merging strategy specified for combined chain %s" % chainDicts[0]['chainName'])
        else:
            theChainConfig = listOfChainConfigs[0]
            
        return theChainConfig



    def getChainsFromMenu(self):
        """
        == returns the list of chain names that are in the menu 
        """
        log.debug('Setting TriggerConfigHLT to get the right menu')
        self.setTriggerConfigHLT()

        log.debug('Creating one big list of of enabled signatures and chains')
        chains = []
        ## we can already use new set of flags
        #from AthenaConfiguration.AllConfigFlags import ConfigFlags
        #from TriggerMenuMT.HLTMenuConfig.Menu.LS2_v1_newJO import setupMenu as setupMenuFlags
        #setupMenuFlags( ConfigFlags ) 
        #ConfigFlags.lock()
        
        #if ConfigFlags.Trigger.menu.electron and self.doEgammaChains:
        if (TriggerFlags.CombinedSlice.signatures() or TriggerFlags.EgammaSlice.signatures()) and self.doEgammaChains:
            chains += TriggerFlags.EgammaSlice.signatures() 
            #chains += ConfigFlags.Trigger.menu.electron
            #log.debug("egamma chains "+str(ConfigFlags.Trigger.menu.egamma))
        else:
            self.doEgammaChains   = False

        if len(chains) == 0:
            log.warning("There seem to be no chains in the menu - please check")
        else:
            log.debug("The following chains were found in the menu %s", chains)
            
        return chains 
                                


    def generateL1Topo(self):
        """
        == Generates the L1Topo menu
        """
        if not TriggerFlags.readL1TopoConfigFromXML() and not TriggerFlags.readMenuFromTriggerDb():
            log.info('Generating L1 topo configuration for %s' % TriggerFlags.triggerMenuSetup())
            from TriggerMenuMT.LVL1MenuConfig.TriggerConfigL1Topo import TriggerConfigL1Topo
            self.trigConfL1Topo = TriggerConfigL1Topo( outputFile = TriggerFlags.outputL1TopoConfigFile() )
            # build the menu structure
            self.trigConfL1Topo.generateMenu()        
            log.info('Topo Menu has %i trigger lines' % len(self.trigConfL1Topo.menu) )
            # write xml file
            self.trigConfL1Topo.writeXML()
        elif TriggerFlags.readLVL1configFromXML():
            log.info("ReadingL1TopocofnigFromXML currently not implemented")
        else:
            log.info("Doing nothing with L1Topo menu configuration...")

    def generateLVL1(self):
        """
        == Generates the LVL1 menu
        """
        if not TriggerFlags.readLVL1configFromXML() and not TriggerFlags.readMenuFromTriggerDb():
            log.info('Generating L1 configuration for %s' % TriggerFlags.triggerMenuSetup() )
            from TriggerMenuMT.LVL1MenuConfig.TriggerConfigLVL1 import TriggerConfigLVL1
            self.trigConfL1 = TriggerConfigLVL1( outputFile = TriggerFlags.outputLVL1configFile())
            # build the menu structure
            self.trigConfL1.generateMenu()        
            log.info('Menu has %i items' % len(self.trigConfL1.menu.items) )
            # write xml file
            self.trigConfL1.writeXML()
        elif TriggerFlags.readLVL1configFromXML():
            log.info("ReadingLVL1cofnigFromXML currently not implemented")
        else:
            log.info("Doing nothing with L1 menu configuration...")
                       


    def setupMenu(self):
        # go over the slices and put together big list of signatures requested
        #(L1Prescales, HLTPrescales, streamConfig) = MenuPrescaleConfig(self.triggerPythonConfig)
        # that does not seem to work
        (self.L1Prescales, self.HLTPrescales) = MenuPrescaleConfig(self.triggerConfigHLT)
        return (self.HLTPrescales)
        #pass



    def generateAllChainConfigs(self):

        # get all chain names from menu 
        log.debug ("getting chains from Menu")
        chainsInMenu = self.getChainsFromMenu()

        # decoding of the chain name
        decodeChainName = DictFromChainName.DictFromChainName()
        
        chainCounter = 0
        for chain in chainsInMenu:
            log.debug("Currently processing chain: %s ", chain) 
            chainDict = decodeChainName.getChainDict(chain)
            self.triggerConfigHLT.allChainDicts.append(chainDict)

            chainCounter += 1
            chainDict['chainCounter'] = chainCounter

            log.debug("Next: getting chain configuration for chain %s ", chain) 
            chainConfig= self.generateChainConfig(chainDict)

            log.debug("Finished with retrieving chain configuration for chain %s", chain) 
            self.triggerConfigHLT.allChainConfigs.append(chainConfig)
            
        return self.triggerConfigHLT.allChainConfigs


    #----------------------#
    # Menu generation #
    #----------------------#
    def generateMT(self):
        """
        == Generates L1, L1Topo and HLT menu
        """
        log.info('GenerateMenuMT.py:generateMT ')
        
        # --------------------------------------------------------------------
        # L1 menu generation 
        # - from the code, from DB and from xmls (if we want to maintain this)
        # currently implementing the generation from configuration code
        # --------------------------------------------------------------------
        #generateL1Topo()
        #generateLVL1()

        
        # --------------------------------------------------------------------
        # HLT menu generation 
        # --------------------------------------------------------------------
        finalListOfChainConfigs = self.generateAllChainConfigs()
        log.debug("Length of FinalListofChainConfigs %s", len(finalListOfChainConfigs))

        log.debug("finalListOfChainConfig %s", finalListOfChainConfigs)
        for cc in finalListOfChainConfigs:
            log.debug('ChainName %s', cc.name)
            log.debug('  L1Seed %s', cc.seed)
            log.debug('  ChainSteps %s', cc.steps)
            for step in cc.steps:
                print step

        makeHLTTree(finalListOfChainConfigs, self.triggerConfigHLT)
        # the return values used for debugging, might be removed later
        return finalListOfChainConfigs
            
