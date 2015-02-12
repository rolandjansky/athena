# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

"""CPS addition  """

__doc__="Addition of coherent prescales to the list of chains"

from TriggerJobOpts.TriggerFlags  import TriggerFlags
from TriggerMenu.l1.Lvl1Flags import Lvl1Flags

from AthenaCommon.Logging import logging

log = logging.getLogger('TriggerMenu.CPS.py')


def hackedCPSAdder(groups, signatures, chains, level):
#     Emanuel: This is an ugly, ugly hack to deal with mutliple
#     instances of the same chain, corresponding to a signature. A
#     proper fix will require re-imagining the filling of
#     triggerPythonConfig's theL2HLTChains and theEFHLTChains (which
#     I'll do after I get back from vacation. See Savannah #84711 for
#     more details.
    for cpsGroup, chainNames in groups.iteritems():
        for chainName in chainNames:
            if chainName not in signatures:
                log.warning('Signature %s not registered to TriggerPythonConfig' % chainName)
                continue

            if level == 'HLT':
                chain_name = signatures[chainName][-1].chain_name
            else:
                log.error("Level of chain not defined")
                
            for c in chains:
                if c.chain_name == chain_name:
                    log.info(' ===> Now in my CPS.py : level=%s, chainName=%s, cpsGroup=%s, chain_name=%s' % (level, chainName, cpsGroup, chain_name))
                    log.info('                       : signatures: [0]=%s, [-1]=%s' % (signatures[chainName][0].chain_name, signatures[chainName][-1].chain_name))
                    log.info('                       : chain: c.chain_name=%s, c.groups=%s, cpsGroup=%s' % (c.chain_name, c.groups, cpsGroup))
                    cpsFound = None
                    for g in c.groups:
                        if g.find('CPS') == 5:
                            cpsFound = g
                    if cpsFound == None:
                        log.info('                       : no CPS group found. group=%s added' % cpsGroup)
                        c.addGroup(cpsGroup)
                    elif cpsFound == cpsGroup:
                        log.info('                       : CPS group with same name already exists (%s). Nothing added' % cpsGroup)
                    else:
                        log.warning('                       : CPS group with different name already exists (%s). Group =%s not added' % (cpsFound, cpsGroup))

            #[c.addGroup(cpsGroup) for c in chains if c.chain_name == chain_name]
            



def genericCPSAdder(groups, chains, level):
    #levelMap = {"L2": 0,
    #            "EF": 1,
    #            }
    #level = levelMap[level]
    #print 'igb level', level
    
    for cpsGroup, chainNames in groups.iteritems():
        for chainName in chainNames:
            if chainName not in chains.keys():
                log.warning('Signature %s not registered to TriggerPythonConfig' % chainName)
                continue

            if level=='L2':
                l2chain = chains[chainName][0]
                l2chain.addGroup(cpsGroup)
            elif level=='EF':
                if  len(chains[chainName]) ==2:
                    efchain = chains[chainName][1]
                    efchain.addGroup(cpsGroup)
                elif len(chains[chainName]) ==1:
                    efchain = chains[chainName][0]
                    efchain.addGroup(cpsGroup)

def defineCPSGroups():
    HLT_CPS_Groups = {
        #EXAMPLE:  
        # "RATE:CPS:HLT_mu4"  :  ['mu4_cosmic_L1MU11_EMPTY', 
        #                         'mu4_msonly_cosmic_L1MU11_EMPTY']        
        }
    return (HLT_CPS_Groups)

def addCPS(triggerPythonConfig):
    HLT_CPS_Groups = defineCPSGroups()
    hackedCPSAdder(HLT_CPS_Groups, triggerPythonConfig.allChains, triggerPythonConfig.theHLTChains, "HLT")
