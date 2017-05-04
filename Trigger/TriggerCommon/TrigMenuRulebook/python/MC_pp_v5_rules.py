# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from RuleClasses import TriggerRule

tag = 'MC_pp_v5'

#################################################################################################
#
#
# switching luminosity points
#
#
#################################################################################################

########################################
# L1 Topo
lumi_l1topo_enable_bphys=10001

lumi_l1topo_enable_tau_tau=10001
lumi_l1topo_enable_tau_e=10001
lumi_l1topo_enable_tau_mu=10001

lumi_l1topo_enable_btag=10001

lumi_l1topo_enable_ht=10001

lumi_l1topo_enable_tap_e=10001

lumi_l1topo_enable_vbf=10001

lumi_l1topo_enable_j_xe=10001

lumi_l1topo_enable_dy=10001
lumi_l1topo_enable_llp=10001
lumi_l1topo_enable_nscan=10001
lumi_l1topo_enable_tau_lfv=10001

########################################
# L1 EM/TAU Iso
lumi_l1iso_enable_em=5001  # EM20VH -> 20VHI
lumi_l1iso_enable_tau=3001
lumi_l1iso_enable_tau_xe=10001
	
########################################
# L1 Jet requirement add
lumi_l1jet_enable_tau_tau=5001

########################################
# Bphys L1 evolution
lumi_disable_l1_2mu4=5001
lumi_disable_l1_mu6_2mu4=10001

lumi_disable_l1_3mu4=5001
	
########################################
# XE L1 evolution
lumi_disable_l1_xe50=5001
lumi_disable_l1_xe60=10001
	
########################################
# e/gamma L1 evolution
lumi_disable_l1_em20vhi=10001
lumi_disable_l1_em18vh=3001
	
lumi_disable_l1_2em13vh=10001
lumi_disable_l1_2em10vh=5001
lumi_disable_l1_2em15=3001
	
########################################
# muon L1 evolution
lumi_disable_l1_mu15=10001

########################################
# tau L1 evolution
lumi_disable_l1_tau30=3001

########################################
# bjet L1 evolution
lumi_disable_l1_3j25_0eta23=10001
lumi_disable_l1_4j15_0eta25=5001


#################################################################################################
#
#
# Muon
#
#
#################################################################################################


########################################
# 2e34 

muon_Rules_2e34={}

L1_list=['L1_MU20', 'L1_2MU10', 'L1_3MU6']
muon_Rules_2e34.update(dict(map(None,L1_list,len(L1_list)*[{
    1000 : TriggerRule(PS=1, comment='L1'),
    }])))

HLT_list=[ 'HLT_mu26_imedium', 'HLT_mu50', 'HLT_mu60_0eta105_msonly',
           'HLT_mu24_mu8noL1', 'HLT_mu24_2mu4noL1', 'HLT_3mu6_msonly' ]
muon_Rules_2e34.update(dict(map(None,HLT_list,len(HLT_list)*[{
    1000 : TriggerRule(PS=1, comment=''), 
    }])))

HLT_list=[ 'HLT_2mu14' ]
muon_Rules_2e34.update(dict(map(None,HLT_list,len(HLT_list)*[{
    1000 : TriggerRule(PS=1, comment=''), 
    5001 : TriggerRule(PS=1, comment='Express', ESRate=0.3), 
    }])))

HLT_list=[ 'HLT_3mu6' ]
muon_Rules_2e34.update(dict(map(None,HLT_list,len(HLT_list)*[{
    1000 : TriggerRule(PS=1, comment='Express', ESRate=0.1), 
    }])))

########################################
# 1.5e34 

muon_Rules_1p5e34={}

HLT_list=['HLT_mu22_mu8noL1', 'HLT_mu22_2mu4noL1']
muon_Rules_1p5e34.update(dict(map(None,HLT_list,len(HLT_list)*[{
     1000 : TriggerRule(PS=1, comment=''), 
    15001 : TriggerRule(PS=-1, comment='Disabled'),
    }])))

HLT_list=['HLT_mu24_imedium']
muon_Rules_1p5e34.update(dict(map(None,HLT_list,len(HLT_list)*[{
     1000 : TriggerRule(PS=1, comment='Express', ESRate=0.8), 
    15001 : TriggerRule(rate=0.8, comment='Express', ESValue=1),
    }])))

########################################
# 1.0e34 

muon_Rules_1e34={}

L1_list=['L1_MU15']
muon_Rules_1e34.update(dict(map(None,L1_list,len(L1_list)*[{
                    1000 : TriggerRule(PS=1, comment='L1'),
    lumi_disable_l1_mu15 : TriggerRule(rate=1000, maxRate=1000, comment='Prescaled'),
                    }])))

HLT_list=['HLT_mu24_iloose_L1MU15', 'HLT_mu18_mu8noL1', 'HLT_mu18_2mu4noL1', 'HLT_mu20_mu8noL1', 'HLT_mu20_2mu4noL1']
muon_Rules_1e34.update(dict(map(None,HLT_list,len(HLT_list)*[{
                     1000 : TriggerRule(PS=1, comment=''), 
     lumi_disable_l1_mu15 : TriggerRule(PS=-1, comment='Disabled'),
                     }])))

########################################
# 0.5e34 

muon_Rules_0p5e34={}

HLT_list=['HLT_mu20_iloose_L1MU15']
muon_Rules_0p5e34.update(dict(map(None,HLT_list,len(HLT_list)*[{
    1000 : TriggerRule(PS=1, comment=''), 
    5001 : TriggerRule(PS=-1, comment='Disabled'),
    }])))

HLT_list=['HLT_2mu10']
muon_Rules_0p5e34.update(dict(map(None,HLT_list,len(HLT_list)*[{
    1000 : TriggerRule(PS=1, comment='Express', ESRate=0.3), 
    5001 : TriggerRule(PS=-1, comment='Disabled'),
    }])))

########################################
# supporting

muon_Rules_supporting = {

    # ----------------------------------------
    # Level-1
    # low pT used for supporting (also for bJpsi_Trkloose)
    'L1_MU4'                                   : {  1000 : TriggerRule(rate=1000, maxRate=1000, comment='Prescaled') },
    'L1_MU6'                                   : {  1000 : TriggerRule(rate=1000, maxRate=1000, comment='Prescaled') },

    # ----------------------------------------
    # J/Psi T&P trigger
    'HLT_mu20_2mu0noL1_JpsimumuFS'             : {  1000 : TriggerRule(PS=1, comment='') },
    'HLT_mu20_2mu4_JpsimumuL2'                 : {  1000 : TriggerRule(PS=1, comment='') },

    # 1.0e34
    'HLT_mu18_2mu0noL1_JpsimumuFS'             : {  1000 : TriggerRule(PS=1, comment=''),
                                    lumi_disable_l1_mu15 : TriggerRule(PS=-1, comment='Disabled') },
    'HLT_mu18_2mu4_JpsimumuL2'                 : {  1000 : TriggerRule(PS=1, comment=''),
                                    lumi_disable_l1_mu15 : TriggerRule(PS=-1, comment='Disabled') },

    # ----------------------------------------
    # Z T&P idperf
    'HLT_mu13_mu13_idperf_Zmumu'               : {  1000 : TriggerRule(PS=1, comment='Express', ESRate=0.2) },

    # ----------------------------------------
    # Background estimation (non-iso single muon)
    # PS to 32 (PS is better for a same pileup distribution)

    'HLT_mu18'                                 : {  1000 : TriggerRule(PS=32, comment='Express', ESRate=0.4, rerun=1),
                                                    5001 : TriggerRule(rate=0.4, comment='Express', ESValue=1, rerun=1) },

    'HLT_mu20_L1MU15'                          : {  1000 : TriggerRule(PS=0, comment='', rerun=1),
                                                    5001 : TriggerRule(PS=32, comment='PS=32 at 0.5 < L < 1.0e34', rerun=1),
                                                   10001 : TriggerRule(PS=0, comment='', rerun=1) },

    'HLT_mu22'                                 : {  1000 : TriggerRule(PS=0, comment='', rerun=1),
                                                   10001 : TriggerRule(PS=32, comment='PS=32 at 1.0 < L < 1.5e34', rerun=1),
                                                   15001 : TriggerRule(PS=0, comment='', rerun=1) },

    'HLT_mu24'                                 : {  1000 : TriggerRule(PS=0, comment='', rerun=1),
                                                   15001 : TriggerRule(PS=32, comment='PS=32 at > 1.5e34', rerun=1) },

    ### 
    'HLT_mu26'                                 : {  1000 : TriggerRule(PS=0, comment='', rerun=1) },
    'HLT_mu24_L1MU15'                          : {  1000 : TriggerRule(PS=0, comment='', rerun=1) },
    'HLT_mu20'                                 : {  1000 : TriggerRule(PS=0, comment='', rerun=1) },

    # ----------------------------------------
    # Background estimation (muon + jet)
    # Run1: 3j PS=3, 4j PS=1
    'HLT_mu26_2j20noL1'                        : {  1000 : TriggerRule(rate=1.0, comment='') },
    'HLT_mu26_3j20noL1'                        : {  1000 : TriggerRule(rate=1.0, comment='') },
    'HLT_mu26_4j15noL1'                        : {  1000 : TriggerRule(rate=1.0, comment='') },
    'HLT_mu26_5j15noL1'                        : {  1000 : TriggerRule(rate=1.0, comment='') },

    # 1.5e34
    'HLT_mu24_2j20noL1'                        : {  1000 : TriggerRule(rate=1.0, comment=''),
                                                   15001 : TriggerRule(PS=-1, comment='Disabled at > 1.5e34') },
    'HLT_mu24_3j20noL1'                        : {  1000 : TriggerRule(rate=1.0, comment=''),
                                                   15001 : TriggerRule(PS=-1, comment='Disabled at > 1.5e34') },
    'HLT_mu24_4j15noL1'                        : {  1000 : TriggerRule(rate=1.0, comment=''),
                                                   15001 : TriggerRule(PS=-1, comment='Disabled at > 1.5e34') },
    'HLT_mu24_5j15noL1'                        : {  1000 : TriggerRule(rate=1.0, comment=''),
                                                   15001 : TriggerRule(PS=-1, comment='Disabled at > 1.5e34') },

    # 1.0e34
    'HLT_mu20_L1MU15_2j20noL1'                 : {  1000 : TriggerRule(rate=1.0, comment=''),
                                    lumi_disable_l1_mu15 : TriggerRule(PS=-1, comment='Disabled') },
    'HLT_mu20_L1MU15_3j20noL1'                 : {  1000 : TriggerRule(rate=1.0, comment=''),
                                    lumi_disable_l1_mu15 : TriggerRule(PS=-1, comment='Disabled') },
    'HLT_mu20_L1MU15_4j15noL1'                 : {  1000 : TriggerRule(rate=1.0, comment=''),
                                    lumi_disable_l1_mu15 : TriggerRule(PS=-1, comment='Disabled') },
    'HLT_mu20_L1MU15_5j15noL1'                 : {  1000 : TriggerRule(rate=1.0, comment=''),
                                    lumi_disable_l1_mu15 : TriggerRule(PS=-1, comment='Disabled') },

    # ----------------------------------------
    # Efficiency measurement (rerun)
    'HLT_mu4'                                  : {  1000 : TriggerRule(rate=1.0, comment='', rerun=1) },
    'HLT_mu6'                                  : {  1000 : TriggerRule(rate=0.5, comment='', rerun=1) },
    'HLT_mu10'                                 : {  1000 : TriggerRule(rate=0.5, comment='', rerun=1) },
    'HLT_mu14'                                 : {  1000 : TriggerRule(rate=0.5, comment='', rerun=1) },
    'HLT_mu14_iloose'                          : {  1000 : TriggerRule(PS=0, comment='', rerun=1) },
    'HLT_mu4_msonly'                           : {  1000 : TriggerRule(PS=0, comment='', rerun=1) },
    'HLT_mu6_msonly'                           : {  1000 : TriggerRule(PS=0, comment='', rerun=1) },
    'HLT_mu10_msonly'                          : {  1000 : TriggerRule(PS=0, comment='', rerun=1) },
    # passthrough rerun
    'HLT_mu0_perf'                             : {  1000 : TriggerRule(PS=0, comment='', rerun=1) },

    # ----------------------------------------
    # L1 streamers
    'HLT_noalg_L1MU10'                         : {  1000 : TriggerRule(inputRate=0.2, comment='') },
    'HLT_noalg_L1MU15'                         : {  1000 : TriggerRule(inputRate=0.2, comment='') },
    'HLT_noalg_L1MU20'                         : {  1000 : TriggerRule(inputRate=0.2, comment='') },

    # ----------------------------------------
    # idperf
    # 0.1 Hz for mu18, 0.5 Hz for mu22 @ Run1
    'HLT_mu4_idperf'                           : {  1000 : TriggerRule(rate=0.2, comment='', rerun=1) },
    'HLT_mu6_idperf'                           : {  1000 : TriggerRule(rate=0.2, comment='Express', ESRate=0.1, rerun=1) },
    'HLT_mu10_idperf'                          : {  1000 : TriggerRule(rate=0.2, comment='', rerun=1) },
    'HLT_mu20_idperf'                          : {  1000 : TriggerRule(rate=1.0, comment='Express', ESRate=0.4, rerun=1) },
    'HLT_mu24_idperf'                          : {  1000 : TriggerRule(rate=0.5, comment='', rerun=1) },
    #
    'HLT_mu6_L2Star_idperf'                    : {  1000 : TriggerRule(rate=0.1, comment='', rerun=1) },
    'HLT_mu24_L2Star_idperf'                   : {  1000 : TriggerRule(rate=0.1, comment='', rerun=1) },
                                                    
    # ----------------------------------------
    # run1 extr
    'HLT_mu4_r1extr'                           : {  1000 : TriggerRule(PS=0, comment='', rerun=1) },
    'HLT_mu10_r1extr'                          : {  1000 : TriggerRule(PS=0, comment='', rerun=1) },
    'HLT_mu20_r1extr'                          : {  1000 : TriggerRule(PS=0, comment='', rerun=1) },
}

########################################

muon_Rules={}

RulesList=[muon_Rules_2e34, muon_Rules_1p5e34, muon_Rules_1e34, muon_Rules_0p5e34, muon_Rules_supporting] 
for Rules in RulesList:
    for newrule in Rules.keys():
        if muon_Rules.has_key(newrule):
            print 'CRITICAL: duplicated rule cannot be added.',newrule
    muon_Rules.update(Rules)


#################################################################################################
#
#
# B-physics
#
#
#################################################################################################


########################################
# 2e34 

bphys_Rules_2e34={}

HLT_list=['HLT_2mu10_bBmumu','HLT_2mu10_bBmumuxv2','HLT_2mu10_bBmumux_BcmumuDsloose','HLT_2mu10_bBmumux_BpmumuKp','HLT_2mu10_bJpsimumu','HLT_2mu10_bUpsimumu',
          'HLT_3mu6_bDimu','HLT_3mu6_bTau','HLT_3mu6_bUpsi' ]
bphys_Rules_2e34.update(dict(map(None,HLT_list,len(HLT_list)*[{
    1000 : TriggerRule(PS=1, comment=''), 
    }])))

HLT_list=['HLT_3mu6_bJpsi']
bphys_Rules_2e34.update(dict(map(None,HLT_list,len(HLT_list)*[{
    1000 : TriggerRule(PS=1, comment='Express', ESRate=0.1), 
    }])))

########################################
# 1e34 

bphys_Rules_1e34={}

L1_list=['L1_2MU6']
bphys_Rules_1e34.update(dict(map(None,L1_list,len(L1_list)*[{
                        1000 : TriggerRule(PS=1, comment='L1'), 
    lumi_l1topo_enable_bphys : TriggerRule(rate=1000, maxRate=1000, comment='Prescaled'),
    }])))

HLT_list=['HLT_2mu6_bBmumu','HLT_2mu6_bBmumuxv2','HLT_2mu6_bBmumux_BpmumuKp','HLT_2mu6_bJpsimumu','HLT_2mu6_bUpsimumu']
bphys_Rules_1e34.update(dict(map(None,HLT_list,len(HLT_list)*[{
                        1000 : TriggerRule(PS=1, comment='Express',ESRate=0.1), 
    lumi_l1topo_enable_bphys : TriggerRule(rate=0.5, comment='Express',ESRate=0.1),
                        }])))

HLT_list=['HLT_2mu6_bBmumux_BcmumuDsloose']
bphys_Rules_1e34.update(dict(map(None,HLT_list,len(HLT_list)*[{
                        1000 : TriggerRule(PS=1, comment=''), 
    lumi_l1topo_enable_bphys : TriggerRule(rate=0.5, comment='Prescaled'),
                        }])))

# --------------------------------------

L1_list=['L1_MU6_2MU4']
bphys_Rules_1e34.update(dict(map(None,L1_list,len(L1_list)*[{
                        1000 : TriggerRule(PS=1, comment='L1'), 
    lumi_disable_l1_mu6_2mu4 : TriggerRule(rate=1000, maxRate=1000, comment='Prescaled'),
    lumi_l1topo_enable_bphys : TriggerRule(rate=0.5, comment='Prescaled'),
    }])))

HLT_list=['HLT_mu6_mu4_bBmumu','HLT_mu6_mu4_bBmumuxv2','HLT_mu6_mu4_bBmumux_BcmumuDsloose','HLT_mu6_mu4_bBmumux_BpmumuKp','HLT_mu6_mu4_bJpsimumu','HLT_mu6_mu4_bUpsimumu' ]
bphys_Rules_1e34.update(dict(map(None,HLT_list,len(HLT_list)*[{
                        1000 : TriggerRule(PS=1, comment=''), 
    lumi_disable_l1_mu6_2mu4 : TriggerRule(PS=1, comment='Prescaled at L1'),
    lumi_l1topo_enable_bphys : TriggerRule(rate=0.5, comment='Prescaled'),
                        }])))

########################################
# 0.5e34 

bphys_Rules_0p5e34={}

L1_list=['L1_2MU4']
bphys_Rules_0p5e34.update(dict(map(None,L1_list,len(L1_list)*[{
                    1000 : TriggerRule(PS=1, comment='L1'), 
    lumi_disable_l1_2mu4 : TriggerRule(rate=1000, maxRate=1000, comment='Prescaled'),
    }])))

HLT_list=['HLT_2mu4_bBmumu','HLT_2mu4_bBmumuxv2','HLT_2mu4_bBmumux_BcmumuDsloose','HLT_2mu4_bBmumux_BpmumuKp','HLT_2mu4_bJpsimumu','HLT_2mu4_bUpsimumu']
bphys_Rules_0p5e34.update(dict(map(None,HLT_list,len(HLT_list)*[{
                        1000 : TriggerRule(PS=1, comment=''), 
        lumi_disable_l1_2mu4 : TriggerRule(PS=1, comment='Prescaled at L1'),
    lumi_l1topo_enable_bphys : TriggerRule(rate=0.5, comment='Prescaled'),
    }])))

# --------------------------------------

L1_list=['L1_3MU4']
bphys_Rules_0p5e34.update(dict(map(None,L1_list,len(L1_list)*[{
                        1000 : TriggerRule(PS=1, comment='L1'), 
        lumi_disable_l1_3mu4 : TriggerRule(rate=1000, maxRate=1000, comment='Prescaled'),
    lumi_l1topo_enable_bphys : TriggerRule(rate=0.5, comment='Prescaled'),
    }])))

HLT_list=['HLT_3mu4_bJpsi','HLT_3mu4_bTau','HLT_3mu4_bUpsi']
bphys_Rules_0p5e34.update(dict(map(None,HLT_list,len(HLT_list)*[{
                        1000 : TriggerRule(PS=1, comment=''), 
        lumi_disable_l1_3mu4 : TriggerRule(PS=1, comment='Prescaled at L1'),
    lumi_l1topo_enable_bphys : TriggerRule(rate=0.5, comment='Prescaled'),
                    }])))

########################################
# other rules

bphys_Rules_supporting = {
    # ----------------------------------------
    # bDiMu
    # 1 Hz @ Run1
    'HLT_2mu10_bDimu'                                    : {  1000 : TriggerRule(rate=1.0, comment='') },
    'HLT_2mu6_bDimu'                                     : {  1000 : TriggerRule(rate=1.0, comment='Express', ESRate=0.2),
                                          lumi_l1topo_enable_bphys : TriggerRule(rate=0.2, comment='Express', ESValue=1) },
    'HLT_mu6_mu4_bDimu'                                  : {  1000 : TriggerRule(rate=1.0, comment=''),
                                          lumi_disable_l1_mu6_2mu4 : TriggerRule(PS=-1, comment='Disabled') },
    'HLT_2mu4_bDimu'                                     : {  1000 : TriggerRule(rate=1.0, comment=''),
                                              lumi_disable_l1_2mu4 : TriggerRule(PS=-1, comment='Disabled') },
    
    'HLT_3mu4_bDimu'                                     : {  1000 : TriggerRule(rate=1.0, comment=''),
                                              lumi_disable_l1_3mu4 : TriggerRule(PS=-1, comment='Disabled') },

    # ----------------------------------------
    # bDiMu, no Vtx, no OS
    # 1 Hz @ Run1
    'HLT_2mu10_bDimu_novtx_noos'                         : {  1000 : TriggerRule(rate=1.0, comment='') },
    'HLT_2mu6_bDimu_novtx_noos'                          : {  1000 : TriggerRule(rate=1.0, comment='Express', ESRate=0.2),
                                          lumi_l1topo_enable_bphys : TriggerRule(rate=0.2, comment='Express', ESValue=1) },
    'HLT_mu6_mu4_bDimu_novtx_noos'                       : {  1000 : TriggerRule(rate=1.0, comment=''),
                                          lumi_disable_l1_mu6_2mu4 : TriggerRule(PS=-1, comment='Disabled') },
    'HLT_2mu4_bDimu_novtx_noos'                          : {  1000 : TriggerRule(rate=1.0, comment=''),
                                              lumi_disable_l1_2mu4 : TriggerRule(PS=-1, comment='Disabled') },

    # ----------------------------------------
    # bDiMu, no Vtx, same sign
    'HLT_2mu10_bDimu_noinvm_novtx_ss'                    : {  1000 : TriggerRule(rate=1.0, comment='') },
    'HLT_2mu6_bDimu_noinvm_novtx_ss'                     : {  1000 : TriggerRule(rate=1.0, comment='Express', ESRate=0.2),
                                          lumi_l1topo_enable_bphys : TriggerRule(rate=0.2, comment='Express', ESValue=1) },
    'HLT_mu6_mu4_bDimu_noinvm_novtx_ss'                  : {  1000 : TriggerRule(rate=1.0, comment=''),
                                          lumi_disable_l1_mu6_2mu4 : TriggerRule(PS=-1, comment='Disabled') },
    'HLT_2mu4_bDimu_noinvm_novtx_ss'                     : {  1000 : TriggerRule(rate=1.0, comment=''),
                                              lumi_disable_l1_2mu4 : TriggerRule(PS=-1, comment='Disabled') },

    # ----------------------------------------
    # di-muons w/o cuts (prescaled)
    'HLT_2mu4'                                           : {  1000 : TriggerRule(rate=0.2, comment='') },
    'HLT_2mu6'                                           : {  1000 : TriggerRule(rate=0.2, comment='') },

    # ----------------------------------------
    # idperf at Jpsi mumu
    # 1.5 Hz @ Run1
    'HLT_mu4_mu4_idperf_bJpsimumu_noid'                  : {  1000 : TriggerRule(rate=1.5, comment='Express', ESRate=0.1) },

    # ----------------------------------------
    # Jpsi Trkloose
    # 4 Hz for mu6 @ Run1
    'HLT_mu4_bJpsi_Trkloose'                             : {  1000 : TriggerRule(rate=4.0, comment='') },
    'HLT_mu6_bJpsi_Trkloose'                             : {  1000 : TriggerRule(rate=2.0, comment='') },
    'HLT_mu10_bJpsi_Trkloose'                            : {  1000 : TriggerRule(rate=2.0, comment='') },
    'HLT_mu18_bJpsi_Trkloose'                            : {  1000 : TriggerRule(rate=2.0, comment='') },
}

########################################

bphys_Rules={}

RulesList=[bphys_Rules_2e34, bphys_Rules_1e34, bphys_Rules_0p5e34, bphys_Rules_supporting] 
for Rules in RulesList:
    for newrule in Rules.keys():
        if bphys_Rules.has_key(newrule):
            print 'CRITICAL: duplicated rule cannot be added.',newrule
    bphys_Rules.update(Rules)


#################################################################################################
#
#
# Missing ET
#
#
#################################################################################################


########################################
# 2e34 

met_Rules_2e34={}

L1_list=['L1_XE70']
met_Rules_2e34.update(dict(map(None,L1_list,len(L1_list)*[{
    1000 : TriggerRule(PS=1, comment='L1'), 
    }])))

HLT_list=['HLT_xe100',      'HLT_xe100_mht',      'HLT_xe100_pueta',      'HLT_xe100_pufit',      'HLT_xe100_tc_em',      'HLT_xe100_tc_lcw',
          'HLT_xe100_wEFMu','HLT_xe100_mht_wEFMu','HLT_xe100_pueta_wEFMu','HLT_xe100_pufit_wEFMu','HLT_xe100_tc_em_wEFMu','HLT_xe100_tc_lcw_wEFMu']
met_Rules_2e34.update(dict(map(None,HLT_list,len(HLT_list)*[{
    1000 : TriggerRule(PS=1, comment=''), 
    }])))

########################################
# 1.5e34 

met_Rules_1p5e34={}

HLT_list=['HLT_xe80_L1XE70',       'HLT_xe80_mht_L1XE70',      'HLT_xe80_pueta_L1XE70',
          'HLT_xe80_pufit_L1XE70', 'HLT_xe80_tc_em_L1XE70',    'HLT_xe80_tc_lcw_L1XE70',
          'HLT_xe80_wEFMu_L1XE70',      'HLT_xe80_mht_wEFMu_L1XE70',  'HLT_xe80_pueta_wEFMu_L1XE70',
          'HLT_xe80_pufit_wEFMu_L1XE70','HLT_xe80_tc_em_wEFMu_L1XE70','HLT_xe80_tc_lcw_wEFMu_L1XE70']
met_Rules_1p5e34.update(dict(map(None,HLT_list,len(HLT_list)*[{
    1000 : TriggerRule(PS=1, comment=''), 
   15001 : TriggerRule(rate=0.5, comment='Prescaled'),
    }])))

########################################
# 1e34 

met_Rules_1e34={}

L1_list=['L1_XE60']
met_Rules_1e34.update(dict(map(None,L1_list,len(L1_list)*[{
                    1000 : TriggerRule(PS=1, comment='L1'), 
    lumi_disable_l1_xe60 : TriggerRule(rate=500, maxRate=500, comment='Prescaled'),
    }])))

HLT_list=['HLT_xe80',      'HLT_xe80_mht',      'HLT_xe80_pueta',      'HLT_xe80_pufit',      'HLT_xe80_tc_em',      'HLT_xe80_tc_lcw',
          'HLT_xe80_wEFMu','HLT_xe80_mht_wEFMu','HLT_xe80_pueta_wEFMu','HLT_xe80_pufit_wEFMu','HLT_xe80_tc_em_wEFMu','HLT_xe80_tc_lcw_wEFMu',]
met_Rules_1e34.update(dict(map(None,HLT_list,len(HLT_list)*[{
                    1000 : TriggerRule(PS=1, comment=''), 
    lumi_disable_l1_xe60 : TriggerRule(rate=0.5, comment='Prescaled'),
    }])))

########################################
# 0.5e34 

met_Rules_0p5e34={}

L1_list=['L1_XE50']
met_Rules_0p5e34.update(dict(map(None,L1_list,len(L1_list)*[{
                    1000 : TriggerRule(PS=1, comment='L1'), 
    lumi_disable_l1_xe50 : TriggerRule(rate=500, maxRate=500, comment='Prescaled'),
    }])))

HLT_list=['HLT_xe70',      'HLT_xe70_mht',      'HLT_xe70_pueta',      'HLT_xe70_pufit',      'HLT_xe70_tc_em',      'HLT_xe70_tc_lcw',
          'HLT_xe70_wEFMu','HLT_xe70_mht_wEFMu','HLT_xe70_pueta_wEFMu','HLT_xe70_pufit_wEFMu','HLT_xe70_tc_em_wEFMu','HLT_xe70_tc_lcw_wEFMu',]
met_Rules_0p5e34.update(dict(map(None,HLT_list,len(HLT_list)*[{
                    1000 : TriggerRule(PS=1, comment=''), 
    lumi_disable_l1_xe50 : TriggerRule(rate=0.5, comment='Prescaled'),
    }])))

HLT_list=['HLT_xe80_L1XE50',       'HLT_xe80_mht_L1XE50',     'HLT_xe80_pueta_L1XE50',
          'HLT_xe80_pufit_L1XE50', 'HLT_xe80_tc_em_L1XE50',   'HLT_xe80_tc_lcw_L1XE50',
          'HLT_xe80_wEFMu_L1XE50',      'HLT_xe80_mht_wEFMu_L1XE50',  'HLT_xe80_pueta_wEFMu_L1XE50',
          'HLT_xe80_pufit_wEFMu_L1XE50','HLT_xe80_tc_em_wEFMu_L1XE50','HLT_xe80_tc_lcw_wEFMu_L1XE50',]
met_Rules_0p5e34.update(dict(map(None,HLT_list,len(HLT_list)*[{
                    1000 : TriggerRule(PS=1, comment=''), 
    lumi_disable_l1_xe50 : TriggerRule(rate=0.5, comment='Prescaled'),
    }])))

########################################
# prescaled low MET

# 0.25 Hz for xe30,50,60,70 @ Run1

met_Rules_supporting={}

L1_list=['L1_XE35', 'L1_XE40']
met_Rules_supporting.update(dict(map(None,L1_list,len(L1_list)*[{
    1000 : TriggerRule(rate=200, maxRate=200, comment='L1'), 
    }])))

HLT_list=['HLT_xe35',      'HLT_xe35_mht',      'HLT_xe35_pueta',      'HLT_xe35_pufit',      'HLT_xe35_tc_em',      'HLT_xe35_tc_lcw',
          'HLT_xe35_wEFMu','HLT_xe35_mht_wEFMu','HLT_xe35_pueta_wEFMu','HLT_xe35_pufit_wEFMu','HLT_xe35_tc_em_wEFMu','HLT_xe35_tc_lcw_wEFMu',]
met_Rules_supporting.update(dict(map(None,HLT_list,len(HLT_list)*[{
### cost file does not contain xe35 --> setting PS by hand for now
###    1000 : TriggerRule(rate=2.0, comment='', rerun=1), 
    1000 : TriggerRule(PS=20, comment='', rerun=1), 
    }])))

