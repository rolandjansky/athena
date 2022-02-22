# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
import itertools

from TriggerMenuMT.HLT.Config.Utility.DictFromChainName import dictFromChainName
from TriggerMenuMT.HLT.Config.ControlFlow.HLTCFConfig_newJO import generateDecisionTree
from TriggerMenuMT.HLT.Config.Utility.TriggerConfigHLT import TriggerConfigHLT
from TriggerMenuMT.HLT.Config.Utility.ChainMerging import mergeChainDefs
from TriggerMenuMT.HLT.Config.Utility.ChainDictTools import splitInterSignatureChainDict
from TriggerMenuMT.HLT.Config.Utility.MenuAlignmentTools import MenuAlignment

import importlib

from AthenaCommon.Logging import logging
log = logging.getLogger(__name__)

def getChainsOfMenu(flags):
    """
    Import set of chains (object of ChainProp) according to the menu setting in flags
    """    
    setupMenuPath = "TriggerMenuMT.HLT.Menu."+flags.Trigger.triggerMenuSetup+"_newJO"
    setupMenuModule = importlib.import_module( setupMenuPath )
    assert setupMenuModule is not None, "Could not import module {}".format(setupMenuPath)
    assert setupMenuModule.setupMenu is not None, "Could not import setupMenu function from {}".format(setupMenuPath)
    return setupMenuModule.setupMenu()

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

def getGenerator(signature):
    """
    Fill the mapping from the flag container name to the function responsible for generating the Chain objects

    Here the files naming convention is employed: the chains mentioned in Trigger.menu.XYZ are served by the function in HLT.XYZ.generateXYZ
    where XYZ is the "signature" property of the chainDict (passed here directly) """

    importString = 'TriggerMenuMT.HLT.{0}.generate{0}'.format(signature)
    gen = importlib.import_module( importString )
    log.info('Imported generator for %s', signature)
    return gen.generateChains


def fillAllGeneratorsMap(chainsInMenu, sigGenMap):
    """
    For all chains finds out which signatures they depend on, and load appropriate generators

    Arument is the list of ChainDictsm and the map: signature-> generator to fill
    """
    for chain in chainsInMenu:
        for signature in chain["signatures"]:
            if  signature not in sigGenMap:
                sigGenMap[signature] = getGenerator(signature)

chainCounterGenerator = itertools.count()

def generateChainConfig(flags, chain, sigGenMap):
    """
    Generte chain definitions (Chain objects) given chainName
    """
    mainChainDict = chain
    mainChainDict['chainCounter'] = next(chainCounterGenerator)
    mainChainDict['prescale'] = 1
    chainDicts = splitInterSignatureChainDict(mainChainDict)
    listOfChainConfigs = []
    alignmentLengths ={}
    for chainDict, alignmentGroup in zip(chainDicts, mainChainDict['alignmentGroups']):
        signature = chainDict['signature']
        if signature not in sigGenMap:
            log.error('Generator for %s is missing. Chain config can not be built', signature)
            return
        chainConfig = sigGenMap[signature](flags, chainDict)
        alignmentLengths[alignmentGroup] = len(chainConfig.steps)
        chainConfig.alignmentGroups = [alignmentGroup]
        listOfChainConfigs.append(chainConfig)

    if len(listOfChainConfigs) > 1:
        theChainConfig = mergeChainDefs(listOfChainConfigs, mainChainDict)
    else:
        theChainConfig = listOfChainConfigs[0]

    mainChainDict["alignmentLengths"] = alignmentLengths
    return mainChainDict, theChainConfig


