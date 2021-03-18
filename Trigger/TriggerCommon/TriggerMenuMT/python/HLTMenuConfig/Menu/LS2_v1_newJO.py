# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
from TriggerMenuMT.HLTMenuConfig.Menu.ChainDefInMenu import ChainProp

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

def setupMenu():
    """
    Assign chains for LS2_v1
    """
    chains = {}

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
    from TriggerMenuMT.HLTMenuConfig.Menu.Physics_pp_run3_v1 import (PhysicsStream,SingleMuonGroup,MultiMuonGroup, # noqa: F401
                                                                    SingleElectronGroup,MultiElectronGroup,SinglePhotonGroup,
                                                                    MultiPhotonGroup,SingleMETGroup,MultiMETGroup,
                                                                    SingleJetGroup,MultiJetGroup,SingleBjetGroup,SingleTauGroup,
                                                                    MultiTauGroup,BphysicsGroup,EgammaMuonGroup,MuonJetGroup,MuonMETGroup,
                                                                    EgammaJetGroup,EgammaMETGroup,EgammaTauGroup,MuonTauGroup,TauMETGroup,MinBiasGroup)


    chains["muon"] = [
        ChainProp(name='HLT_mu20_L1MU20', groups=SingleMuonGroup),
        ChainProp(name='HLT_mu10_L1MU10', groups=SingleMuonGroup),
        ChainProp(name='HLT_mu8_L1MU6',   groups=SingleMuonGroup),

        ChainProp(name='HLT_mu20_msonly_L1MU20', groups=SingleMuonGroup),
        ChainProp(name='HLT_mu10_msonly_L1MU10', groups=SingleMuonGroup),
        ChainProp(name='HLT_mu8_msonly_L1MU6',   groups=SingleMuonGroup),

        ChainProp(name='HLT_mu8_ivarmedium_L1MU6',   groups=SingleMuonGroup),
        ChainProp(name='HLT_mu6noL1_L1MU6', l1SeedThresholds=['FSNOSEED'], groups=SingleMuonGroup),
    ]
    chains["bphysics"] = [
#        ChainProp(name='HLT_2mu4_bJpsimumu_L12MU4', groups=BphysicsGroup),
    ]

    chains["electron"] = [
        ChainProp(name='HLT_e3_etcut_L1EM3', groups=SingleElectronGroup),
        ChainProp(name='HLT_2e3_etcut_L12EM3', groups=MultiElectronGroup),
# this chain does not work yet
   #     ChainProp(name='HLT_e5_etcut_e3_etcut_L12EM3', groups=MultiElectronGroup),
        ChainProp(name='HLT_e5_etcut_L1EM3', groups=SingleElectronGroup),
        ChainProp(name='HLT_e7_etcut_L1EM7', groups=SingleElectronGroup)
    ]

    chains["photon"] = [
        ChainProp(name='HLT_g10_etcut_L1EM7',  groups=SinglePhotonGroup),
        ChainProp(name='HLT_g11_etcut_L1EM7', groups=SinglePhotonGroup),
    ]

    chains["jet"] = [
        ChainProp(name='HLT_j45_L1J20',  groups=SingleJetGroup),
        ChainProp(name='HLT_j85_L1J20',  groups=SingleJetGroup),
        ChainProp(name='HLT_2j35_L1J20', groups=SingleJetGroup)
    ]

    chains["bjet"] = [
#        ChainProp(name='HLT_j275_subjesgscIS_ftf_bdl1r60_split_L1J100', groups=SingleBjetGroup),
    ]

    chains["tau"] = [
        ChainProp(name='HLT_tau0_perf_ptonly_L1TAU12', groups=SingleTauGroup),
    ]
    chains["met"] = [
        ChainProp(name='HLT_xe30_cell_L1XE10', groups=SingleMETGroup),
    ]

    chains["minbias"] = [
        ChainProp(name='HLT_mb_sptrk_L1RD0_FILLED', l1SeedThresholds=['FSNOSEED'], groups=MinBiasGroup),
    ]

    chains["combined"] = [
        ChainProp(name='HLT_e7_etcut_mu10_L1EM7_MU10', groups=EgammaMuonGroup),
        ChainProp(name='HLT_e7_etcut_mu10_msonly_L1EM7_MU10', groups=EgammaMuonGroup)
    ]



    return chains

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

    from AthenaConfiguration.TestDefaults import defaultTestFiles
    ConfigFlags.Input.Files = defaultTestFiles.RAW
    ConfigFlags.Trigger.triggerMenuSetup="LS2_v1"
    ConfigFlags.lock()
    ConfigFlags.dump()

    from TriggerMenuMT.HLTMenuConfig.Menu.GenerateMenuMT_newJO import generateMenu
    menu = generateMenu( ConfigFlags)

    acc.merge(menu)

    acc.printConfig()

    # print all hypo algs and their hypo tools for debugging
    from AthenaCommon.CFElements import flatAlgorithmSequences
    fs = flatAlgorithmSequences( menu.getSequence('HLTAllSteps') )
    for seq, algs in fs.items():
        for alg in algs:
            if 'HypoTools' in alg._properties:
                log.verbose("%s %s", alg.name, [ t.getFullJobOptName() for t in alg.HypoTools ])

    f = open('newJOMenu.pkl', 'wb')
    acc.store(f)
    f.close()
