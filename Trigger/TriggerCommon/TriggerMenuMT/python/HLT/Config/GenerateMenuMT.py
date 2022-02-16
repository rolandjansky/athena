# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

import importlib
import string

from TriggerMenuMT.HLT.Menu.TriggerConfigHLT  import TriggerConfigHLT
from TriggerMenuMT.HLT.Menu.HLTCFConfig import makeHLTTree
from TriggerMenuMT.HLT.Menu.DictFromChainName import dictFromChainName
from TriggerMenuMT.HLT.Menu.ChainDictTools import splitInterSignatureChainDict
from TriggerMenuMT.HLT.Menu.MenuPrescaleConfig import MenuPrescaleConfig, applyHLTPrescale
from TriggerMenuMT.HLT.Menu.ChainMerging import mergeChainDefs
from TriggerMenuMT.HLT.Menu.MenuAlignmentTools import MenuAlignment
from TriggerMenuMT.HLT.Menu.ComboHypoHandling import addTopoInfo, comboConfigurator, topoLegIndices
from TriggerMenuMT.HLT.CommonSequences import EventBuildingSequences, TLABuildingSequences

from AthenaConfiguration.AllConfigFlags import ConfigFlags
from AthenaCommon.Logging import logging
log = logging.getLogger(__name__)


class Singleton(type):
    _instances = {}
    def __call__(cls, *args, **kwargs):
        if cls not in cls._instances:
            cls._instances[cls] = super(Singleton, cls).__call__(*args, **kwargs)
        return cls._instances[cls]