def doMenuAlignment(chains):
    """
    Invoke menu alignment procedures and register aligned chains in the TriggerConfigHLT

    Input is a list of pairs, (chain dict, chain config)
    """
    groups = [c[0]['alignmentGroups'] for c in chains]
    log.info('Alignment Combinations %s', groups)
    alignmentCombinations = set([tuple(set(g)) for g in groups if len(g) > 1])
    alignmentGroups = set(*alignmentCombinations)
    log.info('Alignment Combinations %s', alignmentCombinations)
    log.info('Alignment Groups %s', alignmentGroups)

    alignmentLengths = dict.fromkeys(list(itertools.chain(*groups)), 0)
    
    for chainDict, chainConfig in chains:
        for group, clen in chainDict['alignmentLengths'].items():
            alignmentLengths[group] = max(alignmentLengths[group], clen)
        del chainDict['alignmentLengths']
    log.info('Alignment Lengths %s', alignmentLengths)

    menuAlignment = MenuAlignment(alignmentCombinations,
                                  alignmentGroups,
                                  alignmentLengths)
    menuAlignment.analyse_combinations()

    reverseAlignmentLengths = [ el[::-1] for el in alignmentLengths.items()]
    for chainDict, chainConfig in chains:
        # needs to match up with the maximum number of steps in a signature in the menu (length_of_configs)
        alignmentGroups = chainDict['alignmentGroups']
        #parallel-merged single-signature chains or single signature chains. Anything that needs no splitting!
        if len(set(alignmentGroups)) == 1:
            alignedChainConfig = menuAlignment.single_align(chainDict, chainConfig)
        elif len(alignmentGroups) == 2:
            alignedChainConfig = menuAlignment.multi_align(chainDict, chainConfig, reverseAlignmentLengths)
        else:
            assert False, "Do not handle more than one calignment group"
        TriggerConfigHLT.registerChain(chainDict, alignedChainConfig)


def loadChains(flags):
    """
    Using the menu set in flags load configuration of all needed chains into the TriggerConfigHLT
    return list of pairs(chain dict, chain config)
    """
    log.info('Obtaining Menu Chain objects for menu %s', flags.Trigger.triggerMenuSetup)
    chainsInThisMenu = getChainsOfMenu(flags)
    chainsList = []
    for oneSigList in chainsInThisMenu.values():
        for chainProp in oneSigList:
            mainChainDict = dictFromChainName(chainProp)
            if acceptChain( mainChainDict, flags.Trigger.triggerMenuModifier ):
                chainsList.append(mainChainDict)
    sigGenMap = {}
    fillAllGeneratorsMap(chainsList, sigGenMap)
    log.info("Loaded generators for these signatures %s", " ".join(sigGenMap.keys()))
    listDictsAndConfigs = []
    for chain in chainsList:
        listDictsAndConfigs.append(generateChainConfig(flags, chain, sigGenMap))
    log.info("Chain configs generated")
    doMenuAlignment( listDictsAndConfigs )
    log.info("Menu aligned")


def generateMenu(flags):
    """
    Generate appropriate Control Flow Graph wiht all HLT algorithms
    """
    loadChains(flags)

    menuAcc = generateDecisionTree(flags, TriggerConfigHLT.configsList())
    menuAcc.wasMerged()
    if log.getEffectiveLevel() <= logging.DEBUG:
        menuAcc.printConfig()

    log.info('CF is built')

    # # generate JOSON representation of the config
    from TriggerMenuMT.HLT.Config.JSON.HLTMenuJSON import generateJSON_newJO
    generateJSON_newJO(flags, TriggerConfigHLT.dictsList(), TriggerConfigHLT.configsList(), menuAcc.getSequence("HLTAllSteps"))

    from TriggerMenuMT.HLT.Config.JSON.HLTPrescaleJSON import generateJSON_newJO as generatePrescaleJSON_newJO
    generatePrescaleJSON_newJO(flags, TriggerConfigHLT.dictsList(), TriggerConfigHLT.configsList())

    return menuAcc


if __name__ == "__main__":
    assert getGenerator("muon") is not None, "Can not get basic generator"
    chainsInMenu = [dictFromChainName("HLT_e10_L1EM7"), dictFromChainName("HLT_e12_L1EM10"),
                    dictFromChainName("HLT_e10_mu6_L1EM7_MU3V")]
    sigMap = {}
    fillAllGeneratorsMap(chainsInMenu, sigMap)
    assert "Muon" in sigMap, "Muons missing"
    assert "Electron" in sigMap, "Electrons missing"
    log.info("Generators laoding works ok")

    from AthenaCommon.Configurable import Configurable
    Configurable.configurableRun3Behavior = 1
    from AthenaConfiguration.AllConfigFlags import ConfigFlags
    from AthenaConfiguration.TestDefaults import defaultTestFiles

    ConfigFlags.Input.Files = defaultTestFiles.RAW
    ConfigFlags.Trigger.triggerMenuSetup = "Dev_pp_run3_v1"
    ca = generateMenu(ConfigFlags)
    ca.printConfig()
    ca.wasMerged()
    log.info("All ok")
