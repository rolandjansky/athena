# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaCommon.CFElements import seqAND

from TriggerMenuMT.HLTMenuConfig.Menu.HLTCFConfig import generateDecisionTreeOld
from TriggerMenuMT.HLTMenuConfig.Menu.HLTCFConfig_newJO import generateDecisionTree

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


        # fill the map[signature, generating function]

        signature = name.split('.')[-1]
        fillGeneratorsMap( signatureToGenerator, signature )

        if signature not in signatureToGenerator:
            log.warning('Generator for {} is missing. Chain dict will not be built'.format(signature))
            continue

        for chain in cfgFlag.get():

            chainDict = dictFromChainName( chain )

            counter += 1
            chainDict['chainCounter'] = counter

            allChainDicts.append(chainDict)
            # TODO topo threshold

            # call generating function and pass to CF builder

            chain = signatureToGenerator[signature](flags, chainDict)
            menuChains.append( chain )


    log.info('Obtained Menu Chain objects')

    # pass all menuChain to CF builder
    useReworked = True

    if useReworked:
        menuAcc.wasMerged()
        menuAcc = generateDecisionTree(menuChains)
    else:
        menuAcc.wasMerged()
        menuAcc = ComponentAccumulator()
        mainSequenceName = 'HLTAllSteps'
        menuAcc.addSequence( seqAND(mainSequenceName) )
        chainsAcc = generateDecisionTreeOld(menuAcc.getSequence(mainSequenceName), menuChains, allChainDicts)
        menuAcc.merge(chainsAcc)

    menuAcc.printConfig()

    log.info('CF is built')


    # # generate JOSON representation of the config
    from TriggerMenuMT.HLTMenuConfig.Menu.HLTMenuJSON import generateJSON_newJO    
    generateJSON_newJO( allChainDicts, menuChains, menuAcc.getSequence("HLTAllSteps") )

    return menuAcc


