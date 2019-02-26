# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

"""CPS addition  """

__doc__="Addition of coherent prescales to the list of chains"

from TriggerJobOpts.TriggerFlags import TriggerFlags

from AthenaCommon.Logging import logging
log = logging.getLogger(__name__)


def genericCPSAdder(groups, signatures, chains, level, signatureOverwritten):

    for cpsGroup, chainNames in groups.iteritems():
        if "RATE" not in cpsGroup:
            log.error('Following group do not start for RATE [%s]' %cpsGroup)
        for chainName in chainNames:
            if chainName not in signatures:
                if not signatureOverwritten:
                    log.error('Signature %s not registered to TriggerPythonConfig' % chainName)
                else:
                    log.warning('Signature %s not registered to TriggerPythonConfig' % chainName)
                continue

            if level == 'HLT':
                chain_name = signatures[chainName][-1].chain_name
            else:
                log.error("Level of %s not defined", chainName)
                
            for c in chains:
                if c.chain_name == chain_name:
                    log.debug('chain: c.chain_name=%s, c.groups=%s, cpsGroup=%s', c.chain_name, c.groups, cpsGroup)
                    cpsFound = None
                    for g in c.groups:
                        if g.find('CPS') == 5:
                            cpsFound = g
                    if cpsFound == None:
                        log.info('CPS group %s added to %s', cpsGroup, chain_name)
                        c.addGroup(cpsGroup)
                    elif cpsFound == cpsGroup:
                        log.debug('CPS group %s already assigned to %s. Nothing added', cpsGroup, chain_name)
                    else:
                        log.warning('CPS group with different name (%s) already exists for %s. Group %s not added', cpsFound, chain_name, cpsGroup)

            #[c.addGroup(cpsGroup) for c in chains if c.chain_name == chain_name]



def defineCPSGroups():
    #Since v8 separate cps into a different file
    if "v8" in TriggerFlags.triggerMenuSetup():
        from TriggerMenu.menu.CPS_v8 import HLT_CPS_Groups
    else:
        from TriggerMenu.menu.CPS_prev8 import HLT_CPS_Groups
    return HLT_CPS_Groups

def addCPS(triggerPythonConfig,signatureOverwritten):
    HLT_CPS_Groups = defineCPSGroups()
    genericCPSAdder(HLT_CPS_Groups, triggerPythonConfig.allChains, triggerPythonConfig.theHLTChains, "HLT",signatureOverwritten)
