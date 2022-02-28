# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

#------------------------------------------------------------------------#
# MC_pp_run3_v1.py menu for the long shutdown development
#------------------------------------------------------------------------#

# This defines the input format of the chain and it's properties with the defaults set
# always required are: name, stream and groups
#['name', 'L1chainParts'=[], 'stream', 'groups', 'merging'=[], 'topoStartFrom'=False],
from TriggerMenuMT.HLT.Config.Utility.ChainDefInMenu import ChainProp
from .SignatureDicts import ChainStore

import TriggerMenuMT.HLT.Menu.Physics_pp_run3_v1 as physics_menu 
from TriggerMenuMT.HLT.Menu.Physics_pp_run3_v1 import ( SingleElectronGroup,
                                                        BphysicsGroup,
                                                        EOFBPhysL1MuGroup,
                                                        SupportPhIGroup,
                                                        SupportLegGroup,
                                                        SingleBjetGroup,
                                                        SingleJetGroup,
                                                        SingleMuonGroup,
                                                        MultiMuonGroup,
                                                        BphysElectronGroup,
)


def addMCSignatures(chains):
    from AthenaCommon.Logging import logging
    log = logging.getLogger( __name__ )
    log.info('[setupMenu] going to add the MC menu chains now')

    chainsMC = ChainStore()

    chainsMC['Muon'] = [

        ChainProp(name="HLT_mu8_L1MU5VF", groups=SingleMuonGroup),
        ChainProp(name="HLT_mu10_L1MU8F", groups=SingleMuonGroup),
        ChainProp(name="HLT_mu14_L1MU8F", groups=SingleMuonGroup),
        ChainProp(name="HLT_mu14_L1MU8VFC", groups=SingleMuonGroup),
        ChainProp(name="HLT_mu24_L1MU14FCH", groups=SingleMuonGroup),
        ChainProp(name='HLT_2mu4_L12MU3V',  groups=MultiMuonGroup),

    ]

    chainsMC['Jet'] = [
        # Low-threshold calibration Large-R jets
        ChainProp(name='HLT_j85_a10sd_cssk_pf_nojcalib_ftf_preselj50_L1jLJ60', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup+SupportPhIGroup+['RATE:CPS_jLJ60']),
        ChainProp(name='HLT_j85_a10sd_cssk_pf_jes_ftf_preselj50_L1jLJ60', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup+SupportPhIGroup+['RATE:CPS_jLJ60']),
        ChainProp(name='HLT_j85_a10t_lcw_nojcalib_L1jLJ60', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup+SupportPhIGroup+['RATE:CPS_jLJ60']),
        ChainProp(name='HLT_j85_a10t_lcw_jes_L1jLJ60',      l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup+SupportPhIGroup+['RATE:CPS_jLJ60']),

        ChainProp(name='HLT_j110_a10sd_cssk_pf_jes_ftf_preselj80_L1jLJ80', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup+SupportPhIGroup+['RATE:CPS_jLJ80']),
        ChainProp(name='HLT_j110_a10t_lcw_jes_L1jLJ80', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup+SupportPhIGroup+['RATE:CPS_jLJ80']),
        ChainProp(name='HLT_j175_a10sd_cssk_pf_jes_ftf_preselj140_L1jLJ100', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup+SupportPhIGroup+['RATE:CPS_jLJ100']),
        ChainProp(name='HLT_j175_a10t_lcw_jes_L1jLJ100', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup+SupportPhIGroup+['RATE:CPS_jLJ100']),
        ChainProp(name='HLT_j260_a10sd_cssk_pf_jes_ftf_preselj200_L1jLJ120', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup+SupportPhIGroup+['RATE:CPS_jLJ120']),
        ChainProp(name='HLT_j260_a10t_lcw_jes_L1jLJ120', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup+SupportPhIGroup+['RATE:CPS_jLJ120']),
    ]

    chainsMC['Bjet'] = [
        # B-tagger training
        ChainProp(name='HLT_j20_0eta290_020jvt_pf_ftf_boffperf_L1J15', l1SeedThresholds=['FSNOSEED'], groups=SingleBjetGroup+SupportLegGroup),
        ChainProp(name='HLT_j20_0eta290_020jvt_pf_ftf_boffperf_L1jJ40', l1SeedThresholds=['FSNOSEED'], groups=SingleBjetGroup+SupportPhIGroup),
    ]

    chainsMC['Egamma'] = [
        ChainProp(name='HLT_e5_etcut_L1EM3', groups=SingleElectronGroup),
        ChainProp(name='HLT_e5_etcut_L1eEM5', groups=SingleElectronGroup),
        ChainProp(name='HLT_e26_etcut_L1EM22VHI', groups=SingleElectronGroup),
        ChainProp(name='HLT_e26_etcut_L1eEM26M', groups=SingleElectronGroup),
        ChainProp(name='HLT_e26_lhtight_L1EM22VHI', groups=SingleElectronGroup),
        ChainProp(name='HLT_e26_lhtight_L1eEM26', groups=SingleElectronGroup),
        ChainProp(name='HLT_e26_lhtight_L1eEM26L', groups=SingleElectronGroup),
        ChainProp(name='HLT_e26_lhtight_L1eEM26T', groups=SingleElectronGroup),
        ChainProp(name='HLT_e60_etcut_L1EM22VHI', groups=SingleElectronGroup),
        ChainProp(name='HLT_e60_etcut_L1eEM26M', groups=SingleElectronGroup),
        ChainProp(name='HLT_e5_lhtight_gsf_L1EM3', groups=SingleElectronGroup),
        ChainProp(name='HLT_e5_lhtight_gsf_L1eEM5', groups=SingleElectronGroup),

        #------------ dnn chains
        ChainProp(name='HLT_e5_dnnloose_L1EM3', groups=SingleElectronGroup),
        ChainProp(name='HLT_e5_dnnmedium_L1EM3', groups=SingleElectronGroup),
        ChainProp(name='HLT_e5_dnntight_L1EM3', groups=SingleElectronGroup),

        # ATR-24268, K*ee chains for rate and acceptance studies
        ChainProp(name='HLT_e5_lhvloose_bBeeM6000_L1All', l1SeedThresholds=['EM3'], stream=['BphysDelayed'], groups=BphysElectronGroup),
        ChainProp(name='HLT_2e5_lhvloose_bBeeM6000_L1All', l1SeedThresholds=['EM3'], stream=['BphysDelayed'], groups=BphysElectronGroup),
        ChainProp(name='HLT_2e5_lhvloose_bBeeM6000_L1EM22VHI', l1SeedThresholds=['EM3'], stream=['BphysDelayed'], groups=BphysElectronGroup),
        ChainProp(name='HLT_e5_lhvloose_bBeeM6000_L1EM22VHI', l1SeedThresholds=['EM3'], stream=['BphysDelayed'], groups=BphysElectronGroup),
        ChainProp(name='HLT_2e5_lhvloose_bBeeM6000_L1BPH-0M9-EM7-EM5_MU5VF', l1SeedThresholds=['EM3'], stream=['BphysDelayed'], groups=BphysElectronGroup),
        ChainProp(name='HLT_e5_lhvloose_bBeeM6000_L1BPH-0DR3-EM7J15_MU5VF', l1SeedThresholds=['EM3'], stream=['BphysDelayed'], groups=BphysElectronGroup),
        ChainProp(name='HLT_2e5_lhvloose_bBeeM6000_L14J15', l1SeedThresholds=['EM3'], stream=['BphysDelayed'], groups=BphysElectronGroup),
        ChainProp(name='HLT_e5_lhvloose_bBeeM6000_L14J15', l1SeedThresholds=['EM3'], stream=['BphysDelayed'], groups=BphysElectronGroup),        

    ]

    chainsMC['Bphysics'] = [    

        #ATR-21566, chains for di-muon TLA, but with HLT selections to test rates. Here streaming into BphysDelayed (not in TLA stream)   
        ChainProp(name='HLT_2mu4_b7invmAB22vtx20_L1BPH-7M22-2MU3VF', l1SeedThresholds=['MU3VF'],stream=['BphysDelayed'], groups=BphysicsGroup+EOFBPhysL1MuGroup),
        ChainProp(name='HLT_mu6_mu4_b7invmAB22vtx20_L1BPH-7M22-MU5VFMU3VF', l1SeedThresholds=['MU5VF','MU3VF'],stream=['BphysDelayed'], groups=BphysicsGroup+EOFBPhysL1MuGroup),
        ChainProp(name='HLT_2mu4_b0dRAB207invmAB22vtx20_L1BPH-7M22-0DR20-2MU3V', l1SeedThresholds=['MU3V'],stream=['BphysDelayed'], groups=BphysicsGroup+EOFBPhysL1MuGroup),
        ChainProp(name='HLT_2mu4_b0dRAB207invmAB22vtx20_L1BPH-7M22-0DR20-2MU3VF', l1SeedThresholds=['MU3VF'],stream=['BphysDelayed'], groups=BphysicsGroup+EOFBPhysL1MuGroup),
        ChainProp(name='HLT_2mu4_b0dRAB127invmAB22vtx20_L1BPH-7M22-0DR12-2MU3V', l1SeedThresholds=['MU3V'],stream=['BphysDelayed'], groups=BphysicsGroup+EOFBPhysL1MuGroup),

    ]

    chainsMC['Streaming'] += [
        ChainProp(name='HLT_noalg_L1All', l1SeedThresholds=['FSNOSEED'], groups=['Primary:CostAndRate', 'RATE:SeededStreamers', 'BW:Other']), # ATR-22072, for rates in MC.
    ]

    # check for chains that have the 'PS:Online' group, so that they are not simulated
    # -- does not make sense in MC menu
    for sig in chainsMC:
        for chain in chainsMC[sig]:
            if 'PS:Online' in chain.groups:
                log.error("chain %s in MC menu has the group 'PS:Online', will not be simulated!", chain.name)
                raise RuntimeError("Remove the group 'PS:Online' from the chain %s",chain.name)

    for sig in chainsMC:
        chains[sig] += chainsMC[sig]

def setupMenu():

    from AthenaCommon.Logging import logging
    log = logging.getLogger( __name__ )
    log.info('[setupMenu] going to add the MC menu chains now')
    
    chains = physics_menu.setupMenu()

    addMCSignatures(chains)

    return chains