HLT_list=['HLT_xe50_mht', 'HLT_xe50_pueta', 'HLT_xe50_pufit', 'HLT_xe50_tc_em', 'HLT_xe50_tc_lcw']
met_Rules_supporting.update(dict(map(None,HLT_list,len(HLT_list)*[{
    1000 : TriggerRule(rate=0.5, comment=''), 
    }])))

HLT_list=['HLT_xe50']  # rerun needed for tau (ATR-11530)
met_Rules_supporting.update(dict(map(None,HLT_list,len(HLT_list)*[{
    1000 : TriggerRule(rate=0.5, comment='', rerun=1), 
    }])))

HLT_list=['HLT_xe60',      'HLT_xe60_mht',      'HLT_xe60_pueta',      'HLT_xe60_pufit',      'HLT_xe60_tc_em',      'HLT_xe60_tc_lcw',
          'HLT_xe60_wEFMu','HLT_xe60_mht_wEFMu','HLT_xe60_pueta_wEFMu','HLT_xe60_pufit_wEFMu','HLT_xe60_tc_em_wEFMu','HLT_xe60_tc_lcw_wEFMu',]
met_Rules_supporting.update(dict(map(None,HLT_list,len(HLT_list)*[{
    1000 : TriggerRule(rate=0.5, comment=''), 
    }])))

########################################

met_Rules={}

RulesList=[met_Rules_2e34, met_Rules_1p5e34, met_Rules_1e34, met_Rules_0p5e34, met_Rules_supporting] 
for Rules in RulesList:
    for newrule in Rules.keys():
        if met_Rules.has_key(newrule):
            print 'CRITICAL: duplicated rule cannot be added.',newrule
    met_Rules.update(Rules)


#################################################################################################
#
#
# Jet
#
#
#################################################################################################


########################################
# 2e34 

jet_Rules_2e34={}

L1_list=['L1_J100','L1_J100.32ETA49','L1_3J50','L1_4J20','L1_5J15.0ETA25','L1_6J15']
jet_Rules_2e34.update(dict(map(None,L1_list,len(L1_list)*[{
    1000 : TriggerRule(PS=1, comment='L1'), 
    }])))

HLT_list=[
    # 1jet
    'HLT_j400','HLT_j400_jes','HLT_j400_lcw','HLT_j400_lcw_jes','HLT_j400_nojcalib','HLT_j400_sub',
    'HLT_j420','HLT_j420_jes','HLT_j420_lcw','HLT_j420_lcw_jes','HLT_j420_nojcalib','HLT_j420_lcw_nojcalib',
    'HLT_j440',
    'HLT_j460',
    # reclustered jet
    'HLT_j460_a10r_L1J100',
    # Fat jet
    'HLT_j460_a10_nojcalib_L1J100','HLT_j460_a10_lcw_sub_L1J100','HLT_j460_a10_lcw_nojcalib_L1J100',
    # Forward jet
    'HLT_j360_320eta490','HLT_j360_320eta490_jes','HLT_j360_320eta490_lcw','HLT_j360_320eta490_lcw_jes','HLT_j360_320eta490_lcw_nojcalib','HLT_j360_320eta490_nojcalib',
    # 3jet
    'HLT_3j175',
    # 4jet
    'HLT_4j100',
    # 5jet 
    'HLT_5j85','HLT_5j85_jes','HLT_5j85_lcw','HLT_5j85_lcw_jes','HLT_5j85_lcw_nojcalib','HLT_5j85_nojcalib',
    # 6jet
    'HLT_6j50_0eta240_L14J20',
    'HLT_6j55_0eta240_L14J20',
    'HLT_6j50_0eta240_L15J150ETA25',
    'HLT_6j55_0eta240_L15J150ETA25',
    # 7jet
    'HLT_7j45',
    'HLT_7j45_L14J20',
    'HLT_7j45_0eta240_L14J20',
    'HLT_7j45_0eta240_L15J150ETA25',
    # HT
    'HLT_ht1000_L1J100',
    ]
jet_Rules_2e34.update(dict(map(None,HLT_list,len(HLT_list)*[{
    1000 : TriggerRule(PS=1, comment=''), 
    }])))

# ----------------------------------------

HLT_list=['HLT_j460_a10_sub_L1J100']
jet_Rules_2e34.update(dict(map(None,HLT_list,len(HLT_list)*[{
    1000 : TriggerRule(PS=1, comment='Express', ESRate=0.2), 
    }])))

########################################
# 1e34 

jet_Rules_1e34={}

HLT_list=[
    # 1jet
    'HLT_j380','HLT_j380_jes','HLT_j380_lcw','HLT_j380_lcw_jes','HLT_j380_lcw_nojcalib','HLT_j380_nojcalib',
    # fat jet
    'HLT_j360_a10_sub_L1J100','HLT_j360_a10_lcw_sub_L1J100',
    ]
jet_Rules_1e34.update(dict(map(None,HLT_list,len(HLT_list)*[{
     1000 : TriggerRule(PS=1, comment=''), 
    10001 : TriggerRule(PS=-1, comment='Disabled'),
     }])))

HLT_list=['HLT_5j70']
jet_Rules_1e34.update(dict(map(None,HLT_list,len(HLT_list)*[{
     1000 : TriggerRule(PS=1, comment=''), 
    10001 : TriggerRule(rate=1.0, comment='Prescaled'),
    }])))

########################################
# 0p5e34 

jet_Rules_0p5e34={}

L1_list=['L1_J75','L1_J75.32ETA49']
jet_Rules_0p5e34.update(dict(map(None,L1_list,len(L1_list)*[{
    1000 : TriggerRule(PS=1, comment='L1'), 
    5001 : TriggerRule(rate=100, maxRate=100, comment='Prescaled'),
    }])))

L1_list=['L1_3J40']
jet_Rules_0p5e34.update(dict(map(None,L1_list,len(L1_list)*[{
    1000 : TriggerRule(PS=1, comment='L1'), 
    5001 : TriggerRule(PS=-1, comment='Disabled'),
    }])))

L1_list=['L1_4J15',]
jet_Rules_0p5e34.update(dict(map(None,L1_list,len(L1_list)*[{
    1000 : TriggerRule(PS=1, comment='L1'), 
    5001 : TriggerRule(rate=2000, maxRate=2000, comment='Used for beamspot'),
    }])))

HLT_list=[
    # reclustered jet
    'HLT_j360_a10r_L1J100',
    # forward jet
    'HLT_j260_320eta490','HLT_j260_320eta490_jes','HLT_j260_320eta490_lcw','HLT_j260_320eta490_lcw_jes','HLT_j260_320eta490_lcw_nojcalib','HLT_j260_320eta490_nojcalib',
    # 6jet
    'HLT_6j45',
    'HLT_6j45_0eta240',
    'HLT_6j45_0eta240_L14J20',
    'HLT_6j45_0eta240_L15J150ETA25',
    # ht
    'HLT_ht850_L1J100',
    ]
jet_Rules_0p5e34.update(dict(map(None,HLT_list,len(HLT_list)*[{
    1000 : TriggerRule(PS=1, comment=''), 
    5001 : TriggerRule(PS=-1, comment='Disabled'),
    }])))

HLT_list=['HLT_j360']
jet_Rules_0p5e34.update(dict(map(None,HLT_list,len(HLT_list)*[{
    1000 : TriggerRule(PS=1, comment='Express', ESRate=0.2), 
    5001 : TriggerRule(rate=0.2, comment='Express', ESValue=1),
    }])))

# ----------------------------------------

HLT_list=[
    # 4jet
    'HLT_4j85','HLT_4j85_jes','HLT_4j85_lcw','HLT_4j85_lcw_jes','HLT_4j85_lcw_nojcalib','HLT_4j85_nojcalib',
    # 5jet
    'HLT_5j60',
    ]
jet_Rules_0p5e34.update(dict(map(None,HLT_list,len(HLT_list)*[{
     1000 : TriggerRule(PS=1, comment=''), 
     5001 : TriggerRule(rate=1.0, comment='Prescaled'),
    }])))

########################################
# 0p15e34 

jet_Rules_0p15e34={}

HLT_list=['HLT_ht700_L1J100']
jet_Rules_0p15e34.update(dict(map(None,HLT_list,len(HLT_list)*[{
    1000 : TriggerRule(PS=1, comment=''), 
    1501 : TriggerRule(PS=-1, comment='Disabled'),
    }])))

########################################
# prescaled 

jet_Rules_supporting = {
    'L1_RD0_FILLED'  : { 1000 : TriggerRule(PS=-1, comment='L1') }, # cannot control this now
    'L1_RD3_FILLED'  : { 1000 : TriggerRule(PS=-1, comment='L1') },
    #'L1_RD0_FILLED'  : { 1000 : TriggerRule(rate=1000, scaling='bunches', comment='L1') },
    #'L1_RD3_FILLED'  : { 1000 : TriggerRule(rate=1000, scaling='bunches', comment='L1') },
    }

# ========================================
# prescaled single jets

L1_list=['L1_J15', 'L1_J20', 'L1_J25', 'L1_J30', 'L1_J40', 'L1_J50', 'L1_J60', 'L1_J85']
jet_Rules_supporting.update(dict(map(None,L1_list,len(L1_list)*[{
    1000 : TriggerRule(rate=100, maxRate=100, comment='Prescaled'),
    }])))

HLT_list=[
    # RD0
    'HLT_j15',
    'HLT_j35','HLT_j35_jes','HLT_j35_lcw','HLT_j35_lcw_jes','HLT_j35_lcw_nojcalib','HLT_j35_nojcalib',
    'HLT_j45_L1RD0',
    'HLT_j55_L1RD0',
    'HLT_j85_L1RD0',
    # J15
    'HLT_j55',
    # J20
    'HLT_j85','HLT_j85_jes','HLT_j85_lcw','HLT_j85_lcw_jes','HLT_j85_lcw_nojcalib','HLT_j85_nojcalib',
    # J25
    'HLT_j100',
    # J30
    'HLT_j110',
    # J40
    'HLT_j150',
    # J50
    'HLT_j175','HLT_j175_jes','HLT_j175_lcw','HLT_j175_lcw_jes','HLT_j175_lcw_nojcalib','HLT_j175_nojcalib',
    # J60
    'HLT_j200',
    # J75
    'HLT_j260',
    # J85
    'HLT_j300','HLT_j300_lcw_nojcalib',
    'HLT_j320',
    ]
jet_Rules_supporting.update(dict(map(None,HLT_list,len(HLT_list)*[{
    1000 : TriggerRule(rate=1.0, comment=''), 
    }])))

HLT_list=[
    'HLT_j60_L1RD0', # RD0
    'HLT_j60',  # J15
    ]
jet_Rules_supporting.update(dict(map(None,HLT_list,len(HLT_list)*[{
    1000 : TriggerRule(rate=1.0, comment='Express', ESRate=0.2), 
    }])))

HLT_list=['HLT_j25' #RD0
          ]
jet_Rules_supporting.update(dict(map(None,HLT_list,len(HLT_list)*[{
    1000 : TriggerRule(rate=1.0, comment='Express', ESRate=0.8), 
    }])))

# ----------------------------------------

L1_list=['J15.29ETA32', 'J15.32ETA49', 'L1_J20.29ETA32', 'L1_J20.32ETA49', 'L1_J30.32ETA49', 'L1_J50.32ETA49',]
jet_Rules_supporting.update(dict(map(None,L1_list,len(L1_list)*[{
    1000 : TriggerRule(rate=100, maxRate=100, comment='Prescaled'),
    }])))

HLT_list=[
    # J20.29ETA32
    'HLT_j85_280eta320','HLT_j85_280eta320_jes','HLT_j85_280eta320_lcw','HLT_j85_280eta320_lcw_jes','HLT_j85_280eta320_lcw_nojcalib','HLT_j85_280eta320_nojcalib',
    # J20.32ETA49
    'HLT_j85_320eta490',
    # J30.32ETA49
    'HLT_j110_320eta490',
    # J50.32ETA49
    'HLT_j175_320eta490','HLT_j175_320eta490_jes','HLT_j175_320eta490_lcw','HLT_j175_320eta490_lcw_jes','HLT_j175_320eta490_lcw_nojcalib','HLT_j175_320eta490_nojcalib',
    ]
jet_Rules_supporting.update(dict(map(None,HLT_list,len(HLT_list)*[{
    1000 : TriggerRule(rate=1.0, comment=''), 
    }])))

HLT_list=[
    # J15.29ETA32
    'HLT_j60_280eta320',
    # J15.32ETA49
    'HLT_j60_320eta490',
    ]
jet_Rules_supporting.update(dict(map(None,HLT_list,len(HLT_list)*[{
    1000 : TriggerRule(rate=1.0, comment='Express', ESRate=0.2), 
    }])))

# ========================================
# prescaled multi jets

L1_list=['L1_3J15']
jet_Rules_supporting.update(dict(map(None,L1_list,len(L1_list)*[{
    1000 : TriggerRule(rate=1000, maxRate=1000, comment='Prescaled'),
    }])))

HLT_list=[
    # 4j
    # 4j85 (3J40 seeded) PS is in the 'primary' section (0.5e34 trigger)
    'HLT_4j25', # RD0
    'HLT_4j45', # 3J15
    # 5j
    'HLT_5j25', # RD0
    'HLT_5j55', # 4J15
    # 6j
    'HLT_6j25', # RD0
    # 7j
    'HLT_7j25', # RD0
    ]
jet_Rules_supporting.update(dict(map(None,HLT_list,len(HLT_list)*[{
    1000 : TriggerRule(rate=1.0, comment=''), 
    }])))

# ----------------------------------------
# SUSY 0-lepton + multijets analysis request on 5j45
# - envisaged prescale: 7-10 (flat, i.e. no change)
# - envisaged period for the prescale change: early 50 ns data-taking for lumis <= 5e33

HLT_list=['HLT_5j45']
jet_Rules_supporting.update(dict(map(None,HLT_list,len(HLT_list)*[{
    1000 : TriggerRule(PS=10, comment=''), 
    5001 : TriggerRule(PS=260, comment=''), 
    }])))

########################################

jet_Rules={}

RulesList=[jet_Rules_2e34, jet_Rules_1e34, jet_Rules_0p5e34, jet_Rules_0p15e34, jet_Rules_supporting] 
for Rules in RulesList:
    for newrule in Rules.keys():
        if jet_Rules.has_key(newrule):
            print 'CRITICAL: duplicated rule cannot be added.',newrule
    jet_Rules.update(Rules)


#################################################################################################
#
#
# b-Jet
#
#
#################################################################################################


########################################
# 2e34 

bjet_Rules_2e34={}

HLT_list=[
    # 2b + 1j
    'HLT_2j75_bmedium_j75', 'HLT_2j75_bmedium_split_j75',
    'HLT_2j70_btight_j70',  'HLT_2j70_btight_split_j70',
    # 2b + 2j
    'HLT_2j55_bmedium_2j55','HLT_2j55_bmedium_split_2j55',
    'HLT_2j45_btight_2j45', 'HLT_2j45_btight_split_2j45',
    # 1b + 3j
    'HLT_j75_bmedium_3j75', 'HLT_j75_bmedium_split_3j75',
    'HLT_j70_btight_3j70',  'HLT_j70_btight_split_3j70',
    # 2b + 1j
    'HLT_j100_2j55_bmedium','HLT_j100_2j55_bmedium_split',
    # 2b
    'HLT_j175_bmedium_j60_bmedium','HLT_j175_bmedium_split_j60_bmedium_split',
    # 1b
    'HLT_j300_bloose','HLT_j300_bloose_split',
    ]

bjet_Rules_2e34.update(dict(map(None,HLT_list,len(HLT_list)*[{
     1000 : TriggerRule(PS=1, comment=''), 
    }])))

########################################
# 1e34 

bjet_Rules_1e34={}

L1_list=['L1_3J25.0ETA23']
bjet_Rules_1e34.update(dict(map(None,L1_list,len(L1_list)*[{
                           1000 : TriggerRule(PS=1, comment='L1'), 
    lumi_disable_l1_3j25_0eta23 : TriggerRule(PS=-1, comment='Disabled'), 
    }])))

HLT_list=[
    #
    # 2b + 2j
    # 3J25.0ETA23 seeded
    'HLT_2j45_bmedium_2j45_L13J25.0ETA23','HLT_2j45_bmedium_split_2j45_L13J25.0ETA23',
    'HLT_2j35_btight_2j35_L13J25.0ETA23', 'HLT_2j35_btight_split_2j35_L13J25.0ETA23',
    # backup
    'HLT_2j55_bmedium_2j55_L13J25.ETA23', 'HLT_2j55_bmedium_split_2j55_L13J25.ETA23',
    'HLT_2j45_btight_2j45_L13J25.ETA23',  'HLT_2j45_btight_split_2j45_L13J25.ETA23',
    #
    # 1b + 3j
    # 3J25.0ETA23 seeded
    'HLT_j70_bmedium_3j70_L13J25.0ETA23','HLT_j70_bmedium_split_3j70_L13J25.0ETA23',
    'HLT_j65_btight_3j65_L13J25.0ETA23', 'HLT_j65_btight_split_3j65_L13J25.0ETA23',
    # backup
    'HLT_j75_bmedium_3j75_L13J25.ETA23', 'HLT_j75_bmedium_split_3j75_L13J25.ETA23',
    'HLT_j70_btight_3j70_L13J25.ETA23',  'HLT_j70_btight_split_3j70_L13J25.ETA23',
    ]
bjet_Rules_1e34.update(dict(map(None,HLT_list,len(HLT_list)*[{
                           1000 : TriggerRule(PS=1, comment=''), 
    lumi_disable_l1_3j25_0eta23 : TriggerRule(PS=-1, comment='Disabled'), 
    }])))

# ----------------------------------------

L1_list=['L1_MU4_J50', 'L1_MU6_J40']
bjet_Rules_1e34.update(dict(map(None,L1_list,len(L1_list)*[{
                       1000 : TriggerRule(PS=1, comment='L1'), 
    lumi_l1topo_enable_btag : TriggerRule(PS=-1, comment='Disabled'),
    }])))

HLT_list=['HLT_mu4_j70_dr05_L1MU4_J50', 'HLT_mu6_j60_dr05_L1MU6_J40']
bjet_Rules_1e34.update(dict(map(None,HLT_list,len(HLT_list)*[{
                           1000 : TriggerRule(PS=1, comment=''), 
    lumi_disable_l1_3j25_0eta23 : TriggerRule(PS=-1, comment='Disabled'), 
    }])))

########################################
# 0.5e34 

bjet_Rules_0p5e34={}

HLT_list=[
    # 2b + 1j
    'HLT_2j70_bmedium_j70','HLT_2j70_bmedium_split_j70',
    'HLT_2j65_btight_j65', 'HLT_2j65_btight_split_j65',
    # 2b
    'HLT_j150_bmedium_j50_bmedium','HLT_j150_bmedium_split_j50_bmedium_split',
    # 1b
    'HLT_j225_bloose','HLT_j225_bloose_split',
    ]
bjet_Rules_0p5e34.update(dict(map(None,HLT_list,len(HLT_list)*[{
    1000 : TriggerRule(PS=1, comment=''), 
    5001 : TriggerRule(PS=-1, comment='Disabled'),
    }])))

# --------------------------------------

L1_list=['L1_4J15.0ETA25']
bjet_Rules_0p5e34.update(dict(map(None,L1_list,len(L1_list)*[{
                           1000 : TriggerRule(PS=1, comment=''), 
    lumi_disable_l1_4j15_0eta25 : TriggerRule(PS=-1, comment='Disabled'),
    }])))

HLT_list=[
    # 2b + 2j
    # 4J15.0ETA25 seeded
    'HLT_2j45_bmedium_2j45_L14J15.0ETA25','HLT_2j45_bmedium_split_2j45_L14J15.0ETA25',
    'HLT_2j35_btight_2j35_L14J15.0ETA25', 'HLT_2j35_btight_split_2j35_L14J15.0ETA25',
    # 1b + 3j
    # 4J15.0ETA25 seeded
    'HLT_j70_bmedium_3j70_L14J15.0ETA25', 'HLT_j70_bmedium_split_3j70_L14J15.0ETA25',
    'HLT_j65_btight_3j65_L14J15.0ETA25',  'HLT_j65_btight_split_3j65_L14J15.0ETA25',
    ]
bjet_Rules_0p5e34.update(dict(map(None,HLT_list,len(HLT_list)*[{
                           1000 : TriggerRule(PS=1, comment=''), 
    lumi_disable_l1_4j15_0eta25 : TriggerRule(PS=-1, comment='Disabled'),
    }])))

########################################
# 0.3e34 

bjet_Rules_0p3e34={}

HLT_list=['HLT_j175_bmedium','HLT_j175_bmedium_split']
bjet_Rules_0p3e34.update(dict(map(None,HLT_list,len(HLT_list)*[{
    1000 : TriggerRule(PS=1, comment=''), 
    3001 : TriggerRule(PS=-1, comment='Disabled'),
    }])))

########################################
# 0.15e34 

bjet_Rules_0p15e34={}

L1_list=['L1_MU4_J30', 'L1_MU6_J20']
bjet_Rules_0p5e34.update(dict(map(None,L1_list,len(L1_list)*[{
    1000 : TriggerRule(PS=1, comment=''), 
    1501 : TriggerRule(PS=-1, comment='Disabled'),
    }])))

HLT_list=['HLT_mu4_j70_dr05_L1MU4_J30','HLT_mu6_j60_dr05_L1MU6_J20']
bjet_Rules_0p15e34.update(dict(map(None,HLT_list,len(HLT_list)*[{
    1000 : TriggerRule(PS=1, comment=''), 
    1501 : TriggerRule(PS=-1, comment='Disabled'),
    }])))

########################################
# supporting

bjet_Rules_supporting={}

# ========================================
# lepton + b-perf

# PS=1, as it is showeed by lepton
HLT_list=[
    'HLT_2e17_loose_2j35_bperf',
    'HLT_e26_tight_iloose_2j35_bperf',
    'HLT_e26_lhtight_iloose_2j35_bperf',
    'HLT_e26_lhtight_nod0_iloose_2j35_bperf',
    #
    'HLT_mu26_imedium_2j35_bperf',
    'HLT_2mu14_2j35_bperf',
    ]
bjet_Rules_supporting.update(dict(map(None,HLT_list,len(HLT_list)*[{
    1000 : TriggerRule(PS=1, comment=''), 
    }])))

# ========================================
# bperf

L1_list=['L1_3J15.0ETA25', 'L1_3J20']
bjet_Rules_supporting.update(dict(map(None,L1_list,len(L1_list)*[{
    1000 : TriggerRule(rate=1000, maxRate=1000, comment='Prescaled'),
    }])))

HLT_list=[
    # RD0
    'HLT_j25_bperf',
    # J15
    'HLT_j35_bperf',
    'HLT_j45_bperf',
    # J20
    'HLT_j55_bperf_split', 'HLT_j55_boffperf',
    'HLT_j85_bperf',
    # J40
    'HLT_j150_bperf',
    # J50
    'HLT_j175_bperf', 'HLT_j175_bperf_split',
    # J75
    'HLT_j260_bperf',
    # J85
    'HLT_j320_bperf',
    # j100
    'HLT_j225_bperf', 'HLT_j225_bperf_split',
    'HLT_j300_bperf', 'HLT_j300_bperf_split',
    'HLT_j400_bperf',
    # 3J15
    'HLT_j45_bperf_3j45', 'HLT_j45_bperf_split_3j45', 'HLT_j45_boffperf_3j45',
    # 3J15.0ETA25
    'HLT_j45_bperf_3j45_L13J15.0ETA25', 'HLT_j45_bperf_split_3j45_L13J150ETA25', 'HLT_j45_boffperf_3j45_L13J15.0ETA25',
    # 3J20
    'HLT_j45_bperf_3j45_L13J20', 'HLT_j45_bperf_split_3j45_L13J20', 'HLT_j45_boffperf_3j45_L13J20',
    # 3J25.0ETA23
    'HLT_j65_bperf_3j65_L13J25.0ETA23',  'HLT_j65_bperf_split_3j65_L13J25.0ETA23',
    'HLT_j70_bperf_3j70_L13J25.0ETA23',  'HLT_j70_bperf_split_3j70_L13J25.0ETA23',
    'HLT_j70_bperf_3j70_L13J25.ETA23',   'HLT_j70_bperf_split_3j70_L13J25.ETA23',
    'HLT_j75_bperf_3j75_L13J25.ETA23',   'HLT_j75_bperf_split_3j75_L13J25.ETA23',
    'HLT_2j35_bperf_2j35_L13J25.0ETA23', 'HLT_2j35_bperf_split_2j35_L13J25.0ETA23',
    'HLT_2j45_bperf_2j45_L13J25.0ETA23', 'HLT_2j45_bperf_split_2j45_L13J25.0ETA23',
    'HLT_2j45_bperf_2j45_L13J25.ETA23',  'HLT_2j45_bperf_split_2j45_L13J25.ETA23', ### duplication ###
    'HLT_2j55_bperf_2j55_L13J25.ETA23',  'HLT_2j55_bperf_split_2j55_L13J25.ETA23',
    'HLT_2j65_bperf_j65',                'HLT_2j65_bperf_split_j65',
    'HLT_2j70_bperf_j70',                'HLT_2j70_bperf_split_j70',
    'HLT_2j75_bperf_j75',                'HLT_2j75_bperf_split_j75',
    # 4J15.0ETA25
    'HLT_j65_bperf_3j65_L14J15.0ETA25',  'HLT_j65_bperf_split_3j65_L14J15.0ETA25',
    'HLT_j70_bperf_3j70_L14J15.0ETA25',  'HLT_j70_bperf_split_3j70_L14J15.0ETA25',
    'HLT_2j35_bperf_2j35_L14J15.0ETA25', 'HLT_2j35_bperf_split_2j35_L14J15.0ETA25',
    'HLT_2j45_bperf_2j45_L14J15.0ETA25', 'HLT_2j45_bperf_split_2j45_L14J15.0ETA25',
    # 4J20
    'HLT_j45_bperf_3j45_L14J20',
    'HLT_j55_bperf_3j55',
    'HLT_j70_bperf_3j70',  'HLT_j70_bperf_split_3j70',
    'HLT_j75_bperf_3j75',  'HLT_j75_bperf_split_3j75',
    'HLT_2j45_bperf_2j45', 'HLT_2j45_bperf_split_2j45',
    'HLT_2j55_bperf_2j55', 'HLT_2j55_bperf_split_2j55',
    # J75_3J20
    'HLT_j100_2j55_bperf', 'HLT_j100_2j55_bperf_split',
    # J100
    'HLT_j150_bperf_j50_bperf', 'HLT_j150_bperf_split_j50_bperf_split',
    'HLT_j175_bperf_j60_bperf', 'HLT_j175_bperf_split_j60_bperf_split',
    ]
bjet_Rules_supporting.update(dict(map(None,HLT_list,len(HLT_list)*[{
    1000 : TriggerRule(rate=0.2, comment=''), 
    }])))

HLT_list=[
    # RD0
    'HLT_j15_bperf',
    # J20
    'HLT_j55_bperf',
    # J30
    'HLT_j110_bperf',
    ]
bjet_Rules_supporting.update(dict(map(None,HLT_list,len(HLT_list)*[{
    1000 : TriggerRule(rate=0.2, comment='Express', ESRate=0.2), 
    }])))

# ========================================
# mu + jets

L1_list=['L1_MU4_3J15', 'L1_MU4_3J20']
bjet_Rules_supporting.update(dict(map(None,L1_list,len(L1_list)*[{
                       1000 : TriggerRule(rate=1000, maxRate=1000, comment='Prescaled'),
    lumi_l1topo_enable_btag : TriggerRule(PS=-1, comment='Disabled after L1Topo'),
    }])))

HLT_list=[
    # 4J20
    'HLT_mu4_j40_dr05_3j40_L14J20',
    'HLT_mu4_3j30_dr05_j30_bmedium_L14J20',
    'HLT_mu4_3j35_dr05_j35_bloose_L14J20',
    # MU4_3J15
    'HLT_mu4_3j45_dr05_L1MU4_3J15',
    'HLT_mu4_4j40_dr05_L1MU4_3J15',
    'HLT_mu4_2j35_dr05_j35_bmedium_L1MU4_3J15',
    'HLT_mu4_2j40_dr05_j40_bloose_L1MU4_3J15',
    'HLT_mu4_3j30_dr05_j30_bmedium_L1MU4_3J15',
    'HLT_mu4_3j35_dr05_j35_bloose_L1MU4_3J15',
    'HLT_mu4_j60_dr05_2j35_L1MU4_3J15',
    'HLT_mu6_j50_dr05_2j35_L1MU4_3J15',
    # MU4_3J20
    'HLT_mu4_3j45_dr05_L1MU4_3J20',
    'HLT_mu4_4j40_dr05_L1MU4_3J20',
    'HLT_mu4_2j35_dr05_j35_bmedium_L1MU4_3J20',
    'HLT_mu4_2j40_dr05_j40_bloose_L1MU4_3J20',
    'HLT_mu4_3j30_dr05_j30_bmedium_L1MU4_3J20',
    'HLT_mu4_3j35_dr05_j35_bloose_L1MU4_3J20',
    'HLT_mu4_j60_dr05_2j35_L1MU4_3J20',
    'HLT_mu6_j50_dr05_2j35_L1MU4_3J20',
    ]
bjet_Rules_supporting.update(dict(map(None,HLT_list,len(HLT_list)*[{
    1000 : TriggerRule(rate=1.0, comment=''),
    }])))

# ----------------------------------------

### Disabled due to ATR-11126
###L1_list=['L1_MU4_J12', 'L1_MU6_J75']
###bjet_Rules_supporting.update(dict(map(None,L1_list,len(L1_list)*[{
###                       1000 : TriggerRule(rate=500, maxRate=500, comment='Prescaled'),
###    lumi_l1topo_enable_btag : TriggerRule(PS=-1, comment='Disabled after L1Topo'),
###    }])))

HLT_list=[
    # MU4
    'HLT_mu4_j15_bperf_dr05',
    'HLT_mu4_j25_bperf_dr05',
    'HLT_mu4_j35_bperf_dr05',
    # MU4_J12
    'HLT_mu4_j55_bperf_dr05',
    # MU6_J20
    'HLT_mu6_j85_bperf_dr05',
    'HLT_mu6_j110_bperf_dr05',
    # MU6_J40
    'HLT_mu6_j150_bperf_dr05',
    'HLT_mu6_j175_bperf_dr05',
    # MU6_J75
    'HLT_mu6_j260_bperf_dr05',
    'HLT_mu6_j320_bperf_dr05',
    'HLT_mu6_j400_bperf_dr05',
    ]
bjet_Rules_supporting.update(dict(map(None,HLT_list,len(HLT_list)*[{
### Disabled due to ATR-11126 
###    1000 : TriggerRule(rate=0.2, comment=''),
    1000 : TriggerRule(PS=-1, comment='Disabled because chain is not working (ATR-11126)'),
    }])))

