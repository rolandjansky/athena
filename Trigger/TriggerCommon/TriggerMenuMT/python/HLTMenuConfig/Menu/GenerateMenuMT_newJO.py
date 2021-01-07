# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaCommon.CFElements import seqAND

from TriggerMenuMT.HLTMenuConfig.Menu.HLTCFConfig_newJO import generateDecisionTree
from TriggerMenuMT.HLTMenuConfig.Menu.TriggerConfigHLT import TriggerConfigHLT
from TriggerMenuMT.HLTMenuConfig.Menu.ChainMerging import mergeChainDefs
from TriggerMenuMT.HLTMenuConfig.Menu.ChainDictTools import splitInterSignatureChainDict

from AthenaCommon.Logging import logging
log = logging.getLogger( __name__ )

def fillGeneratorsMap( sigMap, signature ):
    """ Fill the mapping from the flag container name to the function responsible for generating the Chain objects

    Here the files naming convention is employed: the chains mentioned in Trigger.menu.XYZ are served by the function in HLTMenuConfig.XYZ.generateChains"""

    if signature in sigMap:
        return

    capitalizedSignature = signature.capitalize()
    importString = 'TriggerMenuMT.HLTMenuConfig.{}.generate{}'.format(capitalizedSignature, capitalizedSignature)


    gen = __import__(importString, globals(), locals(), ['generateChains'])
    sigMap[signature] = gen.generateChains
    log.info( 'Imported generator for %s', signature )

def obtainChainsOfMenu(flags):
    import importlib
    setupMenuPath = "TriggerMenuMT.HLTMenuConfig.Menu."+flags.Trigger.triggerMenuSetup+"_newJO"
    setupMenuModule = importlib.import_module( setupMenuPath )
    assert setupMenuModule is not None, "Could not import module {}".format(setupMenuPath)
    assert setupMenuModule.setupMenu is not None, "Could not import setupMenu from {}".format(setupMenuPath)
    return setupMenuModule.setupMenu(flags)


def acceptChain(chainDict, whichSignatures):
    """
    Helper to accept chains to be part of the menu depending on their name or which signatures they belong to.

    If the keyword "all" is present all chains that are in the menu are accepted.
    If the keyword "emptyMenu" is present other keywords are checked.
      They can be name of signature: e.g. Muon or name of chain(s) HLT_j50_L1J30
    Leaving the "emptyMenu" keyword only results in no chains that are configured. As a consequence would cause an issue somewhere downstream.
    """
    if "all" in whichSignatures:
        return True
    if "emptyMenu" in whichSignatures:
        if chainDict["chainName"] in whichSignatures:  #explicit list of chains specified
            return True
        if all( [ sig in whichSignatures for sig in chainDict['signatures'] ]): # all signatures for the chain are mentioned 
            return True
    return False

def generateMenu(flags):
    """
    Using flags generate appropriate Control Flow Graph wiht all HLT algorithms
    
    """

    # convert to chainDefs
    from TriggerMenuMT.HLTMenuConfig.Menu.DictFromChainName import dictFromChainName

    counter = 0
    signatureToGenerator = {}
    menuChains = []
    allChainDicts =[]

    menuAcc = ComponentAccumulator()
    mainSequenceName = 'HLTAllSteps'
    menuAcc.addSequence(seqAND(mainSequenceName))

    chainsInMenu = obtainChainsOfMenu(flags)

    for signatureName, chains  in chainsInMenu.items():
        if len(chains) == 0:
            continue

        signatures = []
        # fill the map[signature, generating function]
        if signatureName== 'combined':
            for chain in chains:
                signatures += dictFromChainName(chain)['signatures']
        else:
            signatures = [signatureName]

        for sig in signatures:
            fillGeneratorsMap( signatureToGenerator, sig.lower() )

        # call generating function and pass to CF builder
        for chain in chains:
            # TODO topo threshold
            mainChainDict = dictFromChainName( chain )
            if not acceptChain( mainChainDict, flags.Trigger.triggerMenuModifier ):
                continue
            counter += 1
            mainChainDict['chainCounter'] = counter

            #set default chain prescale
            mainChainDict['prescale'] = 1

            allChainDicts.append(mainChainDict)

            chainDicts = splitInterSignatureChainDict(mainChainDict)
            listOfChainConfigs = []

            for chainDict in chainDicts:
                signature = chainDict['signature'].lower()

                if signature not in signatureToGenerator:
                    log.warning('Generator for %s is missing. Chain dict will not be built', signature)
                    continue

                chainConfig = signatureToGenerator[signature](flags, chainDict)
                listOfChainConfigs.append(chainConfig)

            if len(listOfChainConfigs) > 1:
                theChainConfig = mergeChainDefs(listOfChainConfigs, mainChainDict)

            else:
                theChainConfig = listOfChainConfigs[0]

            TriggerConfigHLT.registerChain( mainChainDict, theChainConfig )
            menuChains.append( theChainConfig )

    log.info('Obtained Menu Chain objects')

    # pass all menuChain to CF builder
    menuAcc.wasMerged()
    menuAcc = generateDecisionTree(menuChains)
    
    menuAcc.printConfig()

    log.info('CF is built')


    # # generate JOSON representation of the config
    from TriggerMenuMT.HLTMenuConfig.Menu.HLTMenuJSON import generateJSON_newJO    
    generateJSON_newJO( allChainDicts, menuChains, menuAcc.getSequence("HLTAllSteps") )

    from TriggerMenuMT.HLTMenuConfig.Menu.HLTPrescaleJSON import generateJSON_newJO as generatePrescaleJSON_newJO
    generatePrescaleJSON_newJO( allChainDicts, menuChains )

    return menuAcc


