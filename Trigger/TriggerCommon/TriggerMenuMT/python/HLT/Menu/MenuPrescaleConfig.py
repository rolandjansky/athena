# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

import itertools, importlib, re

from AthenaCommon.Logging import logging
log = logging.getLogger( __name__ )


def MenuPrescaleConfig(hltMenuConfig, flags):
    L1Prescales = {}
    HLTPrescales = {}
    chains = {}
    Prescales = PrescaleClass()

    menu_name = flags.Trigger.triggerMenuSetup

    log.info('Menu name: %s', menu_name)

    # Generate the list of chains from the basic menu (terminates in a version number)
    base_menu_name = re.match(r'\w*_v\d*',menu_name).group(0)
    try:
        menu_module = importlib.import_module(f'TriggerMenuMT.HLT.Menu.{base_menu_name}')
    except Exception as e:
        log.fatal(f'Failed to import menu module "{base_menu_name}" inferred from menu "{menu_name}"!')
        raise e
    chains = menu_module.setupMenu()

    # Determine prescale sets from the remainder of the menu name
    # These filters become progressively less inclusive:
    #  - Primary: primary (always unprescaled) triggers only
    #  - BulkMCProd: triggers for generic CP and physics analysis work (veto label PS:NoBulkMCProd)
    #  - TriggerValidation: adds more specialised triggers for trigger performance
    #    and/or CP studies, possibly run heavily prescaled in data (veto label PS:NoTrigVal)
    #  - HLTReprocessing: excludes a handful of triggers not to be
    #    used in reprocessing jobs, mostly due to CPU cost (veto label PS:NoHLTRepro)
    # 
    # Besides the filter, explicit prescale values may also be set for specific triggers
    # -- see the end of the file for these mappings, which should not overlap with the
    #    disabled triggers (else they will be overwritten)
    if 'Primary_prescale' in menu_name:
        filterChains(chains, Prescales.HLTPrescales_primary_prescale,
                        ['Primary:L1Muon','Primary:Legacy','Primary:PhaseI','Primary:CostAndRate'],
                        invert = True)  # enable these groups
        L1Prescales = Prescales.L1Prescales_primary_prescale
        HLTPrescales = Prescales.HLTPrescales_primary_prescale
    elif 'BulkMCProd_prescale' in menu_name:
        assert(menu_name.startswith('Dev') or menu_name.startswith('MC'))
        filterChains(chains, Prescales.HLTPrescales_bulkmcprod_prescale, ["PS:NoHLTRepro","PS:NoTrigVal","PS:NoBulkMCProd","PS:Online"])
        L1Prescales = Prescales.L1Prescales_bulkmcprod_prescale
        HLTPrescales = Prescales.HLTPrescales_bulkmcprod_prescale
    elif 'TriggerValidation_prescale' in menu_name:
        filterChains(chains, Prescales.HLTPrescales_trigvalid_prescale, ["PS:NoHLTRepro","PS:NoTrigVal","PS:Online"])
        L1Prescales = Prescales.L1Prescales_trigvalid_prescale
        HLTPrescales = Prescales.HLTPrescales_trigvalid_prescale
    elif 'HLTReprocessing_prescale' in menu_name:
        filterChains(chains, Prescales.HLTPrescales_HLTRepro_prescale, ["PS:NoHLTRepro"])
        L1Prescales = Prescales.L1Prescales_HLTRepro_prescale
        HLTPrescales = Prescales.HLTPrescales_HLTRepro_prescale
    elif 'prescale' in menu_name:
        log.error(f'Unknown menu prescale set for menu {menu_name}!')
        raise RuntimeError('Unknown menu prescale set')
    else:
        L1Prescales = Prescales.L1Prescales
        HLTPrescales = Prescales.HLTPrescales

    return (L1Prescales, HLTPrescales, chains)


def filterChains(chains, type_prescales, type_groups, invert=False):
    """Disable(enable if invert==True) chains with groups matching type_groups"""

    def match(s1, s2): return not s1.isdisjoint(s2)

    # Loop over all chains in all signatures and find chains to enable/disable
    chain_disable_list = [ch.name for ch in itertools.chain.from_iterable(chains.values())
                          if match(set(type_groups), ch.groups) ^ invert ]

    type_prescales.update(itertools.zip_longest(chain_disable_list, [], fillvalue=-1))


def applyHLTPrescale(triggerPythonConfig, HLTPrescale, ignoreUnknownChains=False):
    for item, prescale in HLTPrescale.items():
        if item not in triggerPythonConfig.dicts().keys():
            if ignoreUnknownChains:
                log.warning('Attempt to set prescales for nonexisting chain: %s', item)
                continue
            else:
                log.error('Attempt to set prescales for nonexisting chain: %s', item)
                continue
        hltchain = triggerPythonConfig.dicts()[item]
        hltchain['prescale'] = prescale
        log.info('Applied HLTPS to the item %s: PS %s', item, hltchain['prescale'])
       
class PrescaleClass(object):
    #   Item name             | Prescale
    #----------------------------------------------------------
    L1Prescales = {}
    L1Prescales = dict([(ctpid,1) for ctpid in L1Prescales])  # setting all L1 prescales to 1 # NOT DOING ANYTHING ATM

    #   Item name   | HLTprescale
    #   - Chains only need adding if have a Prescale value different from 1 (default)
    #----------------------------------------------------------
    HLTPrescales = {}

    L1Prescales_primary_prescale  = {}
    HLTPrescales_primary_prescale = {}

    L1Prescales_trigvalid_prescale  = {}
    HLTPrescales_trigvalid_prescale = {}

    L1Prescales_bulkmcprod_prescale  = {}
    HLTPrescales_bulkmcprod_prescale = {}

    # PS for reprocessing and related ART tests
    # Avoid high CPU usage for low-mu triggers
    # mainly to be used for 900 GeV, or other triggers
    # that should normally run with prescales in standard pp
    L1Prescales_HLTRepro_prescale  = {}
    HLTPrescales_HLTRepro_prescale = {
        'HLT_cosmic_id_L1MU3V_EMPTY':          10,
        'HLT_cosmic_id_L1MU8VF_EMPTY':         10,
    }

    chain_list=[]
