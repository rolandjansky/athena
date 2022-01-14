# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
from TriggerMenuMT.HLT.Menu.ChainDefInMenu import ChainProp

def setupMenu():
    """
    Assign chains for Dev_pp_run3_v1
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
    from TriggerMenuMT.HLT.Menu.Physics_pp_run3_v1 import (PhysicsStream,SingleMuonGroup,MultiMuonGroup, # noqa: F401
                                                                    SingleElectronGroup,MultiElectronGroup,SinglePhotonGroup,
                                                                    MultiPhotonGroup,SingleMETGroup,MultiMETGroup,
                                                                    SingleJetGroup,MultiJetGroup,SingleBjetGroup,SingleTauGroup,
                                                                    MultiTauGroup,BphysicsGroup,EgammaMuonGroup,MuonJetGroup,MuonMETGroup,
                                                                    EgammaJetGroup,EgammaMETGroup,EgammaTauGroup,MuonTauGroup,TauMETGroup,MinBiasGroup)


    chains["muon"] = [
        ChainProp(name='HLT_mu20_L1MU14FCH', groups=SingleMuonGroup),
        ChainProp(name='HLT_mu10_L1MU8F', groups=SingleMuonGroup),
        ChainProp(name='HLT_mu8_L1MU5VF',   groups=SingleMuonGroup),

        ChainProp(name='HLT_mu20_msonly_L1MU14FCH', groups=SingleMuonGroup),
        ChainProp(name='HLT_mu10_msonly_L1MU8F', groups=SingleMuonGroup),
        ChainProp(name='HLT_mu8_msonly_L1MU5VF',   groups=SingleMuonGroup),

        ChainProp(name='HLT_mu8_ivarmedium_L1MU5VF',   groups=SingleMuonGroup),
        ChainProp(name='HLT_mu6noL1_L1MU5VF', l1SeedThresholds=['FSNOSEED'], groups=SingleMuonGroup),
    ]
    chains["bphysics"] = [
#        ChainProp(name='HLT_2mu4_bJpsimumu_L12MU3V', groups=BphysicsGroup),
    ]

    chains["electron"] = [
        ChainProp(name='HLT_e3_etcut_L1EM3', groups=SingleElectronGroup),
        ChainProp(name='HLT_2e3_etcut_L12EM3', groups=MultiElectronGroup),
# this chain does not work yet
   #     ChainProp(name='HLT_e5_etcut_e3_etcut_L12EM3', groups=MultiElectronGroup),
        ChainProp(name='HLT_e5_etcut_L1EM3', groups=SingleElectronGroup),
        ChainProp(name='HLT_e7_etcut_L1EM7', groups=SingleElectronGroup),
        ChainProp(name='HLT_e7_lhvloose_L1EM7', groups=SingleElectronGroup)
    ]

    chains["photon"] = [
        ChainProp(name='HLT_g10_etcut_L1EM7',  groups=SinglePhotonGroup),
        ChainProp(name='HLT_g11_etcut_L1EM7', groups=SinglePhotonGroup),
    ]

    chains["jet"] = [
        ChainProp(name='HLT_j45_L1J20', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup),
        ChainProp(name='HLT_j85_L1J20', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup),
        ChainProp(name='HLT_2j35_L1J20', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup)
    ]

    chains["bjet"] = [
#        ChainProp(name='HLT_j275_subjesgscIS_ftf_bdl1r60_split_L1J100', groups=SingleBjetGroup),
    ]

    chains["tau"] = [
        ChainProp(name='HLT_tau0_perf_ptonly_L1TAU8', groups=SingleTauGroup),
        ChainProp(name='HLT_tau25_mediumRNN_tracktwoMVA_L1TAU12IM', groups=SingleTauGroup),
        ChainProp(name='HLT_tau25_mediumRNN_tracktwoMVABDT_L1TAU12IM', groups=SingleTauGroup),
    ]
    chains["met"] = [
        ChainProp(name='HLT_xe30_tcpufit_L1XE30', l1SeedThresholds=['FSNOSEED'], groups=SingleMETGroup),
        ChainProp(name='HLT_xe30_trkmht_L1XE30', l1SeedThresholds=['FSNOSEED'], groups=SingleMETGroup),
        ChainProp(name='HLT_xe30_pfsum_L1XE30', l1SeedThresholds=['FSNOSEED'], groups=SingleMETGroup),
        ChainProp(name='HLT_xe30_pfsum_cssk_L1XE30', l1SeedThresholds=['FSNOSEED'], groups=SingleMETGroup),
        ChainProp(name='HLT_xe30_pfsum_vssk_L1XE30', l1SeedThresholds=['FSNOSEED'], groups=SingleMETGroup),
        ChainProp(name='HLT_xe30_pfopufit_L1XE30', l1SeedThresholds=['FSNOSEED'], groups=SingleMETGroup),
        ChainProp(name='HLT_xe30_mhtpufit_em_subjesgscIS_L1XE30', l1SeedThresholds=['FSNOSEED'], groups=SingleMETGroup),
        ChainProp(name='HLT_xe30_mhtpufit_pf_subjesgscIS_L1XE30', l1SeedThresholds=['FSNOSEED'], groups=SingleMETGroup),
        ChainProp(name='HLT_xe30_cell_L1XE30', l1SeedThresholds=['FSNOSEED'], groups=SingleMETGroup),
    ]

    chains["minbias"] = [
        ChainProp(name='HLT_mb_sptrk_L1RD0_FILLED', l1SeedThresholds=['FSNOSEED'], groups=MinBiasGroup),
    ]

    chains["combined"] = [
        ChainProp(name='HLT_e7_etcut_mu10_L1EM7_MU8F', groups=EgammaMuonGroup),
        ChainProp(name='HLT_e7_etcut_mu10_msonly_L1EM7_MU8F', groups=EgammaMuonGroup)
    ]



    return chains

if __name__ == "__main__":
    from AthenaCommon.Configurable import Configurable
    Configurable.configurableRun3Behavior=True

    from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
    from AthenaConfiguration.AccumulatorCache import AccumulatorDecorator
    from AthenaCommon.Logging import logging
    log = logging.getLogger(__name__)

    acc = ComponentAccumulator()

    from AthenaCommon.Configurable import Configurable
    Configurable.configurableRun3Behavior=True

    from AthenaConfiguration.AllConfigFlags import ConfigFlags
    ConfigFlags.Trigger.generateMenuDiagnostics = True

    from AthenaConfiguration.TestDefaults import defaultTestFiles
    ConfigFlags.Input.Files = defaultTestFiles.RAW
    ConfigFlags.Trigger.triggerMenuSetup="Dev_pp_run3_v1"
    ConfigFlags.lock()
    ConfigFlags.dump()

    from TriggerMenuMT.HLT.Menu.GenerateMenuMT_newJO import generateMenu
    menu = generateMenu( ConfigFlags)

    acc.merge(menu)

    acc.printConfig()
    AccumulatorDecorator.printStats()

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
