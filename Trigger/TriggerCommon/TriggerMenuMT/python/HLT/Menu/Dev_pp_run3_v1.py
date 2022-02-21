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
                                                                 MultiTauGroup,
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
                                                                 PrimaryL1MuGroup,
                                                                 PrimaryLegGroup,
                                                                 PrimaryPhIGroup,
                                                                 SupportGroup,
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

        #-- nscan ATR-19376, TODO: to be moved to physics once debugged
        ChainProp(name='HLT_mu20_msonly_iloosems_mu6noL1_msonly_nscan_L1MU14FCH_J40', l1SeedThresholds=['MU14FCH','FSNOSEED'], groups=PrimaryLegGroup+MultiMuonGroup),
        ChainProp(name='HLT_mu20_msonly_iloosems_mu6noL1_msonly_nscan_L1MU14FCH_jJ80', l1SeedThresholds=['MU14FCH','FSNOSEED'], groups=PrimaryPhIGroup+MultiMuonGroup),
        ChainProp(name='HLT_mu20_msonly_iloosems_mu6noL1_msonly_nscan_L1MU14FCH_XE30', l1SeedThresholds=['MU14FCH','FSNOSEED'], groups=PrimaryLegGroup+MultiMuonGroup),
        ChainProp(name='HLT_mu20_msonly_iloosems_mu6noL1_msonly_nscan_L1MU14FCH_jXE70', l1SeedThresholds=['MU14FCH','FSNOSEED'], groups=PrimaryPhIGroup+MultiMuonGroup),
        ChainProp(name='HLT_mu20_msonly_iloosems_mu6noL1_msonly_nscan_L110DR-MU14FCH-MU5VF', l1SeedThresholds=['MU14FCH','FSNOSEED'],   groups=PrimaryL1MuGroup+MultiMuonGroup),
        ## ATR-24198
        ChainProp(name='HLT_mu20_msonly_iloosems_mu6noL1_msonly_nscan_L1MU14FCH', l1SeedThresholds=['MU14FCH','FSNOSEED'], groups=SupportGroup+MultiMuonGroup),

    ]

    chains['Egamma'] += [
        # ElectronChains----------


        # Keep for validation of the AB/AC/BC overlap-removal, remove after
        ChainProp(name='HLT_e24_lhmedium_g12_loose_g12_loose_02dRAB_02dRAC_L1EM20VH_3EM10VH', l1SeedThresholds=['EM20VH','EM10VH','EM10VH'], groups=DevGroup+MultiElectronGroup),


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

    ]

    chains['MET'] += [

        ChainProp(name='HLT_xe30_cell_L1XE30', l1SeedThresholds=['FSNOSEED'], groups=SingleMETGroup),
        ChainProp(name='HLT_xe30_mht_L1XE30', l1SeedThresholds=['FSNOSEED'], groups=SingleMETGroup),
        ChainProp(name='HLT_xe30_tcpufit_L1XE30', l1SeedThresholds=['FSNOSEED'], groups=SingleMETGroup),
        ChainProp(name='HLT_xe30_trkmht_L1XE30', l1SeedThresholds=['FSNOSEED'], groups=SingleMETGroup),
        ChainProp(name='HLT_xe30_pfsum_L1XE30', l1SeedThresholds=['FSNOSEED'], groups=SingleMETGroup),
        ChainProp(name='HLT_xe30_pfsum_cssk_L1XE30', l1SeedThresholds=['FSNOSEED'], groups=SingleMETGroup),
        ChainProp(name='HLT_xe30_pfsum_vssk_L1XE30', l1SeedThresholds=['FSNOSEED'], groups=SingleMETGroup),
        ChainProp(name='HLT_xe30_pfopufit_L1XE30', l1SeedThresholds=['FSNOSEED'], groups=SingleMETGroup),
        ChainProp(name='HLT_xe30_cvfpufit_L1XE30', l1SeedThresholds=['FSNOSEED'], groups=SingleMETGroup),
        ChainProp(name='HLT_xe30_mhtpufit_em_subjesgscIS_L1XE30', l1SeedThresholds=['FSNOSEED'], groups=SingleMETGroup),
        ChainProp(name='HLT_xe30_mhtpufit_pf_subjesgscIS_L1XE30', l1SeedThresholds=['FSNOSEED'], groups=SingleMETGroup),

        ChainProp(name='HLT_xe110_tc_em_L1XE50', l1SeedThresholds=['FSNOSEED'], groups=SingleMETGroup),
        ChainProp(name='HLT_xe110_mht_L1XE50', l1SeedThresholds=['FSNOSEED'], groups=SingleMETGroup),
        ChainProp(name='HLT_xe110_tcpufit_L1XE50', l1SeedThresholds=['FSNOSEED'], groups=SingleMETGroup),
        ChainProp(name='HLT_xe110_pfsum_L1XE50', l1SeedThresholds=['FSNOSEED'], groups=SingleMETGroup),
        ChainProp(name='HLT_xe110_pfsum_cssk_L1XE50', l1SeedThresholds=['FSNOSEED'], groups=SingleMETGroup),
        ChainProp(name='HLT_xe110_pfsum_vssk_L1XE50', l1SeedThresholds=['FSNOSEED'], groups=SingleMETGroup),

        # Test chains to determine rate after calo-only preselection for tracking
        ChainProp(name='HLT_xe60_cell_L1XE50', l1SeedThresholds=['FSNOSEED'], groups=SingleMETGroup),
        ChainProp(name='HLT_xe55_cell_xe70_tcpufit_L1XE50', l1SeedThresholds=['FSNOSEED']*2, groups=MultiMETGroup),
    ]


    chains['Jet'] += [
        ### PURE TEST CHAINS

        # Low-pt jet chains -- keep any?
        ChainProp(name='HLT_j85_L1J20', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup+DevGroup),
        ChainProp(name='HLT_j85_ftf_L1J20', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup+DevGroup),
        ChainProp(name='HLT_j80_j60_SHARED_j40__L1J15', l1SeedThresholds=['FSNOSEED']*3, groups=MultiJetGroup+DevGroup),
        ChainProp(name='HLT_j85_CLEANlb_L1J20', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup+DevGroup),
        ChainProp(name='HLT_j85_CLEANllp_L1J20', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup+DevGroup),
        ChainProp(name='HLT_j85_pf_ftf_L1J20', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup),

        # j45 calibration variants -- Are any for support?
        # Do we need nojcalib chains considering that we get the nojcalib jets from other chains anyway?
        # EMTopo
        ChainProp(name='HLT_j45_L1J15', l1SeedThresholds=['FSNOSEED'], groups=['PS:Online']+SingleJetGroup),
        ChainProp(name='HLT_j45_nojcalib_L1J15', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup),
        ChainProp(name='HLT_j45_subjesgsc_ftf_L1J15', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup),
        ChainProp(name='HLT_j45_subjesgscIS_ftf_L1J15', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup),
        ChainProp(name='HLT_j45_subresjesgsc_ftf_L1J15', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup),
        ChainProp(name='HLT_j45_ftf_L1J15', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup),
        # PFlow
        ChainProp(name='HLT_j45_pf_ftf_L1J15', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup),
        ChainProp(name='HLT_j45_pf_nojcalib_ftf_L1J15', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup),
        ChainProp(name='HLT_j45_pf_subjesgsc_ftf_L1J15', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup),
        ChainProp(name='HLT_j45_pf_subjesgscIS_ftf_L1J15', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup),
        ChainProp(name='HLT_j45_pf_subresjesgsc_ftf_L1J15', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup),
        # SK/CSSK test chains -- remove?
        ChainProp(name='HLT_j45_sk_nojcalib_L1J15', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup+DevGroup),
        ChainProp(name='HLT_j45_cssk_nojcalib_L1J15', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup+DevGroup),
        ChainProp(name='HLT_j45_cssk_pf_nojcalib_ftf_L1J15', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup+DevGroup),

        # Test chains for JVT -- keep any?
        ChainProp(name='HLT_j45_subjesgscIS_ftf_011jvt_L1J15', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup+DevGroup),
        ChainProp(name='HLT_j45_subjesgscIS_ftf_015jvt_L1J15', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup+DevGroup),
        ChainProp(name='HLT_j45_subjesgscIS_ftf_059jvt_L1J15', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup+DevGroup),
        ChainProp(name='HLT_j45_pf_ftf_010jvt_L1J15', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup),
        ChainProp(name='HLT_j45_pf_ftf_020jvt_L1J15', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup),
        ChainProp(name='HLT_j45_pf_ftf_050jvt_L1J15', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup),

        # Test chains with moment cuts -- still needed?
        ChainProp(name='HLT_j85_050momemfrac100_L1J20', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup+DevGroup),
        ChainProp(name='HLT_j85_momhecfrac010_L1J20', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup+DevGroup),
        ChainProp(name='HLT_j85_050momemfrac100XXmomhecfrac010_L1J20', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup+DevGroup),

        # Test chains with loose L1 seed, can remove?
        ChainProp(name='HLT_j260_320eta490_L1J20', l1SeedThresholds=['FSNOSEED'], groups=['PS:Online']+SingleJetGroup+DevGroup),
        ChainProp(name='HLT_j460_a10_lcw_subjes_L1J20', l1SeedThresholds=['FSNOSEED'], groups=['PS:Online']+SingleJetGroup+DevGroup),
        ChainProp(name='HLT_j460_a10r_L1J20', l1SeedThresholds=['FSNOSEED'], groups=['PS:Online']+SingleJetGroup+DevGroup),

        # Uncalibrated large-R jet chains, needed?
        ChainProp(name='HLT_j460_a10t_lcw_nojcalib_L1J100', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup),
        ChainProp(name='HLT_j460_a10t_lcw_nojcalib_35smcINF_L1J100', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup),
        ChainProp(name='HLT_2j330_a10t_lcw_nojcalib_35smcINF_L1J100', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup),
        ChainProp(name='HLT_j460_a10sd_lcw_nojcalib_L1J100', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup),
        #
        ChainProp(name='HLT_j460_a10sd_pf_nojcalib_ftf_L1J100', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup),
        ChainProp(name='HLT_j460_a10sd_cssk_pf_nojcalib_ftf_L1J100', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup),
        ChainProp(name='HLT_j460_a10sd_cssk_pf_nojcalib_ftf_35smcINF_L1J100', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup),
        ChainProp(name='HLT_2j330_a10sd_cssk_pf_nojcalib_ftf_35smcINF_L1J100', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup),

        # Preselection thresholds for large-R jets, for rate determination
        ChainProp(name='HLT_j140_a10_tc_em_nojcalib_L1J100', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup+DevGroup),
        ChainProp(name='HLT_j180_a10_tc_em_nojcalib_L1J100', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup+DevGroup),
        ChainProp(name='HLT_j225_a10_tc_em_nojcalib_L1J100', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup+DevGroup),
        ChainProp(name='HLT_j140_a10_tc_em_nojcalib_L1SC111-CJ15', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup+DevGroup),
        ChainProp(name='HLT_j180_a10_tc_em_nojcalib_L1SC111-CJ15', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup+DevGroup),
        ChainProp(name='HLT_j225_a10_tc_em_nojcalib_L1SC111-CJ15', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup+DevGroup),
        # Large-R jets without preselections
        ChainProp(name='HLT_j460_a10sd_cssk_pf_jes_ftf_L1J100', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup+DevGroup),
        ChainProp(name='HLT_j460_a10sd_cssk_pf_jes_ftf_L1SC111-CJ15', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup+DevGroup),
        # CSSKPFlow
        ChainProp(name='HLT_j420_a10sd_cssk_pf_jes_ftf_35smcINF_L1J100', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup+DevGroup),
        ChainProp(name='HLT_2j330_a10sd_cssk_pf_jes_ftf_35smcINF_L1J100', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup+DevGroup),

        ChainProp(name='HLT_j0_FBDJNOSHARED10etXX20etXX34massXX50fbet_L1J20', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup+DevGroup),
        ChainProp(name='HLT_j0_FBDJSHARED_L1J20', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup+DevGroup),
        ChainProp(name='HLT_j60_j0_FBDJSHARED_L1J20', l1SeedThresholds=['FSNOSEED']*2, groups=MultiJetGroup+DevGroup),

        # HT chains using pt conditions
        ChainProp(name='HLT_j0_HT1000_L1J20', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup+DevGroup),
        ChainProp(name='HLT_j0_HT500_L1J20', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup+DevGroup),
        ChainProp(name='HLT_j0_pf_ftf_HT50_L1J20', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup+DevGroup),
        ChainProp(name='HLT_j0_pf_ftf_HT50XX010jvt_L1J20', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup+DevGroup),

        # HT chains using et conditions
        ChainProp(name='HLT_j0_HT1000XX30et_L1J20', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup+DevGroup),
        ChainProp(name='HLT_j0_HT500XX30et_L1J20', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup+DevGroup),
        ChainProp(name='HLT_j0_pf_ftf_HT50XX30et_L1J20', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup+DevGroup),
        ChainProp(name='HLT_j0_pf_ftf_HT50XX30etXX010jvt_L1J20', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup+DevGroup),
        # multiscenario with HT chains using pt conditions
        ChainProp(name='HLT_j0_HT1000_j0_DIJET80j12ptXX0j12eta240XX700djmass_L1J20', l1SeedThresholds=['FSNOSEED']*2, groups=MultiJetGroup+DevGroup),

        # dijet chains using pt
        ChainProp(name='HLT_j0_DIJET80j12ptXX0j12eta240XX700djmass_L1J20', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup+DevGroup),
        ChainProp(name='HLT_j0_DIJET80j12ptXX700djmassXXdjdphi260_L1J20', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup+DevGroup),
        ChainProp(name='HLT_j0_DIJET70j12ptXX1000djmassXXdjdphi200XX400djdeta_L1J20', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup+DevGroup),
        ChainProp(name='HLT_j0_DIJET20j12ptXX110djmass_L1J20', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup+DevGroup),
        ChainProp(name='HLT_j0_DIJET20j12ptXX110djmass_PTRANGE2r3_L1J20', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup+DevGroup),

        # dijet chains using et
        ChainProp(name='HLT_j0_DIJET80j12etXX0j12eta240XX700djmass_L1J20', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup+DevGroup),
        ChainProp(name='HLT_j0_DIJET80j12etXX700djmassXXdjdphi260_L1J20', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup+DevGroup),
        ChainProp(name='HLT_j0_DIJET70j12etXX1000djmassXXdjdphi200XX400djdeta_L1J20', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup+DevGroup),
        ChainProp(name='HLT_j0_DIJET20j12etXX110djmass_L1J20', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup+DevGroup),
        ChainProp(name='HLT_j0_DIJET20j12etXX110djmass_PTRANGE2r3_L1J20', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup+DevGroup),

        ChainProp(name='HLT_j85_ftf_MASK300ceta210XX300nphi10_L1J20', l1SeedThresholds=['FSNOSEED'],
                  groups=SingleJetGroup+DevGroup),

        ChainProp(name='HLT_j40_j0_HT50XX10etXX0eta320_L1J20',
                  l1SeedThresholds=['FSNOSEED']*2, groups=MultiJetGroup+DevGroup),
        ChainProp(name='HLT_j40_j0_HT50XX10ptXX0eta320_L1J20',
                  l1SeedThresholds=['FSNOSEED']*2, groups=MultiJetGroup+DevGroup), # same as above but using pt conditions

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

        #  Chains to test JVT and PFlow effects in low-threshold 6j
        ChainProp(name='HLT_6j25_0eta240_L14J15', l1SeedThresholds=['FSNOSEED'],            groups=MultiJetGroup+DevGroup),
        ChainProp(name='HLT_6j25_ftf_0eta240_L14J15', l1SeedThresholds=['FSNOSEED'],        groups=MultiJetGroup+DevGroup),
        ChainProp(name='HLT_6j25_ftf_0eta240_010jvt_L14J15', l1SeedThresholds=['FSNOSEED'], groups=MultiJetGroup+DevGroup),
        ChainProp(name='HLT_6j25_ftf_0eta240_020jvt_L14J15', l1SeedThresholds=['FSNOSEED'], groups=MultiJetGroup+DevGroup),
        ChainProp(name='HLT_6j25_ftf_0eta240_050jvt_L14J15', l1SeedThresholds=['FSNOSEED'], groups=MultiJetGroup+DevGroup),

        ChainProp(name='HLT_6j35_0eta240_L14J15', l1SeedThresholds=['FSNOSEED'],            groups=MultiJetGroup+DevGroup),
        ChainProp(name='HLT_6j35_ftf_0eta240_L14J15', l1SeedThresholds=['FSNOSEED'],        groups=MultiJetGroup+DevGroup),
        ChainProp(name='HLT_6j35_ftf_0eta240_010jvt_L14J15', l1SeedThresholds=['FSNOSEED'], groups=MultiJetGroup+DevGroup),
        ChainProp(name='HLT_6j35_ftf_0eta240_020jvt_L14J15', l1SeedThresholds=['FSNOSEED'], groups=MultiJetGroup+DevGroup),
        ChainProp(name='HLT_6j35_ftf_0eta240_050jvt_L14J15', l1SeedThresholds=['FSNOSEED'], groups=MultiJetGroup+DevGroup),

        ChainProp(name='HLT_6j45_0eta240_L14J15', l1SeedThresholds=['FSNOSEED'],            groups=MultiJetGroup+DevGroup),
        ChainProp(name='HLT_6j45_ftf_0eta240_L14J15', l1SeedThresholds=['FSNOSEED'],        groups=MultiJetGroup+DevGroup),
        ChainProp(name='HLT_6j45_ftf_0eta240_010jvt_L14J15', l1SeedThresholds=['FSNOSEED'], groups=MultiJetGroup+DevGroup),
        ChainProp(name='HLT_6j45_ftf_0eta240_020jvt_L14J15', l1SeedThresholds=['FSNOSEED'], groups=MultiJetGroup+DevGroup),
        ChainProp(name='HLT_6j45_ftf_0eta240_050jvt_L14J15', l1SeedThresholds=['FSNOSEED'], groups=MultiJetGroup+DevGroup),

        ChainProp(name='HLT_6j25_pf_ftf_0eta240_L14J15', l1SeedThresholds=['FSNOSEED'],        groups=MultiJetGroup+DevGroup),
        ChainProp(name='HLT_6j25_pf_ftf_0eta240_010jvt_L14J15', l1SeedThresholds=['FSNOSEED'], groups=MultiJetGroup+DevGroup),
        ChainProp(name='HLT_6j25_pf_ftf_0eta240_020jvt_L14J15', l1SeedThresholds=['FSNOSEED'], groups=MultiJetGroup+DevGroup),
        ChainProp(name='HLT_6j25_pf_ftf_0eta240_050jvt_L14J15', l1SeedThresholds=['FSNOSEED'], groups=MultiJetGroup+DevGroup),

        ChainProp(name='HLT_6j35_pf_ftf_0eta240_L14J15', l1SeedThresholds=['FSNOSEED'],        groups=MultiJetGroup+DevGroup),
        ChainProp(name='HLT_6j35_pf_ftf_0eta240_010jvt_L14J15', l1SeedThresholds=['FSNOSEED'], groups=MultiJetGroup+DevGroup),
        ChainProp(name='HLT_6j35_pf_ftf_0eta240_020jvt_L14J15', l1SeedThresholds=['FSNOSEED'], stream=['VBFDelayed'], groups=MultiJetGroup+DevGroup),
        ChainProp(name='HLT_6j35_pf_ftf_0eta240_050jvt_L14J15', l1SeedThresholds=['FSNOSEED'], groups=MultiJetGroup+DevGroup),

        ChainProp(name='HLT_6j45_pf_ftf_0eta240_L14J15', l1SeedThresholds=['FSNOSEED'],        groups=MultiJetGroup+DevGroup),
        ChainProp(name='HLT_6j45_pf_ftf_0eta240_010jvt_L14J15', l1SeedThresholds=['FSNOSEED'], groups=MultiJetGroup+DevGroup),
        ChainProp(name='HLT_6j45_pf_ftf_0eta240_020jvt_L14J15', l1SeedThresholds=['FSNOSEED'], stream=['VBFDelayed'], groups=MultiJetGroup+DevGroup),
        ChainProp(name='HLT_6j45_pf_ftf_0eta240_050jvt_L14J15', l1SeedThresholds=['FSNOSEED'], groups=MultiJetGroup+DevGroup),

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

        ChainProp(name='HLT_j420_a10sd_cssk_pf_jes_ftf_35smcINF_L1SC111-CJ15', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup+DevGroup),
        ChainProp(name='HLT_2j330_a10sd_cssk_pf_jes_ftf_35smcINF_L1SC111-CJ15', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup+DevGroup),
        ChainProp(name='HLT_j360_a10sd_cssk_pf_jes_ftf_60smcINF_j360_a10sd_cssk_pf_jes_ftf_L1SC111-CJ15', l1SeedThresholds=['FSNOSEED']*2, groups=DevGroup+MultiJetGroup),
        ChainProp(name='HLT_j370_a10sd_cssk_pf_jes_ftf_35smcINF_j370_a10sd_cssk_pf_jes_ftf_L1SC111-CJ15', l1SeedThresholds=['FSNOSEED']*2, groups=DevGroup+MultiJetGroup),

        # HH4b -- need any for support?
        # PFlow variants
        ChainProp(name='HLT_j80_pf_ftf_0eta240_020jvt_j55_pf_ftf_0eta240_020jvt_j28_pf_ftf_0eta240_020jvt_j20_pf_ftf_0eta240_020jvt_L1J45p0ETA21_3J15p0ETA25', l1SeedThresholds=['FSNOSEED']*4, groups=MultiJetGroup+DevGroup),
        ChainProp(name='HLT_j80_pf_ftf_0eta240_j55_pf_ftf_0eta240_j28_pf_ftf_0eta240_j20_pf_ftf_0eta240_L1J45p0ETA21_3J15p0ETA25', l1SeedThresholds=['FSNOSEED']*4, groups=MultiJetGroup+DevGroup),
        # Adjusted thresholds for BSM (k_l=10)
        ChainProp(name='HLT_j75_pf_ftf_0eta240_020jvt_j50_pf_ftf_0eta240_020jvt_j35_pf_ftf_0eta240_020jvt_j25_pf_ftf_0eta240_020jvt_L1J45p0ETA21_3J15p0ETA25', l1SeedThresholds=['FSNOSEED']*4, groups=MultiJetGroup+DevGroup),
        # Mu-seeded
        ChainProp(name='HLT_j80_pf_ftf_0eta240_020jvt_j55_pf_ftf_0eta240_020jvt_j28_pf_ftf_0eta240_020jvt_j20_pf_ftf_0eta240_020jvt_L1MU14FCH', l1SeedThresholds=['FSNOSEED']*4, groups=MultiJetGroup+DevGroup),
        ChainProp(name='HLT_j80_pf_ftf_0eta240_j55_pf_ftf_0eta240_j28_pf_ftf_0eta240_j20_pf_ftf_0eta240_L1MU14FCH', l1SeedThresholds=['FSNOSEED']*4, groups=MultiJetGroup+DevGroup),
        ChainProp(name='HLT_j80_pf_ftf_0eta240_020jvt_j55_pf_ftf_0eta240_020jvt_j28_pf_ftf_0eta240_020jvt_j20_pf_ftf_0eta240_020jvt_L1MU8F_2J15_J20', l1SeedThresholds=['FSNOSEED']*4, groups=MultiJetGroup+DevGroup),
        ChainProp(name='HLT_j80_pf_ftf_0eta240_j55_pf_ftf_0eta240_j28_pf_ftf_0eta240_j20_pf_ftf_0eta240_L1MU8F_2J15_J20', l1SeedThresholds=['FSNOSEED']*4, groups=MultiJetGroup+DevGroup),
        # Test chains to check tracking/b-tagging rates for Run 2 HH4b chain
        ChainProp(name="HLT_2j35_0eta240_020jvt_pf_ftf_2j35_0eta240_020jvt_pf_ftf_L14J15p0ETA25", l1SeedThresholds=['FSNOSEED','FSNOSEED'], groups=MultiBjetGroup+DevGroup),
        ChainProp(name="HLT_2j35_0eta240_2j35_0eta240_L14J15p0ETA25", l1SeedThresholds=['FSNOSEED','FSNOSEED'], groups=MultiBjetGroup+DevGroup),

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


        # ATR-24720 Testing additions to Run 3 baseline menu
        # Find lower threshold for lowest unprescaled single forward jet trigger
        ChainProp(name='HLT_j200_320eta490_L1J75p31ETA49', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup+DevGroup),
        ChainProp(name='HLT_j210_320eta490_L1J75p31ETA49', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup+DevGroup),
        ChainProp(name='HLT_j220_320eta490_L1J75p31ETA49', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup+DevGroup),
        ChainProp(name='HLT_j230_320eta490_L1J75p31ETA49', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup+DevGroup),
        ChainProp(name='HLT_j240_320eta490_L1J75p31ETA49', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup+DevGroup),
        ChainProp(name='HLT_j250_320eta490_L1J75p31ETA49', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup+DevGroup),
        # HT preselection studies
        ChainProp(name='HLT_j0_HT1000_pf_ftf_preselj140_L1HT190-J15s5pETA21', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup+DevGroup),
        ChainProp(name='HLT_j0_HT1000_pf_ftf_presel3j45_L1HT190-J15s5pETA21', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup+DevGroup),
        ChainProp(name='HLT_j0_HT1000_pf_ftf_presel4j40_L1HT190-J15s5pETA21', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup+DevGroup),
        ChainProp(name='HLT_j0_HT1000_pf_ftf_presel4c40_L1HT190-J15s5pETA21', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup+DevGroup),
        ChainProp(name='HLT_j0_HT1000_pf_ftf_presel4j45_L1HT190-J15s5pETA21', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup+DevGroup),
        ChainProp(name='HLT_j0_HT1000_pf_ftf_presel4j50_L1HT190-J15s5pETA21', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup+DevGroup),
        ChainProp(name='HLT_j0_HT1000_pf_ftf_presel5j25_L1HT190-J15s5pETA21', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup+DevGroup),

        ### END PURE TEST CHAINS
 
        # FTF+EMTopo
        ChainProp(name='HLT_j420_ftf_preselj225_L1J100', l1SeedThresholds=['FSNOSEED'],             groups=SingleJetGroup+SupportLegGroup),
        ChainProp(name='HLT_2j250_ftf_0eta240_j120_ftf_0eta240_presel2j180XXj80_L1J100', l1SeedThresholds=['FSNOSEED']*2, groups=MultiJetGroup+SupportLegGroup ),
        ChainProp(name='HLT_3j200_ftf_presel3j150_L1J100', l1SeedThresholds=['FSNOSEED'],            groups=MultiJetGroup+SupportLegGroup),
        ChainProp(name='HLT_4j115_ftf_presel4j85_L13J50', l1SeedThresholds=['FSNOSEED'],            groups=MultiJetGroup+SupportLegGroup),
        ChainProp(name='HLT_5j70_ftf_0eta240_presel5j50_L14J15', l1SeedThresholds=['FSNOSEED'],     groups=MultiJetGroup+SupportLegGroup),
        ChainProp(name='HLT_5j85_ftf_presel5j50_L14J15', l1SeedThresholds=['FSNOSEED'],             groups=MultiJetGroup+SupportLegGroup),
        ChainProp(name='HLT_6j55_ftf_0eta240_presel6j40_L14J15', l1SeedThresholds=['FSNOSEED'],     groups=MultiJetGroup+SupportLegGroup),
        ChainProp(name='HLT_6j70_ftf_presel6j40_L14J15', l1SeedThresholds=['FSNOSEED'],             groups=MultiJetGroup+SupportLegGroup),
        ChainProp(name='HLT_7j45_ftf_presel7j30_L14J15', l1SeedThresholds=['FSNOSEED'],             groups=MultiJetGroup+SupportLegGroup),
        ChainProp(name='HLT_10j40_ftf_presel7j30_L14J15', l1SeedThresholds=['FSNOSEED'],            groups=MultiJetGroup+SupportLegGroup),

        # Central Exclusive Production for SM group
        ChainProp(name='HLT_2j100_L1CEP-CjJ90', l1SeedThresholds=['FSNOSEED'], groups=PrimaryPhIGroup+MultiJetGroup),
        ChainProp(name='HLT_2j100_L1CEP-CjJ100', l1SeedThresholds=['FSNOSEED'], groups=PrimaryPhIGroup+MultiJetGroup),

        # TLA test chains, ATR-20395
        ChainProp(name='HLT_j20_pf_ftf_preselj140_PhysicsTLA_L1J50', l1SeedThresholds=['FSNOSEED'], stream=['TLA'], groups=EOFTLALegGroup+SingleJetGroup),
        ChainProp(name='HLT_j20_pf_ftf_preselj140_PhysicsTLA_L1J100', l1SeedThresholds=['FSNOSEED'], stream=['TLA'], groups=PrimaryLegGroup+SingleJetGroup),
        ChainProp(name='HLT_j20_pf_ftf_preselj180_PhysicsTLA_L1J100', l1SeedThresholds=['FSNOSEED'], stream=['TLA'], groups=PrimaryLegGroup+SingleJetGroup),
        ChainProp(name='HLT_j20_pf_ftf_PhysicsTLA_L1J100', l1SeedThresholds=['FSNOSEED'], stream=['TLA'], groups=PrimaryLegGroup+SingleJetGroup),
        ChainProp(name='HLT_j0_pf_ftf_DJMASS500j35_PhysicsTLA_L1J50', l1SeedThresholds=['FSNOSEED'], stream=['TLA'], groups=EOFTLALegGroup+SingleJetGroup),
        ChainProp(name='HLT_j0_pf_ftf_DJMASS200j20_PhysicsTLA_L1J50', l1SeedThresholds=['FSNOSEED'], stream=['TLA'], groups=EOFTLALegGroup+SingleJetGroup),
        ChainProp(name='HLT_j0_pf_ftf_DJMASS350j20_PhysicsTLA_L1J50', l1SeedThresholds=['FSNOSEED'], stream=['TLA'], groups=EOFTLALegGroup+SingleJetGroup),
        ChainProp(name='HLT_j0_pf_ftf_DJMASS350j20_PhysicsTLA_L1J100', l1SeedThresholds=['FSNOSEED'], stream=['TLA'], groups=PrimaryLegGroup+SingleJetGroup),
        ChainProp(name='HLT_j0_pf_ftf_DJMASS500j35_PhysicsTLA_L1J100', l1SeedThresholds=['FSNOSEED'], stream=['TLA'], groups=PrimaryLegGroup+SingleJetGroup),
        ChainProp(name='HLT_j0_pf_ftf_DJMASS200j20_PhysicsTLA_L1J100', l1SeedThresholds=['FSNOSEED'], stream=['TLA'], groups=PrimaryLegGroup+SingleJetGroup),
        ChainProp(name='HLT_j0_pf_ftf_DJMASS500j35_preselj180_PhysicsTLA_L1J100', l1SeedThresholds=['FSNOSEED'], stream=['TLA'], groups=PrimaryLegGroup+SingleJetGroup),
        ChainProp(name='HLT_j0_pf_ftf_DJMASS350j20_preselj180_PhysicsTLA_L1J100', l1SeedThresholds=['FSNOSEED'], stream=['TLA'], groups=PrimaryLegGroup+SingleJetGroup),
        ChainProp(name='HLT_j0_pf_ftf_DJMASS200j20_preselj180_PhysicsTLA_L1J100', l1SeedThresholds=['FSNOSEED'], stream=['TLA'], groups=PrimaryLegGroup+SingleJetGroup),

        # multijet TLA test
        ChainProp(name='HLT_j60_pf_ftf_j45_pf_ftf_2j20_pf_ftf_PhysicsTLA_L1J45p0ETA21_3J15p0ETA25', l1SeedThresholds=['FSNOSEED']*3, stream=['TLA'], groups=PrimaryLegGroup+MultiJetGroup),
        ChainProp(name='HLT_j60_0eta290_020jvt_pf_ftf_j45_0eta290_020jvt_pf_ftf_2j20_0eta290_020jvt_pf_ftf_PhysicsTLA_L1J45p0ETA21_3J15p0ETA25', l1SeedThresholds=['FSNOSEED']*3, stream=['TLA'], groups=PrimaryLegGroup+MultiJetGroup),

        # ATR-24838 Large R L1J100 jet chains with jLJ L1 items (L1J100->L1jLJ140)
        ChainProp(name='HLT_j175_a10sd_cssk_pf_jes_ftf_0eta180_emergingPTF0p08dR1p2_L1jLJ140', groups=SingleJetGroup+PrimaryPhIGroup, l1SeedThresholds=['FSNOSEED']),
        ChainProp(name='HLT_2j110_a10sd_cssk_pf_jes_ftf_0eta200_emergingPTF0p1dR1p2_L1jLJ140', groups=SingleJetGroup+PrimaryPhIGroup, l1SeedThresholds=['FSNOSEED']),
        ChainProp(name='HLT_2j110_a10sd_cssk_pf_jes_ftf_0eta180_emergingPTF0p09dR1p2_L1jLJ140', groups=SingleJetGroup+PrimaryPhIGroup, l1SeedThresholds=['FSNOSEED']),
        ChainProp(name='HLT_j175_a10r_subjesIS_ftf_0eta200_tracklessdR1p2_L1jLJ140',    groups=SingleJetGroup+PrimaryPhIGroup, l1SeedThresholds=['FSNOSEED']),
        ChainProp(name='HLT_j260_a10r_subjesIS_ftf_0eta200_tracklessdR1p2_L1jLJ140',    groups=SingleJetGroup+PrimaryPhIGroup, l1SeedThresholds=['FSNOSEED']),

        ]
        

    chains['Bjet'] += [
        # these chains are taken from the Run 2 menu for now --- likely to be loosened
        ChainProp(name="HLT_j275_0eta290_020jvt_pf_ftf_bdl1r60_L1J100", l1SeedThresholds=['FSNOSEED'], groups=DevGroup+SingleBjetGroup),
        ChainProp(name="HLT_j300_0eta290_020jvt_pf_ftf_bdl1r70_L1J100", l1SeedThresholds=['FSNOSEED'], groups=DevGroup+SingleBjetGroup),
        ChainProp(name="HLT_j360_0eta290_020jvt_pf_ftf_bdl1r77_L1J100", l1SeedThresholds=['FSNOSEED'], groups=DevGroup+SingleBjetGroup),

        # dl1d test chains
        ChainProp(name="HLT_j275_0eta290_020jvt_pf_ftf_bdl1d60_L1J100", l1SeedThresholds=['FSNOSEED'], groups=DevGroup+SingleBjetGroup),
        ChainProp(name="HLT_j300_0eta290_020jvt_pf_ftf_bdl1d70_L1J100", l1SeedThresholds=['FSNOSEED'], groups=DevGroup+SingleBjetGroup),
        ChainProp(name="HLT_j360_0eta290_020jvt_pf_ftf_bdl1d77_L1J100", l1SeedThresholds=['FSNOSEED'], groups=DevGroup+SingleBjetGroup),

        # HH4b primary candidates with 2 sets of potential jet thresholds
        # 3b85 symmetric b-jet pt for Physics_Main
        ChainProp(name='HLT_j80_pf_ftf_0eta240_020jvt_j55_pf_ftf_0eta240_020jvt_j28_pf_ftf_0eta240_020jvt_j20_pf_ftf_0eta240_020jvt_SHARED_3j20_pf_ftf_0eta240_020jvt_bdl1r85_L1J45p0ETA21_3J15p0ETA25', l1SeedThresholds=['FSNOSEED']*5, stream=[PhysicsStream], groups=DevGroup+MultiBjetGroup),
        ChainProp(name='HLT_j75_pf_ftf_0eta240_020jvt_j50_pf_ftf_0eta240_020jvt_j35_pf_ftf_0eta240_020jvt_j25_pf_ftf_0eta240_020jvt_SHARED_3j25_pf_ftf_0eta240_020jvt_bdl1r85_L1J45p0ETA21_3J15p0ETA25', l1SeedThresholds=['FSNOSEED']*5, stream=[PhysicsStream], groups=DevGroup+MultiBjetGroup),
        # 2b60 asymmetric b-jet pt alternative for Physics_Main
        ChainProp(name='HLT_j80_pf_ftf_0eta240_020jvt_j55_pf_ftf_0eta240_020jvt_j28_pf_ftf_0eta240_020jvt_j20_pf_ftf_0eta240_020jvt_SHARED_j28_pf_ftf_0eta240_020jvt_bdl1r60_j20_pf_ftf_0eta240_020jvt_bdl1r60_L1J45p0ETA21_3J15p0ETA25', l1SeedThresholds=['FSNOSEED']*6, stream=['VBFDelayed'], groups=DevGroup+MultiBjetGroup),
        ChainProp(name='HLT_j75_pf_ftf_0eta240_020jvt_j50_pf_ftf_0eta240_020jvt_j35_pf_ftf_0eta240_020jvt_j25_pf_ftf_0eta240_020jvt_SHARED_j35_pf_ftf_0eta240_020jvt_bdl1r60_j25_pf_ftf_0eta240_020jvt_bdl1r60_L1J45p0ETA21_3J15p0ETA25', l1SeedThresholds=['FSNOSEED']*6, stream=['VBFDelayed'], groups=DevGroup+MultiBjetGroup),
        # 2b77 symmetric b-jet pt for VBFDelayed
        ChainProp(name='HLT_j80_pf_ftf_0eta240_020jvt_j55_pf_ftf_0eta240_020jvt_j28_pf_ftf_0eta240_020jvt_j20_pf_ftf_0eta240_020jvt_SHARED_2j20_pf_ftf_0eta240_020jvt_bdl1r77_L1J45p0ETA21_3J15p0ETA25', l1SeedThresholds=['FSNOSEED']*5, stream=['VBFDelayed'], groups=DevGroup+MultiBjetGroup),
        ChainProp(name='HLT_j75_pf_ftf_0eta240_020jvt_j50_pf_ftf_0eta240_020jvt_j35_pf_ftf_0eta240_020jvt_j25_pf_ftf_0eta240_020jvt_SHARED_2j25_pf_ftf_0eta240_020jvt_bdl1r77_L1J45p0ETA21_3J15p0ETA25', l1SeedThresholds=['FSNOSEED']*5, stream=['VBFDelayed'], groups=DevGroup+MultiBjetGroup),

        ChainProp(name="HLT_j225_0eta290_pf_ftf_bdl1r70_L1J100", l1SeedThresholds=['FSNOSEED'], groups=SingleBjetGroup + DevGroup),
        ChainProp(name="HLT_j225_0eta290_pf_ftf_bdl1r77_L1J100", l1SeedThresholds=['FSNOSEED'], groups=SingleBjetGroup + DevGroup),
        ChainProp(name='HLT_j275_0eta290_pf_ftf_bdl1r85_L1J100', l1SeedThresholds=['FSNOSEED'], groups=SingleBjetGroup + DevGroup),
        ChainProp(name='HLT_j300_0eta290_pf_ftf_bdl1r85_L1J100', l1SeedThresholds=['FSNOSEED'], groups=SingleBjetGroup + DevGroup),

        ChainProp(name="HLT_3j65_0eta290_020jvt_pf_ftf_bdl1r77_L13J35p0ETA23", l1SeedThresholds=['FSNOSEED'], groups=MultiBjetGroup+DevGroup),
        ChainProp(name="HLT_4j35_0eta290_020jvt_pf_ftf_bdl1r77_L14J15p0ETA25", l1SeedThresholds=['FSNOSEED'], groups=MultiBjetGroup+DevGroup),

        # single bjet pflow options, # changes according to ATR-23883
        ChainProp(name="HLT_j225_0eta290_pf_ftf_bdl1r60_L1J100", l1SeedThresholds=['FSNOSEED'], groups=SingleBjetGroup + DevGroup),
        ChainProp(name="HLT_j225_0eta290_pf_ftf_bdl1r85_L1J100", l1SeedThresholds=['FSNOSEED'], groups=SingleBjetGroup + DevGroup),

        ChainProp(name='HLT_j275_0eta290_pf_ftf_bdl1r70_L1J100', l1SeedThresholds=['FSNOSEED'], groups=SingleBjetGroup + DevGroup),
        ChainProp(name='HLT_j275_0eta290_pf_ftf_bdl1r77_L1J100', l1SeedThresholds=['FSNOSEED'], groups=SingleBjetGroup + DevGroup),

        ChainProp(name='HLT_j300_0eta290_pf_ftf_bdl1r60_L1J100', l1SeedThresholds=['FSNOSEED'], groups=SingleBjetGroup + DevGroup),
        ChainProp(name='HLT_j300_0eta290_pf_ftf_bdl1r77_L1J100', l1SeedThresholds=['FSNOSEED'], groups=SingleBjetGroup + DevGroup),

        ChainProp(name='HLT_j360_0eta290_pf_ftf_bdl1r60_L1J100', l1SeedThresholds=['FSNOSEED'], groups=SingleBjetGroup + DevGroup),
        ChainProp(name='HLT_j360_0eta290_pf_ftf_bdl1r70_L1J100', l1SeedThresholds=['FSNOSEED'], groups=SingleBjetGroup + DevGroup),
        ChainProp(name='HLT_j360_0eta290_pf_ftf_bdl1r85_L1J100', l1SeedThresholds=['FSNOSEED'], groups=SingleBjetGroup + DevGroup),

        # for monitoring
        ### IS THIS SUPPORT?
        ChainProp(name='HLT_j45_0eta290_020jvt_pf_ftf_bdl1r70_L1J20', l1SeedThresholds=['FSNOSEED'], groups=SingleBjetGroup),

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

        # Test chains for ttbar and HH->4b chains
        # 3j20_b[70,77,85]
        ChainProp(name='HLT_j80_pf_ftf_0eta240_020jvt_j55_pf_ftf_0eta240_020jvt_j28_pf_ftf_0eta240_020jvt_j20_pf_ftf_0eta240_020jvt_SHARED_3j20_pf_ftf_0eta240_020jvt_bdl1r70_L1J45p0ETA21_3J15p0ETA25', l1SeedThresholds=['FSNOSEED']*5, stream=[PhysicsStream], groups=MultiBjetGroup+DevGroup),
        ChainProp(name='HLT_j80_pf_ftf_0eta240_020jvt_j55_pf_ftf_0eta240_020jvt_j28_pf_ftf_0eta240_020jvt_j20_pf_ftf_0eta240_020jvt_SHARED_3j20_pf_ftf_0eta240_020jvt_bdl1r77_L1J45p0ETA21_3J15p0ETA25', l1SeedThresholds=['FSNOSEED']*5, stream=[PhysicsStream], groups=MultiBjetGroup+DevGroup),
        # 2j20_b[60,70,77,85]
        ChainProp(name='HLT_j80_pf_ftf_0eta240_020jvt_j55_pf_ftf_0eta240_020jvt_j28_pf_ftf_0eta240_020jvt_j20_pf_ftf_0eta240_020jvt_SHARED_2j20_pf_ftf_0eta240_020jvt_bdl1r60_L1J45p0ETA21_3J15p0ETA25', l1SeedThresholds=['FSNOSEED']*5, stream=['VBFDelayed'], groups=MultiBjetGroup+DevGroup),
        ChainProp(name='HLT_j80_pf_ftf_0eta240_020jvt_j55_pf_ftf_0eta240_020jvt_j28_pf_ftf_0eta240_020jvt_j20_pf_ftf_0eta240_020jvt_SHARED_2j20_pf_ftf_0eta240_020jvt_bdl1r70_L1J45p0ETA21_3J15p0ETA25', l1SeedThresholds=['FSNOSEED']*5, stream=['VBFDelayed'], groups=MultiBjetGroup+DevGroup),
        ChainProp(name='HLT_j80_pf_ftf_0eta240_020jvt_j55_pf_ftf_0eta240_020jvt_j28_pf_ftf_0eta240_020jvt_j20_pf_ftf_0eta240_020jvt_SHARED_2j20_pf_ftf_0eta240_020jvt_bdl1r85_L1J45p0ETA21_3J15p0ETA25', l1SeedThresholds=['FSNOSEED']*5, stream=['VBFDelayed'], groups=MultiBjetGroup+DevGroup),
        # j[55,28,20]_b[70,77,85]
        ChainProp(name='HLT_j80_pf_ftf_0eta240_020jvt_j55_pf_ftf_0eta240_020jvt_j28_pf_ftf_0eta240_020jvt_j20_pf_ftf_0eta240_020jvt_SHARED_j55_pf_ftf_0eta240_020jvt_bdl1r70_j28_pf_ftf_0eta240_020jvt_bdl1r70_j20_pf_ftf_0eta240_020jvt_bdl1r70_L1J45p0ETA21_3J15p0ETA25', l1SeedThresholds=['FSNOSEED']*7, stream=[PhysicsStream], groups=MultiBjetGroup+DevGroup),
        ChainProp(name='HLT_j80_pf_ftf_0eta240_020jvt_j55_pf_ftf_0eta240_020jvt_j28_pf_ftf_0eta240_020jvt_j20_pf_ftf_0eta240_020jvt_SHARED_j55_pf_ftf_0eta240_020jvt_bdl1r77_j28_pf_ftf_0eta240_020jvt_bdl1r77_j20_pf_ftf_0eta240_020jvt_bdl1r77_L1J45p0ETA21_3J15p0ETA25', l1SeedThresholds=['FSNOSEED']*7, stream=[PhysicsStream], groups=MultiBjetGroup+DevGroup),
        ChainProp(name='HLT_j80_pf_ftf_0eta240_020jvt_j55_pf_ftf_0eta240_020jvt_j28_pf_ftf_0eta240_020jvt_j20_pf_ftf_0eta240_020jvt_SHARED_j55_pf_ftf_0eta240_020jvt_bdl1r85_j28_pf_ftf_0eta240_020jvt_bdl1r85_j20_pf_ftf_0eta240_020jvt_bdl1r85_L1J45p0ETA21_3J15p0ETA25', l1SeedThresholds=['FSNOSEED']*7, stream=[PhysicsStream], groups=MultiBjetGroup+DevGroup),
        # j[28,20]_b[60,70,77,85]
        ChainProp(name='HLT_j80_pf_ftf_0eta240_020jvt_j55_pf_ftf_0eta240_020jvt_j28_pf_ftf_0eta240_020jvt_j20_pf_ftf_0eta240_020jvt_SHARED_j28_pf_ftf_0eta240_020jvt_bdl1r70_j20_pf_ftf_0eta240_020jvt_bdl1r70_L1J45p0ETA21_3J15p0ETA25', l1SeedThresholds=['FSNOSEED']*6, stream=['VBFDelayed'], groups=MultiBjetGroup+DevGroup),
        ChainProp(name='HLT_j80_pf_ftf_0eta240_020jvt_j55_pf_ftf_0eta240_020jvt_j28_pf_ftf_0eta240_020jvt_j20_pf_ftf_0eta240_020jvt_SHARED_j28_pf_ftf_0eta240_020jvt_bdl1r77_j20_pf_ftf_0eta240_020jvt_bdl1r77_L1J45p0ETA21_3J15p0ETA25', l1SeedThresholds=['FSNOSEED']*6, stream=['VBFDelayed'], groups=MultiBjetGroup+DevGroup),
        ChainProp(name='HLT_j80_pf_ftf_0eta240_020jvt_j55_pf_ftf_0eta240_020jvt_j28_pf_ftf_0eta240_020jvt_j20_pf_ftf_0eta240_020jvt_SHARED_j28_pf_ftf_0eta240_020jvt_bdl1r85_j20_pf_ftf_0eta240_020jvt_bdl1r85_L1J45p0ETA21_3J15p0ETA25', l1SeedThresholds=['FSNOSEED']*6, stream=['VBFDelayed'], groups=MultiBjetGroup+DevGroup),
        # Adjusted thresholds for BSM (k_l=10)
        ChainProp(name='HLT_j75_pf_ftf_0eta240_020jvt_j50_pf_ftf_0eta240_020jvt_j35_pf_ftf_0eta240_020jvt_j25_pf_ftf_0eta240_020jvt_SHARED_j50_pf_ftf_0eta240_020jvt_bdl1r70_j35_pf_ftf_0eta240_020jvt_bdl1r70_j25_pf_ftf_0eta240_020jvt_bdl1r70_L1J45p0ETA21_3J15p0ETA25', l1SeedThresholds=['FSNOSEED']*7, stream=[PhysicsStream], groups=MultiBjetGroup+DevGroup),
        ChainProp(name='HLT_j75_pf_ftf_0eta240_020jvt_j50_pf_ftf_0eta240_020jvt_j35_pf_ftf_0eta240_020jvt_j25_pf_ftf_0eta240_020jvt_SHARED_j50_pf_ftf_0eta240_020jvt_bdl1r77_j35_pf_ftf_0eta240_020jvt_bdl1r77_j25_pf_ftf_0eta240_020jvt_bdl1r77_L1J45p0ETA21_3J15p0ETA25', l1SeedThresholds=['FSNOSEED']*7, stream=[PhysicsStream], groups=MultiBjetGroup+DevGroup),
        ChainProp(name='HLT_j75_pf_ftf_0eta240_020jvt_j50_pf_ftf_0eta240_020jvt_j35_pf_ftf_0eta240_020jvt_j25_pf_ftf_0eta240_020jvt_SHARED_j50_pf_ftf_0eta240_020jvt_bdl1r85_j35_pf_ftf_0eta240_020jvt_bdl1r85_j25_pf_ftf_0eta240_020jvt_bdl1r85_L1J45p0ETA21_3J15p0ETA25', l1SeedThresholds=['FSNOSEED']*7, stream=[PhysicsStream], groups=MultiBjetGroup+DevGroup),
        ChainProp(name='HLT_j75_pf_ftf_0eta240_020jvt_j50_pf_ftf_0eta240_020jvt_j35_pf_ftf_0eta240_020jvt_j25_pf_ftf_0eta240_020jvt_SHARED_j35_pf_ftf_0eta240_020jvt_bdl1r70_j25_pf_ftf_0eta240_020jvt_bdl1r70_L1J45p0ETA21_3J15p0ETA25', l1SeedThresholds=['FSNOSEED']*6, stream=['VBFDelayed'], groups=MultiBjetGroup+DevGroup),
        ChainProp(name='HLT_j75_pf_ftf_0eta240_020jvt_j50_pf_ftf_0eta240_020jvt_j35_pf_ftf_0eta240_020jvt_j25_pf_ftf_0eta240_020jvt_SHARED_j35_pf_ftf_0eta240_020jvt_bdl1r77_j25_pf_ftf_0eta240_020jvt_bdl1r77_L1J45p0ETA21_3J15p0ETA25', l1SeedThresholds=['FSNOSEED']*6, stream=['VBFDelayed'], groups=MultiBjetGroup+DevGroup),
        ChainProp(name='HLT_j75_pf_ftf_0eta240_020jvt_j50_pf_ftf_0eta240_020jvt_j35_pf_ftf_0eta240_020jvt_j25_pf_ftf_0eta240_020jvt_SHARED_j35_pf_ftf_0eta240_020jvt_bdl1r85_j25_pf_ftf_0eta240_020jvt_bdl1r85_L1J45p0ETA21_3J15p0ETA25', l1SeedThresholds=['FSNOSEED']*6, stream=['VBFDelayed'], groups=MultiBjetGroup+DevGroup),
        ChainProp(name='HLT_j75_pf_ftf_0eta240_020jvt_j50_pf_ftf_0eta240_020jvt_j35_pf_ftf_0eta240_020jvt_j25_pf_ftf_0eta240_020jvt_SHARED_3j25_pf_ftf_0eta240_020jvt_bdl1r77_L1J45p0ETA21_3J15p0ETA25', l1SeedThresholds=['FSNOSEED']*5, stream=[PhysicsStream], groups=MultiBjetGroup+DevGroup),
        ChainProp(name='HLT_j75_pf_ftf_0eta240_020jvt_j50_pf_ftf_0eta240_020jvt_j35_pf_ftf_0eta240_020jvt_j25_pf_ftf_0eta240_020jvt_SHARED_3j25_pf_ftf_0eta240_020jvt_bdl1r70_L1J45p0ETA21_3J15p0ETA25', l1SeedThresholds=['FSNOSEED']*5, stream=['VBFDelayed'], groups=MultiBjetGroup+DevGroup),
        ChainProp(name='HLT_j75_pf_ftf_0eta240_020jvt_j50_pf_ftf_0eta240_020jvt_j35_pf_ftf_0eta240_020jvt_j25_pf_ftf_0eta240_020jvt_SHARED_2j25_pf_ftf_0eta240_020jvt_bdl1r60_L1J45p0ETA21_3J15p0ETA25', l1SeedThresholds=['FSNOSEED']*5, stream=['VBFDelayed'], groups=MultiBjetGroup+DevGroup),
        ChainProp(name='HLT_j75_pf_ftf_0eta240_020jvt_j50_pf_ftf_0eta240_020jvt_j35_pf_ftf_0eta240_020jvt_j25_pf_ftf_0eta240_020jvt_SHARED_2j25_pf_ftf_0eta240_020jvt_bdl1r70_L1J45p0ETA21_3J15p0ETA25', l1SeedThresholds=['FSNOSEED']*5, stream=['VBFDelayed'], groups=MultiBjetGroup+DevGroup),
        ChainProp(name='HLT_j75_pf_ftf_0eta240_020jvt_j50_pf_ftf_0eta240_020jvt_j35_pf_ftf_0eta240_020jvt_j25_pf_ftf_0eta240_020jvt_SHARED_2j25_pf_ftf_0eta240_020jvt_bdl1r85_L1J45p0ETA21_3J15p0ETA25', l1SeedThresholds=['FSNOSEED']*5, stream=['VBFDelayed'], groups=MultiBjetGroup+DevGroup),
        # Copies with L1 muon+jet seed
        # j[28,20]_b60
        ChainProp(name='HLT_j80_pf_ftf_0eta240_020jvt_j55_pf_ftf_0eta240_020jvt_j28_pf_ftf_0eta240_020jvt_j20_pf_ftf_0eta240_020jvt_SHARED_j28_pf_ftf_0eta240_020jvt_bdl1r70_j20_pf_ftf_0eta240_020jvt_bdl1r70_L1MU8F_2J15_J20', l1SeedThresholds=['FSNOSEED']*6, stream=['VBFDelayed'], groups=PrimaryLegGroup+MultiBjetGroup),
        # j20_b60
        ChainProp(name='HLT_j80_pf_ftf_0eta240_020jvt_j55_pf_ftf_0eta240_020jvt_j28_pf_ftf_0eta240_020jvt_j20_pf_ftf_0eta240_020jvt_bdl1r70_L1MU8F_2J15_J20', l1SeedThresholds=['FSNOSEED']*4, stream=['VBFDelayed'], groups=PrimaryLegGroup+MultiBjetGroup),

        # Boffperf to facilitate emulation studies
        ChainProp(name='HLT_j80_pf_ftf_j55_pf_ftf_j28_pf_ftf_j20_0eta290_pf_ftf_boffperf_L1J45p0ETA21_3J15p0ETA25', l1SeedThresholds=['FSNOSEED']*4, groups=MultiBjetGroup+DevGroup),

        ChainProp(name='HLT_5j25_pf_ftf_0eta240_020jvt_j25_pf_ftf_0eta240_020jvt_bdl1r60_L14J15', l1SeedThresholds=['FSNOSEED']*2, groups=MultiBjetGroup+DevGroup),
        ChainProp(name='HLT_5j35_pf_ftf_0eta240_020jvt_j25_pf_ftf_0eta240_020jvt_bdl1r60_L14J15', l1SeedThresholds=['FSNOSEED']*2, stream=['VBFDelayed'], groups=PrimaryLegGroup+MultiBjetGroup),
        ChainProp(name='HLT_5j45_pf_ftf_0eta240_020jvt_j25_pf_ftf_0eta240_020jvt_bdl1r60_L14J15', l1SeedThresholds=['FSNOSEED']*2, stream=['VBFDelayed'], groups=PrimaryLegGroup+MultiBjetGroup),

        # Boffperf copy to facilitate emulation studies
        ChainProp(name='HLT_5j25_pf_ftf_0eta240_j25_pf_ftf_0eta240_boffperf_L14J15', l1SeedThresholds=['FSNOSEED']*2, groups=MultiBjetGroup+DevGroup),

        # VBF chains
        ChainProp(name='HLT_j80_pf_ftf_0eta240_j60_pf_ftf_0eta320_j45_pf_ftf_320eta490_SHARED_2j45_pf_ftf_0eta290_bdl1r60_L1J40p0ETA25_2J25_J20p31ETA49', l1SeedThresholds=['FSNOSEED']*4, groups=PrimaryLegGroup+MultiBjetGroup),
        ChainProp(name="HLT_j80_pf_ftf_0eta320_bdl1r70_j60_pf_ftf_0eta320_bdl1r85_j45_pf_ftf_320eta490_L1J40p0ETA25_2J25_J20p31ETA49", l1SeedThresholds=['FSNOSEED']*3,stream=[PhysicsStream], groups=PrimaryLegGroup+MultiBjetGroup),
        ChainProp(name="HLT_j55_pf_ftf_0eta320_bdl1r70_2j45_pf_ftf_320eta490_L1J25p0ETA23_2J15p31ETA49",l1SeedThresholds=['FSNOSEED']*2,  stream=[PhysicsStream], groups=PrimaryLegGroup+MultiBjetGroup),
        ChainProp(name='HLT_j70_pf_ftf_0eta490_j50_pf_ftf_0eta490_2j35_pf_ftf_0eta490_SHARED_2j35_pf_ftf_0eta290_bdl1r70_j0_pf_ftf_DJMASS1000j50_L1MJJ-500-NFF', l1SeedThresholds=['FSNOSEED']*5,stream=['VBFDelayed'], groups=PrimaryLegGroup+MultiBjetGroup),

        # Various multi-b 
        ChainProp(name="HLT_j150_0eta320_pf_ftf_2j55_0eta290_020jvt_pf_ftf_bdl1r70_L1J85_3J30", l1SeedThresholds=['FSNOSEED','FSNOSEED'], groups=PrimaryLegGroup+MultiBjetGroup),
        ChainProp(name="HLT_3j35_0eta290_020jvt_pf_ftf_bdl1r70_j35_pf_ftf_0eta320_L14J15p0ETA25", l1SeedThresholds=['FSNOSEED','FSNOSEED'], groups=PrimaryLegGroup+MultiBjetGroup),
        ChainProp(name="HLT_j175_0eta290_020jvt_pf_ftf_bdl1r60_j60_0eta290_020jvt_pf_ftf_bdl1r60_L1J100", l1SeedThresholds=['FSNOSEED','FSNOSEED'], groups=PrimaryLegGroup+MultiBjetGroup),
        ChainProp(name="HLT_2j35_0eta290_020jvt_pf_ftf_bdl1r70_2j35_0eta290_020jvt_pf_ftf_bdl1r85_L14J15p0ETA25", l1SeedThresholds=['FSNOSEED','FSNOSEED'], groups=PrimaryLegGroup+MultiBjetGroup),
        ChainProp(name="HLT_2j55_0eta290_020jvt_pf_ftf_bdl1r60_2j55_pf_ftf_0eta320_L14J15p0ETA25", l1SeedThresholds=['FSNOSEED','FSNOSEED'], groups=PrimaryLegGroup+MultiBjetGroup),
        ChainProp(name="HLT_2j35_0eta290_020jvt_pf_ftf_bdl1r60_3j35_pf_ftf_0eta320_L15J15p0ETA25", l1SeedThresholds=['FSNOSEED','FSNOSEED'], groups=PrimaryLegGroup+MultiBjetGroup),
        ChainProp(name="HLT_2j45_0eta290_020jvt_pf_ftf_bdl1r60_3j45_pf_ftf_0eta320_L15J15p0ETA25", l1SeedThresholds=['FSNOSEED','FSNOSEED'], groups=PrimaryLegGroup+MultiBjetGroup),
        ChainProp(name="HLT_j75_0eta290_020jvt_pf_ftf_bdl1r60_3j75_pf_ftf_L14J20", l1SeedThresholds=['FSNOSEED','FSNOSEED'], groups=PrimaryLegGroup+MultiBjetGroup),
        ChainProp(name="HLT_2j45_0eta290_020jvt_pf_ftf_bdl1r60_2j45_pf_ftf_L14J15p0ETA25", l1SeedThresholds=['FSNOSEED','FSNOSEED'], groups=PrimaryLegGroup+MultiBjetGroup),
        # Run 2 HH4b low-threshold chain
        ChainProp(name="HLT_2j35_0eta240_020jvt_pf_ftf_bdl1r60_2j35_0eta240_020jvt_pf_ftf_L14J15p0ETA25", l1SeedThresholds=['FSNOSEED','FSNOSEED'], groups=PrimaryLegGroup+MultiBjetGroup),

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

        # very loose chain for tagger training
        ChainProp(name='HLT_j20_0eta290_020jvt_pf_ftf_boffperf_L1J15', l1SeedThresholds=['FSNOSEED'], groups=SingleBjetGroup+DevGroup),

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
        #ATR-20049
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
        ChainProp(name="HLT_tau180_tightRNN_tracktwoLLP_L1TAU100", groups=SupportLegGroup+SingleTauGroup),    # 

        # displaced tau+X (ATR-21754)
        ChainProp(name='HLT_tau35_mediumRNN_tracktwoMVA_tau25_mediumRNN_tracktwoMVA_03dRAB30_L1DR-TAU20ITAU12I',         l1SeedThresholds=['TAU20IM','TAU12IM'], groups=SupportLegGroup+MultiTauGroup),
        ChainProp(name='HLT_tau35_mediumRNN_tracktwoMVABDT_tau25_mediumRNN_tracktwoMVABDT_03dRAB30_L1DR-TAU20ITAU12I',   l1SeedThresholds=['TAU20IM','TAU12IM'], groups=SupportLegGroup+MultiTauGroup),
        ChainProp(name='HLT_tau35_mediumRNN_tracktwoMVA_tau25_mediumRNN_tracktwoMVA_03dRAB_L1TAU20IM_2TAU12IM',    l1SeedThresholds=['TAU20IM','TAU12IM'], groups=SupportLegGroup+MultiTauGroup),
        ChainProp(name='HLT_tau35_mediumRNN_tracktwoMVABDT_tau25_mediumRNN_tracktwoMVABDT_03dRAB_L1TAU20IM_2TAU12IM', l1SeedThresholds=['TAU20IM','TAU12IM'], groups=SupportLegGroup+MultiTauGroup),

        # More Phase-I tau chains (ATR-24182)
        ChainProp(name='HLT_tau200_mediumRNN_tracktwoMVABDT_L1eTAU140', groups=PrimaryPhIGroup+SingleTauGroup),
        ChainProp(name='HLT_tau80_mediumRNN_tracktwoMVABDT_tau60_mediumRNN_tracktwoMVABDT_03dRAB_L1eTAU80_2eTAU60', l1SeedThresholds=['eTAU80','eTAU60'], groups=PrimaryPhIGroup+MultiTauGroup),
        ChainProp(name='HLT_tau80_mediumRNN_tracktwoMVABDT_tau35_mediumRNN_tracktwoMVABDT_03dRAB30_L1eTAU80_2cTAU20M_DR-eTAU30eTAU20', l1SeedThresholds=['eTAU80','eTAU20'], groups=PrimaryPhIGroup+MultiTauGroup), # TODO: seeding from cTAU20M
        ChainProp(name='HLT_tau35_mediumRNN_tracktwoMVABDT_tau25_mediumRNN_tracktwoMVABDT_03dRAB30_L1cTAU30M_2cTAU20M_DR-eTAU30MeTAU20M-jJ55', l1SeedThresholds=['eTAU30','eTAU20'], groups=PrimaryPhIGroup+MultiTauGroup), # TODO: seeding from cTAU20M,cTAU30M
        ChainProp(name='HLT_tau35_mediumRNN_tracktwoMVABDT_tau25_mediumRNN_tracktwoMVABDT_03dRAB_L1cTAU30M_2cTAU20M_4jJ30p0ETA25', l1SeedThresholds=['eTAU30','eTAU20'], groups=PrimaryPhIGroup+MultiTauGroup), # TODO: seeding from cTAU20M,cTAU30M
        ChainProp(name='HLT_tau40_mediumRNN_tracktwoMVABDT_tau35_mediumRNN_tracktwoMVABDT_03dRAB_L1cTAU35M_2cTAU30M_2jJ55_3jJ50', l1SeedThresholds=['eTAU35','eTAU30'], groups=PrimaryPhIGroup+MultiTauGroup), # TODO: seeding from cTAU30M,cTAU35M
        ChainProp(name="HLT_tau80_mediumRNN_tracktwoLLP_tau60_mediumRNN_tracktwoLLP_03dRAB_L1eTAU80_2eTAU60", l1SeedThresholds=['eTAU80','eTAU60'], groups=PrimaryPhIGroup+MultiTauGroup), 

        # Phase-I support
        #ChainProp(name="HLT_tau25_mediumRNN_tracktwoMVABDT_L1cTAU20",   groups=SupportPhIGroup+SingleTauGroup), #TODO: cTAU seeding missing
        ChainProp(name="HLT_tau35_mediumRNN_tracktwoMVABDT_L1eTAU30",   groups=SupportPhIGroup+SingleTauGroup),
        #ChainProp(name="HLT_tau35_mediumRNN_tracktwoMVABDT_L1cTAU30",   groups=SupportPhIGroup+SingleTauGroup), #TODO: cTAU seeding missing

    ]

    chains['Bphysics'] += [
        #ATR-21003; default dimuon and Bmumux chains from Run2; l2io validation; should not be moved to Physics
        ChainProp(name='HLT_2mu4_noL2Comb_bJpsimumu_L12MU3V', stream=["BphysDelayed"], groups=BphysicsGroup+DevGroup),
        ChainProp(name='HLT_mu6_noL2Comb_mu4_noL2Comb_bJpsimumu_L1MU5VF_2MU3V', l1SeedThresholds=['MU5VF','MU3V'], stream=["BphysDelayed"], groups=BphysicsGroup+DevGroup),
        ChainProp(name='HLT_2mu4_noL2Comb_bBmumux_BpmumuKp_L12MU3V', stream=["BphysDelayed"], groups=BphysicsGroup+DevGroup),
        ChainProp(name='HLT_2mu4_noL2Comb_bBmumux_BsmumuPhi_L12MU3V', stream=["BphysDelayed"], groups=BphysicsGroup+DevGroup),
        ChainProp(name='HLT_2mu4_noL2Comb_bBmumux_LbPqKm_L12MU3V', stream=["BphysDelayed"], groups=BphysicsGroup+DevGroup),

        #ATR-20603; B-trigger monitoring, ART tests; should not be moved to Physics
        ChainProp(name='HLT_2mu4_bJpsimumu_L12MU3V', stream=["BphysDelayed"], groups=BphysicsGroup+DevGroup),
        ChainProp(name='HLT_2mu4_bUpsimumu_L12MU3V', stream=["BphysDelayed"], groups=BphysicsGroup+DevGroup),
        #ATR-20839; validation and ART; should not be moved to Physics
        ChainProp(name='HLT_2mu4_bDimu_L12MU3V', stream=["BphysDelayed"], groups=BphysicsGroup+DevGroup),

        #ATR-21639; validation and monitoring of Bmumux chains; should not be moved to Physics
        ChainProp(name='HLT_2mu4_bBmumux_BpmumuKp_L12MU3V', stream=["BphysDelayed"], groups=BphysicsGroup+DevGroup),
        ChainProp(name='HLT_2mu4_bBmumux_BcmumuPi_L12MU3V', stream=["BphysDelayed"], groups=BphysicsGroup+DevGroup),
        ChainProp(name='HLT_2mu4_bBmumux_BsmumuPhi_L12MU3V', stream=["BphysDelayed"], groups=BphysicsGroup+DevGroup),
        ChainProp(name='HLT_2mu4_bBmumux_BdmumuKst_L12MU3V', stream=["BphysDelayed"], groups=BphysicsGroup+DevGroup),
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
        ChainProp(name='HLT_tau20_mediumRNN_tracktwoMVABDT_probe_j15_pf_ftf_03dRAB_L1RD0_FILLED', l1SeedThresholds=['PROBETAU8','FSNOSEED'], groups=TagAndProbeLegGroup+TauJetGroup),
        ChainProp(name='HLT_g140_loose_tau20_mediumRNN_tracktwoMVABDT_03dRAB_L1EM22VHI', l1SeedThresholds=['EM22VHI','PROBETAU8'], groups=TagAndProbeLegGroup+TauPhotonGroup),
 
        # photon + multijets (ATR-22594)
        ChainProp(name='HLT_g85_tight_3j50_L1EM22VHI',l1SeedThresholds=['EM22VHI','FSNOSEED'],stream=[PhysicsStream], groups=SupportLegGroup+EgammaJetGroup),
        ChainProp(name='HLT_g85_tight_3j50_L1eEM26M',l1SeedThresholds=['eEM26M','FSNOSEED'],stream=[PhysicsStream], groups=SupportPhIGroup+EgammaJetGroup),
        ChainProp(name='HLT_g85_tight_3j50_pf_ftf_L1EM22VHI',l1SeedThresholds=['EM22VHI','FSNOSEED'],stream=[PhysicsStream], groups=PrimaryLegGroup+EgammaJetGroup),
        ChainProp(name='HLT_g85_tight_3j50_pf_ftf_L1eEM26M',l1SeedThresholds=['eEM26M','FSNOSEED'],stream=[PhysicsStream], groups=PrimaryPhIGroup+EgammaJetGroup),

        # photon + MET (ATR-22594, ATR-21565)
        ChainProp(name='HLT_g90_loose_xe90_cell_L1EM22VHI',l1SeedThresholds=['EM22VHI','FSNOSEED'],stream=[PhysicsStream], groups=PrimaryLegGroup+EgammaMETGroup),
        ChainProp(name='HLT_g90_loose_xe90_cell_L1eEM26M',l1SeedThresholds=['eEM26M','FSNOSEED'],stream=[PhysicsStream], groups=PrimaryPhIGroup+EgammaMETGroup),
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

        # IDCalib Chains
        ChainProp(name='HLT_idcalib_trk9_IDCalibPEB_L1XE50', stream=['IDCalib'], groups=SingleMETGroup, l1SeedThresholds=['FSNOSEED']),
        ChainProp(name='HLT_idcalib_trk9_IDCalibPEB_L1J100', stream=['IDCalib'], groups=SingleJetGroup, l1SeedThresholds=['FSNOSEED']),
        ChainProp(name='HLT_idcalib_trk9_IDCalibPEB_L14J15', stream=['IDCalib'], groups=MultiJetGroup,  l1SeedThresholds=['FSNOSEED'])
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
