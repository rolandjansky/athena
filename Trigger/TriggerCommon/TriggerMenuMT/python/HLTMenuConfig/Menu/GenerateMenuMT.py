# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from PyUtils.Decorators import memoize
from six import add_metaclass

# Configure the scheduler
from AthenaCommon.AlgScheduler import AlgScheduler
AlgScheduler.ShowControlFlow( True )
AlgScheduler.ShowDataFlow( True )

from TriggerJobOpts.TriggerFlags import TriggerFlags

from TriggerMenuMT.HLTMenuConfig.Menu.TriggerConfigHLT  import TriggerConfigHLT
from TriggerMenuMT.HLTMenuConfig.Menu.HLTCFConfig import makeHLTTree
from TriggerMenuMT.HLTMenuConfig.Menu.DictFromChainName import dictFromChainName
from TriggerMenuMT.HLTMenuConfig.Menu.ChainDictTools import splitInterSignatureChainDict
from TriggerMenuMT.HLTMenuConfig.Menu.MenuPrescaleConfig import MenuPrescaleConfig, applyHLTPrescale
from TriggerMenuMT.HLTMenuConfig.Menu.ChainMerging import mergeChainDefs
from TriggerMenuMT.HLTMenuConfig.Menu.MenuAlignmentTools import analyseCombinations, groupSignatures, setChainSignatures

from AthenaCommon.Logging import logging
log = logging.getLogger( __name__ )

_func_to_modify_signatures = None



        
        
class Singleton(type):
    _instances = {}
    def __call__(cls, *args, **kwargs):
        if cls not in cls._instances:
            cls._instances[cls] = super(Singleton, cls).__call__(*args, **kwargs)
        return cls._instances[cls]