########################################

bjet_Rules={}

RulesList=[bjet_Rules_2e34, bjet_Rules_1e34, bjet_Rules_0p5e34, bjet_Rules_0p3e34, bjet_Rules_0p15e34, bjet_Rules_supporting] 
for Rules in RulesList:
    for newrule in Rules.keys():
        if bjet_Rules.has_key(newrule):
            print 'CRITICAL: duplicated rule cannot be added.',newrule
    bjet_Rules.update(Rules)


#################################################################################################
#
#
# Tau
#
#
#################################################################################################


########################################
# 2e34 

tau_Rules_2e34={}

L1_list=[
    # single
    'L1_TAU60',
    #
    # tau + xe, w/ jet
    'L1_TAU20IM_2J20_XE45',
    #
    # di-tau + xe
    'L1_TAU20IM_2TAU12IM_XE35','L1_TAU20IM_2TAU12IM_XE40',
    #
    # tau + e (high et, w/o jet)
    'L1_EM15HI_TAU40_2TAU15',
    #
    # tau + e + xe
    'L1_EM15HI_2TAU12IM_XE35',
    #
    # tau + mu + xe
    'L1_MU10_TAU12IM_XE35','L1_MU10_TAU12IM_XE40',
    ]

tau_Rules_2e34.update(dict(map(None,L1_list,len(L1_list)*[{
    1000 : TriggerRule(PS=1, comment='L1'), 
    }])))

HLT_list=[
    #
    # single tau
    'HLT_tau160_medium1_tracktwo',
    #
    # high-pT di-tau from single Tau
    'HLT_tau125_medium1_tracktwo_tau50_medium1_tracktwo_L1TAU12',
    #
    # tau + xe
    'HLT_tau35_medium1_tracktwo_xe70_L1XE45',
    #
    # di-tau + xe
    'HLT_tau35_medium1_tracktwo_tau25_medium1_tracktwo_xe50',
    #
    # tau + e (high et, w/o jet)
    'HLT_e17_medium_tau80_medium1_tracktwo',
    'HLT_e17_lhmedium_tau80_medium1_tracktwo', 'HLT_e17_lhmedium_nod0_tau80_medium1_tracktwo',
    #
    # tau + e + xe
    'HLT_e17_medium_tau25_medium1_tracktwo_xe50',
    'HLT_e17_lhmedium_tau25_medium1_tracktwo_xe50', 'HLT_e17_lhmedium_nod0_tau25_medium1_tracktwo_xe50',
    #
    'HLT_e17_medium_iloose_tau25_medium1_tracktwo_xe50',
    'HLT_e17_lhmedium_iloose_tau25_medium1_tracktwo_xe50', 'HLT_e17_lhmedium_nod0_iloose_tau25_medium1_tracktwo_xe50',
    #
    # tau + mu (high et, w/o jet)
    'HLT_mu14_iloose_tau35_medium1_tracktwo',
    #
    # tau + mu + xe
    'HLT_mu14_iloose_tau25_medium1_tracktwo_xe50',
    ]

tau_Rules_2e34.update(dict(map(None,HLT_list,len(HLT_list)*[{
    1000 : TriggerRule(PS=1, comment=''), 
    }])))

########################################
# 1e34 

tau_Rules_1e34={}

HLT_list=[
    #
    # single tau
    'HLT_tau125_medium1_tracktwo','HLT_tau125_medium1_calo','HLT_tau125_medium1_track',
    #
    # high-pT di-tau from single Tau
    'HLT_tau80_medium1_tracktwo_L1TAU60_tau50_medium1_tracktwo_L1TAU12',
    #
    # tau + muon + xe
    'HLT_mu14_tau25_medium1_tracktwo_xe50',
    ]
tau_Rules_1e34.update(dict(map(None,HLT_list,len(HLT_list)*[{
     1000 : TriggerRule(PS=1, comment=''), 
    10001 : TriggerRule(PS=-1, comment='Disabled'),
    }])))

# --------------------------------------

L1_list=['L1_EM15HI_2TAU12IM_J25_3J12']
tau_Rules_1e34.update(dict(map(None,L1_list,len(L1_list)*[{
                        1000 : TriggerRule(PS=1, comment='L1'), 
    lumi_l1topo_enable_tau_e : TriggerRule(rate=500, maxRate=500, comment='Prescaled'),
    }])))

HLT_list=['HLT_e17_medium_iloose_tau25_medium1_tracktwo','HLT_e17_lhmedium_iloose_tau25_medium1_tracktwo','HLT_e17_lhmedium_nod0_iloose_tau25_medium1_tracktwo',]
tau_Rules_1e34.update(dict(map(None,HLT_list,len(HLT_list)*[{
                        1000 : TriggerRule(PS=1, comment=''), 
    lumi_l1topo_enable_tau_e : TriggerRule(rate=0.1, comment='Prescaled'),
    }])))

# --------------------------------------

L1_list=['L1_MU10_TAU12IM_J25_2J12',]
tau_Rules_1e34.update(dict(map(None,L1_list,len(L1_list)*[{
                         1000 : TriggerRule(PS=1, comment='L1'), 
    lumi_l1topo_enable_tau_mu : TriggerRule(rate=500, maxRate=500, comment='Prescaled'),
    }])))

HLT_list=['HLT_mu14_iloose_tau25_medium1_tracktwo',]
tau_Rules_1e34.update(dict(map(None,HLT_list,len(HLT_list)*[{
                         1000 : TriggerRule(PS=1, comment=''), 
    lumi_l1topo_enable_tau_mu : TriggerRule(rate=0.1, comment='Prescaled'),
    }])))

# --------------------------------------

L1_list=['L1_TAU20IM_2TAU12IM_J25_2J20_3J12',]
tau_Rules_1e34.update(dict(map(None,L1_list,len(L1_list)*[{
                          1000 : TriggerRule(PS=1, comment='L1'), 
    lumi_l1topo_enable_tau_tau : TriggerRule(rate=500, maxRate=500, comment='Prescaled'),
    }])))

HLT_list=['HLT_tau35_medium1_tracktwo_tau25_medium1_tracktwo',
          'HLT_tau35_tight1_tracktwo_tau25_tight1_tracktwo', # backup
          ]
tau_Rules_1e34.update(dict(map(None,HLT_list,len(HLT_list)*[{
                          1000 : TriggerRule(PS=1, comment=''), 
    lumi_l1topo_enable_tau_tau : TriggerRule(rate=0.1, comment='Prescaled'),
    }])))

########################################
# 0.7e34 

tau_Rules_0p7e34={}

HLT_list=['HLT_tau35_loose1_tracktwo_tau25_loose1_tracktwo',]
tau_Rules_0p7e34.update(dict(map(None,HLT_list,len(HLT_list)*[{
    1000 : TriggerRule(PS=1, comment=''), 
    7001 : TriggerRule(rate=0.1, comment='Prescaled'),
    }])))

########################################
# 0.5e34 

tau_Rules_0p5e34={}

HLT_list=[
    #
    # single tau
    'HLT_tau80_medium1_tracktwo_L1TAU60',
    #
    # tau + e
    'HLT_e17_medium_tau25_medium1_tracktwo','HLT_e17_lhmedium_tau25_medium1_tracktwo','HLT_e17_lhmedium_nod0_tau25_medium1_tracktwo',
    #
    # tau + mu
    'HLT_mu14_tau25_medium1_tracktwo',
    #
    # tau + mu (high et, w/o jet)
    'HLT_mu14_tau35_medium1_tracktwo',
    ]
tau_Rules_0p5e34.update(dict(map(None,HLT_list,len(HLT_list)*[{
    1000 : TriggerRule(PS=1, comment=''), 
    5001 : TriggerRule(PS=-1, comment='Disabled'),
    }])))

# --------------------------------------

L1_list=['L1_TAU20_2TAU12_XE35','L1_TAU20IM_2TAU12IM',]

tau_Rules_0p5e34.update(dict(map(None,L1_list,len(L1_list)*[{
                         1000 : TriggerRule(PS=1, comment='L1'), 
    lumi_l1jet_enable_tau_tau : TriggerRule(PS=-1, comment='Disabled'),
    }])))

HLT_list=[
    'HLT_tau35_medium1_tracktwo_tau25_medium1_tracktwo_L1TAU20IM_2TAU12IM',
    'HLT_tau35_tight1_tracktwo_tau25_tight1_tracktwo_L1TAU20IM_2TAU12IM',
    #
    # di-tau + xe
    'HLT_tau35_medium1_tracktwo_L1TAU20_tau25_medium1_tracktwo_L1TAU12_xe50',
    ]
tau_Rules_0p5e34.update(dict(map(None,HLT_list,len(HLT_list)*[{
                         1000 : TriggerRule(PS=1, comment=''), 
    lumi_l1jet_enable_tau_tau : TriggerRule(PS=-1, comment='Disabled'),
    }])))

########################################
# 0.3e34 

tau_Rules_0p3e34={}

HLT_list=['HLT_tau80_medium1_tracktwo','HLT_tau80_medium1_calo','HLT_tau80_medium1_track',]
tau_Rules_0p3e34.update(dict(map(None,HLT_list,len(HLT_list)*[{
    1000 : TriggerRule(PS=1, comment=''), 
    3001 : TriggerRule(PS=-1, comment='Disabled'),
    }])))

# --------------------------------------

L1_list=['L1_TAU30', 'L1_TAU40',]
tau_Rules_0p3e34.update(dict(map(None,L1_list,len(L1_list)*[{
                     1000 : TriggerRule(PS=1, comment='L1'), 
    lumi_disable_l1_tau30 : TriggerRule(PS=-1, comment='Disabled'),
    }])))

# --------------------------------------

L1_list=[
    'L1_TAU20_2TAU12',
    #
    'L1_MU10_TAU20',
    #
    # tau + xe, w/ jet
    'L1_TAU20_2J20_XE45',
    ]
tau_Rules_0p3e34.update(dict(map(None,L1_list,len(L1_list)*[{
                     1000 : TriggerRule(PS=1, comment='L1'), 
    lumi_l1iso_enable_tau : TriggerRule(PS=-1, comment='Disabled'),
    }])))

HLT_list=[
    # 
    # 2tau
    'HLT_tau35_medium1_tracktwo_L1TAU20_tau25_medium1_tracktwo_L1TAU12',
    #
    # tau + mu (high et, w/o jet)
    # w/o L1 tau iso
    'HLT_mu14_tau35_medium1_tracktwo_L1TAU20',
    #
    # tau + xe
    'HLT_tau35_medium1_tracktwo_L1TAU20_xe70_L1XE45',
    ]
tau_Rules_0p3e34.update(dict(map(None,HLT_list,len(HLT_list)*[{
                     1000 : TriggerRule(PS=1, comment=''), 
    lumi_l1iso_enable_tau : TriggerRule(PS=-1, comment='Disabled'),
    }])))

########################################
# 0.15e34 

tau_Rules_0p15e34={}

HLT_list=['HLT_tau35_loose1_tracktwo_tau25_loose1_tracktwo_L1TAU20IM_2TAU12IM',]
tau_Rules_0p15e34.update(dict(map(None,HLT_list,len(HLT_list)*[{
    1000 : TriggerRule(PS=1, comment=''), 
    1501 : TriggerRule(PS=-1, comment='Disabled'),
    }])))

########################################
# supporting

tau_Rules_supporting = {
    # --------------------------------------
    # Express
    'HLT_tau0_perf_ptonly_L1TAU12'                  : {  1000 : TriggerRule(rate=0.4, comment='Express', ESValue=1, rerun=1) },
    'HLT_tau0_perf_ptonly_L1TAU60'                  : {  1000 : TriggerRule(rate=0.2, comment='Express', ESValue=1, rerun=1) },
    'HLT_tau25_medium1_tracktwo'                    : {  1000 : TriggerRule(rate=0.2, comment='Express', ESValue=1, rerun=1) },
    'HLT_tau35_perf_tracktwo_tau25_perf_tracktwo'   : {  1000 : TriggerRule(rate=0.4, comment='Express', ESValue=1, rerun=1) },
    #
    'HLT_tau25_idperf_tracktwo'                     : {  1000 : TriggerRule(rate=0.2, comment='Express', ESValue=1, rerun=1) },
    'HLT_tau25_idperf_track'                        : {  1000 : TriggerRule(rate=0.2, comment='Express', ESValue=1, rerun=1) },
    #
    ### cost file does not contain xe35 --> setting PS by hand for now
    ###'HLT_tau160_idperf_tracktwo'                    : {  1000 : TriggerRule(rate=0.2, comment='Express', ESValue=1, rerun=1) },
    ###'HLT_tau160_idperf_track'                       : {  1000 : TriggerRule(rate=0.2, comment='Express', ESValue=1, rerun=1) },
    'HLT_tau160_idperf_tracktwo'                    : {  1000 : TriggerRule(PS=100, comment='Express', ESValue=1, rerun=1) },
    'HLT_tau160_idperf_track'                       : {  1000 : TriggerRule(PS=100, comment='Express', ESValue=1, rerun=1) },

    # --------------------------------------
    # Z T&P
    'HLT_mu14_iloose_tau25_perf_tracktwo'           : {  1000 : TriggerRule(PS=1, comment='') },
}

# --------------------------------------

HLT_list=[
    'HLT_tau5_perf_ptonly_L1TAU8',
    'HLT_tau20_r1_idperf',
    'HLT_tau25_r1_idperf',
    'HLT_tau25_loose1_ptonly',
    'HLT_tau25_loose1_tracktwo',
    'HLT_tau25_medium1_mvonly',
    'HLT_tau25_medium1_ptonly',
    'HLT_tau25_medium1_track',
    'HLT_tau25_medium1_tracktwo_L1TAU12',
    'HLT_tau25_medium1_tracktwo_L1TAU12IL',
    'HLT_tau25_medium1_tracktwo_L1TAU12IT',
    'HLT_tau25_perf_ptonly',
    'HLT_tau25_perf_ptonly_L1TAU12',
    'HLT_tau25_perf_track',
    'HLT_tau25_perf_track_L1TAU12',
    'HLT_tau25_perf_tracktwo',
    'HLT_tau25_perf_tracktwo_L1TAU12',
    'HLT_tau25_tight1_ptonly',
    'HLT_tau25_tight1_tracktwo',
    'HLT_tau35_loose1_ptonly',
    'HLT_tau35_loose1_tracktwo',
    'HLT_tau35_medium1_ptonly',
    'HLT_tau35_medium1_ptonly_L1TAU20',
    'HLT_tau35_medium1_track',
    'HLT_tau35_medium1_tracktwo',
    'HLT_tau35_medium1_tracktwo_L1TAU20',
    'HLT_tau35_medium1_tracktwo_L1TAU20IL',
    'HLT_tau35_medium1_tracktwo_L1TAU20IT',
    'HLT_tau35_perf_ptonly',
    'HLT_tau35_perf_tracktwo',
    'HLT_tau35_tight1_ptonly',
    'HLT_tau50_medium1_tracktwo_L1TAU12',
    'HLT_tau125_perf_ptonly',
    'HLT_tau125_perf_tracktwo',
    ]
tau_Rules_supporting.update(dict(map(None,HLT_list,len(HLT_list)*[{
    1000 : TriggerRule(PS=0, comment='', rerun=1), 
    }])))

########################################

tau_Rules={}

RulesList=[tau_Rules_2e34, tau_Rules_1e34, tau_Rules_0p7e34, tau_Rules_0p5e34, tau_Rules_0p3e34, tau_Rules_0p15e34, tau_Rules_supporting] 
for Rules in RulesList:
    for newrule in Rules.keys():
        if tau_Rules.has_key(newrule):
            print 'CRITICAL: duplicated rule cannot be added.',newrule
    tau_Rules.update(Rules)


#################################################################################################
#
#
# e/gamma
#
#
#################################################################################################


########################################
# 2e34 

egamma_Rules_2e34={}

L1_list=['L1_EM22VHI','L1_2EM15VH','L1_EM15VH_3EM7',]
egamma_Rules_2e34.update(dict(map(None,L1_list,len(L1_list)*[{
    1000 : TriggerRule(PS=1, comment='L1'), 
    }])))

HLT_list=[
    # 1e
    'HLT_e26_tight_iloose',
    'HLT_e26_lhtight_iloose','HLT_e26_lhtight_nod0_iloose','HLT_e26_lhtight_cutd0dphideta_iloose','HLT_e26_lhtight_iloose_HLTCalo',
    'HLT_e26_lhtight_smooth_iloose',
    #
    'HLT_e60_medium',
    'HLT_e60_lhmedium','HLT_e60_lhmedium_nod0','HLT_e60_lhmedium_cutd0dphideta','HLT_e60_lhmedium_HLTCalo',
    #
    'HLT_e140_loose',
    'HLT_e140_lhloose','HLT_e140_lhloose_nod0','HLT_e140_lhloose_HLTCalo',
    #
    # 2e
    'HLT_2e17_lhloose_nod0','HLT_2e17_lhloose_cutd0dphideta','HLT_2e17_lhloose_HLTCalo',
    #
    # 3e
    'HLT_e17_medium_2e9_medium',
    'HLT_e17_lhmedium_2e9_lhmedium','HLT_e17_lhmedium_nod0_2e9_lhmedium_nod0',
    #
    'HLT_e17_medium_iloose_2e9_medium',
    'HLT_e17_lhmedium_iloose_2e9_lhmedium','HLT_e17_lhmedium_nod0_iloose_2e9_lhmedium_nod0',
    #
    # 1g
    'HLT_g200_etcut',
    #
    # 2g
    'HLT_g35_medium_g25_medium','HLT_g35_medium_HLTCalo_g25_medium_HLTCalo',
    'HLT_2g60_loose_L12EM15VH',
    'HLT_2g22_tight',
    'HLT_2g25_tight',
    #
    # 3g
    'HLT_3g20_loose',
    ]

egamma_Rules_2e34.update(dict(map(None,HLT_list,len(HLT_list)*[{
    1000 : TriggerRule(PS=1, comment=''), 
    }])))

# --------------------------------------

HLT_list=['HLT_g140_loose']

egamma_Rules_2e34.update(dict(map(None,HLT_list,len(HLT_list)*[{
    1000 : TriggerRule(PS=1, comment=''), 
    5001 : TriggerRule(PS=1, comment='Express', ESRate=0.2), 
    }])))

# --------------------------------------

HLT_list=['HLT_2e17_loose', 'HLT_2e17_lhloose']

egamma_Rules_2e34.update(dict(map(None,HLT_list,len(HLT_list)*[{
                       1000 : TriggerRule(PS=1, comment=''), 
    lumi_disable_l1_2em10vh : TriggerRule(PS=1, comment='Express', ESRate=0.2), 
    }])))

########################################
# 1e34 

egamma_Rules_1e34={}

HLT_list=[
    # 3e
    'HLT_e17_loose_2e9_loose',
    'HLT_e17_lhloose_2e9_lhloose','HLT_e17_lhloose_nod0_2e9_lhloose_nod0',
    # 2g
    'HLT_2g50_loose',
    'HLT_2g20_tight',
    # 3g
    'HLT_2g20_loose_g15_loose',
    ]
egamma_Rules_1e34.update(dict(map(None,HLT_list,len(HLT_list)*[{
     1000 : TriggerRule(PS=1, comment=''), 
    10001 : TriggerRule(PS=-1, comment='Disabled'),
    }])))

# --------------------------------------

L1_list=['L1_EM20VHI',]
egamma_Rules_1e34.update(dict(map(None,L1_list,len(L1_list)*[{
                       1000 : TriggerRule(PS=1, comment='L1'),
    lumi_disable_l1_em20vhi : TriggerRule(PS=-1, comment='Disabled'),
                       }])))

HLT_list=['HLT_e24_tight_iloose',
          'HLT_e24_lhtight_iloose','HLT_e24_lhtight_nod0_iloose','HLT_e24_lhtight_cutd0dphideta_iloose','HLT_e24_lhtight_iloose_HLTCalo',]
egamma_Rules_1e34.update(dict(map(None,HLT_list,len(HLT_list)*[{
                       1000 : TriggerRule(PS=1, comment=''), 
    lumi_disable_l1_em20vhi : TriggerRule(PS=-1, comment='Disabled'),
    }])))

# --------------------------------------

L1_list=['L1_2EM13VH',]
egamma_Rules_1e34.update(dict(map(None,L1_list,len(L1_list)*[{
                       1000 : TriggerRule(PS=1, comment='L1'),
    lumi_disable_l1_2em13vh : TriggerRule(PS=-1, comment='Disabled'),
    }])))

HLT_list=['HLT_2e15_loose_L12EM13VH',
          'HLT_2e15_lhloose_L12EM13VH','HLT_2e15_lhloose_nod0_L12EM13VH','HLT_2e15_lhloose_cutd0dphideta_L12EM13VH','HLT_2e15_lhloose_HLTCalo_L12EM13VH',]
egamma_Rules_1e34.update(dict(map(None,HLT_list,len(HLT_list)*[{
                       1000 : TriggerRule(PS=1, comment=''), 
    lumi_disable_l1_2em13vh : TriggerRule(PS=-1, comment='Disabled'),
    }])))

########################################
# 0.5e34 

egamma_Rules_0p5e34={}

HLT_list=[
    # 1e
    'HLT_e120_loose',
    'HLT_e120_lhloose','HLT_e120_lhloose_nod0', 'HLT_e120_lhloose_HLTCalo',
    # 1g
    'HLT_g120_loose_HLTCalo',
    # 3g
    'HLT_3g15_loose',
    ]

egamma_Rules_0p5e34.update(dict(map(None,HLT_list,len(HLT_list)*[{
    1000 : TriggerRule(PS=1, comment=''), 
    5001 : TriggerRule(PS=-1, comment='Disabled'),
    }])))

# --------------------------------------

HLT_list=['HLT_g120_loose']

egamma_Rules_0p5e34.update(dict(map(None,HLT_list,len(HLT_list)*[{
    1000 : TriggerRule(PS=1, comment='Express', ESRate=0.2), 
    5001 : TriggerRule(PS=-1, comment='Disabled'),
    }])))

# --------------------------------------

L1_list=['L1_EM20VH',]

egamma_Rules_0p5e34.update(dict(map(None,L1_list,len(L1_list)*[{
                    1000 : TriggerRule(PS=1, comment='L1'),
    lumi_l1iso_enable_em : TriggerRule(PS=32, comment='Prescaled by 32 for supporting'),
                    }])))

HLT_list=[
    'HLT_e24_medium_iloose_L1EM20VH',
    'HLT_e24_lhmedium_iloose_L1EM20VH','HLT_e24_lhmedium_nod0_iloose_L1EM20VH','HLT_e24_lhmedium_iloose_HLTCalo_L1EM20VH','HLT_e24_lhmedium_cutd0dphideta_iloose_L1EM20VH',
    #
    'HLT_e24_tight_iloose_L1EM20VH','HLT_e24_tight_iloose_HLTCalo_L1EM20VH','HLT_e24_tight_iloose_L2EFCalo_L1EM20VH','HLT_e24_tight_iloose_etisem_L1EM20VH',
    'HLT_e24_lhtight_iloose_L1EM20VH','HLT_e24_lhtight_nod0_iloose_L1EM20VH','HLT_e24_lhtight_cutd0dphideta_iloose_L1EM20VH','HLT_e24_lhtight_iloose_L2EFCalo_L1EM20VH','HLT_e24_lhtight_iloose_HLTCalo_L1EM20VH',
    ]

egamma_Rules_0p5e34.update(dict(map(None,HLT_list,len(HLT_list)*[{
                    1000 : TriggerRule(PS=1, comment=''), 
    lumi_l1iso_enable_em : TriggerRule(PS=-1, comment='Disabled'),
    }])))

# --------------------------------------

L1_list=['L1_2EM10VH',]
egamma_Rules_0p5e34.update(dict(map(None,L1_list,len(L1_list)*[{
                       1000 : TriggerRule(PS=1, comment='L1'),
    lumi_disable_l1_2em10vh : TriggerRule(PS=-1, comment='Disabled'),
                       }])))

HLT_list=['HLT_2e12_loose_L12EM10VH', 'HLT_2e12_lhloose_L12EM10VH']

egamma_Rules_0p5e34.update(dict(map(None,HLT_list,len(HLT_list)*[{
                       1000 : TriggerRule(PS=1, comment='Express', ESRate=0.2), 
    lumi_disable_l1_2em10vh : TriggerRule(PS=-1, comment='Disabled'),
    }])))

# --------------------------------------

HLT_list=['HLT_2e12_lhloose_cutd0dphideta_L12EM10VH','HLT_2e12_lhloose_HLTCalo_L12EM10VH','HLT_2e12_lhloose_nodphires_L12EM10VH','HLT_2e12_lhloose_nodeta_L12EM10VH','HLT_2e12_lhloose_nod0_L12EM10VH',
    ]

egamma_Rules_0p5e34.update(dict(map(None,HLT_list,len(HLT_list)*[{
                       1000 : TriggerRule(PS=1, comment=''), 
    lumi_disable_l1_2em10vh : TriggerRule(PS=-1, comment='Disabled'),
    }])))

########################################
# 0.3e34 

egamma_Rules_0p3e34={}

L1_list=['L1_EM18VH',]
egamma_Rules_0p3e34.update(dict(map(None,L1_list,len(L1_list)*[{
                      1000 : TriggerRule(PS=1, comment='L1'),
    lumi_disable_l1_em18vh : TriggerRule(PS=32, comment='Prescaled by 32 for supporting'),
    }])))

HLT_list=[
    'HLT_e24_medium_iloose_L1EM18VH',
    'HLT_e24_lhmedium_iloose_L1EM18VH','HLT_e24_lhmedium_nod0_iloose_L1EM18VH','HLT_e24_lhmedium_nodphires_iloose_L1EM18VH','HLT_e24_lhmedium_iloose_HLTCalo_L1EM18VH','HLT_e24_lhmedium_cutd0dphideta_iloose_L1EM18VH','HLT_e24_lhmedium_nodeta_iloose_L1EM18VH',
    ]

egamma_Rules_0p3e34.update(dict(map(None,HLT_list,len(HLT_list)*[{
                      1000 : TriggerRule(PS=1, comment=''), 
    lumi_disable_l1_em18vh : TriggerRule(PS=-1, comment='Disabled'),
    }])))

# --------------------------------------

L1_list=['L1_2EM15',]
egamma_Rules_0p3e34.update(dict(map(None,L1_list,len(L1_list)*[{
                     1000 : TriggerRule(PS=1, comment='L1'),
    lumi_disable_l1_2em15 : TriggerRule(rate=1000, maxRate=1000, comment='Prescaled'),
    }])))

HLT_list=['HLT_g35_loose_L1EM15_g25_loose_L1EM15',]
egamma_Rules_0p3e34.update(dict(map(None,HLT_list,len(HLT_list)*[{
                     1000 : TriggerRule(PS=1, comment=''), 
    lumi_disable_l1_2em15 : TriggerRule(PS=-1, comment='Disabled'),
    }])))

########################################
# supporting

egamma_Rules_supporting={}

# ========================================
# support for electron
# ATR-11040

HLT_list=[
    'HLT_e5_etcut',
    'HLT_e10_etcut_L1EM7',
    'HLT_e15_etcut_L1EM7',
    'HLT_e20_etcut_L1EM12',
    'HLT_e25_etcut_L1EM15',
    'HLT_e30_etcut_L1EM15',
    'HLT_e40_etcut_L1EM15',
    'HLT_e50_etcut_L1EM15',
    'HLT_e60_etcut',
    # 'HLT_e70_etcut', missing?
    'HLT_e80_etcut',
    'HLT_e100_etcut',
    'HLT_e120_etcut',
    ]

egamma_Rules_supporting.update(dict(map(None,HLT_list,len(HLT_list)*[{
    1000 : TriggerRule(rate=1.0, comment=''), 
    }])))

# --------------------------------------

HLT_list=[
    'HLT_e5_vloose',        'HLT_e5_lhvloose',        'HLT_e5_lhvloose_nod0',
    'HLT_e10_vloose_L1EM7', 'HLT_e10_lhvloose_L1EM7',
    'HLT_e15_vloose_L1EM7', 'HLT_e15_lhvloose_L1EM7',
    'HLT_e20_vloose',       'HLT_e20_lhvloose',       'HLT_e20_lhvloose_nod0',
    'HLT_e25_vloose_L1EM15','HLT_e25_lhvloose_L1EM15',
    'HLT_e30_vloose_L1EM15','HLT_e30_lhvloose_L1EM15',
    'HLT_e40_vloose_L1EM15','HLT_e40_lhvloose_L1EM15',
    'HLT_e50_vloose_L1EM15','HLT_e50_lhvloose_L1EM15',
    'HLT_e70_vloose',       'HLT_e70_lhvloose',
    'HLT_e80_vloose',       'HLT_e80_lhvloose',
    'HLT_e100_vloose',      'HLT_e100_lhvloose',
    'HLT_e120_vloose',      'HLT_e120_lhvloose',
    ]

egamma_Rules_supporting.update(dict(map(None,HLT_list,len(HLT_list)*[{
    1000 : TriggerRule(rate=0.02, comment='',rerun=1), 
    }])))

# --------------------------------------

egamma_Rules_supporting.update({
    'HLT_e12_vloose_L1EM10VH'         : {  1000 : TriggerRule(PS=11000, comment='', rerun=1) },
    'HLT_e12_lhvloose_L1EM10VH'       : {  1000 : TriggerRule(PS=6300,  comment='', rerun=1) },
    'HLT_e12_lhvloose_nod0_L1EM10VH'  : {  1000 : TriggerRule(PS=6300,  comment='', rerun=1) },

    'HLT_e15_vloose_L1EM13VH'         : {  1000 : TriggerRule(PS=4560, comment='', rerun=1) },
    'HLT_e15_lhvloose_L1EM13VH'       : {  1000 : TriggerRule(PS=2660, comment='', rerun=1) },
    'HLT_e15_lhvloose_nod0_L1EM13VH'  : {  1000 : TriggerRule(PS=2660, comment='', rerun=1) },

    'HLT_e17_vloose'                  : {  1000 : TriggerRule(PS=2910, comment='', rerun=1) },
    'HLT_e17_lhvloose'                : {  1000 : TriggerRule(PS=1760, comment='', rerun=1) },
    'HLT_e17_lhvloose_nod0'           : {  1000 : TriggerRule(PS=1760, comment='', rerun=1) },

    'HLT_e20_vloose_L1EM12'           : {  1000 : TriggerRule(PS=2080, comment='', rerun=1) },
    'HLT_e20_lhvloose_L1EM12'         : {  1000 : TriggerRule(PS=1500, comment='', rerun=1) },
    'HLT_e20_lhvloose_nod0_L1EM12'    : {  1000 : TriggerRule(PS=1500, comment='', rerun=1) },

    'HLT_e40_vloose'                  : {  1000 : TriggerRule(PS=240, comment='', rerun=1) },
    'HLT_e40_lhvloose'                : {  1000 : TriggerRule(PS=190, comment='', rerun=1) },
    'HLT_e40_lhvloose_nod0'           : {  1000 : TriggerRule(PS=190, comment='', rerun=1) },
    
    'HLT_e60_vloose'                  : {  1000 : TriggerRule(PS=55, comment='', rerun=1) },
    'HLT_e60_lhvloose'                : {  1000 : TriggerRule(PS=48, comment='', rerun=1) },
    'HLT_e60_lhvloose_nod0'           : {  1000 : TriggerRule(PS=48, comment='', rerun=1) },
})
    
