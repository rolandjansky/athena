# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

#------------------------------------------------------------------------#
# LS2_v1.py menu for the long shutdown development
#------------------------------------------------------------------------#

# This defines the input format of the chain and it's properties with the defaults set
# always required are: name, stream and groups
#['name', 'L1chainParts'=[], 'stream', 'groups', 'merging'=[], 'topoStartFrom'=False],

from TriggerMenuMT.HLTMenuConfig.Menu.ChainDefInMenu import ChainProp
from TriggerMenuMT.HLTMenuConfig.Menu.MenuPrescaleConfig import addSliceChainsToPrescales

import TriggerMenuMT.HLTMenuConfig.Menu.MC_pp_run3_v1 as mc_menu
import TriggerMenuMT.HLTMenuConfig.Menu.PhysicsP1_pp_run3_v1 as p1_menu

from TriggerMenuMT.HLTMenuConfig.Menu.Physics_pp_run3_v1 import PhysicsStream,SingleMuonGroup,MultiMuonGroup,SingleElectronGroup,MultiElectronGroup,SinglePhotonGroup,MultiPhotonGroup,SingleMETGroup,MultiMETGroup,SingleJetGroup,MultiJetGroup,SingleBjetGroup,SingleTauGroup,MinBiasGroup
#BphysicsGroup (re-add to the preceding line when putting bphys back in)


