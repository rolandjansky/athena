# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

#------------------------------------------------------------------------#
# Dev_pp_run3_v1.py menu for the long shutdown development
#------------------------------------------------------------------------#

# This defines the input format of the chain and it's properties with the defaults set
# always required are: name, stream and groups
#['name', 'L1chainParts'=[], 'stream', 'groups', 'merging'=[], 'topoStartFrom'=False],

import TriggerMenuMT.HLT.Menu.MC_pp_run3_v1 as mc_menu
from TriggerMenuMT.HLT.Config.Utility.ChainDefInMenu import ChainProp

# this is not the best option, due to flake violation, this list has to be changed when some groups are removed
from TriggerMenuMT.HLT.Menu.Physics_pp_run3_v1 import (PhysicsStream,
                                                                 SingleMuonGroup,
                                                                 MultiMuonGroup,
                                                                 SingleElectronGroup,
                                                                 MultiElectronGroup,
                                                                 SinglePhotonGroup,
                                                                 SingleMETGroup,
                                                                 MultiMETGroup,
                                                                 SingleJetGroup,
                                                                 MultiJetGroup,
                                                                 SingleBjetGroup,
                                                                 MultiBjetGroup,
                                                                 SingleTauGroup,
                                                                 BphysicsGroup,
                                                                 BphysElectronGroup,
                                                                 EgammaMETGroup,
                                                                 EgammaMuonGroup,
                                                                 EgammaTauGroup,
                                                                 MuonJetGroup,
                                                                 TauJetGroup,
                                                                 TauPhotonGroup,
                                                                 MuonMETGroup,
                                                                 EgammaJetGroup,
                                                                 MinBiasGroup,
                                                                 PrimaryLegGroup,
                                                                 PrimaryPhIGroup,
                                                                 SupportLegGroup,
                                                                 SupportPhIGroup,
                                                                 TagAndProbeLegGroup,
                                                                 UnconvTrkGroup,
                                                                 METPhaseIStreamersGroup,
                                                                 EOFTLALegGroup,
                                                                 )

DevGroup = ['Development']