# --------------------------------------

HLT_list=['HLT_e24_medium_L1EM18VH','HLT_e24_lhmedium_L1EM18VH']

egamma_Rules_supporting.update(dict(map(None,HLT_list,len(HLT_list)*[{
                      1000 : TriggerRule(PS=32, comment='Express', ESRate=0.2),
    lumi_disable_l1_em18vh : TriggerRule(PS=1, comment='L1 is prescaled'),
    }])))

HLT_list=[
    'HLT_e24_lhmedium_nod0_L1EM18VH',
    'HLT_e24_lhmedium_nodeta_L1EM18VH',
    'HLT_e24_lhmedium_cutd0dphideta_L1EM18VH',
    'HLT_e24_lhmedium_nodphires_L1EM18VH',
    ]

egamma_Rules_supporting.update(dict(map(None,HLT_list,len(HLT_list)*[{
                      1000 : TriggerRule(PS=32, comment=''),
    lumi_disable_l1_em18vh : TriggerRule(PS=1, comment='L1 is prescaled'),
    }])))

# --------------------------------------

HLT_list=['HLT_e24_tight_L1EM20VH', 'HLT_e24_lhtight_L1EM20VH']

egamma_Rules_supporting.update(dict(map(None,HLT_list,len(HLT_list)*[{
                      1000 : TriggerRule(PS=32, comment=''),
    lumi_disable_l1_em18vh : TriggerRule(PS=32, comment='Express', ESRate=0.2),
      lumi_l1iso_enable_em : TriggerRule(PS=1, comment='L1 is prescaled, Express', ESRate=0.2),
    }])))

HLT_list=['HLT_e24_lhtight_nod0_L1EM20VH']

egamma_Rules_supporting.update(dict(map(None,HLT_list,len(HLT_list)*[{
                    1000 : TriggerRule(PS=32, comment=''),
    lumi_l1iso_enable_em : TriggerRule(PS=1, comment='L1 is prescaled'),
    }])))

# ========================================
# rerun

HLT_list=[
    'HLT_e4_etcut',
    'HLT_e9_etcut',
    'HLT_e14_etcut',
    'HLT_e7_medium',           'HLT_e7_lhmedium',           'HLT_e7_lhmedium_nod0',    
    'HLT_e9_loose',            'HLT_e9_lhloose',            'HLT_e9_lhloose_nod0',
    'HLT_e9_medium',           'HLT_e9_lhmedium',           'HLT_e9_lhmedium_nod0',
    'HLT_e12_medium',          'HLT_e12_lhmedium',          'HLT_e12_lhmedium_nod0',
    'HLT_e17_medium',          'HLT_e17_lhmedium',          'HLT_e17_lhmedium_nod0',
    'HLT_e17_medium_iloose',   'HLT_e17_lhmedium_iloose',   'HLT_e17_lhmedium_nod0_iloose',
    'HLT_e20_medium',          'HLT_e20_lhmedium',          'HLT_e20_lhmedium_nod0',
    'HLT_e24_medium_L1EM15VH', 'HLT_e24_lhmedium_L1EM15VH', 'HLT_e24_lhmedium_nod0_L1EM15VH',
    'HLT_e24_medium_L1EM20VHI','HLT_e24_lhmedium_L1EM20VHI','HLT_e24_lhmedium_nod0_L1EM20VHI',
    'HLT_e26_medium_L1EM22VHI','HLT_e26_lhmedium_L1EM22VHI','HLT_e26_lhmedium_nod0_L1EM22VHI',
    'HLT_e60_loose',           'HLT_e60_lhloose',           'HLT_e60_lhloose_nod0',
    'HLT_e70_loose',           'HLT_e70_lhloose',           'HLT_e70_lhloose_nod0',
    'HLT_e5_tight',            'HLT_e5_lhtight',            'HLT_e5_lhtight_nod0',
    'HLT_e9_tight',            'HLT_e9_lhtight',            'HLT_e9_lhtight_nod0',
    'HLT_e14_tight',           'HLT_e14_lhtight',           'HLT_e14_lhtight_nod0',
    #
    'HLT_e12_loose_L1EM10VH',                'HLT_e12_lhloose_L1EM10VH',       'HLT_e12_lhloose_nod0_L1EM10VH',
    'HLT_e12_lhloose_cutd0dphideta_L1EM10VH','HLT_e12_lhloose_nodeta_L1EM10VH','HLT_e12_lhloose_nodphires_L1EM10VH',
    'HLT_e15_loose_L1EM13VH',                'HLT_e15_lhloose_L1EM13VH',       'HLT_e15_lhloose_nod0_L1EM13VH',
    'HLT_e15_lhloose_cutd0dphideta_L1EM13VH',
    'HLT_e17_loose',                         'HLT_e17_lhloose',                'HLT_e17_lhloose_nod0',
    'HLT_e17_lhloose_cutd0dphideta',
    #
    # missing from list but most likely needed
    'HLT_e5_loose',                        'HLT_e5_lhloose',
    'HLT_e12_loose',                       'HLT_e12_lhloose',              'HLT_e12_lhloose_nod0',
    'HLT_e17_loose_L1EM15',                'HLT_e17_lhloose_L1EM15',       'HLT_e17_lhloose_nod0_L1EM15',
    'HLT_e17_lhloose_cutd0dphideta_L1EM15','HLT_e17_lhloose_nodeta_L1EM15','HLT_e17_lhloose_nodphires_L1EM15',
    'HLT_e26_medium_L1EM20VHI',            'HLT_e26_lhmedium_L1EM20VHI',   'HLT_e26_lhmedium_nod0_L1EM20VHI',
    #
    'HLT_g10_loose',
    'HLT_g20_loose_L1EM15',
    'HLT_g25_loose',
    'HLT_g25_medium',
    'HLT_g35_loose',
    'HLT_g35_medium',
    'HLT_g40_tight',
    ]

egamma_Rules_supporting.update(dict(map(None,HLT_list,len(HLT_list)*[{
    1000 : TriggerRule(PS=0, comment='', rerun=1),
    }])))

# ========================================
# photon prescaled

HLT_list=[
    'HLT_g15_loose_L1EM7',
    'HLT_g20_loose_L1EM12',
    'HLT_g25_loose_L1EM15',
    'HLT_g35_loose_L1EM15',
    'HLT_g40_loose_L1EM15',
    'HLT_g45_loose_L1EM15',
    'HLT_g50_loose_L1EM15',
    'HLT_g60_loose',
    'HLT_g70_loose',
    'HLT_g80_loose',
    'HLT_g100_loose',
    ]
egamma_Rules_supporting.update(dict(map(None,HLT_list,len(HLT_list)*[{
    1000 : TriggerRule(rate=1.0, comment='',rerun=1), 
    }])))

# ========================================
# photon rerun

HLT_list=[
    'HLT_g10_medium', 
    'HLT_g15_loose_L1EM3',
    'HLT_g15_loose',
    'HLT_g20_loose',
    'HLT_g20_tight',
    'HLT_g50_loose',
    'HLT_g60_loose_L1EM15VH',    
    'HLT_g45_tight',
    ]
egamma_Rules_supporting.update(dict(map(None,HLT_list,len(HLT_list)*[{
    1000 : TriggerRule(PS=0, comment='rerun',rerun=1), 
    }])))

HLT_list=['HLT_g20_etcut_L1EM12',]
egamma_Rules_supporting.update(dict(map(None,HLT_list,len(HLT_list)*[{
    1000 : TriggerRule(rate=1.0, comment=''), 
    }])))


########################################
# T&P for electron

egamma_Rules_supporting.update({
    'HLT_e26_lhtight_e15_etcut_Zee'                  : {  1000 : TriggerRule(PS=1, comment=''),
                                          lumi_l1iso_enable_em : TriggerRule(PS=1, comment='Express', ESRate=0.8) },
    'HLT_e26_tight_e15_etcut_Zee'                    : {  1000 : TriggerRule(PS=1, comment='') },
    # 
    'HLT_e24_lhtight_L1EM20VH_e15_etcut_Zee'         : {  1000 : TriggerRule(PS=1, comment='Express', ESRate=0.8),
                                          lumi_l1iso_enable_em : TriggerRule(PS=-1, comment='Disabled') },
    'HLT_e24_tight_L1EM20VH_e15_etcut_Zee'           : {  1000 : TriggerRule(PS=1, comment=''),
                                          lumi_l1iso_enable_em : TriggerRule(PS=-1, comment='Disabled') },
    })

# --------------------------------------

L1_list=['L1_EM10_XS20', 'L1_EM15_XE30']
egamma_Rules_supporting.update(dict(map(None,L1_list,len(L1_list)*[{
    1000 : TriggerRule(rate=1000, maxRate=1000, comment='L1'),
    }])))

egamma_Rules_supporting.update({
    'HLT_e13_etcut_trkcut_xs15'    : {  1000 : TriggerRule(rate=1.0, comment='') }, # EM10_XS20
    'HLT_e18_etcut_trkcut_xs20'    : {  1000 : TriggerRule(rate=1.0, comment='') }, # EM15_XS30
    })

# --------------------------------------

egamma_Rules_supporting.update({
    'L1_2EM3'        : {  1000 : TriggerRule(rate=1000, maxRate=1000, comment='') },
    'L1_EM7_2EM3'    : {  1000 : TriggerRule(rate=1000, maxRate=1000, comment='') },
    'L1_EM12_2EM3'   : {  1000 : TriggerRule(rate=1000, maxRate=1000, comment='') },
    })

HLT_list=['HLT_e5_lhtight_e4_etcut_Jpsiee']
egamma_Rules_supporting.update(dict(map(None,HLT_list,len(HLT_list)*[{
    1000 : TriggerRule(rate=1.0, comment='Express', ESRate=0.4), 
    }])))

HLT_list=[
    'HLT_e14_tight_e4_etcut_Jpsiee', 'HLT_e14_lhtight_e4_etcut_Jpsiee', 'HLT_e14_lhtight_nod0_e4_etcut_Jpsiee',
    'HLT_e14_etcut_e5_tight_Jpsiee', 'HLT_e14_etcut_e5_lhtight_Jpsiee', 'HLT_e14_etcut_e5_lhtight_nod0_Jpsiee',
    #
    'HLT_e9_tight_e4_etcut_Jpsiee',  'HLT_e9_lhtight_e4_etcut_Jpsiee',  'HLT_e9_lhtight_nod0_e4_etcut_Jpsiee',
    'HLT_e9_etcut_e5_tight_Jpsiee',  'HLT_e9_etcut_e5_lhtight_Jpsiee',  'HLT_e9_etcut_e5_lhtight_nod0_Jpsiee',
    #
    'HLT_e5_tight_e4_etcut_Jpsiee',  'HLT_e5_lhtight_nod0_e4_etcut_Jpsiee',
    ]

egamma_Rules_supporting.update(dict(map(None,HLT_list,len(HLT_list)*[{
    1000 : TriggerRule(rate=1.0, comment=''), 
    }])))

########################################
# other rules

egamma_Rules_supporting.update({

    # ----------------------------------------
    # Di-photon triggers (medium pT)
    #

    # loose version (also for supporting)
    # PS=6 for supporting purpose (background evaluation to give ~5 Hz)
    'HLT_g35_loose_g25_loose'                                    : {  1000 : TriggerRule(PS=1, comment=''),
                                                                      5001 : TriggerRule(PS=6, comment='Prescaled as supporting') },

    # supporting ??
    'HLT_g35_medium_g25_medium_L1EM7_EMPTY'                      : {  1000 : TriggerRule(PS=1, comment='') },
    'HLT_g35_medium_g25_medium_L1EM7_UNPAIRED_ISO'               : {  1000 : TriggerRule(PS=1, comment='') },

    # ----------------------------------------
    # J/Psi T&P

    # supporting
    'HLT_e5_tight_e4_etcut'                                      : {  1000 : TriggerRule(rate=0.2, comment='') },
    'HLT_e5_lhtight_e4_etcut'                                    : {  1000 : TriggerRule(rate=0.2, comment='') },
    'HLT_e5_lhtight_nod0_e4_etcut'                               : {  1000 : TriggerRule(rate=0.2, comment='') },

    # ----------------------------------------
    # W T&P

    # supporting
    'HLT_e13_etcut_trkcut'                                       : {  1000 : TriggerRule(rate=0.2, comment='') },
    'HLT_e13_etcut_trkcut_L1EM10'                                : {  1000 : TriggerRule(rate=0.2, comment='') },
    'HLT_e13_etcut_trkcut_xe20'                                  : {  1000 : TriggerRule(rate=0.2, comment='') },
    'HLT_e18_etcut_trkcut'                                       : {  1000 : TriggerRule(rate=0.2, comment='') },
    'HLT_e18_etcut_trkcut_L1EM15'                                : {  1000 : TriggerRule(rate=0.2, comment='') },
    'HLT_e18_etcut_trkcut_xe35'                                  : {  1000 : TriggerRule(rate=0.2, comment='') },

    # ----------------------------------------
    # electron + nJets (backgrounds)
    # should be 1 Hz coherent
    'HLT_e26_vloose_L1EM20VH'                                    : {  1000 : TriggerRule(rate=0.5, comment='') },
    'HLT_e26_vloose_L1EM20VH_3j20noL1'                           : {  1000 : TriggerRule(rate=0.5, comment='') },
    'HLT_e26_vloose_L1EM20VH_4j20noL1'                           : {  1000 : TriggerRule(rate=0.5, comment='') },
    'HLT_e26_vloose_L1EM20VH_5j15noL1'                           : {  1000 : TriggerRule(rate=0.5, comment='') },
    'HLT_e26_vloose_L1EM20VH_6j15noL1'                           : {  1000 : TriggerRule(rate=0.5, comment='') },
    'HLT_e26_lhvloose_L1EM20VH'                                  : {  1000 : TriggerRule(rate=0.5, comment='') },
    'HLT_e26_lhvloose_L1EM20VH_3j20noL1'                         : {  1000 : TriggerRule(rate=0.5, comment='') },
    'HLT_e26_lhvloose_L1EM20VH_4j20noL1'                         : {  1000 : TriggerRule(rate=0.5, comment='') },
    'HLT_e26_lhvloose_L1EM20VH_5j15noL1'                         : {  1000 : TriggerRule(rate=0.5, comment='') },
    'HLT_e26_lhvloose_L1EM20VH_6j15noL1'                         : {  1000 : TriggerRule(rate=0.5, comment='') },
    'HLT_e26_lhvloose_nod0_L1EM20VH'                             : {  1000 : TriggerRule(rate=0.5, comment='') },
    'HLT_e26_lhvloose_nod0_L1EM20VH_3j20noL1'                    : {  1000 : TriggerRule(rate=0.5, comment='') },
    'HLT_e26_lhvloose_nod0_L1EM20VH_4j20noL1'                    : {  1000 : TriggerRule(rate=0.5, comment='') },
    'HLT_e26_lhvloose_nod0_L1EM20VH_5j15noL1'                    : {  1000 : TriggerRule(rate=0.5, comment='') },
    'HLT_e26_lhvloose_nod0_L1EM20VH_6j15noL1'                    : {  1000 : TriggerRule(rate=0.5, comment='') },

    # 1.5e34
    'HLT_e24_vloose_L1EM20VH'                                    : {  1000 : TriggerRule(rate=0.5, comment='') },
    'HLT_e24_vloose_L1EM20VH_3j20noL1'                           : {  1000 : TriggerRule(rate=0.5, comment='') },
    'HLT_e24_vloose_L1EM20VH_4j20noL1'                           : {  1000 : TriggerRule(rate=0.5, comment='') },
    'HLT_e24_vloose_L1EM20VH_5j15noL1'                           : {  1000 : TriggerRule(rate=0.5, comment='') },
    'HLT_e24_vloose_L1EM20VH_6j15noL1'                           : {  1000 : TriggerRule(rate=0.5, comment='') },
    'HLT_e24_lhvloose_L1EM20VH'                                  : {  1000 : TriggerRule(rate=0.5, comment='') },
    'HLT_e24_lhvloose_L1EM20VH_3j20noL1'                         : {  1000 : TriggerRule(rate=0.5, comment='') },
    'HLT_e24_lhvloose_L1EM20VH_4j20noL1'                         : {  1000 : TriggerRule(rate=0.5, comment='') },
    'HLT_e24_lhvloose_L1EM20VH_5j15noL1'                         : {  1000 : TriggerRule(rate=0.5, comment='') },
    'HLT_e24_lhvloose_L1EM20VH_6j15noL1'                         : {  1000 : TriggerRule(rate=0.5, comment='') },
    'HLT_e24_lhvloose_nod0_L1EM20VH'                             : {  1000 : TriggerRule(rate=0.5, comment='') },
    'HLT_e24_lhvloose_nod0_L1EM20VH_3j20noL1'                    : {  1000 : TriggerRule(rate=0.5, comment='') },
    'HLT_e24_lhvloose_nod0_L1EM20VH_4j20noL1'                    : {  1000 : TriggerRule(rate=0.5, comment='') },
    'HLT_e24_lhvloose_nod0_L1EM20VH_5j15noL1'                    : {  1000 : TriggerRule(rate=0.5, comment='') },
    'HLT_e24_lhvloose_nod0_L1EM20VH_6j15noL1'                    : {  1000 : TriggerRule(rate=0.5, comment='') },

    # 0.3e34
    'HLT_e24_vloose_L1EM18VH'                                    : {  1000 : TriggerRule(rate=0.5, comment='') },
    'HLT_e24_vloose_L1EM18VH_3j20noL1'                           : {  1000 : TriggerRule(rate=0.5, comment='') },
    'HLT_e24_vloose_L1EM18VH_4j20noL1'                           : {  1000 : TriggerRule(rate=0.5, comment='') },
    'HLT_e24_vloose_L1EM18VH_5j15noL1'                           : {  1000 : TriggerRule(rate=0.5, comment='') },
    'HLT_e24_vloose_L1EM18VH_6j15noL1'                           : {  1000 : TriggerRule(rate=0.5, comment='') },
    'HLT_e24_lhvloose_L1EM18VH'                                  : {  1000 : TriggerRule(rate=0.5, comment='') },
    'HLT_e24_lhvloose_L1EM18VH_3j20noL1'                         : {  1000 : TriggerRule(rate=0.5, comment='') },
    'HLT_e24_lhvloose_L1EM18VH_4j20noL1'                         : {  1000 : TriggerRule(rate=0.5, comment='') },
    'HLT_e24_lhvloose_L1EM18VH_5j15noL1'                         : {  1000 : TriggerRule(rate=0.5, comment='') },
    'HLT_e24_lhvloose_L1EM18VH_6j15noL1'                         : {  1000 : TriggerRule(rate=0.5, comment='') },
    'HLT_e24_lhvloose_nod0_L1EM18VH'                             : {  1000 : TriggerRule(rate=0.5, comment='') },
    'HLT_e24_lhvloose_nod0_L1EM18VH_3j20noL1'                    : {  1000 : TriggerRule(rate=0.5, comment='') },
    'HLT_e24_lhvloose_nod0_L1EM18VH_4j20noL1'                    : {  1000 : TriggerRule(rate=0.5, comment='') },
    'HLT_e24_lhvloose_nod0_L1EM18VH_5j15noL1'                    : {  1000 : TriggerRule(rate=0.5, comment='') },
    'HLT_e24_lhvloose_nod0_L1EM18VH_6j15noL1'                    : {  1000 : TriggerRule(rate=0.5, comment='') },

    # ----------------------------------------
    # loose multi-electron (backgrounds)
    'HLT_2e17_loose_L12EM15'                                     : {  1000 : TriggerRule(PS=32, comment='') },
    'HLT_2e17_lhloose_L12EM15'                                   : {  1000 : TriggerRule(PS=32, comment='') },
    
    # ----------------------------------------
    # loose di-photons (backgrounds)
    # PS=10
    'HLT_2g20_loose_L12EM15'                                     : {  1000 : TriggerRule(PS=10, comment='') },
    # initial commissioning
    'HLT_g20_loose_2g15_loose_L12EM13VH'                         : {  1000 : TriggerRule(PS=32, comment='') },

    # kn: rate=0.2 ??
    'HLT_2g10_loose'                                             : {  1000 : TriggerRule(rate=0.2, comment='') },

    # ----------------------------------------
    # etcut (not in the 26 Nov list, giving 0.2 H each for now)
    'HLT_e17_etcut_L1EM15'                                       : {  1000 : TriggerRule(rate=0.2, comment='') },
    # 
    'HLT_g10_etcut'                                              : {  1000 : TriggerRule(rate=0.2, comment='') },

    # ----------------------------------------
    # idperf
    'HLT_e5_loose_idperf'                                        : {  1000 : TriggerRule(rate=0.2, comment='') },
    'HLT_e5_lhloose_idperf'                                      : {  1000 : TriggerRule(rate=0.2, comment='') },
    'HLT_e5_tight_idperf'                                        : {  1000 : TriggerRule(rate=0.2, comment='') },
    'HLT_e5_lhtight_idperf'                                      : {  1000 : TriggerRule(rate=0.2, comment='') },
    'HLT_e10_tight_idperf'                                       : {  1000 : TriggerRule(rate=0.2, comment='') },
    'HLT_e10_lhtight_idperf'                                     : {  1000 : TriggerRule(rate=0.2, comment='') },
    'HLT_e17_etcut_idperf_L1EM15'                                : {  1000 : TriggerRule(rate=0.2, comment='') },
    'HLT_e24_medium_idperf_L1EM20VH'                             : {  1000 : TriggerRule(rate=0.2, comment='') },
    'HLT_e24_lhmedium_idperf_L1EM20VH'                           : {  1000 : TriggerRule(rate=0.2, comment='') },
    #
    'HLT_e5_lhloose_L2Star_idperf'                               : {  1000 : TriggerRule(rate=0.1, comment='') },
    'HLT_e5_loose_L2Star_idperf'                                 : {  1000 : TriggerRule(rate=0.1, comment='') },
    'HLT_e17_etcut_L2Star_idperf_L1EM15'                         : {  1000 : TriggerRule(rate=0.1, comment='') },
    'HLT_e24_medium_L2Star_idperf_L1EM20VH'                      : {  1000 : TriggerRule(rate=0.1, comment='') },
    'HLT_e24_lhmedium_L2Star_idperf_L1EM20VH'                    : {  1000 : TriggerRule(rate=0.1, comment='') },
})

########################################

egamma_Rules={}

RulesList=[egamma_Rules_2e34, egamma_Rules_1e34, egamma_Rules_0p5e34, egamma_Rules_0p3e34, egamma_Rules_supporting]
for Rules in RulesList:
    for newrule in Rules.keys():
        if egamma_Rules.has_key(newrule):
            print 'CRITICAL: duplicated rule cannot be added.',newrule
    egamma_Rules.update(Rules)


#################################################################################################
#
#
# Combined
#
#
#################################################################################################


########################################
# 2e34 

combined_Rules_2e34={}

HLT_list=[
    #
    # e + 2g
    'HLT_e20_medium_2g10_loose', 'HLT_e20_lhmedium_2g10_loose', 'HLT_e20_lhmedium_nod0_2g10_loose',
    'HLT_e20_medium_2g10_medium','HLT_e20_lhmedium_2g10_medium','HLT_e20_lhmedium_nod0_2g10_medium',
    #
    # e + g
    'HLT_e20_medium_g35_loose','HLT_e20_lhmedium_g35_loose','HLT_e20_lhmedium_nod0_g35_loose',
    #
    'HLT_e24_medium_L1EM15VH_g25_medium','HLT_e24_lhmedium_L1EM15VH_g25_medium','HLT_e24_lhmedium_nod0_L1EM15VH_g25_medium',
    #
    # e + mu
    'HLT_e17_loose_mu14','HLT_e17_lhloose_mu14','HLT_e17_lhloose_nod0_mu14',
    #
    'HLT_e7_medium_mu24','HLT_e7_lhmedium_mu24','HLT_e7_lhmedium_nod0_mu24',
    #
    'HLT_e26_medium_L1EM22VHI_mu8noL1','HLT_e26_lhmedium_L1EM22VHI_mu8noL1','HLT_e26_lhmedium_nod0_L1EM22VHI_mu8noL1',
    #
    # e + 2mu
    'HLT_e12_loose_2mu10', 'HLT_e12_lhloose_2mu10', 'HLT_e12_lhloose_nod0_2mu10',
    'HLT_e12_medium_2mu10','HLT_e12_lhmedium_2mu10','HLT_e12_lhmedium_nod0_2mu10',
    #
    # 2e + mu
    'HLT_2e12_loose_mu10', 'HLT_2e12_lhloose_mu10', 'HLT_2e12_lhloose_nod0_mu10',
    'HLT_2e12_medium_mu10','HLT_2e12_lhmedium_mu10','HLT_2e12_lhmedium_nod0_mu10',
    #
    # mu + g
    'HLT_g25_medium_mu24',
    #
    # mu + 2g
    'HLT_2g10_loose_mu20',
    'HLT_2g10_medium_mu20',
    #
    # e + xe
    'HLT_e70_loose_xe70noL1','HLT_e70_lhloose_xe70noL1','HLT_e70_lhloose_nod0_xe70noL1',
    #
    # g + xe (loose)
    'HLT_g70_loose_xe70noL1',
    ]

combined_Rules_2e34.update(dict(map(None,HLT_list,len(HLT_list)*[{
    1000 : TriggerRule(PS=1, comment=''), 
    }])))

########################################
# 1e34 

combined_Rules_1e34={}

HLT_list=[
    # e + xe
    'HLT_e60_loose_xe60noL1','HLT_e60_lhloose_xe60noL1','HLT_e60_lhloose_nod0_xe60noL1',
    # g + xe (loose)
    'HLT_g60_loose_xe60noL1',
    ]
combined_Rules_1e34.update(dict(map(None,HLT_list,len(HLT_list)*[{
     1000 : TriggerRule(PS=1, comment=''), 
    10001 : TriggerRule(PS=-1, comment='Disabled'),
    }])))

HLT_list=['HLT_e24_medium_L1EM20VHI_mu8noL1','HLT_e24_lhmedium_L1EM20VHI_mu8noL1','HLT_e24_lhmedium_nod0_L1EM20VHI_mu8noL1',]
combined_Rules_1e34.update(dict(map(None,HLT_list,len(HLT_list)*[{
                       1000 : TriggerRule(PS=1, comment=''), 
    lumi_disable_l1_em20vhi : TriggerRule(PS=-1, comment='Disabled'),
    }])))

########################################
# other rules

combined_Rules_other={

    # ----------------------------------------
    # jet + xe

    # 1.0e34
    'HLT_j100_xe80'                      : {  1000 : TriggerRule(PS=1, comment=''),
                           lumi_l1topo_enable_j_xe : TriggerRule(rate=0.2, comment='Prescaled') },

    # 1.0e34 as xe80 is un-prescaled until 1.0e34
    'HLT_j80_xe80'                       : {  1000 : TriggerRule(PS=1, comment='Express', ESRate=0.2),
                                             10001 : TriggerRule(rate=0.2, comment='Express', ESValue=1) },

    # ----------------------------------------
    # g + xe (tight)

    # up to 0.5e33 !!
    'HLT_g45_tight_L1EM22VHI_xe45noL1'   : {  1000 : TriggerRule(PS=1, comment=''),
                                              5001 : TriggerRule(PS=-1, comment='Disabled') },
    # 0.5e34
    'HLT_g45_tight_xe45noL1'             : {  1000 : TriggerRule(PS=1, comment=''),
                                              5001 : TriggerRule(PS=-1, comment='Disabled') },
    # 0.3e34
    'HLT_g40_tight_xe40noL1'             : {  1000 : TriggerRule(PS=1, comment=''),
                                              3001 : TriggerRule(PS=-1, comment='Disabled') },
}

########################################

combined_Rules={}

RulesList=[combined_Rules_2e34, combined_Rules_1e34, combined_Rules_other]
for Rules in RulesList:
    for newrule in Rules.keys():
        if combined_Rules.has_key(newrule):
            print 'CRITICAL: duplicated rule cannot be added.',newrule
    combined_Rules.update(Rules)


#################################################################################################
#
#
# VBF
#
#
#################################################################################################


########################################
# 1e34 

vbf_Rules_1e34={}

L1_list=[
    'L1_MU6_J30.0ETA49_2J20.0ETA49',  # 3.8 kHz @ 2e34
    'L1_4J20.0ETA49',  # 2.3 kHz @ 2e34
    ]

vbf_Rules_1e34.update(dict(map(None,L1_list,len(L1_list)*[{
     1000 : TriggerRule(PS=1, comment='L1'), 
    10001 : TriggerRule(PS=-1, comment='Disabled'),
    }])))

HLT_list=[
    'HLT_mu6_2j40_0eta490_invm1000_L1MU6_J30.0ETA49_2J20.0ETA49',
    #
    'HLT_2j55_bmedium_L14J20.0ETA49',
    #
    'HLT_g25_loose_2j40_0eta490_3j25_0eta490_invm700', # 22 VHI
    ]

vbf_Rules_1e34.update(dict(map(None,HLT_list,len(HLT_list)*[{
     1000 : TriggerRule(PS=1, comment=''), 
    10001 : TriggerRule(PS=-1, comment='Disabled'),
    }])))

########################################
# 0.5e34 

vbf_Rules_0p5e34={}

HLT_list=[
    'HLT_mu6_2j40_0eta490_invm800_L1MU6_J30.0ETA49_2J20.0ETA49',
    'HLT_2j55_bloose_L14J20.0ETA49',
    ]
vbf_Rules_0p5e34.update(dict(map(None,HLT_list,len(HLT_list)*[{
    1000 : TriggerRule(PS=1, comment=''), 
    5001 : TriggerRule(PS=-1, comment='Disabled'),
    }])))

HLT_list=['HLT_g25_loose_L1EM20VH_2j40_0eta490_3j25_0eta490_invm700',]
vbf_Rules_0p5e34.update(dict(map(None,HLT_list,len(HLT_list)*[{
                    1000 : TriggerRule(PS=1, comment=''), 
    lumi_l1iso_enable_em : TriggerRule(PS=-1, comment='Disabled'),
    }])))

########################################
# 0.3e34 

vbf_Rules_0p3e34={}

