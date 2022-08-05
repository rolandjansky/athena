# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

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
                                                        SinglePhotonGroup,
                                                        BphysicsGroup,
                                                        EOFBPhysL1MuGroup,
                                                        EOFL1MuGroup,
                                                        EgammaJetGroup,
                                                        EgammaMETGroup,
                                                        MultiJetGroup,
                                                        PrimaryLegGroup,
                                                        PrimaryPhIGroup,
                                                        PrimaryL1MuGroup,
                                                        SupportGroup,
                                                        SupportPhIGroup,
                                                        SupportLegGroup,
                                                        SingleBjetGroup,
                                                        MultiBjetGroup,
                                                        SingleJetGroup,
                                                        SingleMuonGroup,
                                                        MultiMuonGroup,
                                                        BphysElectronGroup,
                                                        Topo2Group,
                                                        Topo3Group,
                                                        LegacyTopoGroup,
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

        #-- nscan ATR-19376, TODO: to be moved to physics once debugged
        ChainProp(name='HLT_mu20_msonly_iloosems_mu6noL1_msonly_nscan_L1MU14FCH_J40', l1SeedThresholds=['MU14FCH','FSNOSEED'], groups=PrimaryLegGroup+MultiMuonGroup),
        ChainProp(name='HLT_mu20_msonly_iloosems_mu6noL1_msonly_nscan_L1MU14FCH_jJ80', l1SeedThresholds=['MU14FCH','FSNOSEED'], groups=PrimaryPhIGroup+MultiMuonGroup),
        ChainProp(name='HLT_mu20_msonly_iloosems_mu6noL1_msonly_nscan_L1MU14FCH_XE30', l1SeedThresholds=['MU14FCH','FSNOSEED'], groups=PrimaryLegGroup+MultiMuonGroup),
        ChainProp(name='HLT_mu20_msonly_iloosems_mu6noL1_msonly_nscan_L1MU14FCH_jXE70', l1SeedThresholds=['MU14FCH','FSNOSEED'], groups=PrimaryPhIGroup+MultiMuonGroup),
        ChainProp(name='HLT_mu20_msonly_iloosems_mu6noL1_msonly_nscan_L110DR-MU14FCH-MU5VF', l1SeedThresholds=['MU14FCH','FSNOSEED'],   groups=PrimaryL1MuGroup+MultiMuonGroup+Topo2Group),
        ## ATR-24198
        ChainProp(name='HLT_mu20_msonly_iloosems_mu6noL1_msonly_nscan_L1MU14FCH', l1SeedThresholds=['MU14FCH','FSNOSEED'], groups=SupportGroup+MultiMuonGroup),

        ## ATR-25456 - dimuon TLA with L1TOPO
        ChainProp(name='HLT_2mu4_PhysicsTLA_L1BPH-7M22-2MU3VF', l1SeedThresholds=['MU3VF'],stream=['TLA'], groups=MultiMuonGroup+EOFL1MuGroup+Topo3Group),
        ChainProp(name='HLT_2mu4_PhysicsTLA_L1BPH-7M22-0DR20-2MU3VF', l1SeedThresholds=['MU3VF'],stream=['TLA'], groups=MultiMuonGroup+EOFL1MuGroup+Topo3Group),
        ChainProp(name='HLT_2mu4_PhysicsTLA_L1BPH-7M22-0DR12-2MU3V', l1SeedThresholds=['MU3V'],stream=['TLA'], groups=MultiMuonGroup+EOFL1MuGroup+Topo3Group),

        ## ATR-25456 - 4mu
        ChainProp(name='HLT_2mu4_L1BPH-7M11-25DR99-2MU3VF', l1SeedThresholds=['MU3VF'], stream=["BphysDelayed"], groups=BphysicsGroup+EOFL1MuGroup+Topo2Group),
        ChainProp(name='HLT_mu6_mu4_L1BPH-7M14-MU5VFMU3VF', l1SeedThresholds=['MU5VF','MU3VF'], stream=["BphysDelayed"], groups=BphysicsGroup+EOFL1MuGroup+Topo3Group),
        ChainProp(name='HLT_2mu4_L1BPH-7M14-2MU3V', l1SeedThresholds=['MU3V'], stream=["BphysDelayed"], groups=BphysicsGroup+EOFL1MuGroup+Topo3Group),
        ChainProp(name='HLT_2mu4_L1BPH-7M14-2MU3VF', l1SeedThresholds=['MU3VF'], stream=["BphysDelayed"], groups=BphysicsGroup+EOFL1MuGroup+Topo3Group),

        ## ATR-25456 - 4mu L1 with DR, for optimization
        ChainProp(name='HLT_2mu4_L1BPH-7M14-0DR25-MU5VFMU3VF', l1SeedThresholds=['MU3VF'], stream=["BphysDelayed"], groups=MultiMuonGroup+Topo3Group),
    ]

    chainsMC['Jet'] = [

        # low threshold single jet support chains with JVT
        ChainProp(name='HLT_j45_020jvt_pf_ftf_preselj20_L1J15', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup+SupportLegGroup),

        ChainProp(name="HLT_j20_roiftf_preselj20_L1RD0_FILLED", l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup+SupportGroup+['RATE:CPS_RD0_FILLED']),

        # Low-threshold calibration Large-R jets
        # Non-commissioning L1 thresholds
        ChainProp(name='HLT_j85_a10sd_cssk_pf_nojcalib_ftf_preselj50_L1jLJ60', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup+SupportPhIGroup+['RATE:CPS_jLJ60']),
        ChainProp(name='HLT_j85_a10sd_cssk_pf_jes_ftf_preselj50_L1jLJ60', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup+SupportPhIGroup+['RATE:CPS_jLJ60']),
        ChainProp(name='HLT_j85_a10t_lcw_nojcalib_L1jLJ60', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup+SupportPhIGroup+['RATE:CPS_jLJ60']),
        ChainProp(name='HLT_j85_a10t_lcw_jes_L1jLJ60',      l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup+SupportPhIGroup+['RATE:CPS_jLJ60']),
        ChainProp(name='HLT_j175_a10sd_cssk_pf_jes_ftf_preselj140_L1jLJ100', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup+SupportPhIGroup+['RATE:CPS_jLJ100']),
        ChainProp(name='HLT_j175_a10t_lcw_jes_L1jLJ100', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup+SupportPhIGroup+['RATE:CPS_jLJ100']),

        ## ATR-25456 - Emerging jet
        # alternate emerging jet single-jet chain
        ChainProp(name='HLT_j200_0eta200_tracklessdR1p2_a10r_subjesIS_ftf_preselj200_L1J100', groups=SingleJetGroup+PrimaryLegGroup, l1SeedThresholds=['FSNOSEED']),    
        # backup Trackless jet chain
        ChainProp(name='HLT_j260_0eta200_tracklessdR1p2_a10r_subjesIS_ftf_preselj200_L1J100', groups=SingleJetGroup+PrimaryLegGroup, l1SeedThresholds=['FSNOSEED']),    

        ## ATR-25456 - calratio jet chains
        ChainProp(name='HLT_j30_CLEANllp_calratio_L1TAU100', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup+PrimaryLegGroup),
        ChainProp(name='HLT_j30_CLEANllp_calratiormbib_L1TAU100', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup+PrimaryLegGroup),
        ChainProp(name='HLT_j30_CLEANllp_calratio_L1TAU60', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup+SupportGroup),
        ChainProp(name='HLT_j30_CLEANllp_calratiormbib_L1TAU60', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup+SupportGroup),
        ChainProp(name='HLT_j30_CLEANllp_calratio_L1LLP-NOMATCH', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup+PrimaryLegGroup+LegacyTopoGroup),
        ChainProp(name='HLT_j30_CLEANllp_calratiormbib_L1LLP-NOMATCH', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup+PrimaryLegGroup+LegacyTopoGroup),
        # supporting chains for background studies
        ChainProp(name='HLT_j30_CLEANllp_calratio_L1TAU40_EMPTY', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup+SupportLegGroup),
        ChainProp(name='HLT_j30_CLEANllp_calratiormbib_L1TAU40_EMPTY', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup+SupportLegGroup),
        ChainProp(name='HLT_j30_CLEANllp_calratio_L1TAU40_UNPAIRED_ISO', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup+SupportLegGroup),
        ChainProp(name='HLT_j30_CLEANllp_calratiormbib_L1TAU40_UNPAIRED_ISO', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup+SupportLegGroup),

        ## ATR-25456 - Central Exclusive Production for SM group
        ChainProp(name='HLT_2j100_L1CEP-CjJ90', l1SeedThresholds=['FSNOSEED'], groups=PrimaryPhIGroup+MultiJetGroup+Topo3Group),
        ChainProp(name='HLT_2j100_L1CEP-CjJ100', l1SeedThresholds=['FSNOSEED'], groups=PrimaryPhIGroup+MultiJetGroup+Topo3Group),

    ]

    chainsMC['Bjet'] = [
        # B-tagger training
        ChainProp(name='HLT_j20_0eta290_020jvt_boffperf_pf_ftf_L1J15', l1SeedThresholds=['FSNOSEED'], groups=SingleBjetGroup+SupportLegGroup, monGroups=['bJetMon:online']),
        ChainProp(name='HLT_j20_0eta290_020jvt_boffperf_pf_ftf_L1jJ40', l1SeedThresholds=['FSNOSEED'], groups=SingleBjetGroup+SupportPhIGroup, monGroups=['bJetMon:online']),
        # HH4b
        # Muon seeded candidates
        ChainProp(name='HLT_j80c_020jvt_j55c_020jvt_j28c_020jvt_j20c_020jvt_SHARED_3j20c_020jvt_bdl1d82_pf_ftf_presel2c20XX2c20b85_L1MU8F_2J15_J20', l1SeedThresholds=['FSNOSEED']*5, groups=PrimaryLegGroup+MultiBjetGroup),
        ChainProp(name='HLT_j80c_020jvt_j55c_020jvt_j28c_020jvt_j20c_020jvt_SHARED_3j20c_020jvt_bdl1d85bb82_pf_ftf_presel2c20XX2c20b85_L1MU8F_2J15_J20', l1SeedThresholds=['FSNOSEED']*5, groups=PrimaryLegGroup+MultiBjetGroup),
        # Background studies
        ChainProp(name='HLT_j60c_020jvt_j40c_020jvt_j25c_020jvt_j20c_020jvt_SHARED_3j20c_020jvt_bdl1d85bb82_pf_ftf_presel2c20XX2c20b85_L1MU8F_2J15_J20', l1SeedThresholds=['FSNOSEED']*5, groups=SupportLegGroup+MultiBjetGroup),
        ChainProp(name='HLT_j60c_020jvt_j40c_020jvt_j25c_020jvt_j20c_020jvt_SHARED_3j20c_020jvt_bdl1d85bb82_pf_ftf_presel2c20XX2c20b85_L1J45p0ETA21_3J15p0ETA25', l1SeedThresholds=['FSNOSEED']*5, groups=SupportLegGroup+MultiBjetGroup),
        ChainProp(name='HLT_j60c_020jvt_j40c_020jvt_j25c_020jvt_j20c_020jvt_SHARED_3j20c_020jvt_bdl1d85bb77_pf_ftf_presel2c20XX2c20b85_L1MU8F_2J15_J20', l1SeedThresholds=['FSNOSEED']*5, groups=SupportLegGroup+MultiBjetGroup),
        ChainProp(name='HLT_j60c_020jvt_j40c_020jvt_j25c_020jvt_j20c_020jvt_SHARED_3j20c_020jvt_bdl1d85bb77_pf_ftf_presel2c20XX2c20b85_L1J45p0ETA21_3J15p0ETA25', l1SeedThresholds=['FSNOSEED']*5, groups=SupportLegGroup+MultiBjetGroup),
    ]

    chainsMC['Egamma'] = [
        ChainProp(name='HLT_e5_lhtight_L1EM3', groups=SingleElectronGroup),
        ChainProp(name='HLT_e5_lhtight_noringer_L1EM3', groups=SingleElectronGroup),

        #------------ dnn chains
        ChainProp(name='HLT_e5_dnnloose_L1EM3', groups=SingleElectronGroup),
        ChainProp(name='HLT_e5_dnnmedium_L1EM3', groups=SingleElectronGroup),
        ChainProp(name='HLT_e5_dnntight_L1EM3', groups=SingleElectronGroup),

        # ATR-24268, K*ee chains for rate and acceptance studies
        ChainProp(name='HLT_e5_lhvloose_bBeeM6000_L1All', l1SeedThresholds=['EM3'], stream=['BphysDelayed'], groups=BphysElectronGroup),
        ChainProp(name='HLT_2e5_lhvloose_bBeeM6000_L1All', l1SeedThresholds=['EM3'], stream=['BphysDelayed'], groups=BphysElectronGroup),
        ChainProp(name='HLT_2e5_lhvloose_bBeeM6000_L1EM22VHI', l1SeedThresholds=['EM3'], stream=['BphysDelayed'], groups=BphysElectronGroup),
        ChainProp(name='HLT_e5_lhvloose_bBeeM6000_L1EM22VHI', l1SeedThresholds=['EM3'], stream=['BphysDelayed'], groups=BphysElectronGroup),
        ChainProp(name='HLT_2e5_lhvloose_bBeeM6000_L1BPH-0M9-EM7-EM5_MU5VF', l1SeedThresholds=['EM3'], stream=['BphysDelayed'], groups=BphysElectronGroup+LegacyTopoGroup),
        ChainProp(name='HLT_e5_lhvloose_bBeeM6000_L1BPH-0DR3-EM7J15_MU5VF', l1SeedThresholds=['EM3'], stream=['BphysDelayed'], groups=BphysElectronGroup+LegacyTopoGroup),
        ChainProp(name='HLT_2e5_lhvloose_bBeeM6000_L14J15', l1SeedThresholds=['EM3'], stream=['BphysDelayed'], groups=BphysElectronGroup),
        ChainProp(name='HLT_e5_lhvloose_bBeeM6000_L14J15', l1SeedThresholds=['EM3'], stream=['BphysDelayed'], groups=BphysElectronGroup),        

        #------------ hipTRT trigger, ATR-22603
        ChainProp(name='HLT_g0_hiptrt_L1EM22VHI', groups=SinglePhotonGroup), 
        ChainProp(name='HLT_g0_hiptrt_L1eEM26M' , groups=SinglePhotonGroup), 

        #ATR-25764 - adding Photon chains with different isolation WPs
        ChainProp(name='HLT_g25_tight_icaloloose_L1EM22VHI', groups=PrimaryLegGroup+EgammaJetGroup, monGroups=['egammaMon:shifter']),
        ChainProp(name='HLT_g25_tight_icalotight_L1EM22VHI', groups=PrimaryLegGroup+EgammaJetGroup, monGroups=['egammaMon:shifter']),
    ]

    chainsMC['Bphysics'] = [    

        #ATR-21566, chains for di-muon TLA, but with HLT selections to test rates. Here streaming into BphysDelayed (not in TLA stream)   
        ChainProp(name='HLT_2mu4_b7invmAB22vtx20_L1BPH-7M22-2MU3VF', l1SeedThresholds=['MU3VF'],stream=['BphysDelayed'], groups=BphysicsGroup+EOFBPhysL1MuGroup+Topo3Group),
        ChainProp(name='HLT_mu6_mu4_b7invmAB22vtx20_L1BPH-7M22-MU5VFMU3VF', l1SeedThresholds=['MU5VF','MU3VF'],stream=['BphysDelayed'], groups=BphysicsGroup+EOFBPhysL1MuGroup+Topo3Group),
        ChainProp(name='HLT_2mu4_b0dRAB207invmAB22vtx20_L1BPH-7M22-0DR20-2MU3V', l1SeedThresholds=['MU3V'],stream=['BphysDelayed'], groups=BphysicsGroup+EOFBPhysL1MuGroup+Topo3Group),
        ChainProp(name='HLT_2mu4_b0dRAB207invmAB22vtx20_L1BPH-7M22-0DR20-2MU3VF', l1SeedThresholds=['MU3VF'],stream=['BphysDelayed'], groups=BphysicsGroup+EOFBPhysL1MuGroup+Topo3Group),
        ChainProp(name='HLT_2mu4_b0dRAB127invmAB22vtx20_L1BPH-7M22-0DR12-2MU3V', l1SeedThresholds=['MU3V'],stream=['BphysDelayed'], groups=BphysicsGroup+EOFBPhysL1MuGroup+Topo3Group),
    ]

    chainsMC['Streaming'] += [
        ChainProp(name='HLT_noalg_L1All', l1SeedThresholds=['FSNOSEED'], groups=['Primary:CostAndRate', 'RATE:SeededStreamers', 'BW:Other']), # ATR-22072, for rates in MC.
    ]

    chainsMC['Combined'] += [

        ## ATR-25456 - Photon+MET reoptimised
        ChainProp(name='HLT_g25_tight_icalotight_xe40_cell_xe50_tcpufit_80mTAC_L1EM22VHI',l1SeedThresholds=['EM22VHI']+2*['FSNOSEED'], groups=PrimaryLegGroup+EgammaMETGroup),
        ChainProp(name='HLT_g25_tight_icalotight_xe40_cell_xe40_tcpufit_xe40_pfopufit_80mTAC_L1EM22VHI',l1SeedThresholds=['EM22VHI']+3*['FSNOSEED'], groups=PrimaryLegGroup+EgammaMETGroup),
        ChainProp(name='HLT_g25_tight_icalotight_xe40_cell_xe50_tcpufit_80mTAC_L1eEM26M',l1SeedThresholds=['eEM26M']+2*['FSNOSEED'], groups=PrimaryPhIGroup+EgammaMETGroup),
        ChainProp(name='HLT_g25_tight_icalotight_xe40_cell_xe40_tcpufit_xe40_pfopufit_80mTAC_L1eEM26M',l1SeedThresholds=['eEM26M']+3*['FSNOSEED'], groups=PrimaryPhIGroup+EgammaMETGroup),

        ## ATR-25456 - B/D to e+mu triggers
        ChainProp(name='HLT_e14_lhtight_mu6_dRAB15_invmAB10_L1LFV-eEM15L-MU5VF', l1SeedThresholds=['eEM12L','MU5VF'], groups=PrimaryPhIGroup+BphysicsGroup+Topo3Group),
        ChainProp(name='HLT_e12_lhtight_mu11_dRAB15_invmAB10_L1LFV-eEM10L-MU8VF', l1SeedThresholds=['eEM10L','MU8VF'], groups=PrimaryPhIGroup+BphysicsGroup+Topo3Group),

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
