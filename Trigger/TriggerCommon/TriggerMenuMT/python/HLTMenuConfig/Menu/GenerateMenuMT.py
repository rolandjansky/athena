# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

# Configure the scheduler
from AthenaCommon.AlgScheduler import AlgScheduler
AlgScheduler.ShowControlFlow( True )
AlgScheduler.ShowDataFlow( True )

from TriggerJobOpts.TriggerFlags              import TriggerFlags

from TriggerMenuMT.HLTMenuConfig.Menu.TriggerConfigHLT  import TriggerConfigHLT
from TriggerMenuMT.HLTMenuConfig.Menu.HLTCFConfig import makeHLTTree
from TriggerMenuMT.HLTMenuConfig.Menu.DictFromChainName import dictFromChainName
from TriggerMenuMT.HLTMenuConfig.Menu.ChainDictTools import splitInterSignatureChainDict
from TriggerMenuMT.HLTMenuConfig.Menu.MenuPrescaleConfig import MenuPrescaleConfig
from TriggerMenuMT.HLTMenuConfig.Menu.ChainMerging import mergeChainDefs


from AthenaCommon.Logging import logging
log = logging.getLogger( 'TriggerMenuMT.HLTMenuConfig.Menu.GenerateMenuMT' )

_func_to_modify_signatures = None