HLT_list=['HLT_mu6_2j40_0eta490_invm600_L1MU6_J30.0ETA49_2J20.0ETA49',]
vbf_Rules_0p3e34.update(dict(map(None,HLT_list,len(HLT_list)*[{
    1000 : TriggerRule(PS=1, comment=''), 
    3001 : TriggerRule(PS=-1, comment='Disabled'),
    }])))

HLT_list=['HLT_g20_loose_L1EM18VH_2j40_0eta490_3j25_0eta490_invm700',]
vbf_Rules_0p3e34.update(dict(map(None,HLT_list,len(HLT_list)*[{
                      1000 : TriggerRule(PS=1, comment=''), 
    lumi_disable_l1_em18vh : TriggerRule(PS=-1, comment='Disabled'),
    }])))

########################################
# 0.15e34 

vbf_Rules_0p15e34={}

HLT_list=['HLT_mu6_2j40_0eta490_invm400_L1MU6_J30.0ETA49_2J20.0ETA49',]
vbf_Rules_0p15e34.update(dict(map(None,HLT_list,len(HLT_list)*[{
    1000 : TriggerRule(PS=1, comment=''), 
    1501 : TriggerRule(PS=-1, comment='Disabled'),
    }])))

########################################
# supporting

HLT_list=['HLT_2j55_bperf_L14J20.0ETA49',]
vbf_Rules_supporting=dict(map(None,HLT_list,len(HLT_list)*[{
    1000 : TriggerRule(rate=0.2, comment=''), 
    }]))

########################################
# other rules

vbf_Rules_supporting.update({
    # ----------------------------------------
    # MET + VBF
    # 1.8 kHz @ 2e34 for XE55
    'HLT_2j40_0eta490_invm250_L1XE55'   : {  1000 : TriggerRule(rate=10, comment='Prescaled') },
})

########################################

vbf_Rules={}

RulesList=[vbf_Rules_1e34, vbf_Rules_0p5e34, vbf_Rules_0p3e34, vbf_Rules_0p15e34, vbf_Rules_supporting] 
for Rules in RulesList:
    for newrule in Rules.keys():
        if vbf_Rules.has_key(newrule):
            print 'CRITICAL: duplicated rule cannot be added.',newrule
    vbf_Rules.update(Rules)


#################################################################################################
#
#
# Dedicated
#
#
#################################################################################################


dedicated_Rules = {

    # ========================================
    # Drell-Yan
    # 1.5 Hz @ Run1
    'HLT_mu6_iloose_mu6_24invm60_noos'                          : {  1000 : TriggerRule(rate=3.0, comment='') },
    'HLT_mu6_iloose_mu6_11invm24_noos'                          : {  1000 : TriggerRule(rate=3.0, comment='') },
    'HLT_mu6_iloose_mu6_11invm60_noos'                          : {  1000 : TriggerRule(rate=3.0, comment='') },
    'HLT_mu4_iloose_mu4_11invm60_noos'                          : {  1000 : TriggerRule(rate=3.0, comment='') },
    'HLT_mu4_iloose_mu4_7invm9_noos'                            : {  1000 : TriggerRule(rate=3.0, comment='') },

    # ========================================
    # razor
    'HLT_j30_xe10_razor170'                                     : {  1000 : TriggerRule(PS=1, comment='') },

    # ========================================
    # LLP
    'HLT_j30_muvtx'                                             : {  1000 : TriggerRule(PS=1, comment='') },
    'HLT_j30_muvtx_noiso'                                       : {  1000 : TriggerRule(PS=1, comment='') },

    'HLT_g15_loose_2mu10_msonly'                                : {  1000 : TriggerRule(PS=1, comment='') },
    # supporting
    'HLT_g15_loose_2mu10_msonly_L1MU4_EMPTY'                    : {  1000 : TriggerRule(PS=1, comment='') },
    'HLT_g15_loose_2mu10_msonly_L1MU4_UNPAIRED_ISO'             : {  1000 : TriggerRule(PS=1, comment='') },

    # supporting
    'HLT_3mu6_msonly_L1MU4_EMPTY'                               : {  1000 : TriggerRule(PS=1, comment='') },
    'HLT_3mu6_msonly_L1MU4_UNPAIRED_ISO'                        : {  1000 : TriggerRule(PS=1, comment='') },

    # 
    'HLT_j30_jes_PS_llp_L1TAU60'                                : {  1000 : TriggerRule(PS=1, comment='') },
    'HLT_j30_jes_PS_llp_L1TAU30'                                : {  1000 : TriggerRule(PS=1, comment=''),
                                                    lumi_disable_l1_tau30 : TriggerRule(PS=-1, comment='Disabled') },
    'HLT_j30_jes_PS_llp_noiso_L1TAU60'                          : {  1000 : TriggerRule(PS=1, comment='') },

    # ========================================
    # tau->3mu (narrow scan)
    'HLT_mu11_2mu4noL1_nscan03_L1MU11_2MU6'                     : {  1000 : TriggerRule(PS=1, comment=''),
                                                 lumi_l1topo_enable_nscan : TriggerRule(rate=0.1, comment='Prescaled') },
    # 0.5e34 ?
    'HLT_mu11_2mu4noL1_nscan03_L1MU10_2MU6'                     : {  1000 : TriggerRule(PS=1, comment=''),
                                                                     5001 : TriggerRule(PS=-1, comment='Disabled') },

    # ========================================
    # Lepton-Jet
    'HLT_mu20_msonly_mu6noL1_msonly_nscan05'                    : {  1000 : TriggerRule(PS=1, comment='') },

    # supporting
    'HLT_mu20_msonly_mu6noL1_msonly_nscan05_L1MU4_EMPTY'        : {  1000 : TriggerRule(PS=1, comment='') },
    'HLT_mu20_msonly_mu6noL1_msonly_nscan05_L1MU4_UNPAIRED_ISO' : {  1000 : TriggerRule(PS=1, comment='') },

    # supporting (background, rerun)
    'HLT_mu20_msonly'                                           : {  1000 : TriggerRule(rate=0.2, comment='', rerun=1) },

    # ========================================
    # LFV tau->emu
    # At the moment taumass gives huge rate (both 1 kHz @ 2e34)  --> put max rate limit
    #'L1_EM7_MU10'                                               : {  1000 : TriggerRule(PS=1, comment=''),
    #                                           lumi_l1topo_enable_tau_lfv : TriggerRule(rate=500, maxRate=500, comment='Prescaled') },
    #'L1_EM15_MU4'                                               : {  1000 : TriggerRule(PS=1, comment=''),
    #                                           lumi_l1topo_enable_tau_lfv : TriggerRule(rate=500, maxRate=500, comment='Prescaled') },
    # 
    #'HLT_g20_etcut_L1EM15_mu4_taumass'                          : {  1000 : TriggerRule(PS=1, comment=''),
    #                                                       lumi_l1topo_enable_tau_lfv : TriggerRule(rate=0.1, comment='Prescaled after L1Topo') },
    #'HLT_g10_etcut_L1EM7_mu10_taumass'                          : {  1000 : TriggerRule(PS=1, comment=''),
    #                                                       lumi_l1topo_enable_tau_lfv : TriggerRule(rate=0.1, comment='Prescaled after L1Topo') },
    #
    'HLT_g20_etcut_L1EM15_mu4_taumass'                          : {  1000 : TriggerRule(rate=10, maxRate=10, comment='') },
    'HLT_g10_etcut_L1EM7_mu10_taumass'                          : {  1000 : TriggerRule(rate=10, maxRate=10, comment='') },

    # ========================================
    # HIP
    'HLT_g0_hiptrt_L1EM22VHI'                                   : {  1000 : TriggerRule(PS=1, comment='') },
    'HLT_g0_hiptrt_L1EM20VHI'                                   : {  1000 : TriggerRule(PS=1, comment=''),
                                                  lumi_disable_l1_em20vhi : TriggerRule(PS=-1, comment='Disabled') },
    'HLT_g0_hiptrt_L1EM20VH'                                    : {  1000 : TriggerRule(PS=1, comment=''),
                                                     lumi_l1iso_enable_em : TriggerRule(PS=-1, comment='Disabled') },
    'HLT_g0_hiptrt_L1EM18VH'                                    : {  1000 : TriggerRule(PS=1, comment=''),
                                                   lumi_disable_l1_em18vh : TriggerRule(PS=-1, comment='Disabled') },
}

#################################################################################################
#
#
# topo rules
#
#
#################################################################################################


########################################

topo_Rules_bphys={

    # ========================================
    # Level-1

    # TOPO DR + 4<M<8 (B, DiMu)
    'L1_BPH-DR-2MU6_BPH-4M8-2MU6'       : {  1000 : TriggerRule(PS=-1, comment=''),
                         lumi_l1topo_enable_bphys : TriggerRule(PS=1, comment='') },
    'L1_BPH-DR-2MU4-BO_BPH-4M8-2MU4-BO' : {  1000 : TriggerRule(PS=-1, comment='') },
    'L1_BPH-DR-2MU4-B_BPH-4M8-2MU4-B'   : {  1000 : TriggerRule(PS=-1, comment='') },
    'L1_BPH-DR-2MU4_BPH-4M8-2MU4'       : {  1000 : TriggerRule(PS=-1, comment='') },

    # TOPO DR + M>2 (J/Psi, B, Dimu)
    'L1_BPH-DR-2MU6_BPH-2M-2MU6'        : {  1000 : TriggerRule(PS=-1, comment=''),
                         lumi_l1topo_enable_bphys : TriggerRule(PS=1, comment='') },
    'L1_BPH-DR-2MU4-BO_BPH-2M-2MU4-BO'  : {  1000 : TriggerRule(PS=-1, comment='') },
    'L1_BPH-DR-2MU4-B_BPH-2M-2MU4-B'    : {  1000 : TriggerRule(PS=-1, comment='') },
    'L1_BPH-DR-2MU4_BPH-2M-2MU4'        : {  1000 : TriggerRule(PS=-1, comment='') },

    # TOPO M>2 (Upsilon)
    'L1_BPH-2M-2MU6'                    : {  1000 : TriggerRule(PS=-1, comment=''),
                         lumi_l1topo_enable_bphys : TriggerRule(PS=1, comment='') },
    'L1_BPH-2M-2MU4-BO'                 : {  1000 : TriggerRule(PS=-1, comment='') },
    'L1_BPH-2M-2MU4-B'                  : {  1000 : TriggerRule(PS=-1, comment='') },
    'L1_BPH-2M-2MU4'                    : {  1000 : TriggerRule(PS=-1, comment='') },

    # ========================================
    # Primaries

    # ----------------------------------------
    # TOPO JPsi
    'HLT_2mu6_bJpsimumu_L1BPH-DR-2MU6-BPH-2M-2MU6'                   : {  1000 : TriggerRule(PS=-1, comment=''),
                                                      lumi_l1topo_enable_bphys : TriggerRule(PS=1, comment='') },
    'HLT_2mu4_bJpsimumu_L1BPH-DR-2MU4-BO-BPH-2M-2MU4-BO'             : {  1000 : TriggerRule(PS=-1, comment='') },
    'HLT_2mu4_bJpsimumu_L1BPH-DR-2MU4-B-BPH-2M-2MU4-B'               : {  1000 : TriggerRule(PS=-1, comment='') },
    'HLT_2mu4_bJpsimumu_L1BPH-DR-2MU4-BPH-2M-2MU4'                   : {  1000 : TriggerRule(PS=-1, comment='') },

    # ----------------------------------------
    # TOPO Upsilon
    'HLT_2mu6_bUpsimumu_L1BPH-2M-2MU6'                               : {  1000 : TriggerRule(PS=-1, comment=''),
                                                      lumi_l1topo_enable_bphys : TriggerRule(PS=1, comment='') },
    'HLT_mu6_mu4_bUpsimumu_L1BPH-2M-MU6MU4-BO'                       : {  1000 : TriggerRule(PS=-1, comment='') },
    'HLT_mu6_mu4_bUpsimumu_L1BPH-2M-MU6MU4'                          : {  1000 : TriggerRule(PS=-1, comment='') },
    'HLT_2mu4_bUpsimumu_L1BPH-2M-2MU4-BO'                            : {  1000 : TriggerRule(PS=-1, comment='') },
    'HLT_2mu4_bUpsimumu_L1BPH-2M-2MU4-B'                             : {  1000 : TriggerRule(PS=-1, comment='') },
    'HLT_2mu4_bUpsimumu_L1BPH-2M-2MU4'                               : {  1000 : TriggerRule(PS=-1, comment='') },

    # ----------------------------------------
    # TOPO Bmumu, 4 < M < 8
    'HLT_2mu6_bBmumu_L1BPH-DR-2MU6-BPH-4M8-2MU6'                     : {  1000 : TriggerRule(PS=-1, comment=''),
                                                      lumi_l1topo_enable_bphys : TriggerRule(PS=1, comment='') },
    'HLT_2mu4_bBmumu_L1BPH-DR-2MU4-BO-BPH-4M8-2MU4-BO'               : {  1000 : TriggerRule(PS=-1, comment='') },
    'HLT_2mu4_bBmumu_L1BPH-DR-2MU4-B-BPH-4M8-2MU4-B'                 : {  1000 : TriggerRule(PS=-1, comment='') },
    'HLT_2mu4_bBmumu_L1BPH-DR-2MU4-BPH-4M8-2MU4'                     : {  1000 : TriggerRule(PS=-1, comment='') },
    
    # ----------------------------------------
    # TOPO Bmumux, 4 < M < 8
    'HLT_2mu6_bBmumux_BcmumuDsloose_L1BPH-DR-2MU6-BPH-4M8-2MU6'      : {  1000 : TriggerRule(PS=-1, comment=''),
                                                      lumi_l1topo_enable_bphys : TriggerRule(PS=1, comment='') },
    'HLT_2mu4_bBmumux_BcmumuDsloose_L1BPH-DR-2MU4-BO-BPH-4M8-2MU4-BO': {  1000 : TriggerRule(PS=-1, comment='') },
    'HLT_2mu4_bBmumux_BcmumuDsloose_L1BPH-DR-2MU4-B-BPH-4M8-2MU4-B'  : {  1000 : TriggerRule(PS=-1, comment='') },
    'HLT_2mu4_bBmumux_BcmumuDsloose_L1BPH-DR-2MU4-BPH-4M8-2MU4'      : {  1000 : TriggerRule(PS=-1, comment='') },
    
    # ----------------------------------------
    # TOPO Bmumuxv2, 4 < M < 8
    'HLT_2mu6_bBmumuxv2_L1BPH-DR-2MU6-BPH-4M8-2MU6'                  : {  1000 : TriggerRule(PS=-1, comment=''),
                                                      lumi_l1topo_enable_bphys : TriggerRule(PS=1, comment='') },
    'HLT_2mu4_bBmumuxv2_L1BPH-DR-2MU4-BO-BPH-4M8-2MU4-BO'            : {  1000 : TriggerRule(PS=-1, comment='') },
    'HLT_2mu4_bBmumuxv2_L1BPH-DR-2MU4-B-BPH-4M8-2MU4-B'              : {  1000 : TriggerRule(PS=-1, comment='') },
    'HLT_2mu4_bBmumuxv2_L1BPH-DR-2MU4-BPH-4M8-2MU4'                  : {  1000 : TriggerRule(PS=-1, comment='') },

    # ----------------------------------------
    # TOPO Bmumu, 2 < M
    'HLT_2mu6_bBmumu_L1BPH-DR-2MU6-BPH-2M-2MU6'                      : {  1000 : TriggerRule(PS=-1, comment=''),
                                                      lumi_l1topo_enable_bphys : TriggerRule(PS=1, comment='') },
    'HLT_2mu4_bBmumu_L1BPH-DR-2MU4-BO-BPH-2M-2MU4-BO'                : {  1000 : TriggerRule(PS=-1, comment='') },
    'HLT_2mu4_bBmumu_L1BPH-DR-2MU4-B-BPH-2M-2MU4-B'                  : {  1000 : TriggerRule(PS=-1, comment='') },
    'HLT_2mu4_bBmumu_L1BPH-DR-2MU4-BPH-2M-2MU4'                      : {  1000 : TriggerRule(PS=-1, comment='') },
    
    # ----------------------------------------
    # TOPO Bmumux, 2 < M
    'HLT_2mu6_bBmumux_BcmumuDsloose_L1BPH-DR-2MU6-BPH-2M-2MU6'       : {  1000 : TriggerRule(PS=-1, comment=''),
                                                      lumi_l1topo_enable_bphys : TriggerRule(PS=1, comment='') },
    'HLT_2mu4_bBmumux_BcmumuDsloose_L1BPH-DR-2MU4-BO-BPH-2M-2MU4-BO' : {  1000 : TriggerRule(PS=-1, comment='') },
    'HLT_2mu4_bBmumux_BcmumuDsloose_L1BPH-DR-2MU4-B-BPH-2M-2MU4-B'   : {  1000 : TriggerRule(PS=-1, comment='') },
    'HLT_2mu4_bBmumux_BcmumuDsloose_L1BPH-DR-2MU4-BPH-2M-2MU4'       : {  1000 : TriggerRule(PS=-1, comment='') },

    # ----------------------------------------
    # TOPO Bmumuxv2, 2 < M
    'HLT_2mu6_bBmumuxv2_L1BPH-DR-2MU6-BPH-2M-2MU6'                   : {  1000 : TriggerRule(PS=-1, comment=''),
                                                      lumi_l1topo_enable_bphys : TriggerRule(PS=1, comment='') },
    'HLT_2mu4_bBmumuxv2_L1BPH-DR-2MU4-BO-BPH-2M-2MU4-BO'             : {  1000 : TriggerRule(PS=-1, comment='') },
    'HLT_2mu4_bBmumuxv2_L1BPH-DR-2MU4-B-BPH-2M-2MU4-B'               : {  1000 : TriggerRule(PS=-1, comment='') },
    'HLT_2mu4_bBmumuxv2_L1BPH-DR-2MU4-BPH-2M-2MU4'                   : {  1000 : TriggerRule(PS=-1, comment='') },


    # ========================================
    # Support triggers
    
    # ----------------------------------------
    # Dimu, no Vtx, no OS

    'HLT_2mu6_bDimu_novtx_noos_L1BPH-2M-2MU6'                    : {  1000 : TriggerRule(PS=-1, comment=''),
                                                  lumi_l1topo_enable_bphys : TriggerRule(rate=0.3, comment='') },
    'HLT_2mu6_bDimu_novtx_noos_L1BPH-4M8-2MU6'                   : {  1000 : TriggerRule(PS=-1, comment=''),
                                                  lumi_l1topo_enable_bphys : TriggerRule(rate=0.3, comment='') },
    'HLT_2mu6_bDimu_novtx_noos_L1BPH-DR-2MU6'                    : {  1000 : TriggerRule(PS=-1, comment=''),
                                                  lumi_l1topo_enable_bphys : TriggerRule(rate=0.3, comment='') },
    'HLT_2mu6_bDimu_novtx_noos_L1BPH-DR-2MU6-BPH-2M-2MU6'        : {  1000 : TriggerRule(PS=-1, comment=''),
                                                  lumi_l1topo_enable_bphys : TriggerRule(rate=0.3, comment='') },
    'HLT_2mu6_bDimu_novtx_noos_L1BPH-DR-2MU6-BPH-4M8-2MU6'       : {  1000 : TriggerRule(PS=-1, comment=''),
                                                  lumi_l1topo_enable_bphys : TriggerRule(rate=0.3, comment='') },

    # 
    'HLT_mu6_mu4_bDimu_novtx_noos_L1BPH-2M-MU6MU4-B'             : {  1000 : TriggerRule(PS=-1, comment='') },
    'HLT_mu6_mu4_bDimu_novtx_noos_L1BPH-4M8-MU6MU4-B'            : {  1000 : TriggerRule(PS=-1, comment='') },
    'HLT_mu6_mu4_bDimu_novtx_noos_L1BPH-DR-MU6MU4-B'             : {  1000 : TriggerRule(PS=-1, comment='') },
    'HLT_mu6_mu4_bDimu_novtx_noos_L1BPH-2M-MU6MU4-BO'            : {  1000 : TriggerRule(PS=-1, comment='') },
    'HLT_mu6_mu4_bDimu_novtx_noos_L1BPH-4M8-MU6MU4-BO'           : {  1000 : TriggerRule(PS=-1, comment='') },
    'HLT_mu6_mu4_bDimu_novtx_noos_L1BPH-DR-MU6MU4-BO'            : {  1000 : TriggerRule(PS=-1, comment='') },
    'HLT_2mu4_bDimu_novtx_noos_L1BPH-2M-2MU4'                    : {  1000 : TriggerRule(PS=-1, comment='') },
    'HLT_2mu4_bDimu_novtx_noos_L1BPH-4M8-2MU4'                   : {  1000 : TriggerRule(PS=-1, comment='') },
    'HLT_2mu4_bDimu_novtx_noos_L1BPH-DR-2MU4'                    : {  1000 : TriggerRule(PS=-1, comment='') },
    'HLT_2mu4_bDimu_novtx_noos_L1BPH-2M-2MU4-B'                  : {  1000 : TriggerRule(PS=-1, comment='') },
    'HLT_2mu4_bDimu_novtx_noos_L1BPH-4M8-2MU4-B'                 : {  1000 : TriggerRule(PS=-1, comment='') },
    'HLT_2mu4_bDimu_novtx_noos_L1BPH-DR-2MU4-B'                  : {  1000 : TriggerRule(PS=-1, comment='') },
    'HLT_2mu4_bDimu_novtx_noos_L1BPH-2M-2MU4-BO'                 : {  1000 : TriggerRule(PS=-1, comment='') },
    'HLT_2mu4_bDimu_novtx_noos_L1BPH-4M8-2MU4-BO'                : {  1000 : TriggerRule(PS=-1, comment='') },
    'HLT_2mu4_bDimu_novtx_noos_L1BPH-DR-2MU4-BO'                 : {  1000 : TriggerRule(PS=-1, comment='') },
    'HLT_2mu4_bDimu_novtx_noos_L1BPH-DR-2MU4-BPH-2M-2MU4'        : {  1000 : TriggerRule(PS=-1, comment='') },
    'HLT_2mu4_bDimu_novtx_noos_L1BPH-DR-2MU4-BPH-4M8-2MU4'       : {  1000 : TriggerRule(PS=-1, comment='') },
    'HLT_2mu4_bDimu_novtx_noos_L1BPH-DR-2MU4-B-BPH-2M-2MU4-B'    : {  1000 : TriggerRule(PS=-1, comment='') },
    'HLT_2mu4_bDimu_novtx_noos_L1BPH-DR-2MU4-B-BPH-4M8-2MU4-B'   : {  1000 : TriggerRule(PS=-1, comment='') },
    'HLT_2mu4_bDimu_novtx_noos_L1BPH-DR-2MU4-BO-BPH-2M-2MU4-BO'  : {  1000 : TriggerRule(PS=-1, comment='') },
    'HLT_2mu4_bDimu_novtx_noos_L1BPH-DR-2MU4-BO-BPH-4M8-2MU4-BO' : {  1000 : TriggerRule(PS=-1, comment='') },

    # ----------------------------------------
    # Dimu
    'HLT_2mu6_bDimu_L1BPH-2M-2MU6'                               : {  1000 : TriggerRule(PS=-1, comment=''),
                                                  lumi_l1topo_enable_bphys : TriggerRule(rate=0.3, comment='') },
    'HLT_2mu6_bDimu_L1BPH-4M8-2MU6'                              : {  1000 : TriggerRule(PS=-1, comment=''),
                                                  lumi_l1topo_enable_bphys : TriggerRule(rate=0.3, comment='') },
    'HLT_2mu6_bDimu_L1BPH-DR-2MU6'                               : {  1000 : TriggerRule(PS=-1, comment=''),
                                                  lumi_l1topo_enable_bphys : TriggerRule(rate=0.3, comment='') },
    'HLT_2mu6_bDimu_L1BPH-DR-2MU6-BPH-2M-2MU6'                   : {  1000 : TriggerRule(PS=-1, comment=''),
                                                  lumi_l1topo_enable_bphys : TriggerRule(rate=0.3, comment='') },
    'HLT_2mu6_bDimu_L1BPH-DR-2MU6-BPH-4M8-2MU6'                  : {  1000 : TriggerRule(PS=-1, comment=''),
                                                  lumi_l1topo_enable_bphys : TriggerRule(rate=0.3, comment='') },

    #
    'HLT_mu6_mu4_bDimu_L1BPH-2M-MU6MU4'                          : {  1000 : TriggerRule(PS=-1, comment='') },
    'HLT_mu6_mu4_bDimu_L1BPH-4M8-MU6MU4'                         : {  1000 : TriggerRule(PS=-1, comment='') },
    'HLT_mu6_mu4_bDimu_L1BPH-BPH-DR-MU6MU4'                      : {  1000 : TriggerRule(PS=-1, comment='') },
    'HLT_mu6_mu4_bDimu_L1BPH-2M-MU6MU4-BO'                       : {  1000 : TriggerRule(PS=-1, comment='') },
    'HLT_mu6_mu4_bDimu_L1BPH-4M8-MU6MU4-BO'                      : {  1000 : TriggerRule(PS=-1, comment='') },
    'HLT_mu6_mu4_bDimu_L1BPH-BPH-DR-MU6MU4-BO'                   : {  1000 : TriggerRule(PS=-1, comment='') },
    'HLT_2mu4_bDimu_L1BPH-2M-2MU4'                               : {  1000 : TriggerRule(PS=-1, comment='') },
    'HLT_2mu4_bDimu_L1BPH-4M8-2MU4'                              : {  1000 : TriggerRule(PS=-1, comment='') },
    'HLT_2mu4_bDimu_L1BPH-DR-2MU4'                               : {  1000 : TriggerRule(PS=-1, comment='') },
    'HLT_2mu4_bDimu_L1BPH-2M-2MU4-B'                             : {  1000 : TriggerRule(PS=-1, comment='') },
    'HLT_2mu4_bDimu_L1BPH-4M8-2MU4-B'                            : {  1000 : TriggerRule(PS=-1, comment='') },
    'HLT_2mu4_bDimu_L1BPH-DR-2MU4-B'                             : {  1000 : TriggerRule(PS=-1, comment='') },
    'HLT_2mu4_bDimu_L1BPH-2M-2MU4-BO'                            : {  1000 : TriggerRule(PS=-1, comment='') },
    'HLT_2mu4_bDimu_L1BPH-4M8-2MU4-BO'                           : {  1000 : TriggerRule(PS=-1, comment='') },
    'HLT_2mu4_bDimu_L1BPH-DR-2MU4-BO'                            : {  1000 : TriggerRule(PS=-1, comment='') },
    'HLT_2mu4_bDimu_L1BPH-DR-2MU4-BO-BPH-4M8-2MU4-BO'            : {  1000 : TriggerRule(PS=-1, comment='') },
    'HLT_2mu4_bDimu_L1BPH-DR-2MU4-B-BPH-4M8-2MU4-B'              : {  1000 : TriggerRule(PS=-1, comment='') },
    'HLT_2mu4_bDimu_L1BPH-DR-2MU4-BPH-4M8-2MU4'                  : {  1000 : TriggerRule(PS=-1, comment='') },
    'HLT_2mu4_bDimu_L1BPH-DR-2MU4-BO-BPH-2M-2MU4-BO'             : {  1000 : TriggerRule(PS=-1, comment='') },
    'HLT_2mu4_bDimu_L1BPH-DR-2MU4-B-BPH-2M-2MU4-B'               : {  1000 : TriggerRule(PS=-1, comment='') },
    'HLT_2mu4_bDimu_L1BPH-DR-2MU4-BPH-2M-2MU4'                   : {  1000 : TriggerRule(PS=-1, comment='') },

    # ----------------------------------------
    # TOPO validation triggers

    #
    # Jpsi
    'HLT_2mu6_bJpsimumu_L1BPH-2M-2MU6'                           : {  1000 : TriggerRule(PS=-1, comment='') },
    'HLT_2mu6_bJpsimumu_L1BPH-DR-2MU6'                           : {  1000 : TriggerRule(PS=-1, comment='') },
    #
    'HLT_mu6_mu4_bJpsimumu_L1BPH-2M-MU6MU4'                      : {  1000 : TriggerRule(PS=-1, comment='') },
    'HLT_mu6_mu4_bJpsimumu_L1BPH-DR-MU6MU4'                      : {  1000 : TriggerRule(PS=-1, comment='') },
    'HLT_mu6_mu4_bJpsimumu_L1BPH-2M-MU6MU4-BO'                   : {  1000 : TriggerRule(PS=-1, comment='') },
    'HLT_mu6_mu4_bJpsimumu_L1BPH-DR-MU6MU4-BO'                   : {  1000 : TriggerRule(PS=-1, comment='') },
    #
    'HLT_2mu4_bJpsimumu_L1BPH-2M-2MU4'                           : {  1000 : TriggerRule(PS=-1, comment='') },
    'HLT_2mu4_bJpsimumu_L1BPH-DR-2MU4'                           : {  1000 : TriggerRule(PS=-1, comment='') },
    'HLT_2mu4_bJpsimumu_L1BPH-2M-2MU4-B'                         : {  1000 : TriggerRule(PS=-1, comment='') },
    'HLT_2mu4_bJpsimumu_L1BPH-DR-2MU4-B'                         : {  1000 : TriggerRule(PS=-1, comment='') },
    'HLT_2mu4_bJpsimumu_L1BPH-2M-2MU4-BO'                        : {  1000 : TriggerRule(PS=-1, comment='') },
    'HLT_2mu4_bJpsimumu_L1BPH-DR-2MU4-BO'                        : {  1000 : TriggerRule(PS=-1, comment='') },

    #
    # Upsi
    'HLT_mu6_mu4_bUpsimumu_L1BPH-DR-MU6MU4'                      : {  1000 : TriggerRule(PS=-1, comment='') },
    'HLT_mu6_mu4_bUpsimumu_L1BPH-DR-MU6MU4-BO'                   : {  1000 : TriggerRule(PS=-1, comment='') },

    #
    # bBmumu
    'HLT_2mu6_bBmumu_L1BPH-2M-2MU6'                              : {  1000 : TriggerRule(PS=-1, comment='') },
    'HLT_2mu6_bBmumu_L1BPH-4M8-2MU6'                             : {  1000 : TriggerRule(PS=-1, comment='') },
    'HLT_2mu6_bBmumu_L1BPH-DR-2MU6'                              : {  1000 : TriggerRule(PS=-1, comment='') },
    #
    'HLT_mu6_mu4_bBmumu_L1BPH-2M-MU6MU4'                         : {  1000 : TriggerRule(PS=-1, comment='') },
    'HLT_mu6_mu4_bBmumu_L1BPH-4M8-MU6MU4'                        : {  1000 : TriggerRule(PS=-1, comment='') },
    'HLT_mu6_mu4_bBmumu_L1BPH-DR-MU6MU4'                         : {  1000 : TriggerRule(PS=-1, comment='') },
    'HLT_mu6_mu4_bBmumu_L1BPH-2M-MU6MU4-BO'                      : {  1000 : TriggerRule(PS=-1, comment='') },
    'HLT_mu6_mu4_bBmumu_L1BPH-4M8-MU6MU4-BO'                     : {  1000 : TriggerRule(PS=-1, comment='') },
    'HLT_mu6_mu4_bBmumu_L1BPH-DR-MU6MU4-BO'                      : {  1000 : TriggerRule(PS=-1, comment='') },
    #
    'HLT_2mu4_bBmumu_L1BPH-2M-2MU4'                              : {  1000 : TriggerRule(PS=-1, comment='') },
    'HLT_2mu4_bBmumu_L1BPH-4M8-2MU4'                             : {  1000 : TriggerRule(PS=-1, comment='') },
    'HLT_2mu4_bBmumu_L1BPH-DR-2MU4'                              : {  1000 : TriggerRule(PS=-1, comment='') },
    'HLT_2mu4_bBmumu_L1BPH-2M-2MU4-B'                            : {  1000 : TriggerRule(PS=-1, comment='') },
    'HLT_2mu4_bBmumu_L1BPH-4M8-2MU4-B'                           : {  1000 : TriggerRule(PS=-1, comment='') },
    'HLT_2mu4_bBmumu_L1BPH-DR-2MU4-B'                            : {  1000 : TriggerRule(PS=-1, comment='') },
    'HLT_2mu4_bBmumu_L1BPH-2M-2MU4-BO'                           : {  1000 : TriggerRule(PS=-1, comment='') },
    'HLT_2mu4_bBmumu_L1BPH-4M8-2MU4-BO'                          : {  1000 : TriggerRule(PS=-1, comment='') },
    'HLT_2mu4_bBmumu_L1BPH-DR-2MU4-BO'                           : {  1000 : TriggerRule(PS=-1, comment='') },

    #
    # bBMumux
    'HLT_2mu6_bBmumux_BcmumuDsloose_L1BPH-2M-2MU6'               : {  1000 : TriggerRule(PS=-1, comment='') },
    'HLT_2mu6_bBmumux_BcmumuDsloose_L1BPH-4M8-2MU6'              : {  1000 : TriggerRule(PS=-1, comment='') },
    'HLT_2mu6_bBmumux_BcmumuDsloose_L1BPH-DR-2MU6'               : {  1000 : TriggerRule(PS=-1, comment='') },
    #
    'HLT_mu6_mu4_bBmumux_BcmumuDsloose_L1BPH-2M-MU6MU4-B'        : {  1000 : TriggerRule(PS=-1, comment='') },
    'HLT_mu6_mu4_bBmumux_BcmumuDsloose_L1BPH-4M8-MU6MU4-B'       : {  1000 : TriggerRule(PS=-1, comment='') },
    'HLT_mu6_mu4_bBmumux_BcmumuDsloose_L1BPH-DR-MU6MU4-B'        : {  1000 : TriggerRule(PS=-1, comment='') },
    'HLT_mu6_mu4_bBmumux_BcmumuDsloose_L1BPH-2M-MU6MU4-BO'       : {  1000 : TriggerRule(PS=-1, comment='') },
    'HLT_mu6_mu4_bBmumux_BcmumuDsloose_L1BPH-4M8-MU6MU4-BO'      : {  1000 : TriggerRule(PS=-1, comment='') },
    'HLT_mu6_mu4_bBmumux_BcmumuDsloose_L1BPH-DR-MU6MU4-BO'       : {  1000 : TriggerRule(PS=-1, comment='') },
    #
    'HLT_2mu4_bBmumux_BcmumuDsloose_L1BPH-2M-2MU4'               : {  1000 : TriggerRule(PS=-1, comment='') },
    'HLT_2mu4_bBmumux_BcmumuDsloose_L1BPH-4M8-2MU4'              : {  1000 : TriggerRule(PS=-1, comment='') },
    'HLT_2mu4_bBmumux_BcmumuDsloose_L1BPH-DR-2MU4'               : {  1000 : TriggerRule(PS=-1, comment='') },
    'HLT_2mu4_bBmumux_BcmumuDsloose_L1BPH-2M-2MU4-B'             : {  1000 : TriggerRule(PS=-1, comment='') },
    'HLT_2mu4_bBmumux_BcmumuDsloose_L1BPH-4M8-2MU4-B'            : {  1000 : TriggerRule(PS=-1, comment='') },
    'HLT_2mu4_bBmumux_BcmumuDsloose_L1BPH-DR-2MU4-B'             : {  1000 : TriggerRule(PS=-1, comment='') },
    'HLT_2mu4_bBmumux_BcmumuDsloose_L1BPH-2M-2MU4-BO'            : {  1000 : TriggerRule(PS=-1, comment='') },
    'HLT_2mu4_bBmumux_BcmumuDsloose_L1BPH-4M8-2MU4-BO'           : {  1000 : TriggerRule(PS=-1, comment='') },
    'HLT_2mu4_bBmumux_BcmumuDsloose_L1BPH-DR-2MU4-BO'            : {  1000 : TriggerRule(PS=-1, comment='') },

    #
    # bBMumuxv2
    'HLT_2mu6_bBmumuxv2_L1BPH-2M-2MU6'                           : {  1000 : TriggerRule(PS=-1, comment='') },
    'HLT_2mu6_bBmumuxv2_L1BPH-4M8-2MU6'                          : {  1000 : TriggerRule(PS=-1, comment='') },
    'HLT_2mu6_bBmumuxv2_L1BPH-DR-2MU6'                           : {  1000 : TriggerRule(PS=-1, comment='') },
    #
    'HLT_mu6_mu4_bBmumuxv2_L1BPH-2M-MU6MU4'                      : {  1000 : TriggerRule(PS=-1, comment='') },
    'HLT_mu6_mu4_bBmumuxv2_L1BPH-4M8-MU6MU4'                     : {  1000 : TriggerRule(PS=-1, comment='') },
    'HLT_mu6_mu4_bBmumuxv2_L1BPH-DR-MU6MU4'                      : {  1000 : TriggerRule(PS=-1, comment='') },
    'HLT_mu6_mu4_bBmumuxv2_L1BPH-2M-MU6MU4-BO'                   : {  1000 : TriggerRule(PS=-1, comment='') },
    'HLT_mu6_mu4_bBmumuxv2_L1BPH-4M8-MU6MU4-BO'                  : {  1000 : TriggerRule(PS=-1, comment='') },
    'HLT_mu6_mu4_bBmumuxv2_L1BPH-DR-MU6MU4-BO'                   : {  1000 : TriggerRule(PS=-1, comment='') },
    #
    'HLT_2mu4_bBmumuxv2_L1BPH-2M-2MU4'                           : {  1000 : TriggerRule(PS=-1, comment='') },
    'HLT_2mu4_bBmumuxv2_L1BPH-4M8-2MU4'                          : {  1000 : TriggerRule(PS=-1, comment='') },
    'HLT_2mu4_bBmumuxv2_L1BPH-DR-2MU4'                           : {  1000 : TriggerRule(PS=-1, comment='') },
    'HLT_2mu4_bBmumuxv2_L1BPH-2M-2MU4-B'                         : {  1000 : TriggerRule(PS=-1, comment='') },
    'HLT_2mu4_bBmumuxv2_L1BPH-4M8-2MU4-B'                        : {  1000 : TriggerRule(PS=-1, comment='') },
    'HLT_2mu4_bBmumuxv2_L1BPH-DR-2MU4-B'                         : {  1000 : TriggerRule(PS=-1, comment='') },
    'HLT_2mu4_bBmumuxv2_L1BPH-2M-2MU4-BO'                        : {  1000 : TriggerRule(PS=-1, comment='') },
    'HLT_2mu4_bBmumuxv2_L1BPH-4M8-2MU4-BO'                       : {  1000 : TriggerRule(PS=-1, comment='') },
    'HLT_2mu4_bBmumuxv2_L1BPH-DR-2MU4-BO'                        : {  1000 : TriggerRule(PS=-1, comment='') },
}

