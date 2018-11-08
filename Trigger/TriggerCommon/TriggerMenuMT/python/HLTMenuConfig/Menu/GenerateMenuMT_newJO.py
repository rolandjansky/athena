# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
from AthenaCommon.Logging import logging
from AthenaCommon.Constants import VERBOSE,INFO,DEBUG
_log = logging.getLogger('GenerateMenuMT_newJO')
_log.setLevel( VERBOSE )

def fillGeneratorsMap( sigMap, signature ):
    """ Fill the mapping from the flag container name to the function responsible for generating the Chain objects
    
    Here the files naming convention is employed: the chains mentioned in Trigger.menu.XYZ are served by the function in HLTMenuConfig.XYZ.generateChains"""

    capitalizedSignature = signature.capitalize()
    importString = 'TriggerMenuMT.HLTMenuConfig.{}.generate{}'.format(capitalizedSignature, capitalizedSignature)

    try:
        gen = __import__(importString, globals(), locals(), ['generateChains'])
        sigMap[signature] = gen.generateChains
        _log.info( 'Imported generator for %s' % signature )
    except Exception as ex:
        _log.warning( 'Cant import {} {}'.format(signature, ex) )

    

def generateMenu( flags ):
    """
    Using flags generate appropriate Control Flow Graph wiht all HLT algorithms
    """

    # convert to chainDefs
    from TriggerMenuMT.HLTMenuConfig.Menu.DictFromChainName import DictFromChainName
    toChainDictTranslator = DictFromChainName()

    flatChainDicts = []
    counter = 0
    signatureToGenerator = {}
    menuChains = []

    for name, cfgFlag in flags._flagdict.iteritems():
        if not 'Trigger.menu.' in name:
            continue

        # fill the map[signature, generating function]
        variableName = name.split('.')[-1]
        fillGeneratorsMap( signatureToGenerator, variableName )

        print(cfgFlag.get())

        for chain in cfgFlag.get():

            chainDict = toChainDictTranslator.getChainDict( chain )

            counter += 1
            chainDict['chainCounter'] = counter
            # todo topo threshold

            flatChainDicts.append( chainDict )

            # call generating function and pass to CF builder
            if variableName in signatureToGenerator:
                menuChains.append( signatureToGenerator[variableName](flags, chainDict) )
            else:
                _log.warning('Missing generator for the chain {}'.format(chainDict['chainName']))

    _log.info('Obtained Menu Chain objects')

    # pass all menuChain to CF builder

    _log.info('CF is built')

###########################################################################################################
    # # join flags
    # menuFlags = [f.get() for name, f in flags._flagdict.iteritems() if "Trigger.menu." in name ]
    # flatMenu = reduce( lambda x,y: x+y, menuFlags, [] )
    #
    # # convert to chainDefs
    # from TriggerMenuMT.HLTMenuConfig.Menu.DictFromChainName import DictFromChainName
    # toChainDictTranslator = DictFromChainName()
    # flatChainDicts = [ toChainDictTranslator.getChainDict( chain ) for chain in flatMenu ]
    #
    # for counter, d in enumerate( flatChainDicts, 1 ) :
    #     d["chainCounter"] = counter
    #     # todo topo threshold

    # _log.info("Translated menu flags to chainDicts")
###########################################################################################################



    # # import signature fragments and fill the map[signature, generating function]
    # signatureToGenerator = {}
    # signatures = [ name.split('.')[-1]  for name in flags._flagdict.iterkeys() if 'Trigger.menu.' in name ]
    # for s in signatures:
    #     fillGeneratorsMap( signatureToGenerator, s )
    # _log.info('Retrieved generators')
        
    # # for each chain call generating function and pass to CF builder
    # menuChains = []
    # for chain in flatChainDicts:
    #     if chain['sourceVariableName'] in signatureToGenerator:
    #         menuChains.append(  signatureToGenerator[ chain['sourceVariableName'] ]( flags, chain ) )
    #     else:
    #         _log.warning( 'Missing generator for the chain ' + chain['chainName'] )
    
    # _log.info( 'Obtained Menu Chain objects' )
    # # pass all menuChain to CF builder
    #
    # _log.info( 'CF is built' )
            
    return None # will return once the CF build is realy invoked