def setupMenu():

    chains = mc_menu.setupMenu()

    from AthenaCommon.Logging import logging
    log = logging.getLogger( __name__ )
    log.info('[setupMenu] going to add the Dev menu chains now')

    chains['Muon'] += [

        ChainProp(name='HLT_mu6_ivarmedium_L1MU5VF', groups=DevGroup+SingleMuonGroup),

        # LRT
        ChainProp(name='HLT_mu6_LRT_idperf_L1MU5VF', groups=DevGroup+SingleMuonGroup),
        ChainProp(name='HLT_mu24_LRT_d0medium_L1MU14FCH', groups=DevGroup+SingleMuonGroup),
      
        # ATR-19452
        ChainProp(name='HLT_2mu4_muonqual_L12MU3V',  groups=DevGroup+MultiMuonGroup),
        ChainProp(name='HLT_2mu6_muonqual_L12MU5VF', groups=DevGroup+MultiMuonGroup),

        #ATR-21003
        ChainProp(name='HLT_2mu14_l2io_L12MU8F', groups=DevGroup+MultiMuonGroup),
        ChainProp(name='HLT_2mu6_l2io_L12MU5VF', groups=DevGroup+MultiMuonGroup),

        # Test T&P dimuon
        ChainProp(name='HLT_mu24_mu6_L1MU14FCH', l1SeedThresholds=['MU14FCH','MU3V'], groups=DevGroup+MultiMuonGroup),
        ChainProp(name='HLT_mu24_mu6_probe_L1MU14FCH', l1SeedThresholds=['MU14FCH','PROBEMU3V'], groups=DevGroup+MultiMuonGroup),

        #ATR-21566, di-muon TLA       
        ChainProp(name='HLT_mu10_PhysicsTLA_L1MU8F',   stream=['TLA'], groups=SingleMuonGroup+DevGroup), 
        ChainProp(name='HLT_mu10_mu6_probe_PhysicsTLA_L1MU8F', stream=['TLA'],l1SeedThresholds=['MU8F','PROBEMU3V'], groups=MultiMuonGroup+DevGroup),
    ]

    chains['Egamma'] += [
        # ElectronChains----------

        # electron forward triggers (keep this only for dev now)
        #ChainProp(name='HLT_e30_etcut_fwd_L1EM22VHI', groups=SingleElectronGroup),

        #ATR-22749
        ChainProp(name='HLT_2e5_lhvloose_bBeeM6000_L12EM3', l1SeedThresholds=['EM3'], stream=['BphysDelayed'], groups=BphysElectronGroup+DevGroup),
        ChainProp(name='HLT_e9_lhvloose_e5_lhvloose_bBeeM6000_L1BPH-0M9-EM7-EM5', l1SeedThresholds=['EM7','EM3'], stream=['BphysDelayed'], groups=BphysElectronGroup+DevGroup),
        ChainProp(name='HLT_e5_lhvloose_bBeeM6000_L1BPH-0DR3-EM7J15', l1SeedThresholds=['EM7'], stream=['BphysDelayed'], groups=BphysElectronGroup+DevGroup),
        ChainProp(name='HLT_e9_lhvloose_bBeeM6000_L1BPH-0DR3-EM7J15', l1SeedThresholds=['EM7'], stream=['BphysDelayed'], groups=BphysElectronGroup+DevGroup),

        # Photon chains for TLA
        ChainProp(name='HLT_g35_loose_PhysicsTLA_L1EM22VHI',stream=['TLA'], groups=SinglePhotonGroup+DevGroup),
            
        #------------ hipTRT trigger, ATR-22603
        ChainProp(name='HLT_g0_hiptrt_L1EM22VHI', groups=SinglePhotonGroup), 
        ChainProp(name='HLT_g0_hiptrt_L1eEM26M', groups=SinglePhotonGroup), 

        # Alternative formulation of T&P chains with generic mass cut combohypotool
        # With & without 'probe' expression to check count consistency
        # ATR-24117

        # Jpsiee
        ChainProp(name='HLT_e5_lhtight_e9_etcut_probe_1invmAB5_L1JPSI-1M5-EM7', l1SeedThresholds=['EM3','EM7'], groups=DevGroup+MultiElectronGroup),
        ChainProp(name='HLT_e5_lhtight_e14_etcut_probe_1invmAB5_L1JPSI-1M5-EM12', l1SeedThresholds=['EM3','EM12'], groups=DevGroup+MultiElectronGroup),
        ChainProp(name='HLT_e9_lhtight_e4_etcut_probe_1invmAB5_L1JPSI-1M5-EM7', l1SeedThresholds=['EM7','EM3'], groups=DevGroup+MultiElectronGroup),
        ChainProp(name='HLT_e14_lhtight_e4_etcut_probe_1invmAB5_L1JPSI-1M5-EM12', l1SeedThresholds=['EM12','EM3'], groups=DevGroup+MultiElectronGroup),

        #Photon Ringer Chains ATR-24384
        ChainProp(name='HLT_g20_loose_ringer_L1EM15VHI', groups=SinglePhotonGroup),
        ChainProp(name='HLT_g20_medium_ringer_L1EM15VHI', groups=SinglePhotonGroup),
        ChainProp(name='HLT_g20_tight_ringer_L1EM15VHI', groups=SinglePhotonGroup),
        ChainProp(name='HLT_g120_loose_ringer_L1EM22VHI', groups=SinglePhotonGroup),

        # Further optimisation of LRT, higher threshold looser WP
        # Primary candidates pending rate assessment ATR-24851
        ChainProp(name='HLT_e30_lhloose_nopix_lrtmedium_L1EM22VHI', groups=PrimaryLegGroup+SingleElectronGroup),
        ChainProp(name='HLT_e35_lhloose_nopix_lrtmedium_L1EM22VHI', groups=PrimaryLegGroup+SingleElectronGroup),
        ChainProp(name='HLT_e40_lhloose_nopix_lrtmedium_L1EM22VHI', groups=PrimaryLegGroup+SingleElectronGroup),
        ChainProp(name='HLT_e45_lhloose_nopix_lrtmedium_L1EM22VHI', groups=PrimaryLegGroup+SingleElectronGroup),

        ChainProp(name='HLT_e30_lhloose_nopix_lrtmedium_L1eEM26M', groups=PrimaryPhIGroup+SingleElectronGroup),
        ChainProp(name='HLT_e35_lhloose_nopix_lrtmedium_L1eEM26M', groups=PrimaryPhIGroup+SingleElectronGroup),
        ChainProp(name='HLT_e40_lhloose_nopix_lrtmedium_L1eEM26M', groups=PrimaryPhIGroup+SingleElectronGroup),
        ChainProp(name='HLT_e45_lhloose_nopix_lrtmedium_L1eEM26M', groups=PrimaryPhIGroup+SingleElectronGroup),

    ]

    chains['MET'] += [

        ChainProp(name='HLT_xe30_cell_L1XE30',       l1SeedThresholds=['FSNOSEED'], groups=SingleMETGroup+DevGroup),
        ChainProp(name='HLT_xe30_mht_L1XE30',        l1SeedThresholds=['FSNOSEED'], groups=SingleMETGroup+DevGroup),
        ChainProp(name='HLT_xe30_tcpufit_L1XE30',    l1SeedThresholds=['FSNOSEED'], groups=SingleMETGroup+DevGroup),
        ChainProp(name='HLT_xe30_trkmht_L1XE30',     l1SeedThresholds=['FSNOSEED'], groups=SingleMETGroup+DevGroup),
        ChainProp(name='HLT_xe30_pfsum_L1XE30',      l1SeedThresholds=['FSNOSEED'], groups=SingleMETGroup+DevGroup),
        ChainProp(name='HLT_xe30_pfsum_cssk_L1XE30', l1SeedThresholds=['FSNOSEED'], groups=SingleMETGroup+DevGroup),
        ChainProp(name='HLT_xe30_pfsum_vssk_L1XE30', l1SeedThresholds=['FSNOSEED'], groups=SingleMETGroup+DevGroup),
        ChainProp(name='HLT_xe30_pfopufit_L1XE30',   l1SeedThresholds=['FSNOSEED'], groups=SingleMETGroup+DevGroup),
        ChainProp(name='HLT_xe30_cvfpufit_L1XE30',   l1SeedThresholds=['FSNOSEED'], groups=SingleMETGroup+DevGroup),
        ChainProp(name='HLT_xe30_mhtpufit_em_subjesgscIS_L1XE30', l1SeedThresholds=['FSNOSEED'], groups=SingleMETGroup+DevGroup),
        ChainProp(name='HLT_xe30_mhtpufit_pf_subjesgscIS_L1XE30', l1SeedThresholds=['FSNOSEED'], groups=SingleMETGroup+DevGroup),

        ChainProp(name='HLT_xe110_tc_em_L1XE50',      l1SeedThresholds=['FSNOSEED'], groups=SingleMETGroup+DevGroup),
        ChainProp(name='HLT_xe110_mht_L1XE50',        l1SeedThresholds=['FSNOSEED'], groups=SingleMETGroup+DevGroup),
        ChainProp(name='HLT_xe110_tcpufit_L1XE50',    l1SeedThresholds=['FSNOSEED'], groups=SingleMETGroup+DevGroup),
        ChainProp(name='HLT_xe110_pfsum_L1XE50',      l1SeedThresholds=['FSNOSEED'], groups=SingleMETGroup+DevGroup),
        ChainProp(name='HLT_xe110_pfsum_cssk_L1XE50', l1SeedThresholds=['FSNOSEED'], groups=SingleMETGroup+DevGroup),
        ChainProp(name='HLT_xe110_pfsum_vssk_L1XE50', l1SeedThresholds=['FSNOSEED'], groups=SingleMETGroup+DevGroup),

        # Test chains to determine rate after calo-only preselection for tracking
        ChainProp(name='HLT_xe60_cell_L1XE50', l1SeedThresholds=['FSNOSEED'], groups=SingleMETGroup+DevGroup),
        ChainProp(name='HLT_xe55_cell_xe70_tcpufit_L1XE50', l1SeedThresholds=['FSNOSEED']*2, groups=MultiMETGroup+DevGroup),
    ]


    chains['Jet'] += [

        # Currently in jet monitoring, which should switch to use Physics chains, or these should be moved
        ChainProp(name='HLT_j85_pf_ftf_L1J20', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup+DevGroup),
        ChainProp(name='HLT_j45_subjesgscIS_ftf_L1J15', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup),
        ChainProp(name='HLT_j45_pf_ftf_L1J15', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup),
        ChainProp(name='HLT_j45_pf_nojcalib_ftf_L1J15', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup),
        ChainProp(name='HLT_j45_pf_subjesgscIS_ftf_L1J15', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup),
        ChainProp(name='HLT_j45_cssk_pf_nojcalib_ftf_L1J15', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup+DevGroup),
        ChainProp(name='HLT_j460_a10sd_pf_nojcalib_ftf_L1J100', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup),
        ChainProp(name='HLT_j460_a10sd_cssk_pf_nojcalib_ftf_L1J100', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup),
        ChainProp(name='HLT_j460_a10sd_cssk_pf_nojcalib_ftf_35smcINF_L1J100', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup),
        ChainProp(name='HLT_j20_pf_ftf_PhysicsTLA_L1J100', l1SeedThresholds=['FSNOSEED'], stream=['TLA'], groups=SingleJetGroup+DevGroup),
        ChainProp(name='HLT_j60_pf_ftf_j45_pf_ftf_2j20_pf_ftf_PhysicsTLA_L1J45p0ETA21_3J15p0ETA25', l1SeedThresholds=['FSNOSEED']*3, stream=['TLA'], groups=DevGroup+MultiJetGroup),
        ChainProp(name='HLT_j60_0eta290_020jvt_pf_ftf_j45_0eta290_020jvt_pf_ftf_2j20_0eta290_020jvt_pf_ftf_PhysicsTLA_L1J45p0ETA21_3J15p0ETA25', l1SeedThresholds=['FSNOSEED']*3, stream=['TLA'], groups=DevGroup+MultiJetGroup),
        #
        ChainProp(name='HLT_6j25_0eta240_L14J15', l1SeedThresholds=['FSNOSEED'],            groups=MultiJetGroup+DevGroup),
        ChainProp(name='HLT_6j25_ftf_0eta240_L14J15', l1SeedThresholds=['FSNOSEED'],        groups=MultiJetGroup+DevGroup),
        ChainProp(name='HLT_6j25_ftf_0eta240_010jvt_L14J15', l1SeedThresholds=['FSNOSEED'], groups=MultiJetGroup+DevGroup),
        ChainProp(name='HLT_6j25_ftf_0eta240_020jvt_L14J15', l1SeedThresholds=['FSNOSEED'], groups=MultiJetGroup+DevGroup),
        ChainProp(name='HLT_6j25_ftf_0eta240_050jvt_L14J15', l1SeedThresholds=['FSNOSEED'], groups=MultiJetGroup+DevGroup),
        #
        ChainProp(name='HLT_6j25_pf_ftf_0eta240_L14J15', l1SeedThresholds=['FSNOSEED'],        groups=MultiJetGroup+DevGroup),
        ChainProp(name='HLT_6j25_pf_ftf_0eta240_010jvt_L14J15', l1SeedThresholds=['FSNOSEED'], groups=MultiJetGroup+DevGroup),
        ChainProp(name='HLT_6j25_pf_ftf_0eta240_020jvt_L14J15', l1SeedThresholds=['FSNOSEED'], groups=MultiJetGroup+DevGroup),
        ChainProp(name='HLT_6j25_pf_ftf_0eta240_050jvt_L14J15', l1SeedThresholds=['FSNOSEED'], groups=MultiJetGroup+DevGroup),

        ### PURE TEST CHAINS

        ChainProp(name='HLT_j0_FBDJNOSHARED10etXX20etXX34massXX50fbet_L1J20', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup+DevGroup),
        ChainProp(name='HLT_j0_FBDJSHARED_L1J20', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup+DevGroup),
        ChainProp(name='HLT_j60_j0_FBDJSHARED_L1J20', l1SeedThresholds=['FSNOSEED']*2, groups=MultiJetGroup+DevGroup),

        # Emerging Jets test chains ATR-21593

        # primary emerging jets chain
        ChainProp(name='HLT_j175_a10sd_cssk_pf_jes_ftf_0eta180_emergingPTF0p08dR1p2_L1J100', groups=SingleJetGroup+PrimaryLegGroup, l1SeedThresholds=['FSNOSEED']),

        # backup emerging jets chains to be used for rate refinement in enhanced bias reprocessing
        ChainProp(name='HLT_j175_a10sd_cssk_pf_jes_ftf_0eta180_emergingPTF0p075dR1p2_L1J100', groups=SingleJetGroup+DevGroup, l1SeedThresholds=['FSNOSEED']),
        ChainProp(name='HLT_j175_a10sd_cssk_pf_jes_ftf_0eta160_emergingPTF0p075dR1p2_L1J100', groups=SingleJetGroup+DevGroup, l1SeedThresholds=['FSNOSEED']),
        ChainProp(name='HLT_j175_a10sd_cssk_pf_jes_ftf_0eta180_emergingPTF0p07dR1p2_L1J100', groups=SingleJetGroup+DevGroup, l1SeedThresholds=['FSNOSEED']),
        ChainProp(name='HLT_j175_a10sd_cssk_pf_jes_ftf_0eta160_emergingPTF0p07dR1p2_L1J100', groups=SingleJetGroup+DevGroup, l1SeedThresholds=['FSNOSEED']),

        ChainProp(name='HLT_j175_a10sd_cssk_pf_jes_ftf_0eta180_preselj200_emergingPTF0p075dR1p2_L1J100', groups=SingleJetGroup+DevGroup, l1SeedThresholds=['FSNOSEED']),
        ChainProp(name='HLT_j175_a10sd_cssk_pf_jes_ftf_0eta160_preselj200_emergingPTF0p075dR1p2_L1J100', groups=SingleJetGroup+DevGroup, l1SeedThresholds=['FSNOSEED']),
        ChainProp(name='HLT_j175_a10sd_cssk_pf_jes_ftf_0eta180_preselj200_emergingPTF0p07dR1p2_L1J100', groups=SingleJetGroup+DevGroup, l1SeedThresholds=['FSNOSEED']),
        ChainProp(name='HLT_j175_a10sd_cssk_pf_jes_ftf_0eta160_preselj200_emergingPTF0p07dR1p2_L1J100', groups=SingleJetGroup+DevGroup, l1SeedThresholds=['FSNOSEED']),


        # primary dijet emerging jets chain
        ChainProp(name='HLT_2j110_a10sd_cssk_pf_jes_ftf_0eta200_emergingPTF0p1dR1p2_L1J100', groups=SingleJetGroup+PrimaryLegGroup, l1SeedThresholds=['FSNOSEED']),
        # backup dijet emerging jets chain
        ChainProp(name='HLT_2j110_a10sd_cssk_pf_jes_ftf_0eta180_emergingPTF0p09dR1p2_L1J100', groups=SingleJetGroup+PrimaryLegGroup, l1SeedThresholds=['FSNOSEED']),

        # primary Trackless jet chain
        ChainProp(name='HLT_j175_a10r_subjesIS_ftf_0eta200_tracklessdR1p2_L1J100',    groups=SingleJetGroup+PrimaryLegGroup, l1SeedThresholds=['FSNOSEED']),
        # backup Trackless jet chain
        ChainProp(name='HLT_j260_a10r_subjesIS_ftf_0eta200_tracklessdR1p2_L1J100',    groups=SingleJetGroup+PrimaryLegGroup, l1SeedThresholds=['FSNOSEED']),

        # end of emerging jets chains

        #####
        # Primary jet chains w/o preselection, for comparison
        ChainProp(name='HLT_j420_pf_ftf_L1J100', l1SeedThresholds=['FSNOSEED'],  groups=SingleJetGroup+DevGroup),
        ChainProp(name='HLT_2j250_pf_ftf_0eta240_j120_pf_ftf_0eta240_L1J100',    l1SeedThresholds=['FSNOSEED']*2, groups=MultiJetGroup+DevGroup ),
        ChainProp(name='HLT_3j200_pf_ftf_L1J100', l1SeedThresholds=['FSNOSEED'], groups=MultiJetGroup+DevGroup),
        ChainProp(name='HLT_4j115_pf_ftf_L13J50', l1SeedThresholds=['FSNOSEED'], groups=MultiJetGroup+DevGroup),
        ChainProp(name='HLT_5j70_pf_ftf_0eta240_L14J15', l1SeedThresholds=['FSNOSEED'],  groups=MultiJetGroup+DevGroup),
        ChainProp(name='HLT_5j85_pf_ftf_L14J15', l1SeedThresholds=['FSNOSEED'],  groups=MultiJetGroup+DevGroup),
        ChainProp(name='HLT_6j55_pf_ftf_0eta240_L14J15', l1SeedThresholds=['FSNOSEED'], groups=MultiJetGroup+DevGroup),
        ChainProp(name='HLT_6j70_pf_ftf_L14J15', l1SeedThresholds=['FSNOSEED'],  groups=MultiJetGroup+DevGroup),
        ChainProp(name='HLT_7j45_pf_ftf_L14J15', l1SeedThresholds=['FSNOSEED'],  groups=MultiJetGroup+DevGroup),
        ChainProp(name='HLT_10j40_pf_ftf_L14J15', l1SeedThresholds=['FSNOSEED'], groups=MultiJetGroup+DevGroup),

        ChainProp(name='HLT_j0_HT1000_pf_ftf_L1J100', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup+DevGroup),
        ChainProp(name='HLT_j0_HT1000_pf_ftf_L1HT190-J15s5pETA21', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup+DevGroup),

        # CSSKPFlow
        ChainProp(name='HLT_j420_a10sd_cssk_pf_jes_ftf_35smcINF_L1J100', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup+DevGroup),
        ChainProp(name='HLT_2j330_a10sd_cssk_pf_jes_ftf_35smcINF_L1J100', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup+DevGroup),
        ChainProp(name='HLT_j420_a10sd_cssk_pf_jes_ftf_35smcINF_L1SC111-CJ15', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup+DevGroup),
        ChainProp(name='HLT_2j330_a10sd_cssk_pf_jes_ftf_35smcINF_L1SC111-CJ15', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup+DevGroup),
        ChainProp(name='HLT_j360_a10sd_cssk_pf_jes_ftf_60smcINF_j360_a10sd_cssk_pf_jes_ftf_L1SC111-CJ15', l1SeedThresholds=['FSNOSEED']*2, groups=DevGroup+MultiJetGroup),
        ChainProp(name='HLT_j370_a10sd_cssk_pf_jes_ftf_35smcINF_j370_a10sd_cssk_pf_jes_ftf_L1SC111-CJ15', l1SeedThresholds=['FSNOSEED']*2, groups=DevGroup+MultiJetGroup),

        ##### End no-preselection

        # Prototyping RoI jet tracking
        ChainProp(name="HLT_j80_roiftf_preselj20_L1J20", l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup+DevGroup),
        ChainProp(name="HLT_j80_roiftf_95bdips_preselj20_L1J20", l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup+DevGroup),
        ChainProp(name="HLT_j80_roiftf_77bdips_preselj20_L1J20", l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup+DevGroup),
        #
        ChainProp(name='HLT_2j20_0eta240_roiftf_2j20_0eta240_85bdips_roiftf_presel4c20_L14J15p0ETA25', l1SeedThresholds=['FSNOSEED','FSNOSEED'], groups=MultiJetGroup+DevGroup),
        ChainProp(name='HLT_2j25_0eta240_roiftf_2j25_0eta240_85bdips_roiftf_presel4c25_L14J15p0ETA25', l1SeedThresholds=['FSNOSEED','FSNOSEED'], groups=MultiJetGroup+DevGroup),
        ChainProp(name='HLT_2j30_0eta240_roiftf_2j30_0eta240_85bdips_roiftf_presel4c30_L14J15p0ETA25', l1SeedThresholds=['FSNOSEED','FSNOSEED'], groups=MultiJetGroup+DevGroup),
        ChainProp(name='HLT_2j35_0eta240_roiftf_2j35_0eta240_85bdips_roiftf_presel4c35_L14J15p0ETA25', l1SeedThresholds=['FSNOSEED','FSNOSEED'], groups=MultiJetGroup+DevGroup),
        ChainProp(name='HLT_2j45_0eta240_roiftf_2j35_0eta240_85bdips_roiftf_presel4c45_L14J15p0ETA25', l1SeedThresholds=['FSNOSEED','FSNOSEED'], groups=MultiJetGroup+DevGroup),
        #
        ChainProp(name='HLT_2j25_0eta240_roiftf_2j25_0eta240_85bdips_roiftf_presel4c25_L1J45p0ETA21_3J15p0ETA25', l1SeedThresholds=['FSNOSEED','FSNOSEED'], groups=MultiJetGroup+DevGroup),
        ChainProp(name='HLT_2j20_0eta240_roiftf_2j20_0eta240_85bdips_roiftf_presel4c20_L1J45p0ETA21_3J15p0ETA25', l1SeedThresholds=['FSNOSEED','FSNOSEED'], groups=MultiJetGroup+DevGroup),
        ChainProp(name='HLT_2j30_0eta240_roiftf_2j30_0eta240_85bdips_roiftf_presel4c30_L1J45p0ETA21_3J15p0ETA25', l1SeedThresholds=['FSNOSEED','FSNOSEED'], groups=MultiJetGroup+DevGroup),
        ChainProp(name='HLT_2j35_0eta240_roiftf_2j35_0eta240_85bdips_roiftf_presel4c35_L1J45p0ETA21_3J15p0ETA25', l1SeedThresholds=['FSNOSEED','FSNOSEED'], groups=MultiJetGroup+DevGroup),
        ChainProp(name='HLT_2j45_0eta240_roiftf_2j35_0eta240_85bdips_roiftf_presel4c45_L1J45p0ETA21_3J15p0ETA25', l1SeedThresholds=['FSNOSEED','FSNOSEED'], groups=MultiJetGroup+DevGroup),
        #
        # some specific tighter test chains at 20 and 25 GeV
        ChainProp(name='HLT_2j25_0eta240_roiftf_2j25_0eta240_80bdips_roiftf_presel4c25_L1J45p0ETA21_3J15p0ETA25', l1SeedThresholds=['FSNOSEED','FSNOSEED'], groups=MultiJetGroup+DevGroup),
        ChainProp(name='HLT_2j20_0eta240_roiftf_2j20_0eta240_80bdips_roiftf_presel4c20_L1J45p0ETA21_3J15p0ETA25', l1SeedThresholds=['FSNOSEED','FSNOSEED'], groups=MultiJetGroup+DevGroup),




        # ATR-24720 Testing additions to Run 3 baseline menu
        # HT preselection studies
        ChainProp(name='HLT_j0_HT1000_pf_ftf_preselj140_L1HT190-J15s5pETA21', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup+DevGroup),
        ChainProp(name='HLT_j0_HT1000_pf_ftf_presel3j45_L1HT190-J15s5pETA21', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup+DevGroup),
        ChainProp(name='HLT_j0_HT1000_pf_ftf_presel4j40_L1HT190-J15s5pETA21', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup+DevGroup),
        ChainProp(name='HLT_j0_HT1000_pf_ftf_presel4c40_L1HT190-J15s5pETA21', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup+DevGroup),
        ChainProp(name='HLT_j0_HT1000XX0eta240XX020jvt_pf_ftf_presel4c40_L1HT190-J15s5pETA21', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup+DevGroup),
        ChainProp(name='HLT_j0_HT1000_pf_ftf_presel4j45_L1HT190-J15s5pETA21', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup+DevGroup),
        ChainProp(name='HLT_j0_HT1000XX020jvt_pf_ftf_presel4j45_L1HT190-J15s5pETA21', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup+DevGroup),
        ChainProp(name='HLT_j0_HT1000_pf_ftf_presel4j50_L1HT190-J15s5pETA21', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup+DevGroup),
        ChainProp(name='HLT_j0_HT1000_pf_ftf_presel5j25_L1HT190-J15s5pETA21', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup+DevGroup),
        ChainProp(name='HLT_j0_HT1000XX020jvt_pf_ftf_presel5j25_L1HT190-J15s5pETA21', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup+DevGroup),
        ###
        ChainProp(name='HLT_j0_HT1000_pf_ftf_preseljHT400_L1HT190-J15s5pETA21', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup+DevGroup),
        ChainProp(name='HLT_j0_HT1000_pf_ftf_preselcHT400_L1HT190-J15s5pETA21', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup+DevGroup),
        ChainProp(name='HLT_j0_HT1000XX020jvt_pf_ftf_preseljHT400_L1HT190-J15s5pETA21', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup+DevGroup),
        ChainProp(name='HLT_j0_HT1000XX020jvt_pf_ftf_preselcHT400_L1HT190-J15s5pETA21', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup+DevGroup),
        ChainProp(name='HLT_j0_HT1000XX0eta240_pf_ftf_preselcHT400_L1HT190-J15s5pETA21', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup+DevGroup),
        ChainProp(name='HLT_j0_HT1000XX0eta240XX020jvt_pf_ftf_preselcHT400_L1HT190-J15s5pETA21', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup+DevGroup),
        ChainProp(name='HLT_j0_HT1000_pf_ftf_preseljHT500_L1HT190-J15s5pETA21', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup+DevGroup),
        ChainProp(name='HLT_j0_HT1000XX020jvt_pf_ftf_preseljHT500_L1HT190-J15s5pETA21', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup+DevGroup),
        ChainProp(name='HLT_j0_HT1000XX020jvt_pf_ftf_preselcHT500_L1HT190-J15s5pETA21', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup+DevGroup),
        ChainProp(name='HLT_j0_HT1000XX0eta240XX020jvt_pf_ftf_preselcHT500_L1HT190-J15s5pETA21', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup+DevGroup),
        ChainProp(name='HLT_j0_HT1000_pf_ftf_preseljHT600_L1HT190-J15s5pETA21', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup+DevGroup),
        ChainProp(name='HLT_j0_HT1000XX020jvt_pf_ftf_preseljHT600_L1HT190-J15s5pETA21', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup+DevGroup),
        ChainProp(name='HLT_j0_HT1000XX020jvt_pf_ftf_preselcHT600_L1HT190-J15s5pETA21', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup+DevGroup),
        ChainProp(name='HLT_j0_HT1000XX0eta240XX020jvt_pf_ftf_preselcHT600_L1HT190-J15s5pETA21', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup+DevGroup),

        ### END PURE TEST CHAINS
 
        # Central Exclusive Production for SM group
        ChainProp(name='HLT_2j100_L1CEP-CjJ90', l1SeedThresholds=['FSNOSEED'], groups=PrimaryPhIGroup+MultiJetGroup),
        ChainProp(name='HLT_2j100_L1CEP-CjJ100', l1SeedThresholds=['FSNOSEED'], groups=PrimaryPhIGroup+MultiJetGroup),

        # ATR-24838 Large R L1J100 jet chains with jLJ L1 items (L1J100->L1jLJ140)
        ChainProp(name='HLT_j175_a10sd_cssk_pf_jes_ftf_0eta180_emergingPTF0p08dR1p2_L1jLJ140', groups=SingleJetGroup+PrimaryPhIGroup, l1SeedThresholds=['FSNOSEED']),
        ChainProp(name='HLT_2j110_a10sd_cssk_pf_jes_ftf_0eta200_emergingPTF0p1dR1p2_L1jLJ140', groups=SingleJetGroup+PrimaryPhIGroup, l1SeedThresholds=['FSNOSEED']),
        ChainProp(name='HLT_2j110_a10sd_cssk_pf_jes_ftf_0eta180_emergingPTF0p09dR1p2_L1jLJ140', groups=SingleJetGroup+PrimaryPhIGroup, l1SeedThresholds=['FSNOSEED']),
        ChainProp(name='HLT_j175_a10r_subjesIS_ftf_0eta200_tracklessdR1p2_L1jLJ140',    groups=SingleJetGroup+PrimaryPhIGroup, l1SeedThresholds=['FSNOSEED']),
        ChainProp(name='HLT_j260_a10r_subjesIS_ftf_0eta200_tracklessdR1p2_L1jLJ140',    groups=SingleJetGroup+PrimaryPhIGroup, l1SeedThresholds=['FSNOSEED']),

        ]
        

    chains['Bjet'] += [

        ChainProp(name="HLT_j225_0eta290_pf_ftf_bdl1r70_L1J100", l1SeedThresholds=['FSNOSEED'], groups=SingleBjetGroup + DevGroup),
        ChainProp(name="HLT_j225_0eta290_pf_ftf_bdl1r77_L1J100", l1SeedThresholds=['FSNOSEED'], groups=SingleBjetGroup + DevGroup),
        ChainProp(name='HLT_j275_0eta290_pf_ftf_bdl1r85_L1J100', l1SeedThresholds=['FSNOSEED'], groups=SingleBjetGroup + DevGroup),
        ChainProp(name='HLT_j300_0eta290_pf_ftf_bdl1r85_L1J100', l1SeedThresholds=['FSNOSEED'], groups=SingleBjetGroup + DevGroup),

        ChainProp(name="HLT_3j65_0eta290_020jvt_pf_ftf_bdl1r77_L13J35p0ETA23", l1SeedThresholds=['FSNOSEED'], groups=MultiBjetGroup+DevGroup),
        ChainProp(name="HLT_4j35_0eta290_020jvt_pf_ftf_bdl1r77_L14J15p0ETA25", l1SeedThresholds=['FSNOSEED'], groups=MultiBjetGroup+DevGroup),

        # single bjet pflow options, # changes according to ATR-23883
        ChainProp(name="HLT_j225_0eta290_pf_ftf_bdl1d60_L1J100", l1SeedThresholds=['FSNOSEED'], groups=SingleBjetGroup + DevGroup),
        ChainProp(name="HLT_j225_0eta290_pf_ftf_bdl1d85_L1J100", l1SeedThresholds=['FSNOSEED'], groups=SingleBjetGroup + DevGroup),

        ChainProp(name='HLT_j275_0eta290_pf_ftf_bdl1d70_L1J100', l1SeedThresholds=['FSNOSEED'], groups=SingleBjetGroup + DevGroup),
        ChainProp(name='HLT_j275_0eta290_pf_ftf_bdl1d77_L1J100', l1SeedThresholds=['FSNOSEED'], groups=SingleBjetGroup + DevGroup),

        ChainProp(name='HLT_j300_0eta290_pf_ftf_bdl1d60_L1J100', l1SeedThresholds=['FSNOSEED'], groups=SingleBjetGroup + DevGroup),
        ChainProp(name='HLT_j300_0eta290_pf_ftf_bdl1d77_L1J100', l1SeedThresholds=['FSNOSEED'], groups=SingleBjetGroup + DevGroup),

        ChainProp(name='HLT_j360_0eta290_pf_ftf_bdl1d60_L1J100', l1SeedThresholds=['FSNOSEED'], groups=SingleBjetGroup + DevGroup),
        ChainProp(name='HLT_j360_0eta290_pf_ftf_bdl1d70_L1J100', l1SeedThresholds=['FSNOSEED'], groups=SingleBjetGroup + DevGroup),
        ChainProp(name='HLT_j360_0eta290_pf_ftf_bdl1d85_L1J100', l1SeedThresholds=['FSNOSEED'], groups=SingleBjetGroup + DevGroup),

        # for monitoring
        ### IS THIS SUPPORT?
        ChainProp(name='HLT_j45_0eta290_020jvt_pf_ftf_bdl1r70_L1J20', l1SeedThresholds=['FSNOSEED'], groups=SingleBjetGroup),

        # Chains without preselection for cost estimates
        # these chains are taken from the Run 2 menu for now --- likely to be loosened
        ChainProp(name="HLT_j275_0eta290_020jvt_pf_ftf_bdl1r60_L1J100", l1SeedThresholds=['FSNOSEED'], groups=DevGroup+SingleBjetGroup),
        ChainProp(name="HLT_j300_0eta290_020jvt_pf_ftf_bdl1r70_L1J100", l1SeedThresholds=['FSNOSEED'], groups=DevGroup+SingleBjetGroup),
        ChainProp(name="HLT_j360_0eta290_020jvt_pf_ftf_bdl1r77_L1J100", l1SeedThresholds=['FSNOSEED'], groups=DevGroup+SingleBjetGroup),

        # dl1d test chains
        ChainProp(name="HLT_j275_0eta290_020jvt_pf_ftf_bdl1d60_L1J100", l1SeedThresholds=['FSNOSEED'], groups=DevGroup+SingleBjetGroup),
        ChainProp(name="HLT_j300_0eta290_020jvt_pf_ftf_bdl1d70_L1J100", l1SeedThresholds=['FSNOSEED'], groups=DevGroup+SingleBjetGroup),
        ChainProp(name="HLT_j360_0eta290_020jvt_pf_ftf_bdl1d77_L1J100", l1SeedThresholds=['FSNOSEED'], groups=DevGroup+SingleBjetGroup),

        # Various multi-b
        ChainProp(name="HLT_j150_0eta320_pf_ftf_2j55_0eta290_020jvt_pf_ftf_bdl1r70_L1J85_3J30", l1SeedThresholds=['FSNOSEED','FSNOSEED'], groups=DevGroup+MultiBjetGroup),
        ChainProp(name="HLT_3j35_0eta290_020jvt_pf_ftf_bdl1r70_j35_pf_ftf_0eta320_L14J15p0ETA25", l1SeedThresholds=['FSNOSEED','FSNOSEED'], groups=DevGroup+MultiBjetGroup),
        ChainProp(name="HLT_j175_0eta290_020jvt_pf_ftf_bdl1r60_j60_0eta290_020jvt_pf_ftf_bdl1r60_L1J100", l1SeedThresholds=['FSNOSEED','FSNOSEED'], groups=DevGroup+MultiBjetGroup),
        ChainProp(name="HLT_2j35_0eta290_020jvt_pf_ftf_bdl1r70_2j35_0eta290_020jvt_pf_ftf_bdl1r85_L14J15p0ETA25", l1SeedThresholds=['FSNOSEED','FSNOSEED'], groups=DevGroup+MultiBjetGroup),
        ChainProp(name="HLT_2j55_0eta290_020jvt_pf_ftf_bdl1r60_2j55_pf_ftf_0eta320_L14J15p0ETA25", l1SeedThresholds=['FSNOSEED','FSNOSEED'], groups=DevGroup+MultiBjetGroup),
        ChainProp(name="HLT_2j35_0eta290_020jvt_pf_ftf_bdl1r60_3j35_pf_ftf_0eta320_L15J15p0ETA25", l1SeedThresholds=['FSNOSEED','FSNOSEED'], groups=DevGroup+MultiBjetGroup),
        ChainProp(name="HLT_2j45_0eta290_020jvt_pf_ftf_bdl1r60_3j45_pf_ftf_0eta320_L15J15p0ETA25", l1SeedThresholds=['FSNOSEED','FSNOSEED'], groups=DevGroup+MultiBjetGroup),
        ChainProp(name="HLT_j75_0eta290_020jvt_pf_ftf_bdl1r60_3j75_pf_ftf_L14J20", l1SeedThresholds=['FSNOSEED','FSNOSEED'], groups=DevGroup+MultiBjetGroup),
        ChainProp(name="HLT_2j45_0eta290_020jvt_pf_ftf_bdl1r60_2j45_pf_ftf_L14J15p0ETA25", l1SeedThresholds=['FSNOSEED','FSNOSEED'], groups=DevGroup+MultiBjetGroup),
        # Run 2 HH4b low-threshold chain
        ChainProp(name="HLT_2j35_0eta240_020jvt_pf_ftf_bdl1r60_2j35_0eta240_020jvt_pf_ftf_L14J15p0ETA25", l1SeedThresholds=['FSNOSEED','FSNOSEED'], groups=DevGroup+MultiBjetGroup),
        # VBF chains
        ChainProp(name='HLT_j80_pf_ftf_0eta240_j60_pf_ftf_0eta320_j45_pf_ftf_320eta490_SHARED_2j45_pf_ftf_0eta290_bdl1r60_L1J40p0ETA25_2J25_J20p31ETA49', l1SeedThresholds=['FSNOSEED']*4, groups=PrimaryLegGroup+MultiBjetGroup),
        ChainProp(name="HLT_j80_pf_ftf_0eta320_bdl1r70_j60_pf_ftf_0eta320_bdl1r85_j45_pf_ftf_320eta490_L1J40p0ETA25_2J25_J20p31ETA49", l1SeedThresholds=['FSNOSEED']*3,stream=[PhysicsStream], groups=PrimaryLegGroup+MultiBjetGroup),
        ChainProp(name="HLT_j55_pf_ftf_0eta320_bdl1r70_2j45_pf_ftf_320eta490_L1J25p0ETA23_2J15p31ETA49",l1SeedThresholds=['FSNOSEED']*2,  stream=[PhysicsStream], groups=PrimaryLegGroup+MultiBjetGroup),
        ChainProp(name='HLT_j70_pf_ftf_0eta490_j50_pf_ftf_0eta490_2j35_pf_ftf_0eta490_SHARED_2j35_pf_ftf_0eta290_bdl1r70_j0_pf_ftf_DJMASS1000j50_L1MJJ-500-NFF', l1SeedThresholds=['FSNOSEED']*5,stream=['VBFDelayed'], groups=PrimaryLegGroup+MultiBjetGroup),

        #### TESTING CHAINS

        # ATR-22937
        # multi-b chains for assessing mistag rates and flavor fractions
        ChainProp(name="HLT_3j65_0eta290_020jvt_pf_ftf_bdl1r60_L1J45p0ETA21_3J15p0ETA25", l1SeedThresholds=['FSNOSEED'], groups=MultiBjetGroup+DevGroup),
        ChainProp(name="HLT_3j65_0eta290_020jvt_pf_ftf_bdl1r70_L1J45p0ETA21_3J15p0ETA25", l1SeedThresholds=['FSNOSEED'], groups=MultiBjetGroup+DevGroup),
        ChainProp(name="HLT_3j65_0eta290_020jvt_pf_ftf_bdl1r77_L1J45p0ETA21_3J15p0ETA25", l1SeedThresholds=['FSNOSEED'], groups=MultiBjetGroup+DevGroup),
        ChainProp(name="HLT_3j65_0eta290_020jvt_pf_ftf_bdl1r85_L1J45p0ETA21_3J15p0ETA25", l1SeedThresholds=['FSNOSEED'], groups=MultiBjetGroup+DevGroup),

        ChainProp(name="HLT_4j35_0eta290_020jvt_pf_ftf_bdl1r60_L1J45p0ETA21_3J15p0ETA25", l1SeedThresholds=['FSNOSEED'], groups=MultiBjetGroup+DevGroup),
        ChainProp(name="HLT_4j35_0eta290_020jvt_pf_ftf_bdl1r70_L1J45p0ETA21_3J15p0ETA25", l1SeedThresholds=['FSNOSEED'], groups=MultiBjetGroup+DevGroup),
        ChainProp(name="HLT_4j35_0eta290_020jvt_pf_ftf_bdl1r77_L1J45p0ETA21_3J15p0ETA25", l1SeedThresholds=['FSNOSEED'], groups=MultiBjetGroup+DevGroup),
        ChainProp(name="HLT_4j35_0eta290_020jvt_pf_ftf_bdl1r85_L1J45p0ETA21_3J15p0ETA25", l1SeedThresholds=['FSNOSEED'], groups=MultiBjetGroup+DevGroup),

        ChainProp(name='HLT_5j25_pf_ftf_0eta240_020jvt_j25_pf_ftf_0eta240_020jvt_bdl1r60_L14J15', l1SeedThresholds=['FSNOSEED']*2, groups=MultiBjetGroup+DevGroup),
        ChainProp(name='HLT_5j35_pf_ftf_0eta240_020jvt_j25_pf_ftf_0eta240_020jvt_bdl1r60_L14J15', l1SeedThresholds=['FSNOSEED']*2, stream=['VBFDelayed'], groups=PrimaryLegGroup+MultiBjetGroup),
        ChainProp(name='HLT_5j45_pf_ftf_0eta240_020jvt_j25_pf_ftf_0eta240_020jvt_bdl1r60_L14J15', l1SeedThresholds=['FSNOSEED']*2, stream=['VBFDelayed'], groups=PrimaryLegGroup+MultiBjetGroup),

        # Tests of potential TLA chains for cost/rate
        # ATR-23002 - b-jets
        ChainProp(name='HLT_j20_0eta290_pf_ftf_boffperf_L1HT190-J15s5pETA21', l1SeedThresholds=['FSNOSEED'], groups=SingleBjetGroup+DevGroup),
        ChainProp(name='HLT_j100_pf_ftf_0eta320_j20_0eta290_pf_ftf_boffperf_L1HT190-J15s5pETA21', l1SeedThresholds=['FSNOSEED']*2, groups=SingleBjetGroup+DevGroup),
        ChainProp(name='HLT_4j20_0eta290_pf_ftf_boffperf_L1HT190-J15s5pETA21', l1SeedThresholds=['FSNOSEED'], groups=MultiBjetGroup+DevGroup),
        ChainProp(name='HLT_4j20_pf_ftf_020jvt_boffperf_L1HT190-J15s5pETA21', l1SeedThresholds=['FSNOSEED'], groups=MultiBjetGroup+DevGroup),
        ChainProp(name='HLT_3j20_pf_ftf_020jvt_j20_0eta290_pf_ftf_boffperf_L1HT190-J15s5pETA21', l1SeedThresholds=['FSNOSEED']*2, groups=MultiBjetGroup+DevGroup),
        ChainProp(name='HLT_4j20_pf_ftf_020jvt_boffperf_L1J45p0ETA21_3J15p0ETA25', l1SeedThresholds=['FSNOSEED'], groups=MultiBjetGroup+DevGroup),

        # EMTopo Chains (likely not used)
        # ATR-22165
        # TODO: Broken due to ATR-24730, uncomment after fixed
        # ChainProp(name='HLT_j275_subjesgscIS_ftf_bdl1r60_L1J100', l1SeedThresholds=['FSNOSEED'], groups=SingleBjetGroup+DevGroup),
        # ChainProp(name='HLT_j300_subjesgscIS_ftf_bdl1r70_L1J100', l1SeedThresholds=['FSNOSEED'], groups=SingleBjetGroup+DevGroup),
        # ChainProp(name='HLT_j360_subjesgscIS_ftf_bdl1r77_L1J100', l1SeedThresholds=['FSNOSEED'], groups=SingleBjetGroup+DevGroup),
        # ChainProp(name='HLT_j45_subjesgscIS_ftf_bdl1r70_L1J20', l1SeedThresholds=['FSNOSEED'], groups=SingleBjetGroup+DevGroup),

        # ChainProp(name="HLT_j110_subjesgscIS_ftf_bdl1r60_j45_subjesgscIS_ftf_bdl1r70_L1J50", l1SeedThresholds=['FSNOSEED','FSNOSEED'], groups=MultiBjetGroup+DevGroup),

        # TLA btag ATR-23002
        ## dijet btag TLA
        ChainProp(name='HLT_j20_0eta290_pf_ftf_boffperf_preselj140_PhysicsTLA_L1J50', l1SeedThresholds=['FSNOSEED'], stream=['TLA'], groups=EOFTLALegGroup+SingleBjetGroup),
        ChainProp(name='HLT_j20_0eta290_pf_ftf_boffperf_preselj180_PhysicsTLA_L1J100', l1SeedThresholds=['FSNOSEED'], stream=['TLA'], groups=SingleBjetGroup+DevGroup),
        ChainProp(name='HLT_j20_0eta290_pf_ftf_boffperf_preselj140_PhysicsTLA_L1J50_DETA20-J50J', l1SeedThresholds=['FSNOSEED'], stream=['TLA'], groups=EOFTLALegGroup+SingleBjetGroup),
        ## multijet btag TLA - HT190
        ChainProp(name='HLT_j20_0eta290_pf_ftf_boffperf_preselj180_PhysicsTLA_L1HT190-J15s5pETA21', l1SeedThresholds=['FSNOSEED'], stream=['TLA'], groups=MultiBjetGroup+DevGroup),
        ChainProp(name='HLT_j20_0eta290_pf_ftf_boffperf_presel4j20_PhysicsTLA_L1HT190-J15s5pETA21', l1SeedThresholds=['FSNOSEED'], stream=['TLA'], groups=MultiBjetGroup+DevGroup),
        ChainProp(name='HLT_j20_0eta290_pf_ftf_boffperf_presel4j25_PhysicsTLA_L1HT190-J15s5pETA21', l1SeedThresholds=['FSNOSEED'], stream=['TLA'], groups=MultiBjetGroup+DevGroup),
        ChainProp(name='HLT_4j20_0eta290_pf_ftf_020jvt_boffperf_preselj180_PhysicsTLA_L1HT190-J15s5pETA21', l1SeedThresholds=['FSNOSEED'], stream=['TLA'], groups=MultiBjetGroup+DevGroup),
        # multijet btag TLA - MultiJet L1
        ChainProp(name='HLT_j60_0eta290_pf_ftf_j45_0eta290_pf_ftf_j25_0eta290_pf_ftf_j20_0eta290_pf_ftf_boffperf_preselc60XXc45XXc25XXc20_PhysicsTLA_L1J45p0ETA21_3J15p0ETA25', l1SeedThresholds=['FSNOSEED']*4, stream=['TLA'], groups=MultiBjetGroup+DevGroup),
        ChainProp(name='HLT_j60_pf_ftf_j20_0eta290_pf_ftf_boffperf_preselj60XXj40_PhysicsTLA_L1J45p0ETA21_3J15p0ETA25', l1SeedThresholds=['FSNOSEED']*2, stream=['TLA'], groups=MultiBjetGroup+DevGroup),
        ChainProp(name='HLT_j140_pf_ftf_j20_0eta290_pf_ftf_boffperf_preselj140XXj45_PhysicsTLA_L1J45p0ETA21_3J15p0ETA25', l1SeedThresholds=['FSNOSEED']*2, stream=['TLA'], groups=MultiBjetGroup+DevGroup),
        ChainProp(name='HLT_j60_pf_ftf_3j20_pf_ftf_boffperf_preselj60XXj40_PhysicsTLA_L1J45p0ETA21_3J15p0ETA25', l1SeedThresholds=['FSNOSEED']*2, stream=['TLA'], groups=MultiBjetGroup+DevGroup),
        ChainProp(name='HLT_j60_pf_ftf_3j20_020jvt_pf_ftf_boffperf_preselj60XXj40_PhysicsTLA_L1J45p0ETA21_3J15p0ETA25', l1SeedThresholds=['FSNOSEED']*2, stream=['TLA'], groups=MultiBjetGroup+DevGroup),
        ChainProp(name='HLT_j20_0eta290_pf_ftf_boffperf_presel4j20_PhysicsTLA_L14J15p0ETA25', l1SeedThresholds=['FSNOSEED'], stream=['TLA'], groups=MultiBjetGroup+DevGroup),
        ChainProp(name='HLT_4j20_020jvt_pf_ftf_boffperf_presel4j20_PhysicsTLA_L14J15p0ETA25', l1SeedThresholds=['FSNOSEED'], stream=['TLA'], groups=MultiBjetGroup+DevGroup),
        ChainProp(name='HLT_j20_0eta290_pf_ftf_boffperf_presel4j25_PhysicsTLA_L14J15p0ETA25', l1SeedThresholds=['FSNOSEED'], stream=['TLA'], groups=MultiBjetGroup+DevGroup),
        ChainProp(name='HLT_4j20_020jvt_pf_ftf_boffperf_presel4j25_PhysicsTLA_L14J15p0ETA25', l1SeedThresholds=['FSNOSEED'], stream=['TLA'], groups=MultiBjetGroup+DevGroup),
    ]

    chains['Tau'] += [
        ChainProp(name="HLT_tau25_looseRNN_tracktwoMVA_L1TAU12IM", groups=SingleTauGroup),
        ChainProp(name="HLT_tau25_looseRNN_tracktwoMVABDT_L1TAU12IM", groups=SingleTauGroup),
        ChainProp(name="HLT_tau25_looseRNN_tracktwoLLP_L1TAU12IM", groups=SingleTauGroup),
        ChainProp(name="HLT_tau25_tightRNN_tracktwoMVA_L1TAU12IM", groups=SingleTauGroup),
        ChainProp(name="HLT_tau25_tightRNN_tracktwoMVABDT_L1TAU12IM", groups=SingleTauGroup),
        ChainProp(name="HLT_tau25_tightRNN_tracktwoLLP_L1TAU12IM", groups=SingleTauGroup),
        ChainProp(name="HLT_tau35_looseRNN_tracktwoMVA_L1TAU20IM", groups=SingleTauGroup),
        ChainProp(name="HLT_tau35_looseRNN_tracktwoMVABDT_L1TAU20IM", groups=SingleTauGroup),
        ChainProp(name="HLT_tau35_tightRNN_tracktwoMVA_L1TAU20IM", groups=SingleTauGroup),
        ChainProp(name="HLT_tau35_tightRNN_tracktwoMVABDT_L1TAU20IM", groups=SingleTauGroup),
        ChainProp(name="HLT_tau160_ptonly_L1TAU100", groups=SingleTauGroup),
    ]

    chains['Bphysics'] += [
        #ATR-21003; default dimuon and Bmumux chains from Run2; l2io validation; should not be moved to Physics
        ChainProp(name='HLT_2mu4_noL2Comb_bJpsimumu_L12MU3V', stream=["BphysDelayed"], groups=BphysicsGroup+DevGroup),
        ChainProp(name='HLT_mu6_noL2Comb_mu4_noL2Comb_bJpsimumu_L1MU5VF_2MU3V', l1SeedThresholds=['MU5VF','MU3V'], stream=["BphysDelayed"], groups=BphysicsGroup+DevGroup),
        ChainProp(name='HLT_2mu4_noL2Comb_bBmumux_BpmumuKp_L12MU3V', stream=["BphysDelayed"], groups=BphysicsGroup+DevGroup),
        ChainProp(name='HLT_2mu4_noL2Comb_bBmumux_BsmumuPhi_L12MU3V', stream=["BphysDelayed"], groups=BphysicsGroup+DevGroup),
        ChainProp(name='HLT_2mu4_noL2Comb_bBmumux_LbPqKm_L12MU3V', stream=["BphysDelayed"], groups=BphysicsGroup+DevGroup),

        #ATR-20603; B-trigger monitoring, ART tests; should not be moved to Physics
        ChainProp(name='HLT_2mu4_bJpsimumu_L12MU3V', stream=["BphysDelayed"], groups=BphysicsGroup+DevGroup, monGroups=['bphysMon:online','bphysMon:val']),
        ChainProp(name='HLT_2mu4_bUpsimumu_L12MU3V', stream=["BphysDelayed"], groups=BphysicsGroup+DevGroup),
        #ATR-20839; validation and ART; should not be moved to Physics
        ChainProp(name='HLT_2mu4_bDimu_L12MU3V', stream=["BphysDelayed"], groups=BphysicsGroup+DevGroup, monGroups=['bphysMon:online','bphysMon:val']),

        #ATR-21639; validation and monitoring of Bmumux chains; should not be moved to Physics
        ChainProp(name='HLT_2mu4_bBmumux_BpmumuKp_L12MU3V', stream=["BphysDelayed"], groups=BphysicsGroup+DevGroup, monGroups=['bphysMon:online','bphysMon:val']),
        ChainProp(name='HLT_2mu4_bBmumux_BcmumuPi_L12MU3V', stream=["BphysDelayed"], groups=BphysicsGroup+DevGroup),
        ChainProp(name='HLT_2mu4_bBmumux_BsmumuPhi_L12MU3V', stream=["BphysDelayed"], groups=BphysicsGroup+DevGroup, monGroups=['bphysMon:online','bphysMon:val']),
        ChainProp(name='HLT_2mu4_bBmumux_BdmumuKst_L12MU3V', stream=["BphysDelayed"], groups=BphysicsGroup+DevGroup, monGroups=['bphysMon:online','bphysMon:val']),
        ChainProp(name='HLT_2mu4_bBmumux_LbPqKm_L12MU3V', stream=["BphysDelayed"], groups=BphysicsGroup+DevGroup),
        ChainProp(name='HLT_2mu4_bBmumux_BcmumuDsloose_L12MU3V', stream=["BphysDelayed"], groups=BphysicsGroup+DevGroup),
        ChainProp(name='HLT_2mu4_bBmumux_BcmumuDploose_L12MU3V', stream=["BphysDelayed"], groups=BphysicsGroup+DevGroup),
        ChainProp(name='HLT_2mu4_bBmumux_BcmumuD0Xloose_L12MU3V', stream=["BphysDelayed"], groups=BphysicsGroup+DevGroup),
        ChainProp(name='HLT_2mu4_bBmumux_BcmumuDstarloose_L12MU3V', stream=["BphysDelayed"], groups=BphysicsGroup+DevGroup),
    ]

    chains['Combined'] += [
        
        # Test chains for muon + jet/MET merging/aligning
        ChainProp(name='HLT_mu6_xe30_mht_L1XE30', l1SeedThresholds=['MU5VF','FSNOSEED'], stream=[PhysicsStream], groups=MuonMETGroup),
        ChainProp(name='HLT_mu6_j45_nojcalib_L1J20', l1SeedThresholds=['MU5VF','FSNOSEED'], stream=[PhysicsStream], groups=MuonJetGroup),

        # tau+X chains (ATR-21609) 
        ChainProp(name='HLT_tau25_mediumRNN_tracktwoMVA_tau20_mediumRNN_tracktwoMVA_03dRAB_j70_0eta320_j50_0eta490_j0_DJMASS900j50_L1MJJ-500-NFF',l1SeedThresholds=['TAU8','TAU8','FSNOSEED','FSNOSEED','FSNOSEED'], groups=SupportLegGroup+TauJetGroup),
        # mu-tag & tau-probe triggers for LLP (ATR-23150)
        ChainProp(name='HLT_mu26_ivarmedium_tau100_mediumRNN_tracktwoLLP_03dRAB_L1MU14FCH', l1SeedThresholds=['MU14FCH','TAU60'], stream=[PhysicsStream], groups=TagAndProbeLegGroup+SingleMuonGroup),
        ChainProp(name='HLT_e26_lhtight_ivarloose_tau100_mediumRNN_tracktwoLLP_03dRAB_L1EM22VHI', l1SeedThresholds=['EM22VHI','TAU60'], stream=[PhysicsStream], groups=TagAndProbeLegGroup+SingleElectronGroup),
 
        # photon + multijets TLA
        ChainProp(name="HLT_g35_tight_3j25_PhysicsTLA_L1EM22VHI", stream=['TLA'], l1SeedThresholds=['EM22VHI','FSNOSEED'], groups=SupportLegGroup+EgammaJetGroup),
        ChainProp(name="HLT_g35_tight_3j25_pf_ftf_PhysicsTLA_L1EM22VHI", stream=['TLA'], l1SeedThresholds=['EM22VHI','FSNOSEED'], groups=PrimaryLegGroup+EgammaJetGroup),
        
        # tau + jet and tau + photon tag and probe (ATR-24031)
        # *** Temporarily commented because counts are fluctuating in CI and causing confusion ***
        #ChainProp(name='HLT_tau20_mediumRNN_tracktwoMVABDT_probe_j15_pf_ftf_03dRAB_L1RD0_FILLED', l1SeedThresholds=['PROBETAU8','FSNOSEED'], groups=TagAndProbeLegGroup+TauJetGroup),
        # *** Temporarily commented because counts are fluctuating in CI and causing confusion ***
        ChainProp(name='HLT_g140_loose_tau20_mediumRNN_tracktwoMVABDT_03dRAB_L1EM22VHI', l1SeedThresholds=['EM22VHI','PROBETAU8'], groups=TagAndProbeLegGroup+TauPhotonGroup),
 
        # Photon+MET reoptimised ATR-21565
        ChainProp(name='HLT_g25_tight_icalotight_xe40_cell_xe50_tcpufit_18dphiAB_18dphiAC_80mTAC_L1EM22VHI',l1SeedThresholds=['EM22VHI','FSNOSEED','FSNOSEED'],stream=[PhysicsStream], groups=PrimaryLegGroup+EgammaMETGroup),
        ChainProp(name='HLT_g25_tight_icalotight_xe40_cell_xe40_tcpufit_xe40_pfopufit_18dphiAB_18dphiAC_80mTAC_L1EM22VHI',l1SeedThresholds=['EM22VHI','FSNOSEED','FSNOSEED','FSNOSEED'],stream=[PhysicsStream], groups=PrimaryLegGroup+EgammaMETGroup),
        ChainProp(name='HLT_g25_tight_icalotight_xe40_cell_xe50_tcpufit_18dphiAB_18dphiAC_80mTAC_L1eEM26M',l1SeedThresholds=['eEM26M','FSNOSEED','FSNOSEED'],stream=[PhysicsStream], groups=PrimaryPhIGroup+EgammaMETGroup),
        ChainProp(name='HLT_g25_tight_icalotight_xe40_cell_xe40_tcpufit_xe40_pfopufit_18dphiAB_18dphiAC_80mTAC_L1eEM26M',l1SeedThresholds=['eEM26M','FSNOSEED','FSNOSEED','FSNOSEED'],stream=[PhysicsStream], groups=PrimaryPhIGroup+EgammaMETGroup),


    
        # meson + photon (ATR-22644, ATR-23239)
        ChainProp(name='HLT_g25_medium_tau25_dikaonmass_tracktwoMVA_50invmAB_L1EM22VHI', l1SeedThresholds=['EM22VHI','TAU8'], stream=[PhysicsStream], groups=PrimaryLegGroup+EgammaTauGroup),
        ChainProp(name='HLT_g25_medium_tau25_kaonpi1_tracktwoMVA_50invmAB_L1EM22VHI', l1SeedThresholds=['EM22VHI','TAU8'], stream=[PhysicsStream], groups=PrimaryLegGroup+EgammaTauGroup),
        ChainProp(name='HLT_g25_medium_tau25_kaonpi2_tracktwoMVA_50invmAB_L1EM22VHI', l1SeedThresholds=['EM22VHI','TAU8'], stream=[PhysicsStream], groups=PrimaryLegGroup+EgammaTauGroup),
        ChainProp(name='HLT_g25_medium_tau25_singlepion_tracktwoMVA_50invmAB_L1EM22VHI', l1SeedThresholds=['EM22VHI','TAU8'], stream=[PhysicsStream], groups=PrimaryLegGroup+EgammaTauGroup),
        ChainProp(name='HLT_g25_medium_tau25_dipion1_tracktwoMVA_50invmAB_L1EM22VHI', l1SeedThresholds=['EM22VHI','TAU8'], stream=[PhysicsStream], groups=PrimaryLegGroup+EgammaTauGroup),
        ChainProp(name='HLT_g25_medium_tau25_dipion2_tracktwoMVA_50invmAB_L1EM22VHI', l1SeedThresholds=['EM22VHI','TAU8'], stream=[PhysicsStream], groups=PrimaryLegGroup+EgammaTauGroup),
        ChainProp(name='HLT_g35_medium_tau25_dipion3_tracktwoMVA_60invmAB_L1EM22VHI', l1SeedThresholds=['EM22VHI','TAU8'], stream=[PhysicsStream], groups=PrimaryLegGroup+EgammaTauGroup),
        ChainProp(name='HLT_g25_medium_tau25_dipion4_tracktwoMVA_50invmAB_L1EM22VHI', l1SeedThresholds=['EM22VHI','TAU8'], stream=[PhysicsStream], groups=PrimaryLegGroup+EgammaTauGroup),

        # Tests of potential TLA chains for cost/rate
        # ATR-19317 - dijet+ISR 
        ChainProp(name='HLT_g35_loose_3j25_pf_ftf_L1EM22VHI',          l1SeedThresholds=['EM22VHI','FSNOSEED'], groups=EgammaJetGroup),
        ChainProp(name='HLT_g35_medium_3j25_pf_ftf_L1EM22VHI',         l1SeedThresholds=['EM22VHI','FSNOSEED'], groups=EgammaJetGroup),
        ChainProp(name='HLT_g35_tight_3j25_pf_ftf_L1EM22VHI',          l1SeedThresholds=['EM22VHI','FSNOSEED'], groups=EgammaJetGroup),
        ChainProp(name='HLT_g35_tight_3j25_0eta290_pf_ftf_boffperf_L1EM22VHI', l1SeedThresholds=['EM22VHI','FSNOSEED'], groups=EgammaJetGroup),


        # high-mu AFP
        ChainProp(name='HLT_2j20_mb_afprec_afpdijet_L1RD0_FILLED', l1SeedThresholds=['FSNOSEED']*2, stream=[PhysicsStream],groups=['PS:Online']+MinBiasGroup+SupportLegGroup),

        #ATR-23156 will be superseeded by ATR-24698
        ChainProp(name='HLT_mu4_j20_0eta290_pf_ftf_boffperf_dRAB03_L1MU3V', l1SeedThresholds=['MU3V','FSNOSEED'], groups=SingleBjetGroup),
        ChainProp(name='HLT_mu4_j35_0eta290_pf_ftf_boffperf_dRAB03_L1BTAG-MU3VjJ40', l1SeedThresholds=['MU3V','FSNOSEED'], groups=SingleBjetGroup),
        ChainProp(name='HLT_mu6_j45_0eta290_pf_ftf_boffperf_dRAB03_L1BTAG-MU5VFjJ50', l1SeedThresholds=['MU5VF','FSNOSEED'], groups=SingleBjetGroup),
    
        #ATR-24698
        #L1Topo
        ChainProp(name='HLT_mu4_j20_0eta290_pf_ftf_boffperf_dRAB04_L1MU3V', l1SeedThresholds=['MU3V','FSNOSEED'], groups=SingleBjetGroup),
        ChainProp(name='HLT_mu4_j35_0eta290_pf_ftf_boffperf_dRAB04_L1BTAG-MU3VjJ40', l1SeedThresholds=['MU3V','FSNOSEED'], groups=SingleBjetGroup),
        ChainProp(name='HLT_mu4_j45_0eta290_pf_ftf_boffperf_dRAB04_L1BTAG-MU3VjJ40', l1SeedThresholds=['MU3V','FSNOSEED'], groups=SingleBjetGroup),
        ChainProp(name='HLT_mu6_j60_0eta290_pf_ftf_boffperf_dRAB04_L1BTAG-MU3VjJ40', l1SeedThresholds=['MU3V','FSNOSEED'], groups=SingleBjetGroup),
        ChainProp(name='HLT_mu6_j100_0eta290_pf_ftf_boffperf_dRAB04_L1BTAG-MU5VFjJ90', l1SeedThresholds=['MU5VF','FSNOSEED'], groups=SingleBjetGroup),
        #jFEX
        ChainProp(name='HLT_mu4_j20_0eta290_pf_ftf_boffperf_dRAB04_L1MU3V_jJ30', l1SeedThresholds=['MU3V','FSNOSEED'], groups=SingleBjetGroup), # added temporarily 
        ChainProp(name='HLT_mu4_j35_0eta290_pf_ftf_boffperf_dRAB04_L1MU3V_jJ40', l1SeedThresholds=['MU3V','FSNOSEED'], groups=SingleBjetGroup),
        ChainProp(name='HLT_mu4_j45_0eta290_pf_ftf_boffperf_dRAB04_L1MU3V_jJ40', l1SeedThresholds=['MU3V','FSNOSEED'], groups=SingleBjetGroup),
        ChainProp(name='HLT_mu6_j60_0eta290_pf_ftf_boffperf_dRAB04_L1MU3V_jJ40', l1SeedThresholds=['MU3V','FSNOSEED'], groups=SingleBjetGroup),
        ChainProp(name='HLT_mu6_j100_0eta290_pf_ftf_boffperf_dRAB04_L1MU5VF_jJ90', l1SeedThresholds=['MU5VF','FSNOSEED'], groups=SingleBjetGroup),
        #Legacy
        ChainProp(name='HLT_mu4_j20_0eta290_pf_ftf_boffperf_dRAB04_L1MU3V_J12', l1SeedThresholds=['MU3V','FSNOSEED'], groups=SingleBjetGroup), # added temporarily 
        ChainProp(name='HLT_mu4_j35_0eta290_pf_ftf_boffperf_dRAB04_L1MU3V_J15', l1SeedThresholds=['MU3V','FSNOSEED'], groups=SingleBjetGroup),
        ChainProp(name='HLT_mu4_j45_0eta290_pf_ftf_boffperf_dRAB04_L1MU3V_J15', l1SeedThresholds=['MU3V','FSNOSEED'], groups=SingleBjetGroup),
        ChainProp(name='HLT_mu6_j60_0eta290_pf_ftf_boffperf_dRAB04_L1MU3V_J15', l1SeedThresholds=['MU3V','FSNOSEED'], groups=SingleBjetGroup),
        ChainProp(name='HLT_mu6_j100_0eta290_pf_ftf_boffperf_dRAB04_L1MU5VF_J40', l1SeedThresholds=['MU5VF','FSNOSEED'], groups=SingleBjetGroup),


        #ATR-23394
        ChainProp(name='HLT_e14_lhtight_mu6_dRAB15_invmAB10_L1LFV-eEM15L-MU5VF', l1SeedThresholds=['eEM12L','MU5VF'], stream=[PhysicsStream], groups=PrimaryPhIGroup+BphysicsGroup), #TODO: eEM15L->eEM12L in HLT seeding to fix
        ChainProp(name='HLT_e12_lhtight_mu11_dRAB15_invmAB10_L1LFV-eEM10L-MU8VF', l1SeedThresholds=['eEM10L','MU8VF'], stream=[PhysicsStream], groups=PrimaryPhIGroup+BphysicsGroup),    


        # Emerging Jets test chains ATR-21593
        # primary dijet + photon emerging jets chain
        ChainProp(name='HLT_g45_tight_icaloloose_2j55_pf_ftf_0eta200_emergingPTF0p1dR0p4_L1EM22VHI', groups=PrimaryLegGroup+EgammaJetGroup, l1SeedThresholds=['EM22VHI','FSNOSEED']),

        # backup dijet + photon emerging jets chain  
        ChainProp(name='HLT_g60_tight_icaloloose_2j55_pf_ftf_0eta200_emergingPTF0p1dR0p4_L1EM22VHI', groups=PrimaryLegGroup+EgammaJetGroup, l1SeedThresholds=['EM22VHI','FSNOSEED']),

        # Phase-I egamma+X chains with muon L1
        ChainProp(name='HLT_e7_lhmedium_L1eEM5_mu24_L1MU14FCH',l1SeedThresholds=['eEM5','MU14FCH'],  stream=[PhysicsStream], groups=PrimaryPhIGroup+EgammaMuonGroup),
        ChainProp(name='HLT_e9_lhvloose_L1eEM5_mu20_mu8noL1_L1MU14FCH', l1SeedThresholds=['eEM5','MU14FCH','FSNOSEED'], stream=[PhysicsStream], groups=PrimaryLegGroup+EgammaMuonGroup),
        ChainProp(name='HLT_e12_lhloose_L1eEM10L_2mu10_L12MU8F', l1SeedThresholds=['eEM10L','MU8F'], stream=[PhysicsStream], groups=PrimaryPhIGroup+EgammaMuonGroup),
        ChainProp(name='HLT_g25_medium_L1eEM18L_mu24_L1MU14FCH',l1SeedThresholds=['eEM18L','MU14FCH'], stream=[PhysicsStream], groups=PrimaryPhIGroup+EgammaMuonGroup), #ATR-22594
        ChainProp(name='HLT_g15_loose_L1eEM10L_2mu10_msonly_L1MU3V_EMPTY', l1SeedThresholds=['eEM10L','MU3V'], stream=['Late'], groups=PrimaryPhIGroup+EgammaMuonGroup),
        ChainProp(name='HLT_g15_loose_L1eEM10L_2mu10_msonly_L1MU5VF_EMPTY', l1SeedThresholds=['eEM10L','MU5VF'], stream=['Late'], groups=PrimaryPhIGroup+EgammaMuonGroup),
        ChainProp(name='HLT_g15_loose_L1eEM10L_2mu10_msonly_L1MU3V_UNPAIRED_ISO', l1SeedThresholds=['eEM10L','MU3V'], stream=['Late'], groups=PrimaryPhIGroup+EgammaMuonGroup),

    ]

    chains['Beamspot'] += [
        ChainProp(name='HLT_beamspot_allTE_trkfast_BeamSpotPEB_L1J15',  l1SeedThresholds=['FSNOSEED'], stream=['BeamSpot'], groups=['PS:Online', 'RATE:BeamSpot',  'BW:BeamSpot']),
    ]

    chains['MinBias'] += [

    ]

    chains['Calib'] += [
        #ChainProp(name='HLT_noalg_AlfaPEB_L1ALFA_ANY', l1SeedThresholds=['FSNOSEED'], stream=['ALFACalib'], groups=['RATE:ALFACalibration','BW:Detector']+LowMuGroup),
        # Calib Chains
        ChainProp(name='HLT_larpsallem_L1EM3', groups=SingleElectronGroup),
    ]

    chains['Streaming'] += [

        # ATR-24037
        ChainProp(name='HLT_noalg_L1jXEPerf100',     l1SeedThresholds=['FSNOSEED'], groups=METPhaseIStreamersGroup),

    ]

    chains['Monitor'] += [
       ChainProp(name='HLT_l1topodebug_legacy_L1All', l1SeedThresholds=['FSNOSEED'], stream=['L1TopoMismatches'], groups=['PS:Online', 'RATE:Monitoring', 'BW:Other']),
    ]

    chains['UnconventionalTracking'] += [
        #Isolated High Pt Trigger Test chain for optimisation studies
        ChainProp(name='HLT_unconvtrk50_isohpttrack_L1XE50', groups=SingleMETGroup, l1SeedThresholds=['FSNOSEED']),


        ChainProp(name='HLT_unconvtrk0_fslrt_L1J100', groups=SingleJetGroup, l1SeedThresholds=['FSNOSEED']),
        ChainProp(name='HLT_unconvtrk0_fslrt_L14J15', groups=MultiJetGroup, l1SeedThresholds=['FSNOSEED']),
        ChainProp(name='HLT_unconvtrk0_fslrt_L1XE50', groups=SingleMETGroup, l1SeedThresholds=['FSNOSEED']),

        ChainProp(name='HLT_unconvtrk0_dispj_L1J100', groups=SingleJetGroup, l1SeedThresholds=['FSNOSEED'], monGroups=['DJTrigMon:online']), 


        # disappearing track trigger
        ChainProp(name='HLT_unconvtrk20_distrk_tight_L1XE50',               groups=SupportLegGroup+UnconvTrkGroup+['RATE:CPS_XE50'], l1SeedThresholds=['FSNOSEED']),
        ChainProp(name='HLT_unconvtrk20_distrk_medium_L1XE50',              groups=SupportLegGroup+UnconvTrkGroup+['RATE:CPS_XE50'], l1SeedThresholds=['FSNOSEED']),
        # Phase-I L1Calo
        ChainProp(name='HLT_unconvtrk20_distrk_tight_L1jXE100',             groups=SupportPhIGroup+UnconvTrkGroup+['RATE:CPS_jXE100'], l1SeedThresholds=['FSNOSEED']),
        ChainProp(name='HLT_unconvtrk20_distrk_medium_L1jXE100',            groups=SupportPhIGroup+UnconvTrkGroup+['RATE:CPS_jXE100'], l1SeedThresholds=['FSNOSEED']),
    ]

    return chains