########################################

topo_Rules_jet={

    # ========================================
    # L1

    # 5 kHz @ 2e34
    'L1_HT190-J15s5.ETA30'                                       : {  1000 : TriggerRule(PS=-1, comment=''),
                                                     lumi_l1topo_enable_ht : TriggerRule(PS=1, comment='') },
    'L1_HT190-J15.ETA30  '                                       : {  1000 : TriggerRule(PS=-1, comment=''),
                                                     lumi_l1topo_enable_ht : TriggerRule(PS=1, comment='') },

    # 12 kHz @ 2e34
    'L1_HT150-J20s5.ETA30'                                       : {  1000 : TriggerRule(PS=-1, comment='') },
    'L1_HT150-J20.ETA30  '                                       : {  1000 : TriggerRule(PS=-1, comment='') },

    # ========================================
    # HLT

    # ----------------------------------------
    # reclustered jet

    # HT150-J20s5.ETA30
    ### L1 SHOULD BE CHANGED TO HT190
    # 21 Hz @ 2e34
    ###'HLT_j460_a10r'                                              : {  1000 : TriggerRule(PS=-1, comment=''),
    ###                                                 lumi_l1topo_enable_ht : TriggerRule(PS=1, comment='') },
    'HLT_j460_a10r'                                              : {  1000 : TriggerRule(PS=-1, comment='') },
    # 0.5e34
    # HT190-J15s5.ETA20
    # 93 Hz @ 2e34
    'HLT_j360_a10r'                                              : {  1000 : TriggerRule(PS=-1, comment='') },

    # ----------------------------------------
    # Fat jet
    # HT190-J15s5.ETA20
    'HLT_j460_a10_sub'                                           : {  1000 : TriggerRule(PS=-1, comment=''),
                                                     lumi_l1topo_enable_ht : TriggerRule(PS=1, comment='') },
    'HLT_j460_a10_nojcalib'                                      : {  1000 : TriggerRule(PS=-1, comment=''),
                                                     lumi_l1topo_enable_ht : TriggerRule(PS=1, comment='') },
    'HLT_j460_a10_lcw_sub'                                       : {  1000 : TriggerRule(PS=-1, comment=''),
                                                     lumi_l1topo_enable_ht : TriggerRule(PS=1, comment='') },
    'HLT_j460_a10_lcw_nojcalib'                                  : {  1000 : TriggerRule(PS=-1, comment=''),
                                                     lumi_l1topo_enable_ht : TriggerRule(PS=1, comment='') },
    # backup
    'HLT_j460_a10_sub_L1HT190-J15.ETA20'                         : {  1000 : TriggerRule(PS=-1, comment=''),
                                                     lumi_l1topo_enable_ht : TriggerRule(PS=1, comment=''),
                                                                     15001 : TriggerRule(PS=-1, comment='Disabled') },
    'HLT_j460_a10_nojcalib_L1HT190-J15.ETA20'                    : {  1000 : TriggerRule(PS=-1, comment=''),
                                                     lumi_l1topo_enable_ht : TriggerRule(PS=1, comment=''),
                                                                     15001 : TriggerRule(PS=-1, comment='Disabled') },
    'HLT_j460_a10_lcw_sub_L1HT190-J15.ETA20'                     : {  1000 : TriggerRule(PS=-1, comment=''),
                                                     lumi_l1topo_enable_ht : TriggerRule(PS=1, comment=''),
                                                                     15001 : TriggerRule(PS=-1, comment='Disabled') },
    'HLT_j460_a10_lcw_nojcalib_L1HT190-J15.ETA20'                : {  1000 : TriggerRule(PS=-1, comment=''),
                                                     lumi_l1topo_enable_ht : TriggerRule(PS=1, comment=''),
                                                                     15001 : TriggerRule(PS=-1, comment='Disabled') },
    # 1.0e34
    # HT150-J20s5.ETA30
    'HLT_j360_a10_sub'                                           : {  1000 : TriggerRule(PS=-1, comment='') },
    'HLT_j360_a10_nojcalib'                                      : {  1000 : TriggerRule(PS=-1, comment='') },
    'HLT_j360_a10_lcw_sub'                                       : {  1000 : TriggerRule(PS=-1, comment='') },
    'HLT_j360_a10_lcw_nojcalib'                                  : {  1000 : TriggerRule(PS=-1, comment='') },

    # 0.5e34
    'HLT_j360_a10_lcw_nojcalib_L1HT150-J20.ETA30'                : {  1000 : TriggerRule(PS=-1, comment='') },
    'HLT_j360_a10_lcw_sub_L1HT150-J20.ETA30'                     : {  1000 : TriggerRule(PS=-1, comment='') },
    'HLT_j360_a10_nojcalib_L1HT150-J20.ETA30'                    : {  1000 : TriggerRule(PS=-1, comment='') },
    'HLT_j360_a10_sub_L1HT150-J20.ETA30'                         : {  1000 : TriggerRule(PS=-1, comment='') },

    # ----------------------------------------
    # HT
    'HLT_ht1000'                                                 : {  1000 : TriggerRule(PS=-1, comment=''),
                                                     lumi_l1topo_enable_ht : TriggerRule(PS=1, comment='') },
    # 1.5e34
    'HLT_ht1000_L1HT190-J15.ETA20'                               : {  1000 : TriggerRule(PS=-1, comment=''),
                                                     lumi_l1topo_enable_ht : TriggerRule(PS=1, comment=''),
                                                                     15001 : TriggerRule(PS=-1, comment='Disabled') },
    # 0.5e34
    'HLT_ht850'                                                  : {  1000 : TriggerRule(PS=-1, comment='') },
    'HLT_ht850_L1HT190-J15.ETA20'                                : {  1000 : TriggerRule(PS=-1, comment='') },

    # ----------------------------------------
    # Prescaled low HT
    'HLT_ht700'                                                  : {  1000 : TriggerRule(PS=-1, comment=''),
                                                     lumi_l1topo_enable_ht : TriggerRule(rate=1.0, comment='Prescaled') },
    'HLT_ht700_L1HT190-J15.ETA20'                                : {  1000 : TriggerRule(PS=-1, comment=''),
                                                     lumi_l1topo_enable_ht : TriggerRule(rate=1.0, comment='Prescaled') },
    'HLT_ht550'                                                  : {  1000 : TriggerRule(PS=-1, comment=''),
                                                     lumi_l1topo_enable_ht : TriggerRule(rate=1.0, comment='Prescaled') },
    'HLT_ht550_L1HT150-J20.ETA30'                                : {  1000 : TriggerRule(PS=-1, comment=''),
                                                     lumi_l1topo_enable_ht : TriggerRule(rate=1.0, comment='Prescaled') },
    'HLT_ht400'                                                  : {  1000 : TriggerRule(PS=-1, comment=''),
                                                     lumi_l1topo_enable_ht : TriggerRule(rate=1.0, comment='Prescaled') },
    'HLT_ht400_L1HT150-J20.ETA30'                                : {  1000 : TriggerRule(PS=-1, comment=''),
                                                     lumi_l1topo_enable_ht : TriggerRule(rate=1.0, comment='Prescaled') },
}

########################################

topo_Rules_bjet={

    # ========================================
    # L1

    # 1.3 kHz @ 2e34
    'L1_3J15_BTAG-MU4J15'                                        : {  1000 : TriggerRule(PS=-1, comment=''),
                                                   lumi_l1topo_enable_btag : TriggerRule(rate=500, maxRate=500, comment='Prescaled') },
    # 0.7 kHz @ 2e34
    'L1_3J20_BTAG-MU4J20'                                        : {  1000 : TriggerRule(PS=-1, comment=''),
                                                   lumi_l1topo_enable_btag : TriggerRule(rate=500, maxRate=500, comment='Prescaled') },
    # 0.7 kHz @ 2e34
    'L1_3J15_BTAG-MU4J30'                                        : {  1000 : TriggerRule(PS=-1, comment=''),
                                                   lumi_l1topo_enable_btag : TriggerRule(rate=500, maxRate=500, comment='Prescaled') },
    # 0.6 kHz @ 2e34
    'L1_3J15_BTAG-MU6J25'                                        : {  1000 : TriggerRule(PS=-1, comment=''),
                                                   lumi_l1topo_enable_btag : TriggerRule(rate=500, maxRate=500, comment='Prescaled') },

    # ========================================
    # HLT

    'HLT_mu4_2j35_dr05_j35_bmedium_L13J15_BTAG-MU4J15'           : {  1000 : TriggerRule(PS=-1, comment=''),
                                                   lumi_l1topo_enable_btag : TriggerRule(rate=0.2, comment='Prescaled') },
    'HLT_mu4_2j35_dr05_j35_bmedium_L13J20_BTAG-MU4J20'           : {  1000 : TriggerRule(PS=-1, comment=''),
                                                   lumi_l1topo_enable_btag : TriggerRule(rate=0.2, comment='Prescaled') },
    #
    'HLT_mu4_2j40_dr05_j40_bloose_L13J15_BTAG-MU4J15'            : {  1000 : TriggerRule(PS=-1, comment=''),
                                                   lumi_l1topo_enable_btag : TriggerRule(rate=0.2, comment='Prescaled') },
    'HLT_mu4_2j40_dr05_j40_bloose_L13J20_BTAG-MU4J20'            : {  1000 : TriggerRule(PS=-1, comment=''),
                                                   lumi_l1topo_enable_btag : TriggerRule(rate=0.2, comment='Prescaled') },
    #
    'HLT_mu4_3j30_dr05_j30_bmedium_L13J15_BTAG-MU4J15'           : {  1000 : TriggerRule(PS=-1, comment=''),
                                                   lumi_l1topo_enable_btag : TriggerRule(rate=0.2, comment='Prescaled') },
    'HLT_mu4_3j30_dr05_j30_bmedium_L13J20_BTAG-MU4J20'           : {  1000 : TriggerRule(PS=-1, comment=''),
                                                   lumi_l1topo_enable_btag : TriggerRule(rate=0.2, comment='Prescaled') },
    #
    'HLT_mu4_3j35_dr05_j35_bloose_L13J15_BTAG-MU4J15'            : {  1000 : TriggerRule(PS=-1, comment=''),
                                                   lumi_l1topo_enable_btag : TriggerRule(rate=0.2, comment='Prescaled') },
    'HLT_mu4_3j35_dr05_j35_bloose_L13J20_BTAG-MU4J20'            : {  1000 : TriggerRule(PS=-1, comment=''),
                                                   lumi_l1topo_enable_btag : TriggerRule(rate=0.2, comment='Prescaled') },
    #
    'HLT_mu4_3j45_dr05_L13J15_BTAG-MU4J15'                       : {  1000 : TriggerRule(PS=-1, comment=''),
                                                   lumi_l1topo_enable_btag : TriggerRule(rate=0.2, comment='Prescaled') },
    'HLT_mu4_3j45_dr05_L13J20_BTAG-MU4J20'                       : {  1000 : TriggerRule(PS=-1, comment=''),
                                                   lumi_l1topo_enable_btag : TriggerRule(rate=0.2, comment='Prescaled') },
    #
    'HLT_mu4_4j40_dr05_L13J15_BTAG-MU4J15'                       : {  1000 : TriggerRule(PS=-1, comment=''),
                                                   lumi_l1topo_enable_btag : TriggerRule(rate=0.2, comment='Prescaled') },
    'HLT_mu4_4j40_dr05_L13J20_BTAG-MU4J20'                       : {  1000 : TriggerRule(PS=-1, comment=''),
                                                   lumi_l1topo_enable_btag : TriggerRule(rate=0.2, comment='Prescaled') },
    #
    'HLT_mu4_j60_dr05_2j35_L13J15_BTAG-MU4J30'                   : {  1000 : TriggerRule(PS=-1, comment=''),
                                                   lumi_l1topo_enable_btag : TriggerRule(rate=0.2, comment='Prescaled') },
    'HLT_mu4_j70_dr05_L1BTAG-MU4J30'                             : {  1000 : TriggerRule(PS=-1, comment=''),
                                                   lumi_l1topo_enable_btag : TriggerRule(rate=0.2, comment='Prescaled') },
    #
    'HLT_mu6_j50_dr05_2j35_L13J15_BTAG-MU6J25'                   : {  1000 : TriggerRule(PS=-1, comment=''),
                                                   lumi_l1topo_enable_btag : TriggerRule(rate=0.2, comment='Prescaled') },
    'HLT_mu6_j60_dr05_L1BTAG-MU6J25'                             : {  1000 : TriggerRule(PS=-1, comment=''),
                                                   lumi_l1topo_enable_btag : TriggerRule(rate=0.2, comment='Prescaled') },

    # ----------------------------------------
    # Support triggers
    'HLT_mu4_j35_bperf_dr05_L1BTAG-MU4J15'                       : {  1000 : TriggerRule(PS=-1, comment='') },
    'HLT_mu4_j55_bperf_dr05_L1BTAG-MU4J15'                       : {  1000 : TriggerRule(PS=-1, comment='') },
    'HLT_mu6_j85_bperf_dr05_L1BTAG-MU6J20'                       : {  1000 : TriggerRule(PS=-1, comment='') },
    'HLT_mu6_j110_bperf_dr05_L1BTAG-MU6J20'                      : {  1000 : TriggerRule(PS=-1, comment='') },
    'HLT_mu6_j150_bperf_dr05_L1BTAG-MU6J20'                      : {  1000 : TriggerRule(PS=-1, comment='') },
    'HLT_mu6_j175_bperf_dr05_L1BTAG-MU6J20'                      : {  1000 : TriggerRule(PS=-1, comment='') },
    'HLT_mu6_j260_bperf_dr05_L1BTAG-MU6J20'                      : {  1000 : TriggerRule(PS=-1, comment='') },
    'HLT_mu6_j320_bperf_dr05_L1BTAG-MU6J20'                      : {  1000 : TriggerRule(PS=-1, comment='') },
    'HLT_mu6_j400_bperf_dr05_L1BTAG-MU6J20'                      : {  1000 : TriggerRule(PS=-1, comment='') },
}

########################################

topo_Rules_tau={

    # ========================================
    # di-tau
    # TOPO: DR, IT, J
    'HLT_tau35_medium1_tracktwo_tau25_medium1_tracktwo_L1DR-TAU20ITAU12I-J25'   : {  1000 : TriggerRule(PS=-1, comment=''),
                                                               lumi_l1topo_enable_tau_tau : TriggerRule(PS=1, comment='') },
    # 1.5e34
    # TOPO: DR, IT
    'HLT_tau35_medium1_tracktwo_tau25_medium1_tracktwo_L1DR-TAU20ITAU12I'       : {  1000 : TriggerRule(PS=-1, comment=''),
                                                               lumi_l1topo_enable_tau_tau : TriggerRule(PS=1, comment=''),
                                                                                    15001 : TriggerRule(PS=-1, comment='Disabled') },
    # backup (DR->BOX)
    'HLT_tau35_medium1_tracktwo_tau25_medium1_tracktwo_L1BOX-TAU20ITAU12I'      : {  1000 : TriggerRule(PS=-1, comment=''),
                                                               lumi_l1topo_enable_tau_tau : TriggerRule(PS=1, comment=''),
                                                                                    15001 : TriggerRule(PS=-1, comment='Disabled') },
    # 1.5e34
    # TOPO: IT, J
    'HLT_tau35_medium1_tracktwo_tau25_medium1_tracktwo_L1TAU20ITAU12I-J25'      : {  1000 : TriggerRule(PS=-1, comment=''),
                                                               lumi_l1topo_enable_tau_tau : TriggerRule(PS=1, comment=''),
                                                                                    15001 : TriggerRule(PS=-1, comment='Disabled') },

    # ========================================
    # tau + electron + xe

    # TOPO (DISAMB)
    'HLT_e17_medium_tau25_medium1_tracktwo_xe50_L1XE35_EM15-TAU12I'            : {  1000 : TriggerRule(PS=-1, comment=''),
                                                                lumi_l1topo_enable_tau_e : TriggerRule(PS=1, comment='') },
    'HLT_e17_lhmedium_tau25_medium1_tracktwo_xe50_L1XE35_EM15-TAU12I'          : {  1000 : TriggerRule(PS=-1, comment=''),
                                                                lumi_l1topo_enable_tau_e : TriggerRule(PS=1, comment='') },
    'HLT_e17_lhmedium_nod0_tau25_medium1_tracktwo_xe50_L1XE35_EM15-TAU12I'     : {  1000 : TriggerRule(PS=-1, comment=''),
                                                                lumi_l1topo_enable_tau_e : TriggerRule(PS=1, comment='') },
    # backup
    'HLT_e17_medium_tau25_medium1_tracktwo_xe50_L1XE40_EM15-TAU12I'            : {  1000 : TriggerRule(PS=-1, comment=''),
                                                                lumi_l1topo_enable_tau_e : TriggerRule(PS=1, comment='') },
    'HLT_e17_lhmedium_tau25_medium1_tracktwo_xe50_L1XE40_EM15-TAU12I'          : {  1000 : TriggerRule(PS=-1, comment=''),
                                                                lumi_l1topo_enable_tau_e : TriggerRule(PS=1, comment='') },
    'HLT_e17_lhmedium_nod0_tau25_medium1_tracktwo_xe50_L1XE40_EM15-TAU12I'     : {  1000 : TriggerRule(PS=-1, comment=''),
                                                                lumi_l1topo_enable_tau_e : TriggerRule(PS=1, comment='') },

    # ========================================
    # tau + electron: low threshold with jet, dR

    # TOPO(DR,DISAMB)
    'HLT_e17_medium_iloose_tau25_medium1_tracktwo_L1DR-EM15TAU12I-J25'        : {  1000 : TriggerRule(PS=-1, comment=''),
                                                               lumi_l1topo_enable_tau_e : TriggerRule(PS=1, comment='') },
    'HLT_e17_lhmedium_iloose_tau25_medium1_tracktwo_L1DR-EM15TAU12I-J25'      : {  1000 : TriggerRule(PS=-1, comment=''),
                                                               lumi_l1topo_enable_tau_e : TriggerRule(PS=1, comment='') },
    'HLT_e17_lhmedium_nod0_iloose_tau25_medium1_tracktwo_L1DR-EM15TAU12I-J25' : {  1000 : TriggerRule(PS=-1, comment=''),
                                                               lumi_l1topo_enable_tau_e : TriggerRule(PS=1, comment='') },

    # 1.5e34
    'HLT_e17_medium_tau25_medium1_tracktwo_L1DR-EM15TAU12I-J25'          : {  1000 : TriggerRule(PS=-1, comment=''),
                                                          lumi_l1topo_enable_tau_e : TriggerRule(PS=1, comment=''),
                                                                             15001 : TriggerRule(PS=-1, comment='Disabled') },
    'HLT_e17_lhmedium_tau25_medium1_tracktwo_L1DR-EM15TAU12I-J25'        : {  1000 : TriggerRule(PS=-1, comment=''),
                                                          lumi_l1topo_enable_tau_e : TriggerRule(PS=1, comment=''),
                                                                             15001 : TriggerRule(PS=-1, comment='Disabled') },
    'HLT_e17_lhmedium_nod0_tau25_medium1_tracktwo_L1DR-EM15TAU12I-J25'   : {  1000 : TriggerRule(PS=-1, comment=''),
                                                          lumi_l1topo_enable_tau_e : TriggerRule(PS=1, comment=''),
                                                                             15001 : TriggerRule(PS=-1, comment='Disabled') },

    # ========================================
    # tau + electron: high threshold w/o jet

    # TOPO(DISAMB)
    'HLT_e17_medium_tau80_medium1_tracktwo_L1EM15-TAU40'                 : {  1000 : TriggerRule(PS=-1, comment=''),
                                                          lumi_l1topo_enable_tau_e : TriggerRule(PS=1, comment='') },
    'HLT_e17_lhmedium_tau80_medium1_tracktwo_L1EM15-TAU40'               : {  1000 : TriggerRule(PS=-1, comment=''),
                                                          lumi_l1topo_enable_tau_e : TriggerRule(PS=1, comment='') },
    'HLT_e17_lhmedium_nod0_tau80_medium1_tracktwo_L1EM15-TAU40'          : {  1000 : TriggerRule(PS=-1, comment=''),
                                                          lumi_l1topo_enable_tau_e : TriggerRule(PS=1, comment='') },

    # ========================================
    # tau + muon: low threshold with jet, dR

    # TOPO (DR, I, J)
    'HLT_mu14_tau25_medium1_tracktwo_L1DR-MU10TAU12I_TAU12-J25'          : {  1000 : TriggerRule(PS=-1, comment=''),
                                                         lumi_l1topo_enable_tau_mu : TriggerRule(PS=1, comment='') },

    # ========================================
    # L1 TOPO Comissioning triggers (disabled in rules)

    'HLT_tau35_medium1_tracktwo_L1TAU20_tau25_medium1_tracktwo_L1TAU12_L1TAU20_2TAU12' : {  1000 : TriggerRule(PS=-1, comment='') },

    'HLT_e17_medium_tau25_medium1_tracktwo_L1EM15TAU12I-J25'                           : {  1000 : TriggerRule(PS=-1, comment='') },
    'HLT_e17_lhmedium_tau25_medium1_tracktwo_L1EM15TAU12I-J25'                         : {  1000 : TriggerRule(PS=-1, comment='') },
    'HLT_e17_lhmedium_nod0_tau25_medium1_tracktwo_L1EM15TAU12I-J25'                    : {  1000 : TriggerRule(PS=-1, comment='') },

    'HLT_e17_medium_tau25_medium1_tracktwo_L1EM15-TAU12I'                              : {  1000 : TriggerRule(PS=-1, comment='') },
    'HLT_e17_lhmedium_tau25_medium1_tracktwo_L1EM15-TAU12I'                            : {  1000 : TriggerRule(PS=-1, comment='') },
    'HLT_e17_lhmedium_nod0_tau25_medium1_tracktwo_L1EM15-TAU12I'                       : {  1000 : TriggerRule(PS=-1, comment='') },

    'HLT_mu14_tau25_medium1_tracktwo_L1MU10_TAU12I-J25'                                : {  1000 : TriggerRule(PS=-1, comment='') },
    'HLT_mu14_tau25_medium1_tracktwo_L1DR-MU10TAU12I'                                  : {  1000 : TriggerRule(PS=-1, comment='') },
 }

########################################