class GenerateMenuMT(object):

    def overwriteSignaturesWith(f):
        """
        == Function to generate menu for certain signatures only
        """
        log.info('In overwriteSignaturesWith ')
        global _func_to_modify_signatures
        if _func_to_modify_signatures is not None:
            log.warning('Updating the function to modify signatures from %s to %s',
                        _func_to_modify_signatures.__name__, f.__name__)
        _func_to_modify_signatures = f
    overwriteSignaturesWith = staticmethod(overwriteSignaturesWith)
            
    
    def __init__(self):
        self.chains = []
        self.chainDefs = []
        self.listOfErrorChainDefs = []
        self.signaturesOverwritten = False
        self.L1Prescales = None
        self.HLTPrescales = None

        self.availableSignatures = []
        self.signaturesToGenerate = []
        self.allSignatures = ['Egamma', 'Muon', 'Jet', 'Bjet', 'Bphysics', 'MET', 'Tau', 
                              'HeavyIon', 'Beamspot', 'Cosmic', 'EnhancedBias', 
                              'Monitor', 'Calib', 'Streaming', 'Combined'] #, AFP
        self.calibCosmicMonSigs = ['Streaming','Monitor'] #others not implemented yet ['Beamspot', 'Cosmic', 'EnhancedBias', 'Monitor', 'Calib', 'Streaming']

        # flags
        self.doEgammaChains         = True
        self.doJetChains            = True
        self.doBjetChains           = True
        self.doMuonChains           = True
        self.doBphysicsChains       = True
        self.doMETChains            = True
        self.doTauChains            = True
        self.doAFPChains            = True
        self.doMinBiasChains        = True
        self.doHeavyIonChains       = True
        self.doCosmicChains         = True
        self.doCalibrationChains    = True
        self.doStreamingChains      = True
        self.doMonitorChains        = True
        self.doBeamspotChains       = True
        self.doEnhancedBiasChains   = True
        self.doTestChains           = True
        self.doCombinedChains       = True

        
    def setTriggerConfigHLT(self):
        """
        == Setup of TriggerConfigHLT, menu and prescale names
        """
        (HLTPrescales) = self.setupMenu()
        log.debug("   and prescales : %s", HLTPrescales)
        

    def setupMenu(self):
        """
        == Setup of menu in terms of prescales and overwrite function
        """
        # go over the slices and put together big list of signatures requested
        #(L1Prescales, HLTPrescales, streamConfig) = MenuPrescaleConfig(self.triggerPythonConfig)
        # that does not seem to work
        (self.L1Prescales, self.HLTPrescales) = MenuPrescaleConfig(TriggerConfigHLT)
        global _func_to_modify_signatures
        if _func_to_modify_signatures is not None:
            log.info('setupMenu:  Modifying trigger signatures in TriggerFlags with %s',
                     _func_to_modify_signatures.__name__)
            _func_to_modify_signatures()
            self.signaturesOverwritten = True
        else:
            log.debug("No signature overwrite flags are set")

        return (self.HLTPrescales)


    def generateL1Topo(self):
        """
        == Generates the L1Topo menu
        """
        if not TriggerFlags.readL1TopoConfigFromXML() and not TriggerFlags.readMenuFromTriggerDb():
            log.info('Generating L1 topo configuration for %s', TriggerFlags.triggerMenuSetup())
            from TriggerMenuMT.LVL1MenuConfig.TriggerConfigL1Topo import TriggerConfigL1Topo
            self.trigConfL1Topo = TriggerConfigL1Topo( outputFile = TriggerFlags.outputL1TopoConfigFile() )
            # build the menu structure
            self.trigConfL1Topo.generateMenu()        
            log.debug('Topo Menu has %i trigger lines', len(self.trigConfL1Topo.menu))
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
            log.info('Generating L1 configuration for %s', TriggerFlags.triggerMenuSetup() )
            from TriggerMenuMT.LVL1MenuConfig.TriggerConfigLVL1 import TriggerConfigLVL1
            self.trigConfL1 = TriggerConfigLVL1( outputFile = TriggerFlags.outputLVL1configFile())
            # build the menu structure
            self.trigConfL1.generateMenu()        
            log.debug('Menu has %i items', len(self.trigConfL1.menu.items) )
            # write xml file
            self.trigConfL1.writeXML()
        elif TriggerFlags.readLVL1configFromXML():
            log.info("ReadingLVL1cofnigFromXML currently not implemented")
        else:
            log.info("Doing nothing with L1 menu configuration...")
                       

    def generateAllChainConfigs(self):
        """
        == Obtains chain configs for all chains in menu
        """
        
        # get all chain names from menu 
        log.debug ("getting chains from Menu")
        chainsInMenu = self.getChainsFromMenu()
        
        # decoding of the chain name
        chainCounter = 0

        for chain in chainsInMenu:
            log.debug("Currently processing chain: %s ", chain) 
            chainDict = dictFromChainName(chain)

            chainCounter += 1
            chainDict['chainCounter'] = chainCounter

            log.debug("Next: getting chain configuration for chain %s ", chain) 
            chainConfig= self.generateChainConfig(chainDict)

            log.debug("Finished with retrieving chain configuration for chain %s", chain) 
            TriggerConfigHLT.registerChain( chainDict, chainConfig )


        return TriggerConfigHLT.configsList()

    def getChainsFromMenu(self):
        """
        == Returns the list of chain names that are in the menu 
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

        for sig in self.allSignatures:  
            if eval('TriggerFlags.' + sig + 'Slice.signatures()') and eval('self.do' + sig + 'Chains'):
                log.debug("Adding %s chains to the list of chains to be configured", sig)
                chains+= eval('TriggerFlags.' + sig + 'Slice.signatures()')
                self.signaturesToGenerate.append(sig)
                log.debug('Signatures to generate %s', sig)
            else:
                log.debug('Signature %s is not switched on (no chains in menu or disabled by flag)', sig)
        
        log.info("The following signature(s) is (are) enabled: %s", self.signaturesToGenerate)

        if len(chains) == 0:
            log.warning("There seem to be no chains in the menu - please check")
        else:
            log.debug("The following chains were found in the menu %s", chains)
            
        return chains 
                                

    def generateChainConfig(self, mainChainDict):
        """
        # Assembles the chain configuration and returns a chain object with (name, L1see and list of ChainSteps)
        """
        # check if all the signature files can be imported files can be imported
        for sig in self.signaturesToGenerate:
            log.debug("current sig %s", sig)
            try:
                if eval('self.do' + sig + 'Chains'):
                    if sig == 'Egamma':
                        sigFolder = sig
                        subSigs = ['Electron', 'Photon']
                    elif sig in self.calibCosmicMonSigs:
                        sigFolder = 'CalibCosmicMon'
                        subSigs = self.calibCosmicMonSigs
                    else:
                        sigFolder = sig
                        subSigs = [sig]
                    for ss in subSigs: 
                        if sigFolder == 'Combined':
                            continue
                        else:
                            exec('import TriggerMenuMT.HLTMenuConfig.' + sigFolder + '.Generate' + ss + 'ChainDefs')                
                            if ss not in self.availableSignatures:
                                self.availableSignatures.append(ss)                        

            except ImportError:
                log.exception('Problems when importing ChainDef generating code for %s', sig)

        log.info('Available signature(s) for chain generation: %s', self.availableSignatures)

        import pprint
        pp = pprint.PrettyPrinter(indent=4, depth=8)



        # split the the chainDictionaries for each chain and print them in a pretty way
        chainDicts = splitInterSignatureChainDict(mainChainDict) 

        if log.isEnabledFor(logging.DEBUG):
            import pprint
            pp = pprint.PrettyPrinter(indent=4, depth=8)
            log.debug('dictionary is: %s', pp.pformat(chainDicts))

        # Loop over all chainDicts and send them off to their respective assembly code
        listOfChainConfigs = []

        for chainDict in chainDicts:
            chainConfigs = None
            currentSig = chainDict['signature']
            chainName = chainDict['chainName']
            log.debug('Checking chainDict for chain %s' , currentSig)

            sigFolder = ''
            if currentSig == 'Electron' or currentSig == 'Photon':
                sigFolder = 'Egamma'
            elif currentSig in self.calibCosmicMonSigs:
                sigFolder = 'CalibCosmicMon'
            else:
                sigFolder = currentSig

            if currentSig in self.availableSignatures and currentSig != 'Combined':
                try:                    
                    log.debug("Trying to get chain config for %s", currentSig)
                    functionToCall ='TriggerMenuMT.HLTMenuConfig.' + sigFolder + '.Generate' + currentSig + 'ChainDefs.generateChainConfigs(chainDict)' 
                    chainConfigs = eval(functionToCall)
                except RuntimeError:
                    log.exception( 'Problems creating ChainDef for chain\n %s ', chainName)
                    continue
            else:                
                log.error('Chain %s ignored - Signature not available', chainDict['chainName'])
            
            log.debug('ChainConfigs  %s ', chainConfigs)
            listOfChainConfigs.append(chainConfigs)

        if log.isEnabledFor(logging.DEBUG):
            import pprint
            pp = pprint.PrettyPrinter(indent=4, depth=8)
            log.debug('mainChainDict dictionary: %s', pp.pformat(mainChainDict))


        # This part is to deal with combined chains between different signatures
        if len(listOfChainConfigs) == 0:  
            log.error('No Chain Configuration found ')
            return False        

        elif len(listOfChainConfigs)>1:
                log.debug("Merging strategy from dictionary: %s", mainChainDict["mergingStrategy"])
                theChainConfig = mergeChainDefs(listOfChainConfigs, mainChainDict, mainChainDict["mergingStrategy"], mainChainDict["mergingOffset"])

                # This needs to be added for topological chains - needs implementation
                #doTopo = self.CheckIntraSignatureTopo(chainDicts) and chainDict["topo"]
                #if doTopo:
                # theChainDef = TriggerMenu.combined.generateCombinedChainDefs._addTopoInfo(theChainDef,chainDicts,listOfChainDefs)

        else:
            theChainConfig = listOfChainConfigs[0]
        
        # Configure event building strategy
        eventBuildType = mainChainDict['eventBuildType']
        if eventBuildType:
            log.debug('Configuring event building sequence %s for chain %s', eventBuildType, mainChainDict['chainName'])
            from TriggerMenuMT.HLTMenuConfig.CommonSequences.EventBuildingSequenceSetup import addEventBuildingSequence
            addEventBuildingSequence(theChainConfig, eventBuildType)

        return theChainConfig


    def generateMT(self):
        """
        == Main function of the class which generates L1, L1Topo and HLT menu
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
            log.debug('  ChainSteps %s', cc.steps)
            for step in cc.steps:
                log.debug(step)

        makeHLTTree(newJO=False, triggerConfigHLT = TriggerConfigHLT)
        # the return values used for debugging, might be removed later

        from TriggerMenuMT.HLTMenuConfig.Menu.HLTMenuJSON import generateJSON
        generateJSON()

        return finalListOfChainConfigs
            
