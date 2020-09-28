# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaCommon.CFElements import seqAND

from TriggerMenuMT.HLTMenuConfig.Menu.HLTCFConfig import generateDecisionTreeOld
from TriggerMenuMT.HLTMenuConfig.Menu.HLTCFConfig_newJO import generateDecisionTree
from TriggerMenuMT.HLTMenuConfig.Menu.TriggerConfigHLT import TriggerConfigHLT
from TriggerMenuMT.HLTMenuConfig.Menu.ChainMerging import mergeChainDefs
from TriggerMenuMT.HLTMenuConfig.Menu.ChainDictTools import splitInterSignatureChainDict
from six import iteritems

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


def generateMenu( flags ):
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
    menuAcc.addSequence( seqAND(mainSequenceName) )


    for name, cfgFlag in list(iteritems(flags._flagdict)):
        if 'Trigger.menu.' not in name:
            continue
        value = flags._get(name)
        if len(value) == 0:
            continue

        signatureName = name.split('.')[-1]
        signatures = []

        # fill the map[signature, generating function]
        if signatureName== 'combined':
            for chain in cfgFlag.get():
                signatures += dictFromChainName(chain)['signatures']
        else:
            signatures = [signatureName]

        for sig in signatures:
            fillGeneratorsMap( signatureToGenerator, sig.lower() )

        # call generating function and pass to CF builder
        for chain in cfgFlag.get():
            # TODO topo threshold
            mainChainDict = dictFromChainName( chain )
            
            counter += 1
            mainChainDict['chainCounter'] = counter

            #set default chain prescale
            mainChainDict['prescale'] = 1

            allChainDicts.append(mainChainDict)

            chainDicts = splitInterSignatureChainDict(mainChainDict)
            listOfChainConfigs = []
            print("aaa", chainDicts)
            for chainDict in chainDicts:
                signature = chainDict['signature'].lower()

                if signature not in signatureToGenerator:
                    log.warning('Generator for {} is missing. Chain dict will not be built'.format(signature))
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