topo_Rules_egamma={

    # ========================================
    # L1

    # 8 kHz @ 2e34
    'L1_JPSI-1M5-EM12'                                           : {  1000 : TriggerRule(PS=-1, comment=''),
                                                  lumi_l1topo_enable_tap_e : TriggerRule(rate=1000, maxRate=1000, comment='') },
    # 41 kHz @ 2e34
    'L1_JPSI-1M5-EM7'                                            : {  1000 : TriggerRule(PS=-1, comment=''),
                                                  lumi_l1topo_enable_tap_e : TriggerRule(rate=1000, maxRate=1000, comment='') },
    # 2000 kHz @ 2e34
    'L1_JPSI-1M5'                                                : {  1000 : TriggerRule(PS=-1, comment=''),
                                                  lumi_l1topo_enable_tap_e : TriggerRule(rate=1000, maxRate=1000, comment='') },

    # 70 kHz @ 2e34
    'L1_EM10_W-MT25_W-15DPHI-JXE-0_W-15DPHI-EMXE'                : {  1000 : TriggerRule(PS=-1, comment=''),
                                                  lumi_l1topo_enable_tap_e : TriggerRule(rate=1000, maxRate=1000, comment='') },
    # 57 kHz @ 2e34
    'L1_EM10_W-MT25_W-15DPHI-JXE-0_W-15DPHI-EMXE_XS20'           : {  1000 : TriggerRule(PS=-1, comment=''),
                                                  lumi_l1topo_enable_tap_e : TriggerRule(rate=1000, maxRate=1000, comment='') },
    # 18 kHz @ 2e34
    'L1_EM15_W-MT35_W-05DPHI-JXE-0_W-15DPHI-EMXE'                : {  1000 : TriggerRule(PS=-1, comment=''),
                                                  lumi_l1topo_enable_tap_e : TriggerRule(rate=1000, maxRate=1000, comment='') },

    # 10 kHz @ 2e34
    'L1_EM15_W-MT35_W-05DPHI-JXE-0_W-15DPHI-EMXE_XS30'           : {  1000 : TriggerRule(PS=-1, comment=''),
                                                  lumi_l1topo_enable_tap_e : TriggerRule(rate=1000, maxRate=1000, comment='') },

    # supporting
    'L1_EM15_W-MT35'                                             : {  1000 : TriggerRule(PS=-1, comment=''),
                                                  lumi_l1topo_enable_tap_e : TriggerRule(rate=100, maxRate=100, comment='') },
    'L1_EM10_W-MT25'                                             : {  1000 : TriggerRule(PS=-1, comment=''),
                                                  lumi_l1topo_enable_tap_e : TriggerRule(rate=100, maxRate=100, comment='') },
    'L1_EM10_W-MT30'                                             : {  1000 : TriggerRule(PS=-1, comment=''),
                                                  lumi_l1topo_enable_tap_e : TriggerRule(rate=100, maxRate=100, comment='') },
    'L1_W-NOMATCH'                                               : {  1000 : TriggerRule(PS=-1, comment=''),
                                                  lumi_l1topo_enable_tap_e : TriggerRule(rate=100, maxRate=100, comment='') },
    'L1_W-NOMATCH_W-05RO-XEEMHT'                                 : {  1000 : TriggerRule(PS=-1, comment=''),
                                                  lumi_l1topo_enable_tap_e : TriggerRule(rate=100, maxRate=100, comment='') },
    'L1_W-05DPHI-EMXE-1'                                         : {  1000 : TriggerRule(PS=-1, comment=''),
                                                  lumi_l1topo_enable_tap_e : TriggerRule(rate=100, maxRate=100, comment='') },
    'L1_W-05DPHI-JXE-0'                                          : {  1000 : TriggerRule(PS=-1, comment=''),
                                                  lumi_l1topo_enable_tap_e : TriggerRule(rate=100, maxRate=100, comment='') },
    'L1_W-05RO-XEHT-0'                                           : {  1000 : TriggerRule(PS=-1, comment=''),
                                                  lumi_l1topo_enable_tap_e : TriggerRule(rate=100, maxRate=100, comment='') },
    'L1_W-10DPHI-EMXE-0'                                         : {  1000 : TriggerRule(PS=-1, comment=''),
                                                  lumi_l1topo_enable_tap_e : TriggerRule(rate=100, maxRate=100, comment='') },
    'L1_W-10DPHI-JXE-0'                                          : {  1000 : TriggerRule(PS=-1, comment=''),
                                                  lumi_l1topo_enable_tap_e : TriggerRule(rate=100, maxRate=100, comment='') },
    'L1_W-15DPHI-EMXE-0'                                         : {  1000 : TriggerRule(PS=-1, comment=''),
                                                  lumi_l1topo_enable_tap_e : TriggerRule(rate=100, maxRate=100, comment='') },
    'L1_W-15DPHI-JXE-0'                                          : {  1000 : TriggerRule(PS=-1, comment=''),
                                                  lumi_l1topo_enable_tap_e : TriggerRule(rate=100, maxRate=100, comment='') },
    'L1_W-250RO2-XEHT-0'                                         : {  1000 : TriggerRule(PS=-1, comment=''),
                                                  lumi_l1topo_enable_tap_e : TriggerRule(rate=100, maxRate=100, comment='') },
    'L1_W-90RO2-XEHT-0'                                          : {  1000 : TriggerRule(PS=-1, comment=''),
                                                  lumi_l1topo_enable_tap_e : TriggerRule(rate=100, maxRate=100, comment='') },
    'L1_W-HT20-JJ15.ETA49'                                       : {  1000 : TriggerRule(PS=-1, comment=''),
                                                  lumi_l1topo_enable_tap_e : TriggerRule(rate=100, maxRate=100, comment='') },

    # ========================================
    # J/Psi T&P
    'HLT_e14_tight_e4_etcut_Jpsiee_L1JPSI-1M5-EM12'              : {  1000 : TriggerRule(PS=-1, comment=''),
                                                  lumi_l1topo_enable_tap_e : TriggerRule(rate=1.0, comment='') },
    'HLT_e14_lhtight_e4_etcut_Jpsiee_L1JPSI-1M5-EM12'            : {  1000 : TriggerRule(PS=-1, comment=''),
                                                  lumi_l1topo_enable_tap_e : TriggerRule(rate=1.0, comment='') },
    'HLT_e14_lhtight_nod0_e4_etcut_Jpsiee_L1JPSI-1M5-EM12'       : {  1000 : TriggerRule(PS=-1, comment=''),
                                                  lumi_l1topo_enable_tap_e : TriggerRule(rate=1.0, comment='') },
    'HLT_e14_etcut_e5_tight_Jpsiee_L1JPSI-1M5-EM12'              : {  1000 : TriggerRule(PS=-1, comment=''),
                                                  lumi_l1topo_enable_tap_e : TriggerRule(rate=1.0, comment='') },
    'HLT_e14_etcut_e5_lhtight_Jpsiee_L1JPSI-1M5-EM12'            : {  1000 : TriggerRule(PS=-1, comment=''),
                                                  lumi_l1topo_enable_tap_e : TriggerRule(rate=1.0, comment='') },
    'HLT_e14_etcut_e5_lhtight_nod0_Jpsiee_L1JPSI-1M5-EM12'       : {  1000 : TriggerRule(PS=-1, comment=''),
                                                  lumi_l1topo_enable_tap_e : TriggerRule(rate=1.0, comment='') },
    
    'HLT_e9_tight_e4_etcut_Jpsiee_L1JPSI-1M5-EM7'                : {  1000 : TriggerRule(PS=-1, comment=''),
                                                  lumi_l1topo_enable_tap_e : TriggerRule(rate=1.0, comment='') },
    'HLT_e9_lhtight_e4_etcut_Jpsiee_L1JPSI-1M5-EM7'              : {  1000 : TriggerRule(PS=-1, comment=''),
                                                  lumi_l1topo_enable_tap_e : TriggerRule(rate=1.0, comment='') },
    'HLT_e9_lhtight_nod0_e4_etcut_Jpsiee_L1JPSI-1M5-EM7'         : {  1000 : TriggerRule(PS=-1, comment=''),
                                                  lumi_l1topo_enable_tap_e : TriggerRule(rate=1.0, comment='') },
    'HLT_e9_etcut_e5_tight_Jpsiee_L1JPSI-1M5-EM7'                : {  1000 : TriggerRule(PS=-1, comment=''),
                                                  lumi_l1topo_enable_tap_e : TriggerRule(rate=1.0, comment='') },
    'HLT_e9_etcut_e5_lhtight_Jpsiee_L1JPSI-1M5-EM7'              : {  1000 : TriggerRule(PS=-1, comment=''),
                                                  lumi_l1topo_enable_tap_e : TriggerRule(rate=1.0, comment='') },
    'HLT_e9_etcut_e5_lhtight_nod0_Jpsiee_L1JPSI-1M5-EM7'         : {  1000 : TriggerRule(PS=-1, comment=''),
                                                  lumi_l1topo_enable_tap_e : TriggerRule(rate=1.0, comment='') },

    'HLT_e5_tight_e4_etcut_Jpsiee_L1JPSI-1M5'                    : {  1000 : TriggerRule(PS=-1, comment=''),
                                                  lumi_l1topo_enable_tap_e : TriggerRule(rate=1.0, comment='') },
    'HLT_e5_lhtight_e4_etcut_Jpsiee_L1JPSI-1M5'                  : {  1000 : TriggerRule(PS=-1, comment=''),
                                                  lumi_l1topo_enable_tap_e : TriggerRule(rate=1.0, comment='') },
    'HLT_e5_lhtight_nod0_e4_etcut_Jpsiee_L1JPSI-1M5'             : {  1000 : TriggerRule(PS=-1, comment=''),
                                                  lumi_l1topo_enable_tap_e : TriggerRule(rate=1.0, comment='') },

    # supporting                                                                      
    'HLT_e5_tight_e4_etcut_L1JPSI-1M5'                           : {  1000 : TriggerRule(PS=-1, comment=''),
                                                  lumi_l1topo_enable_tap_e : TriggerRule(rate=0.1, comment='') },
    'HLT_e5_lhtight_e4_etcut_L1JPSI-1M5'                         : {  1000 : TriggerRule(PS=-1, comment=''),
                                                  lumi_l1topo_enable_tap_e : TriggerRule(rate=0.1, comment='') },
    'HLT_e5_lhtight_nod0_e4_etcut_L1JPSI-1M5'                    : {  1000 : TriggerRule(PS=-1, comment=''),
                                                  lumi_l1topo_enable_tap_e : TriggerRule(rate=0.1, comment='') },

    # ========================================
    # W T&P

    'HLT_e13_etcut_trkcut_L1EM10_W-MT25_W-15DPHI-JXE-0_W-15DPHI-EMXE'            : {  1000 : TriggerRule(PS=-1, comment=''),
                                                                  lumi_l1topo_enable_tap_e : TriggerRule(rate=1.0, comment='') },
    'HLT_e13_etcut_trkcut_xe20_L1EM10_W-MT25_W-15DPHI-JXE-0_W-15DPHI-EMXE_XS20'  : {  1000 : TriggerRule(PS=-1, comment=''),
                                                                  lumi_l1topo_enable_tap_e : TriggerRule(rate=1.0, comment='') },

    'HLT_e18_etcut_trkcut_L1EM15_W-MT35_W-05DPHI-JXE-0_W-15DPHI-EMXE'            : {  1000 : TriggerRule(PS=-1, comment=''),
                                                                  lumi_l1topo_enable_tap_e : TriggerRule(rate=1.0, comment='') },
    'HLT_e18_etcut_trkcut_xe35_L1EM15_W-MT35_W-05DPHI-JXE-0_W-15DPHI-EMXE_XS30'  : {  1000 : TriggerRule(PS=-1, comment=''),
                                                                  lumi_l1topo_enable_tap_e : TriggerRule(rate=1.0, comment='') },

    # supporting
    'HLT_e18_etcut_L1EM15_W-MT35'                                                : {  1000 : TriggerRule(PS=-1, comment=''),
                                                                  lumi_l1topo_enable_tap_e : TriggerRule(rate=0.1, comment='') },
    'HLT_e13_etcut_L1EM10_W-MT25'                                                : {  1000 : TriggerRule(PS=-1, comment=''),
                                                                  lumi_l1topo_enable_tap_e : TriggerRule(rate=0.1, comment='') },
    'HLT_e13_etcut_L1EM10_W-MT30'                                                : {  1000 : TriggerRule(PS=-1, comment=''),
                                                                  lumi_l1topo_enable_tap_e : TriggerRule(rate=0.1, comment='') },
    'HLT_e13_etcut_L1W-NOMATCH'                                                  : {  1000 : TriggerRule(PS=-1, comment=''),
                                                                  lumi_l1topo_enable_tap_e : TriggerRule(rate=0.1, comment='') },
    'HLT_e13_etcut_L1W-NOMATCH_W-05RO-XEEMHT'                                    : {  1000 : TriggerRule(PS=-1, comment=''),
                                                                  lumi_l1topo_enable_tap_e : TriggerRule(rate=0.1, comment='') },
    'HLT_e5_etcut_L1W-05DPHI-EMXE-1'                                             : {  1000 : TriggerRule(PS=-1, comment=''),
                                                                  lumi_l1topo_enable_tap_e : TriggerRule(rate=0.1, comment='') },
    'HLT_e5_etcut_L1W-05DPHI-JXE-0'                                              : {  1000 : TriggerRule(PS=-1, comment=''),
                                                                  lumi_l1topo_enable_tap_e : TriggerRule(rate=0.1, comment='') },
    'HLT_e5_etcut_L1W-05RO-XEHT-0'                                               : {  1000 : TriggerRule(PS=-1, comment=''),
                                                                  lumi_l1topo_enable_tap_e : TriggerRule(rate=0.1, comment='') },
    'HLT_e5_etcut_L1W-10DPHI-EMXE-0'                                             : {  1000 : TriggerRule(PS=-1, comment=''),
                                                                  lumi_l1topo_enable_tap_e : TriggerRule(rate=0.1, comment='') },
    'HLT_e5_etcut_L1W-10DPHI-JXE-0'                                              : {  1000 : TriggerRule(PS=-1, comment=''),
                                                                  lumi_l1topo_enable_tap_e : TriggerRule(rate=0.1, comment='') },
    'HLT_e5_etcut_L1W-15DPHI-EMXE-0'                                             : {  1000 : TriggerRule(PS=-1, comment=''),
                                                                  lumi_l1topo_enable_tap_e : TriggerRule(rate=0.1, comment='') },
    'HLT_e5_etcut_L1W-15DPHI-JXE-0'                                              : {  1000 : TriggerRule(PS=-1, comment=''),
                                                                  lumi_l1topo_enable_tap_e : TriggerRule(rate=0.1, comment='') },
    'HLT_e5_etcut_L1W-250RO2-XEHT-0'                                             : {  1000 : TriggerRule(PS=-1, comment=''),
                                                                  lumi_l1topo_enable_tap_e : TriggerRule(rate=0.1, comment='') },
    'HLT_e5_etcut_L1W-90RO2-XEHT-0'                                              : {  1000 : TriggerRule(PS=-1, comment=''),
                                                                  lumi_l1topo_enable_tap_e : TriggerRule(rate=0.1, comment='') },
    'HLT_e5_etcut_L1W-HT20-JJ15.ETA49'                                           : {  1000 : TriggerRule(PS=-1, comment=''),
                                                                  lumi_l1topo_enable_tap_e : TriggerRule(rate=0.1, comment='') },
}

########################################

topo_Rules_vbf={

    # ========================================
    # muon + VBF
    # MU6_MJJ500 (L1: 80 Hz @ 2e34)
    'HLT_mu6_2j40_0eta490_invm1000'                              : {  1000 : TriggerRule(PS=-1, comment=''),
                                                    lumi_l1topo_enable_vbf : TriggerRule(PS=1, comment=''), },
    # 1.0e34
    # MU6_MJJ400
    'HLT_mu6_2j40_0eta490_invm800'                               : {  1000 : TriggerRule(PS=-1, comment='') },
    # 0.5e34
    # MU6_MJJ300
    'HLT_mu6_2j40_0eta490_invm600'                               : {  1000 : TriggerRule(PS=-1, comment='') },
    # 0.3e34
    # MU6_MJJ200
    'HLT_mu6_2j40_0eta490_invm400'                               : {  1000 : TriggerRule(PS=-1, comment='') },

    # ========================================
    # MET + VBF
    # Topo (XE35-MJJ-200)
    # 4 kHz @ 2e34
    'HLT_2j40_0eta490_invm250'                                   : {  1000 : TriggerRule(PS=-1, comment=''),
                                                    lumi_l1topo_enable_vbf : TriggerRule(rate=10, comment='Prescaled') },

    # ========================================
    # b-jet + VBF

    # J30_2J20_4J20.0ETA49
    # L1: 90 Hz @ 2e34
    'HLT_2j55_bmedium_L1J30_2J20_4J20.0ETA49_MJJ-900'            : {  1000 : TriggerRule(PS=-1, comment=''),
                                                    lumi_l1topo_enable_vbf : TriggerRule(PS=1, comment=''), },
    # 1.0e34
    'HLT_2j55_bmedium_L1J30_2J20_4J20.0ETA49_MJJ-800'            : {  1000 : TriggerRule(PS=-1, comment='') },
    'HLT_2j55_bmedium_L1J30_2J20_4J20.0ETA49_MJJ-700'            : {  1000 : TriggerRule(PS=-1, comment='') },
    # 0.5e34
    'HLT_2j55_bmedium_L1J30_2J20_4J20.0ETA49_MJJ-400'            : {  1000 : TriggerRule(PS=-1, comment='') },

    # supporting
    'HLT_2j55_bloose_L1J30_2J20_4J20.0ETA49_MJJ-900'             : {  1000 : TriggerRule(PS=-1, comment=''),
                                                    lumi_l1topo_enable_vbf : TriggerRule(PS=30, comment=''), },
    'HLT_2j55_bloose_L1J30_2J20_4J20.0ETA49_MJJ-800'             : {  1000 : TriggerRule(PS=-1, comment='') },
    'HLT_2j55_bloose_L1J30_2J20_4J20.0ETA49_MJJ-700'             : {  1000 : TriggerRule(PS=-1, comment='') },
    'HLT_2j55_bloose_L1J30_2J20_4J20.0ETA49_MJJ-400'             : {  1000 : TriggerRule(PS=-1, comment='') },

    # ----------------------------------------
    # 3J20_4J20.0ETA49
    # L1: 60 Hz @ 2e34
    'HLT_2j55_bmedium_L13J20_4J20.0ETA49_MJJ-900'                : {  1000 : TriggerRule(PS=-1, comment=''),
                                                    lumi_l1topo_enable_vbf : TriggerRule(PS=1, comment=''), },
    # 1.0e34
    'HLT_2j55_bmedium_L13J20_4J20.0ETA49_MJJ-800'                : {  1000 : TriggerRule(PS=-1, comment='') },
    'HLT_2j55_bmedium_L13J20_4J20.0ETA49_MJJ-700'                : {  1000 : TriggerRule(PS=-1, comment='') },
    # 0.5e34
    'HLT_2j55_bmedium_L13J20_4J20.0ETA49_MJJ-400'                : {  1000 : TriggerRule(PS=-1, comment='') },

    # supporting
    'HLT_2j55_bloose_L13J20_4J20.0ETA49_MJJ-900'                 : {  1000 : TriggerRule(PS=-1, comment=''),
                                                    lumi_l1topo_enable_vbf : TriggerRule(PS=30, comment=''), },
    'HLT_2j55_bloose_L13J20_4J20.0ETA49_MJJ-800'                 : {  1000 : TriggerRule(PS=-1, comment='') },
    'HLT_2j55_bloose_L13J20_4J20.0ETA49_MJJ-700'                 : {  1000 : TriggerRule(PS=-1, comment='') },
    'HLT_2j55_bloose_L13J20_4J20.0ETA49_MJJ-400'                 : {  1000 : TriggerRule(PS=-1, comment='') },

    # bperf
    'HLT_2j55_bperf_L13J20_4J20.0ETA49_MJJ-900'                  : {  1000 : TriggerRule(PS=-1, comment=''),
                                                    lumi_l1topo_enable_vbf : TriggerRule(rate=0.1, comment=''), },
    'HLT_2j55_bperf_L1J30_2J20_4J20.0ETA49_MJJ-900'              : {  1000 : TriggerRule(PS=-1, comment=''),
                                                    lumi_l1topo_enable_vbf : TriggerRule(rate=0.1, comment=''), },
    'HLT_2j55_bperf_L13J20_4J20.0ETA49_MJJ-400'                  : {  1000 : TriggerRule(PS=-1, comment='') },
    'HLT_2j55_bperf_L13J20_4J20.0ETA49_MJJ-700'                  : {  1000 : TriggerRule(PS=-1, comment='') },
    'HLT_2j55_bperf_L13J20_4J20.0ETA49_MJJ-800'                  : {  1000 : TriggerRule(PS=-1, comment='') },
    'HLT_2j55_bperf_L1J30_2J20_4J20.0ETA49_MJJ-400'              : {  1000 : TriggerRule(PS=-1, comment='') },
    'HLT_2j55_bperf_L1J30_2J20_4J20.0ETA49_MJJ-700'              : {  1000 : TriggerRule(PS=-1, comment='') },
    'HLT_2j55_bperf_L1J30_2J20_4J20.0ETA49_MJJ-800'              : {  1000 : TriggerRule(PS=-1, comment='') },

    # ========================================
    # photon + VBF
    # L1: 900 Hz @ 2e34
    'HLT_g20_loose_2j40_0eta490_3j25_0eta490_L1MJJ-900'          : {  1000 : TriggerRule(PS=-1, comment=''),
                                                    lumi_l1topo_enable_vbf : TriggerRule(PS=1, comment=''), },
    # 1.0e34
    'HLT_g20_loose_2j40_0eta490_3j25_0eta490'                    : {  1000 : TriggerRule(PS=-1, comment='') },
    'HLT_g20_loose_2j40_0eta490_3j25_0eta490_L1MJJ-700'          : {  1000 : TriggerRule(PS=-1, comment='') },
    # 0.5e34
    # MJJ-400
    'HLT_g15_loose_2j40_0eta490_3j25_0eta490'                    : {  1000 : TriggerRule(PS=-1, comment='') },
}

########################################

topo_Rules_combined={

    # ========================================
    # jet + xe (+topo)

    # MCG 2015.05.28
    # J20XE50 and J20s2XE50 selected amongst 4
    # SUSY disappearing prefers J20s2XE50

    'HLT_j80_xe80_dphi1_L1J40_DPHI-J20XE50'       : {  1000 : TriggerRule(PS=-1, comment=''),
                                    lumi_l1topo_enable_j_xe : TriggerRule(PS=1, comment=''), },
    'HLT_j80_xe80_dphi1_L1J40_DPHI-J20s2XE50'     : {  1000 : TriggerRule(PS=-1, comment=''),
                                    lumi_l1topo_enable_j_xe : TriggerRule(PS=1, comment=''), },

    # supporting: no HLT topo
    'HLT_j100_xe80_L1J40_DPHI-J20XE50'            : {  1000 : TriggerRule(PS=-1, comment=''),
                                    lumi_l1topo_enable_j_xe : TriggerRule(rate=0.2, comment=''), },
    'HLT_j100_xe80_L1J40_DPHI-J20s2XE50'          : {  1000 : TriggerRule(PS=-1, comment=''),
                                    lumi_l1topo_enable_j_xe : TriggerRule(rate=0.2, comment=''), },

    # backup
    'HLT_j80_xe80_dphi1_L1J40_DPHI-CJ20XE50'      : {  1000 : TriggerRule(PS=-1, comment='') },
    'HLT_j80_xe80_dphi1_L1J40_DPHI-Js2XE50'       : {  1000 : TriggerRule(PS=-1, comment='') },
    'HLT_j100_xe80_L1J40_DPHI-CJ20XE50'           : {  1000 : TriggerRule(PS=-1, comment='') },
    'HLT_j100_xe80_L1J40_DPHI-Js2XE50'            : {  1000 : TriggerRule(PS=-1, comment='') },
}

########################################

topo_Rules_dedicated={

    # ========================================
    # L1
    # 2 kHz @ 2e34
    'L1_DY-BOX-2MU6'              : {  1000 : TriggerRule(PS=-1, comment=''),
                      lumi_l1topo_enable_dy : TriggerRule(rate=1000, maxRate=1000, comment=''), },
    # 27 kHz @ 2e34
    'L1_DY-BOX-2MU4'              : {  1000 : TriggerRule(PS=-1, comment=''),
                      lumi_l1topo_enable_dy : TriggerRule(rate=1000, maxRate=1000, comment=''), },


    # ========================================
    # Drell-Yan
    # 1.5 Hz @ Run1
    'HLT_mu6_iloose_mu6_24invm60_noos_L1DY-BOX-2MU6'             : {  1000 : TriggerRule(PS=-1, comment=''),
                                                     lumi_l1topo_enable_dy : TriggerRule(rate=3.0, comment=''), },
    'HLT_mu6_iloose_mu6_11invm24_noos_L1DY-BOX-2MU6'             : {  1000 : TriggerRule(PS=-1, comment=''),
                                                     lumi_l1topo_enable_dy : TriggerRule(rate=3.0, comment=''), },
    'HLT_mu4_iloose_mu4_11invm60_noos_L1DY-BOX-2MU4'             : {  1000 : TriggerRule(PS=-1, comment=''),
                                                     lumi_l1topo_enable_dy : TriggerRule(rate=3.0, comment=''), },
    'HLT_mu4_iloose_mu4_7invm9_noos_L1DY-BOX-2MU4'               : {  1000 : TriggerRule(PS=-1, comment=''),
                                                     lumi_l1topo_enable_dy : TriggerRule(rate=3.0, comment=''), },

    # supporting
    # 0.5 Hz @ Run1
    'HLT_mu6_iloose_mu6_24invm60_noos_novtx_L1DY-BOX-2MU6'       : {  1000 : TriggerRule(PS=-1, comment=''),
                                                     lumi_l1topo_enable_dy : TriggerRule(rate=0.5, comment=''), },
    'HLT_mu6_iloose_mu6_11invm24_noos_novtx_L1DY-BOX-2MU6'       : {  1000 : TriggerRule(PS=-1, comment=''),
                                                     lumi_l1topo_enable_dy : TriggerRule(rate=0.5, comment=''), },
    'HLT_mu4_iloose_mu4_11invm60_noos_novtx_L1DY-BOX-2MU4'       : {  1000 : TriggerRule(PS=-1, comment=''),
                                                     lumi_l1topo_enable_dy : TriggerRule(rate=0.5, comment=''), },

    # ========================================
    # LLP
    'HLT_j30_jes_PS_llp_L1LLP-NOMATCH'                           : {  1000 : TriggerRule(PS=-1, comment=''),
                                                    lumi_l1topo_enable_llp : TriggerRule(PS=1, comment=''), },
    'HLT_j30_jes_PS_llp_noiso_L1LLP-NOMATCH'                     : {  1000 : TriggerRule(PS=-1, comment=''),
                                                    lumi_l1topo_enable_llp : TriggerRule(PS=1, comment=''), },

    # ========================================
    # tau->3mu (narrow scan)

    # MU11_2MU6
    'HLT_mu11_2mu4noL1_nscan03_L1MU11_LFV-MU'                    : {  1000 : TriggerRule(PS=-1, comment=''),
                                                  lumi_l1topo_enable_nscan : TriggerRule(PS=1, comment=''), },
    # MU10_2MU6
    # 1.5e34 ?
    'HLT_mu11_2mu4noL1_nscan03'                                  : {  1000 : TriggerRule(PS=-1, comment=''),
                                                  lumi_l1topo_enable_nscan : TriggerRule(PS=1, comment=''),
                                                                     15001 : TriggerRule(PS=-1, comment='Disabled'), },

    # ========================================
    # LFV tau->emu
    # At the moment taumass gives huge rate (both 1 kHz @ 2e34)
    # --> put max rate limit

    # w/ taumass
    'HLT_g20_etcut_mu4_taumass'                                  : {  1000 : TriggerRule(PS=-1, comment=''),
                                                lumi_l1topo_enable_tau_lfv : TriggerRule(rate=10, comment=''), },
    'HLT_g10_etcut_mu10_taumass'                                 : {  1000 : TriggerRule(PS=-1, comment=''),
                                                lumi_l1topo_enable_tau_lfv : TriggerRule(rate=10, comment=''), },
    # supporting, w/o taumass
    'HLT_g20_etcut_mu4_L1LFV-EM15I'                              : {  1000 : TriggerRule(PS=-1, comment=''),
                                                lumi_l1topo_enable_tau_lfv : TriggerRule(rate=0.2, comment=''), },
    'HLT_g10_etcut_mu10_L1LFV-EM8I'                              : {  1000 : TriggerRule(PS=-1, comment=''),
                                                lumi_l1topo_enable_tau_lfv : TriggerRule(rate=0.2, comment=''), },
}

########################################

topo_Rules={}

RulesList=[topo_Rules_bphys, topo_Rules_jet, topo_Rules_bjet, topo_Rules_tau, topo_Rules_egamma, topo_Rules_vbf, topo_Rules_combined, topo_Rules_dedicated] 
for Rules in RulesList:
    for newrule in Rules.keys():
        if topo_Rules.has_key(newrule):
            print 'CRITICAL: duplicated rule cannot be added.',newrule
    topo_Rules.update(Rules)


#################################################################################################
#
#
# disable rules
#
#
#################################################################################################

disabled_Rules={}

# --------------------------------------
# signatures, EB