def setupMenu():

    mc_menu.setupMenu()
    p1_menu.addP1Signatures()

    from TriggerJobOpts.TriggerFlags          import TriggerFlags
    from AthenaCommon.Logging                 import logging
    log = logging.getLogger( __name__ )
    log.info('setupMenu ...')


    TriggerFlags.TestSlice.signatures = TriggerFlags.TestSlice.signatures() + []

    TriggerFlags.MuonSlice.signatures = TriggerFlags.MuonSlice.signatures() + [
        #ART-19985
        ChainProp(name='HLT_mu6_idperf_L1MU6', groups=SingleMuonGroup),
        ChainProp(name='HLT_mu24_idperf_L1MU20', groups=SingleMuonGroup),        
        ChainProp(name='HLT_mu6_mu6noL1_L1MU6', l1SeedThresholds=['MU6','FSNOSEED'], mergingStrategy='serial', groups=MultiMuonGroup),

        #ATR-20049
        ChainProp(name='HLT_mu6fast_L1MU6', groups=SingleMuonGroup),
        ChainProp(name='HLT_mu6Comb_L1MU6', groups=SingleMuonGroup),
        ChainProp(name='HLT_mu6_L1MU6',     groups=SingleMuonGroup),

        ChainProp(name='HLT_mu20_ivar_L1MU6',      groups=SingleMuonGroup),
        ChainProp(name='HLT_mu6_ivarmedium_L1MU6', groups=SingleMuonGroup),

        # commented because it is conflict with dimuon noL1 serial chain
        # ChainProp(name='HLT_mu6noL1_L1MU6', l1SeedThresholds=['FSNOSEED'], groups=SingleMuonGroup),
        
        ChainProp(name='HLT_mu6_msonly_L1MU6',     groups=SingleMuonGroup),

        ChainProp(name='HLT_2mu6_10invm70_L1MU6', groups=SingleMuonGroup),
        ChainProp(name='HLT_mu10_lateMu_L1MU10', l1SeedThresholds=['FSNOSEED'], groups=SingleMuonGroup),

        # this is for test only
        ChainProp(name='HLT_2mu6_Dr_L12MU4',  groups=MultiMuonGroup),
         #  ChainProp(name='HLT_mu6_Dr_mu4_Dr_L12MU4', l1SeedThresholds=['MU4']*2, groups=MultiMuonGroup),
        # ATR-20049
        ChainProp(name='HLT_mu6_mu4_L12MU4',  l1SeedThresholds=['MU4']*2, groups=MultiMuonGroup),

        # Additional intermediate thresholds for validation comparisons
        ChainProp(name='HLT_mu28_ivarmedium_L1MU20', groups=SingleMuonGroup),
        ChainProp(name='HLT_mu35_ivarmedium_L1MU20', groups=SingleMuonGroup),
        ChainProp(name='HLT_2mu15_L12MU10', groups=SingleMuonGroup),
        ChainProp(name='HLT_3mu8_msonly_L13MU6', groups=SingleMuonGroup),

       # ATR-19452
        ChainProp(name='HLT_2mu4_muonqual_L12MU4',  groups=MultiMuonGroup),
        ChainProp(name='HLT_2mu6_muonqual_L12MU6',  groups=MultiMuonGroup),

       # ATR-20650
        ChainProp(name='HLT_mu0_muoncalib_L1MU4_EMPTY', groups=SingleMuonGroup),
        ChainProp(name='HLT_mu0_muoncalib_L1MU20',      groups=SingleMuonGroup),

     ]

    TriggerFlags.EgammaSlice.signatures = TriggerFlags.EgammaSlice.signatures() + [
        # ElectronChains----------
        ChainProp(name='HLT_e3_etcut_L1EM3', groups=SingleElectronGroup),
        ChainProp(name='HLT_e5_etcut_L1EM3', groups=SingleElectronGroup),
        ChainProp(name='HLT_e7_etcut_L1EM3', stream=[PhysicsStream, 'express'], groups=SingleElectronGroup),

        ChainProp(name='HLT_2e3_etcut_L12EM3', stream=[PhysicsStream], groups=MultiElectronGroup),

        # low et threshold for debugging
        ChainProp(name='HLT_e5_lhloose_noringer_L1EM3', groups=SingleElectronGroup),
        ChainProp(name='HLT_e5_lhmedium_noringer_L1EM3', groups=SingleElectronGroup),
        ChainProp(name='HLT_e5_lhtight_noringer_L1EM3', groups=SingleElectronGroup),
        ChainProp(name='HLT_e5_lhtight_noringer_nod0_L1EM3', groups=SingleElectronGroup),

        # Primary
        ChainProp(name='HLT_e26_lhtight_L1EM22VHI', groups=SingleElectronGroup),
        ChainProp(name='HLT_e26_lhtight_nod0_L1EM22VHI', groups=SingleElectronGroup),
        ChainProp(name='HLT_e26_lhtight_nod0_L1EM24VHI', groups=SingleElectronGroup),
        ChainProp(name='HLT_e60_lhmedium_L1EM22VHI', groups=SingleElectronGroup),
        ChainProp(name='HLT_e60_lhmedium_nod0_L1EM22VHI', groups=SingleElectronGroup),
        ChainProp(name='HLT_e140_lhloose_L1EM22VHI', groups=SingleElectronGroup),
        ChainProp(name='HLT_e140_lhloose_nod0_L1EM22VHI', groups=SingleElectronGroup),
        ChainProp(name='HLT_e300_etcut_L1EM22VHI', groups=SingleElectronGroup),

        ChainProp(name='HLT_2e17_lhvloose_L12EM3', stream=[PhysicsStream], groups=MultiElectronGroup),
        ChainProp(name='HLT_2e17_lhvloose_L12EM15VH', stream=[PhysicsStream], groups=MultiElectronGroup),
        ChainProp(name='HLT_e17_lhvloose_nod0_L1EM15VH', stream=[PhysicsStream], groups=SingleElectronGroup),

        # for moving to PhysicsP1, ATR-21242
        # ChainProp(name='HLT_2e17_etcut_L12EM15VHI', stream=[PhysicsStream], groups=MultiElectronGroup),

        # PhotonChains------------
        # these are to debug photon working points should be removed in production
        ChainProp(name='HLT_g5_etcut_L1EM3', groups=SinglePhotonGroup),
        ChainProp(name='HLT_g5_loose_L1EM3', groups=SinglePhotonGroup),
        ChainProp(name='HLT_g5_medium_L1EM3', groups=SinglePhotonGroup),
        ChainProp(name='HLT_g5_tight_L1EM3', groups=SinglePhotonGroup),
        # Primary photon chains
        ChainProp(name='HLT_g140_loose_L1EM22VHI', groups=SinglePhotonGroup),
        ChainProp(name='HLT_2g35_medium_L12EM20VH', groups=MultiPhotonGroup),
        ChainProp(name='HLT_g35_medium_g25_medium_L12EM20VH', groups=MultiPhotonGroup),
        ChainProp(name='HLT_2g20_tight_L12EM20VH', groups=MultiPhotonGroup),

        # ATR-19360
        ChainProp(name='HLT_g5_etcut_LArPEB_L1EM3',stream=['LArCells'], groups=SinglePhotonGroup),

        # for moving to PhysicsP1, ATR-21242
        ChainProp(name='HLT_g140_etcut_L1EM22VHI', groups=SinglePhotonGroup),
    ]

    TriggerFlags.METSlice.signatures = TriggerFlags.METSlice.signatures() + [
        ChainProp(name='HLT_xe30_cell_L1XE10', groups=SingleMETGroup),
        ChainProp(name='HLT_xe30_mht_L1XE10', groups=SingleMETGroup),
        ChainProp(name='HLT_xe30_tcpufit_L1XE10', groups=SingleMETGroup),
        ChainProp(name='HLT_xe30_trkmht_L1XE10', groups=SingleMETGroup),
        ChainProp(name='HLT_xe30_pfsum_L1XE10', groups=SingleMETGroup),

        ChainProp(name='HLT_xe110_tc_em_L1XE50', groups=SingleMETGroup),
        ChainProp(name='HLT_xe110_mht_L1XE50', groups=SingleMETGroup),
        ChainProp(name='HLT_xe110_tcpufit_L1XE50', groups=SingleMETGroup),
        # MultiMET Chain
        ChainProp(name='HLT_xe30_cell_xe30_tcpufit_L1XE10',l1SeedThresholds=['XE10']*2, groups=MultiMETGroup), #must be FS seeded
    ]

    TriggerFlags.JetSlice.signatures = TriggerFlags.JetSlice.signatures() + [
        ChainProp(name='HLT_j85_L1J20', groups=SingleJetGroup),
        ChainProp(name='HLT_j45_L1J15', groups=SingleJetGroup),
        ChainProp(name='HLT_j420_L1J20', groups=SingleJetGroup),
        ChainProp(name='HLT_j45_ftf_subjesgscIS_L1J15', groups=SingleJetGroup),
        ChainProp(name='HLT_j45_ftf_subresjesgscIS_L1J15', groups=SingleJetGroup),
        ChainProp(name='HLT_j45_ftf_subjesgscIS_011jvt_L1J15', groups=SingleJetGroup),
        ChainProp(name='HLT_j45_ftf_subjesgscIS_015jvt_L1J15', groups=SingleJetGroup),
        ChainProp(name='HLT_j45_ftf_subjesgscIS_059jvt_L1J15', groups=SingleJetGroup),
        ChainProp(name='HLT_j45_ftf_L1J15', groups=SingleJetGroup),
        ChainProp(name='HLT_j85_ftf_L1J20', groups=SingleJetGroup),


        ChainProp(name='HLT_j45_ftf_pf_L1J20', groups=SingleJetGroup),
        ChainProp(name='HLT_j45_ftf_subjesgscIS_pf_L1J20', groups=SingleJetGroup),
        ChainProp(name='HLT_j45_ftf_subresjesgscIS_pf_L1J20', groups=SingleJetGroup),
        ChainProp(name='HLT_j85_ftf_pf_L1J20', groups=SingleJetGroup),

        ChainProp(name='HLT_j420_ftf_subjesgscIS_L1J20', groups=SingleJetGroup),

        ChainProp(name='HLT_j45_nojcalib_L1J20', groups=SingleJetGroup),
        ChainProp(name='HLT_j45_sktc_nojcalib_L1J20', groups=SingleJetGroup),
        ChainProp(name='HLT_j45_cssktc_nojcalib_L1J20', groups=SingleJetGroup),
        ChainProp(name='HLT_j45_ftf_pf_nojcalib_L1J20', groups=SingleJetGroup),
        ChainProp(name='HLT_j45_ftf_csskpf_nojcalib_L1J20', groups=SingleJetGroup),

        ChainProp(name='HLT_j260_320eta490_L1J20', groups=SingleJetGroup),

        ChainProp(name='HLT_j460_a10_lcw_subjes_L1J20', groups=SingleJetGroup),
        ChainProp(name='HLT_j460_a10r_L1J20', groups=SingleJetGroup),

        ChainProp(name='HLT_3j200_L1J20', groups=MultiJetGroup),
        ChainProp(name='HLT_j0_vbenfSEP30etSEP34mass35SEP50fbet_L1J20', groups=SingleJetGroup),

        # ATR-20624
        ChainProp(name='HLT_j0_perf_L1J12_EMPTY', stream=['Main'], groups=SingleJetGroup),
    ]

    TriggerFlags.BjetSlice.signatures = TriggerFlags.BjetSlice.signatures() + [
        ChainProp(name="HLT_j45_ftf_subjesgscIS_boffperf_split_L1J20", groups=SingleBjetGroup),
        ChainProp(name="HLT_j45_ftf_subjesgscIS_bmv2c1070_split_L1J20", groups=SingleBjetGroup),
    ]

    TriggerFlags.TauSlice.signatures = TriggerFlags.TauSlice.signatures() + [
        #ATR-20049
        ChainProp(name="HLT_tau0_perf_ptonly_L1TAU12", groups=SingleTauGroup),
        ChainProp(name="HLT_tau0_perf_ptonly_L1TAU60", groups=SingleTauGroup),
        ChainProp(name="HLT_tau25_idperf_track_L1TAU12IM", groups=SingleTauGroup),
        ChainProp(name="HLT_tau25_idperf_tracktwo_L1TAU12IM", groups=SingleTauGroup),
        ChainProp(name="HLT_tau25_perf_tracktwo_L1TAU12IM", groups=SingleTauGroup),
        ChainProp(name="HLT_tau25_medium1_tracktwo_L1TAU12IM", groups=SingleTauGroup),
        ChainProp(name="HLT_tau25_idperf_tracktwoMVA_L1TAU12IM",groups=SingleTauGroup),
        ChainProp(name="HLT_tau25_perf_tracktwoMVA_L1TAU12IM",groups=SingleTauGroup),
        ChainProp(name="HLT_tau25_medium1_tracktwoMVA_L1TAU12IM",groups=SingleTauGroup),
        ChainProp(name="HLT_tau25_verylooseRNN_tracktwoMVA_L1TAU12IM",groups=SingleTauGroup),
        ChainProp(name="HLT_tau25_looseRNN_tracktwoMVA_L1TAU12IM",groups=SingleTauGroup),
        ChainProp(name="HLT_tau25_mediumRNN_tracktwoMVA_L1TAU12IM",groups=SingleTauGroup),
        ChainProp(name="HLT_tau25_tightRNN_tracktwoMVA_L1TAU12IM",groups=SingleTauGroup),
        ChainProp(name="HLT_tau25_verylooseRNN_tracktwo_L1TAU12IM",groups=SingleTauGroup),
        ChainProp(name="HLT_tau25_looseRNN_tracktwo_L1TAU12IM",groups=SingleTauGroup),
        ChainProp(name="HLT_tau25_mediumRNN_tracktwo_L1TAU12IM",groups=SingleTauGroup),
        ChainProp(name="HLT_tau25_tightRNN_tracktwo_L1TAU12IM",groups=SingleTauGroup),
        ChainProp(name="HLT_tau25_medium1_tracktwoEF_L1TAU12IM", groups=SingleTauGroup),
        ChainProp(name="HLT_tau35_mediumRNN_tracktwoMVA_L1TAU12IM", groups=SingleTauGroup),
        ChainProp(name="HLT_tau160_idperf_track_L1TAU100",groups=SingleTauGroup),
        ChainProp(name="HLT_tau0_perf_ptonly_L1TAU100",groups=SingleTauGroup),
        ChainProp(name="HLT_tau160_idperf_tracktwo_L1TAU100",groups=SingleTauGroup),
        ChainProp(name="HLT_tau160_perf_tracktwo_L1TAU100",groups=SingleTauGroup),
        ChainProp(name="HLT_tau160_idperf_tracktwoMVA_L1TAU100",groups=SingleTauGroup),
        ChainProp(name="HLT_tau160_perf_tracktwoMVA_L1TAU100",groups=SingleTauGroup),

    ]
    # TimM temporary disable due to !31039
    TriggerFlags.BphysicsSlice.signatures = TriggerFlags.BphysicsSlice.signatures() + [
        #ATR 20603
        # ChainProp(name='HLT_2mu4_bJpsimumu_L12MU4',     groups=BphysicsGroup),
        # ChainProp(name='HLT_2mu4_bUpsimumu_L12MU4',     groups=BphysicsGroup),
        #ATR-20839
        # ChainProp(name='HLT_2mu4_bDimu_L12MU4',     groups=BphysicsGroup),
    ]
    TriggerFlags.CombinedSlice.signatures = TriggerFlags.CombinedSlice.signatures() + [
        # groups need to be properly assigned here later

        # Test chain that is using parallel merging with different number of steps
        ChainProp(name='HLT_e3_etcut1step_mu26_L1EM8I_MU10', l1SeedThresholds=['EM8I', 'MU10'], stream=[PhysicsStream], groups=MultiElectronGroup),
        # Primary e-mu chains
        ChainProp(name='HLT_e17_lhloose_mu14_L1EM15VH_MU10', l1SeedThresholds=['EM15VH','MU10'], stream=[PhysicsStream], groups=MultiElectronGroup),
        ChainProp(name='HLT_e7_lhmedium_mu24_L1MU20',l1SeedThresholds=['EM3','MU20'],  stream=[PhysicsStream], groups=MultiElectronGroup),
        # Test photon-muon chain (isolation is there to have different number of steps)
        ChainProp(name='HLT_g25_medium_mu24_ivarmedium_L1MU20',l1SeedThresholds=['EM15VH','MU20'], stream=[PhysicsStream], groups=MultiElectronGroup),
   ]
    TriggerFlags.HeavyIonSlice.signatures  = TriggerFlags.HeavyIonSlice.signatures() + []
    TriggerFlags.BeamspotSlice.signatures  = TriggerFlags.BeamspotSlice.signatures() + []
    TriggerFlags.MinBiasSlice.signatures   = TriggerFlags.MinBiasSlice.signatures() + [
        ChainProp(name='HLT_mb_sptrk_L1RD0_FILLED', l1SeedThresholds=['FSNOSEED'], stream=[PhysicsStream], groups=MinBiasGroup),
        # ChainProp(name='HLT_mb_sp400_trk40_hmt_L1RD0_FILLED',        l1SeedThresholds=['FSNOSEED'], stream=[PhysicsStream], groups=MinBiasGroup),
        # ChainProp(name='HLT_mb_sp600_trk45_hmt_L1RD0_FILLED',        l1SeedThresholds=['FSNOSEED'], stream=[PhysicsStream], groups=MinBiasGroup),
    ]
    TriggerFlags.CalibSlice.signatures     = TriggerFlags.CalibSlice.signatures() + []
    TriggerFlags.CosmicSlice.signatures    = TriggerFlags.CosmicSlice.signatures() + [
    ]

    TriggerFlags.StreamingSlice.signatures = TriggerFlags.StreamingSlice.signatures() + [
    ]
    TriggerFlags.MonitorSlice.signatures   = TriggerFlags.MonitorSlice.signatures() + [
    ]

    # Random Seeded EB chains which select at the HLT based on L1 TBP bits
    TriggerFlags.EnhancedBiasSlice.signatures = TriggerFlags.EnhancedBiasSlice.signatures() + [ ]

    addSliceChainsToPrescales(TriggerFlags, Prescales.HLTPrescales_cosmics)


Prescales = mc_menu.Prescales