# for now we make this a singleton because calling menu generation twice leads to problems
class GenerateMenuMT(object, metaclass=Singleton):

    # Applicable to all menu instances
    calibCosmicMonSigs = ['Streaming','Monitor','Beamspot','Cosmic', 'Calib', 'EnhancedBias']
    combinedSigs = ['MinBias','Electron','Photon','Muon','Tau','Jet', 'Bjet','MET','UnconventionalTracking']
    defaultSigs = ['Streaming']  # for noalg chains

    # Define which signatures (folders) are required for each slice
    def getRequiredSignatures(theslice):
        # All possible signatures
        allSigs = [
            'Test','Streaming','Monitor','Beamspot','Cosmic', 'Calib', 'EnhancedBias',
            'Electron','Photon','Muon','Tau','Jet', 'Bjet','MET','Bphysics',
            'MinBias','UnconventionalTracking'
        ]
        signatureDeps = {sig:[sig] for sig in allSigs}
        # Special cases
        signatureDeps.update({
            # Bjet always requires jet
            'Bjet': ['Bjet','Jet'],
            # Egamma contains two signatures
            'Egamma': ['Electron','Photon'],
            'Combined': GenerateMenuMT.combinedSigs,
        })
        return set(signatureDeps[theslice]+['Streaming']) # always allow streamers

    def __init__(self):
        self.chainsInMenu = {}  # signature : [chains]

        self.allChainsForAlignment = []
        self.chainDicts = []
        self.combinationsInMenu = []
        self.alignmentGroupsToAlign = set()
        self.configLengthDict = {}
        
        self.signaturesOverwritten = False
        self.L1Prescales = None
        self.HLTPrescales = None

        self.chainFilter = None
        self.availableSignatures = []

        self.sigDicts = {}

        self.chainDefModule = {}   # Generate[SIG]ChainDefs module for each SIGnature

    def setChainFilter(self, f):
        """Set chain filter for menu generation.

           This can be any callable object taking two
           arguments for signature and chain name and returning a boolean.
           E.g. to only generate Egamma chains:
                menu.setChainFilter(lambda slice,chain : slice=='Egamma').

           In the special case that f is a functor with the list attributes
           selectChains and/or disableChains, the contents will be explicitly
           checked to be in the menu.
        """
        fname = f.__class__.__name__ if isinstance(f,object) else f.__name__
        import inspect
        if len(inspect.signature(f).parameters)!=2:
            log.error('%s is not a valid chain filter. Function/callable needs take two arguments '
                      'for signature and chain name and return a boolean', fname)
        else:
            log.warning('Setting chain filter to %s', fname)
            self.chainFilter = f


    def getChainDicts(self):

        def validSignature(currentSig, chainSig):
            """Check if chain is assigned to the correct signature"""
            reqd = GenerateMenuMT.getRequiredSignatures(currentSig)
            isValid = chainSig.issubset( reqd )
            log.debug("Chain signatures: %s, required signatures: %s",chainSig,reqd)
            if not isValid:
                log.error("Chain signatures %s not a subset of required signatures %s",set(chainSig),reqd)
            return isValid

        chainCounter = 0
        invalid = False
        for sig, chains in self.chainsInMenu.items():
            for chain in chains:
                log.debug("Now processing chain: %s from signature %s", chain, sig)
                chainCounter += 1
                chainDict = dictFromChainName(chain)
                chainDict['chainCounter'] = chainCounter
                chainDict['prescale'] = 1  # set default chain prescale

                # Pick out the folder and subsignature directories to import
                for sigfo, subsig in chainDict['sigDicts'].items():
                    if sigfo not in self.sigDicts:
                        self.sigDicts[sigfo] = subsig
                    else:
                        for ss in subsig:
                            if ss not in self.sigDicts[sigfo]:
                                self.sigDicts[sigfo].append(ss)

                self.chainDicts.append(chainDict)

                if not validSignature(sig, set(chainDict['signatures'])):
                    invalid=True
                    log.error('Chain %s assigned to signature %s but creates %s',
                              chainDict['chainName'], sig, set(chainDict['signatures']))
        if invalid:
            raise RuntimeError('Incorrect assignment of chains to slices -- see preceding messages.')

    def importSignaturesToGenerate(self):
        """check if all the signature files can be imported and then import them"""

        for sig, subSigs in self.sigDicts.items():
            try:
                for ss in subSigs:
                    import_module = 'TriggerMenuMT.HLT.' + sig +'.Generate' + ss + 'ChainDefs'
                    self.chainDefModule[ss] = importlib.import_module(import_module)

                    if ss not in self.availableSignatures:
                        self.availableSignatures.append(ss)

            except ImportError:
                log.exception('Problems when importing ChainDef generating code for %s', sig)
                import traceback
                traceback.print_exc()

        log.info('Available signature(s) for chain generation: %s', self.availableSignatures)

        return

    def generateChains(self):

        all_chains = []
        combinations_in_menu = []
        alignmentGroups_to_align = set()
        length_of_configs = {}
        
        for chainDict in self.chainDicts:
            log.debug("Next: getting chain configuration for chain %s ", chainDict['chainName'])
            chainConfig,lengthOfChainConfigs = self.__generateChainConfig(chainDict)

            all_chains += [(chainDict,chainConfig,lengthOfChainConfigs)]
            
            #update the alignment group length dictionary if we have a longer number of steps
            #or the signature isn't registered in the dictionary yet
            for config_length, config_grp in lengthOfChainConfigs:
                if config_grp in length_of_configs:
                    if config_length > length_of_configs[config_grp]:
                        length_of_configs[config_grp] = config_length
                else:
                    length_of_configs[config_grp] = config_length

            # find the chains that contain more than one alignment group, to keep track
            # of what combinations do we need to deal with.
            # we're using sets here so we don't end up with duplicates
            if len(set(chainDict['alignmentGroups'])) > 1:  
                combinations_in_menu += [list(set(chainDict['alignmentGroups']))]
                for align_group in list(set(chainDict['alignmentGroups'])):
                    alignmentGroups_to_align.update([align_group])
                    
        self.allChainsForAlignment = all_chains
        self.combinationsInMenu = combinations_in_menu
        self.alignmentGroupsToAlign = alignmentGroups_to_align
        self.configLengthDict = length_of_configs

        return 


    def generateAllChainConfigs(self, flags):
        """
        == Obtains chain configs for all chains in menu
        """

        # get all chain names from menu
        log.info("Will now get chains from the menu")
        self.getChainsFromMenu()

        # decoding of the chain name
        log.info("Will now get chain dictionaries for each chain")
        self.getChainDicts()
        
        if flags.Trigger.disableCPS:
            log.warning('Removing all CPS group because the flag Trigger.disableCPS is set')
            for chainDict in self.chainDicts:
                chainDict['groups'] = [g for g in chainDict['groups'] if not g.startswith('RATE:CPS_')]

        #import the necessary signatures
        log.info("Importing the necessary signatures")
        self.importSignaturesToGenerate()

        log.info("Will now generate the chain configuration for each chain")
        self.generateChains()

        log.info("Will now calculate the alignment parameters")
        #dict of signature: set it belongs to
        #e.g. {'Electron': ['Electron','Muon','Photon']}        
        menuAlignment = MenuAlignment(self.combinationsInMenu,
                                      self.alignmentGroupsToAlign,
                                      self.configLengthDict)
        menuAlignment.analyse_combinations()

        # alignmentGroups_to_align = menuAlignment.groupsToAlign
        # lengthOfChainConfigs = self.configLengthDict
        # combinationsInMenu = menuAlignment.combinationsInMenu
        # alignmentGroup_sets_to_align = menuAlignment.setsToAlign

        log.info('Aligning the following signatures: %s',sorted(menuAlignment.sets_to_align))
        log.debug('Length of each of the alignment groups: %s',self.configLengthDict)


        for chainDict,chainConfig,lengthOfChainConfigs in self.allChainsForAlignment:

              # start by ordering electron, photon, muon by having e+mu, g+mu, e+g chains
              # desired ordering: electron, photon, muon, tau, jet, met, b-jet
              
              # lengthOfChainConfigs is something like this: [(4, 'Photon'), (5, 'Muon')]
              # needs to match up with the maximum number of steps in a signature in the menu (length_of_configs)
              # start with electron! Only need to add post-steps for combined electron chains if the max length in a combined chain
              # is greater than the number of electron steps combined chain. Assume that the max length of an electron chain occurs 
              # in a combined chain.

              alignmentGroups = chainDict['alignmentGroups']
            
              #parallel-merged single-signature chains or single signature chains. Anything that needs no splitting!
              if len(set(alignmentGroups)) == 1: 
                  alignedChainConfig = menuAlignment.single_align(chainDict, chainConfig)
                  TriggerConfigHLT.registerChain( chainDict, alignedChainConfig )

              elif len(alignmentGroups) >= 2:
                  alignedChainConfig = menuAlignment.multi_align(chainDict, chainConfig, lengthOfChainConfigs)

                  TriggerConfigHLT.registerChain( chainDict, alignedChainConfig )              

              else: 
                  log.error("Menu can't deal with combined chains with more than two alignmentGroups at the moment. oops...")
                  raise NotImplementedError("more than three alignment groups still needs implementing in ChainMerging.py, ATR-22206")

              if not TriggerConfigHLT.isChainRegistered(chainDict['chainName']):
                log.error("Chain %s has not been registered in the menu!", chainDict['chainName'])
                import pprint
                pp = pprint.PrettyPrinter(indent=4, depth=8)
                log.error('The chain dictionary is: %s', pp.pformat(chainDict))
                raise Exception("Please fix the menu or the chain.")
        
        
        
        # align event building sequences
        log.info("[generateAllChainConfigs] general alignment complete, will now align TLA chains")
        TLABuildingSequences.alignTLASteps(TriggerConfigHLT.configs(), TriggerConfigHLT.dicts())    
        log.info("[generateAllChainConfigs] general and TLA alignment complete, will now align PEB chains")
        EventBuildingSequences.alignEventBuildingSteps(TriggerConfigHLT.configs(), TriggerConfigHLT.dicts())
        
         
        log.info("[generateAllChainConfigs] all chain configurations have been generated.")
        return TriggerConfigHLT.configsList()


    def getChainsFromMenu(self):
        """
        == Returns the list of chain names that are in the menu
        """

        # go over the slices and put together big list of signatures requested
        (self.L1Prescales, self.HLTPrescales, self.chainsInMenu) = MenuPrescaleConfig(TriggerConfigHLT)

        log.debug("Setup HLT menu with prescales: %s", self.HLTPrescales)

        ## we can already use new set of flags
        #from AthenaConfiguration.AllConfigFlags import ConfigFlags
        #from TriggerMenuMT.HLT.Menu.Dev_pp_run3_v1_newJO import setupMenu as setupMenuFlags
        #setupMenuFlags( ConfigFlags )
        #ConfigFlags.lock()

        # Filter chains if requested
        if self.chainFilter is not None:
            self.signaturesOverwritten = True

            # Verify that if the chain filter has lists of chains
            # they are all in the menu
            chainsToCheck = []
            if hasattr(self.chainFilter,'selectChains'):
                chainsToCheck += self.chainFilter.selectChains
            if hasattr(self.chainFilter,'disableChains'):
                chainsToCheck += self.chainFilter.disableChains
            for chain in chainsToCheck:
                inMenu = False
                for signame in self.chainsInMenu:
                    if chain in [c.name for c in self.chainsInMenu[signame]]:
                        inMenu = True
                        break
                if not inMenu:
                    raise RuntimeError(f'Request to enable/disable chain {chain} that is not in menu')

            for signame in self.chainsInMenu:
                self.chainsInMenu[signame] = [c for c in self.chainsInMenu[signame]
                                              if self.chainFilter(signame, c.name)]

        if not self.chainsInMenu:
            log.warning("There seem to be no chains in the menu - please check")
        elif log.isEnabledFor(logging.DEBUG):
            import pprint
            log.debug("The following chains were found in the menu:")
            pprint.pprint(self.chainsInMenu)


    def __generateChainConfig(self, mainChainDict):
        """
        # Assembles the chain configuration and returns a chain object with (name, L1see and list of ChainSteps)
        """

        # split the the chainDictionaries for each chain and print them in a pretty way
        chainDicts = splitInterSignatureChainDict(mainChainDict)

        if log.isEnabledFor(logging.DEBUG):
            import pprint
            pp = pprint.PrettyPrinter(indent=4, depth=8)
            log.debug('dictionary is: %s', pp.pformat(chainDicts))

        # Loop over all chainDicts and send them off to their respective assembly code
        listOfChainConfigs = []
        tmp_lengthOfChainConfigs = []

        for chainPartDict in chainDicts:
            chainPartConfig = None
            currentSig = chainPartDict['signature']
            currentAlignGroup = None
            if len(chainPartDict['chainParts']) == 1:
                currentAlignGroup = chainPartDict['chainParts'][0]['alignmentGroup']
            
            chainName = chainPartDict['chainName']
            log.debug('Checking chainDict for chain %s in signature %s, alignment group %s' , chainName, currentSig, currentAlignGroup)

            if currentSig in self.availableSignatures:
                try:
                    log.debug("[__generateChainConfigs] Trying to get chain config for %s", currentSig)
                    chainPartConfig = self.chainDefModule[currentSig].generateChainConfigs(chainPartDict)
                except Exception:
                    log.error('[__generateChainConfigs] Problems creating ChainDef for chain %s ', chainName)
                    log.error('[__generateChainConfigs] I am in chain part\n %s ', chainPartDict)
                    log.exception('[__generateChainConfigs] Full chain dictionary is\n %s ', mainChainDict)
                    raise Exception('[__generateChainConfigs] Stopping menu generation. Please investigate the exception shown above.')
            else:
                log.error('Chain %s cannot be generated - Signature "%s" not available', chainPartDict['chainName'], currentSig)
                log.error('Available signature(s): %s', self.availableSignatures)
                raise Exception('Stopping the execution. Please correct the configuration.')

            log.debug("Chain %s chain configs: %s",chainPartDict['chainName'],chainPartConfig)
            listOfChainConfigs.append(chainPartConfig)
            tmp_lengthOfChainConfigs.append((chainPartConfig.nSteps,chainPartConfig.alignmentGroups))

        # this will be a list of lists for inter-sig combined chains and a list with one 
        # multi-element list for intra-sig combined chains
        # here, we flatten it accordingly (works for both cases!)
        lengthOfChainConfigs = []
        for nSteps, aGrps in tmp_lengthOfChainConfigs:
            if len(nSteps) != len(aGrps):
                log.error("Chain part has %s steps and %s alignment groups - these don't match!",nSteps,aGrps)
            else:
                for a,b in zip(nSteps,aGrps):
                    lengthOfChainConfigs.append((a,b))
            
        ## if log.isEnabledFor(logging.DEBUG):
        ##     import pprint
        ##     pp = pprint.PrettyPrinter(indent=4, depth=8)
        ##     log.debug('mainChainDict dictionary: %s', pp.pformat(mainChainDict))


        # This part is to deal with combined chains between different signatures
        if len(listOfChainConfigs) == 0:
            log.error('[__generateChainConfigs] No Chain Configuration found for %s', mainChainDict['chainName'])
            raise Exception("[__generateChainConfigs] chain generation failed, exiting.")
        else:
            if len(listOfChainConfigs)>1:
                log.debug("Merging strategy from dictionary: %s", mainChainDict["mergingStrategy"])
                theChainConfig = mergeChainDefs(listOfChainConfigs, mainChainDict)
            else:
                theChainConfig = listOfChainConfigs[0]
            
            for topoID in range(len(mainChainDict['extraComboHypos'])):
                thetopo = mainChainDict['extraComboHypos'][topoID].strip(string.digits).rstrip(topoLegIndices)
                theChainConfig.addTopo((comboConfigurator[thetopo],thetopo))

            # Now we know where the topos should go, we can insert them in the right steps
            if len(theChainConfig.topoMap) > 0:
                try:
                    log.debug("Trying to add extra ComboHypoTool for %s",mainChainDict['extraComboHypos'])
                    addTopoInfo(theChainConfig,mainChainDict,listOfChainConfigs,lengthOfChainConfigs)
                except RuntimeError:
                    log.error('[__generateChainConfigs] Problems creating ChainDef for chain %s ', chainName)
                    log.error('[__generateChainConfigs] I am in the extraComboHypos section, for %s ', mainChainDict['extraComboHypos'])
                    log.exception('[__generateChainConfigs] Full chain dictionary is\n %s ', mainChainDict)
                    raise Exception('[__generateChainConfigs] Stopping menu generation. Please investigate the exception shown above.')
                        
        # Configure event building strategy
        eventBuildType = mainChainDict['eventBuildType']
        if eventBuildType:
            if 'PhysicsTLA' in eventBuildType:
                log.debug("Adding TLA Step for chain %s", mainChainDict['chainName'])
                TLABuildingSequences.addTLAStep(theChainConfig, mainChainDict)
            
            log.debug('Configuring event building sequence %s for chain %s', eventBuildType, mainChainDict['chainName'])
            EventBuildingSequences.addEventBuildingSequence(theChainConfig, eventBuildType, mainChainDict)

        

        log.debug('ChainConfigs  %s ', theChainConfig)
        return theChainConfig,lengthOfChainConfigs
 
    
    def resolveEmptySteps(self,chainConfigs):

        max_steps = max([len(cc.steps) for cc in chainConfigs], default=0)    

        steps_are_empty = [True for i in range(0,max_steps)]

        for cc in chainConfigs:
            for istep, the_step in enumerate(cc.steps):
                if not the_step.isEmpty:
                    steps_are_empty[istep] = False
        
        log.info("Are there any fully empty steps? %s", steps_are_empty)
        
        empty_step_indices = [i for i,is_empty in enumerate(steps_are_empty) if is_empty]
        
        if len(empty_step_indices) == 0:
            return chainConfigs
        
        if len(self.availableSignatures) != 1:
            log.error("[resolveEmptySteps] The menu you are trying to generate contains a fully empty step. This is only allowed for partial menus.")
            raise Exception("[resolveEmptySteps] Please find the source of this empty step and remove it from the menu.")

        log.info("Will now delete steps %s (indexed from zero)",empty_step_indices)
        
        for cc in chainConfigs:
            new_steps = []
            #only add non-empty steps to the new steps list!
            for istep,step in enumerate(cc.steps):
                if istep not in empty_step_indices:
                    new_steps += [step]
            cc.steps = new_steps

        return chainConfigs 
 

    def generateMT(self, flags=ConfigFlags):
        """
        == Main function of the class which generates L1, L1Topo and HLT menu
        """
        log.info('Starting menu generation')

        # --------------------------------------------------------------------
        # HLT menu generation
        # --------------------------------------------------------------------
        finalListOfChainConfigs = self.generateAllChainConfigs(flags)
        log.info("Length of FinalListofChainConfigs %s", len(finalListOfChainConfigs))
 
        # make sure that we didn't generate any steps that are fully empty in all chains
        # if there are empty steps, remove them
        finalListOfChainConfigs = self.resolveEmptySteps(finalListOfChainConfigs)

        log.debug("finalListOfChainConfig %s", finalListOfChainConfigs)
        for cc in finalListOfChainConfigs:
            log.debug('Steps for %s are %s', cc.name, cc.steps)

        log.info("Making the HLT configuration tree")
        makeHLTTree(newJO=False, triggerConfigHLT = TriggerConfigHLT)
        # the return values used for debugging, might be removed later

        log.info("Applying HLT prescales")

        # Having built the Menu add prescales for disabling items (e.g. MC production)
        applyHLTPrescale(TriggerConfigHLT, self.HLTPrescales, self.signaturesOverwritten)

        log.info("Checking the L1HLTConsistency...")
        from TriggerMenuMT.HLT.Config.Validation.CheckL1HLTConsistency import checkL1HLTConsistency
        checkL1HLTConsistency()
        
        log.info("Checking the Coherent Prescale assignments...")
        from TriggerMenuMT.HLT.Config.Validation.CheckCPSGroups import checkCPSGroups
        checkCPSGroups(TriggerConfigHLT.dictsList())

        log.info("Generating HLT menu JSON...")
        
        from TriggerMenuMT.HLT.Menu.HLTMenuJSON import generateJSON
        generateJSON()

        log.info("Generating HLT prescale JSON...")

        from TriggerMenuMT.HLT.Menu.HLTPrescaleJSON import generateJSON as generatePrescaleJSON
        generatePrescaleJSON()

        log.info('Generating HLTMonitoring JSON...')
        from TriggerMenuMT.HLT.Menu.HLTMonitoringJSON import generateDefaultMonitoringJSON
        generateDefaultMonitoringJSON()

        log.info('Menu generation is complete.')
        return finalListOfChainConfigs