HLT_list=['HLT_2e12_loose1_L12EM10VH', 'HLT_2e17_loose1', 'HLT_2mu4_bBmumuxv3', 'HLT_2mu4_wOvlpRm_bBmumuxv2', 'HLT_2mu4_wOvlpRm_bDimu', 'HLT_2mu4_wOvlpRm_bDimu_noinvm_novtx_ss', 'HLT_2mu4_wOvlpRm_bDimu_novtx_noos', 'HLT_3j175_jes_PS', 'HLT_3mu6_wOvlpRm_bDimu', 'HLT_4j45_test1', 'HLT_4j45_test2', 'HLT_e0_L2Star_perf_L1EM15', 'HLT_e0_perf_L1EM15', 'HLT_e0_perf_L1EM3_EMPTY', 'HLT_e120_loose1', 'HLT_e17_loose_L1EM15VHJJ1523ETA49', 'HLT_e17_lhloose_L1EM15VHJJ1523ETA49', 'HLT_e17_etcut_L2StarA_L1EM15', 'HLT_e17_etcut_L2StarB_L1EM15', 'HLT_e17_etcut_L2StarC_L1EM15', 'HLT_e24_lhmedium_iloose_L2StarA_L1EM20VH', 'HLT_e24_medium1_L1EM18VH', 'HLT_e24_medium1_L2Star_idperf_L1EM18VH', 'HLT_e24_medium1_idperf_L1EM18VH', 'HLT_e24_medium1_iloose_L1EM18VH', 'HLT_e24_medium1_iloose_L2StarA_L1EM18VH', 'HLT_e24_medium_iloose_L2StarA_L1EM20VH', 'HLT_e24_tight1_iloose', 'HLT_e24_tight1_iloose_L1EM20VH', 'HLT_e26_tight1_iloose', 'HLT_e5_lhloose_L2StarA', 'HLT_e5_loose1_L2Star_idperf', 'HLT_e5_loose1_idperf', 'HLT_e5_loose_L2StarA', 'HLT_e5_tight1_e4_etcut', 'HLT_e5_tight1_e4_etcut_Jpsiee', 'HLT_e5_tight1_e4_etcut_Jpsiee_L1JPSI-1M5', 'HLT_e5_tight1_e4_etcut_L1JPSI-1M5', 'HLT_e60_medium1', 'HLT_eb_empty_L1RD0_EMPTY', 'HLT_eb_high_L1RD2_FILLED', 'HLT_eb_low_L1RD2_FILLED', 'HLT_g0_perf_L1EM15', 'HLT_g0_perf_L1EM3_EMPTY', 'HLT_g120_loose1', 'HLT_g35_loose1_g25_loose1', 'HLT_g35_medium1_g25_medium1', 'HLT_ht0_L1J12_EMPTY', 'HLT_j0_L1J12_EMPTY', 'HLT_j0_lcw_jes_L1J12', 'HLT_j0_perf_bperf_L1J12_EMPTY', 'HLT_j0_perf_bperf_L1MU10', 'HLT_j0_perf_bperf_L1RD0_EMPTY', 'HLT_j0_perf_ds1_L1All', 'HLT_j15_320eta490', 'HLT_j15_j15_320eta490', 'HLT_j200_jes_PS', 'HLT_j25_320eta490', 'HLT_j25_j25_320eta490', 'HLT_j35_320eta490', 'HLT_j35_j35_320eta490', 'HLT_j45_320eta490', 'HLT_j45_j45_320eta490', 'HLT_j55_320eta490', 'HLT_j55_j55_320eta490', 'HLT_j55_j55_320eta490_L1RD0_FILLED', 'HLT_j60_j60_320eta490', 'HLT_j85_0eta240_test1_j25_240eta490_test1', 'HLT_j85_0eta240_test2_j25_240eta490_test2', 'HLT_j85_2j45', 'HLT_j85_j85_320eta490', 'HLT_j85_jes_test1', 'HLT_j85_jes_test2', 'HLT_j85_test1', 'HLT_j85_test1_2j45_test1', 'HLT_j85_test2', 'HLT_j85_test2_2j45_test2', 'HLT_mu0noL1_fsperf', 'HLT_mu15_mucombTag_noEF_L1MU40', 'HLT_mu20_iloose_2mu4noL1', 'HLT_mu20_iloose_mu8noL1', 'HLT_mu20_imedium_mu8noL1', 'HLT_mu20_mucombTag_noEF_L1MU40', 'HLT_mu25_mucombTag_noEF_L1MU40', 'HLT_mu4_cosmic_L1MU11_EMPTY', 'HLT_mu4_cosmic_L1MU4_EMPTY', 'HLT_mu4_msonly_cosmic_L1MU11_EMPTY', 'HLT_mu4_msonly_cosmic_L1MU4_EMPTY', 'HLT_tau125_r1medium1', 'HLT_tau125_r1perf', 'HLT_tau1_cosmic_ptonly_L1MU4_EMPTY', 'HLT_tau1_cosmic_track_L1MU4_EMPTY', 'HLT_tau20_r1medium1', 'HLT_tau20_r1perf', 'HLT_tau25_medium1_calo', 'HLT_tau25_medium1_trackcalo', 'HLT_tau25_medium1_tracktwocalo', 'HLT_tau25_perf_calo', 'HLT_tau25_perf_calo_L1TAU12', 'HLT_tau25_perf_trackcalo', 'HLT_tau25_perf_tracktwocalo', 'HLT_tau25_r1perf', 'HLT_tau29_r1medium1', 'HLT_tau29_r1perf', 'HLT_tau35_medium1_calo', 'HLT_tau35_perf_ptonly_tau25_perf_ptonly_L1TAU20IM_2TAU12IM', 'HLT_tau35_perf_tracktwo_L1TAU20_tau25_perf_tracktwo_L1TAU12', 'HLT_tau35_tight1_tracktwo', 'HLT_tau8_cosmic_ptonly', 'HLT_tau8_cosmic_track', 'HLT_te20', 'HLT_te20_tc_lcw', 'HLT_xe0noL1_l2fsperf', 'HLT_xe0noL1_l2fsperf_mht', 'HLT_xe0noL1_l2fsperf_pueta', 'HLT_xe0noL1_l2fsperf_pufit', 'HLT_xe0noL1_l2fsperf_tc_em', 'HLT_xe0noL1_l2fsperf_tc_lcw', 'HLT_xe35_L2FS', 'HLT_xe35_l2fsperf_wEFMuFEB_wEFMu', 'HLT_xs30', 'HLT_xs30_tc_lcw', ]

disabled_Rules.update(dict(map(None,HLT_list,len(HLT_list)*[{
    1000 : TriggerRule(PS=-1, comment=''), 
    }])))

# --------------------------------------
# low mu run chains

HLT_list=[
'HLT_j10_L1ALFA_Jet', 'HLT_j10_L1ALFA_UNPAIRED_ISO_Jet', 'HLT_j15_L1ALFA_Jet', 'HLT_j15_L1ALFA_UNPAIRED_ISO_Jet', 'HLT_j25_L1ALFA_Jet', 'HLT_j25_L1ALFA_UNPAIRED_ISO_Jet', 'HLT_j35_L1ALFA_Jet', 'HLT_j35_L1ALFA_UNPAIRED_ISO_Jet', 'HLT_lhcfpeb', 'HLT_lhcfpeb_L1LHCF_ALFA_ANY_A', 'HLT_lhcfpeb_L1LHCF_ALFA_ANY_A_UNPAIRED_ISO', 'HLT_lhcfpeb_L1LHCF_ALFA_ANY_C', 'HLT_lhcfpeb_L1LHCF_ALFA_ANY_C_UNPAIRED_ISO', 'HLT_lhcfpeb_L1LHCF_EMPTY', 'HLT_lhcfpeb_L1LHCF_UNPAIRED_ISO', 'HLT_te40_L1MBTS_1_1', 'HLT_te50_L1MBTS_1_1', 'HLT_te50_L1RD3_FILLED', 'HLT_te50_L1TE10', 'HLT_te50_L1TE10.0ETA24', 'HLT_te50_L1TE10.0ETA25', 'HLT_te60_L1MBTS_1_1', 'HLT_te60_L1TE20', 'HLT_te60_L1TE20.0ETA24', 'HLT_te60_L1TE20.0ETA25', 'HLT_te70_L1MBTS_1_1', 'HLT_te80_L1MBTS_1_1',]

disabled_Rules.update(dict(map(None,HLT_list,len(HLT_list)*[{
    1000 : TriggerRule(PS=-1, comment=''), 
    }])))

# --------------------------------------
# streamers

HLT_list=[
'HLT_noalg_L12EM10VH', 'HLT_noalg_L12EM15', 'HLT_noalg_L12EM3', 'HLT_noalg_L12EM7', 'HLT_noalg_L12EM8VH_MU10', 'HLT_noalg_L12J15_XE55', 'HLT_noalg_L12MU4', 'HLT_noalg_L12MU6', 'HLT_noalg_L13J25.0ETA23', 'HLT_noalg_L13J40', 'HLT_noalg_L13MU4', 'HLT_noalg_L14J15', 'HLT_noalg_L14J20.0ETA49', 'HLT_noalg_L1ALFA_Diff', 'HLT_noalg_L1ALFA_Phys', 'HLT_noalg_L1ALFA_PhysAny', 'HLT_noalg_L1ALFA_SYS', 'HLT_noalg_L1ALFA_TRT_Diff', 'HLT_noalg_L1ALFA_TRT_UNPAIRED_ISO_Diff', 'HLT_noalg_L1All', 'HLT_noalg_L1CALREQ2', 'HLT_noalg_L1Calo', 'HLT_noalg_L1Calo_EMPTY', 'HLT_noalg_L1EM12', 'HLT_noalg_L1EM15', 'HLT_noalg_L1EM15HI_2TAU12IM', 'HLT_noalg_L1EM15HI_2TAU12IM_J25_3J12', 'HLT_noalg_L1EM15HI_2TAU12IM_XE35', 'HLT_noalg_L1EM15HI_TAU40_2TAU15', 'HLT_noalg_L1EM15VH_3EM7', 'HLT_noalg_L1EM15VH_MU10', 'HLT_noalg_L1EM15_MU4', 'HLT_noalg_L1EM18VH', 'HLT_noalg_L1EM20VH', 'HLT_noalg_L1EM3', 'HLT_noalg_L1EM7', 'HLT_noalg_L1EM7_MU10', 'HLT_noalg_L1J15_J15.32ETA49', 'HLT_noalg_L1J20_J20.32ETA49', 'HLT_noalg_L1J30.0ETA49_2J20.0ETA49', 'HLT_noalg_L1J400', 'HLT_noalg_L1J50', 'HLT_noalg_L1J75', 'HLT_noalg_L1J75.32ETA49', 'HLT_noalg_L1LHCF', 'HLT_noalg_L1LHCF_EMPTY', 'HLT_noalg_L1LHCF_UNPAIRED_ISO', 'HLT_noalg_L1LowLumi', 'HLT_noalg_L1MBTS_1_BGRP11', 'HLT_noalg_L1MBTS_1_BGRP9', 'HLT_noalg_L1MBTS_2_BGRP11', 'HLT_noalg_L1MBTS_2_BGRP9', 'HLT_noalg_L1MJJ-100', 'HLT_noalg_L1MU10_TAU12IM', 'HLT_noalg_L1MU10_TAU12IM_J25_2J12', 'HLT_noalg_L1MU10_TAU12IM_XE35', 'HLT_noalg_L1MU10_TAU12_J25_2J12', 'HLT_noalg_L1MU10_TAU20', 'HLT_noalg_L1MU4', 'HLT_noalg_L1MU4_3J15', 'HLT_noalg_L1MU4_EMPTY', 'HLT_noalg_L1MU4_FIRSTEMPTY', 'HLT_noalg_L1MU4_J12', 'HLT_noalg_L1MU4_J30', 'HLT_noalg_L1MU6', 'HLT_noalg_L1MU6_J20', 'HLT_noalg_L1MU6_J30.0ETA49_2J20.0ETA49', 'HLT_noalg_L1MU6_J40', 'HLT_noalg_L1MinBias', 'HLT_noalg_L1RD0_BGRP11', 'HLT_noalg_L1RD0_BGRP9', 'HLT_noalg_L1Standby', 'HLT_noalg_L1TAU12', 'HLT_noalg_L1TAU12IL', 'HLT_noalg_L1TAU12IM', 'HLT_noalg_L1TAU12IT', 'HLT_noalg_L1TAU20', 'HLT_noalg_L1TAU20IL', 'HLT_noalg_L1TAU20IM', 'HLT_noalg_L1TAU20IM_2TAU12IM', 'HLT_noalg_L1TAU20IM_2TAU12IM_J25_2J20_3J12', 'HLT_noalg_L1TAU20IM_2TAU12IM_XE35', 'HLT_noalg_L1TAU20IT', 'HLT_noalg_L1TAU20_2J20_XE45', 'HLT_noalg_L1TAU20_2TAU12', 'HLT_noalg_L1TAU20_2TAU12_XE35', 'HLT_noalg_L1TAU30', 'HLT_noalg_L1TAU60', 'HLT_noalg_L1XE35', 'HLT_noalg_L1XE50', 'HLT_noalg_bkg_L1Bkg', 'HLT_noalg_bkg_L1J12', 'HLT_noalg_bkg_L1J12_BGRP0', 'HLT_noalg_bkg_L1J30.32ETA49', 'HLT_noalg_bkg_L1J30.32ETA49_BGRP0', 'HLT_noalg_cosmiccalo_L1EM3_EMPTY', 'HLT_noalg_cosmiccalo_L1J12_EMPTY', 'HLT_noalg_cosmiccalo_L1J12_FIRSTEMPTY', 'HLT_noalg_cosmiccalo_L1J3032ETA49_EMPTY', 'HLT_noalg_cosmiccalo_L1J30_EMPTY', 'HLT_noalg_cosmiccalo_L1J30_FIRSTEMPTY', 'HLT_noalg_cosmiccalo_L1RD1_EMPTY', 'HLT_noalg_cosmicmuons_L1MU11_EMPTY', 'HLT_noalg_cosmicmuons_L1MU4_EMPTY', 'HLT_noalg_dcmmon_L1RD0_EMPTY', 'HLT_noalg_dcmmon_L1RD2_BGRP11', 'HLT_noalg_dcmmon_L1RD2_EMPTY', 'HLT_noalg_eb_L1ABORTGAPNOTCALIB_noPS', 'HLT_noalg_eb_L1EMPTY_noPS', 'HLT_noalg_eb_L1FIRSTEMPTY_noPS', 'HLT_noalg_eb_L1PhysicsHigh_noPS', 'HLT_noalg_eb_L1PhysicsLow_noPS', 'HLT_noalg_eb_L1RD0_EMPTY', 'HLT_noalg_eb_L1RD3_EMPTY', 'HLT_noalg_eb_L1RD3_FILLED', 'HLT_noalg_eb_L1UNPAIRED_ISO_noPS', 'HLT_noalg_eb_L1UNPAIRED_NONISO_noPS', 'HLT_noalg_idcosmic_L1TRT_EMPTY', 'HLT_noalg_idcosmic_L1TRT_FILLED', 'HLT_noalg_idmon_L1RD0_EMPTY', 'HLT_noalg_idmon_L1RD0_FILLED', 'HLT_noalg_idmon_L1RD0_UNPAIRED_ISO', 'HLT_noalg_to_L12MU20_OVERLAY', 'HLT_noalg_zb_L1ZB', ]

disabled_Rules.update(dict(map(None,HLT_list,len(HLT_list)*[{
    1000 : TriggerRule(PS=-1, comment=''), 
    }])))

# --------------------------------------
# beam spot

HLT_list=[
'HLT_beamspot_activeTE_L2StarB_peb', 'HLT_beamspot_activeTE_L2StarB_pebTRT', 'HLT_beamspot_activeTE_L2StarB_pebTRT_L1TRT_EMPTY', 'HLT_beamspot_activeTE_L2StarB_pebTRT_L1TRT_FILLED', 'HLT_beamspot_activeTE_L2StarB_peb_L1TRT_EMPTY', 'HLT_beamspot_activeTE_L2StarB_peb_L1TRT_FILLED', 'HLT_beamspot_activeTE_trkfast_peb', 'HLT_beamspot_activeTE_trkfast_pebTRT', 'HLT_beamspot_activeTE_trkfast_pebTRT_L1TRT_EMPTY', 'HLT_beamspot_activeTE_trkfast_pebTRT_L1TRT_FILLED', 'HLT_beamspot_activeTE_trkfast_peb_L1TRT_EMPTY', 'HLT_beamspot_activeTE_trkfast_peb_L1TRT_FILLED', 'HLT_beamspot_allTE_L2StarB', 'HLT_beamspot_allTE_L2StarB_L1TRT_EMPTY', 'HLT_beamspot_allTE_L2StarB_L1TRT_FILLED', 'HLT_beamspot_allTE_L2StarB_peb', 'HLT_beamspot_allTE_L2StarB_pebTRT', 'HLT_beamspot_allTE_L2StarB_pebTRT_L1TRT_EMPTY', 'HLT_beamspot_allTE_L2StarB_pebTRT_L1TRT_FILLED', 'HLT_beamspot_allTE_L2StarB_peb_L1TRT_EMPTY', 'HLT_beamspot_allTE_L2StarB_peb_L1TRT_FILLED', 'HLT_beamspot_allTE_trkfast', 'HLT_beamspot_allTE_trkfast_L1TRT_EMPTY', 'HLT_beamspot_allTE_trkfast_L1TRT_FILLED', 'HLT_beamspot_allTE_trkfast_peb', 'HLT_beamspot_allTE_trkfast_pebTRT', 'HLT_beamspot_allTE_trkfast_pebTRT_L1TRT_EMPTY', 'HLT_beamspot_allTE_trkfast_pebTRT_L1TRT_FILLED', 'HLT_beamspot_allTE_trkfast_peb_L1TRT_EMPTY', 'HLT_beamspot_allTE_trkfast_peb_L1TRT_FILLED', 'HLT_beamspot_trkFS_L2StarB', 'HLT_beamspot_trkFS_L2StarB_L1TRT_EMPTY', 'HLT_beamspot_trkFS_L2StarB_L1TRT_FILLED', 'HLT_beamspot_trkFS_trkfast', 'HLT_beamspot_trkFS_trkfast_L1TRT_EMPTY', 'HLT_beamspot_trkFS_trkfast_L1TRT_FILLED', ]

disabled_Rules.update(dict(map(None,HLT_list,len(HLT_list)*[{
    1000 : TriggerRule(PS=-1, comment=''), 
    }])))

# --------------------------------------
# calibration

HLT_list=[
'HLT_alfacalib', 'HLT_alfacalib_L1ALFA_A7L1_OD', 'HLT_alfacalib_L1ALFA_A7R1_OD', 'HLT_alfacalib_L1ALFA_ANY', 'HLT_alfacalib_L1ALFA_B7L1_OD', 'HLT_alfacalib_L1ALFA_B7R1_OD', 'HLT_alfacalib_L1ALFA_ELAS', 'HLT_alfacalib_L1ALFA_ELAST15', 'HLT_alfacalib_L1ALFA_ELAST18', 'HLT_alfacalib_L1ALFA_SYS', 'HLT_alfacalib_L1ALFA_TRT', 'HLT_alfacalib_L1LHCF_ALFA_ANY_A', 'HLT_alfacalib_L1LHCF_ALFA_ANY_A_UNPAIRED_ISO', 'HLT_alfacalib_L1LHCF_ALFA_ANY_C', 'HLT_alfacalib_L1LHCF_ALFA_ANY_C_UNPAIRED_ISO', 'HLT_alfaidpeb_L1TRT_ALFA_ANY', 'HLT_conej140_320eta490_larpebj', 'HLT_conej165_larpebj', 'HLT_conej40_larpebj', 'HLT_conej75_320eta490_larpebj', 'HLT_costmonitor', 'HLT_cscmon_L1All', 'HLT_cscmon_L1EM3', 'HLT_cscmon_L1J12', 'HLT_cscmon_L1MU10', 'HLT_ibllumi_L1RD0_ABORTGAPNOTCALIB', 'HLT_ibllumi_L1RD0_FILLED', 'HLT_ibllumi_L1RD0_UNPAIRED_ISO', 'HLT_id_cosmicid', 'HLT_id_cosmicid_L1MU11_EMPTY', 'HLT_id_cosmicid_L1MU4_EMPTY', 'HLT_id_cosmicid_trtxk', 'HLT_id_cosmicid_trtxk_central', 'HLT_idcalib_trk16_central', 'HLT_idcalib_trk16_fwd', 'HLT_idcalib_trk29_central', 'HLT_idcalib_trk29_fwd', 'HLT_idcalib_trk9_central', 'HLT_idcalib_trk9_fwd', 'HLT_j40_L1ZB', 'HLT_l1calocalib', 'HLT_larcalib_L1EM3_EMPTY', 'HLT_larcalib_L1J12_EMPTY', 'HLT_larcalib_L1J3032ETA49_EMPTY', 'HLT_larcalib_L1TAU8_EMPTY', 'HLT_larhec_L1EM7_FIRSTEMPTY', 'HLT_larhec_L1J12_FIRSTEMPTY', 'HLT_larhec_L1TAU8_FIRSTEMPTY', 'HLT_larnoiseburst_L1All', 'HLT_larnoiseburst_L1J40_XE50', 'HLT_larnoiseburst_L1J50', 'HLT_larnoiseburst_L1J75', 'HLT_larnoiseburst_L1J75_XE40', 'HLT_larnoiseburst_L1XE35', 'HLT_larnoiseburst_L1XE50', 'HLT_larnoiseburst_loose_L1All', 'HLT_larnoiseburst_loose_rerun', 'HLT_larnoiseburst_rerun', 'HLT_larps_L1EM3_EMPTY', 'HLT_larps_L1EM7_EMPTY', 'HLT_larps_L1J12_EMPTY', 'HLT_larps_L1J12_FIRSTEMPTY', 'HLT_larps_L1J30_EMPTY', 'HLT_larps_L1J30_FIRSTEMPTY', 'HLT_larps_L1TAU8_EMPTY', 'HLT_lumipeb_L1RD0_ABORTGAPNOTCALIB', 'HLT_lumipeb_L1RD0_EMPTY', 'HLT_lumipeb_L1RD0_FILLED', 'HLT_lumipeb_L1RD0_UNPAIRED_ISO', 'HLT_lumipeb_vdm_L1MBTS_1', 'HLT_lumipeb_vdm_L1MBTS_1_BGRP11', 'HLT_lumipeb_vdm_L1MBTS_1_BGRP9', 'HLT_lumipeb_vdm_L1MBTS_1_UNPAIRED_ISO', 'HLT_lumipeb_vdm_L1MBTS_2', 'HLT_lumipeb_vdm_L1MBTS_2_BGRP11', 'HLT_lumipeb_vdm_L1MBTS_2_BGRP9', 'HLT_lumipeb_vdm_L1MBTS_2_UNPAIRED_ISO', 'HLT_lumipeb_vdm_L1RD0_BGRP11', 'HLT_lumipeb_vdm_L1RD0_BGRP9', 'HLT_lumipeb_vdm_L1RD0_FILLED', 'HLT_lumipeb_vdm_L1RD0_UNPAIRED_ISO', 'HLT_mu0_muoncalib', 'HLT_robrequest', 'HLT_robrequest_L1RD0_EMPTY', 'HLT_sct_noise', 'HLT_tilecalib_laser', 'HLT_timeburner',]

disabled_Rules.update(dict(map(None,HLT_list,len(HLT_list)*[{
    1000 : TriggerRule(PS=-1, comment=''), 
    }])))

# --------------------------------------
# minbias

HLT_list=[
'HLT_mb_mbts_L1MBTS_1', 'HLT_mb_mbts_L1MBTS_1_1', 'HLT_mb_mbts_L1MBTS_1_1_EMPTY', 'HLT_mb_mbts_L1MBTS_1_1_UNPAIRED_ISO', 'HLT_mb_mbts_L1MBTS_1_EMPTY', 'HLT_mb_mbts_L1MBTS_1_UNPAIRED_ISO', 'HLT_mb_mbts_L1MBTS_2', 'HLT_mb_mbts_L1MBTS_2_EMPTY', 'HLT_mb_mbts_L1MBTS_2_UNPAIRED_ISO', 'HLT_mb_perf_L1LUCID', 'HLT_mb_perf_L1LUCID_EMPTY', 'HLT_mb_perf_L1LUCID_UNPAIRED_ISO', 'HLT_mb_perf_L1MBTS_2', 'HLT_mb_perf_L1RD1_FILLED', 'HLT_mb_sp1000_trk70_hmt_L1MBTS_1_1', 'HLT_mb_sp1200_trk75_hmt_L1MBTS_1_1', 'HLT_mb_sp1400_hmtperf_L1TE10', 'HLT_mb_sp1400_hmtperf_L1TE10.0ETA24', 'HLT_mb_sp1400_hmtperf_L1TE20', 'HLT_mb_sp1400_hmtperf_L1TE20.0ETA24', 'HLT_mb_sp1400_trk100_hmt_L1TE20', 'HLT_mb_sp1400_trk100_hmt_L1TE20.0ETA24', 'HLT_mb_sp1400_trk80_hmt_L1MBTS_1_1', 'HLT_mb_sp1400_trk90_hmt_L1TE10', 'HLT_mb_sp1400_trk90_hmt_L1TE10.0ETA24', 'HLT_mb_sp1800_hmtperf', 'HLT_mb_sp1800_hmtperf_L1TE20', 'HLT_mb_sp2000_pusup600_trk70_hmt', 'HLT_mb_sp2000_pusup600_trk70_hmt_L1TE30', 'HLT_mb_sp2000_pusup600_trk70_hmt_L1TE40', 'HLT_mb_sp2000_pusup700_trk50_sumet110_hmt_L1TE40', 'HLT_mb_sp2000_pusup700_trk50_sumet150_hmt_L1TE50', 'HLT_mb_sp2000_pusup700_trk50_sumet70_hmt_L1TE30', 'HLT_mb_sp2000_pusup700_trk60_hmt_L1TE30', 'HLT_mb_sp2000_pusup700_trk70_hmt_L1TE30', 'HLT_mb_sp2000_pusup700_trk70_hmt_L1TE40', 'HLT_mb_sp2000_trk70_hmt', 'HLT_mb_sp2500_hmtperf_L1TE20', 'HLT_mb_sp2500_pusup750_trk90_hmt_L1TE40', 'HLT_mb_sp2_hmtperf', 'HLT_mb_sp2_hmtperf_L1MBTS_1_1', 'HLT_mb_sp3000_pusup800_trk120_hmt_L1TE50', 'HLT_mb_sp300_trk10_sumet40_hmt_L1MBTS_1_1', 'HLT_mb_sp300_trk10_sumet50_hmt_L1MBTS_1_1', 'HLT_mb_sp300_trk10_sumet50_hmt_L1RD3_FILLED', 'HLT_mb_sp300_trk10_sumet50_hmt_L1TE10', 'HLT_mb_sp300_trk10_sumet60_hmt_L1MBTS_1_1', 'HLT_mb_sp300_trk10_sumet60_hmt_L1TE20', 'HLT_mb_sp300_trk10_sumet70_hmt_L1MBTS_1_1', 'HLT_mb_sp300_trk10_sumet80_hmt_L1MBTS_1_1', 'HLT_mb_sp400_trk40_hmt_L1MBTS_1_1', 'HLT_mb_sp500_hmtperf', 'HLT_mb_sp600_trk10_sumet40_hmt_L1MBTS_1_1', 'HLT_mb_sp600_trk10_sumet50_hmt_L1MBTS_1_1', 'HLT_mb_sp600_trk10_sumet50_hmt_L1RD3_FILLED', 'HLT_mb_sp600_trk10_sumet50_hmt_L1TE10.0ETA24', 'HLT_mb_sp600_trk10_sumet60_hmt_L1MBTS_1_1', 'HLT_mb_sp600_trk10_sumet60_hmt_L1TE20.0ETA24', 'HLT_mb_sp600_trk10_sumet70_hmt_L1MBTS_1_1', 'HLT_mb_sp600_trk10_sumet80_hmt_L1MBTS_1_1', 'HLT_mb_sp600_trk45_hmt_L1MBTS_1_1', 'HLT_mb_sp700_trk50_hmt_L1MBTS_1_1', 'HLT_mb_sp700_trk50_hmt_L1RD3_FILLED', 'HLT_mb_sp700_trk55_hmt_L1MBTS_1_1', 'HLT_mb_sp900_trk60_hmt_L1MBTS_1_1', 'HLT_mb_sp900_trk65_hmt_L1MBTS_1_1', 'HLT_mb_sptrk','HLT_mb_sptrk_L1RD0_EMPTY', 'HLT_mb_sptrk_L1RD0_UNPAIRED_ISO', 'HLT_mb_sptrk_L1RD3_FILLED', 'HLT_mb_sptrk_costr', 'HLT_mb_sptrk_costr_L1RD0_EMPTY', 'HLT_mb_sptrk_noisesup', 'HLT_mb_sptrk_noisesup_L1RD0_EMPTY', 'HLT_mb_sptrk_noisesup_L1RD0_UNPAIRED_ISO', 'HLT_mb_sptrk_noisesup_L1RD3_FILLED', 'HLT_mb_sptrk_pt4_L1MBTS_1', 'HLT_mb_sptrk_pt4_L1MBTS_1_1', 'HLT_mb_sptrk_pt4_L1MBTS_2', 'HLT_mb_sptrk_pt4_L1RD3_FILLED', 'HLT_mb_sptrk_pt6_L1MBTS_1', 'HLT_mb_sptrk_pt6_L1MBTS_1_1', 'HLT_mb_sptrk_pt6_L1MBTS_2', 'HLT_mb_sptrk_pt6_L1RD3_FILLED', 'HLT_mb_sptrk_pt8_L1MBTS_1', 'HLT_mb_sptrk_pt8_L1MBTS_1_1', 'HLT_mb_sptrk_pt8_L1MBTS_2', 'HLT_mb_sptrk_pt8_L1RD3_FILLED', 'HLT_mb_sptrk_vetombts2in_L1ALFA_CEP', 'HLT_mb_sptrk_vetombts2in_peb_L1ALFA_ANY', 'HLT_mb_sptrk_vetombts2in_peb_L1ALFA_ANY_UNPAIRED_ISO', 'HLT_noalg_mb_L1LUCID', 'HLT_noalg_mb_L1LUCID_EMPTY', 'HLT_noalg_mb_L1LUCID_UNPAIRED_ISO', 'HLT_noalg_mb_L1MBTS_1', 'HLT_noalg_mb_L1MBTS_1_1', 'HLT_noalg_mb_L1MBTS_1_1_EMPTY', 'HLT_noalg_mb_L1MBTS_1_1_UNPAIRED_ISO', 'HLT_noalg_mb_L1MBTS_1_EMPTY', 'HLT_noalg_mb_L1MBTS_1_UNPAIRED_ISO', 'HLT_noalg_mb_L1MBTS_2', 'HLT_noalg_mb_L1MBTS_2_EMPTY', 'HLT_noalg_mb_L1MBTS_2_UNPAIRED_ISO', 'HLT_noalg_mb_L1RD0_EMPTY', 'HLT_noalg_mb_L1RD0_FILLED', 'HLT_noalg_mb_L1RD0_UNPAIRED_ISO', 'HLT_noalg_mb_L1RD1_FILLED', 'HLT_noalg_mb_L1RD2_EMPTY', 'HLT_noalg_mb_L1RD2_FILLED', 'HLT_noalg_mb_L1RD3_EMPTY', 'HLT_noalg_mb_L1RD3_FILLED', 'HLT_noalg_mb_L1TE10', 'HLT_noalg_mb_L1TE10.0ETA24', 'HLT_noalg_mb_L1TE20', 'HLT_noalg_mb_L1TE20.0ETA24', 'HLT_noalg_mb_L1TE30', 'HLT_noalg_mb_L1TE40', 'HLT_noalg_mb_L1TE50',
]

disabled_Rules.update(dict(map(None,HLT_list,len(HLT_list)*[{
    1000 : TriggerRule(PS=-1, comment=''), 
    }])))

#################################################################################################
#################################################################################################


rules = {}

RulesList=[muon_Rules, bphys_Rules, met_Rules, jet_Rules, bjet_Rules, tau_Rules, egamma_Rules, combined_Rules, vbf_Rules, dedicated_Rules, topo_Rules, disabled_Rules]
### RulesList=[muon_Rules, bphys_Rules, met_Rules, jet_Rules, bjet_Rules, tau_Rules, egamma_Rules, combined_Rules, vbf_Rules, dedicated_Rules, disabled_Rules]
for Rules in RulesList:
    for newrule in Rules.keys():
        if rules.has_key(newrule):
            print 'CRITICAL: duplicated rule cannot be added.',newrule
    rules.update(Rules)

#################################################################################################
#################################################################################################
