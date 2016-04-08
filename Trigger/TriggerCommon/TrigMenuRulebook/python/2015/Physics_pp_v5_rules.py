# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from RuleClasses import TriggerRule

tag = 'Physics_pp_v5'

forRateEstim=False

from runOptions import filled_bunches as NumBunches

#################################################################################################
#
#
# switching luminosity points
#
#
#################################################################################################


########################################
# L1 Topo
lumi_nonl1topo_disable_tau_tau=15001
lumi_nonl1topo_disable_tau_e=15001
lumi_nonl1topo_disable_tau_mu=15001

lumi_nonl1topo_disable_vbf_g_5j=1001
# decision 19/10 OAB
#lumi_nonl1topo_disable_vbf_g_5j=15001
lumi_nonl1topo_disable_vbf_mu=15001
lumi_nonl1topo_disable_vbf_bjet=15001

lumi_nonl1topo_disable_nscan=15001

########################################
# L1 EM/TAU Iso
lumi_l1iso_enable_em=6501  # EM20VH -> 20VHI
lumi_l1iso_enable_tau=3001
lumi_l1iso_enable_tau_xe=10001

########################################
# L1 Jet requirement add
### changed from 5001 to 7501 due to ATR-12007
### lumi_l1jet_enable_tau_tau=5001
lumi_l1jet_enable_tau_tau=8001

########################################
# Bphys L1 evolution
# new evolution as agreed at OAB on October 19
lumi_disable_l1_2mu4=4001
#lumi_disable_l1_2mu4=5501
lumi_disable_l1_mu6_2mu4=12001

lumi_disable_l1_2mu6=15001 ## significant impact if moved to 2mu10

# decision OAB 19/10, leave all physics items til end of 2015
#lumi_disable_l1_3mu4=5001
lumi_disable_l1_3mu4=6501
	
########################################
# muon evolution
lumi_disable_l1_mu15=10001

lumi_disable_l1_mu6_3mu4=12001
lumi_disable_l1_2mu6_3mu4=15001


lumi_disable_hlt_mu20=6501
lumi_disable_hlt_2mu10=6501

########################################
# XE L1 evolution
lumi_disable_l1_xe50=10001
lumi_disable_l1_xe60=12001
	
########################################
# e/gamma evolution
lumi_disable_l1_em20vhi=10001
lumi_disable_l1_em18vh=3001

lumi_disable_l1_2em13vh=10001
lumi_disable_l1_2em10vh=6501
lumi_disable_l1_2em15=3001

# hlt
lumi_disable_hlt_2g_loose=6501
lumi_disable_hlt_eg120=6501

########################################
# tau L1 evolution
lumi_disable_l1_tau30=3001
lumi_disable_l1_tau40=7501

########################################
# jet evolution
# Keep J75 and FJ75 until 7.5E33 for data scouting
lumi_disable_l1_j75=7501

lumi_disable_l1_3j40=6501
lumi_disable_l1_4j15=6501

# hlt
lumi_disable_hlt_j360=6501
lumi_disable_hlt_j360_a10r=6501

########################################
# bjet evolution
lumi_disable_l1_3j25_0eta23=10001
lumi_disable_l1_4j15_0eta25=6501

lumi_disable_mu_j_phys=15001

# hlt
lumi_disable_1b_225=6501
lumi_disable_2b_150=6501
lumi_disable_2b1j_70=6501

########################################
# exotics
lumi_disable_nscan05=8001
lumi_disable_taumass=8001  # EM8I_MU10, EM15I_MU4

########################################
# end-of-fill
lumi_disable_dy=3001  # needs to be earlier than L1_2MU4


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
    1000 : TriggerRule(PS=1, comment='', ESValue=0), 
    5001 : TriggerRule(PS=1, comment='Express', ESRate=0.3), 
    }])))

