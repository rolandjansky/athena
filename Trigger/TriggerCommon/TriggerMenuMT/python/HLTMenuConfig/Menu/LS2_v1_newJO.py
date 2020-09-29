# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
from TriggerMenuMT.HLTMenuConfig.Menu.ChainDefInMenu import ChainProp

import six

# def get_flag_item(chainName, L1itemsChainParts, groups):
#     PhysicsStream = 'Main'

#     if L1itemsChainParts == []:
#         L1item = 'L1_'+chainName.split('_L1')[-1]
#         L1thresholds = L1item.split('_')[1:]
#         L1itemsChainParts = [L1item, L1thresholds ]
#     elif L1itemsChainParts[0] is None:
#         L1item = 'L1_'+chainName.split('_L1')[-1]
#         L1itemsChainParts[0] = L1item

        #return [chainName, L1itemsChainParts, [PhysicsStream], groups, -1]

def setupMenu(flags):
    """ 
    Assign chains for LS2_v1 to menu flags
    """

    #---------------------------------------------------------------------
    # INPUT FORMAT FOR CHAINS:
    # ['chainName', [L1 items for chainParts], [stream], [groups], EBstep],
    #   OPTIONAL: [mergingStrategy, offset,[merginOrder] ]], topoStartsFrom = False
    #---------------------------------------------------------------------

    #---------------------------------------------------------------------
    # if it's needed to temporary remove almost all the chains from the menu
    # be aware that it is necessary to leave at least one chain in the muon slice
    # otherwise athenaHLT will seg-fault 
    #---------------------------------------------------------------------
    SingleMuonGroup = ['RATE:SingleMuon', 'BW:Muon']
    SingleElectronGroup = ['RATE:SingleElectron', 'BW:Electron']
    SinglePhotonGroup = ['RATE:SinglePhoton', 'BW:Photon']
    SingleJetGroup = ['RATE:SingleJet', 'BW:Jet']
    CombinedGroup = ['RATE:Combined', 'BW:Combined']

    flags.Trigger.menu.muon = [        
        ChainProp(name='HLT_mu20_L1MU20', groups=SingleMuonGroup),
        ChainProp(name='HLT_mu10_L1MU10', groups=SingleMuonGroup),
        ChainProp(name='HLT_mu8_L1MU6',   groups=SingleMuonGroup),

        ChainProp(name='HLT_mu20_msonly_L1MU20', groups=SingleMuonGroup),
        ChainProp(name='HLT_mu10_msonly_L1MU10', groups=SingleMuonGroup),
        ChainProp(name='HLT_mu8_msonly_L1MU6',   groups=SingleMuonGroup)
    ]

    flags.Trigger.menu.electron = [
        ChainProp(name='HLT_e3_etcut_L1EM3', groups=SingleElectronGroup),
        ChainProp(name='HLT_e5_etcut_L1EM3', groups=SingleElectronGroup),
        ChainProp(name='HLT_e7_etcut_L1EM7', groups=SingleElectronGroup)
    ]

    flags.Trigger.menu.photon = [
       ChainProp(name='HLT_g10_etcut_L1EM7',  groups=SinglePhotonGroup),
       ChainProp(name='HLT_g15_etcut_L1EM12', groups=SinglePhotonGroup),
    ]

    flags.Trigger.menu.jet = [
       ChainProp(name='HLT_j45_L1J20',  groups=SingleJetGroup),
       ChainProp(name='HLT_j85_L1J20',  groups=SingleJetGroup),
       ChainProp(name='HLT_2j35_L1J20', groups=SingleJetGroup)
    ]

    flags.Trigger.menu.combined = [
        ChainProp(name='HLT_e7_etcut_mu10_L1EM7_MU10', groups=CombinedGroup),
        ChainProp(name='HLT_e7_etcut_mu12_L1EM7_MU10', groups=CombinedGroup)
    ]

if __name__ == "__main__":
    from AthenaCommon.Configurable import Configurable
    Configurable.configurableRun3Behavior=True    

    from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
    from AthenaCommon.Constants import VERBOSE
    from AthenaCommon.Logging import logging
    log = logging.getLogger('ComponentAccumulator')
    log.setLevel(VERBOSE)

    acc = ComponentAccumulator()

    from AthenaCommon.Configurable import Configurable
    Configurable.configurableRun3Behavior=True

    from AthenaConfiguration.AllConfigFlags import ConfigFlags
    ConfigFlags.Trigger.generateMenuDiagnostics = True
    setupMenu(ConfigFlags)

    from AthenaConfiguration.TestDefaults import defaultTestFiles
    ConfigFlags.Input.Files = defaultTestFiles.RAW

    ConfigFlags.lock()
    ConfigFlags.dump()
    
    from TriggerMenuMT.HLTMenuConfig.Menu.GenerateMenuMT_newJO import generateMenu    
    menu = generateMenu( ConfigFlags )

    acc.merge(menu)

    acc.printConfig()

    # print all hypo algs and their hypo tools for debugging
    from AthenaCommon.CFElements import flatAlgorithmSequences    
    fs = flatAlgorithmSequences( menu.getSequence('HLTAllSteps') )
    for seq, algs in six.iteritems (fs):
        for alg in algs:
            if 'HypoTools' in alg._properties:
                log.verbose("%s %s", alg.name, [ t.getFullJobOptName() for t in alg.HypoTools ])

    f = open('newJOMenu.pkl', 'wb')
    acc.store(f)
    f.close()