# for now we make this a singleton because calling menu generation twice leads to problems
@add_metaclass(Singleton)
class GenerateMenuMT(object):

    @staticmethod
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


    def __init__(self):
        self.chains = []
        self.chainDefs = []
        self.listOfErrorChainDefs = []
        self.selectChainsForTesting = []
        self.signaturesOverwritten = False
        self.L1Prescales = None
        self.HLTPrescales = None

        self.availableSignatures = []
        self.signaturesToGenerate = []
        self.allSignatures = ['Egamma', 'Muon', 'Jet', 'Bjet', 'Bphysics', 'MET', 'Tau',
                              'HeavyIon', 'Beamspot', 'Cosmic', 'EnhancedBias',
                              'Monitor', 'Calib', 'Streaming', 'Combined', 'MinBias', 'Test'] #, AFP
        self.calibCosmicMonSigs = ['Streaming','Monitor','Beamspot','Cosmic'] #others not implemented yet ['Beamspot', 'Cosmic', 'EnhancedBias', 'Monitor', 'Calib', 'Streaming']

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
        self.doCombinedChains       = True
        self.doTestChains           = True


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
    
    @memoize
    def generateAllChainConfigs(self):
        """
        == Obtains chain configs for all chains in menu
        """

        # get all chain names from menu
        log.debug ("getting chains from Menu")
        chainsInMenu = self.getChainsFromMenu()

        # decoding of the chain name
        chainCounter = 0

        all_chains = []
        combinations_in_menu = []
        signatures_to_align = set()
        length_of_configs = {}
        for chain in chainsInMenu:
            log.debug("Now processing chain: %s ", chain) 
            chainDict = dictFromChainName(chain)

            chainCounter += 1
            chainDict['chainCounter'] = chainCounter

            #set default chain prescale
            chainDict['prescale'] = 1

            log.debug("Next: getting chain configuration for chain %s ", chain.name) 
            chainConfig,lengthOfChainConfigs = self.__generateChainConfig(chainDict)
            
            all_chains += [(chainDict,chainConfig,lengthOfChainConfigs)]
            
            #update the signature length dictionary if we have a longer number of steps
            #or the signature isn't registered in the dictionary yet
            for config_length, config_sig in lengthOfChainConfigs:
                if config_sig in length_of_configs:
                    if config_length > length_of_configs[config_sig]:
                        length_of_configs[config_sig] = config_length
                else:
                    length_of_configs[config_sig] = config_length

            # find the chains that contain more than one signature - what combinations do we need to deal with?
            # using sets here so we don't end up with duplicates - though in the future this may have to be revisited
            # if we split signatures into multiple types of algorithm
            if len(set(chainDict['signatures'])) > 1:  
                combinations_in_menu += [list(set(chainDict['signatures']))]
                for sig in list(set(chainDict['signatures'])):
                    signatures_to_align.update([sig])
        
        #will likely always be true, but the grouping could be redefined
        groupPeskySignatures = True
        
        if groupPeskySignatures:    
            # do some replacing of Electron --> Egamma, Photon --> Egamma
            # Jet/MET/b-jet --> JetMET
            # B-physics chains are hard-coded as muon chains, since they run muons
            #    --> any B-->ee chains would need to be set as egamma!
        
            # these modified signatures are only used for the aligning! We don't overwrite 
            # any of the signatures the chains are in.
            combinations_in_menu,signatures_to_align,length_of_configs = groupSignatures(combinations_in_menu, signatures_to_align,length_of_configs)


        #dict of signature: set it belongs to
        #e.g. {'Electron': ['Electron','Muon','Photon']}        
        signature_sets_to_align = analyseCombinations(combinations_in_menu, signatures_to_align, doGroupSignatures = groupPeskySignatures)
        
        log.debug('Aligning the following signatures with sets: %s',signature_sets_to_align)
        
        for chainDict,chainConfig,lengthOfChainConfigs in all_chains:
        
              # start by ordering electron, photon, muon by having e+mu, g+mu, e+g chains
              # desired ordering: electron, photon, muon, tau, jet, met, b-jet
              
              # lengthOfChainConfigs is something like this: [(4, 'Photon'), (5, 'Muon')]
              # needs to match up with the maximum number of steps in a signature in the menu (length_of_configs)
              # start with electron! Only need to add post-steps for combined electron chains if the max length in a combined chain
              # is greater than the number of electron steps combined chain. Assume that the max length of an electron chain occurs 
              # in a combined chain.
              
              signatures = chainDict['signatures']
              
              if groupPeskySignatures:
                signatures, lengthOfChainConfigs = setChainSignatures(signatures, lengthOfChainConfigs)
    
              #parallel-merged single-signature chains or single signature chains. Anything that needs no splitting!
              if len(set(signatures)) == 1: 
                  if signatures[0] not in signature_sets_to_align or len(signature_sets_to_align[signatures[0]]) == 1:
                      # no need to align lonely signatures
                      # the latter condition should never happen, because we only put in signatures that are 
                      # in combined chains, and thus will need *some* aligning. but good to check in any case.
                      log.debug("Finished with retrieving chain configuration for chain %s", chain.name)
                      chainConfig.numberAllSteps()
                      TriggerConfigHLT.registerChain( chainDict, chainConfig )
                      continue
                  elif signatures[0] == signature_sets_to_align[signatures[0]][0]:
                      # if it's the first chain in the set to be aligned, again - nothing to do here.
                      log.debug("Finished with retrieving chain configuration for chain %s", chain.name) 
                      chainConfig.numberAllSteps()
                      TriggerConfigHLT.registerChain( chainDict, chainConfig )
                      continue                       
                  else:
                      # now we know that empty steps are necessary before this chain. we can loop through and add accordingly
                      # but we want to do this in reverse
                      the_align_sigs = signature_sets_to_align[signatures[0]][:signature_sets_to_align[signatures[0]].index(signatures[0])]
                      the_align_sigs.reverse()
                      
                      for align_sig in the_align_sigs:
                          chainConfig.insertEmptySteps(chainDict,'Empty'+align_sig+'Align',length_of_configs[align_sig],0)         

                      log.debug("Finished with retrieving chain configuration for chain %s", chain.name)
                      chainConfig.numberAllSteps()
                      TriggerConfigHLT.registerChain( chainDict, chainConfig )
                          
              elif len(signatures) == 2:
                  #check for a few bad conditions first:
                  if(signatures[0] not in signature_sets_to_align or signatures[1] not in signature_sets_to_align):
                      log.error(" one of the signatures in %s is not available in the sets to align dictionary!", signatures)
                  elif signature_sets_to_align[signatures[0]] != signature_sets_to_align[signatures[1]]:
                      log.error(" the two signatures %s point to different sets in the sets to align dictionary. Set1: %s, set2: %s!", 
                                signatures, signature_sets_to_align[signatures[0]],signature_sets_to_align[signatures[1]])
                
                  if len(signature_sets_to_align[signatures[0]]) == 2:
                      
                      # if the pair is on its own, then we just make sure the first signature's number
                      # of steps is equal to the max in that signature (so the next signature starts at the right step)
                      
                      # not a dictionary because we could have a chain mu_jet_munoL1? Not sure. But don't want to
                      # overwrite duplicates yet. 
                      # probably, at some point, will need to divide this beyond signature but instead as unique sequence within a signature. 
                      # munoL1 is already one case...
                      length_firstsig = 0
                      max_length_firstsig = length_of_configs[signature_sets_to_align[signatures[0]][0]]
                      for config_length,config_sig in lengthOfChainConfigs:
                          if config_sig == signature_sets_to_align[signatures[0]][0]:
                              length_firstsig = config_length
                      if length_firstsig < max_length_firstsig:
                          #too short! gotta add padding steps between two signatures...
                          needed_steps = max_length_firstsig - length_firstsig
                          chainConfig.insertEmptySteps(chainDict,'Empty'+signature_sets_to_align[signatures[0]][0]+'Align',needed_steps,length_firstsig) 
                      
                      elif length_firstsig > max_length_firstsig:
                          log.error("%s first signature length %d is greater than the max calculated, %d",chainDict.name,length_firstsig, max_length_firstsig)     
                  
                      log.debug("Finished with retrieving chain configuration for chain %s", chain.name)
                      chainConfig.numberAllSteps()
                      TriggerConfigHLT.registerChain( chainDict, chainConfig )
  

                  #this should probably work for signatures > 2, but might be a few gotchas (and errors need updating)
                  if len(signature_sets_to_align[signatures[0]]) > 2:
                      if(signatures[0] not in signature_sets_to_align or signatures[1] not in signature_sets_to_align):
                          log.error(" one of the signatures in %s is not available in the sets to align dictionary!", signatures)
                      elif signature_sets_to_align[signatures[0]] != signature_sets_to_align[signatures[1]]:
                          log.error(" the two signatures %s point to different sets in the sets to align dictionary. Set1: %s, set2: %s!", 
                                     signatures, signature_sets_to_align[signatures[0]], signature_sets_to_align[signatures[1]])
                      
                      # we need to know which signatures are in the chain in which order. Assume this is always stored correctly.
                      # (this should be true)
                      
                      # never need to align the last chain - it can end a different length, no problem.
                      # ignore any signatures after the end of those in this chain

                      the_align_sigs = signature_sets_to_align[signatures[1]][:signature_sets_to_align[signatures[1]].index(signatures[1])]
                      the_align_sigs.reverse()
                      
                      for align_sig in the_align_sigs:
                          max_length_sig = length_of_configs[align_sig]
                          if align_sig in signatures:
                              length_sig = 0
                              for config_length,config_sig in lengthOfChainConfigs:
                                  if config_sig == align_sig:
                                      length_sig = config_length
                              if length_sig < max_length_sig:
                                  #too short! gotta add padding steps between two signatures...
                                  needed_steps = max_length_sig - length_sig
                                  chainConfig.insertEmptySteps(chainDict,'Empty'+align_sig+'Align',needed_steps,length_sig) 
                          else:
                              # this sig isn't in the chain, but we still will need empty steps for it
                              # always add them to the start, because we're running in reverse order
                              chainConfig.insertEmptySteps(chainDict,'Empty'+align_sig+'Align',length_of_configs[align_sig],0) 
                    
                      log.debug("Finished with retrieving chain configuration for chain %s", chain.name) 
                      chainConfig.numberAllSteps()
                      TriggerConfigHLT.registerChain( chainDict, chainConfig )               
              
              else: 
                  log.error("Menu can't deal with combined chains with more than two signatures at the moment. oops...")

              if not TriggerConfigHLT.isChainRegistered(chainDict['chainName']):
                log.error("Chain %s has not been registered in the menu!", chainDict['chainName'])

        
        return TriggerConfigHLT.configsList()

    @memoize
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
            elif not eval('TriggerFlags.' + sig + 'Slice.signatures()'):
                log.debug('Signature %s is not switched on (no chains in menu)', sig)
            elif not eval('self.do' + sig + 'Chains'):
                log.debug('Signature %s is not switched on (disabled by flag)', sig)

        log.info("The following signature(s) is (are) enabled: %s", self.signaturesToGenerate)

        if self.selectChainsForTesting:
            log.info("Narrowing down the list of chains with the selectChainsForTesting list")
            selectedChains = [ch for ch in chains if ch.name in self.selectChainsForTesting]
            if len(selectedChains) < len(self.selectChainsForTesting):
                selectedNames = [ch.name for ch in selectedChains]
                missingNames = [name for name in self.selectChainsForTesting if name not in selectedNames]
                log.warning("The following chains were specified in selectChainsForTesting but were not found in the menu: %s", str(missingNames))
            chains = selectedChains

        if len(chains) == 0:
            log.warning("There seem to be no chains in the menu - please check")
        else:
            log.debug("The following chains were found in the menu: \n %s", '\n '.join(map(str,(chains) )))
            
        return chains 
                                

    def __generateChainConfig(self, mainChainDict):
        """
        # Assembles the chain configuration and returns a chain object with (name, L1see and list of ChainSteps)
        """
        # check if all the signature files can be imported files can be imported
        for sig in self.signaturesToGenerate:
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
                import traceback
                traceback.print_exc()

        log.debug('Available signature(s) for chain generation: %s', self.availableSignatures)

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
        lengthOfChainConfigs = []

        for chainDict in chainDicts:
            chainConfigs = None
            currentSig = chainDict['signature']
            chainName = chainDict['chainName']
            log.debug('Checking chainDict for chain %s in signature %s' , chainName, currentSig)

            sigFolder = ''
            if currentSig == 'Electron' or currentSig == 'Photon':
                sigFolder = 'Egamma'
            elif currentSig in self.calibCosmicMonSigs:
                sigFolder = 'CalibCosmicMon'
            else:
                sigFolder = currentSig

            if currentSig in self.availableSignatures and currentSig != 'Combined':
                try:                    
                    log.debug("Trying to get chain config for %s in folder %s", currentSig, sigFolder)
                    functionToCall ='TriggerMenuMT.HLTMenuConfig.' + sigFolder + '.Generate' + currentSig + 'ChainDefs.generateChainConfigs(chainDict)' 
                    chainConfigs = eval(functionToCall)
                except RuntimeError:
                    log.exception( 'Problems creating ChainDef for chain\n %s ', chainName)
                    continue
            else:
                log.error('Chain %s ignored - Signature not available', chainDict['chainName'])
            listOfChainConfigs.append(chainConfigs)
            lengthOfChainConfigs.append((len(chainConfigs.steps),currentSig))

        ## if log.isEnabledFor(logging.DEBUG):
        ##     import pprint
        ##     pp = pprint.PrettyPrinter(indent=4, depth=8)
        ##     log.debug('mainChainDict dictionary: %s', pp.pformat(mainChainDict))


        # This part is to deal with combined chains between different signatures
        if len(listOfChainConfigs) == 0:
            log.error('No Chain Configuration found ')
            return False

        elif len(listOfChainConfigs)>1:
                log.debug("Merging strategy from dictionary: %s", mainChainDict["mergingStrategy"])
                theChainConfig = mergeChainDefs(listOfChainConfigs, mainChainDict)

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

        log.debug('ChainConfigs  %s ', theChainConfig)
        return theChainConfig,lengthOfChainConfigs

    @memoize
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
            log.info('ChainName %s', cc.name)
            log.info('  ChainSteps %s', cc.steps)
            for step in cc.steps:
                log.info(step)

        makeHLTTree(newJO=False, triggerConfigHLT = TriggerConfigHLT)
        # the return values used for debugging, might be removed later

        # Having built the Menu add prescales for disabling items (e.g. MC production)
        applyHLTPrescale(TriggerConfigHLT, self.HLTPrescales, self.signaturesOverwritten)

        from TriggerMenuMT.HLTMenuConfig.Menu.HLTMenuJSON import generateJSON
        generateJSON()

        from TriggerMenuMT.HLTMenuConfig.Menu.HLTPrescaleJSON import generateJSON as generatePrescaleJSON
        generatePrescaleJSON()

        return finalListOfChainConfigs