# l2msonly: disable when L>3E33 (2015.09.26)
HLT_list=[ 'HLT_2mu14_l2msonly' ]
muon_Rules_2e34.update(dict(map(None,HLT_list,len(HLT_list)*[{
    1000 : TriggerRule(PS=1, comment=''), 
    3001 : TriggerRule(PS=-1, comment='disabled'), 
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

L1_list=['L1_2MU6_3MU4']
muon_Rules_1p5e34.update(dict(map(None,L1_list,len(L1_list)*[{
                         1000 : TriggerRule(PS=1, comment='L1'),
    lumi_disable_l1_2mu6_3mu4 : TriggerRule(PS=-1, comment='Disabled'),
                    }])))

# l2msonly: disable when L>3E33 (2015.09.26)
HLT_list=['HLT_2mu6_l2msonly_mu4_l2msonly_L12MU6_3MU4']
muon_Rules_1p5e34.update(dict(map(None,HLT_list,len(HLT_list)*[{
    1000 : TriggerRule(PS=1, comment=''), 
    3001 : TriggerRule(PS=-1, comment='disabled'), 
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

HLT_list=['HLT_mu40']
muon_Rules_1e34.update(dict(map(None,HLT_list,len(HLT_list)*[{
     1000 : TriggerRule(PS=1, comment=''), 
    10001: TriggerRule(PS=-1, comment='Disabled'),
     }])))

L1_list=['L1_MU6_3MU4']
muon_Rules_1e34.update(dict(map(None,L1_list,len(L1_list)*[{
                        1000 : TriggerRule(PS=1, comment='L1'),
    lumi_disable_l1_mu6_3mu4 : TriggerRule(PS=-1, comment='Disabled'),
                    }])))

# l2msonly: disable when L>3E33 (2015.09.26)
HLT_list=['HLT_mu6_l2msonly_2mu4_l2msonly_L1MU6_3MU4']
muon_Rules_1e34.update(dict(map(None,HLT_list,len(HLT_list)*[{
    1000 : TriggerRule(PS=1, comment=''), 
    3001 : TriggerRule(PS=-1, comment='Disabled'),
    }])))

########################################
# 0.5e34 

muon_Rules_0p5e34={}

HLT_list=['HLT_mu20_iloose_L1MU15']
muon_Rules_0p5e34.update(dict(map(None,HLT_list,len(HLT_list)*[{
                     1000 : TriggerRule(PS=1, comment=''), 
    lumi_disable_hlt_mu20 : TriggerRule(PS=-1, comment='Disabled'),
    }])))

HLT_list=['HLT_2mu10']
muon_Rules_0p5e34.update(dict(map(None,HLT_list,len(HLT_list)*[{
                      1000 : TriggerRule(PS=1, comment='Express', ESRate=0.3), 
    lumi_disable_hlt_2mu10 : TriggerRule(PS=-1, comment='Disabled', ESValue=0),
    }])))

# l2msonly: disable when L>3E33 (2015.09.26)
HLT_list=['HLT_2mu10_l2msonly']
muon_Rules_0p5e34.update(dict(map(None,HLT_list,len(HLT_list)*[{
    1000 : TriggerRule(PS=1, comment=''), 
    3001 : TriggerRule(PS=-1, comment='Disabled'),
    }])))

HLT_list=['HLT_3mu4']
muon_Rules_0p5e34.update(dict(map(None,HLT_list,len(HLT_list)*[{
                    1000 : TriggerRule(PS=1, comment=''), 
    lumi_disable_l1_3mu4 : TriggerRule(PS=-1, comment='Disabled'),
    }])))

# l2msonly: disable when L>3E33 (2015.09.26)
HLT_list=['HLT_3mu4_l2msonly']
muon_Rules_0p5e34.update(dict(map(None,HLT_list,len(HLT_list)*[{
    1000 : TriggerRule(PS=1, comment=''), 
    3001 : TriggerRule(PS=-1, comment='Disabled'),
    }])))

########################################
# supporting

muon_Rules_supporting={}


muon_Rules_supporting.update({
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
})

# ========================================

HLT_list=[
    'HLT_mu11','HLT_mu20','HLT_mu22','HLT_mu26',
    'HLT_mu14_iloose',
    'HLT_mu4_msonly', 'HLT_mu6_msonly','HLT_mu10_msonly',
    'HLT_mu0_perf',
    ]
muon_Rules_supporting.update(dict(map(None,HLT_list,len(HLT_list)*[{
                    1000 : TriggerRule(PS=0, comment='',rerun=1), 
    }])))

# ========================================

muon_Rules_supporting.update({

    # ----------------------------------------
    # Level-1
    # low pT used for supporting (also for bJpsi_Trkloose)
    'L1_MU4'                                   : {  1000 : TriggerRule(rate=3000, maxRate=3000, comment='Prescaled') },
    'L1_MU6'                                   : {  1000 : TriggerRule(rate=1000, maxRate=1000, comment='Prescaled') },

    # ----------------------------------------
    # un-isolated single muon for Background estimation

    # For mu20/24_iloose_L1MU15
    # PS=10 (2015.06.25 MCG)
    'HLT_mu20_L1MU15'                          : {  1000 : TriggerRule(PS=10, comment='', rerun=1),
                                   lumi_disable_hlt_mu20 : TriggerRule(PS=0, comment='', rerun=1) },
    'HLT_mu24_L1MU15'                          : {  1000 : TriggerRule(PS=10, comment='', rerun=1),
                                    lumi_disable_l1_mu15 : TriggerRule(PS=0, comment='', rerun=1) },

    # For mu24_imedium and mu26_imedium
    # PS=32 gives 10 Hz @ 1.0e34
    'HLT_mu24'                                 : {  1000 : TriggerRule(PS=32, comment='', rerun=1) },

    # ----------------------------------------
    # SM W+nJets
    'HLT_mu26_2j20noL1'                        : {  1000 : TriggerRule(PS=224, comment='1 Hz target at 1.3e34') },
    'HLT_mu26_3j20noL1'                        : {  1000 : TriggerRule(PS=156, comment='1 Hz target at 1.3e34') },
    'HLT_mu26_4j15noL1'                        : {  1000 : TriggerRule(PS=156, comment='1 Hz target at 1.3e34') },
    'HLT_mu26_5j15noL1'                        : {  1000 : TriggerRule(PS=104, comment='1 Hz target at 1.3e34') },

    # 1.5e34
    'HLT_mu24_2j20noL1'                        : {  1000 : TriggerRule(PS=224, comment='1 Hz target at 1e34'),
                                                   15001 : TriggerRule(PS=-1, comment='Disabled') },
    'HLT_mu24_3j20noL1'                        : {  1000 : TriggerRule(PS=156, comment='1 Hz target at 1e34'),
                                                   15001 : TriggerRule(PS=-1, comment='Disabled') },
    'HLT_mu24_4j15noL1'                        : {  1000 : TriggerRule(PS=156, comment='1 Hz target at 1e34'),
                                                   15001 : TriggerRule(PS=-1, comment='Disabled') },
    'HLT_mu24_5j15noL1'                        : {  1000 : TriggerRule(PS=104, comment='1 Hz target at 1e34'),
                                                   15001 : TriggerRule(PS=-1, comment='Disabled') },

    # 1.0e34
    'HLT_mu20_L1MU15_2j20noL1'                 : {  1000 : TriggerRule(PS=117, comment='1 Hz target at 3e33', rerun=1),
                                    lumi_disable_l1_mu15 : TriggerRule(PS=-1, comment='Disabled') },
    'HLT_mu20_L1MU15_3j20noL1'                 : {  1000 : TriggerRule(PS=65, comment='1 Hz target at 3e33'),
                                    lumi_disable_l1_mu15 : TriggerRule(PS=-1, comment='Disabled') },
    'HLT_mu20_L1MU15_4j15noL1'                 : {  1000 : TriggerRule(PS=69, comment='1 Hz target at 3e33'),
                                    lumi_disable_l1_mu15 : TriggerRule(PS=-1, comment='Disabled') },
    'HLT_mu20_L1MU15_5j15noL1'                 : {  1000 : TriggerRule(PS=31, comment='1 Hz target at 3e33'),
                                    lumi_disable_l1_mu15 : TriggerRule(PS=-1, comment='Disabled') },

    # ----------------------------------------
    # prescale + rerun
    'HLT_mu4'                                  : {  1000 : TriggerRule(rate=2.0, comment='', rerun=1) },
    'HLT_mu6'                                  : {  1000 : TriggerRule(rate=1.0, comment='', rerun=1) },
    'HLT_mu10'                                 : {  1000 : TriggerRule(rate=1.0, comment='', rerun=1) },
    'HLT_mu14'                                 : {  1000 : TriggerRule(rate=1.0, comment='', rerun=1) },
    'HLT_mu18'                                 : {  1000 : TriggerRule(rate=1.0, comment='Express', ESRate=0.4, rerun=1) },

    # ----------------------------------------
    # L1 streamers
    'HLT_noalg_L1MU10'                         : {  1000 : TriggerRule(inputRate=1.0, comment='') },
    'HLT_noalg_L1MU15'                         : {  1000 : TriggerRule(inputRate=1.0, comment='') },
    'HLT_noalg_L1MU20'                         : {  1000 : TriggerRule(inputRate=1.0, comment='') },

    # ATR-11652, high rates seen
    # MU4_EMPTY: PS=1000 (700 Hz in this morning's run)
    # MU4_FIRSTEMPTY: PS=30000 (16000 Hz in this morning's run)
    # MU4_UNPAIRED_ISO: PS=10 (6 Hz in this morning's run)
    # 25ns.2 update: 2142 Hz for MU4_E, 2051 Hz for MU4_FE, 50 Hz for MU4_UISO, 5 Hz for MU11_E
    # needs 100 Hz input from MU4_E for IDCosmic
    'L1_MU4_EMPTY'                             : {  1000 : TriggerRule(PS=50,   scaling='bunches', comment='') },
    'L1_MU4_FIRSTEMPTY'                        : {  1000 : TriggerRule(PS=1000, scaling='bunches', comment='') },
    'L1_MU11_EMPTY'                            : {  1000 : TriggerRule(PS=1,    scaling='bunches', comment='') },
    ## for UNPAIRED_ISO, included as override rule at the very end
    ## 'L1_MU4_UNPAIRED_ISO'                      : {  1000 : TriggerRule(PS=10,    comment='') },
    #
    'HLT_noalg_L1MU4_EMPTY'                    : {  1000 : TriggerRule(inputRate=0.5, comment='') },
    'HLT_noalg_L1MU4_FIRSTEMPTY'               : {  1000 : TriggerRule(inputRate=0.5, comment='') },
    'HLT_noalg_L1MU4_UNPAIRED_ISO'             : {  1000 : TriggerRule(inputRate=0.5, comment='') },
    'HLT_noalg_L1MU11_EMPTY'                   : {  1000 : TriggerRule(inputRate=0.5, comment='') },

    # out-of-time monitoring item, disabled for the moment
    'HLT_noalg_L1MU20_FIRSTEMPTY'              : {  1000 : TriggerRule(PS=-1, comment='') },

    # ----------------------------------------
    # idperf
    # c.f. Run1: 0.1 Hz for mu18, 0.5 Hz for mu22
    'HLT_mu4_idperf'                           : {  1000 : TriggerRule(rate=0.5, comment='', rerun=1) },
    'HLT_mu6_idperf'                           : {  1000 : TriggerRule(rate=0.5, comment='Express', ESRate=0.1, rerun=1) },
    'HLT_mu10_idperf'                          : {  1000 : TriggerRule(rate=0.5, comment='', rerun=1) },
    'HLT_mu20_idperf'                          : {  1000 : TriggerRule(rate=5, comment='Express', ESRate=0.4, rerun=1) },
    'HLT_mu24_idperf'                          : {  1000 : TriggerRule(rate=0.5, comment='', rerun=1) },
})

########################################

muon_Rules={}

RulesList=[muon_Rules_2e34, muon_Rules_1p5e34, muon_Rules_1e34, muon_Rules_0p5e34, muon_Rules_supporting] 
for Rules in RulesList:
    for newrule in Rules.keys():
        if muon_Rules.has_key(newrule):
            print 'FATAL     Physics_pp_v5_rules     Duplicated rule inside physics rule. Cannot be added:',newrule
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

HLT_list=['HLT_2mu10_bBmumu','HLT_2mu10_bBmumuxv2','HLT_2mu10_bBmumux_BcmumuDsloose','HLT_2mu10_bBmumux_BpmumuKp','HLT_2mu10_bJpsimumu','HLT_2mu10_bUpsimumu','HLT_3mu6_bTau','HLT_3mu6_bUpsi']
bphys_Rules_2e34.update(dict(map(None,HLT_list,len(HLT_list)*[{
    1000 : TriggerRule(PS=1, comment=''), 
    }])))

HLT_list=['HLT_2mu10_bBmumuxv2_noL2','HLT_2mu10_bBmumux_BcmumuDsloose_noL2','HLT_2mu10_bBmumux_BpmumuKp_noL2']
bphys_Rules_2e34.update(dict(map(None,HLT_list,len(HLT_list)*[{
    1000 : TriggerRule(PS=1, comment=''),
    }])))


HLT_list=['HLT_3mu6_bJpsi']
bphys_Rules_2e34.update(dict(map(None,HLT_list,len(HLT_list)*[{
    1000 : TriggerRule(PS=1, comment='Express', ESRate=0.1), 
    }])))

HLT_list=['HLT_2mu10_bJpsimumu_noL2']
bphys_Rules_2e34.update(dict(map(None,HLT_list,len(HLT_list)*[{
    1000 : TriggerRule(PS=1, comment=''), 
    }])))

# l2msonly: disable when L>3E33 (2015.09.26)
HLT_list=['HLT_2mu10_l2msonly_bJpsimumu_noL2', 'HLT_mu10_mu10_l2msonly_bJpsimumu_noL2',]
bphys_Rules_2e34.update(dict(map(None,HLT_list,len(HLT_list)*[{
    1000 : TriggerRule(PS=1, comment=''), 
    3001 : TriggerRule(PS=-1, comment='disabled'), 
    }])))

########################################
# 1.5e34 

bphys_Rules_1p5e34={}

L1_list=['L1_MU10_2MU6']
bphys_Rules_1p5e34.update(dict(map(None,L1_list,len(L1_list)*[{
     1000 : TriggerRule(PS=1, comment='L1'), 
    15001 : TriggerRule(rate=1000, maxRate=1000, comment='Prescaled'),
    }])))

HLT_list=['HLT_mu10_mu6_bBmumux_BcmumuDsloose', 'HLT_mu10_mu6_bBmumuxv2', 'HLT_mu10_mu6_bBmumux_BpmumuKp']
bphys_Rules_1p5e34.update(dict(map(None,HLT_list,len(HLT_list)*[{
     1000 : TriggerRule(PS=1, comment=''), 
    15001 : TriggerRule(rate=0.5, comment='Prescaled'), 
    }])))

HLT_list=['HLT_mu10_mu6_bBmumuxv2_noL2','HLT_mu10_mu6_bBmumux_BcmumuDsloose_noL2','HLT_mu10_mu6_bBmumux_BpmumuKp_noL2']
bphys_Rules_1p5e34.update(dict(map(None,HLT_list,len(HLT_list)*[{
     1000 : TriggerRule(PS=1, comment=''), 
    15001 : TriggerRule(rate=0.5, comment='Prescaled'), 
    }])))

########################################
# 1e34 

bphys_Rules_1e34={}

L1_list=['L1_2MU6']
bphys_Rules_1e34.update(dict(map(None,L1_list,len(L1_list)*[{
                    1000 : TriggerRule(PS=1, comment='L1'), 
    lumi_disable_l1_2mu6 : TriggerRule(rate=500, maxRate=500, comment='Prescaled'),
    }])))

HLT_list=['HLT_2mu6_bBmumu','HLT_2mu6_bBmumuxv2','HLT_2mu6_bBmumux_BpmumuKp','HLT_2mu6_bJpsimumu','HLT_2mu6_bUpsimumu']
bphys_Rules_1e34.update(dict(map(None,HLT_list,len(HLT_list)*[{
                    1000 : TriggerRule(PS=1, comment='Express', ESRate=0.1), 
    lumi_disable_l1_2mu6 : TriggerRule(PS=1, comment='Prescaled at L1, Express', ESRate=0.1),
    }])))

HLT_list=['HLT_2mu6_bBmumuxv2_noL2','HLT_2mu6_bBmumux_BcmumuDsloose_noL2','HLT_2mu6_bBmumux_BpmumuKp_noL2']
bphys_Rules_1e34.update(dict(map(None,HLT_list,len(HLT_list)*[{
	            1000 : TriggerRule(PS=1, comment=''),
    lumi_disable_l1_2mu6 : TriggerRule(PS=1, comment='l1 2mu6 is pre scaled already '),
    }])))

HLT_list=['HLT_2mu6_bBmumux_BcmumuDsloose', 'HLT_2mu6_bJpsimumu_noL2']
bphys_Rules_1e34.update(dict(map(None,HLT_list,len(HLT_list)*[{
                    1000 : TriggerRule(PS=1, comment=''), 
    lumi_disable_l1_2mu6 : TriggerRule(PS=1, comment='Prescaled at L1'),
    }])))

# l2msonly: disable when L>3E33 (2015.09.26)
HLT_list=['HLT_2mu6_l2msonly_bJpsimumu_noL2','HLT_mu6_mu6_l2msonly_bJpsimumu_noL2',]
bphys_Rules_1e34.update(dict(map(None,HLT_list,len(HLT_list)*[{
    1000 : TriggerRule(PS=1, comment=''), 
    3001 : TriggerRule(PS=-1, comment='disabled'), 
    }])))

# --------------------------------------

L1_list=['L1_MU6_2MU4']
bphys_Rules_1e34.update(dict(map(None,L1_list,len(L1_list)*[{
                        1000 : TriggerRule(PS=1, comment='L1'), 
    lumi_disable_l1_mu6_2mu4 : TriggerRule(rate=500, maxRate=500, comment='Prescaled'),
    }])))

HLT_list=['HLT_mu6_mu4_bBmumu','HLT_mu6_mu4_bBmumuxv2','HLT_mu6_mu4_bBmumux_BcmumuDsloose','HLT_mu6_mu4_bBmumux_BpmumuKp','HLT_mu6_mu4_bJpsimumu','HLT_mu6_mu4_bUpsimumu','HLT_mu6_mu4_bJpsimumu_noL2' ]
bphys_Rules_1e34.update(dict(map(None,HLT_list,len(HLT_list)*[{
                        1000 : TriggerRule(PS=1, comment=''), 
    lumi_disable_l1_mu6_2mu4 : TriggerRule(PS=1, comment='Prescaled at L1'),
    }])))

# l2msonly: disable when L>3E33 (2015.09.26)
HLT_list=['HLT_mu6_mu4_l2msonly_bJpsimumu_noL2','HLT_mu6_l2msonly_mu4_bJpsimumu_noL2']
bphys_Rules_1e34.update(dict(map(None,HLT_list,len(HLT_list)*[{
    1000 : TriggerRule(PS=1, comment=''), 
    3001 : TriggerRule(PS=-1, comment='disabled'), 
    }])))

# l2msonly: disabled at 2501 due to its high CPU usage (ATR-11769) 
HLT_list=['HLT_mu6_l2msonly_mu4_l2msonly_bJpsimumu_noL2']
bphys_Rules_1e34.update(dict(map(None,HLT_list,len(HLT_list)*[{
    1000 : TriggerRule(PS=1, comment=''), 
    2501 : TriggerRule(PS=-1, comment='disabled'), 
    }])))

########################################
# 0.5e34 

bphys_Rules_0p5e34={}

L1_list=['L1_2MU4']
bphys_Rules_0p5e34.update(dict(map(None,L1_list,len(L1_list)*[{
                        1000 : TriggerRule(PS=1, comment='L1'), 
        lumi_disable_l1_2mu4 : TriggerRule(rate=1000, maxRate=1000, comment='Prescaled'),
    lumi_disable_l1_mu6_2mu4 : TriggerRule(rate= 500, maxRate= 500, comment='Prescaled'),
    }])))

HLT_list=['HLT_2mu4_bBmumu','HLT_2mu4_bBmumuxv2','HLT_2mu4_bBmumux_BcmumuDsloose','HLT_2mu4_bBmumux_BpmumuKp','HLT_2mu4_bJpsimumu','HLT_2mu4_bUpsimumu','HLT_2mu4_bJpsimumu_noL2']
bphys_Rules_0p5e34.update(dict(map(None,HLT_list,len(HLT_list)*[{
                    1000 : TriggerRule(PS=1, comment=''), 
    lumi_disable_l1_2mu4 : TriggerRule(PS=1, comment='Prescaled at L1'),
    }])))

# l2msonly: disabled at 2501 due to its high CPU usage (ATR-11769)
HLT_list=['HLT_2mu4_l2msonly_bJpsimumu_noL2','HLT_mu4_mu4_l2msonly_bJpsimumu_noL2']
bphys_Rules_0p5e34.update(dict(map(None,HLT_list,len(HLT_list)*[{
    1000 : TriggerRule(PS=1, comment=''), 
    2501 : TriggerRule(PS=-1, comment='disabled'), 
    }])))

# --------------------------------------

L1_list=['L1_3MU4']
bphys_Rules_0p5e34.update(dict(map(None,L1_list,len(L1_list)*[{
                    1000 : TriggerRule(PS=1, comment='L1'), 
    lumi_disable_l1_3mu4 : TriggerRule(rate=500, maxRate=500, comment='Prescaled'),
    }])))

HLT_list=['HLT_3mu4_bJpsi','HLT_3mu4_bTau','HLT_3mu4_bUpsi']
bphys_Rules_0p5e34.update(dict(map(None,HLT_list,len(HLT_list)*[{
                    1000 : TriggerRule(PS=1, comment=''), 
    lumi_disable_l1_3mu4 : TriggerRule(PS=1, comment='Prescaled at L1'),
    }])))

########################################
# supporting trigger

bphys_Rules_supporting={}

HLT_list=['HLT_2mu4_bDimu_noEFbph', 'HLT_mu6_mu4_bDimu_noEFbph']
bphys_Rules_supporting.update(dict(map(None,HLT_list,len(HLT_list)*[{
                    1000 : TriggerRule(rate=1.0, comment=''),
    }])))


HLT_list=['HLT_2mu10_bDimu', 'HLT_2mu10_bDimu_novtx_noos', 'HLT_2mu10_bDimu_noinvm_novtx_ss', 'HLT_2mu10_bDimu_noL2']
bphys_Rules_supporting.update(dict(map(None,HLT_list,len(HLT_list)*[{
                    1000 : TriggerRule(PS=-1, comment=''),
    lumi_disable_l1_2mu6 : TriggerRule(rate=1.0, comment='1 Hz when 2MU6 is prescaled'),
    }])))

# l2msonly: disable (2015.09.26)
HLT_list=['HLT_2mu10_l2msonly_bDimu_noL2','HLT_mu10_mu10_l2msonly_bDimu_noL2']
bphys_Rules_supporting.update(dict(map(None,HLT_list,len(HLT_list)*[{
    1000 : TriggerRule(PS=-1, comment=''),
    }])))

# ----------------------------------------

HLT_list=['HLT_2mu6_bDimu', 'HLT_2mu6_bDimu_novtx_noos', 'HLT_2mu6_bDimu_noinvm_novtx_ss']
bphys_Rules_supporting.update(dict(map(None,HLT_list,len(HLT_list)*[{
                        1000 : TriggerRule(rate=0.2, comment='Express', ESValue=1),
    lumi_disable_l1_mu6_2mu4 : TriggerRule(rate=1.0, comment='1 Hz when 2MU6 is primary, Express', ESRate=0.2),
    lumi_disable_l1_2mu6     : TriggerRule(rate=0.2, comment='Express', ESValue=1),
    }])))

HLT_list=['HLT_2mu6_bDimu_noL2']
bphys_Rules_supporting.update(dict(map(None,HLT_list,len(HLT_list)*[{
                        1000 : TriggerRule(rate=0.2, comment=''),
    lumi_disable_l1_mu6_2mu4 : TriggerRule(rate=1.0, comment='1 Hz when 2MU6 is primary'),
    lumi_disable_l1_2mu6     : TriggerRule(rate=0.2, comment=''),
    }])))

# l2msonly: disable when L>3E33 (2015.09.26)
HLT_list=['HLT_2mu6_l2msonly_bDimu_noL2','HLT_mu6_mu6_l2msonly_bDimu_noL2']
bphys_Rules_supporting.update(dict(map(None,HLT_list,len(HLT_list)*[{
    1000 : TriggerRule(rate=0.2, comment=''),
    3001 : TriggerRule(PS=-1, comment='disabled'),
    }])))

# ----------------------------------------

# kn: 2015.09.08  assign 1 Hz even when 2MU4 is supposed to be primary, as 2MU4 can be prescaled by hand on the fly at P1

HLT_list=['HLT_mu6_mu4_bDimu', 'HLT_mu6_mu4_bDimu_novtx_noos', 'HLT_mu6_mu4_bDimu_noinvm_novtx_ss', 'HLT_mu6_mu4_bDimu_noL2']
bphys_Rules_supporting.update(dict(map(None,HLT_list,len(HLT_list)*[{
                        1000 : TriggerRule(rate=1.0, comment='1 Hz even when MU6_2MU4 is not primary, because 2MU4 can be prescaled by hand'),
        lumi_disable_l1_2mu4 : TriggerRule(rate=1.0, comment='1 Hz when MU6_2MU4 is primary'),
    lumi_disable_l1_mu6_2mu4 : TriggerRule(rate=0.2, comment='0.2 Hz after MU6_2MU4 is prescaled'),
    }])))

# l2msonly: disable when L>3E33 (2015.09.26)
HLT_list=['HLT_mu6_mu4_l2msonly_bDimu_noL2', 'HLT_mu6_l2msonly_mu4_bDimu_noL2','HLT_mu6_l2msonly_mu4_l2msonly_bDimu_noL2']
bphys_Rules_supporting.update(dict(map(None,HLT_list,len(HLT_list)*[{
    1000 : TriggerRule(rate=1.0, comment='1 Hz even when MU6_2MU4 is not primary, because 2MU4 can be prescaled by hand'),
    3001 : TriggerRule(PS=-1,comment='disabled'),
    }])))

# ----------------------------------------

HLT_list=['HLT_2mu4_bDimu', 'HLT_2mu4_bDimu_novtx_noos', 'HLT_2mu4_bDimu_noinvm_novtx_ss', 'HLT_2mu4_bDimu_noL2']
bphys_Rules_supporting.update(dict(map(None,HLT_list,len(HLT_list)*[{
                    1000 : TriggerRule(rate=1.0, comment='1 Hz when 2MU4 is primary'),
    lumi_disable_l1_2mu4 : TriggerRule(rate=0.2, comment='0.2 Hz after 2MU4 is prescaled'),
     }])))

# l2msonly: disable when L>3E33 (2015.09.26)
HLT_list=['HLT_2mu4_l2msonly_bDimu_noL2','HLT_mu4_mu4_l2msonly_bDimu_noL2']
bphys_Rules_supporting.update(dict(map(None,HLT_list,len(HLT_list)*[{
    1000 : TriggerRule(rate=1.0, comment='1 Hz when 2MU4 is primary'),
    3001 : TriggerRule(PS=-1, comment='disabled'),
    }])))

# ----------------------------------------

HLT_list=['HLT_3mu4_bDimu']
bphys_Rules_supporting.update(dict(map(None,HLT_list,len(HLT_list)*[{
                    1000 : TriggerRule(rate=1.0, comment='1 Hz when 3MU4 is primary'),
    lumi_disable_l1_3mu4 : TriggerRule(rate=0.2, comment='0.2 Hz after 2MU4 is prescaled'),
    }])))

HLT_list=['HLT_3mu6_bDimu']
bphys_Rules_supporting.update(dict(map(None,HLT_list,len(HLT_list)*[{
                    1000 : TriggerRule(PS=-1, comment='Disabled'),
    lumi_disable_l1_3mu4 : TriggerRule(rate=1.0, comment='1 Hz when 3MU6 is primary'),
    }])))

# ========================================

bphys_Rules_supporting.update({
    # ----------------------------------------
    # di-muons w/o cuts
    'HLT_2mu4'                           : {  1000 : TriggerRule(rate=0.2, comment='') },
    'HLT_2mu6'                           : {  1000 : TriggerRule(rate=0.2, comment='') },

    # ----------------------------------------
    # idperf at Jpsi mumu
    # 1.5 Hz @ Run1
    'HLT_mu4_mu4_idperf_bJpsimumu_noid'  : {  1000 : TriggerRule(rate=1.5, comment='Express', ESRate=0.1) },

    # ----------------------------------------
    # Jpsi Trkloose
    # 4 Hz for mu6 @ Run1 -> raised to 20 Hz
    'HLT_mu4_bJpsi_Trkloose'             : {  1000 : TriggerRule(rate=40.0, comment=''),
                                              3000 : TriggerRule(rate=20.0, comment='')},
    'HLT_mu6_bJpsi_Trkloose'             : {  1000 : TriggerRule(rate=2.0, comment='') },
    'HLT_mu10_bJpsi_Trkloose'            : {  1000 : TriggerRule(rate=2.0, comment='') },
    'HLT_mu18_bJpsi_Trkloose'            : {  1000 : TriggerRule(rate=2.0, comment='') },
})

########################################

bphys_Rules={}

RulesList=[bphys_Rules_2e34, bphys_Rules_1p5e34, bphys_Rules_1e34, bphys_Rules_0p5e34, bphys_Rules_supporting] 
for Rules in RulesList:
    for newrule in Rules.keys():
        if bphys_Rules.has_key(newrule):
            print 'FATAL     Physics_pp_v5_rules     Duplicated rule inside physics rule. Cannot be added:',newrule
    bphys_Rules.update(Rules)


#################################################################################################
#
#
# Missing ET
#
#
#################################################################################################

# kn: 2015/9/8  put back to factor=1 as the cost input from 25ns EB run is ok for BGRP7 rates
# --- need it for the moment as input cost is based on EB run with BGRP7 was wrong
# bgrp7_factor=30
bgrp7_factor=1


########################################
# 2e34 

met_Rules_2e34={}

L1_list=['L1_XE70', 'L1_XE80']
met_Rules_2e34.update(dict(map(None,L1_list,len(L1_list)*[{
    1000 : TriggerRule(PS=1, comment='L1'), 
    }])))

# OAB decision 19/10 to save HLT output rate
L1_list=['L1_XE70_BGRP7', 'L1_XE80_BGRP7']
met_Rules_2e34.update(dict(map(None,L1_list,len(L1_list)*[{
    1000 : TriggerRule(PS=-1, comment='Disabled'), 
    }])))

HLT_list=[
    #
    # XE70
    'HLT_xe100',      'HLT_xe100_mht',  'HLT_xe100_pueta',
    'HLT_xe100_pufit','HLT_xe100_tc_em','HLT_xe100_tc_lcw',
    'HLT_xe100_wEFMu',      'HLT_xe100_mht_wEFMu',  'HLT_xe100_pueta_wEFMu',
    'HLT_xe100_pufit_wEFMu','HLT_xe100_tc_em_wEFMu','HLT_xe100_tc_lcw_wEFMu',
    #
    # XE70
    'HLT_xe90',      'HLT_xe90_mht',  'HLT_xe90_pueta',
    'HLT_xe90_pufit','HLT_xe90_tc_em','HLT_xe90_tc_lcw',
    'HLT_xe90_wEFMu',      'HLT_xe90_mht_wEFMu',  'HLT_xe90_pueta_wEFMu',
    'HLT_xe90_pufit_wEFMu','HLT_xe90_tc_em_wEFMu','HLT_xe90_tc_lcw_wEFMu',
    # XE80
    'HLT_xe100_L1XE80',      'HLT_xe100_mht_L1XE80',  'HLT_xe100_pueta_L1XE80',
    'HLT_xe100_pufit_L1XE80','HLT_xe100_tc_em_L1XE80','HLT_xe100_tc_lcw_L1XE80',
    'HLT_xe100_wEFMu_L1XE80',      'HLT_xe100_mht_wEFMu_L1XE80',  'HLT_xe100_pueta_wEFMu_L1XE80',
    'HLT_xe100_pufit_wEFMu_L1XE80','HLT_xe100_tc_em_wEFMu_L1XE80','HLT_xe100_tc_lcw_wEFMu_L1XE80',
    #
    'HLT_xe120',      'HLT_xe120_mht',  'HLT_xe120_pueta',
    'HLT_xe120_pufit','HLT_xe120_tc_em','HLT_xe120_tc_lcw',
    'HLT_xe120_wEFMu',      'HLT_xe120_mht_wEFMu',  'HLT_xe120_pueta_wEFMu',
    'HLT_xe120_pufit_wEFMu','HLT_xe120_tc_em_wEFMu','HLT_xe120_tc_lcw_wEFMu',
    #
    # XE70_BGRP7
    'HLT_xe90_L1XE70_BGRP7',      'HLT_xe90_mht_L1XE70_BGRP7',  'HLT_xe90_pueta_L1XE70_BGRP7',
    'HLT_xe90_pufit_L1XE70_BGRP7','HLT_xe90_tc_em_L1XE70_BGRP7','HLT_xe90_tc_lcw_L1XE70_BGRP7',
    'HLT_xe90_wEFMu_L1XE70_BGRP7',      'HLT_xe90_mht_wEFMu_L1XE70_BGRP7',  'HLT_xe90_pueta_wEFMu_L1XE70_BGRP7',
    'HLT_xe90_pufit_wEFMu_L1XE70_BGRP7','HLT_xe90_tc_em_wEFMu_L1XE70_BGRP7','HLT_xe90_tc_lcw_wEFMu_L1XE70_BGRP7',
    # XE70_BGRP7
    'HLT_xe100_L1XE70_BGRP7',      'HLT_xe100_mht_L1XE70_BGRP7',  'HLT_xe100_pueta_L1XE70_BGRP7',
    'HLT_xe100_pufit_L1XE70_BGRP7','HLT_xe100_tc_em_L1XE70_BGRP7','HLT_xe100_tc_lcw_L1XE70_BGRP7',
    'HLT_xe100_wEFMu_L1XE70_BGRP7',      'HLT_xe100_mht_wEFMu_L1XE70_BGRP7',  'HLT_xe100_pueta_wEFMu_L1XE70_BGRP7',
    'HLT_xe100_pufit_wEFMu_L1XE70_BGRP7','HLT_xe100_tc_em_wEFMu_L1XE70_BGRP7','HLT_xe100_tc_lcw_wEFMu_L1XE70_BGRP7',
    #
    # XE80_BGRP7
    'HLT_xe100_L1XE80_BGRP7',      'HLT_xe100_mht_L1XE80_BGRP7',  'HLT_xe100_pueta_L1XE80_BGRP7',
    'HLT_xe100_pufit_L1XE80_BGRP7','HLT_xe100_tc_em_L1XE80_BGRP7','HLT_xe100_tc_lcw_L1XE80_BGRP7',
    'HLT_xe100_wEFMu_L1XE80_BGRP7',      'HLT_xe100_mht_wEFMu_L1XE80_BGRP7',  'HLT_xe100_pueta_wEFMu_L1XE80_BGRP7',
    'HLT_xe100_pufit_wEFMu_L1XE80_BGRP7','HLT_xe100_tc_em_wEFMu_L1XE80_BGRP7','HLT_xe100_tc_lcw_wEFMu_L1XE80_BGRP7',
    #
    'HLT_xe120_L1XE80_BGRP7',      'HLT_xe120_mht_L1XE80_BGRP7',  'HLT_xe120_pueta_L1XE80_BGRP7', 
    'HLT_xe120_pufit_L1XE80_BGRP7','HLT_xe120_tc_em_L1XE80_BGRP7','HLT_xe120_tc_lcw_L1XE80_BGRP7',
    'HLT_xe120_wEFMu_L1XE80_BGRP7',      'HLT_xe120_mht_wEFMu_L1XE80_BGRP7',  'HLT_xe120_pueta_wEFMu_L1XE80_BGRP7',
    'HLT_xe120_pufit_wEFMu_L1XE80_BGRP7','HLT_xe120_tc_em_wEFMu_L1XE80_BGRP7','HLT_xe120_tc_lcw_wEFMu_L1XE80_BGRP7',
    ]
met_Rules_2e34.update(dict(map(None,HLT_list,len(HLT_list)*[{
    1000 : TriggerRule(PS=1, comment=''), 
    }])))

########################################
# 1.5e34 

met_Rules_1p5e34={}

HLT_list=[
    # XE70
    'HLT_xe80_L1XE70',      'HLT_xe80_mht_L1XE70',  'HLT_xe80_pueta_L1XE70',
    'HLT_xe80_pufit_L1XE70','HLT_xe80_tc_em_L1XE70','HLT_xe80_tc_lcw_L1XE70',
    'HLT_xe80_wEFMu_L1XE70',      'HLT_xe80_mht_wEFMu_L1XE70',  'HLT_xe80_pueta_wEFMu_L1XE70',
    'HLT_xe80_pufit_wEFMu_L1XE70','HLT_xe80_tc_em_wEFMu_L1XE70','HLT_xe80_tc_lcw_wEFMu_L1XE70',
    ]
met_Rules_1p5e34.update(dict(map(None,HLT_list,len(HLT_list)*[{
    1000 : TriggerRule(PS=1, comment=''), 
   15001 : TriggerRule(rate=0.5, comment='Prescaled'),
    }])))

HLT_list=[
    # XE70_BGRP7
    'HLT_xe80_L1XE70_BGRP7',      'HLT_xe80_mht_L1XE70_BGRP7',  'HLT_xe80_pueta_L1XE70_BGRP7',
    'HLT_xe80_pufit_L1XE70_BGRP7','HLT_xe80_tc_em_L1XE70_BGRP7','HLT_xe80_tc_lcw_L1XE70_BGRP7',
    'HLT_xe80_wEFMu_L1XE70_BGRP7',      'HLT_xe80_mht_wEFMu_L1XE70_BGRP7',  'HLT_xe80_tc_lcw_wEFMu_L1XE70_BGRP7',
    'HLT_xe80_pueta_wEFMu_L1XE70_BGRP7','HLT_xe80_pufit_wEFMu_L1XE70_BGRP7','HLT_xe80_tc_em_wEFMu_L1XE70_BGRP7',
    ]
met_Rules_1p5e34.update(dict(map(None,HLT_list,len(HLT_list)*[{
    1000 : TriggerRule(PS=1, comment=''), 
   15001 : TriggerRule(rate=0.5/bgrp7_factor, comment='Prescaled'),
    }])))

########################################
# 1e34 

met_Rules_1e34={}

L1_list=['L1_XE60']
met_Rules_1e34.update(dict(map(None,L1_list,len(L1_list)*[{
                   1000 : TriggerRule(PS=1, comment='L1'), 
   lumi_disable_l1_xe60 : TriggerRule(rate=500, maxRate=500, comment='Prescaled'),
    }])))
L1_list=['L1_XE60_BGRP7']
met_Rules_1e34.update(dict(map(None,L1_list,len(L1_list)*[{
                   1000 : TriggerRule(PS=-1, comment='not needed anymore, save HLT rate'), 
# OAB 19/10 decision, to save HLT output rate
#                   1000 : TriggerRule(PS=1, comment='L1'), 
#   lumi_disable_l1_xe60 : TriggerRule(rate=500/bgrp7_factor, maxRate=500/bgrp7_factor, comment='Prescaled'),
    }])))

HLT_list=[
    # XE60
    'HLT_xe80',      'HLT_xe80_mht',      'HLT_xe80_pueta',      'HLT_xe80_pufit',      'HLT_xe80_tc_em',      'HLT_xe80_tc_lcw',
    'HLT_xe80_wEFMu','HLT_xe80_mht_wEFMu','HLT_xe80_pueta_wEFMu','HLT_xe80_pufit_wEFMu','HLT_xe80_tc_em_wEFMu','HLT_xe80_tc_lcw_wEFMu',
    ]
met_Rules_1e34.update(dict(map(None,HLT_list,len(HLT_list)*[{
                    1000 : TriggerRule(PS=1, comment=''), 
    lumi_disable_l1_xe60 : TriggerRule(rate=0.5, comment='Prescaled'),
    }])))

HLT_list=[
    # XE60_BGRP7
    'HLT_xe80_L1XE60_BGRP7',      'HLT_xe80_mht_L1XE60_BGRP7',  'HLT_xe80_pueta_L1XE60_BGRP7',
    'HLT_xe80_pufit_L1XE60_BGRP7','HLT_xe80_tc_em_L1XE60_BGRP7','HLT_xe80_tc_lcw_L1XE60_BGRP7',
    'HLT_xe80_wEFMu_L1XE60_BGRP7',      'HLT_xe80_mht_wEFMu_L1XE60_BGRP7',  'HLT_xe80_pueta_wEFMu_L1XE60_BGRP7',
    'HLT_xe80_pufit_wEFMu_L1XE60_BGRP7','HLT_xe80_tc_em_wEFMu_L1XE60_BGRP7','HLT_xe80_tc_lcw_wEFMu_L1XE60_BGRP7',
    ]
met_Rules_1e34.update(dict(map(None,HLT_list,len(HLT_list)*[{
                    1000 : TriggerRule(PS=1, comment=''), 
    lumi_disable_l1_xe60 : TriggerRule(rate=0.5/bgrp7_factor, comment='Prescaled'),
    }])))

########################################
# 0.5e34 

met_Rules_0p5e34={}

L1_list=['L1_XE50']
met_Rules_0p5e34.update(dict(map(None,L1_list,len(L1_list)*[{
                    1000 : TriggerRule(PS=1, comment='L1'), 
    lumi_disable_l1_xe50 : TriggerRule(rate=500, maxRate=500, comment='Prescaled'),
    }])))
L1_list=['L1_XE50_BGRP7']
met_Rules_0p5e34.update(dict(map(None,L1_list,len(L1_list)*[{
                    1000 : TriggerRule(PS=-1, comment='Disables'), 
# OAB 19/10 decision, to save HLT output rate
#                    1000 : TriggerRule(PS=1, comment='L1'), 
#    lumi_disable_l1_xe50 : TriggerRule(rate=500/bgrp7_factor, maxRate=500/bgrp7_factor, comment='Prescaled'),
    }])))

HLT_list=[
    # XE50
    'HLT_xe70',      'HLT_xe70_mht',      'HLT_xe70_pueta',      'HLT_xe70_pufit',      'HLT_xe70_tc_em',      'HLT_xe70_tc_lcw',
    'HLT_xe70_wEFMu','HLT_xe70_mht_wEFMu','HLT_xe70_pueta_wEFMu','HLT_xe70_pufit_wEFMu','HLT_xe70_tc_em_wEFMu','HLT_xe70_tc_lcw_wEFMu',
]
met_Rules_0p5e34.update(dict(map(None,HLT_list,len(HLT_list)*[{
                    1000 : TriggerRule(PS=1, comment=''), 
    lumi_disable_l1_xe50 : TriggerRule(rate=0.5, comment='Prescaled'),
    }])))
HLT_list=[
    # XE50_BGRP7
    'HLT_xe70_L1XE50_BGRP7',      'HLT_xe70_mht_L1XE50_BGRP7',  'HLT_xe70_pueta_L1XE50_BGRP7',
    'HLT_xe70_pufit_L1XE50_BGRP7','HLT_xe70_tc_em_L1XE50_BGRP7','HLT_xe70_tc_lcw_L1XE50_BGRP7',
    'HLT_xe70_wEFMu_L1XE50_BGRP7',      'HLT_xe70_mht_wEFMu_L1XE50_BGRP7', 'HLT_xe70_pueta_wEFMu_L1XE50_BGRP7',
    'HLT_xe70_pufit_wEFMu_L1XE50_BGRP7','HLT_xe70_tc_em_wEFMu_L1XE50_BGRP7','HLT_xe70_tc_lcw_wEFMu_L1XE50_BGRP7',
]
met_Rules_0p5e34.update(dict(map(None,HLT_list,len(HLT_list)*[{
                    1000 : TriggerRule(PS=1, comment=''), 
    lumi_disable_l1_xe50 : TriggerRule(rate=0.5/bgrp7_factor, comment='Prescaled'),
    }])))

HLT_list=[
    # XE50
    'HLT_xe80_L1XE50',      'HLT_xe80_mht_L1XE50',  'HLT_xe80_pueta_L1XE50',
    'HLT_xe80_pufit_L1XE50','HLT_xe80_tc_em_L1XE50','HLT_xe80_tc_lcw_L1XE50',
    'HLT_xe80_wEFMu_L1XE50',      'HLT_xe80_mht_wEFMu_L1XE50',  'HLT_xe80_pueta_wEFMu_L1XE50',
    'HLT_xe80_pufit_wEFMu_L1XE50','HLT_xe80_tc_em_wEFMu_L1XE50','HLT_xe80_tc_lcw_wEFMu_L1XE50',
    ]
met_Rules_0p5e34.update(dict(map(None,HLT_list,len(HLT_list)*[{
                    1000 : TriggerRule(PS=1, comment=''), 
    lumi_disable_l1_xe50 : TriggerRule(rate=0.5, comment='Prescaled'),
    }])))


HLT_list=[
    # XE50
    'HLT_xe90_L1XE50',      'HLT_xe90_mht_L1XE50',  'HLT_xe90_pueta_L1XE50',
    'HLT_xe90_pufit_L1XE50','HLT_xe90_tc_em_L1XE50','HLT_xe90_tc_lcw_L1XE50',
    'HLT_xe90_wEFMu_L1XE50',      'HLT_xe90_mht_wEFMu_L1XE50',  'HLT_xe90_pueta_wEFMu_L1XE50',
    'HLT_xe90_pufit_wEFMu_L1XE50','HLT_xe90_tc_em_wEFMu_L1XE50','HLT_xe90_tc_lcw_wEFMu_L1XE50',
    ]
met_Rules_0p5e34.update(dict(map(None,HLT_list,len(HLT_list)*[{
                    1000 : TriggerRule(PS=1, comment=''), 
    lumi_disable_l1_xe50 : TriggerRule(rate=0.5, comment='Prescaled'),
    }])))

HLT_list=[
    # XE50
    'HLT_xe100_L1XE50',      'HLT_xe100_mht_L1XE50',  'HLT_xe100_pueta_L1XE50',
    'HLT_xe100_pufit_L1XE50','HLT_xe100_tc_em_L1XE50','HLT_xe100_tc_lcw_L1XE50',
    'HLT_xe100_wEFMu_L1XE50',      'HLT_xe100_mht_wEFMu_L1XE50',  'HLT_xe100_pueta_wEFMu_L1XE50',
    'HLT_xe100_pufit_wEFMu_L1XE50','HLT_xe100_tc_em_wEFMu_L1XE50','HLT_xe100_tc_lcw_wEFMu_L1XE50',
    ]
met_Rules_0p5e34.update(dict(map(None,HLT_list,len(HLT_list)*[{
                    1000 : TriggerRule(PS=1, comment=''), 
    lumi_disable_l1_xe50 : TriggerRule(rate=0.5, comment='Prescaled'),
    }])))



HLT_list=[
    # XE50_BGRR7
    'HLT_xe80_L1XE50_BGRP7',      'HLT_xe80_mht_L1XE50_BGRP7',  'HLT_xe80_pueta_L1XE50_BGRP7',
    'HLT_xe80_pufit_L1XE50_BGRP7','HLT_xe80_tc_em_L1XE50_BGRP7','HLT_xe80_tc_lcw_L1XE50_BGRP7',
    'HLT_xe80_wEFMu_L1XE50_BGRP7',      'HLT_xe80_mht_wEFMu_L1XE50_BGRP7',  'HLT_xe80_pueta_wEFMu_L1XE50_BGRP7',
    'HLT_xe80_pufit_wEFMu_L1XE50_BGRP7','HLT_xe80_tc_em_wEFMu_L1XE50_BGRP7','HLT_xe80_tc_lcw_wEFMu_L1XE50_BGRP7',
    ]
met_Rules_0p5e34.update(dict(map(None,HLT_list,len(HLT_list)*[{
                    1000 : TriggerRule(PS=1, comment=''), 
    lumi_disable_l1_xe50 : TriggerRule(rate=0.5/bgrp7_factor, comment='Prescaled'),
    }])))

########################################
# supporting trigger

met_Rules_supporting={}

L1_list=['L1_XE35', 'L1_XE40']
met_Rules_supporting.update(dict(map(None,L1_list,len(L1_list)*[{
    1000 : TriggerRule(rate=100, maxRate=100, comment='L1'), 
    }])))
L1_list=['L1_XE35_BGRP7', 'L1_XE40_BGRP7']
met_Rules_supporting.update(dict(map(None,L1_list,len(L1_list)*[{
    1000 : TriggerRule(PS=-1, comment='Disabled'), 
# OAB 19/10 decision, to save HLT output rate
#    1000 : TriggerRule(rate=100/bgrp7_factor, maxRate=100/bgrp7_factor, comment='L1'), 
    }])))

HLT_list=[
    # XE35
    'HLT_xe35',      'HLT_xe35_mht',      'HLT_xe35_pueta',      'HLT_xe35_pufit',      'HLT_xe35_tc_em',      'HLT_xe35_tc_lcw',
    'HLT_xe35_wEFMu','HLT_xe35_mht_wEFMu','HLT_xe35_pueta_wEFMu','HLT_xe35_pufit_wEFMu','HLT_xe35_tc_em_wEFMu','HLT_xe35_tc_lcw_wEFMu',
    ]
met_Rules_supporting.update(dict(map(None,HLT_list,len(HLT_list)*[{
    1000 : TriggerRule(rate=2.0, comment=''), 
    }])))
HLT_list=[
    # XE35_BGRP7
    'HLT_xe35_L1XE35_BGRP7',      'HLT_xe35_mht_L1XE35_BGRP7',  'HLT_xe35_pueta_L1XE35_BGRP7',
    'HLT_xe35_pufit_L1XE35_BGRP7','HLT_xe35_tc_em_L1XE35_BGRP7','HLT_xe35_tc_lcw_L1XE35_BGRP7',
    'HLT_xe35_wEFMu_L1XE35_BGRP7',      'HLT_xe35_mht_wEFMu_L1XE35_BGRP7',  'HLT_xe35_pueta_wEFMu_L1XE35_BGRP7',
    'HLT_xe35_pufit_wEFMu_L1XE35_BGRP7','HLT_xe35_tc_em_wEFMu_L1XE35_BGRP7','HLT_xe35_tc_lcw_wEFMu_L1XE35_BGRP7',
    ]
met_Rules_supporting.update(dict(map(None,HLT_list,len(HLT_list)*[{
    1000 : TriggerRule(rate=2.0/bgrp7_factor, comment=''), 
    }])))

HLT_list=[
    # XE35
    'HLT_xe50_mht', 'HLT_xe50_pueta', 'HLT_xe50_pufit', 'HLT_xe50_tc_em', 'HLT_xe50_tc_lcw',
    ]
met_Rules_supporting.update(dict(map(None,HLT_list,len(HLT_list)*[{
    1000 : TriggerRule(rate=1.0, comment=''), 
    }])))
HLT_list=[
    # XE35_BGRP7
    'HLT_xe50_L1XE35_BGRP7',      'HLT_xe50_mht_L1XE35_BGRP7',  'HLT_xe50_pueta_L1XE35_BGRP7',
    'HLT_xe50_pufit_L1XE35_BGRP7','HLT_xe50_tc_em_L1XE35_BGRP7','HLT_xe50_tc_lcw_L1XE35_BGRP7'
    ]
met_Rules_supporting.update(dict(map(None,HLT_list,len(HLT_list)*[{
    1000 : TriggerRule(rate=1.0/bgrp7_factor, comment=''), 
    }])))

HLT_list=['HLT_xe50']  # rerun needed for tau (ATR-11530)
met_Rules_supporting.update(dict(map(None,HLT_list,len(HLT_list)*[{
    1000 : TriggerRule(rate=1.0, comment='', rerun=1), 
    }])))

HLT_list=['HLT_xe70_L1XE45']  # rerun needed for tau (ATR-11530)
met_Rules_supporting.update(dict(map(None,HLT_list,len(HLT_list)*[{
    1000 : TriggerRule(PS=0, comment='', rerun=1), 
    }])))

# ----------------------------------------

HLT_list=[
    # XE40
    'HLT_xe60',      'HLT_xe60_mht',      'HLT_xe60_pueta',      'HLT_xe60_pufit',      'HLT_xe60_tc_em',      'HLT_xe60_tc_lcw',
    'HLT_xe60_wEFMu','HLT_xe60_mht_wEFMu','HLT_xe60_pueta_wEFMu','HLT_xe60_pufit_wEFMu','HLT_xe60_tc_em_wEFMu','HLT_xe60_tc_lcw_wEFMu',
    ]
met_Rules_supporting.update(dict(map(None,HLT_list,len(HLT_list)*[{
    1000 : TriggerRule(rate=1.0, comment=''), 
    }])))
HLT_list=[
    # XE40_BGRP7
    'HLT_xe60_L1XE40_BGRP7',      'HLT_xe60_mht_L1XE40_BGRP7',  'HLT_xe60_pueta_L1XE40_BGRP7',
    'HLT_xe60_pufit_L1XE40_BGRP7','HLT_xe60_tc_em_L1XE40_BGRP7','HLT_xe60_tc_lcw_L1XE40_BGRP7',
    'HLT_xe60_wEFMu_L1XE40_BGRP7',      'HLT_xe60_mht_wEFMu_L1XE40_BGRP7',  'HLT_xe60_pueta_wEFMu_L1XE40_BGRP7',
    'HLT_xe60_pufit_wEFMu_L1XE40_BGRP7','HLT_xe60_tc_em_wEFMu_L1XE40_BGRP7','HLT_xe60_tc_lcw_wEFMu_L1XE40_BGRP7',
    ]
met_Rules_supporting.update(dict(map(None,HLT_list,len(HLT_list)*[{
    1000 : TriggerRule(rate=1.0/bgrp7_factor, comment=''), 
    }])))

# ========================================

HLT_list=[
    'HLT_xe0noL1_l2fsperf',       'HLT_xe0noL1_l2fsperf_mht',   'HLT_xe0noL1_l2fsperf_pueta',
    'HLT_xe0noL1_l2fsperf_pufit', 'HLT_xe0noL1_l2fsperf_tc_em', 'HLT_xe0noL1_l2fsperf_tc_lcw',
    ]
met_Rules_supporting.update(dict(map(None,HLT_list,len(HLT_list)*[{
    1000 : TriggerRule(PS=0, comment='',rerun=1), 
    }])))

# ========================================

HLT_list=['HLT_noalg_L1XE35']
met_Rules_supporting.update(dict(map(None,HLT_list,len(HLT_list)*[{
    1000 : TriggerRule(inputRate=0.5, comment='Express', ESValue=1), 
    }])))

# ========================================

HLT_list=['HLT_noalg_L1RD0_FILLED']
met_Rules_supporting.update(dict(map(None,HLT_list,len(HLT_list)*[{
    1000 : TriggerRule(inputRate=15, comment='',ESRate=0.1), 
    }])))

HLT_list=['HLT_noalg_L1RD0_EMPTY']
met_Rules_supporting.update(dict(map(None,HLT_list,len(HLT_list)*[{
    1000 : TriggerRule(rate=0.5, comment='express',ESRate=0.5), 
    }])))


########################################

met_Rules={}

RulesList=[met_Rules_2e34, met_Rules_1p5e34, met_Rules_1e34, met_Rules_0p5e34, met_Rules_supporting] 
for Rules in RulesList:
    for newrule in Rules.keys():
        if met_Rules.has_key(newrule):
            print 'FATAL     Physics_pp_v5_rules     Duplicated rule inside physics rule. Cannot be added:',newrule
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

L1_list=['L1_J100','L1_J100.31ETA49','L1_3J50','L1_4J20','L1_5J15.0ETA25','L1_6J15']
jet_Rules_2e34.update(dict(map(None,L1_list,len(L1_list)*[{
    1000 : TriggerRule(PS=1, comment='L1'), 
    }])))

HLT_list=[
    # 1jet
               'HLT_j400_jes','HLT_j400_lcw','HLT_j400_lcw_jes','HLT_j400_nojcalib','HLT_j400_sub',
    'HLT_j420','HLT_j420_jes','HLT_j420_lcw','HLT_j420_lcw_jes','HLT_j420_nojcalib','HLT_j420_lcw_nojcalib',
    'HLT_j440',
    'HLT_j460',
    # reclustered jet
    'HLT_j460_a10r_L1J100',
    # Fat jet
    'HLT_j460_a10_nojcalib_L1J100','HLT_j460_a10_lcw_sub_L1J100','HLT_j460_a10_lcw_nojcalib_L1J100',
    # Forward jet
    'HLT_j360_320eta490_jes','HLT_j360_320eta490_lcw','HLT_j360_320eta490_lcw_jes','HLT_j360_320eta490_lcw_nojcalib','HLT_j360_320eta490_nojcalib',
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

HLT_list=['HLT_noalg_L1J400',]
jet_Rules_2e34.update(dict(map(None,HLT_list,len(HLT_list)*[{
    1000 : TriggerRule(PS=1, comment='Express', ESRate=0.4), 
    }])))

# ----------------------------------------

HLT_list=['HLT_j460_a10_sub_L1J100']
jet_Rules_2e34.update(dict(map(None,HLT_list,len(HLT_list)*[{
    1000 : TriggerRule(PS=1, comment='Express', ESRate=0.2), 
    }])))
# ----------------------------------------

HLT_list=['HLT_j400', 'HLT_j360_320eta490']
jet_Rules_2e34.update(dict(map(None,HLT_list,len(HLT_list)*[{
    1000 : TriggerRule(PS=1, comment='pass-through 1Hz (ATR-11862)', PTRate=1.0), 
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

L1_list=['L1_J75','L1_J75.31ETA49']
jet_Rules_0p5e34.update(dict(map(None,L1_list,len(L1_list)*[{
                   1000 : TriggerRule(PS=1, comment='L1'), 
    lumi_disable_l1_j75 : TriggerRule(rate=100, maxRate=100, comment='Prescaled'),
    }])))

HLT_list=[
    # forward jet
    'HLT_j260_320eta490','HLT_j260_320eta490_jes','HLT_j260_320eta490_lcw','HLT_j260_320eta490_lcw_jes','HLT_j260_320eta490_lcw_nojcalib','HLT_j260_320eta490_nojcalib',
    # ht
    'HLT_ht850_L1J75', 'HLT_ht850_L1J100',
    ]
jet_Rules_0p5e34.update(dict(map(None,HLT_list,len(HLT_list)*[{
                   1000 : TriggerRule(PS=1, comment=''), 
    lumi_disable_l1_j75 : TriggerRule(PS=-1, comment='Disabled'),
    }])))

# ----------------------------------------

L1_list=['L1_3J40']
jet_Rules_0p5e34.update(dict(map(None,L1_list,len(L1_list)*[{
                    1000 : TriggerRule(PS=1, comment='L1'), 
    lumi_disable_l1_3j40 : TriggerRule(PS=-1, comment='Disabled'),
    }])))

HLT_list=['HLT_4j85','HLT_4j85_jes','HLT_4j85_lcw','HLT_4j85_lcw_jes','HLT_4j85_lcw_nojcalib','HLT_4j85_nojcalib',]
jet_Rules_0p5e34.update(dict(map(None,HLT_list,len(HLT_list)*[{
    1000 : TriggerRule(PS=1, comment=''), 
    lumi_disable_l1_3j40 : TriggerRule(rate=1.0, comment='Prescaled'),
    }])))

# ----------------------------------------

L1_list=['L1_4J15',]
jet_Rules_0p5e34.update(dict(map(None,L1_list,len(L1_list)*[{
    1000 : TriggerRule(PS=1, comment='L1'), 
    lumi_disable_l1_4j15 : TriggerRule(rate=2000, maxRate=2000, comment='Used for beamspot'),
    }])))
HLT_list=['HLT_5j60',]
jet_Rules_0p5e34.update(dict(map(None,HLT_list,len(HLT_list)*[{
    1000 : TriggerRule(PS=1, comment=''), 
    lumi_disable_l1_4j15 : TriggerRule(rate=1.0, comment='Prescaled'),
    }])))
HLT_list=['HLT_6j45','HLT_6j45_0eta240','HLT_6j45_0eta240_L14J20','HLT_6j45_0eta240_L15J150ETA25',]
jet_Rules_0p5e34.update(dict(map(None,HLT_list,len(HLT_list)*[{
    1000 : TriggerRule(PS=1, comment=''), 
    lumi_disable_l1_4j15 : TriggerRule(PS=-1, comment='Disabled'),
    }])))

# ----------------------------------------

HLT_list=['HLT_j360_a10r_L1J100',]
jet_Rules_0p5e34.update(dict(map(None,HLT_list,len(HLT_list)*[{
                          1000 : TriggerRule(PS=1, comment=''), 
    lumi_disable_hlt_j360_a10r : TriggerRule(PS=-1, comment='Disabled'),
    }])))

HLT_list=['HLT_j360']
jet_Rules_0p5e34.update(dict(map(None,HLT_list,len(HLT_list)*[{
                     1000 : TriggerRule(PS=1, comment='Express', ESRate=0.2), 
    lumi_disable_hlt_j360 : TriggerRule(rate=0.2, comment='Express', ESValue=1),
    }])))

########################################
# 0p15e34 

jet_Rules_0p15e34={}

HLT_list=['HLT_ht700_L1J75', 'HLT_ht700_L1J100']
jet_Rules_0p15e34.update(dict(map(None,HLT_list,len(HLT_list)*[{
    1000 : TriggerRule(PS=1, comment=''), 
    1501 : TriggerRule(PS=-1, comment='Disabled'),
    }])))

########################################
# prescaled 

jet_Rules_supporting = {
    'L1_RD0_FILLED'  : { 1000 : TriggerRule(rate=1000, maxRate=1000, scaling='bunches', comment='L1') },
    }

# ========================================
# prescaled single jets

L1_list=['L1_J15', 'L1_J20', 'L1_J25', 'L1_J30', 'L1_J40', 'L1_J50', 'L1_J85']
jet_Rules_supporting.update(dict(map(None,L1_list,len(L1_list)*[{
    1000 : TriggerRule(rate=200, maxRate=200, comment='Prescaled'),
    }])))

HLT_list=[
    # RD0
    #'HLT_j15',
    #'HLT_j35',
    'HLT_j35_jes','HLT_j35_lcw','HLT_j35_lcw_jes','HLT_j35_lcw_nojcalib','HLT_j35_nojcalib',
    'HLT_j45_L1RD0_FILLED',
    'HLT_j55_L1RD0_FILLED',
    'HLT_j85_L1RD0_FILLED',
    # J15
    #'HLT_j45',
    #'HLT_j55',
    # J20
    #'HLT_j85',
    'HLT_j85_jes','HLT_j85_lcw','HLT_j85_lcw_jes','HLT_j85_lcw_nojcalib','HLT_j85_nojcalib',
    # J25
    'HLT_j100',
    # J30
    #'HLT_j110',
    # J40
    'HLT_j150',
    # J50
    'HLT_j175','HLT_j175_jes','HLT_j175_lcw','HLT_j175_lcw_jes','HLT_j175_lcw_nojcalib','HLT_j175_nojcalib',
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

HLT_list=['HLT_j15',]
jet_Rules_supporting.update(dict(map(None,HLT_list,len(HLT_list)*[{
    1000 : TriggerRule(rate=4.0, comment='', PTRate=0.5), 
    }])))

HLT_list=[
    # RD0
    #'HLT_j15',
    'HLT_j35',
    # J15
    'HLT_j45',
    'HLT_j55',
    # J20
    'HLT_j85',
    # J30
    'HLT_j110',
    # J15.31ETA49
    'HLT_j45_320eta490','HLT_j55_320eta490',
    # J20.31ETA49
    # J30.31ETA49
#    'HLT_j110_320eta490',
    ]
jet_Rules_supporting.update(dict(map(None,HLT_list,len(HLT_list)*[{
    1000 : TriggerRule(rate=1.0, comment=''), 
    }])))

HLT_list=['HLT_j110_320eta490']
jet_Rules_supporting.update(dict(map(None,HLT_list,len(HLT_list)*[{
    1000 : TriggerRule(rate=1.0, comment=''),
    }])))

HLT_list=['HLT_j60']
jet_Rules_supporting.update(dict(map(None,HLT_list,len(HLT_list)*[{
    1000 : TriggerRule(rate=1.0, comment='Express', ESRate=0.2), 
    }])))

HLT_list=['HLT_j60_L1RD0_FILLED',]
jet_Rules_supporting.update(dict(map(None,HLT_list,len(HLT_list)*[{
    1000 : TriggerRule(rate=1.0, comment='Express', ESRate=0.2), 
    }])))

HLT_list=['HLT_j25']
jet_Rules_supporting.update(dict(map(None,HLT_list,len(HLT_list)*[{
    1000 : TriggerRule(rate=4.0, comment='Express', ESRate=0.8),
    }])))

# ----------------------------------------

L1_list=['L1_J15.28ETA31', 'L1_J15.31ETA49', 'L1_J20.28ETA31', 'L1_J50.31ETA49',]
jet_Rules_supporting.update(dict(map(None,L1_list,len(L1_list)*[{
    1000 : TriggerRule(rate=500, maxRate=500, comment='Prescaled'),
    }])))

L1_list=['L1_J20.31ETA49','L1_J30.31ETA49']
jet_Rules_supporting.update(dict(map(None,L1_list,len(L1_list)*[{
    1000 : TriggerRule(rate=500, maxRate=500, comment='Prescaled'),
    }])))
    
HLT_list=[
    # J15.31ETA49
    #'HLT_j45_320eta490','HLT_j55_320eta490',
    # J20.28ETA31
    'HLT_j85_280eta320','HLT_j85_280eta320_jes','HLT_j85_280eta320_lcw','HLT_j85_280eta320_lcw_jes','HLT_j85_280eta320_lcw_nojcalib','HLT_j85_280eta320_nojcalib',
    # J20.31ETA49
    #'HLT_j85_320eta490',
    # J30.31ETA49
    #'HLT_j110_320eta490',
    # J50.31ETA49
    'HLT_j175_320eta490','HLT_j175_320eta490_jes','HLT_j175_320eta490_lcw','HLT_j175_320eta490_lcw_jes','HLT_j175_320eta490_lcw_nojcalib','HLT_j175_320eta490_nojcalib',
    ]
jet_Rules_supporting.update(dict(map(None,HLT_list,len(HLT_list)*[{
    1000 : TriggerRule(rate=1.0, comment=''), 
    }])))

HLT_list=['HLT_j60_320eta490',]
jet_Rules_supporting.update(dict(map(None,HLT_list,len(HLT_list)*[{
    1000 : TriggerRule(rate=4.0, comment='Express', ESRate=0.2),
    }])))

HLT_list=['HLT_j85_320eta490',]
jet_Rules_supporting.update(dict(map(None,HLT_list,len(HLT_list)*[{
    1000 : TriggerRule(rate=1.0, comment=''),
    }])))

HLT_list=['HLT_j60_280eta320',]
jet_Rules_supporting.update(dict(map(None,HLT_list,len(HLT_list)*[{
    1000 : TriggerRule(rate=1.0, comment='Express', ESRate=0.2), 
    }])))

# ----------------------------------------
# forward jets seeeded by random

HLT_list=['HLT_j15_320eta490', 'HLT_j25_320eta490',]
jet_Rules_supporting.update(dict(map(None,HLT_list,len(HLT_list)*[{
    1000 : TriggerRule(rate=4.0, comment=''),
    }])))

HLT_list=['HLT_j35_320eta490',]
jet_Rules_supporting.update(dict(map(None,HLT_list,len(HLT_list)*[{
    1000 : TriggerRule(rate=1.0, comment=''),
    }])))

# ----------------------------------------
# j0_perf

HLT_list=['HLT_j0_perf_L1RD0_FILLED']
jet_Rules_supporting.update(dict(map(None,HLT_list,len(HLT_list)*[{
    1000 : TriggerRule(rate=0.5, comment=''),
    }])))

# ========================================
# Djet

HLT_list=[
    # RD0_FILLED
    'HLT_j15_j15_320eta490', 'HLT_j25_j25_320eta490',
    'HLT_j35_j35_320eta490',
    'HLT_j55_j55_320eta490_L1RD0_FILLED',    
    # J15_J15.31ETA49
    'HLT_j55_j55_320eta490',
    # J20_J20.31ETA49
    'HLT_j60_j60_320eta490', 
    ]
jet_Rules_supporting.update(dict(map(None,HLT_list,len(HLT_list)*[{
    1000 : TriggerRule(rate=1.0, comment=''), 
    }])))

HLT_list=['HLT_j45_j45_320eta490','HLT_j85_j85_320eta490',]
jet_Rules_supporting.update(dict(map(None,HLT_list,len(HLT_list)*[{
    1000 : TriggerRule(rate=1.0, comment=''), 
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
    1000 : TriggerRule(PS=6, comment='PS=6 to get 10 Hz at 3e33, only up to 5e33'), 
    5001 : TriggerRule(PS=110, comment='1 Hz @ 5e33'), 
    }])))

########################################

jet_Rules={}

RulesList=[jet_Rules_2e34, jet_Rules_1e34, jet_Rules_0p5e34, jet_Rules_0p15e34, jet_Rules_supporting] 
for Rules in RulesList:
    for newrule in Rules.keys():
        if jet_Rules.has_key(newrule):
            print 'FATAL     Physics_pp_v5_rules     Duplicated rule inside physics rule. Cannot be added:',newrule
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
    # 2b + 1j
    # 3J25.0ETA23 seeded
    'HLT_2j75_bmedium_j75', 'HLT_2j75_bmedium_split_j75',
    'HLT_2j70_btight_j70',  'HLT_2j70_btight_split_j70',
    #
    # 2b + 2j
    # 3J25.0ETA23 seeded
    'HLT_2j45_bmedium_2j45_L13J25.0ETA23',   'HLT_2j45_bmedium_split_2j45_L13J25.0ETA23',
    'HLT_2j45_bmv2c2077_2j45_L13J25.0ETA23', 'HLT_2j45_bmv2c2077_split_2j45_L13J25.0ETA23',
    'HLT_2j35_btight_2j35_L13J25.0ETA23',    'HLT_2j35_btight_split_2j35_L13J25.0ETA23',
    'HLT_2j35_bmv2c2070_2j35_L13J25.0ETA23', 'HLT_2j35_bmv2c2070_split_2j35_L13J25.0ETA23',
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

# --------------------------------------

HLT_list=['HLT_mu4_j40_dr05_3j40_L14J20',]
bjet_Rules_1e34.update(dict(map(None,HLT_list,len(HLT_list)*[{
                      1000 : TriggerRule(PS=1, comment=''), 
    lumi_disable_mu_j_phys : TriggerRule(PS=-1, comment='Disabled'), 
    }])))

########################################
# 0.5e34 

bjet_Rules_0p5e34={}

HLT_list=['HLT_2j70_bmedium_j70','HLT_2j70_bmedium_split_j70', 'HLT_2j65_btight_j65', 'HLT_2j65_btight_split_j65',]
bjet_Rules_0p5e34.update(dict(map(None,HLT_list,len(HLT_list)*[{
    1000 : TriggerRule(PS=1, comment=''), 
    lumi_disable_2b1j_70 : TriggerRule(PS=-1, comment='Disabled'),
    }])))

# --------------------------------------

HLT_list=['HLT_j150_bmedium_j50_bmedium','HLT_j150_bmedium_split_j50_bmedium_split',]
bjet_Rules_0p5e34.update(dict(map(None,HLT_list,len(HLT_list)*[{
                   1000 : TriggerRule(PS=1, comment=''), 
    lumi_disable_2b_150 : TriggerRule(PS=-1, comment='Disabled'),
    }])))

# --------------------------------------

HLT_list=['HLT_j225_bloose','HLT_j225_bloose_split',]
bjet_Rules_0p5e34.update(dict(map(None,HLT_list,len(HLT_list)*[{
                   1000 : TriggerRule(PS=1, comment=''), 
    lumi_disable_1b_225 : TriggerRule(PS=-1, comment='Disabled'),
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

# ----------------------------------------

L1_list=['L1_MU4_3J20']
bjet_Rules_0p5e34.update(dict(map(None,L1_list,len(L1_list)*[{
     1000 : TriggerRule(PS=1, comment=''),
     5001 : TriggerRule(rate=300, maxRate=300, comment='Prescaled'),
    }])))

HLT_list=['HLT_mu4_4j40_dr05_L1MU4_3J20']
bjet_Rules_0p5e34.update(dict(map(None,HLT_list,len(HLT_list)*[{
     1000 : TriggerRule(PS=1, comment=''), 
     5001 : TriggerRule(rate=2.0, comment='prescaled'), 
    }])))

########################################
# 0.3e34 

bjet_Rules_0p3e34={}

HLT_list=['HLT_j175_bmedium', 'HLT_j175_bmedium_split']
bjet_Rules_0p3e34.update(dict(map(None,HLT_list,len(HLT_list)*[{
    1000 : TriggerRule(PS=1, comment=''), 
    3001 : TriggerRule(PS=-1, comment='Disabled'),
    }])))

# ----------------------------------------

L1_list=['L1_MU4_3J15']
bjet_Rules_0p3e34.update(dict(map(None,L1_list,len(L1_list)*[{
     1000 : TriggerRule(PS=1, comment='L1'),
     3001 : TriggerRule(rate=300, maxRate=300, comment='Prescaled'),
    }])))

HLT_list=['HLT_mu4_4j40_dr05_L1MU4_3J15']
bjet_Rules_0p3e34.update(dict(map(None,HLT_list,len(HLT_list)*[{
     1000 : TriggerRule(PS=1, comment=''),
     3001 : TriggerRule(rate=2.0, comment='prescaled'),
    }])))

########################################
# 0.15e34 

bjet_Rules_0p15e34={}

# decided at OAB 19/10 to save HLT output rate
HLT_list=[
    # mu+j, 3j
    'HLT_mu4_3j45_dr05_L1MU4_3J15',     'HLT_mu4_3j45_dr05_L1MU4_3J20',
    'HLT_mu4_j60_dr05_2j35_L1MU4_3J15', 'HLT_mu4_j60_dr05_2j35_L1MU4_3J20',
    'HLT_mu6_j50_dr05_2j35_L1MU4_3J15', 'HLT_mu6_j50_dr05_2j35_L1MU4_3J20',
    ]
bjet_Rules_0p15e34.update(dict(map(None,HLT_list,len(HLT_list)*[{
    1000 : TriggerRule(PS=1, comment=''), 
    1501 : TriggerRule(rate=0.5, comment='Prescaled'),
    }])))

# ----------------------------------------

# HLT rate too high for single mu+jet+dR items (800-1400 Hz @2e34)
# prescale to 2 Hz for all

L1_list=['L1_MU4_J30', 'L1_MU6_J20', 'L1_MU4_J50', 'L1_MU6_J40']
bjet_Rules_0p15e34.update(dict(map(None,L1_list,len(L1_list)*[{
    1000 : TriggerRule(rate=200, maxRate=200, comment='Prescaled'),
    }])))

HLT_list=['HLT_mu4_j70_dr05_L1MU4_J30', 'HLT_mu6_j60_dr05_L1MU6_J20', 'HLT_mu4_j70_dr05_L1MU4_J50', 'HLT_mu6_j60_dr05_L1MU6_J40']
bjet_Rules_0p15e34.update(dict(map(None,HLT_list,len(HLT_list)*[{
    1000 : TriggerRule(rate=1.0, comment='Prescaled'), 
    }])))

# ----------------------------------------

# HLT rate too high for single j175_bmv2c2085 (400-700 Hz @2e34)
# prescale to 2 Hz

# decided at OAB 19/10
HLT_list=['HLT_j175_bmv2c2085', 'HLT_j175_bmv2c2085_split']
bjet_Rules_0p15e34.update(dict(map(None,HLT_list,len(HLT_list)*[{
    1000 : TriggerRule(rate=0.5, comment='Prescaled'), 
    }])))

########################################
# supporting

bjet_Rules_supporting={}

# ========================================
# lepton + b-perf

# PS=1, as it is showeed by lepton
HLT_list=[
    'HLT_2e17_loose_2j35_bperf', 'HLT_2e17_lhloose_2j35_bperf',
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
                      'HLT_j15_bperf_split',  'HLT_j15_boffperf',  'HLT_j15_boffperf_split',
    'HLT_j25_bperf',  'HLT_j25_bperf_split',  'HLT_j25_boffperf',  'HLT_j25_boffperf_split',
    # J15
    'HLT_j35_bperf',  'HLT_j35_bperf_split',  'HLT_j35_boffperf',  'HLT_j35_boffperf_split',
    'HLT_j45_bperf',  'HLT_j45_bperf_split',  'HLT_j45_boffperf',  'HLT_j45_boffperf_split',
    # J20
                      'HLT_j55_bperf_split',  'HLT_j55_boffperf',  'HLT_j55_boffperf_split',
    'HLT_j85_bperf',  'HLT_j85_bperf_split',  'HLT_j85_boffperf',  'HLT_j85_boffperf_split',
    # J30
                      'HLT_j110_bperf_split', 'HLT_j110_boffperf', 'HLT_j110_boffperf_split',
    # J40
    'HLT_j150_bperf', 'HLT_j150_bperf_split', 'HLT_j150_boffperf', 'HLT_j150_boffperf_split',
    # J75
    'HLT_j260_bperf', 'HLT_j260_bperf_split', 'HLT_j260_boffperf', 'HLT_j260_boffperf_split',
    # J85
    'HLT_j320_bperf', 'HLT_j320_bperf_split', 'HLT_j320_boffperf', 'HLT_j320_boffperf_split',
    # j100
    'HLT_j400_bperf', 'HLT_j400_bperf_split', 'HLT_j400_boffperf', 'HLT_j400_boffperf_split',
    # 3J15
    'HLT_j45_bperf_3j45',               'HLT_j45_bperf_split_3j45',               'HLT_j45_boffperf_3j45',               'HLT_j45_boffperf_split_3j45',
    # 3J15.0ETA25
    'HLT_j45_bperf_3j45_L13J15.0ETA25', 'HLT_j45_bperf_split_3j45_L13J15.0ETA25', 'HLT_j45_boffperf_3j45_L13J15.0ETA25', 'HLT_j45_boffperf_split_3j45_L13J15.0ETA25',
    # 3J20
    'HLT_j45_bperf_3j45_L13J20',        'HLT_j45_bperf_split_3j45_L13J20',        'HLT_j45_boffperf_3j45_L13J20',        'HLT_j45_boffperf_split_3j45_L13J20',
    # 4J20
    'HLT_j45_bperf_3j45_L14J20',
    'HLT_j55_bperf_3j55',
    ]
bjet_Rules_supporting.update(dict(map(None,HLT_list,len(HLT_list)*[{
    1000 : TriggerRule(rate=0.5, comment=''), 
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
    1000 : TriggerRule(rate=0.5, comment='Express', ESRate=0.2), 
    }])))

# changes agreed at OAB 19/10

HLT_list=[
    # J100
    'HLT_j225_boffperf',
    'HLT_j150_boffperf_j50_boffperf',
    # 3J25.0ETA23
    'HLT_j65_boffperf_3j65_L13J25.0ETA23',
    'HLT_2j35_boffperf_2j35_L13J25.0ETA23',
    'HLT_2j45_boffperf_2j45_L13J25.0ETA23',
    ]
bjet_Rules_supporting.update(dict(map(None,HLT_list,len(HLT_list)*[{
    1000 : TriggerRule(rate=1.0, comment=''), 
    }])))

HLT_list=[
    # J100
    'HLT_j175_boffperf',
    # 3J25.0ETA23
    'HLT_j70_boffperf_3j70_L13J25.0ETA23',
    'HLT_2j65_boffperf_j65',
    # 4J15.0ETA25
    'HLT_j65_boffperf_3j65_L14J15.0ETA25',
    'HLT_j70_boffperf_3j70_L14J15.0ETA25',
    'HLT_2j35_boffperf_2j35_L14J15.0ETA25',
    'HLT_2j45_boffperf_2j45_L14J15.0ETA25',
    ]
bjet_Rules_supporting.update(dict(map(None,HLT_list,len(HLT_list)*[{
    1000 : TriggerRule(PS=-1, comment=''), 
    }])))

# ========================================
# mu + jets, bperf

L1_list=['L1_MU4_J12']
bjet_Rules_supporting.update(dict(map(None,L1_list,len(L1_list)*[{
    1000 : TriggerRule(rate=500, maxRate=500, comment='Prescaled'),
    }])))

L1_list=['L1_MU6_J75']
bjet_Rules_supporting.update(dict(map(None,L1_list,len(L1_list)*[{
    1000 : TriggerRule(PS=1, comment='Prescaled'),
    }])))

HLT_list=[
    # MU4
    'HLT_mu4_j15_bperf_split_dr05_dz02',
    'HLT_mu4_j25_bperf_split_dr05_dz02',
    'HLT_mu4_j35_bperf_split_dr05_dz02',
    # MU4_J12
    'HLT_mu4_j55_bperf_split_dr05_dz02',
    # MU6_J20
    'HLT_mu6_j85_bperf_split_dr05_dz02',
    'HLT_mu6_j110_bperf_split_dr05_dz02',
    # MU6_J40
    'HLT_mu6_j150_bperf_split_dr05_dz02',
    'HLT_mu6_j175_bperf_split_dr05_dz02',
    # MU6_J75
    'HLT_mu6_j260_bperf_split_dr05_dz02',
    'HLT_mu6_j320_bperf_split_dr05_dz02',
    'HLT_mu6_j400_bperf_split_dr05_dz02',
    ]
bjet_Rules_supporting.update(dict(map(None,HLT_list,len(HLT_list)*[{
    1000 : TriggerRule(rate=1.0, comment=''),
    }])))

HLT_list=[
    # MU4
    'HLT_mu4_j15_dr05',
    'HLT_mu4_j25_dr05',
    'HLT_mu4_j35_dr05',
    # MU4_J12
    'HLT_mu4_j55_dr05',
    # MU6_J20
    'HLT_mu6_j85_dr05',
    'HLT_mu6_j110_dr05',
    # MU6_J40
    'HLT_mu6_j150_dr05',
    'HLT_mu6_j175_dr05',
    # MU6_J75
    'HLT_mu6_j260_dr05',
    'HLT_mu6_j320_dr05',
    'HLT_mu6_j400_dr05',
    ]
bjet_Rules_supporting.update(dict(map(None,HLT_list,len(HLT_list)*[{
    1000 : TriggerRule(PS=-1, comment='backup of bperf_split'),
    }])))

########################################

bjet_Rules={}

RulesList=[bjet_Rules_2e34, bjet_Rules_1e34, bjet_Rules_0p5e34, bjet_Rules_0p3e34, bjet_Rules_0p15e34, bjet_Rules_supporting] 
for Rules in RulesList:
    for newrule in Rules.keys():
        if bjet_Rules.has_key(newrule):
            print 'FATAL     Physics_pp_v5_rules     Duplicated rule inside physics rule. Cannot be added:',newrule
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
    'HLT_e17_lhmedium_nod0_tau80_medium1_tracktwo',
    # do not forget to put rerun for HLT_e17_lhmedium_tau80_medium1_tracktwo
    # when it gets prescaled (ATR-12219)
    'HLT_e17_lhmedium_tau80_medium1_tracktwo',
    #
    # tau + e + xe
    'HLT_e17_medium_tau25_medium1_tracktwo_xe50',
    'HLT_e17_lhmedium_tau25_medium1_tracktwo_xe50',
    'HLT_e17_lhmedium_nod0_tau25_medium1_tracktwo_xe50',
    #
    'HLT_e17_medium_iloose_tau25_medium1_tracktwo_xe50',
    'HLT_e17_lhmedium_iloose_tau25_medium1_tracktwo_xe50',
    'HLT_e17_lhmedium_nod0_iloose_tau25_medium1_tracktwo_xe50',
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

HLT_list=['HLT_tau125_medium1_tracktwo', 'HLT_tau125_medium1_track',]
tau_Rules_1e34.update(dict(map(None,HLT_list,len(HLT_list)*[{
     1000 : TriggerRule(PS=1, comment=''), 
    10001 : TriggerRule(PS=0, comment='rerun',rerun=1),
    }])))

# --------------------------------------

L1_list=['L1_EM15HI_2TAU12IM_J25_3J12']
HLT_list=[
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
    lumi_nonl1topo_disable_tau_e : TriggerRule(rate=500, maxRate=500, comment='Prescaled'),
    }])))

HLT_list=['HLT_e17_medium_iloose_tau25_medium1_tracktwo', 'HLT_e17_lhmedium_nod0_iloose_tau25_medium1_tracktwo',]
tau_Rules_1e34.update(dict(map(None,HLT_list,len(HLT_list)*[{
                            1000 : TriggerRule(PS=1, comment=''), 
    lumi_nonl1topo_disable_tau_e : TriggerRule(rate=0.1, comment='Prescaled'),
    }])))

# assign rerun (ATR-12219)
HLT_list=['HLT_e17_lhmedium_iloose_tau25_medium1_tracktwo']
tau_Rules_1e34.update(dict(map(None,HLT_list,len(HLT_list)*[{
                            1000 : TriggerRule(PS=1, comment=''), 
    lumi_nonl1topo_disable_tau_e : TriggerRule(rate=0.1, comment='Prescaled', rerun=1),
    }])))

# --------------------------------------

L1_list=['L1_MU10_TAU12IM_J25_2J12',]
tau_Rules_1e34.update(dict(map(None,L1_list,len(L1_list)*[{
                             1000 : TriggerRule(PS=1, comment='L1'), 
    lumi_nonl1topo_disable_tau_mu : TriggerRule(rate=500, maxRate=500, comment='Prescaled'),
    }])))

# assign rerun (ATR-12219)
HLT_list=['HLT_mu14_iloose_tau25_medium1_tracktwo',]
tau_Rules_1e34.update(dict(map(None,HLT_list,len(HLT_list)*[{
                             1000 : TriggerRule(PS=1, comment=''), 
    lumi_nonl1topo_disable_tau_mu : TriggerRule(rate=0.1, comment='Prescaled', rerun=1),
    }])))

# --------------------------------------

L1_list=['L1_TAU20IM_2TAU12IM_J25_2J20_3J12',]
tau_Rules_1e34.update(dict(map(None,L1_list,len(L1_list)*[{
                              1000 : TriggerRule(PS=1, comment='L1'), 
    lumi_nonl1topo_disable_tau_tau : TriggerRule(rate=500, maxRate=500, comment='Prescaled'),
    }])))

# assign rerun (ATR-12219)
HLT_list=['HLT_tau35_medium1_tracktwo_tau25_medium1_tracktwo',]
tau_Rules_1e34.update(dict(map(None,HLT_list,len(HLT_list)*[{
                              1000 : TriggerRule(PS=1, comment=''), 
    lumi_nonl1topo_disable_tau_tau : TriggerRule(rate=0.1, comment='Prescaled', rerun=1),
    }])))

HLT_list=['HLT_tau35_tight1_tracktwo_tau25_tight1_tracktwo',]
tau_Rules_1e34.update(dict(map(None,HLT_list,len(HLT_list)*[{
                              1000 : TriggerRule(PS=1, comment=''), 
    lumi_nonl1topo_disable_tau_tau : TriggerRule(rate=0.1, comment='Prescaled'),
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

L1_list=['L1_TAU40']
tau_Rules_0p5e34.update(dict(map(None,L1_list,len(L1_list)*[{
                     1000 : TriggerRule(PS=1, comment='L1'), 
    lumi_disable_l1_tau40 : TriggerRule(PS=-1, comment='Disabled'),
    }])))

# --------------------------------------

HLT_list=['HLT_tau80_medium1_tracktwo_L1TAU60',]
tau_Rules_0p5e34.update(dict(map(None,HLT_list,len(HLT_list)*[{
    1000 : TriggerRule(PS=1, comment=''), 
    5001 : TriggerRule(PS=0, comment='Disabled', rerun=1),
    }])))

# --------------------------------------

HLT_list=[
    #
    # tau + e
    'HLT_e17_medium_tau25_medium1_tracktwo','HLT_e17_lhmedium_tau25_medium1_tracktwo',
    'HLT_e17_lhmedium_nod0_tau25_medium1_tracktwo',
    # tau + mu (high et, w/o jet)
    'HLT_mu14_tau35_medium1_tracktwo',
    ]
tau_Rules_0p5e34.update(dict(map(None,HLT_list,len(HLT_list)*[{
    1000 : TriggerRule(PS=1, comment=''), 
    5001 : TriggerRule(PS=-1, comment='Disabled'),
    }])))


HLT_list=[
    # tau + mu
    'HLT_mu14_tau25_medium1_tracktwo',
    ]
tau_Rules_0p5e34.update(dict(map(None,HLT_list,len(HLT_list)*[{
    1000 : TriggerRule(PS=1, comment=''), 
    5001 : TriggerRule(PS=0, comment='Disabled',rerun=1),
    }])))

# --------------------------------------

L1_list=['L1_TAU20_2TAU12_XE35']
tau_Rules_0p5e34.update(dict(map(None,L1_list,len(L1_list)*[{
                         1000 : TriggerRule(PS=1, comment='L1'), 
    lumi_l1jet_enable_tau_tau : TriggerRule(PS=-1, comment='Disabled'),
    }])))

HLT_list=['HLT_tau35_medium1_tracktwo_L1TAU20_tau25_medium1_tracktwo_L1TAU12_xe50',]
tau_Rules_0p5e34.update(dict(map(None,HLT_list,len(HLT_list)*[{
                         1000 : TriggerRule(PS=1, comment=''), 
    lumi_l1jet_enable_tau_tau : TriggerRule(PS=-1, comment='Disabled'),
    }])))

# --------------------------------------

# ATR-11855: needs PS=20 for background estimation

L1_list=['L1_TAU20IM_2TAU12IM',]
tau_Rules_0p5e34.update(dict(map(None,L1_list,len(L1_list)*[{
                         1000 : TriggerRule(PS=1, comment='L1'), 
    lumi_l1jet_enable_tau_tau : TriggerRule(PS=20, comment='Prescaled by 20 (ATR-11855)'),
    }])))

# assign rerun (ATR-12219)
HLT_list=['HLT_tau35_medium1_tracktwo_tau25_medium1_tracktwo_L1TAU20IM_2TAU12IM',]
tau_Rules_0p5e34.update(dict(map(None,HLT_list,len(HLT_list)*[{
                         1000 : TriggerRule(PS=1, comment=''), 
    lumi_l1jet_enable_tau_tau : TriggerRule(PS=1, comment='Prescaled at L1', rerun=1),
    }])))

HLT_list=['HLT_tau35_tight1_tracktwo_tau25_tight1_tracktwo_L1TAU20IM_2TAU12IM',]
tau_Rules_0p5e34.update(dict(map(None,HLT_list,len(HLT_list)*[{
                         1000 : TriggerRule(PS=1, comment=''), 
    lumi_l1jet_enable_tau_tau : TriggerRule(PS=-1, comment='Disabled'),
    }])))

########################################
# 0.3e34 

tau_Rules_0p3e34={}

HLT_list=['HLT_tau80_medium1_tracktwo', 'HLT_tau80_medium1_track',]
tau_Rules_0p3e34.update(dict(map(None,HLT_list,len(HLT_list)*[{
    1000 : TriggerRule(PS=1, comment=''), 
    3001 : TriggerRule(PS=0, comment='rerun', rerun=1),
    }])))

# --------------------------------------

L1_list=['L1_TAU30']
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
    # perf
    'HLT_tau0_perf_ptonly_L1TAU12'                : {  1000 : TriggerRule(rate=2.0, comment='Express', ESRate=0.4, rerun=1) },
    'HLT_tau0_perf_ptonly_L1TAU60'                : {  1000 : TriggerRule(rate=1.0, comment='Express', ESRate=0.2, rerun=1) },

    # --------------------------------------
    # single & di-tau
    'HLT_tau25_medium1_tracktwo'                  : {  1000 : TriggerRule(rate=0.2, comment='Express', ESValue=1, rerun=1) },
    'HLT_tau25_perf_tracktwo'                     : {  1000 : TriggerRule(rate=0.5, comment='Express', ESValue=1, rerun=1) },
    'HLT_tau35_perf_tracktwo_tau25_perf_tracktwo' : {  1000 : TriggerRule(rate=0.4, comment='Express', ESValue=1, rerun=1) },

    # --------------------------------------
    # Z T&P
    # assign rerun (ATR-12219)
    'HLT_mu14_iloose_tau25_perf_tracktwo'         : {  1000 : TriggerRule(rate=0.4, comment='Express', ESValue=1, rerun=1) },

    # --------------------------------------
    # idperf
    'HLT_tau25_idperf_tracktwo'                   : {  1000 : TriggerRule(rate=0.5, comment='Express', ESValue=1, rerun=1) },
    'HLT_tau25_idperf_track'                      : {  1000 : TriggerRule(rate=0.5, comment='Express', ESValue=1, rerun=1) },
    'HLT_tau160_idperf_tracktwo'                  : {  1000 : TriggerRule(rate=0.4, comment='Express', ESRate=0.1, rerun=1) },
    'HLT_tau160_idperf_track'                     : {  1000 : TriggerRule(rate=0.4, comment='Express', ESRate=0.1, rerun=1) },
}

# ========================================

HLT_list=[
    'HLT_tau5_perf_ptonly_L1TAU8',
    'HLT_tau20_r1_idperf',
    'HLT_tau25_r1_idperf',
    #
    'HLT_tau25_loose1_tracktwo', 'HLT_tau25_loose1_ptonly',
    'HLT_tau25_medium1_ptonly',  'HLT_tau25_medium1_track', 'HLT_tau25_medium1_mvonly',
    'HLT_tau25_medium1_tracktwo_L1TAU12',
    'HLT_tau25_medium1_tracktwo_L1TAU12IL',
    'HLT_tau25_medium1_tracktwo_L1TAU12IT',
    'HLT_tau25_perf_ptonly',           'HLT_tau25_perf_track',
    'HLT_tau25_perf_tracktwo_L1TAU12',   'HLT_tau25_perf_ptonly_L1TAU12',   'HLT_tau25_perf_track_L1TAU12',
    'HLT_tau25_tight1_tracktwo', 'HLT_tau25_tight1_ptonly',
    #
    'HLT_tau35_loose1_tracktwo',         'HLT_tau35_loose1_ptonly',
    'HLT_tau35_medium1_tracktwo',        'HLT_tau35_medium1_ptonly',        'HLT_tau35_medium1_track',
    'HLT_tau35_medium1_tracktwo_L1TAU20','HLT_tau35_medium1_ptonly_L1TAU20',
    'HLT_tau35_medium1_tracktwo_L1TAU20IL',
    'HLT_tau35_medium1_tracktwo_L1TAU20IT',
    'HLT_tau35_tight1_ptonly',
    'HLT_tau35_tight1_tracktwo',
    'HLT_tau35_perf_tracktwo', 'HLT_tau35_perf_ptonly',
    #
    'HLT_tau50_medium1_tracktwo_L1TAU12',
    'HLT_tau125_perf_tracktwo', 'HLT_tau125_perf_ptonly',


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
            print 'FATAL     Physics_pp_v5_rules     Duplicated rule inside physics rule. Cannot be added:',newrule
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
    'HLT_e200_etcut',
    #
    # 2e
    'HLT_2e17_lhloose_nod0','HLT_2e17_lhloose_cutd0dphideta','HLT_2e17_lhloose_HLTCalo',
    #
    # vloose version as well: 2 Hz extra by vloose @ 2e34 (ATR-11746)
    'HLT_2e17_vloose','HLT_2e17_lhvloose','HLT_2e17_lhvloose_nod0',
    #
    # 3e
    'HLT_e17_medium_2e9_medium',
    'HLT_e17_lhmedium_2e9_lhmedium','HLT_e17_lhmedium_nod0_2e9_lhmedium_nod0',
    #
    'HLT_e17_medium_iloose_2e9_medium',
    'HLT_e17_lhmedium_iloose_2e9_lhmedium','HLT_e17_lhmedium_nod0_iloose_2e9_lhmedium_nod0',
    #
    # 1g
    'HLT_g140_loose_HLTCalo',
    'HLT_g200_etcut',
    #
    # 2g
    'HLT_g35_medium_HLTCalo_g25_medium_HLTCalo',
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

HLT_list=['HLT_2e17_loose', 'HLT_2e17_lhloose']
egamma_Rules_2e34.update(dict(map(None,HLT_list,len(HLT_list)*[{
                       1000 : TriggerRule(PS=1, comment='Express', ESValue=0), 
    lumi_disable_l1_2em10vh : TriggerRule(PS=1, comment='Express', ESRate=0.2), 
    }])))

# --------------------------------------

HLT_list=['HLT_g140_loose']
egamma_Rules_2e34.update(dict(map(None,HLT_list,len(HLT_list)*[{
    1000 : TriggerRule(PS=1, comment='Express', ESValue=0), 
    5001 : TriggerRule(PS=1, comment='Express', ESRate=0.2), 
    }])))

# --------------------------------------

HLT_list=['HLT_g35_medium_g25_medium']
egamma_Rules_2e34.update(dict(map(None,HLT_list,len(HLT_list)*[{
                         1000 : TriggerRule(PS=1, comment='', ESValue=0), 
    lumi_disable_hlt_2g_loose : TriggerRule(PS=1, comment='Express', ESRate=0.4), 
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

# SUSY request, but too high rate
# MCG 2015.05.28  unique rate to be revisited

HLT_list=['HLT_e60_lhvloose_nod0']
egamma_Rules_1e34.update(dict(map(None,HLT_list,len(HLT_list)*[{
                    1000 : TriggerRule(PS=12, comment='1 Hz target at 3e33', rerun=1),
                    }])))

# --------------------------------------

L1_list=['L1_EM20VHI',]
egamma_Rules_1e34.update(dict(map(None,L1_list,len(L1_list)*[{
                       1000 : TriggerRule(PS=1, comment='L1'),
    lumi_disable_l1_em20vhi : TriggerRule(PS=-1, comment='Disabled'),
                       }])))

HLT_list=['HLT_e24_tight_iloose',
          'HLT_e24_lhtight_iloose','HLT_e24_lhtight_nod0_iloose','HLT_e24_lhtight_cutd0dphideta_iloose','HLT_e24_lhtight_iloose_HLTCalo']

egamma_Rules_1e34.update(dict(map(None,HLT_list,len(HLT_list)*[{
                       1000 : TriggerRule(PS=1, comment=''), 
    lumi_disable_l1_em20vhi : TriggerRule(PS=-1, comment='Disabled'),
    }])))

HLT_list=['HLT_e24_lhmedium_iloose']

egamma_Rules_1e34.update(dict(map(None,HLT_list,len(HLT_list)*[{
                       1000 : TriggerRule(PS=1, comment=''), 
    lumi_disable_l1_em20vhi : TriggerRule(PS=-1, comment='Disabled'),
    }])))

# --------------------------------------

# Exceptionally, e24_(lh)medium_L1EM20VH is kept alive until L1_EM20VH is prescaled
# --> VHI seed is also kept alive

HLT_list=['HLT_e24_lhmedium_L1EM20VHI','HLT_e24_medium_L1EM20VHI']

egamma_Rules_1e34.update(dict(map(None,HLT_list,len(HLT_list)*[{
                    1000 : TriggerRule(PS=1, comment=''),
    lumi_l1iso_enable_em : TriggerRule(PS=0, comment='rerun', rerun=1),
    }])))

# --------------------------------------

L1_list=['L1_2EM13VH',]
egamma_Rules_1e34.update(dict(map(None,L1_list,len(L1_list)*[{
                       1000 : TriggerRule(PS=1, comment='L1'),
    lumi_disable_l1_2em13vh : TriggerRule(PS=-1, comment='Disabled'),
    }])))

HLT_list=[
    'HLT_2e15_loose_L12EM13VH', 'HLT_2e15_lhloose_L12EM13VH','HLT_2e15_lhloose_nod0_L12EM13VH',
    'HLT_2e15_lhloose_cutd0dphideta_L12EM13VH','HLT_2e15_lhloose_HLTCalo_L12EM13VH',
    #
    # vloose version as well: 2 Hz extra by vloose @ 1e34
    'HLT_2e15_vloose_L12EM13VH','HLT_2e15_lhvloose_L12EM13VH','HLT_2e15_lhvloose_nod0_L12EM13VH',
    ]
egamma_Rules_1e34.update(dict(map(None,HLT_list,len(HLT_list)*[{
                       1000 : TriggerRule(PS=1, comment=''), 
    lumi_disable_l1_2em13vh : TriggerRule(PS=-1, comment='Disabled'),
    }])))

########################################
# 0.5e34 

egamma_Rules_0p5e34={}

HLT_list=['HLT_3g15_loose',]
egamma_Rules_0p5e34.update(dict(map(None,HLT_list,len(HLT_list)*[{
    1000 : TriggerRule(PS=1, comment=''), 
    5001 : TriggerRule(PS=-1, comment='Disabled'),
    }])))

# --------------------------------------

HLT_list=['HLT_g35_loose_g25_loose']
egamma_Rules_0p5e34.update(dict(map(None,HLT_list,len(HLT_list)*[{
                         1000 : TriggerRule(PS=1, comment='Express', ESRate=0.4), 
    lumi_disable_hlt_2g_loose : TriggerRule(PS=6, comment='Prescaled for supporting', ESValue=0),
    }])))

# --------------------------------------

HLT_list=['HLT_g120_loose',]
egamma_Rules_0p5e34.update(dict(map(None,HLT_list,len(HLT_list)*[{
                      1000 : TriggerRule(PS=1, comment='Express', ESRate=0.2), 
    lumi_disable_hlt_eg120 : TriggerRule(rate=2.0, comment='',rerun=1, ESValue=0),
    }])))

HLT_list=['HLT_e120_loose','HLT_e120_lhloose','HLT_e120_lhloose_nod0', 'HLT_e120_lhloose_HLTCalo',]
egamma_Rules_0p5e34.update(dict(map(None,HLT_list,len(HLT_list)*[{
                      1000 : TriggerRule(PS=1, comment=''), 
    lumi_disable_hlt_eg120 : TriggerRule(PS=-1, comment='Disabled'),
    }])))

# --------------------------------------

L1_list=['L1_EM20VH']
egamma_Rules_0p5e34.update(dict(map(None,L1_list,len(L1_list)*[{
                    1000 : TriggerRule(PS=1, comment='L1'),
    lumi_l1iso_enable_em : TriggerRule(PS=10, comment='Prescaled by 10 for supporting'),
                    }])))

HLT_list=[
    'HLT_e24_medium_iloose_L1EM20VH',
    'HLT_e24_lhmedium_iloose_L1EM20VH','HLT_e24_lhmedium_nod0_iloose_L1EM20VH','HLT_e24_lhmedium_cutd0dphideta_iloose_L1EM20VH',
    'HLT_e24_lhmedium_iloose_HLTCalo_L1EM20VH',
    #
    'HLT_e24_tight_iloose_L1EM20VH',   'HLT_e24_tight_iloose_HLTCalo_L1EM20VH','HLT_e24_tight_iloose_L2EFCalo_L1EM20VH',
    'HLT_e24_lhtight_iloose_L1EM20VH', 'HLT_e24_lhtight_nod0_iloose_L1EM20VH', 'HLT_e24_lhtight_cutd0dphideta_iloose_L1EM20VH',
    'HLT_e24_lhtight_iloose_HLTCalo_L1EM20VH','HLT_e24_lhtight_iloose_L2EFCalo_L1EM20VH',
    ]
egamma_Rules_0p5e34.update(dict(map(None,HLT_list,len(HLT_list)*[{
                    1000 : TriggerRule(PS=1, comment=''), 
    lumi_l1iso_enable_em : TriggerRule(PS=-1, comment='Disabled'),
    }])))

# --------------------------------------

# Exceptionally, e24_(lh)medium_L1EM20VH is kept alive until L1_EM20VH is prescaled

HLT_list=['HLT_e24_lhmedium_L1EM20VH','HLT_e24_medium_L1EM20VH']
egamma_Rules_0p5e34.update(dict(map(None,HLT_list,len(HLT_list)*[{
                    1000 : TriggerRule(PS=1, comment=''), 
    lumi_l1iso_enable_em : TriggerRule(PS=0, comment='rerun', rerun=1),
    }])))

# --------------------------------------

L1_list=['L1_2EM10VH']
egamma_Rules_0p5e34.update(dict(map(None,L1_list,len(L1_list)*[{
                       1000 : TriggerRule(PS=1, comment='L1'),
    lumi_disable_l1_2em10vh : TriggerRule(PS=-1, comment='Disabled'),
                       }])))

HLT_list=['HLT_2e12_loose_L12EM10VH', 'HLT_2e12_lhloose_L12EM10VH']
egamma_Rules_0p5e34.update(dict(map(None,HLT_list,len(HLT_list)*[{
                       1000 : TriggerRule(PS=1, comment='Express', ESRate=0.2), 
    lumi_disable_l1_2em10vh : TriggerRule(PS=-1, comment='Disabled', ESValue=0),
    }])))

HLT_list=[
    'HLT_2e12_lhloose_nod0_L12EM10VH',         'HLT_2e12_lhloose_nodeta_L12EM10VH',
    'HLT_2e12_lhloose_cutd0dphideta_L12EM10VH','HLT_2e12_lhloose_HLTCalo_L12EM10VH','HLT_2e12_lhloose_nodphires_L12EM10VH',
    #
    # vloose version as well: 12 Hz extra by lhvloose @ 5e33
    'HLT_2e12_vloose_L12EM10VH', 'HLT_2e12_lhvloose_L12EM10VH', 'HLT_2e12_lhvloose_nod0_L12EM10VH',
    ]
egamma_Rules_0p5e34.update(dict(map(None,HLT_list,len(HLT_list)*[{
                       1000 : TriggerRule(PS=1, comment=''), 
    lumi_disable_l1_2em10vh : TriggerRule(PS=-1, comment='Disabled'),
    }])))

########################################
# 0.3e34 

egamma_Rules_0p3e34={}

L1_list=['L1_EM18VH']
egamma_Rules_0p3e34.update(dict(map(None,L1_list,len(L1_list)*[{
                      1000 : TriggerRule(PS=1, comment='L1'),
    lumi_disable_l1_em18vh : TriggerRule(PS=10, comment='Prescaled by 10 for supporting - vloose etc triggers'),
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

L1_list=['L1_2EM15']
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
# non isolated single electron for background

# Exceptionally, e24_(lh)medium_L1EM18VH is kept alive until L1_EM18VH is prescaled

HLT_list=['HLT_e24_medium_L1EM18VH','HLT_e24_lhmedium_L1EM18VH']
egamma_Rules_supporting.update(dict(map(None,HLT_list,len(HLT_list)*[{
                      1000 : TriggerRule(PS=1, comment='Express', ESRate=0.2),
# decided at OAB 19/10 to save HLT output rate
#    lumi_disable_l1_em18vh : TriggerRule(PS=1, comment='L1 is prescaled by 10', ESRate=0.2),
#      lumi_l1iso_enable_em : TriggerRule(PS=1, comment='L1 is prescaled by 10', ESValue=0),
                      lumi_disable_l1_em18vh : TriggerRule(rate=0.2, comment='Disabled to save HLT rate',ESValue=1),
                      lumi_l1iso_enable_em : TriggerRule(PS=0, comment='Disabled to save HLT rate',rerun=1,ESValue=0),
                      }])))

HLT_list=[
    'HLT_e24_lhmedium_nod0_L1EM18VH',
    'HLT_e24_lhmedium_nodeta_L1EM18VH',
    'HLT_e24_lhmedium_cutd0dphideta_L1EM18VH',
    'HLT_e24_lhmedium_nodphires_L1EM18VH',
    ]
egamma_Rules_supporting.update(dict(map(None,HLT_list,len(HLT_list)*[{
                      1000 : TriggerRule(PS=10, comment=''),
# as decided at OAB 19/10 to save rate
#    lumi_disable_l1_em18vh : TriggerRule(PS=1, comment='L1 is prescaled by 10'),
    lumi_disable_l1_em18vh : TriggerRule(PS=0, comment='Disabled to save HLT rate',rerun=1),
    }])))

# --------------------------------------

# Exceptionally, e24_(lh)medium_L1EM20VH is kept alive until L1_EM20VH is prescaled
# --> tight is also kept alive (was PS=10 before)

HLT_list=['HLT_e24_tight_L1EM20VH', 'HLT_e24_lhtight_L1EM20VH']
egamma_Rules_supporting.update(dict(map(None,HLT_list,len(HLT_list)*[{
                      1000 : TriggerRule(PS=1, comment='', ESValue=0),
      lumi_l1iso_enable_em : TriggerRule(PS=1, comment='L1 is prescaled by 10, Express', ESRate=0.2),
    }])))

HLT_list=['HLT_e24_lhtight_nod0_L1EM20VH']
egamma_Rules_supporting.update(dict(map(None,HLT_list,len(HLT_list)*[{
                    1000 : TriggerRule(PS=10, comment=''),
    lumi_l1iso_enable_em : TriggerRule(PS=1, comment='L1 is prescaled by 10'),
    }])))

# ========================================
# other support for electron
# ATR-11040

# --------------------------------------
# Data-driven LH pdf's

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
    'HLT_e70_etcut',
    'HLT_e80_etcut',
    'HLT_e100_etcut',
    'HLT_e120_etcut',
    ]
egamma_Rules_supporting.update(dict(map(None,HLT_list,len(HLT_list)*[{
    1000 : TriggerRule(rate=1.0, comment=''), 
    }])))

HLT_list=[
    'HLT_e5_vloose',        'HLT_e5_lhvloose_nod0',
    'HLT_e10_vloose_L1EM7', 'HLT_e10_lhvloose_L1EM7',
    'HLT_e15_vloose_L1EM7', 'HLT_e15_lhvloose_L1EM7',
    'HLT_e20_vloose_L1EM12','HLT_e20_lhvloose_L1EM12',
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
    1000 : TriggerRule(rate=0.2, comment='',rerun=1), 
    }])))

# SUSY strong-1l request
HLT_list=['HLT_e5_lhvloose']
egamma_Rules_supporting.update(dict(map(None,HLT_list,len(HLT_list)*[{
    1000 : TriggerRule(rate=1.0, comment='',rerun=1), 
    }])))

# --------------------------------------
# physics - background estimation

egamma_Rules_supporting.update({
    'HLT_e12_vloose_L1EM10VH'         : {  1000 : TriggerRule(PS=4000, comment='1 Hz target at 3e33', rerun=1) },
    'HLT_e12_lhvloose_L1EM10VH'       : {  1000 : TriggerRule(PS=2200, comment='1 Hz target at 3e33', rerun=1) },
    'HLT_e12_lhvloose_nod0_L1EM10VH'  : {  1000 : TriggerRule(PS=2200, comment='1 Hz target at 3e33', rerun=1) },

    'HLT_e15_vloose_L1EM13VH'         : {  1000 : TriggerRule(PS=1350, comment='1 Hz target at 3e33', rerun=1) },
    'HLT_e15_lhvloose_L1EM13VH'       : {  1000 : TriggerRule(PS=760, comment='1 Hz target at 3e33', rerun=1) },
    'HLT_e15_lhvloose_nod0_L1EM13VH'  : {  1000 : TriggerRule(PS=760, comment='1 Hz target at 3e33', rerun=1) },

    'HLT_e17_vloose'                  : {  1000 : TriggerRule(PS=820, comment='1 Hz target at 3e33', rerun=1) },
    'HLT_e17_lhvloose'                : {  1000 : TriggerRule(PS=480, comment='1 Hz target at 3e33', rerun=1) },
    'HLT_e17_lhvloose_nod0'           : {  1000 : TriggerRule(PS=480, comment='1 Hz target at 3e33', rerun=1) },

    'HLT_e20_vloose'                  : {  1000 : TriggerRule(PS=620, comment='1 Hz target at 3e33', rerun=1) },
    'HLT_e20_lhvloose'                : {  1000 : TriggerRule(PS=400, comment='1 Hz target at 3e33', rerun=1) },
    'HLT_e20_lhvloose_nod0'           : {  1000 : TriggerRule(PS=400, comment='1 Hz target at 3e33', rerun=1) },

    'HLT_e40_vloose'                  : {  1000 : TriggerRule(PS=60, comment='1 Hz target at 3e33', rerun=1) },
    'HLT_e40_lhvloose'                : {  1000 : TriggerRule(PS=45, comment='1 Hz target at 3e33', rerun=1) },
    'HLT_e40_lhvloose_nod0'           : {  1000 : TriggerRule(PS=45, comment='1 Hz target at 3e33', rerun=1) },
    
    'HLT_e60_vloose'                  : {  1000 : TriggerRule(PS=15, comment='1 Hz target at 3e33', rerun=1) },
    'HLT_e60_lhvloose'                : {  1000 : TriggerRule(PS=12, comment='1 Hz target at 3e33', rerun=1) },
    #
    #
    'HLT_e24_vloose_L1EM18VH'         : {  1000 : TriggerRule(PS=60, comment='5 Hz target at 3e33', rerun=1) },
    'HLT_e24_lhvloose_L1EM18VH'       : {  1000 : TriggerRule(PS=40, comment='5 Hz target at 3e33', rerun=1) },
    'HLT_e24_lhvloose_nod0_L1EM18VH'  : {  1000 : TriggerRule(PS=40, comment='5 Hz target at 3e33', rerun=1) },

    'HLT_e24_vloose_L1EM20VH'         : {  1000 : TriggerRule(PS=55, comment='5 Hz target at 3e33', rerun=1) },
    'HLT_e24_lhvloose_L1EM20VH'       : {  1000 : TriggerRule(PS=35, comment='5 Hz target at 3e33', rerun=1) },
    'HLT_e24_lhvloose_nod0_L1EM20VH'  : {  1000 : TriggerRule(PS=35, comment='5 Hz target at 3e33', rerun=1) },

    'HLT_e26_vloose_L1EM20VH'         : {  1000 : TriggerRule(PS=190, comment='5 Hz target at 1.2e34', rerun=1) },
    'HLT_e26_lhvloose_L1EM20VH'       : {  1000 : TriggerRule(PS=125, comment='5 Hz target at 1.2e34', rerun=1) },
    'HLT_e26_lhvloose_nod0_L1EM20VH'  : {  1000 : TriggerRule(PS=125, comment='5 Hz target at 1.2e34', rerun=1) },
})
    
# ========================================
# rerun

HLT_list=[
    'HLT_e4_etcut',
    'HLT_e9_etcut',
    'HLT_e14_etcut',
    ## to come ??
    ## 'HLT_e13_etcut_trkcut',
    ## 'HLT_e18_etcut_trkcut',
    ##
    'HLT_e7_medium',                  'HLT_e7_lhmedium',                 'HLT_e7_lhmedium_nod0',    
    'HLT_e9_loose',                   'HLT_e9_lhloose',                  'HLT_e9_lhloose_nod0',
    'HLT_e9_medium',                  'HLT_e9_lhmedium',                 'HLT_e9_lhmedium_nod0',
    'HLT_e12_medium',                 'HLT_e12_lhmedium',                'HLT_e12_lhmedium_nod0',
    'HLT_e17_medium',                 'HLT_e17_lhmedium',                'HLT_e17_lhmedium_nod0',
    'HLT_e17_medium_L1EM15HI',        'HLT_e17_lhmedium_L1EM15HI',       'HLT_e17_lhmedium_nod0_L1EM15HI',
    'HLT_e17_medium_iloose',          'HLT_e17_lhmedium_iloose',         'HLT_e17_lhmedium_nod0_iloose',
    'HLT_e17_medium_iloose_L1EM15HI', 'HLT_e17_lhmedium_iloose_L1EM15HI',
    'HLT_e20_medium',                 'HLT_e20_lhmedium',                'HLT_e20_lhmedium_nod0',
    'HLT_e24_medium_L1EM15VH',        'HLT_e24_lhmedium_L1EM15VH',       'HLT_e24_lhmedium_nod0_L1EM15VH',
    'HLT_e24_lhmedium_nod0_L1EM20VHI',
    'HLT_e26_medium_L1EM22VHI',       'HLT_e26_lhmedium_L1EM22VHI',      'HLT_e26_lhmedium_nod0_L1EM22VHI',
    'HLT_e60_loose',                  'HLT_e60_lhloose',                 'HLT_e60_lhloose_nod0',
    'HLT_e70_loose',                  'HLT_e70_lhloose',                 'HLT_e70_lhloose_nod0',
    'HLT_e5_tight',                   'HLT_e5_lhtight',                  'HLT_e5_lhtight_nod0',
    'HLT_e9_tight',                   'HLT_e9_lhtight',                  'HLT_e9_lhtight_nod0',
    'HLT_e14_tight',                  'HLT_e14_lhtight',                 'HLT_e14_lhtight_nod0',
    #
    'HLT_e12_lhloose_HLTCalo_L12EM10VH',
    'HLT_e15_lhloose_HLTCalo_L12EM13VH',
    'HLT_e17_lhloose_HLTCalo',
    #
    # missing from list but most likely needed
    'HLT_e5_loose',                          'HLT_e5_lhloose',
    'HLT_e12_loose',                         'HLT_e12_lhloose',                'HLT_e12_lhloose_nod0',
    'HLT_e12_loose_L1EM10VH',                'HLT_e12_lhloose_L1EM10VH',       'HLT_e12_lhloose_nod0_L1EM10VH',    
    'HLT_e12_lhloose_cutd0dphideta_L1EM10VH','HLT_e12_lhloose_nodeta_L1EM10VH','HLT_e12_lhloose_nodphires_L1EM10VH', 
    'HLT_e15_loose_L1EM13VH',                'HLT_e15_lhloose_L1EM13VH',       'HLT_e15_lhloose_nod0_L1EM13VH',
    'HLT_e15_lhloose_cutd0dphideta_L1EM13VH',
    'HLT_e17_loose_L1EM15',                  'HLT_e17_lhloose_L1EM15',         'HLT_e17_lhloose_nod0_L1EM15',
    'HLT_e17_lhloose_cutd0dphideta_L1EM15',  'HLT_e17_lhloose_nodeta_L1EM15',  'HLT_e17_lhloose_nodphires_L1EM15',
    'HLT_e17_loose',                         'HLT_e17_lhloose',                'HLT_e17_lhloose_nod0',
    'HLT_e17_lhloose_cutd0dphideta',         
    'HLT_e26_medium_L1EM20VHI',              'HLT_e26_lhmedium_L1EM20VHI',     'HLT_e26_lhmedium_nod0_L1EM20VHI',
    ]
egamma_Rules_supporting.update(dict(map(None,HLT_list,len(HLT_list)*[{
    1000 : TriggerRule(PS=0, comment='', rerun=1),
    }])))

# ========================================
# photon prescaled

HLT_list=[
    'HLT_g15_loose_L1EM7',
    'HLT_g25_loose_L1EM15',
    'HLT_g35_loose_L1EM15',
    'HLT_g40_loose_L1EM15',
    'HLT_g45_loose_L1EM15',
    'HLT_g50_loose_L1EM15',
    ]
egamma_Rules_supporting.update(dict(map(None,HLT_list,len(HLT_list)*[{
    1000 : TriggerRule(rate=2.0, comment='',rerun=1), 
    }])))

HLT_list=[
    'HLT_g60_loose',
    'HLT_g70_loose',
    'HLT_g80_loose',
    'HLT_g100_loose',
    ]
egamma_Rules_supporting.update(dict(map(None,HLT_list,len(HLT_list)*[{
    1000 : TriggerRule(rate=2.0, comment='',rerun=1),
    }])))


HLT_list=[
    'HLT_g10_loose',    
    'HLT_g20_loose_L1EM12',
    ]

egamma_Rules_supporting.update(dict(map(None,HLT_list,len(HLT_list)*[{
    1000 : TriggerRule(rate=2.0, comment='',rerun=1, PTRate=1.0), 
    }])))


# ========================================
# photon rerun

HLT_list=[
    'HLT_g10_etcut',
    'HLT_g20_etcut_L1EM12',
    #
    'HLT_g10_medium',
    #
    'HLT_g15_loose_L1EM3',
    'HLT_g15_loose',
    'HLT_g20_loose',
    'HLT_g25_loose',
    'HLT_g35_loose',  
    'HLT_g50_loose',
    'HLT_g60_loose_L1EM15VH',
    #
    'HLT_g25_medium', 'HLT_g25_medium_HLTCalo',
    'HLT_g35_medium', 'HLT_g35_medium_HLTCalo',
    #
    'HLT_g20_tight',
    'HLT_g40_tight',
    'HLT_g45_tight',

    'HLT_g20_loose_L1EM15',

    ]
egamma_Rules_supporting.update(dict(map(None,HLT_list,len(HLT_list)*[{
    1000 : TriggerRule(PS=0, comment='rerun', rerun=1), 
    }])))

########################################
# T&P for electron

# --------------------------------------
# Z

egamma_Rules_supporting.update({
    'HLT_e26_lhtight_e15_etcut_Zee'                  : {  1000 : TriggerRule(PS=1, comment='', ESValue=0),
                                          lumi_l1iso_enable_em : TriggerRule(PS=1, comment='Express', ESRate=0.8) },
    'HLT_e26_tight_e15_etcut_Zee'                    : {  1000 : TriggerRule(PS=1, comment='', ESValue=0) },
    # 
    'HLT_e24_lhtight_L1EM20VH_e15_etcut_Zee'         : {  1000 : TriggerRule(PS=1, comment='Express', ESRate=0.8),
                                          lumi_l1iso_enable_em : TriggerRule(PS=-1, comment='Disabled', ESValue=0) },
    'HLT_e24_tight_L1EM20VH_e15_etcut_Zee'           : {  1000 : TriggerRule(PS=1, comment=''),
                                          lumi_l1iso_enable_em : TriggerRule(PS=-1, comment='Disabled') },
    })

# --------------------------------------
# W

L1_list=['L1_EM10_XS20', 'L1_EM15_XS30']
egamma_Rules_supporting.update(dict(map(None,L1_list,len(L1_list)*[{
    1000 : TriggerRule(rate=500, maxRate=500, comment='L1'),
    }])))

egamma_Rules_supporting.update({
    #
    # ATR-11746: needs rerun also to W T&P triggers
    #
    # EM10_XS20
    # eff 0.16
    'HLT_e13_etcut_trkcut_j20_perf_xe15_2dphi05'            : {  1000 : TriggerRule(rate=0.5, comment='', rerun=1) },
    'HLT_e13_etcut_trkcut_j20_perf_xe15_2dphi05_mt25'       : {  1000 : TriggerRule(rate=0.5, comment='', rerun=1) },
    'HLT_e13_etcut_trkcut_j20_perf_xe15_6dphi05'            : {  1000 : TriggerRule(rate=0.5, comment='', rerun=1) },
    'HLT_e13_etcut_trkcut_j20_perf_xe15_6dphi05_mt25'       : {  1000 : TriggerRule(rate=0.5, comment='', rerun=1) },
    # eff 0.11
    'HLT_e13_etcut_trkcut_xs15_j20_perf_xe15_2dphi05'       : {  1000 : TriggerRule(rate=0.5, comment='', rerun=1) },
    'HLT_e13_etcut_trkcut_xs15_j20_perf_xe15_2dphi05_mt25'  : {  1000 : TriggerRule(rate=0.5, comment='', rerun=1) },
    'HLT_e13_etcut_trkcut_xs15_j20_perf_xe15_6dphi05'       : {  1000 : TriggerRule(rate=0.5, comment='', rerun=1) },
    'HLT_e13_etcut_trkcut_xs15_j20_perf_xe15_6dphi05_mt25'  : {  1000 : TriggerRule(rate=0.5, comment='', rerun=1) },
    #
    'HLT_e13_etcut_trkcut_xs15_mt25'                        : {  1000 : TriggerRule(rate=0.5, comment='', rerun=1) },
    'HLT_e13_etcut_trkcut_xs15'                             : {  1000 : TriggerRule(rate=0.5, comment='', rerun=1) },
    #
    # EM15_XS30    
    # eff 0.05
    'HLT_e18_etcut_trkcut_j20_perf_xe20_6dphi15'            : {  1000 : TriggerRule(rate=0.5, comment='', rerun=1) },
    'HLT_e18_etcut_trkcut_j20_perf_xe20_6dphi15_mt35'       : {  1000 : TriggerRule(rate=0.5, comment='', rerun=1) },
    # eff 0.03
    'HLT_e18_etcut_trkcut_xs20_j20_perf_xe20_6dphi15'       : {  1000 : TriggerRule(rate=0.5, comment='', rerun=1) },
    'HLT_e18_etcut_trkcut_xs20_j20_perf_xe20_6dphi15_mt35'  : {  1000 : TriggerRule(rate=0.5, comment='', rerun=1) },
    #
    'HLT_e18_etcut_trkcut_xs20_mt35'                        : {  1000 : TriggerRule(rate=0.5, comment='', rerun=1) },
    'HLT_e18_etcut_trkcut_xs20'                             : {  1000 : TriggerRule(rate=0.5, comment='', rerun=1) },
    })

HLT_list=['HLT_e13_etcut_trkcut_L1EM10', 'HLT_e18_etcut_trkcut_L1EM15']
egamma_Rules_supporting.update(dict(map(None,HLT_list,len(HLT_list)*[{
    1000 : TriggerRule(rate=0.2, comment='', rerun=1),
    }])))

HLT_list=['HLT_xs15_L1XS20', 'HLT_xs20_L1XS30']
egamma_Rules_supporting.update(dict(map(None,HLT_list,len(HLT_list)*[{
    1000 : TriggerRule(PS=0, comment='rerun', rerun=1),
    }])))

# --------------------------------------
# J/psi 

# PS adjustment by ATR-11886
egamma_Rules_supporting.update({
    'L1_2EM3'        : {  1000 : TriggerRule(rate=200, maxRate=200, comment='') },
    'L1_EM7_2EM3'    : {  1000 : TriggerRule(rate=1200, maxRate=1200, comment='') },
    'L1_EM12_2EM3'   : {  1000 : TriggerRule(rate=2600, maxRate=2600, comment='') },
    })

HLT_list=['HLT_e5_lhtight_e4_etcut_Jpsiee']
egamma_Rules_supporting.update(dict(map(None,HLT_list,len(HLT_list)*[{
    1000 : TriggerRule(PS=1, comment='limited at L1, Express', ESRate=0.4), 
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
    1000 : TriggerRule(PS=1, comment='limited at L1'), 
    }])))

#
HLT_list=['HLT_e5_tight_e4_etcut', 'HLT_e5_lhtight_e4_etcut','HLT_e5_lhtight_nod0_e4_etcut']
egamma_Rules_supporting.update(dict(map(None,HLT_list,len(HLT_list)*[{
    1000 : TriggerRule(PS=1, comment='limited at L1'),
    }])))


########################################
# SM W + nJets

egamma_Rules_supporting.update({
    #
    # 2.0e34
    'HLT_e26_vloose_L1EM20VH_3j20noL1'                     : {  1000 : TriggerRule(PS=550, comment='1 Hz target at 1.2e34') },
    'HLT_e26_vloose_L1EM20VH_4j20noL1'                     : {  1000 : TriggerRule(PS=280, comment='1 Hz target at 1.2e34') },
    'HLT_e26_vloose_L1EM20VH_5j15noL1'                     : {  1000 : TriggerRule(PS=320, comment='1 Hz target at 1.2e34') },
    'HLT_e26_vloose_L1EM20VH_6j15noL1'                     : {  1000 : TriggerRule(PS=200, comment='1 Hz target at 1.2e34') },
    'HLT_e26_lhvloose_L1EM20VH_3j20noL1'                   : {  1000 : TriggerRule(PS=350, comment='1 Hz target at 1.2e34') },
    'HLT_e26_lhvloose_L1EM20VH_4j20noL1'                   : {  1000 : TriggerRule(PS=190, comment='1 Hz target at 1.2e34') },
    'HLT_e26_lhvloose_L1EM20VH_5j15noL1'                   : {  1000 : TriggerRule(PS=220, comment='1 Hz target at 1.2e34') },
    'HLT_e26_lhvloose_L1EM20VH_6j15noL1'                   : {  1000 : TriggerRule(PS=140, comment='1 Hz target at 1.2e34') },
    'HLT_e26_lhvloose_nod0_L1EM20VH_3j20noL1'              : {  1000 : TriggerRule(PS=350, comment='1 Hz target at 1.2e34') },
    'HLT_e26_lhvloose_nod0_L1EM20VH_4j20noL1'              : {  1000 : TriggerRule(PS=190, comment='1 Hz target at 1.2e34') },
    'HLT_e26_lhvloose_nod0_L1EM20VH_5j15noL1'              : {  1000 : TriggerRule(PS=220, comment='1 Hz target at 1.2e34') },
    'HLT_e26_lhvloose_nod0_L1EM20VH_6j15noL1'              : {  1000 : TriggerRule(PS=140, comment='1 Hz target at 1.2e34') },
    #
    # 1.5e34
    'HLT_e24_vloose_L1EM20VH_3j20noL1'                     : {  1000 : TriggerRule(PS=150, comment='1 Hz target at 3e33'),
                                                               15001 : TriggerRule(PS=-1, comment='Disabled') },
    'HLT_e24_vloose_L1EM20VH_4j20noL1'                     : {  1000 : TriggerRule(PS=80, comment='1 Hz target at 3e33'),
                                                               15001 : TriggerRule(PS=-1, comment='Disabled') },
    'HLT_e24_vloose_L1EM20VH_5j15noL1'                     : {  1000 : TriggerRule(PS=90, comment='1 Hz target at 3e33'),
                                                               15001 : TriggerRule(PS=-1, comment='Disabled') },
    'HLT_e24_vloose_L1EM20VH_6j15noL1'                     : {  1000 : TriggerRule(PS=55, comment='1 Hz target at 3e33'),
                                                               15001 : TriggerRule(PS=-1, comment='Disabled') },
    'HLT_e24_lhvloose_L1EM20VH_3j20noL1'                   : {  1000 : TriggerRule(PS=100, comment='1 Hz target at 3e33'),
                                                               15001 : TriggerRule(PS=-1, comment='Disabled') },
    'HLT_e24_lhvloose_L1EM20VH_4j20noL1'                   : {  1000 : TriggerRule(PS=50, comment='1 Hz target at 3e33'),
                                                               15001 : TriggerRule(PS=-1, comment='Disabled') },
    'HLT_e24_lhvloose_L1EM20VH_5j15noL1'                   : {  1000 : TriggerRule(PS=60, comment='1 Hz target at 3e33'),
                                                               15001 : TriggerRule(PS=-1, comment='Disabled') },
    'HLT_e24_lhvloose_L1EM20VH_6j15noL1'                   : {  1000 : TriggerRule(PS=35, comment='1 Hz target at 3e33'),
                                                               15001 : TriggerRule(PS=-1, comment='Disabled') },
    'HLT_e24_lhvloose_nod0_L1EM20VH_3j20noL1'              : {  1000 : TriggerRule(PS=100, comment='1 Hz target at 3e33'),
                                                               15001 : TriggerRule(PS=-1, comment='Disabled') },
    'HLT_e24_lhvloose_nod0_L1EM20VH_4j20noL1'              : {  1000 : TriggerRule(PS=50, comment='1 Hz target at 3e33'),
                                                               15001 : TriggerRule(PS=-1, comment='Disabled') },
    'HLT_e24_lhvloose_nod0_L1EM20VH_5j15noL1'              : {  1000 : TriggerRule(PS=60, comment='1 Hz target at 3e33'),
                                                               15001 : TriggerRule(PS=-1, comment='Disabled') },
    'HLT_e24_lhvloose_nod0_L1EM20VH_6j15noL1'              : {  1000 : TriggerRule(PS=35, comment='1 Hz target at 3e33'),
                                                               15001 : TriggerRule(PS=-1, comment='Disabled') },
    #
    # 0.3e34
    'HLT_e24_vloose_L1EM18VH_3j20noL1'                     : {  1000 : TriggerRule(PS=170, comment='1 Hz target at 3e33'),
                                              lumi_disable_l1_em18vh : TriggerRule(PS=-1, comment='Disabled') },
    'HLT_e24_vloose_L1EM18VH_4j20noL1'                     : {  1000 : TriggerRule(PS=88, comment='1 Hz target at 3e33'),
                                              lumi_disable_l1_em18vh : TriggerRule(PS=-1, comment='Disabled') },
    'HLT_e24_vloose_L1EM18VH_5j15noL1'                     : {  1000 : TriggerRule(PS=88, comment='1 Hz target at 3e33'),
                                              lumi_disable_l1_em18vh : TriggerRule(PS=-1, comment='Disabled') },
    'HLT_e24_vloose_L1EM18VH_6j15noL1'                     : {  1000 : TriggerRule(PS=64, comment='1 Hz target at 3e33'),
                                              lumi_disable_l1_em18vh : TriggerRule(PS=-1, comment='Disabled') },
    'HLT_e24_lhvloose_L1EM18VH_3j20noL1'                   : {  1000 : TriggerRule(PS=110, comment='1 Hz target at 3e33', rerun=1),
                                              lumi_disable_l1_em18vh : TriggerRule(PS=-1, comment='Disabled') },
    'HLT_e24_lhvloose_L1EM18VH_4j20noL1'                   : {  1000 : TriggerRule(PS=56, comment='1 Hz target at 3e33'),
                                              lumi_disable_l1_em18vh : TriggerRule(PS=-1, comment='Disabled') },
    'HLT_e24_lhvloose_L1EM18VH_5j15noL1'                   : {  1000 : TriggerRule(PS=66, comment='1 Hz target at 3e33'),
                                              lumi_disable_l1_em18vh : TriggerRule(PS=-1, comment='Disabled') },
    'HLT_e24_lhvloose_L1EM18VH_6j15noL1'                   : {  1000 : TriggerRule(PS=40, comment='1 Hz target at 3e33'),
                                              lumi_disable_l1_em18vh : TriggerRule(PS=-1, comment='Disabled') },
    'HLT_e24_lhvloose_nod0_L1EM18VH_3j20noL1'              : {  1000 : TriggerRule(PS=110, comment='1 Hz target at 3e33'),
                                              lumi_disable_l1_em18vh : TriggerRule(PS=-1, comment='Disabled') },
    'HLT_e24_lhvloose_nod0_L1EM18VH_4j20noL1'              : {  1000 : TriggerRule(PS=56, comment='1 Hz target at 3e33'),
                                              lumi_disable_l1_em18vh : TriggerRule(PS=-1, comment='Disabled') },
    'HLT_e24_lhvloose_nod0_L1EM18VH_5j15noL1'              : {  1000 : TriggerRule(PS=66, comment='1 Hz target at 3e33'),
                                              lumi_disable_l1_em18vh : TriggerRule(PS=-1, comment='Disabled') },
    'HLT_e24_lhvloose_nod0_L1EM18VH_6j15noL1'              : {  1000 : TriggerRule(PS=40, comment='1 Hz target at 3e33'),
                                              lumi_disable_l1_em18vh : TriggerRule(PS=-1, comment='Disabled') },
})

########################################
# other rules

egamma_Rules_supporting.update({

    # ----------------------------------------
    # Di-photon
    #
    # for 2g20_tight
    'HLT_2g20_loose'                               : {  1000 : TriggerRule(PS=10, comment='') },

    'HLT_2g20_loose_L12EM15'                       : {  1000 : TriggerRule(rate=0.2, comment='') },
    'HLT_2g10_loose'                               : {  1000 : TriggerRule(rate=0.2, comment='') },
    'HLT_g20_loose_2g15_loose_L12EM13VH'           : {  1000 : TriggerRule(rate=0.2, comment='') },

    #
    # what supporting ???
    'HLT_g35_medium_g25_medium_L1EM7_EMPTY'        : {  1000 : TriggerRule(PS=1, comment='') },
    'HLT_g35_medium_g25_medium_L1EM7_UNPAIRED_ISO' : {  1000 : TriggerRule(PS=1, comment='') },

    # ----------------------------------------
    # idperf
    'HLT_e5_loose_idperf'                          : {  1000 : TriggerRule(rate=0.5, comment='') },
    'HLT_e5_lhloose_idperf'                        : {  1000 : TriggerRule(rate=0.5, comment='') },
    'HLT_e5_tight_idperf'                          : {  1000 : TriggerRule(rate=0.5, comment='') },
    'HLT_e5_lhtight_idperf'                        : {  1000 : TriggerRule(rate=0.5, comment='') },
    'HLT_e10_tight_idperf'                         : {  1000 : TriggerRule(rate=0.5, comment='') },
    'HLT_e10_lhtight_idperf'                       : {  1000 : TriggerRule(rate=0.5, comment='') },
    'HLT_e24_medium_idperf_L1EM20VH'               : {  1000 : TriggerRule(rate=1.5, comment='Express', ESRate=0.25) },
    'HLT_e24_lhmedium_idperf_L1EM20VH'             : {  1000 : TriggerRule(rate=1.5, comment='Express', ESRate=0.25) },

    # ----------------------------------------
    # perf
    'HLT_g0_perf_L1EM15'    : {  1000 : TriggerRule(rate=1.0, comment='') },
    'HLT_e0_perf_L1EM15'    : {  1000 : TriggerRule(rate=1.0, comment='') },


})


# ----------------------------------------
# di-electron

HLT_list=['HLT_2e17_loose_L12EM15' ,
          'HLT_2e17_lhloose_L12EM15']
egamma_Rules_supporting.update(dict(map(None,HLT_list,len(HLT_list)*[{
                    1000                  : TriggerRule(PS=1, comment=''),
                    lumi_disable_l1_2em15 : TriggerRule(rate=0.2, comment='L1 item prescaled'),
                    }])))


########################################

egamma_Rules={}

RulesList=[egamma_Rules_2e34, egamma_Rules_1e34, egamma_Rules_0p5e34, egamma_Rules_0p3e34, egamma_Rules_supporting]
for Rules in RulesList:
    for newrule in Rules.keys():
        if egamma_Rules.has_key(newrule):
            print 'FATAL     Physics_pp_v5_rules     Duplicated rule inside physics rule. Cannot be added:',newrule
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
    'HLT_g35_loose_L1EM22VHI_mu15noL1_mu2noL1',
    'HLT_g35_loose_L1EM22VHI_mu18noL1',
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

HLT_list=[
    # 2j + xe (+invmass, deta): ATR-12151
    'HLT_j40_0eta490_j30_0eta490_deta25_xe80_L1XE70',
    'HLT_2j40_0eta490_invm250_xe80_L1XE70',
    ]
combined_Rules_2e34.update(dict(map(None,HLT_list,len(HLT_list)*[{
    1000 : TriggerRule(PS=1, comment='', PTRate=1.0), 
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

    # ATR-11303
    # j80_xe80 unique rate 3.7 Hz @ 1.5e34
    # --> keep both j80/100_xe80 unPS until 1.5e34

    # 1.5e34
    'HLT_j100_xe80'                      : {  1000 : TriggerRule(PS=1, comment=''),
                                             15001 : TriggerRule(rate=0.2, comment='Prescaled') },

    'HLT_j80_xe80'                       : {  1000 : TriggerRule(PS=1, comment='Express', ESRate=0.2),
                                             15001 : TriggerRule(rate=0.2, comment='Express', ESValue=1) },

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
            print 'FATAL     Physics_pp_v5_rules     Duplicated rule inside physics rule. Cannot be added:',newrule
    combined_Rules.update(Rules)


#################################################################################################
#
#
# VBF
#
#
#################################################################################################


########################################
# 2e34 

vbf_Rules_2e34={}

HLT_list=['HLT_g25_medium_L1EM22VHI_2j35_0eta490_bmv2c2077_2j35_0eta490', 'HLT_g25_medium_L1EM22VHI_j35_0eta490_bmv2c2077_3j35_0eta490_invm700', ]
vbf_Rules_2e34.update(dict(map(None,HLT_list,len(HLT_list)*[{
    1000 : TriggerRule(PS=1, comment=''), 
    }])))

########################################
# 1e34 

vbf_Rules_1e34={}

HLT_list=['HLT_g25_loose_2j40_0eta490_3j25_0eta490_invm700',]
vbf_Rules_1e34.update(dict(map(None,HLT_list,len(HLT_list)*[{
                               1000 : TriggerRule(PS=1, comment=''), 
    lumi_nonl1topo_disable_vbf_g_5j : TriggerRule(PS=-1, comment='Disabled'),
    }])))

# ----------------------------------------

# 3.8 kHz @ 2e34
L1_list=['L1_MU6_J30.0ETA49_2J20.0ETA49',]
vbf_Rules_1e34.update(dict(map(None,L1_list,len(L1_list)*[{
                             1000 : TriggerRule(PS=1, comment='L1'), 
    lumi_nonl1topo_disable_vbf_mu : TriggerRule(PS=-1, comment='Disabled'),
    }])))

HLT_list=['HLT_mu6_2j40_0eta490_invm1000_L1MU6_J30.0ETA49_2J20.0ETA49',]
vbf_Rules_1e34.update(dict(map(None,HLT_list,len(HLT_list)*[{
                             1000 : TriggerRule(PS=1, comment=''), 
    lumi_nonl1topo_disable_vbf_mu : TriggerRule(PS=-1, comment='Disabled'),
    }])))
# ----------------------------------------

L1_list=['L1_4J20.0ETA49',]
vbf_Rules_1e34.update(dict(map(None,L1_list,len(L1_list)*[{
                               1000 : TriggerRule(PS=1, comment='L1'), 
    lumi_nonl1topo_disable_vbf_bjet : TriggerRule(PS=-1, comment='Disabled'),
    }])))

HLT_list=['HLT_2j55_bmedium_L14J20.0ETA49']
vbf_Rules_1e34.update(dict(map(None,HLT_list,len(HLT_list)*[{
                               1000 : TriggerRule(PS=1, comment=''), 
    lumi_nonl1topo_disable_vbf_bjet : TriggerRule(PS=-1, comment='Disabled'),
    }])))

########################################
# 0.75e34 

vbf_Rules_0p75e34={}

HLT_list=['HLT_g25_medium_L1EM22VHI_4j35_0eta490_invm700']
vbf_Rules_0p75e34.update(dict(map(None,HLT_list,len(HLT_list)*[{
    1000 : TriggerRule(PS=1, comment=''), 
    9501 : TriggerRule(PS=-1, comment='Disabled'),
    }])))

########################################
# 0.5e34 

vbf_Rules_0p5e34={}

HLT_list=['HLT_mu6_2j40_0eta490_invm800_L1MU6_J30.0ETA49_2J20.0ETA49',]
vbf_Rules_0p5e34.update(dict(map(None,HLT_list,len(HLT_list)*[{
    1000 : TriggerRule(PS=1, comment=''), 
    5001 : TriggerRule(PS=-1, comment='Disabled'),
    }])))

HLT_list=['HLT_g25_loose_L1EM20VH_2j40_0eta490_3j25_0eta490_invm700',]
vbf_Rules_0p5e34.update(dict(map(None,HLT_list,len(HLT_list)*[{
                    1000 : TriggerRule(PS=1, comment=''), 
    lumi_nonl1topo_disable_vbf_g_5j : TriggerRule(PS=-1, comment='Disabled'),
#    lumi_l1iso_enable_em : TriggerRule(PS=-1, comment='Disabled'),
    }])))

HLT_list=['HLT_2j55_bloose_L14J20.0ETA49',]
vbf_Rules_1e34.update(dict(map(None,HLT_list,len(HLT_list)*[{
    1000 : TriggerRule(PS=1, comment=''), 
    5001 : TriggerRule(PS=-1, comment='Disabled'),
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

vbf_Rules_supporting={}

# ATR-11486: 0.5-1.0 Hz
HLT_list=['HLT_g20_loose_L1EM18VH_2j40_0eta490_3j25_0eta490']
vbf_Rules_supporting.update(dict(map(None,HLT_list,len(HLT_list)*[{
    1000 : TriggerRule(rate=0.5, comment=''), 
    lumi_nonl1topo_disable_vbf_g_5j : TriggerRule(PS=-1, comment='Disabled'),
    }])))

# ATR-12120: 0.5 Hz -> increas to 1 Hz
HLT_list=['HLT_g25_loose_L1EM20VH_4j35_0eta490']
vbf_Rules_supporting.update(dict(map(None,HLT_list,len(HLT_list)*[{
    1000 : TriggerRule(rate=1., comment=''), 
    }])))

# ----------------------------------------
# MET + VBF
# 1.8 kHz @ 2e34 for XE55 --> prescaled to 10 Hz

vbf_Rules_supporting.update({
    'HLT_2j40_0eta490_invm250_L1XE55'   : {  1000 : TriggerRule(rate=10, comment='Prescaled') },
})

# ----------------------------------------
# b + VBF
vbf_Rules_supporting.update({
    'HLT_2j55_bperf_L14J20.0ETA49'      : {  1000 : TriggerRule(rate=1.0, comment='Prescaled') },
})

########################################

vbf_Rules={}

RulesList=[vbf_Rules_2e34, vbf_Rules_1e34, vbf_Rules_0p75e34, vbf_Rules_0p5e34, vbf_Rules_0p3e34, vbf_Rules_0p15e34, vbf_Rules_supporting] 
for Rules in RulesList:
    for newrule in Rules.keys():
        if vbf_Rules.has_key(newrule):
            print 'FATAL     Physics_pp_v5_rules     Duplicated rule inside physics rule. Cannot be added:',newrule
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
    #
    'HLT_mu6_iloose_mu6_11invm24_noos'                          : {  1000 : TriggerRule(PS=1, comment=''),
                                                          lumi_disable_dy : TriggerRule(PS=-1, comment='') },
    'HLT_mu6_iloose_mu6_24invm60_noos'                          : {  1000 : TriggerRule(PS=1, comment=''),
                                                          lumi_disable_dy : TriggerRule(PS=-1, comment='') },
    'HLT_mu4_iloose_mu4_7invm9_noos'                            : {  1000 : TriggerRule(PS=4, comment=''),
                                                          lumi_disable_dy : TriggerRule(PS=-1, comment='') },
    'HLT_mu4_iloose_mu4_11invm60_noos'                          : {  1000 : TriggerRule(PS=4, comment=''),
                                                          lumi_disable_dy : TriggerRule(PS=-1, comment='') },

    # supporting
    # PS=10 wrt main
    'HLT_mu6_iloose_mu6_11invm24_noos_novtx'                    : {  1000 : TriggerRule(PS=10, comment=''),
                                                          lumi_disable_dy : TriggerRule(PS=-1, comment='') },
    'HLT_mu6_iloose_mu6_24invm60_noos_novtx'                    : {  1000 : TriggerRule(PS=10, comment=''),
                                                          lumi_disable_dy : TriggerRule(PS=-1, comment='') },
    'HLT_mu4_iloose_mu4_7invm9_noos_novtx'                      : {  1000 : TriggerRule(PS=40, comment=''),
                                                          lumi_disable_dy : TriggerRule(PS=-1, comment='') },
    'HLT_mu4_iloose_mu4_11invm60_noos_novtx'                    : {  1000 : TriggerRule(PS=40, comment=''),
                                                          lumi_disable_dy : TriggerRule(PS=-1, comment='') },

    # ========================================
    # exclusive di-lepton

    'HLT_2mu6_10invm30_pt2_z10'                                 : {  1000 : TriggerRule(PS=1, comment='') },

    # ========================================
    # razor

    # xe60 version: unprescaled
    'HLT_j30_xe60_razor170'                                     : {  1000 : TriggerRule(PS=1, comment='') },
    'HLT_j30_xe60_razor185'                                     : {  1000 : TriggerRule(PS=1, comment='') },
    'HLT_j30_xe60_razor195'                                     : {  1000 : TriggerRule(PS=1, comment='') },

    # xe10 version: up to 1e34 (for razor185)
    'HLT_j30_xe10_razor170'                                     : {  1000 : TriggerRule(PS=1, comment=''),
                                                                     5001 : TriggerRule(PS=-1, comment='Disabled') },
    'HLT_j30_xe10_razor185'                                     : {  1000 : TriggerRule(PS=1, comment=''),
                                                                    10001 : TriggerRule(PS=-1, comment='Disabled') },
    'HLT_j30_xe10_razor195'                                     : {  1000 : TriggerRule(PS=1, comment=''),
                                                                    10001 : TriggerRule(PS=-1, comment='Disabled') },
    # supporting
    # ATR-11539/11756: 1 Hz
    'HLT_j30_xe10_razor100'                                     : {  1000 : TriggerRule(rate=1.0, comment='') },
    # for the moment, cost xml for this is a copy of xe10 -> to be updated later
    'HLT_j30_xe60_razor100'                                     : {  1000 : TriggerRule(rate=1.0, comment='') },

    # ========================================
    # LLP

    # ----------------------------------------
    'HLT_j30_muvtx'                                             : {  1000 : TriggerRule(PS=1, comment='') },
    # supporting
    'HLT_j30_muvtx_noiso'                                       : {  1000 : TriggerRule(PS=1, comment='') },
    'HLT_j30_muvtx_L1MU4_EMPTY'                                 : {  1000 : TriggerRule(PS=1, comment='') },
    'HLT_j30_muvtx_L1MU4_UNPAIRED_ISO'                          : {  1000 : TriggerRule(PS=1, comment='') },

    # ----------------------------------------
    #
    'HLT_g15_loose_2mu10_msonly'                                : {  1000 : TriggerRule(PS=1, comment='') },
    # supporting
    'HLT_g15_loose_2mu10_msonly_L1MU4_EMPTY'                    : {  1000 : TriggerRule(PS=1, comment='') },
    'HLT_g15_loose_2mu10_msonly_L1MU4_UNPAIRED_ISO'             : {  1000 : TriggerRule(PS=1, comment='') },

    # ----------------------------------------
    #
    # supporting
    'HLT_3mu6_msonly_L1MU4_EMPTY'                               : {  1000 : TriggerRule(PS=1, comment='') },
    'HLT_3mu6_msonly_L1MU4_UNPAIRED_ISO'                        : {  1000 : TriggerRule(PS=1, comment='') },

    # ----------------------------------------
    # 
    'HLT_j30_jes_PS_llp_L1TAU60'                                : {  1000 : TriggerRule(PS=1, comment='') },
    'HLT_j30_jes_PS_llp_L1TAU40'                                : {  1000 : TriggerRule(PS=-1, comment=''),
                                                    lumi_disable_l1_tau40 : TriggerRule(PS=-1, comment='Disabled') },
    'HLT_j30_jes_PS_llp_L1TAU30'                                : {  1000 : TriggerRule(PS=-1, comment=''),
                                                    lumi_disable_l1_tau30 : TriggerRule(PS=-1, comment='Disabled') },

    'HLT_j30_jes_PS_llp_noiso_L1TAU60'                          : {  1000 : TriggerRule(PS=1, comment='') },
    'HLT_j30_jes_PS_llp_L1TAU8_EMPTY'                           : {  1000 : TriggerRule(PS=10, comment='') }, ### PS value tbc 
    'HLT_j30_jes_PS_llp_L1TAU8_UNPAIRED_ISO'                    : {  1000 : TriggerRule(PS=10, comment='') }, ### PS value tbc 

    # ========================================
    # tau->3mu (narrow scan)
    'HLT_mu11_2mu4noL1_nscan03_L1MU11_2MU6'                     : {  1000 : TriggerRule(PS=1, comment=''),
                                             lumi_nonl1topo_disable_nscan : TriggerRule(rate=0.1, comment='Prescaled') },
    # tau->3mu (narrow scan)
    'HLT_mu11_llns_2mu4noL1_nscan03_L1MU11_2MU6'                     : {  1000 : TriggerRule(PS=1, comment=''),
                                             lumi_nonl1topo_disable_nscan : TriggerRule(rate=0.1, comment='Prescaled') },
    # backup
    'HLT_mu20_mu6noL1_nscan03'                                  : {  1000 : TriggerRule(PS=1, comment=''),
                                             lumi_nonl1topo_disable_nscan : TriggerRule(rate=0.1, comment='Prescaled') },
    # backup
    'HLT_mu20_llns_mu6noL1_nscan03'                                  : {  1000 : TriggerRule(PS=1, comment=''),
                                             lumi_nonl1topo_disable_nscan : TriggerRule(rate=0.1, comment='Prescaled') },
    # 0.5e34
    'HLT_mu11_L1MU10_2mu4noL1_nscan03_L1MU10_2MU6'              : {  1000 : TriggerRule(PS=1, comment=''),
                                                                     5001 : TriggerRule(PS=-1, comment='Disabled') },

    # 
    'HLT_mu20_l2msonly_mu6noL1_nscan03_l2msonly'                     : {  1000 : TriggerRule(PS=-1, comment='') },
    'HLT_mu11_l2msonly_L1MU10_2mu4noL1_nscan03_l2msonly_L1MU10_2MU6' : {  1000 : TriggerRule(PS=-1, comment='') },
    'HLT_mu11_l2msonly_2mu4noL1_nscan03_l2msonly_L1MU11_2MU6'        : {  1000 : TriggerRule(PS=-1, comment='') },

    
    # ========================================
    # Lepton-Jet
    # too high rate...
    'HLT_mu20_msonly_mu6noL1_msonly_nscan05'                    : {  1000 : TriggerRule(PS=1, comment=''),
                                                     lumi_disable_nscan05 : TriggerRule(PS=-1, comment='Disabled') },

    # supporting
    'HLT_mu20_msonly_mu6noL1_msonly_nscan05_L1MU4_EMPTY'        : {  1000 : TriggerRule(PS=1, comment=''),
                                                     lumi_disable_nscan05 : TriggerRule(PS=-1, comment='Disabled') },
    'HLT_mu20_msonly_mu6noL1_msonly_nscan05_L1MU4_UNPAIRED_ISO' : {  1000 : TriggerRule(PS=1, comment=''),
                                                     lumi_disable_nscan05 : TriggerRule(PS=-1, comment='Disabled') },

    # supporting (background, rerun)
    'HLT_mu20_msonly'                                           : {  1000 : TriggerRule(rate=0.2, comment='', rerun=1) },

    # ========================================
    # LFV tau->emu

    # 11 kHz @ 2e34
    'L1_EM8I_MU10'                                               : {  1000 : TriggerRule(PS=1, comment=''),
                                                      lumi_disable_taumass : TriggerRule(rate=500, maxRate=500, comment='Prescaled') },
    # 13 kHz @ 2e34
    'L1_EM15I_MU4'                                               : {  1000 : TriggerRule(PS=1, comment=''),
                                                      lumi_disable_taumass : TriggerRule(rate=500, maxRate=500, comment='Prescaled') },
    # HLT unPS, limited by L1 after 7501
    # 9 Hz @ 2e34
    'HLT_g10_loose_L1EM8I_mu10_iloose_taumass'                   : {  1000 : TriggerRule(PS=1, comment='') },
    # 20 Hz @ 2e34
    'HLT_g20_loose_L1EM15I_mu4_iloose_taumass'                   : {  1000 : TriggerRule(PS=1, comment='') },

    # too high rate (122 Hz, 129 Hz), prescaled
    'HLT_g10_etcut_L1EM8I_mu10_iloose_taumass'                   : {  1000 : TriggerRule(rate=1.0, comment='') },
    'HLT_g20_etcut_L1EM15I_mu4_iloose_taumass'                   : {  1000 : TriggerRule(rate=1.0, comment='') },

    # too high rate (~1 kHz), prescaled
    'HLT_g10_etcut_L1EM8I_mu10_taumass'                          : {  1000 : TriggerRule(rate=1.0, comment='') },
    'HLT_g20_etcut_L1EM15I_mu4_taumass'                          : {  1000 : TriggerRule(rate=1.0, comment='') },


    # ========================================
    # HIP
    'HLT_g0_hiptrt_L1EM22VHI'                                   : {  1000 : TriggerRule(PS=1, comment='') },
    'HLT_g0_hiptrt_L1EM20VHI'                                   : {  1000 : TriggerRule(PS=1, comment=''),
                                                  lumi_disable_l1_em20vhi : TriggerRule(PS=-1, comment='Disabled') },
    'HLT_g0_hiptrt_L1EM20VH'                                    : {  1000 : TriggerRule(PS=1, comment=''),
                                                     lumi_l1iso_enable_em : TriggerRule(PS=-1, comment='Disabled') },
    'HLT_g0_hiptrt_L1EM18VH'                                    : {  1000 : TriggerRule(PS=1, comment=''),
                                                   lumi_disable_l1_em18vh : TriggerRule(PS=-1, comment='Disabled') },

    # ========================================
    # stopped gluino
    # 10 Hz @ 2e34
    'HLT_j55_0eta240_xe50_L1J30_EMPTY'                          : {  1000 : TriggerRule(PS=1, comment='') },

    # ========================================
    # h->phi gamma -> KK gamma
    # 1 Hz @ 2e34
    'HLT_g35_medium_tau25_dikaontight_tracktwo_L1TAU12'         : {  1000 : TriggerRule(PS=1, comment='') },
    'HLT_g35_medium_tau35_dikaontight_tracktwo_L1TAU12'         : {  1000 : TriggerRule(PS=1, comment='') },
    # 10-15 Hz @ 2E34: unPS until 1.5e34/1.75e34 for the moment, to be revisited
    'HLT_g35_medium_tau25_dikaon_tracktwo_L1TAU12'              : {  1000 : TriggerRule(PS=1, comment=''),
                                                                    15001 : TriggerRule(PS=-1, comment='Disabled') },
    'HLT_g35_medium_tau35_dikaon_tracktwo_L1TAU12'              : {  1000 : TriggerRule(PS=1, comment=''),
                                                                    17501 : TriggerRule(PS=-1, comment='Disabled') },
    # supporting
    'HLT_tau25_dikaon_tracktwo'                                 : {  1000 : TriggerRule(PS=0, comment='',rerun=1) },
    'HLT_tau25_dikaontight_tracktwo'                            : {  1000 : TriggerRule(PS=0, comment='',rerun=1) },
    'HLT_tau35_dikaon_tracktwo_L1TAU12'                         : {  1000 : TriggerRule(PS=0, comment='',rerun=1) },
    'HLT_tau35_dikaontight_tracktwo_L1TAU12'                    : {  1000 : TriggerRule(PS=0, comment='',rerun=1) },
}

#################################################################################################
#
#
# Data scouting
#
#
#################################################################################################


datascouting_Rules = {
    'HLT_j0_perf_ds1_L1J100'               : {  1000 : TriggerRule(PS=-1, comment='Disabled'), 
                                 lumi_disable_l1_j75 : TriggerRule(PS=1, comment='Enabled after J75 is prescaled') },
    'HLT_j0_perf_ds1_L1J75'                : {  1000 : TriggerRule(PS=1, comment=''), 
                                 lumi_disable_l1_j75 : TriggerRule(PS=-1, comment='Disabled') },
    }


#################################################################################################
#
#
# monitoring
#
#
#################################################################################################

import Monitoring_pp_v5_rules

zerobias_Rules = {
    # cost file is patched with online measured rate
    # 'L1_ZB'                    : { 1 : TriggerRule(PS=60, comment='PS adjusted to 476b to give 400 Hz') },
    'L1_ZB'                    : { 1 : TriggerRule(rate=400, comment='400 Hz input') },
    'HLT_noalg_zb_L1ZB'        : { 1 : TriggerRule(PS=80, comment='5 Hz target') },
    'HLT_j40_L1ZB'             : { 1 : TriggerRule(PS=1, comment='5 Hz target, HLT rejection is 80') },
    }

tauoverlay_Rules = { 
    'L1_2MU20_OVERLAY'             : { 1 : TriggerRule(rate=5, comment='TauOverlay') },
    'HLT_noalg_to_L12MU20_OVERLAY' : { 1 : TriggerRule(PS=1, comment='TauOverlay') },
    }

beamspot_Rules = {
    # online measurement: unPS (ATR-10984)
    'HLT_beamspot_trkFS_L2StarB'           : { 1 : TriggerRule(PS=-1, comment='BeamSpot') },
    'HLT_beamspot_trkFS_trkfast'           : { 1 : TriggerRule(PS=1, comment='BeamSpot') },

    # offline measurement
    'HLT_beamspot_activeTE_L2StarB_pebTRT' : { 1 : TriggerRule(PS=-1, comment='BeamSpot') },
    'HLT_beamspot_activeTE_trkfast_pebTRT' : { 1 : TriggerRule(rate=200, comment='BeamSpot') },
    'HLT_beamspot_allTE_L2StarB_pebTRT'    : { 1 : TriggerRule(PS=-1, comment='BeamSpot') },
    'HLT_beamspot_allTE_trkfast_pebTRT'    : { 1 : TriggerRule(rate=20, comment='BeamSpot') },
    }

topo_Rules = {
    'L1_LAR-EM':         { 1 : TriggerRule(PS=NumBunches*0.0014, comment='LAREM for testing, request max rate 0.5 Hz')},
    'L1_MJJ-100':        { 1 : TriggerRule(PS=NumBunches*0.2063, comment='L1Topo commissioning, max rate 0.5 Hz')}  ,#  18.4 kHz
    'L1_MJJ-200':        { 1 : TriggerRule(PS=NumBunches*0.0550, comment='L1Topo commissioning, max rate 0.5 Hz')}   ,# 7.7 kHz
    'L1_MJJ-300':        { 1 : TriggerRule(PS=NumBunches*0.0275, comment='L1Topo commissioning, max rate 0.5 Hz')}   ,# 4.2 kHz
    'L1_MJJ-400':        { 1 : TriggerRule(PS=NumBunches*0.0124, comment='L1Topo commissioning, max rate 0.5 Hz')}   ,# 2.6 kHz
    'L1_MJJ-700':        { 1 : TriggerRule(PS=NumBunches*0.0028, comment='L1Topo commissioning, max rate 0.5 Hz')} ,#   974 Hz
    'L1_MJJ-800':        { 1 : TriggerRule(PS=NumBunches*0.0028, comment='L1Topo commissioning, max rate 0.5 Hz')} ,#   681 Hz
    'L1_MJJ-900':        { 1 : TriggerRule(PS=NumBunches*0.0014, comment='L1Topo commissioning, max rate 0.5 Hz')} ,#   535 Hz
    'L1_JPSI-1M5':       { 1 : TriggerRule(PS=NumBunches*17.194, comment='L1Topo commissioning, max rate 0.5 Hz')},#    924 kHz
    'L1_JPSI-1M5-EM7':   { 1 : TriggerRule(PS=NumBunches*1.1692, comment='L1Topo commissioning, max rate 0.5 Hz')} ,#   93 kHz
    'L1_JPSI-1M5-EM12':  { 1 : TriggerRule(PS=NumBunches*0.1926, comment='L1Topo commissioning, max rate 0.5 Hz')}  ,#   34 kHz
    'L1_W-10DPHI-EMXE-0':{ 1 : TriggerRule(PS=NumBunches*1.1692, comment='L1Topo commissioning, max rate 0.5 Hz')}  ,#  94 kHz
    'L1_W-15DPHI-EMXE-0':{ 1 : TriggerRule(PS=NumBunches*0.5915, comment='L1Topo commissioning, max rate 0.5 Hz')} ,#   121 kHz
    'L1_EM10_W-MT25'	:{ 1 : TriggerRule(PS=NumBunches*0.0275, comment="L1Topo commissioning, max rate 0.5 Hz")}  ,#6.20E+04
    'L1_EM10_W-MT30'    :{ 1 : TriggerRule(PS=NumBunches*0.0124, comment="L1Topo commissioning, max rate 0.5 Hz")}  ,#4.70E+04
    'L1_EM15_W-MT35' 	:{ 1 : TriggerRule(PS=NumBunches*0.0055, comment="L1Topo commissioning, max rate 0.5 Hz")}  ,#18792	   
    'L1_W-05RO-XEEMHT'	:{ 1 : TriggerRule(PS=NumBunches*0.0083,comment="L1Topo commissioning, max rate 0.5 Hz")}  ,#130568	
    'L1_W-05DPHI-JXE-0'	:{ 1 : TriggerRule(PS=NumBunches*0.7744,comment="L1Topo commissioning, max rate 0.5 Hz")}  ,#66144	
    'L1_W-10DPHI-JXE-0'	:{ 1 : TriggerRule(PS=NumBunches*0.5915,comment="L1Topo commissioning, max rate 0.5 Hz")}  ,#43828	
    'L1_W-15DPHI-JXE-0'	:{ 1 : TriggerRule(PS=NumBunches*0.4553,comment="L1Topo commissioning, max rate 0.5 Hz")}  ,#29336	
    'L1_W-05RO-XEHT-0'	:{ 1 : TriggerRule(PS=NumBunches*0.0646,comment="L1Topo commissioning, max rate 0.5 Hz")}  ,#79477	
    'L1_W-08RO-XEHT-0'	:{ 1 : TriggerRule(PS=NumBunches*0.0092,comment="L1Topo commissioning, max rate 0.5 Hz")}  ,#40830	
    'L1_W-90RO2-XEHT-0'	:{ 1 : TriggerRule(PS=NumBunches*0.0715,comment="L1Topo commissioning, max rate 0.5 Hz")}  ,#61779	
    'L1_W-250RO2-XEHT-0':{ 1 : TriggerRule(PS=NumBunches*0.0124,comment="L1Topo commissioning, max rate 0.5 Hz")}  ,#16873	

    'L1_DPHI-CJ20XE50'	:{ 1 : TriggerRule(PS=NumBunches*0.0014,comment="L1Topo commissioning, max rate 0.5 Hz")}  ,#10.8	
    'L1_DPHI-J20s2XE50'	:{ 1 : TriggerRule(PS=NumBunches*0.0014,comment="L1Topo commissioning, max rate 0.5 Hz")}  ,#6.5	
    'L1_DPHI-Js2XE50':   { 1 : TriggerRule(PS=NumBunches*0.0014, comment='L1Topo commissioning, max rate 0.5 Hz')} ,#   94 Hz

#    'L1_LAR-EM':         { 1 : TriggerRule(PS=15, comment='LAREM for testing, request max rate 0.5 Hz')},
#    'L1_MJJ-100':        { 1 : TriggerRule(PS=25200, comment='L1Topo commissioning, max rate 0.5 Hz')}  ,#  18.4 kHz
#    'L1_MJJ-200':        { 1 : TriggerRule(PS=15332, comment='L1Topo commissioning, max rate 0.5 Hz')}   ,# 7.7 kHz
#    'L1_MJJ-300':        { 1 : TriggerRule(PS=8400, comment='L1Topo commissioning, max rate 0.5 Hz')}   ,# 4.2 kHz
#    'L1_MJJ-400':        { 1 : TriggerRule(PS=5240, comment='L1Topo commissioning, max rate 0.5 Hz')}   ,# 2.6 kHz
#    'L1_MJJ-700':        { 1 : TriggerRule(PS=1948, comment='L1Topo commissioning, max rate 0.5 Hz')} ,#   974 Hz
#    'L1_MJJ-800':        { 1 : TriggerRule(PS=1362, comment='L1Topo commissioning, max rate 0.5 Hz')} ,#   681 Hz
#    'L1_MJJ-900':        { 1 : TriggerRule(PS=1070, comment='L1Topo commissioning, max rate 0.5 Hz')} ,#   535 Hz
#    'L1_DPHI-Js2XE50':   { 1 : TriggerRule(PS=188, comment='L1Topo commissioning, max rate 0.5 Hz')} ,#   94 Hz
#    'L1_JPSI-1M5':       { 1 : TriggerRule(PS=1848000, comment='L1Topo commissioning, max rate 0.5 Hz')},#    924 kHz
#    'L1_JPSI-1M5-EM7':   { 1 : TriggerRule(PS=186000, comment='L1Topo commissioning, max rate 0.5 Hz')} ,#   93 kHz
#    'L1_JPSI-1M5-EM12':  { 1 : TriggerRule(PS=68000, comment='L1Topo commissioning, max rate 0.5 Hz')}  ,#   34 kHz
#    'L1_W-10DPHI-EMXE-0':{ 1 : TriggerRule(PS=188000, comment='L1Topo commissioning, max rate 0.5 Hz')}  ,#  94 kHz
#    'L1_W-15DPHI-EMXE-0':{ 1 : TriggerRule(PS=242000, comment='L1Topo commissioning, max rate 0.5 Hz')} ,#   121 kHz
#    'L1_EM10_W-MT25'	:{ 1: TriggerRule(PS=124000 ,comment="L1Topo commissioning, max rate 0.5 Hz")}  ,#6.20E+04
#    'L1_EM10_W-MT30'        :{ 1: TriggerRule(PS=94000  ,comment="L1Topo commissioning, max rate 0.5 Hz")}  ,#4.70E+04
#    'L1_EM15_W-MT35' 	:{ 1: TriggerRule(PS=37584  ,comment="L1Topo commissioning, max rate 0.5 Hz")}  ,#18792	
#    'L1_DPHI-J20s2XE50'	:{ 1: TriggerRule(PS=24   ,comment="L1Topo commissioning, max rate 0.5 Hz")}  ,#6.5	
#    'L1_W-05RO-XEEMHT'	:{ 1: TriggerRule(PS=506914 ,comment="L1Topo commissioning, max rate 0.5 Hz")}  ,#130568	
#    'L1_W-05DPHI-JXE-0'	:{ 1: TriggerRule(PS=132288 ,comment="L1Topo commissioning, max rate 0.5 Hz")}  ,#66144	
#    'L1_W-10DPHI-JXE-0'	:{ 1: TriggerRule(PS=87656  ,comment="L1Topo commissioning, max rate 0.5 Hz")}  ,#43828	
#    'L1_W-15DPHI-JXE-0'	:{ 1: TriggerRule(PS=58672  ,comment="L1Topo commissioning, max rate 0.5 Hz")}  ,#29336	
#    'L1_DPHI-CJ20XE50'	:{ 1: TriggerRule(PS=21.6   ,comment="L1Topo commissioning, max rate 0.5 Hz")}  ,#10.8	
#    'L1_W-05RO-XEHT-0'	:{ 1: TriggerRule(PS=158954 ,comment="L1Topo commissioning, max rate 0.5 Hz")}  ,#79477	
#    'L1_W-08RO-XEHT-0'	:{ 1: TriggerRule(PS=81660  ,comment="L1Topo commissioning, max rate 0.5 Hz")}  ,#40830	
#    'L1_W-90RO2-XEHT-0'	:{ 1: TriggerRule(PS=123558 ,comment="L1Topo commissioning, max rate 0.5 Hz")}  ,#61779	
#    'L1_W-250RO2-XEHT-0'	:{ 1: TriggerRule(PS=33746  ,comment="L1Topo commissioning, max rate 0.5 Hz")}  ,#16873	
      
    'HLT_noalg_L1Topo' : { 1 : TriggerRule(PS=1, comment='L1Topo for testing')}
}

l1calo_streamer_Rules = {
    'HLT_noalg_l1calo_L12EM3'    : { 1000 : TriggerRule(rate=0.5, comment='')},
    'HLT_noalg_l1calo_L12MU4'    : { 1000 : TriggerRule(rate=0.5, comment='')},
    'HLT_noalg_l1calo_L12MU6'    : { 1000 : TriggerRule(rate=0.5, comment='')},
    'HLT_noalg_l1calo_L14J15'    : { 1000 : TriggerRule(rate=0.5, comment='')},
    'HLT_noalg_l1calo_L14J20.0ETA49'    : { 1000 : TriggerRule(rate=0.5, comment='')},
    'HLT_noalg_l1calo_L1EM12_2EM3'    : { 1000 : TriggerRule(rate=0.5, comment='')},
    'HLT_noalg_l1calo_L1EM15HI'    : { 1000 : TriggerRule(rate=0.5, comment='')},
    'HLT_noalg_l1calo_L1EM15I_MU4'    : { 1000 : TriggerRule(rate=0.5, comment='')},
    'HLT_noalg_l1calo_L1EM3'    : { 1000 : TriggerRule(rate=0.5, comment='')},
    'HLT_noalg_l1calo_L1EM15'    : { 1000 : TriggerRule(rate=0.5, comment='')},
    'HLT_noalg_l1calo_L1EM7_2EM3'    : { 1000 : TriggerRule(rate=0.5, comment='')},
    'HLT_noalg_l1calo_L1EM8I_MU10'    : { 1000 : TriggerRule(rate=0.5, comment='')},
    'HLT_noalg_l1calo_L1EM7'    : { 1000 : TriggerRule(rate=0.5, comment='')},
    'HLT_noalg_l1calo_L1J100'    : { 1000 : TriggerRule(rate=0.5, comment='')},
    'HLT_noalg_l1calo_L1J15'    : { 1000 : TriggerRule(rate=0.5, comment='')},
    'HLT_noalg_l1calo_L1J20'    : { 1000 : TriggerRule(rate=0.5, comment='')},
    'HLT_noalg_l1calo_L1J25'    : { 1000 : TriggerRule(rate=0.5, comment='')},
    'HLT_noalg_l1calo_L1J30.0ETA49_2J20.0ETA49'    : { 1000 : TriggerRule(rate=0.5, comment='')},
    'HLT_noalg_l1calo_L1J40_XE50'    : { 1000 : TriggerRule(rate=0.5, comment='')},
    'HLT_noalg_l1calo_L1MU10_2MU6'    : { 1000 : TriggerRule(rate=0.5, comment='')},
    'HLT_noalg_l1calo_L1MU10_TAU12IM'    : { 1000 : TriggerRule(rate=0.5, comment='')},
    'HLT_noalg_l1calo_L1MU4'    : { 1000 : TriggerRule(rate=0.5, comment='')},
    'HLT_noalg_l1calo_L1MU4_J30'    : { 1000 : TriggerRule(rate=0.5, comment='')},
    'HLT_noalg_l1calo_L1MU6_2MU4'    : { 1000 : TriggerRule(rate=0.5, comment='')},
    'HLT_noalg_l1calo_L1MU6_J20'    : { 1000 : TriggerRule(rate=0.5, comment='')},
    'HLT_noalg_l1calo_L1TAU12IM'    : { 1000 : TriggerRule(rate=0.5, comment='')},
    'HLT_noalg_l1calo_L1TAU20IM_2TAU12IM'    : { 1000 : TriggerRule(rate=0.5, comment='')},
    'HLT_noalg_l1calo_L1TAU30'    : { 1000 : TriggerRule(rate=0.5, comment='')},
    'HLT_noalg_l1calo_L1TAU40'    : { 1000 : TriggerRule(rate=0.5, comment='')},
    'HLT_noalg_l1calo_L1XE35'    : { 1000 : TriggerRule(rate=0.5, comment='')},
    'HLT_noalg_l1calo_L1XE45'    : { 1000 : TriggerRule(rate=0.5, comment='')},
    'HLT_noalg_l1calo_L1XE55'    : { 1000 : TriggerRule(rate=0.5, comment='')},
    'HLT_noalg_l1calo_L1XE60'    : { 1000 : TriggerRule(rate=0.5, comment='')},
    'HLT_noalg_l1calo_L1XE70'    : { 1000 : TriggerRule(rate=0.5, comment='')},
}

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

HLT_list=['HLT_2mu4_bBmumuxv2_noL2','HLT_2mu4_bBmumux_BcmumuDsloose_noL2','HLT_2mu4_bBmumux_BpmumuKp_noL2','HLT_mu6_mu4_bBmumuxv2_noL2','HLT_mu6_mu4_bBmumux_BcmumuDsloose_noL2','HLT_mu6_mu4_bBmumux_BpmumuKp_noL2','HLT_mu6_iloose_mu6_11invm60_noos', 'HLT_mu6_iloose_mu6_11invm60_noos_novtx', 'HLT_2e12_loose1_L12EM10VH', 'HLT_2e17_loose1', 'HLT_2mu4_bBmumuxv3', 'HLT_2mu4_wOvlpRm_bBmumuxv2', 'HLT_2mu4_wOvlpRm_bDimu', 'HLT_2mu4_wOvlpRm_bDimu_noinvm_novtx_ss', 'HLT_2mu4_wOvlpRm_bDimu_novtx_noos', 'HLT_3j175_jes_PS', 'HLT_3mu6_wOvlpRm_bDimu', 'HLT_4j45_test1', 'HLT_4j45_test2', 'HLT_e5_loose_L2Star_idperf', 'HLT_e0_L2Star_perf_L1EM15', 'HLT_e0_perf_L1EM3_EMPTY', 'HLT_e120_loose1', 'HLT_e17_loose_L1EM15VHJJ1523ETA49', 'HLT_e17_lhloose_L1EM15VHJJ1523ETA49', 'HLT_e17_etcut_L2StarA_L1EM15', 'HLT_e17_etcut_L2StarB_L1EM15', 'HLT_e17_etcut_L2StarC_L1EM15', 'HLT_e24_lhmedium_iloose_L2StarA_L1EM20VH', 'HLT_e24_medium1_L1EM18VH', 'HLT_e24_medium1_L2Star_idperf_L1EM18VH', 'HLT_e24_medium1_idperf_L1EM18VH', 'HLT_e24_medium1_iloose_L1EM18VH', 'HLT_e24_medium1_iloose_L2StarA_L1EM18VH', 'HLT_e24_medium_iloose_L2StarA_L1EM20VH', 'HLT_e24_tight1_iloose', 'HLT_e24_tight1_iloose_L1EM20VH', 'HLT_e26_tight1_iloose', 'HLT_e5_lhloose_L2StarA', 'HLT_e5_loose1_L2Star_idperf', 'HLT_e5_loose1_idperf', 'HLT_e5_loose_L2StarA', 'HLT_e5_tight1_e4_etcut', 'HLT_e5_tight1_e4_etcut_Jpsiee', 'HLT_e5_tight1_e4_etcut_Jpsiee_L1JPSI-1M5', 'HLT_e5_tight1_e4_etcut_L1JPSI-1M5', 'HLT_e60_medium1', 'HLT_eb_empty_L1RD0_EMPTY', 'HLT_eb_high_L1RD2_FILLED', 'HLT_eb_low_L1RD2_FILLED',  'HLT_g0_perf_L1EM3_EMPTY', 'HLT_g120_loose1', 'HLT_g35_loose1_g25_loose1', 'HLT_g35_medium1_g25_medium1', 'HLT_ht0_L1J12_EMPTY', 'HLT_j0_L1J12_EMPTY', 'HLT_j0_lcw_jes_L1J12', 'HLT_j0_perf_bperf_L1J12_EMPTY', 'HLT_j0_perf_bperf_L1MU10', 'HLT_j0_perf_bperf_L1RD0_EMPTY', 'HLT_j0_perf_ds1_L1All', 'HLT_j200_jes_PS', 'HLT_j85_0eta240_test1_j25_240eta490_test1', 'HLT_j85_0eta240_test2_j25_240eta490_test2', 'HLT_j85_2j45', 'HLT_j85_jes_test1', 'HLT_j85_jes_test2', 'HLT_j85_test1', 'HLT_j85_test1_2j45_test1', 'HLT_j85_test2', 'HLT_j85_test2_2j45_test2', 'HLT_mu6_L2Star_idperf', 'HLT_mu24_L2Star_idperf', 'HLT_mu0noL1_fsperf', 'HLT_mu4_r1extr', 'HLT_mu10_r1extr', 'HLT_mu20_r1extr', 'HLT_mu8_mucombTag_noEF_L1MU40', 'HLT_mu10_mucombTag_noEF_L1MU40', 'HLT_mu15_mucombTag_noEF_L1MU40', 'HLT_mu20_iloose_2mu4noL1', 'HLT_mu20_iloose_mu8noL1', 'HLT_mu20_imedium_mu8noL1', 'HLT_mu20_mucombTag_noEF_L1MU40', 'HLT_mu25_mucombTag_noEF_L1MU40', 'HLT_mu4_cosmic_L1MU11_EMPTY', 'HLT_mu4_cosmic_L1MU4_EMPTY', 'HLT_mu4_msonly_cosmic_L1MU11_EMPTY', 'HLT_mu4_msonly_cosmic_L1MU4_EMPTY', 'HLT_tau125_r1medium1', 'HLT_tau125_r1perf', 'HLT_tau1_cosmic_ptonly_L1MU4_EMPTY', 'HLT_tau1_cosmic_track_L1MU4_EMPTY', 'HLT_tau20_r1medium1', 'HLT_tau20_r1perf', 'HLT_tau25_medium1_calo', 'HLT_tau25_medium1_trackcalo', 'HLT_tau25_medium1_tracktwocalo', 'HLT_tau25_perf_calo', 'HLT_tau25_perf_calo_L1TAU12', 'HLT_tau25_perf_trackcalo', 'HLT_tau25_perf_tracktwocalo', 'HLT_tau25_r1perf', 'HLT_tau29_r1medium1', 'HLT_tau29_r1perf', 'HLT_tau35_medium1_calo', 'HLT_tau35_perf_ptonly_tau25_perf_ptonly_L1TAU20IM_2TAU12IM', 'HLT_tau35_perf_tracktwo_L1TAU20_tau25_perf_tracktwo_L1TAU12', 'HLT_tau8_cosmic_ptonly', 'HLT_tau8_cosmic_track', 'HLT_te20', 'HLT_te20_tc_lcw',  'HLT_xe35_L2FS', 'HLT_xe35_l2fsperf_wEFMuFEB_wEFMu', 'HLT_xs30', 'HLT_xs30_tc_lcw', 'HLT_j30_jes_PS_llp_noiso_L1LLP-NOMATCH', 'HLT_j30_jes_PS_llp_L1LLP-NOMATCH', 'HLT_xe35_L2FS_L1XE35_BGRP7', 'HLT_xe35_l2fsperf_wEFMuFEB_wEFMu_L1XE35_BGRP7', 'HLT_e24_lhtight_smooth_L1EM20VH', ]
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
'HLT_noalg_L12EM10VH', 'HLT_noalg_L12EM15', 'HLT_noalg_L12EM3', 'HLT_noalg_L12EM7', 'HLT_noalg_L12EM8VH_MU10', 'HLT_noalg_L12J15_XE55', 'HLT_noalg_L12MU4', 'HLT_noalg_L12MU6', 'HLT_noalg_L13J25.0ETA23', 'HLT_noalg_L13J40', 'HLT_noalg_L13MU4', 'HLT_noalg_L14J15', 'HLT_noalg_L14J20.0ETA49', 'HLT_noalg_L1ALFA_Diff', 'HLT_noalg_L1ALFA_Phys', 'HLT_noalg_L1ALFA_PhysAny', 'HLT_noalg_L1ALFA_SYS', 'HLT_noalg_L1ALFA_TRT_Diff', 'HLT_noalg_L1ALFA_TRT_UNPAIRED_ISO_Diff', 'HLT_noalg_L1All', 'HLT_noalg_L1CALREQ2', 'HLT_noalg_L1Calo', 'HLT_noalg_L1Calo_EMPTY', 'HLT_noalg_L1EM12', 'HLT_noalg_L1EM15', 'HLT_noalg_L1EM15HI_2TAU12IM', 'HLT_noalg_L1EM15HI_2TAU12IM_J25_3J12', 'HLT_noalg_L1EM15HI_2TAU12IM_XE35', 'HLT_noalg_L1EM15HI_TAU40_2TAU15', 'HLT_noalg_L1EM15VH_3EM7', 'HLT_noalg_L1EM15VH_MU10', 'HLT_noalg_L1EM15_MU4', 'HLT_noalg_L1EM18VH', 'HLT_noalg_L1EM20VH', 'HLT_noalg_L1EM3', 'HLT_noalg_L1EM7', 'HLT_noalg_L1EM7_MU10', 'HLT_noalg_L1J15_J15.31ETA49', 'HLT_noalg_L1J20_J20.31ETA49', 'HLT_noalg_L1J30.0ETA49_2J20.0ETA49', 'HLT_noalg_L1J50', 'HLT_noalg_L1J75', 'HLT_noalg_L1J75.31ETA49', 'HLT_noalg_L1LHCF', 'HLT_noalg_L1LHCF_EMPTY', 'HLT_noalg_L1LHCF_UNPAIRED_ISO', 'HLT_noalg_L1LowLumi', 'HLT_noalg_L1MBTS_1_BGRP11', 'HLT_noalg_L1MBTS_1_BGRP9', 'HLT_noalg_L1MBTS_2_BGRP11', 'HLT_noalg_L1MBTS_2_BGRP9', 'HLT_noalg_L1MJJ-100', 'HLT_noalg_L1MU10_TAU12IM', 'HLT_noalg_L1MU10_TAU12IM_J25_2J12', 'HLT_noalg_L1MU10_TAU12IM_XE35', 'HLT_noalg_L1MU10_TAU12_J25_2J12', 'HLT_noalg_L1MU10_TAU20', 'HLT_noalg_L1MU4', 'HLT_noalg_L1MU4_3J15', 'HLT_noalg_L1MU4_J12', 'HLT_noalg_L1MU4_J30', 'HLT_noalg_L1MU6', 'HLT_noalg_L1MU6_J20', 'HLT_noalg_L1MU6_J30.0ETA49_2J20.0ETA49', 'HLT_noalg_L1MU6_J40', 'HLT_noalg_L1MinBias', 'HLT_noalg_L1RD0_BGRP11', 'HLT_noalg_L1RD0_BGRP9', 'HLT_noalg_L1Standby', 'HLT_noalg_L1TAU12', 'HLT_noalg_L1TAU12IL', 'HLT_noalg_L1TAU12IM', 'HLT_noalg_L1TAU12IT', 'HLT_noalg_L1TAU20', 'HLT_noalg_L1TAU20IL', 'HLT_noalg_L1TAU20IM', 'HLT_noalg_L1TAU20IM_2TAU12IM', 'HLT_noalg_L1TAU20IM_2TAU12IM_J25_2J20_3J12', 'HLT_noalg_L1TAU20IM_2TAU12IM_XE35', 'HLT_noalg_L1TAU20IT', 'HLT_noalg_L1TAU20_2J20_XE45', 'HLT_noalg_L1TAU20_2TAU12', 'HLT_noalg_L1TAU20_2TAU12_XE35', 'HLT_noalg_L1TAU30', 'HLT_noalg_L1TAU60', 'HLT_noalg_L1XE50', 'HLT_noalg_bkg_L1J12', 'HLT_noalg_bkg_L1J12_BGRP0', 'HLT_noalg_bkg_L1J30.31ETA49', 'HLT_noalg_bkg_L1J30.31ETA49_BGRP0', 'HLT_noalg_cosmiccalo_L1J12_FIRSTEMPTY', 'HLT_noalg_cosmiccalo_L1J30_FIRSTEMPTY', 'HLT_noalg_cosmicmuons_L1MU11_EMPTY', 'HLT_noalg_cosmicmuons_L1MU4_EMPTY', 'HLT_noalg_dcmmon_L1RD2_BGRP12', 'HLT_noalg_dcmmon_L1RD2_EMPTY', 'HLT_noalg_eb_L1ABORTGAPNOTCALIB_noPS', 'HLT_noalg_eb_L1EMPTY_noPS', 'HLT_noalg_eb_L1FIRSTEMPTY_noPS', 'HLT_noalg_eb_L1PhysicsHigh_noPS', 'HLT_noalg_eb_L1PhysicsLow_noPS', 'HLT_noalg_eb_L1RD0_EMPTY', 'HLT_noalg_eb_L1RD3_EMPTY', 'HLT_noalg_eb_L1RD3_FILLED', 'HLT_noalg_eb_L1UNPAIRED_ISO_noPS', 'HLT_noalg_eb_L1UNPAIRED_NONISO_noPS', 'HLT_noalg_idcosmic_L1TRT_EMPTY', 'HLT_noalg_idcosmic_L1TRT_FILLED', 'HLT_noalg_L1ALFA_ANY', 'HLT_noalg_L1BPH-2M-2MU4', 'HLT_noalg_L1BPH-2M-2MU6', 'HLT_noalg_L1BPH-2M-MU6MU4', 'HLT_noalg_L1BPH-4M8-2MU4', 'HLT_noalg_L1BPH-4M8-2MU6', 'HLT_noalg_L1BPH-4M8-MU6MU4', 'HLT_noalg_L1BPH-DR-2MU4', 'HLT_noalg_L1BPH-DR-2MU6', 'HLT_noalg_L1BPH-DR-MU6MU4', 'HLT_noalg_L1DR-MU10TAU12I', 'HLT_noalg_L1DR-TAU20ITAU12I', 'HLT_noalg_L1DY-BOX-2MU4', 'HLT_noalg_L1DY-BOX-2MU6', 'HLT_noalg_L1DY-BOX-MU6MU4', 'HLT_noalg_L1DY-DR-2MU4', 'HLT_noalg_L1EM10', 'HLT_noalg_L1EM10VH', 'HLT_noalg_L1EM13VH', 'HLT_noalg_L1EM15VH', 'HLT_noalg_L1EM20VHI', 'HLT_noalg_L1EM22VHI', 'HLT_noalg_L1EM8VH', 'HLT_noalg_L1J100', 'HLT_noalg_L1J12', 'HLT_noalg_L1J120', 'HLT_noalg_L1J15', 'HLT_noalg_L1J20', 'HLT_noalg_L1J25', 'HLT_noalg_L1J30', 'HLT_noalg_L1J40', 'HLT_noalg_L1J85', 'HLT_noalg_L1LFV-MU', 'HLT_noalg_L1TAU40', 'HLT_noalg_L1TAU8', 'HLT_noalg_L1TE10', 'HLT_noalg_L1TE10.0ETA24', 'HLT_noalg_L1TE20', 'HLT_noalg_L1TE20.0ETA24', 'HLT_noalg_L1TE30', 'HLT_noalg_L1TE30.0ETA24', 'HLT_noalg_L1TE40', 'HLT_noalg_L1TE50',  'HLT_noalg_L1XE40', 'HLT_noalg_L1XE45', 'HLT_noalg_L1XE55', 'HLT_noalg_L1XE60', 'HLT_noalg_L1XE70', 'HLT_noalg_L1XE80', 'HLT_noalg_L1XS20', 'HLT_noalg_L1XS30', 'HLT_noalg_L1XS40', 'HLT_noalg_L1XS50', 'HLT_noalg_L1XS60', 'HLT_noalg_L1J100.31ETA49', 'HLT_noalg_L1J15.28ETA31', 'HLT_noalg_L1J15.31ETA49', 'HLT_noalg_L1J20.28ETA31', 'HLT_noalg_L1J20.31ETA49', 'HLT_noalg_L1J30.31ETA49', 'HLT_noalg_L1J50.31ETA49', 'HLT_noalg_L1RD1_FILLED', 'HLT_noalg_L1RD2_FILLED', 'HLT_noalg_L1RD2_EMPTY', 'HLT_noalg_L1RD3_FILLED', 'HLT_noalg_L1RD3_EMPTY', 'HLT_noalg_standby_L1RD0_FILLED', 'HLT_noalg_standby_L1RD0_EMPTY',]
disabled_Rules.update(dict(map(None,HLT_list,len(HLT_list)*[{
    1000 : TriggerRule(PS=-1, comment=''), 
    }])))

# --------------------------------------
# beam spot

HLT_list=[
'HLT_beamspot_activeTE_L2StarB_peb', 'HLT_beamspot_activeTE_L2StarB_pebTRT_L1TRT_EMPTY', 'HLT_beamspot_activeTE_L2StarB_pebTRT_L1TRT_FILLED', 'HLT_beamspot_activeTE_L2StarB_peb_L1TRT_EMPTY', 'HLT_beamspot_activeTE_L2StarB_peb_L1TRT_FILLED', 'HLT_beamspot_activeTE_trkfast_peb', 'HLT_beamspot_activeTE_trkfast_pebTRT_L1TRT_EMPTY', 'HLT_beamspot_activeTE_trkfast_pebTRT_L1TRT_FILLED', 'HLT_beamspot_activeTE_trkfast_peb_L1TRT_EMPTY', 'HLT_beamspot_activeTE_trkfast_peb_L1TRT_FILLED', 'HLT_beamspot_allTE_L2StarB', 'HLT_beamspot_allTE_L2StarB_L1TRT_EMPTY', 'HLT_beamspot_allTE_L2StarB_L1TRT_FILLED', 'HLT_beamspot_allTE_L2StarB_peb', 'HLT_beamspot_allTE_L2StarB_pebTRT_L1TRT_EMPTY', 'HLT_beamspot_allTE_L2StarB_pebTRT_L1TRT_FILLED', 'HLT_beamspot_allTE_L2StarB_peb_L1TRT_EMPTY', 'HLT_beamspot_allTE_L2StarB_peb_L1TRT_FILLED', 'HLT_beamspot_allTE_trkfast', 'HLT_beamspot_allTE_trkfast_L1TRT_EMPTY', 'HLT_beamspot_allTE_trkfast_L1TRT_FILLED', 'HLT_beamspot_allTE_trkfast_peb', 'HLT_beamspot_allTE_trkfast_pebTRT_L1TRT_EMPTY', 'HLT_beamspot_allTE_trkfast_pebTRT_L1TRT_FILLED', 'HLT_beamspot_allTE_trkfast_peb_L1TRT_EMPTY', 'HLT_beamspot_allTE_trkfast_peb_L1TRT_FILLED', 'HLT_beamspot_trkFS_L2StarB_L1TRT_EMPTY', 'HLT_beamspot_trkFS_L2StarB_L1TRT_FILLED', 'HLT_beamspot_trkFS_trkfast_L1TRT_EMPTY', 'HLT_beamspot_trkFS_trkfast_L1TRT_FILLED', ]
disabled_Rules.update(dict(map(None,HLT_list,len(HLT_list)*[{
    1000 : TriggerRule(PS=-1, comment=''), 
    }])))

# --------------------------------------
# calibration

HLT_list=[
'HLT_alfacalib_L1ALFA_A7L1_OD', 'HLT_alfacalib_L1ALFA_A7R1_OD', 'HLT_alfacalib_L1ALFA_B7L1_OD', 'HLT_alfacalib_L1ALFA_B7R1_OD', 'HLT_alfacalib_L1ALFA_ELAS', 'HLT_alfacalib_L1ALFA_ELAST15', 'HLT_alfacalib_L1ALFA_ELAST18', 'HLT_alfacalib_L1ALFA_SYS', 'HLT_alfacalib_L1ALFA_TRT', 'HLT_alfacalib_L1LHCF_ALFA_ANY_A', 'HLT_alfacalib_L1LHCF_ALFA_ANY_A_UNPAIRED_ISO', 'HLT_alfacalib_L1LHCF_ALFA_ANY_C', 'HLT_alfacalib_L1LHCF_ALFA_ANY_C_UNPAIRED_ISO', 'HLT_alfaidpeb_L1TRT_ALFA_ANY', 'HLT_cscmon_L1EM3', 'HLT_cscmon_L1J12', 'HLT_cscmon_L1MU10', 'HLT_ibllumi_L1RD0_ABORTGAPNOTCALIB', 'HLT_ibllumi_L1RD0_FILLED', 'HLT_ibllumi_L1RD0_UNPAIRED_ISO', 'HLT_id_cosmicid', 'HLT_id_cosmicid_trtxk', 'HLT_id_cosmicid_trtxk_central', 'HLT_l1calocalib', 'HLT_l1calocalib_L1BGRP9', 'HLT_larnoiseburst_L1All', 'HLT_larnoiseburst_loose_L1All', 'HLT_larnoiseburst_loose_rerun', 'HLT_lumipeb_vdm_L1MBTS_1', 'HLT_lumipeb_vdm_L1MBTS_1_BGRP11', 'HLT_lumipeb_vdm_L1MBTS_1_BGRP9', 'HLT_lumipeb_vdm_L1MBTS_1_UNPAIRED_ISO', 'HLT_lumipeb_vdm_L1MBTS_2', 'HLT_lumipeb_vdm_L1MBTS_2_BGRP11', 'HLT_lumipeb_vdm_L1MBTS_2_BGRP9', 'HLT_lumipeb_vdm_L1MBTS_2_UNPAIRED_ISO', 'HLT_lumipeb_vdm_L1RD0_BGRP11', 'HLT_lumipeb_vdm_L1RD0_BGRP9', 'HLT_lumipeb_vdm_L1RD0_FILLED', 'HLT_lumipeb_vdm_L1RD0_UNPAIRED_ISO',]
disabled_Rules.update(dict(map(None,HLT_list,len(HLT_list)*[{
    1000 : TriggerRule(PS=-1, comment=''), 
    }])))

# --------------------------------------
# minbias

HLT_list=[
'HLT_mb_mbts_L1MBTS_1', 'HLT_mb_mbts_L1MBTS_1_1', 'HLT_mb_mbts_L1MBTS_1_1_EMPTY', 'HLT_mb_mbts_L1MBTS_1_1_UNPAIRED_ISO', 'HLT_mb_mbts_L1MBTS_1_EMPTY', 'HLT_mb_mbts_L1MBTS_1_UNPAIRED_ISO', 'HLT_mb_mbts_L1MBTS_2', 'HLT_mb_mbts_L1MBTS_2_EMPTY', 'HLT_mb_mbts_L1MBTS_2_UNPAIRED_ISO', 'HLT_mb_perf_L1LUCID', 'HLT_mb_perf_L1LUCID_EMPTY', 'HLT_mb_perf_L1LUCID_UNPAIRED_ISO', 'HLT_mb_perf_L1MBTS_2', 'HLT_mb_perf_L1RD1_FILLED', 'HLT_mb_sp1000_trk70_hmt_L1MBTS_1_1', 'HLT_mb_sp1200_trk75_hmt_L1MBTS_1_1', 'HLT_mb_sp1400_hmtperf_L1TE10', 'HLT_mb_sp1400_hmtperf_L1TE10.0ETA24', 'HLT_mb_sp1400_hmtperf_L1TE20', 'HLT_mb_sp1400_hmtperf_L1TE20.0ETA24', 'HLT_mb_sp1400_trk100_hmt_L1TE20', 'HLT_mb_sp1400_trk100_hmt_L1TE20.0ETA24', 'HLT_mb_sp1400_trk80_hmt_L1MBTS_1_1', 'HLT_mb_sp1400_trk90_hmt_L1TE10', 'HLT_mb_sp1400_trk90_hmt_L1TE10.0ETA24', 'HLT_mb_sp1800_hmtperf', 'HLT_mb_sp1800_hmtperf_L1TE20', 'HLT_mb_sp2000_pusup600_trk70_hmt', 'HLT_mb_sp2000_pusup600_trk70_hmt_L1TE30', 'HLT_mb_sp2000_pusup600_trk70_hmt_L1TE40', 'HLT_mb_sp2000_pusup700_trk50_sumet110_hmt_L1TE40', 'HLT_mb_sp2000_pusup700_trk50_sumet150_hmt_L1TE50', 'HLT_mb_sp2000_pusup700_trk50_sumet70_hmt_L1TE30', 'HLT_mb_sp2000_pusup700_trk60_hmt_L1TE30', 'HLT_mb_sp2000_pusup700_trk70_hmt_L1TE30', 'HLT_mb_sp2000_pusup700_trk70_hmt_L1TE40', 'HLT_mb_sp2000_trk70_hmt', 'HLT_mb_sp2500_hmtperf_L1TE20', 'HLT_mb_sp2500_pusup750_trk90_hmt_L1TE40', 'HLT_mb_sp2_hmtperf', 'HLT_mb_sp2_hmtperf_L1MBTS_1_1', 'HLT_mb_sp3000_pusup800_trk120_hmt_L1TE50', 'HLT_mb_sp300_trk10_sumet40_hmt_L1MBTS_1_1', 'HLT_mb_sp300_trk10_sumet50_hmt_L1MBTS_1_1', 'HLT_mb_sp300_trk10_sumet50_hmt_L1RD3_FILLED', 'HLT_mb_sp300_trk10_sumet50_hmt_L1TE10', 'HLT_mb_sp300_trk10_sumet60_hmt_L1MBTS_1_1', 'HLT_mb_sp300_trk10_sumet60_hmt_L1TE20', 'HLT_mb_sp300_trk10_sumet70_hmt_L1MBTS_1_1', 'HLT_mb_sp300_trk10_sumet80_hmt_L1MBTS_1_1', 'HLT_mb_sp400_trk40_hmt_L1MBTS_1_1', 'HLT_mb_sp500_hmtperf', 'HLT_mb_sp600_trk10_sumet40_hmt_L1MBTS_1_1', 'HLT_mb_sp600_trk10_sumet50_hmt_L1MBTS_1_1', 'HLT_mb_sp600_trk10_sumet50_hmt_L1RD3_FILLED', 'HLT_mb_sp600_trk10_sumet50_hmt_L1TE10.0ETA24', 'HLT_mb_sp600_trk10_sumet60_hmt_L1MBTS_1_1', 'HLT_mb_sp600_trk10_sumet60_hmt_L1TE20.0ETA24', 'HLT_mb_sp600_trk10_sumet70_hmt_L1MBTS_1_1', 'HLT_mb_sp600_trk10_sumet80_hmt_L1MBTS_1_1', 'HLT_mb_sp600_trk45_hmt_L1MBTS_1_1', 'HLT_mb_sp700_trk50_hmt_L1MBTS_1_1', 'HLT_mb_sp700_trk50_hmt_L1RD3_FILLED', 'HLT_mb_sp700_trk55_hmt_L1MBTS_1_1', 'HLT_mb_sp900_trk60_hmt_L1MBTS_1_1', 'HLT_mb_sp900_trk65_hmt_L1MBTS_1_1', 'HLT_mb_sptrk','HLT_mb_sptrk_L1RD0_EMPTY', 'HLT_mb_sptrk_L1RD0_UNPAIRED_ISO', 'HLT_mb_sptrk_L1RD3_FILLED', 'HLT_mb_sptrk_costr', 'HLT_mb_sptrk_costr_L1RD0_EMPTY', 'HLT_mb_sptrk_noisesup', 'HLT_mb_sptrk_noisesup_L1RD0_EMPTY', 'HLT_mb_sptrk_noisesup_L1RD0_UNPAIRED_ISO', 'HLT_mb_sptrk_noisesup_L1RD3_FILLED', 'HLT_mb_sptrk_pt4_L1MBTS_1', 'HLT_mb_sptrk_pt4_L1MBTS_1_1', 'HLT_mb_sptrk_pt4_L1MBTS_2', 'HLT_mb_sptrk_pt4_L1RD3_FILLED', 'HLT_mb_sptrk_pt6_L1MBTS_1', 'HLT_mb_sptrk_pt6_L1MBTS_1_1', 'HLT_mb_sptrk_pt6_L1MBTS_2', 'HLT_mb_sptrk_pt6_L1RD3_FILLED', 'HLT_mb_sptrk_pt8_L1MBTS_1', 'HLT_mb_sptrk_pt8_L1MBTS_1_1', 'HLT_mb_sptrk_pt8_L1MBTS_2', 'HLT_mb_sptrk_pt8_L1RD3_FILLED', 'HLT_mb_sptrk_vetombts2in_L1ALFA_CEP', 'HLT_mb_sptrk_vetombts2in_peb_L1ALFA_ANY', 'HLT_mb_sptrk_vetombts2in_peb_L1ALFA_ANY_UNPAIRED_ISO', 'HLT_noalg_mb_L1LUCID', 'HLT_noalg_mb_L1LUCID_EMPTY', 'HLT_noalg_mb_L1LUCID_UNPAIRED_ISO', 'HLT_noalg_mb_L1MBTS_1', 'HLT_noalg_mb_L1MBTS_1_1', 'HLT_noalg_mb_L1MBTS_1_1_EMPTY', 'HLT_noalg_mb_L1MBTS_1_1_UNPAIRED_ISO', 'HLT_noalg_mb_L1MBTS_1_EMPTY', 'HLT_noalg_mb_L1MBTS_1_UNPAIRED_ISO', 'HLT_noalg_mb_L1MBTS_2', 'HLT_noalg_mb_L1MBTS_2_EMPTY', 'HLT_noalg_mb_L1MBTS_2_UNPAIRED_ISO', 'HLT_noalg_mb_L1RD0_EMPTY', 'HLT_noalg_mb_L1RD0_FILLED', 'HLT_noalg_mb_L1RD0_UNPAIRED_ISO', 'HLT_noalg_mb_L1RD1_FILLED', 'HLT_noalg_mb_L1RD2_EMPTY', 'HLT_noalg_mb_L1RD2_FILLED', 'HLT_noalg_mb_L1RD3_EMPTY', 'HLT_noalg_mb_L1RD3_FILLED', 'HLT_noalg_mb_L1TE10', 'HLT_noalg_mb_L1TE10.0ETA24', 'HLT_noalg_mb_L1TE20', 'HLT_noalg_mb_L1TE20.0ETA24', 'HLT_noalg_mb_L1TE30', 'HLT_noalg_mb_L1TE40', 'HLT_noalg_mb_L1TE50','HLT_mb_sp1800_hmtperf_L1TE30.0ETA24','HLT_noalg_mb_L1TE30.0ETA24','HLT_noalg_L1TE0', 'HLT_noalg_L1TE0.0ETA24', 'HLT_noalg_L1TE15', 'HLT_noalg_L1TE5', 'HLT_noalg_L1TE5.0ETA24', 'HLT_noalg_mb_L1TE0', 'HLT_noalg_mb_L1TE0.0ETA24', 'HLT_noalg_mb_L1TE15', 'HLT_noalg_mb_L1TE5', 'HLT_noalg_mb_L1TE5.0ETA24', 'HLT_mb_sp1300_hmtperf_L1TE10', 'HLT_mb_sp1300_hmtperf_L1TE10.0ETA24', 'HLT_mb_sp1300_hmtperf_L1TE15', 'HLT_mb_sp1300_hmtperf_L1TE20', 'HLT_mb_sp1300_hmtperf_L1TE20.0ETA24', 'HLT_mb_sp1300_hmtperf_L1TE5', 'HLT_mb_sp1300_hmtperf_L1TE5.0ETA24', 'HLT_mb_sp1400_pusup550_trk90_hmt_L1TE10', 'HLT_mb_sp1400_pusup550_trk90_hmt_L1TE10.0ETA24', 'HLT_mb_sp1400_pusup550_trk90_hmt_L1TE15', 'HLT_mb_sp1400_pusup550_trk90_hmt_L1TE20', 'HLT_mb_sp1400_pusup550_trk90_hmt_L1TE20.0ETA24', 'HLT_mb_sp1400_pusup550_trk90_hmt_L1TE5', 'HLT_mb_sp1400_pusup550_trk90_hmt_L1TE5.0ETA24', 'HLT_mb_sp1400_trk90_hmt_L1TE15', 'HLT_mb_sp1400_trk90_hmt_L1TE20', 'HLT_mb_sp1400_trk90_hmt_L1TE20.0ETA24', 'HLT_mb_sp1400_trk90_hmt_L1TE5', 'HLT_mb_sp1400_trk90_hmt_L1TE5.0ETA24', 'HLT_mb_sp1500_hmtperf_L1TE10', 'HLT_mb_sp1500_hmtperf_L1TE10.0ETA24', 'HLT_mb_sp1500_hmtperf_L1TE15', 'HLT_mb_sp1500_hmtperf_L1TE20', 'HLT_mb_sp1500_hmtperf_L1TE20.0ETA24', 'HLT_mb_sp1500_hmtperf_L1TE5', 'HLT_mb_sp1500_hmtperf_L1TE5.0ETA24', 'HLT_mb_sp1600_pusup550_trk100_hmt_L1TE10', 'HLT_mb_sp1600_pusup550_trk100_hmt_L1TE10.0ETA24', 'HLT_mb_sp1600_pusup550_trk100_hmt_L1TE15', 'HLT_mb_sp1600_pusup550_trk100_hmt_L1TE20', 'HLT_mb_sp1600_pusup550_trk100_hmt_L1TE20.0ETA24', 'HLT_mb_sp1600_pusup550_trk100_hmt_L1TE5', 'HLT_mb_sp1600_pusup550_trk100_hmt_L1TE5.0ETA24', 'HLT_mb_sp1600_trk100_hmt_L1TE10', 'HLT_mb_sp1600_trk100_hmt_L1TE10.0ETA24', 'HLT_mb_sp1600_trk100_hmt_L1TE15', 'HLT_mb_sp1600_trk100_hmt_L1TE20', 'HLT_mb_sp1600_trk100_hmt_L1TE20.0ETA24', 'HLT_mb_sp1600_trk100_hmt_L1TE5', 'HLT_mb_sp1600_trk100_hmt_L1TE5.0ETA24', 'HLT_mb_sp1700_hmtperf_L1TE10', 'HLT_mb_sp1700_hmtperf_L1TE10.0ETA24', 'HLT_mb_sp1700_hmtperf_L1TE15', 'HLT_mb_sp1700_hmtperf_L1TE20', 'HLT_mb_sp1700_hmtperf_L1TE20.0ETA24', 'HLT_mb_sp1700_hmtperf_L1TE5', 'HLT_mb_sp1700_hmtperf_L1TE5.0ETA24', 'HLT_mb_sp1800_pusup600_trk110_hmt_L1TE10', 'HLT_mb_sp1800_pusup600_trk110_hmt_L1TE10.0ETA24', 'HLT_mb_sp1800_pusup600_trk110_hmt_L1TE15', 'HLT_mb_sp1800_pusup600_trk110_hmt_L1TE20', 'HLT_mb_sp1800_pusup600_trk110_hmt_L1TE20.0ETA24', 'HLT_mb_sp1800_pusup600_trk110_hmt_L1TE5', 'HLT_mb_sp1800_pusup600_trk110_hmt_L1TE5.0ETA24', 'HLT_mb_sp1800_trk110_hmt_L1TE10', 'HLT_mb_sp1800_trk110_hmt_L1TE10.0ETA24', 'HLT_mb_sp1800_trk110_hmt_L1TE15', 'HLT_mb_sp1800_trk110_hmt_L1TE20', 'HLT_mb_sp1800_trk110_hmt_L1TE20.0ETA24', 'HLT_mb_sp1800_trk110_hmt_L1TE5', 'HLT_mb_sp1800_trk110_hmt_L1TE5.0ETA24', 'HLT_mb_sp2_hmtperf_L1TE0', 'HLT_mb_sp2_hmtperf_L1TE0.0ETA24', 'HLT_mb_sp900_pusup350_trk60_hmt_L1TE10', 'HLT_mb_sp900_pusup350_trk60_hmt_L1TE10.0ETA24', 'HLT_mb_sp900_pusup350_trk60_hmt_L1TE5', 'HLT_mb_sp900_pusup350_trk60_hmt_L1TE5.0ETA24', 'HLT_mb_sp900_trk60_hmt_L1TE10', 'HLT_mb_sp900_trk60_hmt_L1TE10.0ETA24', 'HLT_mb_sp900_trk60_hmt_L1TE5', 'HLT_mb_sp900_trk60_hmt_L1TE5.0ETA24', 'HLT_lumipeb_L1MBTS_2', 'HLT_mb_sptrk_vetombts2in_L1TRT_ALFA_EINE', 'HLT_mb_sptrk_vetombts2in_peb_L1TRT_ALFA_ANY', 'HLT_mb_sptrk_vetombts2in_peb_L1TRT_ALFA_ANY_UNPAIRED_ISO','mb_sptrk_vetombts2in_L1TRT_ALFA_EINE','mb_sptrk_vetombts2in_peb_L1TRT_ALFA_ANY','mb_sptrk_vetombts2in_peb_L1TRT_ALFA_ANY_UNPAIRED_ISO','lumipeb_L1MBTS_2', 'HLT_lumipeb_L1ALFA_BGT', 'HLT_lumipeb_L1ALFA_BGT_BGRP10', 'HLT_lumipeb_L1ALFA_BGT_UNPAIRED_ISO', 'HLT_lumipeb_L1MBTS_1', 'HLT_lumipeb_L1MBTS_1_UNPAIRED_ISO', 'HLT_lumipeb_L1MBTS_2_UNPAIRED_ISO', 'HLT_mb_sp1300_hmtperf_L1TE3', 'HLT_mb_sp1300_hmtperf_L1TE3.0ETA24', 'HLT_mb_sp1400_pusup550_trk90_hmt_L1TE25', 'HLT_mb_sp1500_pusup700_trk100_hmt_L1TE10', 'HLT_mb_sp1500_pusup700_trk100_hmt_L1TE10.0ETA24', 'HLT_mb_sp1500_pusup700_trk100_hmt_L1TE15', 'HLT_mb_sp1500_pusup700_trk100_hmt_L1TE20', 'HLT_mb_sp1500_pusup700_trk100_hmt_L1TE20.0ETA24', 'HLT_mb_sp1500_pusup700_trk100_hmt_L1TE25', 'HLT_mb_sp1500_pusup700_trk100_hmt_L1TE5', 'HLT_mb_sp1500_pusup700_trk100_hmt_L1TE5.0ETA24', 'HLT_mb_sp1500_trk100_hmt_L1TE10', 'HLT_mb_sp1500_trk100_hmt_L1TE10.0ETA24', 'HLT_mb_sp1500_trk100_hmt_L1TE15', 'HLT_mb_sp1500_trk100_hmt_L1TE20', 'HLT_mb_sp1500_trk100_hmt_L1TE20.0ETA24', 'HLT_mb_sp1500_trk100_hmt_L1TE5', 'HLT_mb_sp1500_trk100_hmt_L1TE5.0ETA24', 'HLT_mb_sp1800_pusup800_trk110_hmt_L1TE10', 'HLT_mb_sp1800_pusup800_trk110_hmt_L1TE10.0ETA24', 'HLT_mb_sp1800_pusup800_trk110_hmt_L1TE15', 'HLT_mb_sp1800_pusup800_trk110_hmt_L1TE20', 'HLT_mb_sp1800_pusup800_trk110_hmt_L1TE20.0ETA24', 'HLT_mb_sp1800_pusup800_trk110_hmt_L1TE25', 'HLT_mb_sp1800_pusup800_trk110_hmt_L1TE5', 'HLT_mb_sp1800_pusup800_trk110_hmt_L1TE5.0ETA24', 'HLT_mb_sp2100_pusup1000_trk120_hmt_L1TE10', 'HLT_mb_sp2100_pusup1000_trk120_hmt_L1TE15', 'HLT_mb_sp2100_pusup1000_trk120_hmt_L1TE20', 'HLT_mb_sp2100_pusup1000_trk120_hmt_L1TE25', 'HLT_mb_sp2400_pusup1100_trk130_hmt_L1TE10', 'HLT_mb_sp2400_pusup1100_trk130_hmt_L1TE15', 'HLT_mb_sp2400_pusup1100_trk130_hmt_L1TE20', 'HLT_mb_sp2400_pusup1100_trk130_hmt_L1TE25', 'HLT_mb_sp2700_pusup1300_trk140_hmt_L1TE10', 'HLT_mb_sp2700_pusup1300_trk140_hmt_L1TE15', 'HLT_mb_sp2700_pusup1300_trk140_hmt_L1TE20', 'HLT_mb_sp2700_pusup1300_trk140_hmt_L1TE25', 'HLT_mb_sp2_hmtperf_L1TE3', 'HLT_mb_sp2_hmtperf_L1TE3.0ETA24', 'HLT_mb_sp3000_pusup1400_trk150_hmt_L1TE10', 'HLT_mb_sp3000_pusup1400_trk150_hmt_L1TE15', 'HLT_mb_sp3000_pusup1400_trk150_hmt_L1TE20', 'HLT_mb_sp3000_pusup1400_trk150_hmt_L1TE25', 'HLT_mb_sp3000_pusup1400_trk150_hmt_L1TE30', 'HLT_mb_sp3000_pusup1500_trk160_hmt_L1TE10', 'HLT_mb_sp3000_pusup1500_trk160_hmt_L1TE15', 'HLT_mb_sp3000_pusup1500_trk160_hmt_L1TE20', 'HLT_mb_sp3000_pusup1500_trk160_hmt_L1TE25', 'HLT_mb_sp3000_pusup1500_trk160_hmt_L1TE30', 'HLT_mb_sptrk_vetombts2in_L1ALFA_ANY', 'HLT_mb_sptrk_vetombts2in_L1ALFA_ANY_UNPAIRED_ISO', 'HLT_mb_sptrk_vetombts2in_L1TRT_ALFA_ANY', 'HLT_mb_sptrk_vetombts2in_L1TRT_ALFA_ANY_UNPAIRED_ISO', 'HLT_noalg_L1ALFA_CDiff_Phys', 'HLT_noalg_L1ALFA_Diff_Phys', 'HLT_noalg_L1ALFA_Jet_Phys', 'HLT_noalg_L1TE15.0ETA24', 'HLT_noalg_L1TE25', 'HLT_noalg_L1TE25.0ETA24', 'HLT_noalg_L1TE3', 'HLT_noalg_L1TE3.0ETA24', 'HLT_noalg_mb_L1TE15.0ETA24', 'HLT_noalg_mb_L1TE25', 'HLT_noalg_mb_L1TE25.0ETA24', 'HLT_noalg_mb_L1TE3', 'HLT_noalg_mb_L1TE3.0ETA24', 'HLT_noalg_mb_L1TE40.0ETA24', ]
disabled_Rules.update(dict(map(None,HLT_list,len(HLT_list)*[{
    1000 : TriggerRule(PS=-1, comment=''), 
    }])))

# --------------------------------------
# L1Topo seeded

HLT_list=['HLT_mu11_2mu4noL1_nscan03_L1MU11_LFV-MU', 'HLT_mu11_L1MU10_2mu4noL1_nscan03', 'HLT_mu14_tau25_medium1_tracktwo_L1DR-MU10TAU12I_TAU12I-J25', 'HLT_2j40_0eta490_invm250', 'HLT_2j55_bloose_L13J20_4J20.0ETA49_MJJ-400', 'HLT_2j55_bloose_L13J20_4J20.0ETA49_MJJ-700', 'HLT_2j55_bloose_L13J20_4J20.0ETA49_MJJ-800', 'HLT_2j55_bloose_L13J20_4J20.0ETA49_MJJ-900', 'HLT_2j55_bloose_L1J30_2J20_4J20.0ETA49_MJJ-400', 'HLT_2j55_bloose_L1J30_2J20_4J20.0ETA49_MJJ-700', 'HLT_2j55_bloose_L1J30_2J20_4J20.0ETA49_MJJ-800', 'HLT_2j55_bloose_L1J30_2J20_4J20.0ETA49_MJJ-900', 'HLT_2j55_bmedium_L13J20_4J20.0ETA49_MJJ-400', 'HLT_2j55_bmedium_L13J20_4J20.0ETA49_MJJ-700', 'HLT_2j55_bmedium_L13J20_4J20.0ETA49_MJJ-800', 'HLT_2j55_bmedium_L13J20_4J20.0ETA49_MJJ-900', 'HLT_2j55_bmedium_L1J30_2J20_4J20.0ETA49_MJJ-400', 'HLT_2j55_bmedium_L1J30_2J20_4J20.0ETA49_MJJ-700', 'HLT_2j55_bmedium_L1J30_2J20_4J20.0ETA49_MJJ-800', 'HLT_2j55_bmedium_L1J30_2J20_4J20.0ETA49_MJJ-900', 'HLT_2j55_bperf_L13J20_4J20.0ETA49_MJJ-400', 'HLT_2j55_bperf_L13J20_4J20.0ETA49_MJJ-700', 'HLT_2j55_bperf_L13J20_4J20.0ETA49_MJJ-800', 'HLT_2j55_bperf_L13J20_4J20.0ETA49_MJJ-900', 'HLT_2j55_bperf_L1J30_2J20_4J20.0ETA49_MJJ-400', 'HLT_2j55_bperf_L1J30_2J20_4J20.0ETA49_MJJ-700', 'HLT_2j55_bperf_L1J30_2J20_4J20.0ETA49_MJJ-800', 'HLT_2j55_bperf_L1J30_2J20_4J20.0ETA49_MJJ-900', 'HLT_2mu4_bBmumu_L1BPH-2M-2MU4', 'HLT_2mu4_bBmumu_L1BPH-2M-2MU4-B', 'HLT_2mu4_bBmumu_L1BPH-2M-2MU4-BO', 'HLT_2mu4_bBmumu_L1BPH-4M8-2MU4', 'HLT_2mu4_bBmumu_L1BPH-4M8-2MU4-B', 'HLT_2mu4_bBmumu_L1BPH-4M8-2MU4-BO', 'HLT_2mu4_bBmumu_L1BPH-DR-2MU4', 'HLT_2mu4_bBmumu_L1BPH-DR-2MU4-B', 'HLT_2mu4_bBmumu_L1BPH-DR-2MU4-B-BPH-2M-2MU4-B', 'HLT_2mu4_bBmumu_L1BPH-DR-2MU4-B-BPH-4M8-2MU4-B', 'HLT_2mu4_bBmumu_L1BPH-DR-2MU4-BO', 'HLT_2mu4_bBmumu_L1BPH-DR-2MU4-BO-BPH-2M-2MU4-BO', 'HLT_2mu4_bBmumu_L1BPH-DR-2MU4-BO-BPH-4M8-2MU4-BO', 'HLT_2mu4_bBmumu_L1BPH-DR-2MU4-BPH-2M-2MU4', 'HLT_2mu4_bBmumu_L1BPH-DR-2MU4-BPH-4M8-2MU4', 'HLT_2mu4_bBmumux_BcmumuDsloose_L1BPH-2M-2MU4', 'HLT_2mu4_bBmumux_BcmumuDsloose_L1BPH-2M-2MU4-B', 'HLT_2mu4_bBmumux_BcmumuDsloose_L1BPH-2M-2MU4-BO', 'HLT_2mu4_bBmumux_BcmumuDsloose_L1BPH-4M8-2MU4', 'HLT_2mu4_bBmumux_BcmumuDsloose_L1BPH-4M8-2MU4-B', 'HLT_2mu4_bBmumux_BcmumuDsloose_L1BPH-4M8-2MU4-BO', 'HLT_2mu4_bBmumux_BcmumuDsloose_L1BPH-DR-2MU4', 'HLT_2mu4_bBmumux_BcmumuDsloose_L1BPH-DR-2MU4-B', 'HLT_2mu4_bBmumux_BcmumuDsloose_L1BPH-DR-2MU4-B-BPH-2M-2MU4-B', 'HLT_2mu4_bBmumux_BcmumuDsloose_L1BPH-DR-2MU4-B-BPH-4M8-2MU4-B', 'HLT_2mu4_bBmumux_BcmumuDsloose_L1BPH-DR-2MU4-BO', 'HLT_2mu4_bBmumux_BcmumuDsloose_L1BPH-DR-2MU4-BO-BPH-2M-2MU4-BO', 'HLT_2mu4_bBmumux_BcmumuDsloose_L1BPH-DR-2MU4-BO-BPH-4M8-2MU4-BO', 'HLT_2mu4_bBmumux_BcmumuDsloose_L1BPH-DR-2MU4-BPH-2M-2MU4', 'HLT_2mu4_bBmumux_BcmumuDsloose_L1BPH-DR-2MU4-BPH-4M8-2MU4', 'HLT_2mu4_bBmumuxv2_L1BPH-2M-2MU4', 'HLT_2mu4_bBmumuxv2_L1BPH-2M-2MU4-B', 'HLT_2mu4_bBmumuxv2_L1BPH-2M-2MU4-BO', 'HLT_2mu4_bBmumuxv2_L1BPH-4M8-2MU4', 'HLT_2mu4_bBmumuxv2_L1BPH-4M8-2MU4-B', 'HLT_2mu4_bBmumuxv2_L1BPH-4M8-2MU4-BO', 'HLT_2mu4_bBmumuxv2_L1BPH-DR-2MU4', 'HLT_2mu4_bBmumuxv2_L1BPH-DR-2MU4-B', 'HLT_2mu4_bBmumuxv2_L1BPH-DR-2MU4-B-BPH-2M-2MU4-B', 'HLT_2mu4_bBmumuxv2_L1BPH-DR-2MU4-B-BPH-4M8-2MU4-B', 'HLT_2mu4_bBmumuxv2_L1BPH-DR-2MU4-BO', 'HLT_2mu4_bBmumuxv2_L1BPH-DR-2MU4-BO-BPH-2M-2MU4-BO', 'HLT_2mu4_bBmumuxv2_L1BPH-DR-2MU4-BO-BPH-4M8-2MU4-BO', 'HLT_2mu4_bBmumuxv2_L1BPH-DR-2MU4-BPH-2M-2MU4', 'HLT_2mu4_bBmumuxv2_L1BPH-DR-2MU4-BPH-4M8-2MU4', 'HLT_2mu4_bDimu_L1BPH-2M-2MU4', 'HLT_2mu4_bDimu_L1BPH-2M-2MU4-B', 'HLT_2mu4_bDimu_L1BPH-2M-2MU4-BO', 'HLT_2mu4_bDimu_L1BPH-4M8-2MU4', 'HLT_2mu4_bDimu_L1BPH-4M8-2MU4-B', 'HLT_2mu4_bDimu_L1BPH-4M8-2MU4-BO', 'HLT_2mu4_bDimu_L1BPH-DR-2MU4', 'HLT_2mu4_bDimu_L1BPH-DR-2MU4-B', 'HLT_2mu4_bDimu_L1BPH-DR-2MU4-B-BPH-2M-2MU4-B', 'HLT_2mu4_bDimu_L1BPH-DR-2MU4-B-BPH-4M8-2MU4-B', 'HLT_2mu4_bDimu_L1BPH-DR-2MU4-BO', 'HLT_2mu4_bDimu_L1BPH-DR-2MU4-BO-BPH-2M-2MU4-BO', 'HLT_2mu4_bDimu_L1BPH-DR-2MU4-BO-BPH-4M8-2MU4-BO', 'HLT_2mu4_bDimu_L1BPH-DR-2MU4-BPH-2M-2MU4', 'HLT_2mu4_bDimu_L1BPH-DR-2MU4-BPH-4M8-2MU4', 'HLT_2mu4_bDimu_novtx_noos_L1BPH-2M-2MU4', 'HLT_2mu4_bDimu_novtx_noos_L1BPH-2M-2MU4-B', 'HLT_2mu4_bDimu_novtx_noos_L1BPH-2M-2MU4-BO', 'HLT_2mu4_bDimu_novtx_noos_L1BPH-4M8-2MU4', 'HLT_2mu4_bDimu_novtx_noos_L1BPH-4M8-2MU4-B', 'HLT_2mu4_bDimu_novtx_noos_L1BPH-4M8-2MU4-BO', 'HLT_2mu4_bDimu_novtx_noos_L1BPH-DR-2MU4', 'HLT_2mu4_bDimu_novtx_noos_L1BPH-DR-2MU4-B', 'HLT_2mu4_bDimu_novtx_noos_L1BPH-DR-2MU4-B-BPH-2M-2MU4-B', 'HLT_2mu4_bDimu_novtx_noos_L1BPH-DR-2MU4-B-BPH-4M8-2MU4-B', 'HLT_2mu4_bDimu_novtx_noos_L1BPH-DR-2MU4-BO', 'HLT_2mu4_bDimu_novtx_noos_L1BPH-DR-2MU4-BO-BPH-2M-2MU4-BO', 'HLT_2mu4_bDimu_novtx_noos_L1BPH-DR-2MU4-BO-BPH-4M8-2MU4-BO', 'HLT_2mu4_bDimu_novtx_noos_L1BPH-DR-2MU4-BPH-2M-2MU4', 'HLT_2mu4_bDimu_novtx_noos_L1BPH-DR-2MU4-BPH-4M8-2MU4', 'HLT_2mu4_bJpsimumu_L1BPH-2M-2MU4', 'HLT_2mu4_bJpsimumu_L1BPH-2M-2MU4-B', 'HLT_2mu4_bJpsimumu_L1BPH-2M-2MU4-BO', 'HLT_2mu4_bJpsimumu_L1BPH-DR-2MU4', 'HLT_2mu4_bJpsimumu_L1BPH-DR-2MU4-B', 'HLT_2mu4_bJpsimumu_L1BPH-DR-2MU4-B-BPH-2M-2MU4-B', 'HLT_2mu4_bJpsimumu_L1BPH-DR-2MU4-BO', 'HLT_2mu4_bJpsimumu_L1BPH-DR-2MU4-BO-BPH-2M-2MU4-BO', 'HLT_2mu4_bJpsimumu_L1BPH-DR-2MU4-BPH-2M-2MU4', 'HLT_2mu4_bUpsimumu_L1BPH-2M-2MU4', 'HLT_2mu4_bUpsimumu_L1BPH-2M-2MU4-B', 'HLT_2mu4_bUpsimumu_L1BPH-2M-2MU4-BO', 'HLT_2mu6_bBmumu_L1BPH-2M-2MU6', 'HLT_2mu6_bBmumu_L1BPH-4M8-2MU6', 'HLT_2mu6_bBmumu_L1BPH-DR-2MU6', 'HLT_2mu6_bBmumu_L1BPH-DR-2MU6-BPH-2M-2MU6', 'HLT_2mu6_bBmumu_L1BPH-DR-2MU6-BPH-4M8-2MU6', 'HLT_2mu6_bBmumux_BcmumuDsloose_L1BPH-2M-2MU6', 'HLT_2mu6_bBmumux_BcmumuDsloose_L1BPH-4M8-2MU6', 'HLT_2mu6_bBmumux_BcmumuDsloose_L1BPH-DR-2MU6', 'HLT_2mu6_bBmumux_BcmumuDsloose_L1BPH-DR-2MU6-BPH-2M-2MU6', 'HLT_2mu6_bBmumux_BcmumuDsloose_L1BPH-DR-2MU6-BPH-4M8-2MU6', 'HLT_2mu6_bBmumuxv2_L1BPH-2M-2MU6', 'HLT_2mu6_bBmumuxv2_L1BPH-4M8-2MU6', 'HLT_2mu6_bBmumuxv2_L1BPH-DR-2MU6', 'HLT_2mu6_bBmumuxv2_L1BPH-DR-2MU6-BPH-2M-2MU6', 'HLT_2mu6_bBmumuxv2_L1BPH-DR-2MU6-BPH-4M8-2MU6', 'HLT_2mu6_bDimu_L1BPH-2M-2MU6', 'HLT_2mu6_bDimu_L1BPH-4M8-2MU6', 'HLT_2mu6_bDimu_L1BPH-DR-2MU6', 'HLT_2mu6_bDimu_L1BPH-DR-2MU6-BPH-2M-2MU6', 'HLT_2mu6_bDimu_L1BPH-DR-2MU6-BPH-4M8-2MU6', 'HLT_2mu6_bDimu_novtx_noos_L1BPH-2M-2MU6', 'HLT_2mu6_bDimu_novtx_noos_L1BPH-4M8-2MU6', 'HLT_2mu6_bDimu_novtx_noos_L1BPH-DR-2MU6', 'HLT_2mu6_bDimu_novtx_noos_L1BPH-DR-2MU6-BPH-2M-2MU6', 'HLT_2mu6_bDimu_novtx_noos_L1BPH-DR-2MU6-BPH-4M8-2MU6', 'HLT_2mu6_bJpsimumu_L1BPH-2M-2MU6', 'HLT_2mu6_bJpsimumu_L1BPH-DR-2MU6', 'HLT_2mu6_bJpsimumu_L1BPH-DR-2MU6-BPH-2M-2MU6', 'HLT_2mu6_bUpsimumu_L1BPH-2M-2MU6', 'HLT_e13_etcut_L1EM10_W-MT25', 'HLT_e13_etcut_L1EM10_W-MT30', 'HLT_e13_etcut_L1W-NOMATCH', 'HLT_e13_etcut_L1W-NOMATCH_W-05RO-XEEMHT', 'HLT_e13_etcut_trkcut', 'HLT_e13_etcut_trkcut_L1EM10_W-MT25_W-15DPHI-JXE-0_W-15DPHI-EMXE', 'HLT_e13_etcut_trkcut_L1EM10_W-MT25_W-15DPHI-JXE-0_W-15DPHI-EMXE_XS20_xe20_mt25', 'HLT_e13_etcut_trkcut_j20_perf_xe15_6dphi05_mt25_L1EM10_W-MT25_W-15DPHI-JXE-0_W-15DPHI-EMXE_W-90RO2-XEHT-0', 'HLT_e13_etcut_trkcut_j20_perf_xe15_6dphi05_mt25_L1EM10_W-MT25_W-15DPHI-JXE-0_W-15DPHI-EMXE_XS20', 'HLT_e13_etcut_trkcut_xe20', 'HLT_e13_etcut_trkcut_xe20_L1EM10_W-MT25_W-15DPHI-JXE-0_W-15DPHI-EMXE_XS20', 'HLT_e13_etcut_trkcut_xe20_mt25', 'HLT_e13_etcut_trkcut_xs15_L1EM10_W-MT25_W-15DPHI-JXE-0_W-15DPHI-EMXE_XS20', 'HLT_e14_etcut_e5_lhtight_Jpsiee_L1JPSI-1M5-EM12', 'HLT_e14_etcut_e5_lhtight_nod0_Jpsiee_L1JPSI-1M5-EM12', 'HLT_e14_etcut_e5_tight_Jpsiee_L1JPSI-1M5-EM12', 'HLT_e14_lhtight_e4_etcut_Jpsiee_L1JPSI-1M5-EM12', 'HLT_e14_lhtight_nod0_e4_etcut_Jpsiee_L1JPSI-1M5-EM12', 'HLT_e14_tight_e4_etcut_Jpsiee_L1JPSI-1M5-EM12', 'HLT_e17_lhmedium_iloose_tau25_medium1_tracktwo_L1DR-EM15TAU12I-J25', 'HLT_e17_lhmedium_nod0_iloose_tau25_medium1_tracktwo_L1DR-EM15TAU12I-J25', 'HLT_e17_lhmedium_nod0_tau25_medium1_tracktwo_L1DR-EM15TAU12I-J25', 'HLT_e17_lhmedium_nod0_tau25_medium1_tracktwo_L1EM15-TAU12I', 'HLT_e17_lhmedium_nod0_tau25_medium1_tracktwo_L1EM15TAU12I-J25', 'HLT_e17_lhmedium_nod0_tau25_medium1_tracktwo_xe50_L1XE35_EM15-TAU12I', 'HLT_e17_lhmedium_nod0_tau25_medium1_tracktwo_xe50_L1XE40_EM15-TAU12I', 'HLT_e17_lhmedium_nod0_tau80_medium1_tracktwo_L1EM15-TAU40', 'HLT_e17_lhmedium_tau25_medium1_tracktwo_L1DR-EM15TAU12I-J25', 'HLT_e17_lhmedium_tau25_medium1_tracktwo_L1EM15-TAU12I', 'HLT_e17_lhmedium_tau25_medium1_tracktwo_L1EM15TAU12I-J25', 'HLT_e17_lhmedium_tau25_medium1_tracktwo_xe50_L1XE35_EM15-TAU12I', 'HLT_e17_lhmedium_tau25_medium1_tracktwo_xe50_L1XE40_EM15-TAU12I', 'HLT_e17_lhmedium_tau80_medium1_tracktwo_L1EM15-TAU40', 'HLT_e17_medium_iloose_tau25_medium1_tracktwo_L1DR-EM15TAU12I-J25', 'HLT_e17_medium_tau25_medium1_tracktwo_L1DR-EM15TAU12I-J25', 'HLT_e17_medium_tau25_medium1_tracktwo_L1EM15-TAU12I', 'HLT_e17_medium_tau25_medium1_tracktwo_L1EM15TAU12I-J25', 'HLT_e17_medium_tau25_medium1_tracktwo_xe50_L1XE35_EM15-TAU12I', 'HLT_e17_medium_tau25_medium1_tracktwo_xe50_L1XE40_EM15-TAU12I', 'HLT_e17_medium_tau80_medium1_tracktwo_L1EM15-TAU40', 'HLT_e18_etcut_L1EM15_W-MT35', 'HLT_e18_etcut_trkcut', 'HLT_e18_etcut_trkcut_L1EM15_W-MT35_W-05DPHI-JXE-0_W-05DPHI-EMXE', 'HLT_e18_etcut_trkcut_L1EM15_W-MT35_W-05DPHI-JXE-0_W-05DPHI-EMXE_XS30_xe35_mt35', 'HLT_e18_etcut_trkcut_j20_perf_xe20_6dphi15_mt35_L1EM15_W-MT35_W-05DPHI-JXE-0_W-05DPHI-EMXE_W-250RO2-XEHT-0', 'HLT_e18_etcut_trkcut_j20_perf_xe20_6dphi15_mt35_L1EM15_W-MT35_W-05DPHI-JXE-0_W-05DPHI-EMXE_XS30', 'HLT_e18_etcut_trkcut_xe35', 'HLT_e18_etcut_trkcut_xe35_L1EM15_W-MT35_W-05DPHI-JXE-0_W-05DPHI-EMXE_XS30', 'HLT_e18_etcut_trkcut_xe35_mt35', 'HLT_e5_etcut_L1W-05DPHI-EMXE-1', 'HLT_e5_etcut_L1W-05DPHI-JXE-0', 'HLT_e5_etcut_L1W-05RO-XEHT-0', 'HLT_e5_etcut_L1W-10DPHI-EMXE-0', 'HLT_e5_etcut_L1W-10DPHI-JXE-0', 'HLT_e5_etcut_L1W-15DPHI-EMXE-0', 'HLT_e5_etcut_L1W-15DPHI-JXE-0', 'HLT_e5_etcut_L1W-250RO2-XEHT-0', 'HLT_e5_etcut_L1W-90RO2-XEHT-0', 'HLT_e5_etcut_L1W-HT20-JJ15.ETA49', 'HLT_e5_lhtight_e4_etcut_Jpsiee_L1JPSI-1M5', 'HLT_e5_lhtight_e4_etcut_L1JPSI-1M5', 'HLT_e5_lhtight_nod0_e4_etcut_Jpsiee_L1JPSI-1M5', 'HLT_e5_lhtight_nod0_e4_etcut_L1JPSI-1M5', 'HLT_e5_tight_e4_etcut_Jpsiee_L1JPSI-1M5', 'HLT_e5_tight_e4_etcut_L1JPSI-1M5', 'HLT_e9_etcut_e5_lhtight_Jpsiee_L1JPSI-1M5-EM7', 'HLT_e9_etcut_e5_lhtight_nod0_Jpsiee_L1JPSI-1M5-EM7', 'HLT_e9_etcut_e5_tight_Jpsiee_L1JPSI-1M5-EM7', 'HLT_e9_lhtight_e4_etcut_Jpsiee_L1JPSI-1M5-EM7', 'HLT_e9_lhtight_nod0_e4_etcut_Jpsiee_L1JPSI-1M5-EM7', 'HLT_e9_tight_e4_etcut_Jpsiee_L1JPSI-1M5-EM7', 'HLT_g10_etcut_mu10_L1LFV-EM8I', 'HLT_g10_etcut_mu10_iloose_taumass_L1LFV-EM8I', 'HLT_g10_etcut_mu10_taumass', 'HLT_g10_loose_mu10_iloose_taumass_L1LFV-EM8I', 'HLT_g15_loose_2j40_0eta490_3j25_0eta490', 'HLT_g20_etcut_mu4_L1LFV-EM15I', 'HLT_g20_etcut_mu4_iloose_taumass_L1LFV-EM15I', 'HLT_g20_etcut_mu4_taumass', 'HLT_g20_loose_2j40_0eta490_3j25_0eta490', 'HLT_g20_loose_2j40_0eta490_3j25_0eta490_L1MJJ-700', 'HLT_g20_loose_2j40_0eta490_3j25_0eta490_L1MJJ-900', 'HLT_g20_loose_mu4_iloose_taumass_L1LFV-EM15I', 'HLT_ht1000', 'HLT_ht1000_L1HT190-J15.ETA21', 'HLT_ht400', 'HLT_ht400_L1HT150-J20.ETA31', 'HLT_ht550', 'HLT_ht550_L1HT150-J20.ETA31', 'HLT_ht700', 'HLT_ht700_L1HT190-J15.ETA21', 'HLT_ht850', 'HLT_ht850_L1HT190-J15.ETA21', 'HLT_j100_xe80_L1J40_DPHI-CJ20XE50', 'HLT_j100_xe80_L1J40_DPHI-J20XE50', 'HLT_j100_xe80_L1J40_DPHI-J20s2XE50', 'HLT_j100_xe80_L1J40_DPHI-Js2XE50', 'HLT_j360_a10_lcw_nojcalib', 'HLT_j360_a10_lcw_nojcalib_L1HT150-J20.ETA31', 'HLT_j360_a10_lcw_sub', 'HLT_j360_a10_lcw_sub_L1HT150-J20.ETA31', 'HLT_j360_a10_nojcalib', 'HLT_j360_a10_nojcalib_L1HT150-J20.ETA31', 'HLT_j360_a10_sub', 'HLT_j360_a10_sub_L1HT150-J20.ETA31', 'HLT_j360_a10r', 'HLT_j460_a10_lcw_nojcalib', 'HLT_j460_a10_lcw_nojcalib_L1HT190-J15.ETA21', 'HLT_j460_a10_lcw_sub', 'HLT_j460_a10_lcw_sub_L1HT190-J15.ETA21', 'HLT_j460_a10_nojcalib', 'HLT_j460_a10_nojcalib_L1HT190-J15.ETA21', 'HLT_j460_a10_sub', 'HLT_j460_a10_sub_L1HT190-J15.ETA21', 'HLT_j460_a10r', 'HLT_j80_xe80_1dphi10_L1J40_DPHI-CJ20XE50', 'HLT_j80_xe80_1dphi10_L1J40_DPHI-J20XE50', 'HLT_j80_xe80_1dphi10_L1J40_DPHI-J20s2XE50', 'HLT_j80_xe80_1dphi10_L1J40_DPHI-Js2XE50', 'HLT_mu14_tau25_medium1_tracktwo_L1DR-MU10TAU12I', 'HLT_mu14_tau25_medium1_tracktwo_L1DR-MU10TAU12I_TAU12-J25', 'HLT_mu14_tau25_medium1_tracktwo_L1MU10_TAU12I-J25', 'HLT_mu4_iloose_mu4_11invm60_noos_L1DY-BOX-2MU4', 'HLT_mu4_iloose_mu4_11invm60_noos_novtx_L1DY-BOX-2MU4', 'HLT_mu4_iloose_mu4_7invm9_noos_L1DY-BOX-2MU4', 'HLT_mu4_iloose_mu4_7invm9_noos_novtx_L1DY-BOX-2MU4', 'HLT_mu4_j60_dr05_2j35_L13J15_BTAG-MU4J30', 'HLT_mu4_j70_dr05_L1BTAG-MU4J30', 'HLT_mu6_2j40_0eta490_invm1000', 'HLT_mu6_2j40_0eta490_invm400', 'HLT_mu6_2j40_0eta490_invm600', 'HLT_mu6_2j40_0eta490_invm800', 'HLT_mu6_iloose_mu6_11invm24_noos_L1DY-BOX-2MU6', 'HLT_mu6_iloose_mu6_11invm24_noos_novtx_L1DY-BOX-2MU6', 'HLT_mu6_iloose_mu6_24invm60_noos_L1DY-BOX-2MU6', 'HLT_mu6_iloose_mu6_24invm60_noos_novtx_L1DY-BOX-2MU6', 'HLT_mu6_j50_dr05_2j35_L13J15_BTAG-MU6J25', 'HLT_mu6_j60_dr05_L1BTAG-MU6J25', 'HLT_mu6_mu4_bBmumu_L1BPH-2M-MU6MU4', 'HLT_mu6_mu4_bBmumu_L1BPH-2M-MU6MU4-BO', 'HLT_mu6_mu4_bBmumu_L1BPH-4M8-MU6MU4', 'HLT_mu6_mu4_bBmumu_L1BPH-4M8-MU6MU4-BO', 'HLT_mu6_mu4_bBmumu_L1BPH-DR-MU6MU4', 'HLT_mu6_mu4_bBmumu_L1BPH-DR-MU6MU4-BO', 'HLT_mu6_mu4_bBmumux_BcmumuDsloose_L1BPH-2M-MU6MU4-B', 'HLT_mu6_mu4_bBmumux_BcmumuDsloose_L1BPH-2M-MU6MU4-BO', 'HLT_mu6_mu4_bBmumux_BcmumuDsloose_L1BPH-4M8-MU6MU4-B', 'HLT_mu6_mu4_bBmumux_BcmumuDsloose_L1BPH-4M8-MU6MU4-BO', 'HLT_mu6_mu4_bBmumux_BcmumuDsloose_L1BPH-DR-MU6MU4-B', 'HLT_mu6_mu4_bBmumux_BcmumuDsloose_L1BPH-DR-MU6MU4-BO', 'HLT_mu6_mu4_bBmumuxv2_L1BPH-2M-MU6MU4', 'HLT_mu6_mu4_bBmumuxv2_L1BPH-2M-MU6MU4-BO', 'HLT_mu6_mu4_bBmumuxv2_L1BPH-4M8-MU6MU4', 'HLT_mu6_mu4_bBmumuxv2_L1BPH-4M8-MU6MU4-BO', 'HLT_mu6_mu4_bBmumuxv2_L1BPH-DR-MU6MU4', 'HLT_mu6_mu4_bBmumuxv2_L1BPH-DR-MU6MU4-BO', 'HLT_mu6_mu4_bDimu_L1BPH-2M-MU6MU4', 'HLT_mu6_mu4_bDimu_L1BPH-2M-MU6MU4-BO', 'HLT_mu6_mu4_bDimu_L1BPH-4M8-MU6MU4', 'HLT_mu6_mu4_bDimu_L1BPH-4M8-MU6MU4-BO', 'HLT_mu6_mu4_bDimu_L1BPH-BPH-DR-MU6MU4', 'HLT_mu6_mu4_bDimu_L1BPH-BPH-DR-MU6MU4-BO', 'HLT_mu6_mu4_bDimu_novtx_noos_L1BPH-2M-MU6MU4-B', 'HLT_mu6_mu4_bDimu_novtx_noos_L1BPH-2M-MU6MU4-BO', 'HLT_mu6_mu4_bDimu_novtx_noos_L1BPH-4M8-MU6MU4-B', 'HLT_mu6_mu4_bDimu_novtx_noos_L1BPH-4M8-MU6MU4-BO', 'HLT_mu6_mu4_bDimu_novtx_noos_L1BPH-DR-MU6MU4-B', 'HLT_mu6_mu4_bDimu_novtx_noos_L1BPH-DR-MU6MU4-BO', 'HLT_mu6_mu4_bJpsimumu_L1BPH-2M-MU6MU4', 'HLT_mu6_mu4_bJpsimumu_L1BPH-2M-MU6MU4-BO', 'HLT_mu6_mu4_bJpsimumu_L1BPH-DR-MU6MU4', 'HLT_mu6_mu4_bJpsimumu_L1BPH-DR-MU6MU4-BO', 'HLT_mu6_mu4_bUpsimumu_L1BPH-2M-MU6MU4', 'HLT_mu6_mu4_bUpsimumu_L1BPH-2M-MU6MU4-BO', 'HLT_mu6_mu4_bUpsimumu_L1BPH-DR-MU6MU4', 'HLT_mu6_mu4_bUpsimumu_L1BPH-DR-MU6MU4-BO', 'HLT_tau35_medium1_tracktwo_tau25_medium1_tracktwo_L1BOX-TAU20ITAU12I', 'HLT_tau35_medium1_tracktwo_tau25_medium1_tracktwo_L1DR-TAU20ITAU12I', 'HLT_tau35_medium1_tracktwo_tau25_medium1_tracktwo_L1DR-TAU20ITAU12I-J25', 'HLT_tau35_medium1_tracktwo_tau25_medium1_tracktwo_L1TAU20ITAU12I-J25', 'HLT_mu14_iloose_tau25_medium1_tracktwo_L1DR-MU10TAU12I', 'HLT_mu14_iloose_tau25_medium1_tracktwo_L1DR-MU10TAU12I_TAU12I-J25', 'HLT_mu14_iloose_tau25_medium1_tracktwo_L1MU10_TAU12I-J25', 'HLT_mu4_j35_bperf_split_dr05_dz02_L1BTAG-MU4J15', 'HLT_mu4_j55_bperf_split_dr05_dz02_L1BTAG-MU4J15', 'HLT_mu6_j110_bperf_split_dr05_dz02_L1BTAG-MU6J20', 'HLT_mu6_j150_bperf_split_dr05_dz02_L1BTAG-MU6J20', 'HLT_mu6_j175_bperf_split_dr05_dz02_L1BTAG-MU6J20', 'HLT_mu6_j260_bperf_split_dr05_dz02_L1BTAG-MU6J20', 'HLT_mu6_j320_bperf_split_dr05_dz02_L1BTAG-MU6J20', 'HLT_mu6_j400_bperf_split_dr05_dz02_L1BTAG-MU6J20', 'HLT_mu6_j85_bperf_split_dr05_dz02_L1BTAG-MU6J20', 'HLT_e17_lhmedium_iloose_tau25_medium1_tracktwo_L1EM15TAU12I-J25', 'HLT_e17_medium_iloose_tau25_medium1_tracktwo_L1EM15TAU12I-J25', ]
disabled_Rules.update(dict(map(None,HLT_list,len(HLT_list)*[{
    1000 : TriggerRule(PS=-1, comment=''), 
    }])))


#################################################################################################
#################################################################################################


rules = {}

RulesList=[muon_Rules, bphys_Rules, met_Rules, jet_Rules, bjet_Rules, tau_Rules, egamma_Rules, combined_Rules, vbf_Rules, dedicated_Rules, disabled_Rules]

for Rules in RulesList:
    for newrule in Rules.keys():
        if rules.has_key(newrule):
            print 'FATAL     Physics_pp_v5_rules     Duplicated rule inside physics rule. Cannot be added:',newrule
    rules.update(Rules)

# ----------------------------------------

RulesList=[datascouting_Rules]
if forRateEstim:
    RulesList=[]

for Rules in RulesList:
    for newrule in Rules.keys():
        if rules.has_key(newrule):
            print 'FATAL     Physics_pp_v5_rules     Duplicated rule inside calibration rule. Cannot be added:',newrule
    rules.update(Rules)

# ----------------------------------------

RulesList=[zerobias_Rules, beamspot_Rules, tauoverlay_Rules, topo_Rules,l1calo_streamer_Rules]
if forRateEstim:
    RulesList=[]

for Rules in RulesList:
    for newrule in Rules.keys():
        if rules.has_key(newrule):
            print 'FATAL     Physics_pp_v5_rules     Duplicated rule inside calibration rule. Cannot be added:',newrule
    rules.update(Rules)

# ----------------------------------------

RulesList=[Monitoring_pp_v5_rules.physics_rules]
if forRateEstim:
    RulesList=[]

for Rules in RulesList:
    for newrule in Rules.keys():
        if rules.has_key(newrule):
            print 'FATAL     Physics_pp_v5_rules     Duplicated rule in monitoring rule. Cannot be added from monitoring rule for:',newrule
    rules.update(Rules)

#################################################################################################
#################################################################################################

# rules to override already-existing rules on purpose

override_rules = {
    # defined in monitoring rule
    'L1_MU4_UNPAIRED_ISO'                      : {  1000 : TriggerRule(PS=50,    comment='') },
#    'HLT_l1calocalib' :  {  1000 : TriggerRule(rate=1000,    comment=' overwritten for 8b4e') },
#    'HLT_noalg_zb_L1ZB'        : { 1 : TriggerRule(PS=40, comment='10 Hz target overwritten for 8b4e') },
    
}


if forRateEstim:
    L1_list=['L1_RD0_FILLED', 'L1_RD0_ABORTGAPNOTCALIB', 'L1_RD0_UNPAIRED_ISO', 'HLT_j55_0eta240_xe50_L1J30_EMPTY']
    override_rules.update(dict(map(None,L1_list,len(L1_list)*[{
        1000 : TriggerRule(PS=-1, scaling='bunches', comment='L1'),
        }])))



rules.update(override_rules)


#################################################################################################
#################################################################################################
