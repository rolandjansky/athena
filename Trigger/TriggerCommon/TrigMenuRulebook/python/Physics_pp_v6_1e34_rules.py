# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# This special rules apply '1e34 rules' to all lumis, to evaluate L1 limits from IBL
# Don't use for usual rate estimation 

from RuleClasses import TriggerRule

tag = 'Physics_pp_v6_1e34'

forRateEstim=True #False
noSupportingTriggers=False #True #
cpuOptimized=False
ElectronRerun=True
MuonRerun=True
TauRerun=True


#
# V6 "cpuOptimised" :
# - noL2 disabled
# - topo bphys chains enabled
#
#################################################################################################
# to do:
# == remove L1 rules if rules at both L1 and HLT are defined: e.g. for new topo items
# == check missing xe100_L1XE60 variations at 2e34
# == read carefully all comments added and address in next week
# == add ES tags in menu to be consistent with support rules: HLT_j15,55,110_boffperf_split (more later when ES chain list is revisited)
#
# == update on rates with RB-00-03-88core on 07/02. Below observations and proposals
# -- 2e34 (L1:99kHz, HLT:1634Hz): needs to reduce too high rates in EM and Bphys (<-high unique rates)
# https://atlas-trig-cost.cern.ch/?dir=nakahama&type=&tag=RatePred-ATR13260-wL2e34PSRB-00-03-88core&run=280500&range=SMK_273_L1_86_HLT_198
# EM20VH_2EM7 -> EM22VH_2EM7 for asym 2e only. Requested e26_lhmedium_e9_lhmedium in ATR-12825
# EM22VHI -> EM24VHI (Don't change at the moment)
# mu6_mu4_bJpsimumu_L1BPH-2M8-MU6MU4#  / bBmumu -> Move to end of fill or 2mu6 ver
# g45_tight_L1EM22VHI_xe45noL1 -> Move to end of fill (<1.5e34). Needs tighter version etc
# Move from e26 to e28 version? Needs reoptimised ivarloose e/mu at ~200 Hz rates.
# -- 1.5e34 (L1:78kHz, HLT:1319Hz): roughly OK but still high rates in EM and Bphys
# https://atlas-trig-cost.cern.ch/?dir=nakahama&type=&tag=RatePred-ATR13260-wL1p5e34PSRB-00-03-88core&run=280500&range=SMK_273_L1_86_HLT_198
# (same as 2e34) EM20VH_2EM7 -> EM22VH_2EM7 for asym 2e only (Requested)
# (same as 2e34) HLT_mu6_mu4_bJpsimumu_L1BPH-2M8-MU6MU4# and bBmumu -> Move to end of fill or tightening the cuts
# (same as 2e34) Needs reoptimised ivarloose e/mu
# -- 1e34 (L1:86kHz, HLT:1380Hz): roughly OK. Needs to reduce high rates in EM and Bphys
# https://atlas-trig-cost.cern.ch/?dir=nakahama&type=&tag=RatePred-ATR13260-wL1e34PSRB-00-03-88core&run=280500&range=SMK_273_L1_86_HLT_198
# (same as 2e34) EM20VH_2EM7 -> EM22VH_2EM7 for asym 2e only (Requested) 
# HLT_2mu4_bJpsimumu/bBmumu -> Removed L1_BPH-1M19-2MU4_BPH-0DR34-2MU4 seeded chains
# xe70 with 106Hz (Don't change at the moment)
# -- 5e33 (L1:56kHz, HLT: 973Hz): Roughly OK but still high rates in EM and Bphys
# https://atlas-trig-cost.cern.ch/?dir=nakahama&type=&tag=RatePred-ATR13260-w5e33PSRB-00-03-88core&run=280500&range=SMK_273_L1_86_HLT_198
# (same as 2e34) EM20VH_2EM7 -> EM22VH_2EM7 for asym 2e only (Requested)
# L1 HLT_2mu4_bJpsimumu # and bBmumu -> Move to end of fill or mu6_2mu4 version (-5kHz)
# Then, IBL limit 60kHz will be ~roughly OK
# -- 3e33 (L1:49kHz, HLT: 682Hz): roughly OK. Needs to reduce high rate in Bphys
# https://atlas-trig-cost.cern.ch/?dir=nakahama&type=&tag=RatePred-ATR13260-w3e33PSRB-00-03-88core&run=280500
# L1_2MU4 HLT_2mu4_bJpsi/bBmumu -> Move to end of fill or mu6_2mu4 version (-10kHz)
# Then, IBL limit 35kHz will be ~roughly OK
#
# ==> Excel sheets and summary can be found here: /afs/cern.ch/work/n/nakahama/public/cost/core/v6-Rates*PSRB-00-03-88core.xls and https://cernbox.cern.ch/index.php/s/H4S0dDWe4NHv2n0
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

lumi_nonl1topo_disable_vbf_g_5j=1001 #AAA5001
lumi_nonl1topo_disable_vbf_mu=10001
lumi_nonl1topo_disable_vbf_bjet=15001

lumi_nonl1topo_disable_nscan=15001

########################################
# L1 EM/TAU Iso
lumi_l1iso_enable_em=1001 #AAA5001  # EM20VH -> 20VHI
lumi_l1iso_enable_tau=1001 #AAA3001
lumi_l1iso_enable_tau_xe=10001

########################################
# L1 Jet requirement add (already pushed from 5e33 to 7e33)
lumi_l1jet_enable_tau_tau=1001 #AAA7001

########################################
# Bphys + light objects L1 evolution

# L1 Topo

lumi_disable_l1_2mu6=10001 # significant impact if moved to 2mu10
lumi_disable_l1_mu6_2mu4=1001 #AAA7001 # SX 090216
lumi_disable_l1_2mu4=1001 #AAA4001

lumi_disable_l1_2mu4_topo=10001
lumi_disable_l1_mu6_2mu4_topo=20001


lumi_disable_l1_3mu4=1001 #AAA5001

########################################
# muon evolution
lumi_disable_l1_mu15=10001
lumi_disable_hlt_mu20=1001 #AAA5001

lumi_disable_hlt_2mu10=1001 #AAA5001

lumi_disable_l1_mu6_3mu4=10001
lumi_disable_l1_2mu6_3mu4=1001 #AAA7001 # SX 090216



	
########################################
# e/gamma evolution
lumi_disable_l1_em20vhi=1001 #AAA10001
lumi_disable_l1_em18vh=1001 #AAA3001
lumi_disable_l1_2em13vh=10001
lumi_disable_l1_2em10vh=1001 #AAA5001
lumi_disable_l1_2em15=1001 #AAA3001

# hlt
lumi_disable_hlt_2g_loose=1001 #AAA5001
lumi_disable_hlt_eg120=1001 #AAA6501

########################################
# tau L1 evolution
lumi_disable_l1_tau30=1001 #AAA3001
lumi_disable_l1_tau40=1001 #AAA7501

########################################
# jet evolution
# Keep J75 and FJ75 until 7.5E33 for data scouting
lumi_disable_l1_j75=1001 #AAA7501

lumi_disable_l1_3j40=1001 #AAA6501
lumi_disable_l1_4j15=1001 #AAA6501

# hlt
lumi_disable_hlt_j360=1001 #AAA6501
lumi_disable_hlt_j360_a10r=1001 #AAA6501

########################################
# bjet evolution
lumi_disable_l1_3j25_0eta23=10001 #SX 110216, back to what is what before Xmas
#lumi_disable_l1_3j25_0eta23=7001 #SX 090216
lumi_disable_l1_4j15_0eta25=1001 #AAA6501

lumi_disable_mu_j_phys=10001

# hlt
lumi_disable_1b_225=1001 #AAA6501
lumi_disable_2b_150=1001 #AAA6501
lumi_disable_2b1j_70=1001 #AAA6501

########################################
# exotics
lumi_disable_nscan05=1001 #AAA8001
lumi_disable_taumass=1001 #AAA8001  # EM8I_MU10, EM15I_MU4

########################################
# end-of-fill
lumi_disable_dy=1001 #AAA3001  # needs to be earlier than L1_2MU4


#################################################################################################
#
#
# Muon  
#
#################################################################################################


########################################
# 2e34 

muon_Rules_2e34={}

L1_list=['L1_MU20', 'L1_2MU10', 'L1_3MU6']
muon_Rules_2e34.update(dict(map(None,L1_list,len(L1_list)*[{
    1000 : TriggerRule(PS=1, comment='Primary L1'),
    }])))

HLT_list=[ 'HLT_mu26_imedium', 'HLT_mu26_ivarmedium','HLT_mu50', 'HLT_mu60_0eta105_msonly',
           'HLT_mu24_mu8noL1', 'HLT_mu24_2mu4noL1',]
muon_Rules_2e34.update(dict(map(None,HLT_list,len(HLT_list)*[{
                    1000 : TriggerRule(PS=1, comment='Primary'), 
                    }])))

HLT_list=[ 'HLT_2mu14' ]
muon_Rules_2e34.update(dict(map(None,HLT_list,len(HLT_list)*[{
                    1000 : TriggerRule(PS=1, comment='Primary', ESValue=0), 
                    5001 : TriggerRule(PS=1, comment='Primary Express', ESRate=0.3), 
                    }])))

HLT_list=[ 'HLT_2mu14_nomucomb' ]
muon_Rules_2e34.update(dict(map(None,HLT_list,len(HLT_list)*[
                { 1000 : TriggerRule(PS=1, comment='Primary'),
                }])))

HLT_list=[ 'HLT_3mu6' ]
muon_Rules_2e34.update(dict(map(None,HLT_list,len(HLT_list)*[{
                    1000 : TriggerRule(PS=1, comment='Primary Express', ESRate=0.1), 
                    }])))

HLT_list=['HLT_3mu6_msonly' ]    
muon_Rules_2e34.update(dict(map(None,HLT_list,len(HLT_list)*[{
                    1000 : TriggerRule(PS=1, comment='Primary, dedicated trigger LLP, ATR-10933'), 
                    }])))

########################################
# 1.5e34 

muon_Rules_1p5e34={}


HLT_list=['HLT_mu22_mu8noL1', 'HLT_mu22_2mu4noL1', 'HLT_mu20_2mu4noL1']
muon_Rules_1p5e34.update(dict(map(None,HLT_list,len(HLT_list)*[{
                    1000 : TriggerRule(PS=1, comment='Primary'), 
                    15001 : TriggerRule(PS=-1, comment='Disabled'),
                    }])))


########################################
# 1.0e34 

muon_Rules_1e34={}

L1_list=['L1_MU15']
muon_Rules_1e34.update(dict(map(None,L1_list,len(L1_list)*[{
                    1000 : TriggerRule(PS=1, comment='Primary L1'),
                    lumi_disable_l1_mu15 : TriggerRule(rate=1000, maxRate=1000, comment='Disabled'),
                    }])))

HLT_list=['HLT_mu18_mu8noL1', 'HLT_mu18_2mu4noL1', ]
muon_Rules_1e34.update(dict(map(None,HLT_list,len(HLT_list)*[{
                    1000 : TriggerRule(PS=1, comment='Primary'), 
                    lumi_disable_l1_mu15 : TriggerRule(PS=-1, comment='Disabled'),
                     }])))

HLT_list=['HLT_mu24_imedium']
muon_Rules_1e34.update(dict(map(None,HLT_list,len(HLT_list)*[{
                    1000 : TriggerRule(PS=1, comment='Primary Express', ESRate=0.8), 
                    10001 : TriggerRule(rate=0.8, comment='Support Express', ESValue=1),
                    }])))

HLT_list=['HLT_mu20_mu8noL1', ]
muon_Rules_1e34.update(dict(map(None,HLT_list,len(HLT_list)*[{
                    1000 : TriggerRule(PS=1, comment='Primary Backup'), 
                    10001 : TriggerRule(PS=-1, comment='Disabled'),
                     }])))

HLT_list=['HLT_mu24_ivarmedium', 'HLT_mu40',]
muon_Rules_1e34.update(dict(map(None,HLT_list,len(HLT_list)*[{
                    1000 : TriggerRule(PS=1, comment='Primary'), 
                    10001: TriggerRule(PS=-1, comment='Disabled'),
                    }])))

# who uses this ? ==> tau->3mu (ATR-11754)
L1_list=['L1_MU6_3MU4']
muon_Rules_1e34.update(dict(map(None,L1_list,len(L1_list)*[{
                        1000 : TriggerRule(PS=1, comment='Primary L1'),
                        lumi_disable_l1_mu6_3mu4 : TriggerRule(PS=-1, comment='Disabled'),
                        }])))

########################################
# 0.5e34 

muon_Rules_0p5e34={}

HLT_list=['HLT_mu20_iloose_L1MU15','HLT_mu20_ivarloose_L1MU15']
muon_Rules_0p5e34.update(dict(map(None,HLT_list,len(HLT_list)*[{
                     1000 : TriggerRule(PS=1, comment='Primary'), 
                     lumi_disable_hlt_mu20 : TriggerRule(PS=-1, comment='Disabled'),
                     }])))

# HLT_mu24_iloose_L1MU15 (154Hz), HLT_mu24_ivarloose_L1MU15 (174Hz) at 1e34 --> moved to 5e33
HLT_list=['HLT_mu24_iloose_L1MU15', 'HLT_mu24_ivarloose_L1MU15']
muon_Rules_0p5e34.update(dict(map(None,HLT_list,len(HLT_list)*[{
                    1000 : TriggerRule(PS=1, comment='Primary, ATR-13367'), 
                    1001 : TriggerRule(PS=-1, comment='Disabled'),
                    ##5001 : TriggerRule(PS=-1, comment='Disabled'),
                    }])))

HLT_list=['HLT_2mu10']
muon_Rules_0p5e34.update(dict(map(None,HLT_list,len(HLT_list)*[{
                      1000 : TriggerRule(PS=1, comment='Primary Express', ESRate=0.3), 
                      lumi_disable_hlt_2mu10 : TriggerRule(PS=-1, comment='Disabled', ESValue=0),
                      }])))

HLT_list=['HLT_3mu4']
muon_Rules_0p5e34.update(dict(map(None,HLT_list,len(HLT_list)*[{
                    1000 : TriggerRule(PS=1, comment='Primary'), 
                    lumi_disable_l1_3mu4 : TriggerRule(PS=-1, comment='Disabled'),
                    }])))

########################################
# supporting

muon_Rules_supporting={}

muon_Rules_supporting.update({
    # ----------------------------------------
    # J/Psi T&P trigger
    # rate so small can stay always unPS    
    'HLT_mu20_2mu0noL1_JpsimumuFS'             : {  1000 : TriggerRule(PS=1, comment='Support, Jpsi tagprobe') },
    'HLT_mu20_2mu4_JpsimumuL2'                 : {  1000 : TriggerRule(PS=1, comment='Support, Jpsi tagprobe') },

    # 1.0e34
    # this could always stay unPS actualy. is it expected rate is so low (< 1Hz at1e34)?
    'HLT_mu18_2mu0noL1_JpsimumuFS'             : {  1000 : TriggerRule(PS=1, comment='Support, Jpsi tagprobe'),
                                                    lumi_disable_l1_mu15 : TriggerRule(PS=-1, comment='Disabled') },
    'HLT_mu18_2mu4_JpsimumuL2'                 : {  1000 : TriggerRule(PS=1, comment='Support, Jpsi tagprobe'),
                                                    lumi_disable_l1_mu15 : TriggerRule(PS=-1, comment='Disabled') },

    # ----------------------------------------
    # Z T&P idperf
    # rate is 10 Hz at 1e34
    'HLT_mu13_mu13_idperf_Zmumu'               : {  1000 : TriggerRule(PS=1, comment='Support Express, Jpsi tagprobe', ESRate=0.2) },
})

# ========================================
# many rerun . heavy CPU? 
if MuonRerun:

    HLT_list=[
        'HLT_mu11','HLT_mu20','HLT_mu22','HLT_mu26',
        'HLT_mu14_iloose',
        'HLT_mu4_msonly', 'HLT_mu6_msonly','HLT_mu10_msonly',
        'HLT_mu0_perf',
        ##NEW20.11.0.2
        #'HLT_mu20_nomucomb', 'HLT_mu11_nomucomb','HLT_mu6_nomucomb','HLT_mu4_nomucomb'
        ]
    muon_Rules_supporting.update(dict(map(None,HLT_list,len(HLT_list)*[{
                        1000 : TriggerRule(PS=0, comment='Support',rerun=1), 
                        }])))

# ========================================

muon_Rules_supporting.update({

    # ----------------------------------------
    # Level-1
    # low pT used for supporting (also for bJpsi_Trkloose)
    'L1_MU4'                                   : {  1000 : TriggerRule(rate=3000, maxRate=3000, comment='Support L1, needed also by bjets ') },
    'L1_MU6'                                   : {  1000 : TriggerRule(rate=1000, maxRate=1000, comment='Support L1') },

})
    
if MuonRerun :
    muon_Rules_supporting.update({
            # ----------------------------------------
            # un-isolated single muon for Background estimation
            
            # heavy for CPU maybe ?
            
            # PS=10 (2015.06.25 MCG), increase to PS=30 else at 1e34 is 35 Hz from each !! is this ok?
            # Q: I guess PS instead of rate =xxx in order to have a constant fraction wrt primary trigger ? 
            # this gives 30-40 Hz from muon

            # For mu20/24_iloose_L1MU15
            'HLT_mu20_L1MU15'                          : {  1000 : TriggerRule(PS=30, comment='Support', rerun=1),
                                                          lumi_disable_hlt_mu20 : TriggerRule(PS=0, comment='Support', rerun=1) },
            'HLT_mu24_L1MU15'                          : {  1000 : TriggerRule(PS=30, comment='Support', rerun=1),
                                                          lumi_disable_l1_mu15 : TriggerRule(PS=0, comment='Support', rerun=1) },
            # For mu24_imedium and mu26_imedium
            'HLT_mu24'                                 : {  1000 : TriggerRule(PS=30, comment='Support', rerun=1) },
            })
    
    
muon_Rules_supporting.update({              
    # ----------------------------------------
    # ~1Hz from each of these.

    # SM W+nJets
    'HLT_mu26_2j20noL1'                        : {  1000 : TriggerRule(PS=224, comment='Support, Phys-SM, 1 Hz target at 1.3e34') },
    'HLT_mu26_3j20noL1'                        : {  1000 : TriggerRule(PS=156, comment='Support, Phys-SM, 1 Hz target at 1.3e34') },
    'HLT_mu26_4j15noL1'                        : {  1000 : TriggerRule(PS=156, comment='Support, Phys-SM, 1 Hz target at 1.3e34') },
    'HLT_mu26_5j15noL1'                        : {  1000 : TriggerRule(PS=104, comment='Support, Phys-SM, 1 Hz target at 1.3e34') },
    
    # 1.5e34
    'HLT_mu24_2j20noL1'                        : {  1000 : TriggerRule(PS=224, comment='Support, Phys-SM, 1 Hz target at 1e34'),
                                                    15001 : TriggerRule(PS=-1, comment='Disabled') },
    'HLT_mu24_3j20noL1'                        : {  1000 : TriggerRule(PS=156, comment='Support, Phys-SM, 1 Hz target at 1e34'),
                                                    15001 : TriggerRule(PS=-1, comment='Disabled') },
    'HLT_mu24_4j15noL1'                        : {  1000 : TriggerRule(PS=156, comment='Support, Phys-SM, 1 Hz target at 1e34'),
                                                    15001 : TriggerRule(PS=-1, comment='Disabled') },
    'HLT_mu24_5j15noL1'                        : {  1000 : TriggerRule(PS=104, comment='Support, Phys-SM, 1 Hz target at 1e34'),
                                                    15001 : TriggerRule(PS=-1, comment='Disabled') },

    # 1.0e34
    'HLT_mu20_L1MU15_2j20noL1'                 : {  1000 : TriggerRule(PS=117, comment='Support, Phys-SM, 1 Hz target at 3e33', rerun=1),
                                                    lumi_disable_l1_mu15 : TriggerRule(PS=-1, comment='Disabled') },
    'HLT_mu20_L1MU15_3j20noL1'                 : {  1000 : TriggerRule(PS=65, comment='Support, Phys-SM, 1 Hz target at 3e33'),
                                                    lumi_disable_l1_mu15 : TriggerRule(PS=-1, comment='Disabled') },
    'HLT_mu20_L1MU15_4j15noL1'                 : {  1000 : TriggerRule(PS=69, comment='Support, Phys-SM, 1 Hz target at 3e33'),
                                                    lumi_disable_l1_mu15 : TriggerRule(PS=-1, comment='Disabled') },
    'HLT_mu20_L1MU15_5j15noL1'                 : {  1000 : TriggerRule(PS=31, comment='Support, Phys-SM, 1 Hz target at 3e33'),
                                                    lumi_disable_l1_mu15 : TriggerRule(PS=-1, comment='Disabled') },
    })

if MuonRerun :
    muon_Rules_supporting.update({              
            # ----------------------------------------
            # prescale + rerun
            # quite hevy CPU wise ?
            'HLT_mu4'                                  : {  1000 : TriggerRule(rate=2.0, comment='Support', rerun=1) },
            'HLT_mu6'                                  : {  1000 : TriggerRule(rate=1.0, comment='Support', rerun=1) },
            'HLT_mu10'                                 : {  1000 : TriggerRule(rate=1.0, comment='Support', rerun=1) },
            'HLT_mu14'                                 : {  1000 : TriggerRule(rate=1.0, comment='Support', rerun=1) },
            ##NEW20.11.0.2
            #'HLT_mu10_nomucomb'                        : {  1000 : TriggerRule(rate=1.0, comment='Support', rerun=1) },
            #'HLT_mu14_nomucomb'                        : {  1000 : TriggerRule(rate=1.0, comment='Support', rerun=1) },
            'HLT_mu18'                                 : {  1000 : TriggerRule(rate=1.0, comment='Support Express', ESRate=0.4, rerun=1) },
            })

muon_Rules_supporting.update({              
            # ----------------------------------------
            # L1 streamers
            'HLT_noalg_L1MU10'                         : {  1000 : TriggerRule(inputRate=1.0, comment='Support') },
            'HLT_noalg_L1MU15'                         : {  1000 : TriggerRule(inputRate=1.0, comment='Support') },
            'HLT_noalg_L1MU20'                         : {  1000 : TriggerRule(inputRate=1.0, comment='Support') },
            
            # ATR-11652, high rates seen
            # MU4_EMPTY: PS=1000 (700 Hz in this morning's run)
            # MU4_FIRSTEMPTY: PS=30000 (16000 Hz in this morning's run)
            # MU4_UNPAIRED_ISO: PS=10 (6 Hz in this morning's run)
            # 25ns.2 update: 2142 Hz for MU4_E, 2051 Hz for MU4_FE, 50 Hz for MU4_UISO, 5 Hz for MU11_E
            # needs 100 Hz input from MU4_E for IDCosmic
            'L1_MU4_EMPTY'                             : {  1000 : TriggerRule(PS=50,   scaling='bunches', comment='Support') },
            'L1_MU4_FIRSTEMPTY'                        : {  1000 : TriggerRule(PS=1000, scaling='bunches', comment='Support') },
            'L1_MU11_EMPTY'                            : {  1000 : TriggerRule(PS=1,    scaling='bunches', comment='Support') },
            ## for UNPAIRED_ISO, included as override rule at the very end
            ## 'L1_MU4_UNPAIRED_ISO'                      : {  1000 : TriggerRule(PS=10,    comment='') },
            #
            'HLT_noalg_L1MU4_EMPTY'                    : {  1000 : TriggerRule(inputRate=0.5, comment='Support') },
            'HLT_noalg_L1MU4_FIRSTEMPTY'               : {  1000 : TriggerRule(inputRate=0.5, comment='Support') },
            'HLT_noalg_L1MU4_UNPAIRED_ISO'             : {  1000 : TriggerRule(inputRate=0.5, comment='Support') },
            'HLT_noalg_L1MU11_EMPTY'                   : {  1000 : TriggerRule(inputRate=0.5, comment='Support') },
            
            # out-of-time monitoring item, disabled for the moment
            'HLT_noalg_L1MU20_FIRSTEMPTY'              : {  1000 : TriggerRule(PS=-1, comment='Support') },
            
            # ----------------------------------------
            # idperf
            # c.f. Run1: 0.1 Hz for mu18, 0.5 Hz for mu22
            # this has 7 Hz is it needed ? 
            # heavy CPU too
            })

if MuonRerun :
    muon_Rules_supporting.update({ 
            'HLT_mu4_idperf'                           : {  1000 : TriggerRule(rate=0.5, comment='Support', rerun=1) },
            'HLT_mu6_idperf'                           : {  1000 : TriggerRule(rate=0.5, comment='Support Express', ESRate=0.1, rerun=1) },
            'HLT_mu10_idperf'                          : {  1000 : TriggerRule(rate=0.5, comment='Support', rerun=1) },
            'HLT_mu20_idperf'                          : {  1000 : TriggerRule(rate=5, comment='Support Express', ESRate=0.4, rerun=1) },
            'HLT_mu24_idperf'                          : {  1000 : TriggerRule(rate=0.5, comment='Support', rerun=1) },
            })

########################################

muon_Rules={}

RulesList=[muon_Rules_2e34, muon_Rules_1p5e34, muon_Rules_1e34, muon_Rules_0p5e34, muon_Rules_supporting] 
if noSupportingTriggers:
    RulesList=[muon_Rules_2e34, muon_Rules_1p5e34, muon_Rules_1e34, muon_Rules_0p5e34] 

for Rules in RulesList:
    for newrule in Rules.keys():
        if muon_Rules.has_key(newrule):
            print 'FATAL     Physics_pp_v6_rules     Duplicated rule inside physics rule. Cannot be added:',newrule
    muon_Rules.update(Rules)


#################################################################################################
#
#
# B-physics
#
# comments:
# looking at https://atlas-trig-cost.cern.ch/?dir=RateProcessings-2016&type=20.7.3.4&tag=mcv6-20.7.3.4-13TeV-nops-L2.0e34&run=280500:
# MU10_2MU6 is 6 kHz at 2e34,  L1BPH-2M9-2MU6_BPH-2DR15-2MU6 has 800 Hz at 2e34.
# 2MU6 for 1.5e34 7kHz , high. HLT 100 Hz, high too. 
# L1 topo for 1.5e34 is L1BPH-2M8-MU6MU4_BPH-0DR15-MU6MU4, 2kHz at L1 (HLT 100 Hz. too high ?)
# for 1e34 MU6_MU4 for non L1 topo is 16 kHz at L1. 
# for L1 topo, L1BPH-2M8-2MU4_BPH-0DR15-2MU4 has 6 kHz (HLT 100 Hz, too high ?).
# for 5e33 MU6_MU4 ok , 8 kHz at L1.
# for L1 topo, L1BPH-2M8-2MU4_BPH-0DR15-2MU4 has 3 kHz  (HLT 50 Hz)
#
# From James and Semen, feedback:
# For B->J/psi, B0->mumuK* and Bs->mumu samples it's likeefficiency drop is
# L1 2mu4/mu6_2mu6/2mu6/mu10_mu6/2mu10 = 100/59/33/16/5 (with little variation between the samples).
# For Upsilon it's a bit different: 2mu4/mu6_2mu6/2mu6/mu10_mu6/2mu10 = 100/44/23/12/6.
#
# ==> Needs to revisit with fixed rate estimation (multi-MU4,6 rates are currently 20% higher than those in v5)
#################################################################################################


bphys_Rules_2p5e34={}

HLT_list=[
    'HLT_2mu10_bDimu_noL2',
    'HLT_2mu10_bJpsimumu_noL2',
]
bphys_Rules_2p5e34.update(dict(map(None,HLT_list,len(HLT_list)*[{
                    1000 : TriggerRule(PS=1, comment='Primary'), 
                    }])))


HLT_list=[
#    'HLT_mu10_mu6_bBmumuxv2',
#    'HLT_mu10_mu6_bBmumuxv2_L1LFV-MU',
    'HLT_mu10_mu6_bBmumuxv2_noL2_L1LFV-MU',
#    'HLT_mu10_mu6_bBmumux_BpmumuKp_noL2',
#    'HLT_mu10_mu6_bBmumux_BpmumuKp',
#    'HLT_mu10_mu6_bBmumux_BcmumuDsloose',
#    'HLT_mu10_mu6_bBmumux_BcmumuDsloose_L1LFV-MU',
#    'HLT_mu10_mu6_bBmumux_BcmumuDsloose_noL2',
#    'HLT_mu10_mu6_bBmumux_BcmumuDsloose_noL2_L1LFV-MU', 
]
bphys_Rules_2p5e34.update(dict(map(None,HLT_list,len(HLT_list)*[{
                    1000 : TriggerRule(PS=1, comment='Primary'), 
                    }])))

########################################
# 2e34 

bphys_Rules_2e34={}

# ----------
#
# non l1 topo
#
L1_list=['L1_2MU6'] # 9000 Hz
bphys_Rules_2e34.update(dict(map(None,L1_list,len(L1_list)*[{
                    1000 : TriggerRule(PS=1, comment='Primary L1'), 
                    lumi_disable_l1_2mu6 : TriggerRule(rate=500, maxRate=500, comment='Support L1 topo'),
                    }])))

#rejection 100 roughly
HLT_list=[
    'HLT_2mu6_bJpsimumu',
#    'HLT_2mu6_bJpsimumu_noL2',
]
bphys_Rules_2e34.update(dict(map(None,HLT_list,len(HLT_list)*[{
                    1000 : TriggerRule(PS=1, comment='Primary'), 
                    1001 : TriggerRule(PS=-1, comment='Disabled'),
                    }])))

HLT_list=[
    'HLT_2mu6_bBmumu_Lxy0',
#    'HLT_2mu6_bBmumu',
#    'HLT_2mu6_bBmumu_noL2',
    'HLT_2mu6_bUpsimumu',
    'HLT_2mu6_bJpsimumu_Lxy0',
]
bphys_Rules_2e34.update(dict(map(None,HLT_list,len(HLT_list)*[{
                    1000 : TriggerRule(PS=1, comment='Primary'), 
                    }])))

HLT_list=[
    'HLT_2mu6_bBmumux_BpmumuKp',
#    'HLT_2mu6_bBmumux_BpmumuKp_noL2',
]
bphys_Rules_2e34.update(dict(map(None,HLT_list,len(HLT_list)*[{
                    1000 : TriggerRule(PS=1, comment='Primary'), 
                    1001 : TriggerRule(PS=2, comment='Primary'),
                    1001 : TriggerRule(PS=-1, comment='Disabled'),
                    }])))

HLT_list=[
    'HLT_2mu6_bBmumux_BcmumuDsloose',
#    'HLT_2mu6_bBmumux_BcmumuDsloose_noL2',
    'HLT_2mu6_bBmumuxv2',
#    'HLT_2mu6_bBmumuxv2_noL2',
]
bphys_Rules_2e34.update(dict(map(None,HLT_list,len(HLT_list)*[{
                    1000 : TriggerRule(PS=1, comment='Primary'), 
                    1001 : TriggerRule(PS=4, comment='Primary'),
                    1001 : TriggerRule(PS=-1, comment='Disabled'),
                    }])))

HLT_list=[
    'HLT_mu10_mu6_bBmumuxv2_noL2',
]
bphys_Rules_2e34.update(dict(map(None,HLT_list,len(HLT_list)*[{
                    1000 : TriggerRule(PS=1, comment='Primary'), 
                    1001 : TriggerRule(PS=3, comment='Primary'),
                    }])))

HLT_list=[
#    'HLT_2mu6_bBmumuxv2_L12CMU6',
#    'HLT_2mu6_bBmumuxv2_L12MU6-B',
    'HLT_2mu6_bBmumuxv2_L1BPH-2M9-2MU6_BPH-2DR15-2MU6',
#    'HLT_2mu6_bBmumuxv2_noL2_L12CMU6',
#    'HLT_2mu6_bBmumuxv2_noL2_L12MU6-B',
#    'HLT_2mu6_bBmumuxv2_noL2_L1BPH-2M9-2MU6_BPH-2DR15-2MU6',
#    'HLT_2mu6_bBmumux_BcmumuDsloose_L12CMU6',
#    'HLT_2mu6_bBmumux_BcmumuDsloose_L12MU6-B',
    'HLT_2mu6_bBmumux_BcmumuDsloose_L1BPH-2M9-2MU6_BPH-2DR15-2MU6',
#    'HLT_2mu6_bBmumux_BcmumuDsloose_noL2_L12CMU6',
#    'HLT_2mu6_bBmumux_BcmumuDsloose_noL2_L12MU6-B',
#    'HLT_2mu6_bBmumux_BcmumuDsloose_noL2_L1BPH-2M9-2MU6_BPH-2DR15-2MU6',
 ]
bphys_Rules_2e34.update(dict(map(None,HLT_list,len(HLT_list)*[{
                    1000 : TriggerRule(PS=1, comment='Primary'), 
                    1001 : TriggerRule(PS=2, comment='Primary'),
                    }])))
HLT_list=[
    'HLT_2mu6_bJpsimumu_L12CMU6',
#    'HLT_2mu6_bJpsimumu_L12MU6-B',
    'HLT_2mu6_bJpsimumu_L1BPH-2M9-2MU6_BPH-2DR15-2MU6',
#    'HLT_2mu6_bJpsimumu_noL2_L12CMU6',
#    'HLT_2mu6_bJpsimumu_noL2_L12MU6-B',
#    'HLT_2mu6_bJpsimumu_noL2_L1BPH-2M9-2MU6_BPH-2DR15-2MU6',
 ]
bphys_Rules_2e34.update(dict(map(None,HLT_list,len(HLT_list)*[{
                    1000 : TriggerRule(PS=1, comment='Primary'), 
                    }])))


HLT_list=[
    'HLT_2mu6_bBmumu_L12CMU6',
#    'HLT_2mu6_bBmumu_L12MU6-B',
    'HLT_2mu6_bBmumu_L1BPH-2M9-2MU6_BPH-2DR15-2MU6',
#    'HLT_2mu6_bBmumu_noL2_L12CMU6',
#    'HLT_2mu6_bBmumu_noL2_L12MU6-B',
#    'HLT_2mu6_bBmumu_noL2_L1BPH-2M9-2MU6_BPH-2DR15-2MU6',
    'HLT_2mu6_bUpsimumu_L12CMU6',
#    'HLT_2mu6_bUpsimumu_L12MU6-B',
#    'HLT_2mu6_bUpsimumu_L1BPH-8M15-2MU6',
    'HLT_2mu6_bUpsimumu_L1BPH-8M15-2MU6_BPH-0DR22-2MU6',
 ]
bphys_Rules_2e34.update(dict(map(None,HLT_list,len(HLT_list)*[{
                    1000 : TriggerRule(PS=1, comment='Primary'), 
                    }])))

# 3 mu6

HLT_list=['HLT_3mu6_bTau',
          'HLT_3mu6_bUpsi']
bphys_Rules_2e34.update(dict(map(None,HLT_list,len(HLT_list)*[{
                1000 : TriggerRule(PS=1, comment='Primary'), 
                }])))

HLT_list=['HLT_3mu6_bJpsi']
bphys_Rules_2e34.update(dict(map(None,HLT_list,len(HLT_list)*[{
                    1000 : TriggerRule(PS=1, comment='Primary Express', ESRate=0.1), 
                    }])))

########################################
# 1.5e34

bphys_Rules_1p5e34={}
L1_list=['L1_MU6_2MU4'] #16000 Hz
bphys_Rules_1p5e34.update(dict(map(None,L1_list,len(L1_list)*[{
                        1000 : TriggerRule(PS=1, comment='Primary L1'), 
                        1001 : TriggerRule(rate=1000, maxRate=1000, comment='Support L1 topo'),
                        }])))

HLT_list=[
    'HLT_mu6_mu4_bBmumuxv2',
#    'HLT_mu6_mu4_bBmumuxv2_noL2',
    'HLT_mu6_mu4_bJpsimumu',
    'HLT_mu6_mu4_bJpsimumu_Lxy0',
    'HLT_mu6_mu4_bBmumux_BpmumuKp',
#    'HLT_mu6_mu4_bJpsimumu_noL2',
]
bphys_Rules_1p5e34.update(dict(map(None,HLT_list,len(HLT_list)*[{
                    1000 : TriggerRule(PS=1, comment='Primary'), 
                    1001 : TriggerRule(rate=0.5,maxRate=0.5, comment='Support to L1 topo'), 
                    }])))


HLT_list=[
    'HLT_mu6_mu4_bBmumu',
    'HLT_mu6_mu4_bBmumu_Lxy0',
#    'HLT_mu6_mu4_bBmumu_noL2',
    'HLT_mu6_mu4_bBmumux_BcmumuDsloose',
#    'HLT_mu6_mu4_bBmumux_BcmumuDsloose_noL2',
#    'HLT_mu6_mu4_bBmumux_BpmumuKp_noL2',
    'HLT_mu6_mu4_bUpsimumu',
]
bphys_Rules_1p5e34.update(dict(map(None,HLT_list,len(HLT_list)*[{
                    1000 : TriggerRule(PS=1, comment='Primary'), 
                    1001 : TriggerRule(rate=0.5,maxRate=0.5, comment='Support to L1 topo'),
                    }])))

HLT_list=[
#    'HLT_mu6_mu4_bBmumu_L12CMU4',
#    'HLT_mu6_mu4_bBmumu_L12MU4-B',
    'HLT_mu6_mu4_bBmumu_L1BPH-2M8-MU6MU4-B_BPH-0DR15-MU6MU4',
    'HLT_mu6_mu4_bBmumu_L1BPH-2M8-MU6MU4_BPH-0DR15-MU6MU4',
#    'HLT_mu6_mu4_bBmumu_noL2_L12CMU4',
#    'HLT_mu6_mu4_bBmumu_noL2_L12MU4-B',
#    'HLT_mu6_mu4_bBmumu_noL2_L1BPH-2M8-MU6MU4-B_BPH-0DR15-MU6MU4',
#    'HLT_mu6_mu4_bBmumu_noL2_L1BPH-2M8-MU6MU4_BPH-0DR15-MU6MU4',
#    'HLT_mu6_mu4_bBmumux_BcmumuDsloose_L12CMU4',
#    'HLT_mu6_mu4_bBmumux_BcmumuDsloose_L12MU4-B',
#    'HLT_mu6_mu4_bBmumux_BcmumuDsloose_L1BPH-2M8-MU6MU4-B_BPH-0DR15-MU6MU4',
#    'HLT_mu6_mu4_bBmumux_BcmumuDsloose_L1BPH-2M8-MU6MU4_BPH-0DR15-MU6MU4',
#    'HLT_mu6_mu4_bBmumux_BcmumuDsloose_noL2_L12CMU4',
#    'HLT_mu6_mu4_bBmumux_BcmumuDsloose_noL2_L12MU4-B',
#    'HLT_mu6_mu4_bBmumux_BcmumuDsloose_noL2_L1BPH-2M8-MU6MU4-B_BPH-0DR15-MU6MU4',
#    'HLT_mu6_mu4_bBmumux_BcmumuDsloose_noL2_L1BPH-2M8-MU6MU4_BPH-0DR15-MU6MU4',
    ]
bphys_Rules_1p5e34.update(dict(map(None,HLT_list,len(HLT_list)*[{
                    1000 : TriggerRule(PS=1, comment='Primary'), 
                    }])))

HLT_list=[
#    'HLT_mu6_mu4_bBmumuxv2_L12CMU4',
#    'HLT_mu6_mu4_bBmumuxv2_L12MU4-B',
#    'HLT_mu6_mu4_bBmumuxv2_L1BPH-2M8-MU6MU4-B_BPH-0DR15-MU6MU4',
#    'HLT_mu6_mu4_bBmumuxv2_L1BPH-2M8-MU6MU4_BPH-0DR15-MU6MU4',
#    'HLT_mu6_mu4_bBmumuxv2_noL2_L12CMU4',
#    'HLT_mu6_mu4_bBmumuxv2_noL2_L12MU4-B',
#    'HLT_mu6_mu4_bBmumuxv2_noL2_L1BPH-2M8-MU6MU4-B_BPH-0DR15-MU6MU4',
#    'HLT_mu6_mu4_bBmumuxv2_noL2_L1BPH-2M8-MU6MU4_BPH-0DR15-MU6MU4',
#    'HLT_mu6_mu4_bJpsimumu_L12CMU4',
#    'HLT_mu6_mu4_bJpsimumu_L12MU4-B',
    'HLT_mu6_mu4_bJpsimumu_L1BPH-2M8-MU6MU4-B_BPH-0DR15-MU6MU4',
#    'HLT_mu6_mu4_bJpsimumu_L1BPH-2M8-MU6MU4_BPH-0DR15-MU6MU4',
#    'HLT_mu6_mu4_bJpsimumu_noL2_L12CMU4',
#    'HLT_mu6_mu4_bJpsimumu_noL2_L12MU4-B',
#    'HLT_mu6_mu4_bJpsimumu_noL2_L1BPH-2M8-MU6MU4-B_BPH-0DR15-MU6MU4',
#    'HLT_mu6_mu4_bJpsimumu_noL2_L1BPH-2M8-MU6MU4_BPH-0DR15-MU6MU4',
#    'HLT_mu6_mu4_bUpsimumu_L12CMU4',
#    'HLT_mu6_mu4_bUpsimumu_L12MU4-B',
#    'HLT_mu6_mu4_bUpsimumu_L1BPH-8M15-MU6MU4',
    'HLT_mu6_mu4_bUpsimumu_L1BPH-8M15-MU6MU4_BPH-0DR22-MU6MU4'
]
bphys_Rules_1p5e34.update(dict(map(None,HLT_list,len(HLT_list)*[{
                    1000 : TriggerRule(PS=1, comment='Primary'), 
                    1000 : TriggerRule(PS=2, comment='Primary'),
                    1001 : TriggerRule(PS=4, comment='Primary'),
                    }])))

########################################
# 1e34 

bphys_Rules_1e34={}

HLT_list=[
    'HLT_2mu4_bBmumu_L1BPH-2M8-2MU4-BO_BPH-0DR15-2MU4',
    'HLT_2mu4_bBmumu_L1BPH-2M8-2MU4-B_BPH-0DR15-2MU4',
]
bphys_Rules_1e34.update(dict(map(None,HLT_list,len(HLT_list)*[{
                    1000 : TriggerRule(PS=1, comment='Primary'), 
                    1001 : TriggerRule(PS=12, comment='Primary'), 
                    }])))
HLT_list=[
    'HLT_2mu4_bBmumu_L1BPH-2M8-2MU4_BPH-0DR15-2MU4',
]
bphys_Rules_1e34.update(dict(map(None,HLT_list,len(HLT_list)*[{
                    1000 : TriggerRule(PS=1, comment='Primary'), 
                    1000 : TriggerRule(PS=2, comment='Primary'), 
                    1001 : TriggerRule(PS=-1, comment='Disabled'), 
                    }])))

HLT_list=[
    'HLT_2mu4_bUpsimumu_L1BPH-7M15-2MU4_BPH-0DR24-2MU4',
]
bphys_Rules_1e34.update(dict(map(None,HLT_list,len(HLT_list)*[{
                    1000 : TriggerRule(PS=1, comment='Primary'), 
                    1000 : TriggerRule(PS=4, comment='Primary'), 
                    1000 : TriggerRule(PS=5, comment='Primary'), 
                    1001 : TriggerRule(PS=-1, comment='Disabled'), 
                    }])))

HLT_list=[
#    'HLT_2mu4_bBmumux_BcmumuDsloose_L1BPH-1M19-2MU4-BO_BPH-0DR34-2MU4',
#    'HLT_2mu4_bBmumux_BcmumuDsloose_L1BPH-1M19-2MU4-B_BPH-0DR34-2MU4',
#    'HLT_2mu4_bBmumux_BcmumuDsloose_L1BPH-1M19-2MU4_BPH-0DR34-2MU4',
    'HLT_2mu4_bUpsimumu_L1BPH-7M15-2MU4_BPH-0DR24-2MU4-B',
    'HLT_2mu4_bUpsimumu_L1BPH-7M15-2MU4_BPH-0DR24-2MU4-BO',
]
bphys_Rules_1e34.update(dict(map(None,HLT_list,len(HLT_list)*[{
                    1000 : TriggerRule(PS=1, comment='Primary'), 
                    1000 : TriggerRule(PS=2, comment='Primary'), 
                    1000 : TriggerRule(PS=3, comment='Primary'), 
                    1001 : TriggerRule(PS=-1, comment='Disabled'), 
                    }])))

HLT_list=[
    'HLT_2mu4_bBmumuxv2_L1BPH-1M19-2MU4-BO_BPH-0DR34-2MU4',
    'HLT_2mu4_bBmumuxv2_L1BPH-1M19-2MU4-B_BPH-0DR34-2MU4',
    'HLT_2mu4_bBmumuxv2_L1BPH-1M19-2MU4_BPH-0DR34-2MU4',
    'HLT_2mu4_bJpsimumu_L1BPH-2M8-2MU4_BPH-0DR15-2MU4',
]
bphys_Rules_1e34.update(dict(map(None,HLT_list,len(HLT_list)*[{
                    1000 : TriggerRule(PS=1, comment='Primary'), 
                    1000 : TriggerRule(PS=6, comment='Primary'), 
                    1000 : TriggerRule(PS=10, comment='Primary'), 
                    1001 : TriggerRule(PS=-1, comment='Disabled'), 
                    }])))

HLT_list=[
    'HLT_2mu4_bJpsimumu_L1BPH-2M8-2MU4-BO_BPH-0DR15-2MU4',
    'HLT_2mu4_bJpsimumu_L1BPH-2M8-2MU4-B_BPH-0DR15-2MU4',
]
bphys_Rules_1e34.update(dict(map(None,HLT_list,len(HLT_list)*[{
                    1000 : TriggerRule(PS=1, comment='Primary'),
                    1000 : TriggerRule(PS=3, comment='Primary'),
                    1000 : TriggerRule(PS=5, comment='Primary'),
                    1001 : TriggerRule(PS=-1, comment='Disabled'),
                    }])))

########################################
# 0.4e34 

bphys_Rules_0p4e34={}
L1_list=['L1_2MU4']
bphys_Rules_0p4e34.update(dict(map(None,L1_list,len(L1_list)*[{
                        1000 : TriggerRule(PS=1, comment='Primary L1'), 
                        lumi_disable_l1_2mu4 : TriggerRule(rate=500, maxRate=500, comment='Support L1 topo'),
                        }])))

HLT_list=['HLT_2mu4_bBmumu',
          'HLT_2mu4_bBmumuxv2',
          'HLT_2mu4_bBmumux_BcmumuDsloose',
          'HLT_2mu4_bBmumux_BpmumuKp',
          'HLT_2mu4_bJpsimumu',
          'HLT_2mu4_bUpsimumu',
]
bphys_Rules_0p4e34.update(dict(map(None,HLT_list,len(HLT_list)*[{
                    1000 : TriggerRule(PS=1, comment='Primary'), 
                    lumi_disable_l1_2mu4 : TriggerRule(rate=0.5,maxRate=0.5, comment='Support L1 topo'),
                    }])))
if cpuOptimized:

          HLT_list=['HLT_2mu4_bJpsimumu_noL2',]
          bphys_Rules_0p4e34.update(dict(map(None,HLT_list,len(HLT_list)*[{
              1000 : TriggerRule(PS=1, comment='Primary'), 
              lumi_disable_l1_2mu4 : TriggerRule(PS=-1, comment='Disabled'),
              }])))

###########################
# 0.5 e34

bphys_Rules_0p5e34={} 
L1_list=['L1_3MU4']
bphys_Rules_0p5e34.update(dict(map(None,L1_list,len(L1_list)*[{
                    1000 : TriggerRule(PS=1, comment='Primary L1'), 
                    lumi_disable_l1_3mu4 : TriggerRule(PS=2, comment='Support L1'),
                    1001 : TriggerRule(PS=3, comment='Support L1'),
                    }])))

HLT_list=['HLT_3mu4_bJpsi','HLT_3mu4_bUpsi','HLT_3mu4_bTau']
bphys_Rules_0p5e34.update(dict(map(None,HLT_list,len(HLT_list)*[{
                    1000 : TriggerRule(PS=1, comment='Primary'), 
                    lumi_disable_l1_3mu4 : TriggerRule(PS=1, comment='Support'),
                    }])))


########################################
# supporting trigger

bphys_Rules_supporting={}


# ----------------------------------------

HLT_list=['HLT_2mu6_bDimu', 'HLT_2mu6_bDimu_novtx_noos', 'HLT_2mu6_bDimu_noinvm_novtx_ss']
bphys_Rules_supporting.update(dict(map(None,HLT_list,len(HLT_list)*[{
                        1000 : TriggerRule(rate=0.2, comment='Support Express', ESValue=1),
                        lumi_disable_l1_mu6_2mu4 : TriggerRule(rate=1.0, comment='Support Express, 1 Hz when 2MU6 is primary, Express', ESRate=0.2),
                        lumi_disable_l1_2mu6     : TriggerRule(rate=0.2, comment='Support Express', ESValue=1),
                        }])))
if cpuOptimized:
          HLT_list=['HLT_2mu6_bDimu_noL2']
          bphys_Rules_supporting.update(dict(map(None,HLT_list,len(HLT_list)*[{
              1000 : TriggerRule(rate=0.2, comment='Support'),
              lumi_disable_l1_mu6_2mu4 : TriggerRule(rate=1.0, comment='Support'),
              lumi_disable_l1_2mu6     : TriggerRule(rate=0.2, comment='Support'),
          }])))
#------------------------------------


HLT_list=['HLT_mu6_mu4_bDimu_L1BPH-2M8-MU6MU4_BPH-0DR15-MU6MU4', #220 Hz . prescale by 100 ?
          'HLT_mu6_mu4_bDimu_L1BPH-8M15-MU6MU4_BPH-0DR22-MU6MU4', #65 Hz
          'HLT_mu6_mu4_bDimu_novtx_noos_L1BPH-2M8-MU6MU4_BPH-0DR15-MU6MU4', #350 Hz. prescale by 100?
          'HLT_mu6_mu4_bDimu_novtx_noos_L1BPH-8M15-MU6MU4_BPH-0DR22-MU6MU4', # 200 Hz. prescale by 100?
          ]

bphys_Rules_supporting.update(dict(map(None,HLT_list,len(HLT_list)*[{
                        1000 : TriggerRule(rate=1.0, comment='Support'),
                        lumi_disable_l1_2mu4_topo : TriggerRule(rate=1.0, comment='Support'),
                        lumi_disable_l1_mu6_2mu4_topo : TriggerRule(rate=0.2, comment='Support'),
                        }])))
          
# ----------------------------------------

# kn: 2015.09.08  assign 1 Hz even when 2MU4 is supposed to be primary, as 2MU4 can be prescaled by hand on the fly at P1

HLT_list=['HLT_mu6_mu4_bDimu', 'HLT_mu6_mu4_bDimu_novtx_noos', 'HLT_mu6_mu4_bDimu_noinvm_novtx_ss',
         ]
bphys_Rules_supporting.update(dict(map(None,HLT_list,len(HLT_list)*[{
                        1000 : TriggerRule(rate=1.0, comment='Support'),
                        lumi_disable_l1_2mu4 : TriggerRule(rate=1.0, comment='Support'),
                        lumi_disable_l1_mu6_2mu4 : TriggerRule(rate=0.2, comment='Support'),
                        }])))

if cpuOptimized:
          HLT_list=['HLT_mu6_mu4_bDimu_noL2','HLT_mu6_mu4_bDimu_novtx_noos_noL2']
          bphys_Rules_supporting.update(dict(map(None,HLT_list,len(HLT_list)*[{
              1000 : TriggerRule(rate=1.0, comment='Support'),
              lumi_disable_l1_2mu4 : TriggerRule(rate=1.0, comment='Support'),
              lumi_disable_l1_mu6_2mu4 : TriggerRule(rate=0.2, comment='Support'),
          }])))

#---------------------------------------

HLT_list=['HLT_2mu4_bDimu_L1BPH-7M15-2MU4_BPH-0DR24-2MU4', #60 Hz
          'HLT_2mu4_bDimu_novtx_noos_L1BPH-7M15-2MU4_BPH-0DR24-2MU4', #  prescaled by 100? 423 Hz
          ]
bphys_Rules_supporting.update(dict(map(None,HLT_list,len(HLT_list)*[{
                    1000 : TriggerRule(rate=1.0, comment='Support'),
                    lumi_disable_l1_2mu4_topo : TriggerRule(rate=0.2, comment='Support'),
                    }])))
         
# ----------------------------------------

HLT_list=['HLT_2mu4_bDimu', 'HLT_2mu4_bDimu_novtx_noos', 'HLT_2mu4_bDimu_noinvm_novtx_ss',]
bphys_Rules_supporting.update(dict(map(None,HLT_list,len(HLT_list)*[{
                    1000 : TriggerRule(rate=1.0, comment='Support'),
                    lumi_disable_l1_2mu4 : TriggerRule(rate=0.2, comment='Support'),
                    }])))

if cpuOptimized:
          HLT_list=['HLT_2mu4_bDimu_novtx_noos_noL2', 'HLT_2mu4_bDimu_noL2']
          bphys_Rules_supporting.update(dict(map(None,HLT_list,len(HLT_list)*[{
              1000 : TriggerRule(rate=1.0, comment='Support'),
              lumi_disable_l1_2mu4 : TriggerRule(rate=0.2, comment='Support'),
          }])))

# ----------------------------------------

HLT_list=['HLT_3mu4_bDimu']
bphys_Rules_supporting.update(dict(map(None,HLT_list,len(HLT_list)*[{
                    1000 : TriggerRule(rate=1.0, comment='Support'),
                    lumi_disable_l1_3mu4 : TriggerRule(rate=0.2, comment='Support'),
                    }])))

HLT_list=['HLT_3mu6_bDimu']
bphys_Rules_supporting.update(dict(map(None,HLT_list,len(HLT_list)*[{
                    1000 : TriggerRule(PS=-1, comment='Disabled'),
                    lumi_disable_l1_3mu4 : TriggerRule(rate=1.0, comment='Support'),
                    }])))

# ========================================

bphys_Rules_supporting.update({
    # ----------------------------------------
    # di-muons w/o cuts
    'HLT_2mu4'                           : {  1000 : TriggerRule(rate=0.2, comment='Support') },
    'HLT_2mu6'                           : {  1000 : TriggerRule(rate=0.2, comment='Support') },

    # ----------------------------------------
    # idperf at Jpsi mumu
    # 1.5 Hz @ Run1
    'HLT_mu4_mu4_idperf_bJpsimumu_noid'  : {  1000 : TriggerRule(rate=1.5, comment='Support Express, Jpsi idperf', ESRate=0.1) },

    # ----------------------------------------
    # Jpsi Trkloose
    # 4 Hz for mu6 @ Run1 -> raised to 20 Hz
    'HLT_mu4_bJpsi_Trkloose'             : {  1000 : TriggerRule(rate=40.0, comment='Support, Jpsi idperf'),
                                              1001 : TriggerRule(rate=20.0, comment='Support, Jpsi idperf')},
    'HLT_mu6_bJpsi_Trkloose'             : {  1000 : TriggerRule(rate=2.0, comment='Support, Jpsi idperf')},
    'HLT_mu10_bJpsi_Trkloose'            : {  1000 : TriggerRule(rate=2.0, comment='Support, Jpsi idperf')},
    'HLT_mu18_bJpsi_Trkloose'            : {  1000 : TriggerRule(rate=2.0, comment='Support, Jpsi idperf')},
})

########################################

bphys_Rules={}

RulesList=[bphys_Rules_2p5e34, bphys_Rules_2e34, bphys_Rules_1p5e34, bphys_Rules_1e34, bphys_Rules_0p5e34, bphys_Rules_0p4e34, bphys_Rules_supporting]
if noSupportingTriggers:
    RulesList=[bphys_Rules_2p5e34, bphys_Rules_2e34, bphys_Rules_1p5e34, bphys_Rules_1e34, bphys_Rules_0p5e34, bphys_Rules_0p4e34]
 
for Rules in RulesList:
    for newrule in Rules.keys():
        if bphys_Rules.has_key(newrule):
            print 'FATAL     Physics_pp_v6_rules     Duplicated rule inside physics rule. Cannot be added:',newrule
    bphys_Rules.update(Rules)


#################################################################################################
#
#
# Missing ET
#
# we miss some xe100_xxx from L1 XE60. also, maybe we need to activate also all tighter xe thresholds?
#
#################################################################################################



########################################
# 2p5e34 

met_Rules_2p5e34={}

L1_list=['L1_XE70', 'L1_XE80']
met_Rules_2p5e34.update(dict(map(None,L1_list,len(L1_list)*[{
                    1000 : TriggerRule(PS=1, comment='Primary L1'), 
                    }])))

HLT_list=['HLT_xe100',
    #
    'HLT_xe120',      'HLT_xe120_mht',  'HLT_xe120_pueta',
    'HLT_xe120_pufit','HLT_xe120_tc_em','HLT_xe120_tc_lcw',
    'HLT_xe120_wEFMu',      'HLT_xe120_mht_wEFMu',  'HLT_xe120_pueta_wEFMu',
    'HLT_xe120_pufit_wEFMu','HLT_xe120_tc_em_wEFMu','HLT_xe120_tc_lcw_wEFMu',
    ]
met_Rules_2p5e34.update(dict(map(None,HLT_list,len(HLT_list)*[{
                    1000 : TriggerRule(PS=1, comment='Primary L1'), 
                    }])))

########################################
# 2e34


met_Rules_2e34={}

L1_list=['L1_XE60'] #4000 Hz
met_Rules_2e34.update(dict(map(None,L1_list,len(L1_list)*[{
                    1000 : TriggerRule(PS=1, comment='Primary L1'), 
                    20001 : TriggerRule(PS=-1, comment='Disabled'), 
                    }])))

HLT_list=['HLT_xe100_L1XE60',  #37Hz
          'HLT_xe100_mht_L1XE60',  'HLT_xe100_pueta_L1XE60', 
          'HLT_xe100_pufit_L1XE60',
          'HLT_xe100_tc_lcw_L1XE60', # 71Hz
          # Need to add? 'HLT_xe100_tc_em_L1XE60',
          # Need to add? 'HLT_xe100_wEFMu_L1XE60', 'HLT_xe100_mht_wEFMu_L1XE60',
          # Need to add?  'HLT_xe100_pueta_wEFMu_L1XE60', 'HLT_xe100_pufit_wEFMu_L1XE60',
          # Need to add? 'HLT_xe100_tc_em_wEFMu_L1XE60', 'HLT_xe100_tc_lcw_wEFMu_L1XE60',
    ]
met_Rules_2e34.update(dict(map(None,HLT_list,len(HLT_list)*[{
                    1000 : TriggerRule(PS=1, comment='Primary'), 
                    20001 : TriggerRule(PS=-1, comment='Disabled'),
                    }])))


########################################
# 1.5e34 

met_Rules_1p5e34={}

# same as what runs at 2e34


########################################
# 1e34 

met_Rules_1e34={}

# why when prescaled, some rate is needed at L1?
L1_list=['L1_XE50']
met_Rules_1e34.update(dict(map(None,L1_list,len(L1_list)*[{
                   1000 : TriggerRule(PS=1, comment='Primary L1'), 
                   10001 : TriggerRule(rate=500, maxRate=500, comment='Support'),
                   }])))

HLT_list=[
    'HLT_xe90_L1XE50', #30Hz
    'HLT_xe90_tc_lcw_L1XE50', #90Hz
    'HLT_xe90_mht_L1XE50',
    'HLT_xe90_tc_em_L1XE50', 
    'HLT_xe100_pueta_L1XE50', 'HLT_xe100_pufit_L1XE50',
    'HLT_xe90_wEFMu_L1XE50',
    'HLT_xe90_tc_lcw_wEFMu_L1XE50', 'HLT_xe90_mht_wEFMu_L1XE50',
    'HLT_xe90_tc_em_wEFMu_L1XE50',
    'HLT_xe100_pueta_wEFMu_L1XE50', 'HLT_xe100_pufit_wEFMu_L1XE50',
    
#    'HLT_xe100_mht_L1XE50',  'HLT_xe100_pueta_L1XE50', #100 Hz
#    'HLT_xe100_pufit_L1XE50','HLT_xe100_tc_em_L1XE50','HLT_xe100_tc_lcw_L1XE50',
#    'HLT_xe100_wEFMu_L1XE50',      'HLT_xe100_mht_wEFMu_L1XE50',  'HLT_xe100_pueta_wEFMu_L1XE50',
#    'HLT_xe100_pufit_wEFMu_L1XE50','HLT_xe100_tc_em_wEFMu_L1XE50','HLT_xe100_tc_lcw_wEFMu_L1XE50',
    ]
met_Rules_1e34.update(dict(map(None,HLT_list,len(HLT_list)*[{
                    1000 : TriggerRule(PS=1, comment='Primary L1'), 
                    10001 : TriggerRule(rate=0.5, comment='Support'),
                    }])))


########################################
# 0.5e34 

met_Rules_0p5e34={}

# same as what runs at 1e34

########################################
# supporting trigger

met_Rules_supporting={}

# what is the point of this?
L1_list=['L1_XE35', 'L1_XE45']
met_Rules_supporting.update(dict(map(None,L1_list,len(L1_list)*[{
    1000 : TriggerRule(rate=100, maxRate=100, comment='Support L1'), 
    }])))

# what is the point of this? it is not set to rerun, and it does not go to ES.
HLT_list=[
    # XE35
    'HLT_xe35',      'HLT_xe35_mht',      'HLT_xe35_pueta',      'HLT_xe35_pufit',      'HLT_xe35_tc_em',      'HLT_xe35_tc_lcw',
    'HLT_xe35_wEFMu','HLT_xe35_mht_wEFMu','HLT_xe35_pueta_wEFMu','HLT_xe35_pufit_wEFMu','HLT_xe35_tc_em_wEFMu','HLT_xe35_tc_lcw_wEFMu',
    ]
met_Rules_supporting.update(dict(map(None,HLT_list,len(HLT_list)*[{
                    1000 : TriggerRule(rate=2.0, comment='Support'), 
                    }])))


#----------------------------------------

HLT_list=['HLT_xe50']  # rerun needed for tau (ATR-11530)
met_Rules_supporting.update(dict(map(None,HLT_list,len(HLT_list)*[{
                    1000 : TriggerRule(rate=1.0, comment='Support', rerun=1), 
                    }])))

HLT_list=['HLT_xe70_L1XE45']  # rerun needed for tau (ATR-11530)
met_Rules_supporting.update(dict(map(None,HLT_list,len(HLT_list)*[{
                    1000 : TriggerRule(PS=0, comment='Support', rerun=1), 
                    }])))


# ========================================
# very important
# rerun, so very expensive. But really needed to do emaulation for all cases, and measure efficiency in data.

HLT_list=[
    'HLT_xe0noL1_l2fsperf',       'HLT_xe0noL1_l2fsperf_mht',   'HLT_xe0noL1_l2fsperf_pueta',
    'HLT_xe0noL1_l2fsperf_pufit', 'HLT_xe0noL1_l2fsperf_tc_em', 'HLT_xe0noL1_l2fsperf_tc_lcw',
    ]
met_Rules_supporting.update(dict(map(None,HLT_list,len(HLT_list)*[{
    1000 : TriggerRule(PS=0, comment='Support',rerun=1), 
    }])))

# ========================================

HLT_list=['HLT_noalg_L1XE35']
met_Rules_supporting.update(dict(map(None,HLT_list,len(HLT_list)*[{
    1000 : TriggerRule(inputRate=0.5, comment='Support Express', ESValue=1), 
    }])))

# ========================================
# why is this here and not in Monitoring rules?
HLT_list=['HLT_noalg_L1RD0_FILLED']
met_Rules_supporting.update(dict(map(None,HLT_list,len(HLT_list)*[{
    1000 : TriggerRule(inputRate=15, comment='Support Express',ESRate=0.1), 
    }])))

# why is this here and not in Monitoring rules?
HLT_list=['HLT_noalg_L1RD0_EMPTY']
met_Rules_supporting.update(dict(map(None,HLT_list,len(HLT_list)*[{
    1000 : TriggerRule(rate=0.5, comment='Support Express',ESRate=0.5), 
    }])))


########################################

met_Rules={}

RulesList=[met_Rules_2p5e34, met_Rules_2e34, met_Rules_1p5e34, met_Rules_1e34, met_Rules_0p5e34, met_Rules_supporting] 
if noSupportingTriggers:
    RulesList=[met_Rules_2p5e34, met_Rules_2e34, met_Rules_1p5e34, met_Rules_1e34, met_Rules_0p5e34] 

for Rules in RulesList:
    for newrule in Rules.keys():
        if met_Rules.has_key(newrule):
            print 'FATAL     Physics_pp_v6_rules     Duplicated rule inside physics rule. Cannot be added:',newrule
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
                     1000 : TriggerRule(PS=1, comment='Primary L1'), 
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
    # 5 jet top analysis
    'HLT_5j65_0eta240','HLT_5j70_0eta240',
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
    # llp
    'HLT_10j40_L14J20',
    'HLT_10j40_L16J15',
    ]
jet_Rules_2e34.update(dict(map(None,HLT_list,len(HLT_list)*[{
                       1000 : TriggerRule(PS=1, comment='Primary'), 
                       }])))

# ----------------------------------------

HLT_list=['HLT_noalg_L1J400',]
jet_Rules_2e34.update(dict(map(None,HLT_list,len(HLT_list)*[{
                       1000 : TriggerRule(PS=1, comment='Support Express', ESRate=0.4), 
                       }])))

# ----------------------------------------

HLT_list=['HLT_j460_a10_sub_L1J100']
jet_Rules_2e34.update(dict(map(None,HLT_list,len(HLT_list)*[{
                       1000 : TriggerRule(PS=1, comment='Primary Express', ESRate=0.2), 
                       }])))
# ----------------------------------------

HLT_list=['HLT_j400', 'HLT_j360_320eta490']
jet_Rules_2e34.update(dict(map(None,HLT_list,len(HLT_list)*[{
                       1000 : TriggerRule(PS=1, comment='Primary, pass-through 1Hz ATR-11862', PTRate=1.0), 
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
                       1000 : TriggerRule(PS=1, comment='Primary'), 
                       10001 : TriggerRule(PS=-1, comment='Disabled'),
                       }])))

HLT_list=['HLT_5j70']
jet_Rules_1e34.update(dict(map(None,HLT_list,len(HLT_list)*[{
                        1000 : TriggerRule(PS=1, comment='Primary'), 
                        10001 : TriggerRule(rate=1.0, comment='Support'),
                        }])))

########################################
# 0p5e34 - 0p75e34

jet_Rules_0p5e34={}

L1_list=['L1_J75','L1_J75.31ETA49']
jet_Rules_0p5e34.update(dict(map(None,L1_list,len(L1_list)*[{
                   1000 : TriggerRule(PS=1, comment='Primary L1'), 
                   lumi_disable_l1_j75 : TriggerRule(rate=100, maxRate=100, comment='Support'),
                   }])))

HLT_list=[
    # forward jet
    'HLT_j260_320eta490','HLT_j260_320eta490_jes','HLT_j260_320eta490_lcw','HLT_j260_320eta490_lcw_jes','HLT_j260_320eta490_lcw_nojcalib','HLT_j260_320eta490_nojcalib',
    # ht
    'HLT_ht850_L1J75', 'HLT_ht850_L1J100',
    ]
jet_Rules_0p5e34.update(dict(map(None,HLT_list,len(HLT_list)*[{
                   1000 : TriggerRule(PS=1, comment='Primary'), 
                   lumi_disable_l1_j75 : TriggerRule(PS=-1, comment='Disabled'),
                   }])))

# ----------------------------------------

L1_list=['L1_3J40']
jet_Rules_0p5e34.update(dict(map(None,L1_list,len(L1_list)*[{
                    1000 : TriggerRule(PS=1, comment='Primary L1'), 
                    lumi_disable_l1_3j40 : TriggerRule(PS=-1, comment='Disabled'),
                    }])))

HLT_list=['HLT_4j85','HLT_4j85_jes','HLT_4j85_lcw','HLT_4j85_lcw_jes','HLT_4j85_lcw_nojcalib','HLT_4j85_nojcalib',]
jet_Rules_0p5e34.update(dict(map(None,HLT_list,len(HLT_list)*[{
                     1000 : TriggerRule(PS=1, comment='Primary'), 
                     lumi_disable_l1_3j40 : TriggerRule(rate=1.0, comment='Support'),
                     }])))

# ----------------------------------------

L1_list=['L1_4J15',]
jet_Rules_0p5e34.update(dict(map(None,L1_list,len(L1_list)*[{
                     1000 : TriggerRule(PS=1, comment='Primary L1'), 
                     lumi_disable_l1_4j15 : TriggerRule(rate=2000, maxRate=2000, comment='Support. Used for beamspot'),
                     }])))

HLT_list=['HLT_5j60',]
jet_Rules_0p5e34.update(dict(map(None,HLT_list,len(HLT_list)*[{
                      1000 : TriggerRule(PS=1, comment='Primary'), 
                      lumi_disable_l1_4j15 : TriggerRule(rate=1.0, comment='Support'),
                      }])))

HLT_list=['HLT_6j45','HLT_6j45_0eta240','HLT_6j45_0eta240_L14J20','HLT_6j45_0eta240_L15J150ETA25',]
jet_Rules_0p5e34.update(dict(map(None,HLT_list,len(HLT_list)*[{
                      1000 : TriggerRule(PS=1, comment='Primary'), 
                      lumi_disable_l1_4j15 : TriggerRule(PS=-1, comment='Disabled'),
                      }])))

# ----------------------------------------

HLT_list=['HLT_j360_a10r_L1J100',]
jet_Rules_0p5e34.update(dict(map(None,HLT_list,len(HLT_list)*[{
                          1000 : TriggerRule(PS=1, comment='Primary'), 
                          lumi_disable_hlt_j360_a10r : TriggerRule(PS=-1, comment='Disabled'),
                          }])))

HLT_list=['HLT_j360']
jet_Rules_0p5e34.update(dict(map(None,HLT_list,len(HLT_list)*[{
                        1000 : TriggerRule(PS=1, comment='Primary Express', ESRate=0.2), 
                        lumi_disable_hlt_j360 : TriggerRule(rate=0.2, comment='Support Express', ESValue=1),
                        }])))

########################################
# 0p15e34 

jet_Rules_0p15e34={}

HLT_list=['HLT_ht700_L1J75', 'HLT_ht700_L1J100']
jet_Rules_0p15e34.update(dict(map(None,HLT_list,len(HLT_list)*[{
                                1000 : TriggerRule(PS=1, comment='Primary'), 
                                1001 : TriggerRule(PS=-1, comment='Disabled'),
                                }])))

########################################
# prescaled 

jet_Rules_supporting = {
    'L1_RD0_FILLED'  : { 1000 : TriggerRule(rate=1000, maxRate=1000, scaling='bunches', comment='Support L1') },
    }

# ========================================
# prescaled single jets
# 64 Hz total. is this really needed !?! make sure what can be set in CPS it is, to make this small

L1_list=['L1_J15', 'L1_J20', 'L1_J25', 'L1_J30', 'L1_J40', 'L1_J50', 'L1_J85']
jet_Rules_supporting.update(dict(map(None,L1_list,len(L1_list)*[{
                       1000 : TriggerRule(rate=200, maxRate=200, comment='Support L1'),
                       }])))

HLT_list=[
    # RD0
    'HLT_j35_jes','HLT_j35_lcw','HLT_j35_lcw_jes','HLT_j35_lcw_nojcalib','HLT_j35_nojcalib',
    'HLT_j45_L1RD0_FILLED',
    'HLT_j55_L1RD0_FILLED',
    'HLT_j85_L1RD0_FILLED',
    # J20
    'HLT_j85_jes',
    'HLT_j85_cleanT',
    'HLT_j85_cleanL',
    'HLT_j85_cleanLLP',
    # J25
    'HLT_j100',
    # J40
    'HLT_j150',
    # J50
    'HLT_j175',
    'HLT_j200',
    # J75
    'HLT_j260',
    # J85
    'HLT_j300',
    'HLT_j320',
    ]
jet_Rules_supporting.update(dict(map(None,HLT_list,len(HLT_list)*[{
                        1000 : TriggerRule(rate=1.0, comment='Support'), 
                        }])))

HLT_list=['HLT_j15',]
jet_Rules_supporting.update(dict(map(None,HLT_list,len(HLT_list)*[{
                        1000 : TriggerRule(rate=4.0, comment='Support', PTRate=0.5), 
                        }])))

HLT_list=[
    # RD0
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
    ]
jet_Rules_supporting.update(dict(map(None,HLT_list,len(HLT_list)*[{
                          1000 : TriggerRule(rate=1.0, comment='Support'), 
                          }])))

HLT_list=['HLT_j110_320eta490']
jet_Rules_supporting.update(dict(map(None,HLT_list,len(HLT_list)*[{
                           1000 : TriggerRule(rate=1.0, comment='Support'),
                           }])))

HLT_list=['HLT_j60']
jet_Rules_supporting.update(dict(map(None,HLT_list,len(HLT_list)*[{
                           1000 : TriggerRule(rate=1.0, comment='Support Express', ESRate=0.2), 
                           }])))

HLT_list=['HLT_j60_L1RD0_FILLED',]
jet_Rules_supporting.update(dict(map(None,HLT_list,len(HLT_list)*[{
                           1000 : TriggerRule(rate=1.0, comment='Support Express', ESRate=0.2), 
                           }])))

HLT_list=['HLT_j25']
jet_Rules_supporting.update(dict(map(None,HLT_list,len(HLT_list)*[{
                           1000 : TriggerRule(rate=4.0, comment='Support Express', ESRate=0.8),
                           }])))

# ----------------------------------------

L1_list=['L1_J15.31ETA49', 'L1_J20.28ETA31', 'L1_J50.31ETA49',]
jet_Rules_supporting.update(dict(map(None,L1_list,len(L1_list)*[{
                          1000 : TriggerRule(rate=500, maxRate=500, comment='Support L1'),
                          }])))

L1_list=['L1_J20.31ETA49','L1_J30.31ETA49']
jet_Rules_supporting.update(dict(map(None,L1_list,len(L1_list)*[{
                           1000 : TriggerRule(rate=500, maxRate=500, comment='Support L1'),
                           }])))
    
HLT_list=[
    # J20.28ETA31
    'HLT_j85_280eta320','HLT_j85_280eta320_jes','HLT_j85_280eta320_lcw','HLT_j85_280eta320_lcw_jes','HLT_j85_280eta320_lcw_nojcalib','HLT_j85_280eta320_nojcalib',
    # J50.31ETA49
    'HLT_j175_320eta490','HLT_j175_320eta490_jes','HLT_j175_320eta490_lcw','HLT_j175_320eta490_lcw_jes','HLT_j175_320eta490_lcw_nojcalib','HLT_j175_320eta490_nojcalib',
    ]
jet_Rules_supporting.update(dict(map(None,HLT_list,len(HLT_list)*[{
                             1000 : TriggerRule(rate=1.0, comment='Support'), 
                             }])))

HLT_list=['HLT_j60_320eta490',]
jet_Rules_supporting.update(dict(map(None,HLT_list,len(HLT_list)*[{
                             1000 : TriggerRule(rate=4.0, comment='Support Express', ESRate=0.2),
                             }])))

HLT_list=['HLT_j85_320eta490',]
jet_Rules_supporting.update(dict(map(None,HLT_list,len(HLT_list)*[{
                             1000 : TriggerRule(rate=1.0, comment='Support'),
                             }])))

HLT_list=['HLT_j60_280eta320',]
jet_Rules_supporting.update(dict(map(None,HLT_list,len(HLT_list)*[{
                             1000 : TriggerRule(rate=1.0, comment='Support Express', ESRate=0.2), 
                             }])))

# ----------------------------------------
# forward jets seeeded by random

HLT_list=['HLT_j15_320eta490', 'HLT_j25_320eta490',]
jet_Rules_supporting.update(dict(map(None,HLT_list,len(HLT_list)*[{
                             1000 : TriggerRule(rate=4.0, comment='Support'),
                             }])))

HLT_list=['HLT_j35_320eta490',]
jet_Rules_supporting.update(dict(map(None,HLT_list,len(HLT_list)*[{
                             1000 : TriggerRule(rate=1.0, comment='Support'),
                             }])))

# ----------------------------------------
# j0_perf

HLT_list=['HLT_j0_perf_L1RD0_FILLED']
jet_Rules_supporting.update(dict(map(None,HLT_list,len(HLT_list)*[{
                             1000 : TriggerRule(rate=0.5, comment='Support'),
                             }])))

# ========================================
# Djet
# 8 Hz. make sure what needs to be in CPS it is.

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
                             1000 : TriggerRule(rate=1.0, comment='Support'), 
                             }])))

HLT_list=['HLT_j45_j45_320eta490','HLT_j85_j85_320eta490',]
jet_Rules_supporting.update(dict(map(None,HLT_list,len(HLT_list)*[{
                             1000 : TriggerRule(rate=1.0, comment='Support'), 
                             }])))

# ========================================
# prescaled multi jets
# 6 Hz

L1_list=['L1_3J15']
jet_Rules_supporting.update(dict(map(None,L1_list,len(L1_list)*[{
                              1000 : TriggerRule(rate=1000, maxRate=1000, comment='Support'),
                              }])))

HLT_list=[
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
                              1000 : TriggerRule(rate=1.0, comment='Support'), 
                              }])))

# ----------------------------------------
# SUSY 0-lepton + multijets analysis request on 5j45
# - envisaged prescale: 7-10 (flat, i.e. no change)
# - envisaged period for the prescale change: early 50 ns data-taking for lumis <= 5e33

HLT_list=['HLT_5j45']
jet_Rules_supporting.update(dict(map(None,HLT_list,len(HLT_list)*[{
                              1000 : TriggerRule(PS=6, comment='Support, PS=6 to get 10 Hz at 3e33, only up to 5e33'), 
                              1000 : TriggerRule(PS=110, comment='Support, 1 Hz @ 5e33'), 
                              1000 : TriggerRule(PS=220, comment='Support, 1 Hz @ 10e33'), 
                              1001 : TriggerRule(PS=330, comment='Support, 1 Hz @ 15e33'), 
                              }])))

########################################

jet_Rules={}

RulesList=[jet_Rules_2e34, jet_Rules_1e34, jet_Rules_0p5e34, jet_Rules_0p15e34, jet_Rules_supporting] 
if noSupportingTriggers:
    RulesList=[jet_Rules_2e34, jet_Rules_1e34, jet_Rules_0p5e34, jet_Rules_0p15e34]
 
for Rules in RulesList:
    for newrule in Rules.keys():
        if jet_Rules.has_key(newrule):
            print 'FATAL     Physics_pp_v6_rules     Duplicated rule inside physics rule. Cannot be added:',newrule
    jet_Rules.update(Rules)


#################################################################################################
#
#
# b-Jet
#
#
#################################################################################################
# for bjets, the default in 2016 is split and bmv2cxxxx
# several boffperf_split will be needed as well, as support
#
# no idea of L1 topo plans for bjets -> still missing
#
########################################
# 2e34 

bjet_Rules_2e34={}

#L1_4J20 is in jet_Rules_2e34
       
HLT_list=[
    'HLT_2j55_bmv2c2077_split_2j55',
    'HLT_2j45_bmv2c2070_split_2j45',
    'HLT_j75_bmv2c2077_split_3j75',
    'HLT_j70_bmv2c2070_split_3j70',
#    'HLT_j100_2j55_bmv2c2077_split',
    'HLT_j175_bmv2c2077_split_j60_bmv2c2077_split',
    'HLT_j300_bmv2c2085_split',
    ]
bjet_Rules_2e34.update(dict(map(None,HLT_list,len(HLT_list)*[{
                 1000 : TriggerRule(PS=1, comment='Primary'), 
                 }])))

HLT_list=[##NEW20.11.0.1 'HLT_j225_bmv2c2070_split',
]
bjet_Rules_2e34.update(dict(map(None,HLT_list,len(HLT_list)*[{
                    1000 : TriggerRule(PS=1, comment='Primary, 35 Hz at 1p5e34, ATR-13386'),
                    }])))

HLT_list=[##NEW20.11.0.1 'HLT_j100_2j55_bmv2c2060_split',
]
bjet_Rules_2e34.update(dict(map(None,HLT_list,len(HLT_list)*[{ 
                    1000 : TriggerRule(PS=1, comment='Primary, ATR-13386'),
                    }])))

HLT_list=[##NEW20.11.0.1 'HLT_j55_bmv2c2060_ht500_L14J20',
]
bjet_Rules_2e34.update(dict(map(None,HLT_list,len(HLT_list)*[{
                    1000 : TriggerRule(PS=1, comment='Primary, ATR-13386'),
                    }])))

########################################
# 1e34 

bjet_Rules_1e34={}

L1_list=['L1_3J25.0ETA23']
bjet_Rules_1e34.update(dict(map(None,L1_list,len(L1_list)*[{
                           1000 : TriggerRule(PS=1, comment='Primary L1'), 
                           lumi_disable_l1_3j25_0eta23 : TriggerRule(PS=-1, comment='Disabled'), 
                           }])))


# the first two also start from L13J25.0ETA23, confusing notation wrt above, should fix it.

HLT_list=[
    'HLT_2j75_bmv2c2077_split_j75',
    'HLT_2j70_bmv2c2070_split_j70',

    'HLT_2j45_bmv2c2077_split_2j45_L13J25.0ETA23',
    'HLT_2j35_bmv2c2070_split_2j35_L13J25.0ETA23',
    'HLT_2j55_bmv2c2077_split_2j55_L13J25.0ETA23',
    'HLT_2j45_bmv2c2070_split_2j45_L13J25.0ETA23',
    'HLT_j70_bmv2c2077_split_3j70_L13J25.0ETA23',
    'HLT_j65_bmv2c2070_split_3j65_L13J25.0ETA23',
    'HLT_j75_bmv2c2077_split_3j75_L13J25.0ETA23',
    'HLT_j70_bmv2c2070_split_3j70_L13J25.0ETA23',

    ]
bjet_Rules_1e34.update(dict(map(None,HLT_list,len(HLT_list)*[{
                           1000 : TriggerRule(PS=1, comment='Primary'), 
                           lumi_disable_l1_3j25_0eta23 : TriggerRule(PS=-1, comment='Disabled'), 
                           }])))


# --------------------------------------

HLT_list=['HLT_mu4_j40_dr05_3j40_L14J20',]
bjet_Rules_1e34.update(dict(map(None,HLT_list,len(HLT_list)*[{
                      1000 : TriggerRule(PS=-1, comment='Disabled until lowers rate. Primary, VBF H->bb, hh->bbbb'), 
                      lumi_disable_mu_j_phys : TriggerRule(PS=-1, comment='Disabled'), 
                      }])))


HLT_list=[##NEW20.11.0.1 'HLT_j225_bmv2c2077_split',
]
bjet_Rules_1e34.update(dict(map(None,HLT_list,len(HLT_list)*[{
                    1000 : TriggerRule(PS=1, comment='Primary, ATR-13386'), 
                    10001 : TriggerRule(PS=-1, comment='Disabled, rate is 35 Hz'), 
                    }])))

HLT_list=[##NEW20.11.0.1 'HLT_j100_2j55_bmv2c2070_split',
]
bjet_Rules_1e34.update(dict(map(None,HLT_list,len(HLT_list)*[{
                    1000 : TriggerRule(PS=1, comment='Primary, ATR-13386'), 
                    10001 : TriggerRule(PS=-1, comment='Disabled, rate is 30 Hz'),
                    }])))

########################################
# 0.5e34 

bjet_Rules_0p5e34={}

# these also start from L13J25.0ETA23, confusing notation wrt above, should fix it.
HLT_list=['HLT_2j70_bmv2c2077_split_j70',
          'HLT_2j65_bmv2c2070_split_j65',          
          ]
bjet_Rules_0p5e34.update(dict(map(None,HLT_list,len(HLT_list)*[{
                      1000 : TriggerRule(PS=1, comment='Primary'), 
                      lumi_disable_2b1j_70 : TriggerRule(PS=-1, comment='Disabled'),
                      }])))


# --------------------------------------

HLT_list=['HLT_j150_bmv2c2077_split_j50_bmv2c2077_split',
          ]
bjet_Rules_0p5e34.update(dict(map(None,HLT_list,len(HLT_list)*[{
                   1000 : TriggerRule(PS=1, comment='Primary'), 
                   lumi_disable_2b_150 : TriggerRule(PS=-1, comment='Disabled'),
                   }])))


# --------------------------------------

L1_list=['L1_4J15.0ETA25']
bjet_Rules_0p5e34.update(dict(map(None,L1_list,len(L1_list)*[{
                           1000 : TriggerRule(PS=1, comment='Primary'), 
                           lumi_disable_l1_4j15_0eta25 : TriggerRule(PS=-1, comment='Disabled'),
                           }])))
HLT_list=['HLT_2j45_bmv2c2077_split_2j45_L14J15.0ETA25',
          'HLT_2j35_bmv2c2070_split_2j35_L14J15.0ETA25',
          'HLT_j65_bmv2c2070_split_3j65_L14J15.0ETA25',
          'HLT_j70_bmv2c2077_split_3j70_L14J15.0ETA25',
          ]

bjet_Rules_0p5e34.update(dict(map(None,HLT_list,len(HLT_list)*[{
                           1000 : TriggerRule(PS=1, comment='Primary'), 
                           lumi_disable_l1_4j15_0eta25 : TriggerRule(PS=-1, comment='Disabled'),
                           }])))


HLT_list=['HLT_j225_bmv2c2085_split',
]
bjet_Rules_0p5e34.update(dict(map(None,HLT_list,len(HLT_list)*[{
                    1000 : TriggerRule(PS=1, comment='Primary'), 
                    1001 : TriggerRule(PS=-1, comment='Disabled, rate is 35 Hz'), 
                    }])))


HLT_list=['HLT_j100_2j55_bmv2c2077_split',
]
bjet_Rules_0p5e34.update(dict(map(None,HLT_list,len(HLT_list)*[{
                    1000 : TriggerRule(PS=1, comment='Primary'),
                    1001 : TriggerRule(PS=-1, comment='Disabled, rate is 30 Hz'), 
                    }])))

########################################
# 0.3e34 

bjet_Rules_0p3e34={}

HLT_list=['HLT_j175_bmv2c2077_split',
    ]
bjet_Rules_0p3e34.update(dict(map(None,HLT_list,len(HLT_list)*[{
                          1000 : TriggerRule(PS=1, comment='Primary'), 
                          1001 : TriggerRule(rate=1., comment='Support'),
                          }])))

########################################
# 0.15e34 

bjet_Rules_0p15e34={}


########################################
# supporting

bjet_Rules_supporting={}


# ========================================
# lepton + boffperf + split,
# purpose is (data) to save sample of ttbar (using the main e/mu trigger, so no additional rate) for calibration, where jets and btagging info are prepared 

HLT_list=[
    'HLT_2e17_lhloose_2j35_boffperf_split',
    'HLT_e26_lhtight_iloose_2j35_boffperf_split', # we miss the nod0 version here, and the ivarloose version too.
    #
    'HLT_mu26_imedium_2j35_boffperf_split', # we miss ivarmedium here
    'HLT_2mu14_2j35_boffperf_split',
    ]
bjet_Rules_supporting.update(dict(map(None,HLT_list,len(HLT_list)*[{
                        1000 : TriggerRule(PS=1, comment='Support'), 
                        }])))

# ========================================
# bperf

L1_list=['L1_3J15.0ETA25', 'L1_3J20']
bjet_Rules_supporting.update(dict(map(None,L1_list,len(L1_list)*[{
                        1000 : TriggerRule(rate=1000, maxRate=1000, comment='Support L1'),
                        }])))

# are these in CPS? else, what's the point?

HLT_list=[
    # RD0
                      'HLT_j15_bperf_split',  'HLT_j15_boffperf',  
    'HLT_j25_bperf',  'HLT_j25_bperf_split',  'HLT_j25_boffperf',  'HLT_j25_boffperf_split',
    # J15
    'HLT_j35_bperf',  'HLT_j35_bperf_split',  'HLT_j35_boffperf',  'HLT_j35_boffperf_split',
    'HLT_j45_bperf',  'HLT_j45_bperf_split',  'HLT_j45_boffperf',  'HLT_j45_boffperf_split',
    # J20
                      'HLT_j55_bperf_split',  'HLT_j55_boffperf',  
    'HLT_j85_bperf',  'HLT_j85_bperf_split',  'HLT_j85_boffperf',  'HLT_j85_boffperf_split',
    # J30
                      'HLT_j110_bperf_split', 'HLT_j110_boffperf', 
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
    # why are these two needed ?

    'HLT_j45_bperf_3j45_L14J20',
    'HLT_j55_bperf_3j55',
    ]
bjet_Rules_supporting.update(dict(map(None,HLT_list,len(HLT_list)*[{
                            1000 : TriggerRule(rate=0.1, comment='Support'), 
                            }])))

# purpose in data? # duplicated as the list above without ES

HLT_list=[
    # RD0
    'HLT_j15_boffperf_split', # Needs to define 'express' in menu first
    # J20
    'HLT_j55_boffperf_split',
    # J30
    'HLT_j110_boffperf_split',

    ]
bjet_Rules_supporting.update(dict(map(None,HLT_list,len(HLT_list)*[{
                           1000 : TriggerRule(rate=0.5, comment='Support Express', ESRate=0.2), 
                           }])))
# ========================================
# mu + jets, bperf (does not matter trigger input variables to btagging), for calibration for offline btagging

# ----------------------------------------
#review motivation/PS. Think not needed anymore


#L1_list=['L1_MU4_3J20']
#bjet_Rules_supporting.update(dict(map(None,L1_list,len(L1_list)*[{
#                          1000 : TriggerRule(rate=300, maxRate=300, comment='Support L1'),
#                          }]))) 

#L1_list=['L1_MU4_3J15']
#bjet_Rules_supporting.update(dict(map(None,L1_list,len(L1_list)*[{
#                         1000 : TriggerRule(rate=300, maxRate=300, comment='Support L1'),
#                         }])))


L1_list=['L1_MU4_J12']
bjet_Rules_supporting.update(dict(map(None,L1_list,len(L1_list)*[{
                         1000 : TriggerRule(rate=500, maxRate=500, comment='Support L1'),
                         }])))
L1_list=['L1_MU6_J20']
bjet_Rules_supporting.update(dict(map(None,L1_list,len(L1_list)*[{
                         1000 : TriggerRule(rate=500, maxRate=500, comment='Support L1'),
                         }])))
L1_list=['L1_MU6_J40']
bjet_Rules_supporting.update(dict(map(None,L1_list,len(L1_list)*[{
                         1000 : TriggerRule(rate=500, maxRate=500, comment='Support L1'),
                         }])))


L1_list=['L1_MU6_J75']
bjet_Rules_supporting.update(dict(map(None,L1_list,len(L1_list)*[{
                         1000 : TriggerRule(PS=1, comment='Support L1'),
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
                       1000 : TriggerRule(rate=1.0, comment='Support'),
                       }])))


########################################

bjet_Rules={}

RulesList=[bjet_Rules_2e34, bjet_Rules_1e34, bjet_Rules_0p5e34, bjet_Rules_0p3e34, bjet_Rules_0p15e34, bjet_Rules_supporting] 
if noSupportingTriggers:
    RulesList=[bjet_Rules_2e34, bjet_Rules_1e34, bjet_Rules_0p5e34, bjet_Rules_0p3e34, bjet_Rules_0p15e34]
 
for Rules in RulesList:
    for newrule in Rules.keys():
        if bjet_Rules.has_key(newrule):
            print 'FATAL     Physics_pp_v6_rules     Duplicated rule inside physics rule. Cannot be added:',newrule

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


#L1 topo # remove
L1_list=['L1_DR-TAU20ITAU12I-J25','L1_DR-EM15TAU12I-J25','L1_DR-MU10TAU12I_TAU12I-J25'] # 5000 , 2500 , 1300 Hz
tau_Rules_2e34.update(dict(map(None,L1_list,len(L1_list)*[{
                    1000 : TriggerRule(PS=1, comment='Primary L1'),
                    }])))

HLT_list=['HLT_tau35_medium1_tracktwo_tau25_medium1_tracktwo_L1DR-TAU20ITAU12I-J25', #30 Hz
          'HLT_e17_lhmedium_tau25_medium1_tracktwo_L1DR-EM15TAU12I-J25', #13 Hz
          'HLT_e17_lhmedium_nod0_tau25_medium1_tracktwo_L1DR-EM15TAU12I-J25',
          'HLT_mu14_tau25_medium1_tracktwo_L1DR-MU10TAU12I_TAU12I-J25', 
          'HLT_tau80_medium1_tracktwo_L1TAU60_tau25_medium1_tracktwo_L1TAU12',] #40 Hz 
tau_Rules_2e34.update(dict(map(None,HLT_list,len(HLT_list)*[{
                            1000 : TriggerRule(PS=1, comment='Primary'), 
                            }])))

L1_list=[
    # single
    'L1_TAU60',
    #
    # tau + xe, w/ jet
    'L1_TAU20IM_2J20_XE45',
    #
    # di-tau + xe
    'L1_TAU20IM_2TAU12IM_XE35',#'L1_TAU20IM_2TAU12IM_XE40',
    #
    # tau + e (high et, w/o jet)
    'L1_EM15HI_TAU40_2TAU15',
    #
    # tau + e + xe
    'L1_EM15HI_2TAU12IM_XE35',
    #
    # tau + mu + xe
    'L1_MU10_TAU12IM_XE35',#'L1_MU10_TAU12IM_XE40',
    ]

tau_Rules_2e34.update(dict(map(None,L1_list,len(L1_list)*[{
    1000 : TriggerRule(PS=1, comment='Primary L1'), 
    }])))

    # do not forget to put rerun for single parts of combined triggers when the trigegr gets prescaled

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
    # should add ivarloose version no?
    #
    # tau + mu (high et, w/o jet)
    'HLT_mu14_iloose_tau35_medium1_tracktwo',
    'HLT_mu14_tau35_medium1_tracktwo',
    #
    # tau + mu + xe
    'HLT_mu14_iloose_tau25_medium1_tracktwo_xe50',
    'HLT_mu14_tau25_medium1_tracktwo_xe50',
    # tau tau high pt
    'HLT_tau80_medium1_tracktwo_L1TAU60_tau50_medium1_tracktwo_L1TAU12',

    ]

tau_Rules_2e34.update(dict(map(None,HLT_list,len(HLT_list)*[{
    1000 : TriggerRule(PS=1, comment='Primary'), 
    }])))

########################################
# 1p5e34 

tau_Rules_1p5e34={}

#L1 topo (more efficient than tau+X+jets regular)

L1_list=['L1_DR-TAU20ITAU12I','L1_DR-EM15TAU12I','L1_MU10_TAU12I-J25'] # 5800 , 4500 , 1840 Hz

tau_Rules_1p5e34.update(dict(map(None,L1_list,len(L1_list)*[{
                           1000 : TriggerRule(PS=1, comment='Primary L1'),
                           15001 : TriggerRule(PS=-1, comment='Disable'),
                           }])))

HLT_list=['HLT_tau35_medium1_tracktwo_tau25_medium1_tracktwo_L1DR-TAU20ITAU12I', #30 Hz
          'HLT_e17_lhmedium_tau25_medium1_tracktwo_L1DR-EM15TAU12I', #12 Hz
          'HLT_e17_lhmedium_nod0_tau25_medium1_tracktwo_L1DR-EM15TAU12I',
          'HLT_mu14_tau25_medium1_tracktwo_L1MU10_TAU12I-J25',] # 30 Hz
          
tau_Rules_1p5e34.update(dict(map(None,HLT_list,len(HLT_list)*[{
                            1000 : TriggerRule(PS=1, comment='Primary'), 
                            15001 : TriggerRule(PS=-1, comment='Disable'),
                            }])))
          
# --------------------------------------

L1_list=['L1_EM15HI_2TAU12IM_J25_3J12'] # 3000 Hz
tau_Rules_1p5e34.update(dict(map(None,L1_list,len(L1_list)*[{
                            1000 : TriggerRule(PS=1, comment='Primary L1'), 
                            lumi_nonl1topo_disable_tau_e : TriggerRule(rate=500, maxRate=500, comment='Support'),
                            }])))

# need to add ivarloose no ?  # 12 Hz 
HLT_list=['HLT_e17_medium_iloose_tau25_medium1_tracktwo', 'HLT_e17_lhmedium_nod0_iloose_tau25_medium1_tracktwo',
      'HLT_e17_medium_tau25_medium1_tracktwo', 'HLT_e17_lhmedium_nod0_tau25_medium1_tracktwo','HLT_e17_lhmedium_tau25_medium1_tracktwo']
tau_Rules_1p5e34.update(dict(map(None,HLT_list,len(HLT_list)*[{
                            1000 : TriggerRule(PS=1, comment='Primary'), 
                            lumi_nonl1topo_disable_tau_e : TriggerRule(rate=0.1, comment='Support'),
                            }])))

# due to tau+e+xe, for trigger efficiency measurement (ATR-12219)
HLT_list=['HLT_e17_lhmedium_iloose_tau25_medium1_tracktwo']
tau_Rules_1p5e34.update(dict(map(None,HLT_list,len(HLT_list)*[{
                            1000 : TriggerRule(PS=1, comment='Primary'), 
                            lumi_nonl1topo_disable_tau_e : TriggerRule(rate=0.1, comment='Support, ATR-12219', rerun=1),
                            }])))

# --------------------------------------

L1_list=['L1_MU10_TAU12IM_J25_2J12',] # 1500 Hz
tau_Rules_1p5e34.update(dict(map(None,L1_list,len(L1_list)*[{
                             1000 : TriggerRule(PS=1, comment='Primary L1'), 
                             lumi_nonl1topo_disable_tau_mu : TriggerRule(rate=500, maxRate=500, comment='Support'),  # why is this needed?
                             }])))

# rerun due to tau+mu+xe, for trigger efficiency measurement (ATR-12219) # 15 and 37 Hz
HLT_list=['HLT_mu14_iloose_tau25_medium1_tracktwo','HLT_mu14_tau25_medium1_tracktwo']
tau_Rules_1p5e34.update(dict(map(None,HLT_list,len(HLT_list)*[{
                             1000 : TriggerRule(PS=1, comment='Primary'), 
                             lumi_nonl1topo_disable_tau_mu : TriggerRule(rate=0.1, comment='Support, ATR-12219', rerun=1),
                             }])))

# --------------------------------------

L1_list=['L1_TAU20IM_2TAU12IM_J25_2J20_3J12',] # 6200 Hz
tau_Rules_1p5e34.update(dict(map(None,L1_list,len(L1_list)*[{
                              1000 : TriggerRule(PS=1, comment='Primary L1'), 
                              lumi_nonl1topo_disable_tau_tau : TriggerRule(rate=500, maxRate=500, comment='Support'),
                              }])))

# rerun due to tau+tau+xe, for trigger efficiency measurement(ATR-12219)  20 Hz
HLT_list=['HLT_tau35_medium1_tracktwo_tau25_medium1_tracktwo',] 
tau_Rules_1p5e34.update(dict(map(None,HLT_list,len(HLT_list)*[{
                              1000 : TriggerRule(PS=1, comment='Primary'), 
                              lumi_nonl1topo_disable_tau_tau : TriggerRule(rate=0.1, comment='Support, ATR-12219', rerun=1),
                              }])))

HLT_list=['HLT_tau35_tight1_tracktwo_tau25_tight1_tracktwo',]
tau_Rules_1p5e34.update(dict(map(None,HLT_list,len(HLT_list)*[{
                              1000 : TriggerRule(PS=1, comment=''), 
                              lumi_nonl1topo_disable_tau_tau : TriggerRule(rate=0.1, comment='Prescaled'),
                              }])))

########################################
# 1e34 

tau_Rules_1e34={}

HLT_list=['HLT_tau125_medium1_tracktwo', 
'HLT_tau125_medium1_track','HLT_tau60_medium1_tracktwo_tau35_medium1_tracktwo',]
tau_Rules_1e34.update(dict(map(None,HLT_list,len(HLT_list)*[{
                            1000 : TriggerRule(PS=1, comment='Primary'), 
                            10001 : TriggerRule(PS=0, comment='Support',rerun=1),
                            }])))


########################################
# 0.7e34 

tau_Rules_0p7e34={}

HLT_list=['HLT_tau35_loose1_tracktwo_tau25_loose1_tracktwo',]
tau_Rules_0p7e34.update(dict(map(None,HLT_list,len(HLT_list)*[{
                          1000 : TriggerRule(PS=1, comment='Primary'), 
                          1001 : TriggerRule(rate=0.1, comment='Support'), # is higher rate needed ? for background determination
                          }])))

# --------------------------------------

L1_list=['L1_TAU20_2TAU12_XE35']
tau_Rules_0p7e34.update(dict(map(None,L1_list,len(L1_list)*[{
                         1000 : TriggerRule(PS=1, comment='Primary L1'), 
                         lumi_l1jet_enable_tau_tau : TriggerRule(PS=-1, comment='Disabled'),
                         }])))

HLT_list=['HLT_tau35_medium1_tracktwo_L1TAU20_tau25_medium1_tracktwo_L1TAU12_xe50',]
tau_Rules_0p7e34.update(dict(map(None,HLT_list,len(HLT_list)*[{
                         1000 : TriggerRule(PS=1, comment='Primary'), 
                         lumi_l1jet_enable_tau_tau : TriggerRule(PS=-1, comment='Disabled'),
                         }])))

# --------------------------------------

L1_list=['L1_TAU20IM_2TAU12IM',]
tau_Rules_0p7e34.update(dict(map(None,L1_list,len(L1_list)*[{
                         1000 : TriggerRule(PS=1, comment='Primary L1'), 
                         lumi_l1jet_enable_tau_tau : TriggerRule(PS=20, comment='Support, PS=20 from ATR-11855'),
                         }])))

# due to tau+tau+xe, for trigger efficiency measurement  (ATR-12219)
HLT_list=['HLT_tau35_medium1_tracktwo_tau25_medium1_tracktwo_L1TAU20IM_2TAU12IM',]
tau_Rules_0p7e34.update(dict(map(None,HLT_list,len(HLT_list)*[{
                         1000 : TriggerRule(PS=1, comment='Primary'), 
                         lumi_l1jet_enable_tau_tau : TriggerRule(PS=0, comment='Support, ATR-12219', rerun=1),
                         }])))

HLT_list=['HLT_tau35_tight1_tracktwo_tau25_tight1_tracktwo_L1TAU20IM_2TAU12IM',]
tau_Rules_0p7e34.update(dict(map(None,HLT_list,len(HLT_list)*[{
                         1000 : TriggerRule(PS=1, comment='Primary'), 
                         lumi_l1jet_enable_tau_tau : TriggerRule(PS=-1, comment='Disabled'),
                         }])))

########################################

# 0.5e34 

tau_Rules_0p5e34={}

L1_list=['L1_TAU40']
tau_Rules_0p5e34.update(dict(map(None,L1_list,len(L1_list)*[{
                     1000 : TriggerRule(PS=1, comment='Primary L1'), 
                     lumi_disable_l1_tau40 : TriggerRule(PS=-1, comment='Disabled'),
                     }])))

# --------------------------------------

HLT_list=['HLT_tau80_medium1_tracktwo_L1TAU60',]
tau_Rules_0p5e34.update(dict(map(None,HLT_list,len(HLT_list)*[{
                     1000 : TriggerRule(PS=1, comment='Primary'), 
                     1001 : TriggerRule(PS=0, comment='Disabled', rerun=1),
                     }])))

L1_list=['L1_EM15TAU12I-J25', 'L1_DR-MU10TAU12I', 'L1_TAU20ITAU12I-J25']  
tau_Rules_0p5e34.update(dict(map(None,L1_list,len(L1_list)*[{
                     1000 : TriggerRule(PS=1, comment='Primary L1'),
                     1001 : TriggerRule(PS=5, comment='Support Commission'),
                     }])))  
HLT_list=['HLT_e17_lhmedium_tau25_medium1_tracktwo_L1EM15TAU12I-J25', 'HLT_e17_lhmedium_nod0_tau25_medium1_tracktwo_L1EM15TAU12I-J25', 'HLT_mu14_tau25_medium1_tracktwo_L1DR-MU10TAU12I', 'HLT_tau35_medium1_tracktwo_tau25_medium1_tracktwo_L1TAU20ITAU12I-J25' ]
tau_Rules_0p5e34.update(dict(map(None,HLT_list,len(HLT_list)*[{
                     1000 : TriggerRule(PS=1, comment='Primary, ATR-13366'),
                     1001 : TriggerRule(PS=5, comment='Primary, ATR-13366'),
                     }])))            

########################################
# 0.3e34 

tau_Rules_0p3e34={}

HLT_list=['HLT_tau80_medium1_tracktwo', 'HLT_tau80_medium1_track',]
tau_Rules_0p3e34.update(dict(map(None,HLT_list,len(HLT_list)*[{
                     1000 : TriggerRule(PS=1, comment='Primary'), 
                     1001 : TriggerRule(PS=0, comment='Support', rerun=1),
                     }])))

# --------------------------------------

L1_list=['L1_TAU30']
tau_Rules_0p3e34.update(dict(map(None,L1_list,len(L1_list)*[{
                     1000 : TriggerRule(PS=1, comment='Primary L1'), 
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
                     1000 : TriggerRule(PS=1, comment='Primary L1'), 
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
                     1000 : TriggerRule(PS=1, comment='Primary'), 
                     lumi_l1iso_enable_tau : TriggerRule(PS=-1, comment='Disabled'),
                     }])))

########################################
# 0.15e34 

tau_Rules_0p15e34={}

HLT_list=['HLT_tau35_loose1_tracktwo_tau25_loose1_tracktwo_L1TAU20IM_2TAU12IM',]
tau_Rules_0p15e34.update(dict(map(None,HLT_list,len(HLT_list)*[{
                     1000 : TriggerRule(PS=1, comment='Primary'), 
                     1001 : TriggerRule(PS=-1, comment='Disabled'),
                     }])))

########################################
# supporting

if TauRerun:
          tau_Rules_supporting = {
            # --------------------------------------
            # perf
            #'HLT_tau0_perf_ptonly_L1TAU12'                : {  1000 : TriggerRule(rate=2.0, comment='Support Express', ESRate=0.4, rerun=1) }, # will enable with new repro of tightperf
            #'HLT_tau0_perf_ptonly_L1TAU60'                : {  1000 : TriggerRule(rate=1.0, comment='Support Express', ESRate=0.2, rerun=1) }, # will enable with new repro of tightperf

            # --------------------------------------
            # single & di-tau
            'HLT_tau25_medium1_tracktwo'                  : {  1000 : TriggerRule(rate=0.2, comment='Support Express', ESValue=1, rerun=1) },
            #'HLT_tau25_perf_tracktwo'                     : {  1000 : TriggerRule(rate=0.5, comment='Support Express', ESValue=1, rerun=1) }, # will enable with new repro of tightperf
            'HLT_tau35_perf_tracktwo_tau25_perf_tracktwo' : {  1000 : TriggerRule(rate=0.4, comment='Support Express', ESValue=1, rerun=1) },

            # --------------------------------------
            # Z T&P
            # assign rerun (ATR-12219)
            'HLT_mu14_iloose_tau25_perf_tracktwo'         : {  1000 : TriggerRule(rate=0.4, comment='Support Express', ESValue=1, rerun=1) },

            # --------------------------------------
            # idperf
            #'HLT_tau25_idperf_tracktwo'                   : {  1000 : TriggerRule(rate=0.5, comment='Support Express', ESValue=1, rerun=1) }, # will enable with new repro of tightperf
            #'HLT_tau25_idperf_track'                      : {  1000 : TriggerRule(rate=0.5, comment='Support Express', ESValue=1, rerun=1) }, # will enable with new repro of tightperf
            #'HLT_tau160_idperf_tracktwo'                  : {  1000 : TriggerRule(rate=0.4, comment='Support Express', ESRate=0.1, rerun=1) }, # will enable with new repro of tightperf
            #'HLT_tau160_idperf_track'                     : {  1000 : TriggerRule(rate=0.4, comment='Support Express', ESRate=0.1, rerun=1) }, # will enable with new repro of tightperf
           }  

# ========================================

if TauRerun :
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
              1000 : TriggerRule(PS=0, comment='Support', rerun=1), 
          }])))

########################################

tau_Rules={}

RulesList=[tau_Rules_2e34, tau_Rules_1p5e34, tau_Rules_1e34, tau_Rules_0p7e34, tau_Rules_0p5e34, tau_Rules_0p3e34, tau_Rules_0p15e34, tau_Rules_supporting] 
if noSupportingTriggers:
    RulesList=[tau_Rules_2e34, tau_Rules_1p5e34, tau_Rules_1e34, tau_Rules_0p7e34, tau_Rules_0p5e34, tau_Rules_0p3e34, tau_Rules_0p15e34] 

for Rules in RulesList:
    for newrule in Rules.keys():
        if tau_Rules.has_key(newrule):
            print 'FATAL     Physics_pp_v6_rules     Duplicated rule inside physics rule. Cannot be added:',newrule
    tau_Rules.update(Rules)


#################################################################################################
#
#
# e/gamma
#
#
#################################################################################################


########################################
# runs up to 2e34 :

egamma_Rules_2e34={}

L1_list=['L1_EM22VHI',]
egamma_Rules_2e34.update(dict(map(None,L1_list,len(L1_list)*[{
                   1000 : TriggerRule(PS=1, comment='Primary L1'), 
                   20001 :  TriggerRule(PS=-1, comment='Disabled'), 
                   }])))

L1_list=['L1_EM24VHI','L1_2EM15VH','L1_EM15VH_3EM7',]
egamma_Rules_2e34.update(dict(map(None,L1_list,len(L1_list)*[{
                   1000 : TriggerRule(PS=1, comment='Primary L1'), 
                   }])))

HLT_list=[
    # 1e. it should really be e28_lhtight fed from EM24VHI but that exists only in menu v6    
    'HLT_e26_lhtight_smooth_iloose','HLT_e26_lhtight_smooth_ivarloose',
    'HLT_e26_lhtight_iloose','HLT_e26_lhtight_ivarloose',
    'HLT_e26_lhtight_nod0_iloose','HLT_e26_lhtight_nod0_ivarloose',
    'HLT_e26_tight_iloose',
]
egamma_Rules_2e34.update(dict(map(None,HLT_list,len(HLT_list)*[{
                   1000 : TriggerRule(PS=1, comment='Primary'), 
                   20001 : TriggerRule(PS=-1, comment='Disabled')}])))

HLT_list=[
    'HLT_e26_lhtight_nod0_ringer_iloose','HLT_e26_lhtight_nod0_ringer_ivarloose',  
]
egamma_Rules_2e34.update(dict(map(None,HLT_list,len(HLT_list)*[{
                   1000 : TriggerRule(PS=1, comment='Primary, Commission'), 
                   20001 : TriggerRule(PS=-1, comment='Disabled')}])))

HLT_list=[
    'HLT_e26_lhtight_cutd0dphideta_iloose',
]
egamma_Rules_2e34.update(dict(map(None,HLT_list,len(HLT_list)*[{
                   1000 : TriggerRule(PS=-1, comment='Support, Commission'), 
                   20001 : TriggerRule(PS=-1, comment='Disabled')}])))


HLT_list=[
    # 1e. it should really be e28_lhtight fed from EM24VHI but that exists only in menu v6
    'HLT_e28_lhtight_smooth_iloose','HLT_e28_lhtight_smooth_ivarloose',
    'HLT_e28_lhtight_iloose','HLT_e28_lhtight_ivarloose',
    'HLT_e28_lhtight_nod0_iloose','HLT_e28_lhtight_nod0_ivarloose',
    'HLT_e28_tight_iloose',
]
egamma_Rules_2e34.update(dict(map(None,HLT_list,len(HLT_list)*[{
                    1000 : TriggerRule(PS=1, comment='Primary'), 
                    }])))

HLT_list=[
    'HLT_e28_lhtight_nod0_ringer_iloose','HLT_e28_lhtight_nod0_ringer_ivarloose',
    ]
egamma_Rules_2e34.update(dict(map(None,HLT_list,len(HLT_list)*[{
                    1000 : TriggerRule(PS=1, comment='Support, Commission'), 
                    }])))

HLT_list=[
    'HLT_e60_lhmedium','HLT_e60_lhmedium_nod0',
    'HLT_e60_medium',
    # need to add alternative for when EM22VHI is prescaled ==> added EM24VHI seeded ones
    'HLT_e60_lhmedium_L1EM24VHI', 'HLT_e60_lhmedium_nod0_L1EM24VHI', ##NEW20.11.0.2 'HLT_e60_medium_L1EM24VHI',
    'HLT_e140_lhloose','HLT_e140_lhloose_nod0', 'HLT_e140_loose',
    # need to add alternative for when EM22VHI is prescaled ==> added EM24VHI seeded ones
    'HLT_e140_lhloose_L1EM24VHI', 'HLT_e140_lhloose_nod0_L1EM24VHI', 'HLT_e140_loose_L1EM24VHI',
    # need to add alternative for when EM22VHI is prescaled ==> missing EM24VHI seeded one
    'HLT_e200_etcut',
    #
    # 2e
    'HLT_2e17_lhvloose_nod0','HLT_2e17_lhvloose',
    ## 'HLT_e24_lhmedium_e9_lhmedium', #ATR-12825. need an alterntative to use EM22VH/EM24VH. too high rate at 1e34 already # tentatively disabled until e26_lhmedium_e9_lhmedium is added in menu
    #
    # 3e
    'HLT_e17_lhmedium_2e9_lhmedium','HLT_e17_lhmedium_nod0_2e9_lhmedium_nod0',
    # need to add alternative for when EM22VHI is prescaled
    # 1g
    'HLT_g200_etcut',
    #
    # 2g
    'HLT_2g60_loose_L12EM15VH',
    'HLT_2g22_tight',
    #
    # 3g
    'HLT_3g20_loose',
    ]
egamma_Rules_2e34.update(dict(map(None,HLT_list,len(HLT_list)*[{
    1000 : TriggerRule(PS=1, comment='Primary')
}])))

HLT_list=['HLT_e60_lhmedium_cutd0dphideta','HLT_2e17_lhloose_cutd0dphideta']
egamma_Rules_2e34.update(dict(map(None,HLT_list,len(HLT_list)*[{
    1000 : TriggerRule(PS=-1, comment='Support, Commission')
}])))

# --------------------------------------
# express stream changes for 5e33. Put here because this is where items are enabled

HLT_list=['HLT_2e17_lhloose']
egamma_Rules_2e34.update(dict(map(None,HLT_list,len(HLT_list)*[{
                       1000 : TriggerRule(PS=1, comment='Primary Express', ESValue=0), 
                       lumi_disable_l1_2em10vh : TriggerRule(PS=1, comment='Support Express', ESRate=0.2), 
                       }])))


HLT_list=['HLT_g140_loose']
egamma_Rules_2e34.update(dict(map(None,HLT_list,len(HLT_list)*[{
                    1000 : TriggerRule(PS=1, comment='Primary Express', ESValue=0), 
                    1001 : TriggerRule(PS=1, comment='Primary Express', ESRate=0.2), 
                    }])))

HLT_list=['HLT_g35_medium_g25_medium']
egamma_Rules_2e34.update(dict(map(None,HLT_list,len(HLT_list)*[{
                         1000 : TriggerRule(PS=1, comment='Primary', ESValue=0), 
                         lumi_disable_hlt_2g_loose : TriggerRule(PS=1, comment='Support Express', ESRate=0.4), 
    }])))

########################################
# runs up to 1e34 :

egamma_Rules_1e34={}

L1_list=['L1_EM20VHI',]
egamma_Rules_1e34.update(dict(map(None,L1_list,len(L1_list)*[{
                       1000 : TriggerRule(PS=1, comment='Primary L1'),
                       lumi_disable_l1_em20vhi : TriggerRule(PS=-1, comment='Disabled'),
                       }])))

HLT_list=['HLT_e24_lhtight_iloose','HLT_e24_lhtight_nod0_iloose','HLT_e24_lhtight_ivarloose','HLT_e24_lhtight_nod0_ivarloose',
          'HLT_e24_tight_iloose', 
          ]
egamma_Rules_1e34.update(dict(map(None,HLT_list,len(HLT_list)*[{
                    1000 : TriggerRule(PS=1, comment='Primary'),
                    1001 : TriggerRule(PS=-1, comment='Disabled, on 24022016'),
                    lumi_disable_l1_em20vhi : TriggerRule(PS=-1, comment='Disabled'),
                    }])))

HLT_list=['HLT_e24_lhtight_cutd0dphideta_iloose',]					
egamma_Rules_1e34.update(dict(map(None,HLT_list,len(HLT_list)*[{
                    1000 : TriggerRule(PS=-1, comment='Support, Commission'),                    
                    lumi_disable_l1_em20vhi : TriggerRule(PS=-1, comment='Disabled'),
                    }])))

#-------

HLT_list=[
    # 3e
    'HLT_e17_lhloose_2e9_lhloose','HLT_e17_lhloose_nod0_2e9_lhloose_nod0',
    # 2g
    'HLT_2g50_loose',
    'HLT_2g20_tight',
    # 3g
    'HLT_2g20_loose_g15_loose',
    ]
egamma_Rules_1e34.update(dict(map(None,HLT_list,len(HLT_list)*[{
                    1000 : TriggerRule(PS=1, comment='Primary'), 
                    10001 : TriggerRule(PS=-1, comment='Disabled'),
    }])))

# --------------------------------------

# SUSY request, but too high rate
# MCG 2015.05.28  unique rate to be revisited
# support ? is it still needed? ==> No, no SUSY analyses use this

HLT_list=['HLT_e60_lhvloose_nod0']
egamma_Rules_1e34.update(dict(map(None,HLT_list,len(HLT_list)*[{
                    1000 : TriggerRule(PS=12, comment='Support, 1 Hz target at 3e33', rerun=1),
                    10001 : TriggerRule(PS=-1, comment='Disabled'),
                    }])))

# --------------------------------------


# --------------------------------------

L1_list=['L1_2EM13VH',]
egamma_Rules_1e34.update(dict(map(None,L1_list,len(L1_list)*[{
                       1000 : TriggerRule(PS=1, comment='L1'),
                       lumi_disable_l1_2em13vh : TriggerRule(PS=-1, comment='Disabled'),
                       }])))

HLT_list=['HLT_2e15_lhvloose_L12EM13VH','HLT_2e15_lhvloose_nod0_L12EM13VH',]
egamma_Rules_1e34.update(dict(map(None,HLT_list,len(HLT_list)*[{
                       1000 : TriggerRule(PS=1, comment='Primary'), 
                       lumi_disable_l1_2em13vh : TriggerRule(PS=-1, comment='Disabled'),
                       }])))

########################################
# 0.5e34 

egamma_Rules_0p5e34={}


# --------------------------------------

HLT_list=['HLT_e24_lhmedium_iloose','HLT_e24_lhmedium_ivarloose','HLT_e24_lhmedium_nod0_iloose','HLT_e24_lhmedium_nod0_ivarloose', 
          ]
egamma_Rules_0p5e34.update(dict(map(None,HLT_list,len(HLT_list)*[{
                       1000 : TriggerRule(PS=1, comment='Primary'), 
                       1001 : TriggerRule(PS=-1, comment='Disabled'),
                       }])))
# --------------------------------------
# keep some L1 rate as support,500 Hz, when prescaled. Ask why?
L1_list=['L1_EM20VH']
egamma_Rules_0p5e34.update(dict(map(None,L1_list,len(L1_list)*[{
                    1000 : TriggerRule(PS=1, comment='Primary L1'),
                    lumi_l1iso_enable_em : TriggerRule(rate=500, maxRate=500, comment='Support'),
                    }])))

# --------------------------------------
# is this needed, to run in paralle with e24_lhmedium_iloose? would it not be better to keep it PS, low rate, for commissioning only? maybe PS=1 only at start?

HLT_list=['HLT_e24_lhmedium_iloose_L1EM20VH','HLT_e24_lhmedium_nod0_iloose_L1EM20VH',
          'HLT_e24_lhmedium_ivarloose_L1EM20VH',
          'HLT_e24_lhmedium_nod0_ivarloose_L1EM20VH',
          ]
egamma_Rules_0p5e34.update(dict(map(None,HLT_list,len(HLT_list)*[{
                    1000 : TriggerRule(PS=1, comment='Primary'), 
                    lumi_l1iso_enable_em : TriggerRule(PS=-1, comment='Disabled'),
                    }])))


HLT_list=['HLT_e24_lhmedium_cutd0dphideta_iloose_L1EM20VH',
          ]
egamma_Rules_0p5e34.update(dict(map(None,HLT_list,len(HLT_list)*[{
                    1000 : TriggerRule(PS=-1,  comment='Support, Commission'), 
                    lumi_l1iso_enable_em : TriggerRule(PS=-1, comment='Disabled'),
                    }])))


HLT_list=['HLT_e24_lhmedium_L1EM20VH',
          'HLT_e24_lhmedium_nod0_L1EM20VH'
          ]
egamma_Rules_0p5e34.update(dict(map(None,HLT_list,len(HLT_list)*[{
                    1000 : TriggerRule(PS=1, comment='Primary'), 
                    lumi_l1iso_enable_em : TriggerRule(PS=0, comment='Support', rerun=1),
                    }])))

HLT_list=['HLT_e24_medium_iloose_L1EM20VH'] 
egamma_Rules_0p5e34.update(dict(map(None,HLT_list,len(HLT_list)*[{
                    1000 : TriggerRule(PS=1, comment='Primary'), 
                    lumi_l1iso_enable_em : TriggerRule(PS=0, comment='Support', rerun=1),
                    }])))

HLT_list=['HLT_3g15_loose',]
egamma_Rules_0p5e34.update(dict(map(None,HLT_list,len(HLT_list)*[{
                    1000 : TriggerRule(PS=1, comment='Primary'), 
                    1001 : TriggerRule(PS=-1, comment='Disabled'),
                    }])))

# --------------------------------------
# keep some rate as support,0.5 Hz, when prescaled. Ask if ok.

HLT_list=['HLT_g35_loose_g25_loose']
egamma_Rules_0p5e34.update(dict(map(None,HLT_list,len(HLT_list)*[{
                         1000 : TriggerRule(PS=1, comment='Primary Express', ESRate=0.4), 
                         lumi_disable_hlt_2g_loose : TriggerRule(rate=0.5, comment='Support', ESValue=0),
                         }])))

# --------------------------------------
# keep some rate as support,0.5 Hz, when prescaled. Ask if ok.

HLT_list=['HLT_g120_loose',]
egamma_Rules_0p5e34.update(dict(map(None,HLT_list,len(HLT_list)*[{
                      1000 : TriggerRule(PS=1, comment='Primary Express', ESRate=0.2), 
                      lumi_disable_hlt_eg120 : TriggerRule(rate=0.5, comment='Support',rerun=1, ESValue=0),
                      }])))

HLT_list=['HLT_e120_lhloose','HLT_e120_lhloose_nod0',]
egamma_Rules_0p5e34.update(dict(map(None,HLT_list,len(HLT_list)*[{
                      1000 : TriggerRule(PS=1, comment='Primary'), 
                      lumi_disable_hlt_eg120 : TriggerRule(PS=-1, comment='Disabled'),
                      }])))


# --------------------------------------

L1_list=['L1_2EM10VH']
egamma_Rules_0p5e34.update(dict(map(None,L1_list,len(L1_list)*[{
                       1000 : TriggerRule(PS=1, comment='Primary L1'),
                       lumi_disable_l1_2em10vh : TriggerRule(PS=-1, comment='Disabled'),
                       }])))

HLT_list=['HLT_2e12_lhvloose_L12EM10VH', 'HLT_2e12_lhvloose_nod0_L12EM10VH']
egamma_Rules_0p5e34.update(dict(map(None,HLT_list,len(HLT_list)*[{
                       1000 : TriggerRule(PS=1, comment='Primary'),
                       #YU2016NoExpressConfig  1000 : TriggerRule(PS=1, comment='Express', ESRate=0.2), 
                       lumi_disable_l1_2em10vh : TriggerRule(PS=-1, comment='Disabled', ESValue=0),
                       }])))


########################################
# 0.3e34 

egamma_Rules_0p3e34={}

# keep some L1 rate as support,500 Hz, when prescaled. Ask why
L1_list=['L1_EM18VH']
egamma_Rules_0p3e34.update(dict(map(None,L1_list,len(L1_list)*[{
                      1000 : TriggerRule(PS=1, comment='Primary L1'),
                      lumi_disable_l1_em18vh : TriggerRule(rate=500, maxRate=500, comment='Support L1'),
                      }])))

HLT_list=['HLT_e24_lhmedium_iloose_L1EM18VH','HLT_e24_lhmedium_nod0_iloose_L1EM18VH',
          'HLT_e24_lhmedium_ivarloose_L1EM18VH',
          'HLT_e24_lhmedium_nod0_ivarloose_L1EM18VH',
          ]
egamma_Rules_0p3e34.update(dict(map(None,HLT_list,len(HLT_list)*[{
                      1000 : TriggerRule(PS=1, comment='Primary'), 
                      lumi_disable_l1_em18vh : TriggerRule(PS=-1, comment='Disabled'),
                      }])))

HLT_list=['HLT_e24_lhmedium_nod0_ringer_iloose',
          'HLT_e24_lhmedium_nod0_ringer_ivarloose',
          ]
egamma_Rules_0p3e34.update(dict(map(None,HLT_list,len(HLT_list)*[{
                    1000 : TriggerRule(PS=1, comment='Primary, Commission'), 
                    lumi_disable_l1_em18vh : TriggerRule(PS=-1, comment='Disabled'),
                    }])))


# --------------------------------------

# keep some L1 rate as support,500 Hz, when prescaled. Ask why
L1_list=['L1_2EM15']
egamma_Rules_0p3e34.update(dict(map(None,L1_list,len(L1_list)*[{
                     1000 : TriggerRule(PS=1, comment='Primary L1'),
                     lumi_disable_l1_2em15 : TriggerRule(rate=500, maxRate=500, comment='Prescaled'),
                     }])))

HLT_list=['HLT_g35_loose_L1EM15_g25_loose_L1EM15',]
egamma_Rules_0p3e34.update(dict(map(None,HLT_list,len(HLT_list)*[{
                     1000 : TriggerRule(PS=1, comment='Primary'), 
                     lumi_disable_l1_2em15 : TriggerRule(PS=-1, comment='Disabled'),
                     }])))

########################################
# supporting

egamma_Rules_supporting={}

# ----------------------------------------
# di-electron
# what is the point of this? 

HLT_list=['HLT_2e17_loose_L12EM15' ,
          'HLT_2e17_lhloose_L12EM15',
          ##NEW20.11.0.2 'HLT_2e17_lhloose_nod0_L12EM15',
					]
egamma_Rules_supporting.update(dict(map(None,HLT_list,len(HLT_list)*[{
                    1000                  : TriggerRule(PS=1, comment='Primary'),
                    lumi_disable_l1_2em15 : TriggerRule(rate=0.2, comment='Support'),
                    }])))

# ========================================
# non isolated single electron for background

# Exceptionally, e24_(lh)medium_L1EM18VH is kept alive until L1_EM18VH is prescaled. Is this still needed?
HLT_list=['HLT_e24_lhmedium_L1EM18VH']
egamma_Rules_supporting.update(dict(map(None,HLT_list,len(HLT_list)*[{
                      1000 : TriggerRule(PS=1, comment='Primary Express', ESRate=0.2),
                      lumi_disable_l1_em18vh : TriggerRule(rate=0.2, comment='Support Express',ESValue=1),
# is this still needed? expensive at 1.e34 and on.
#                      lumi_l1iso_enable_em : TriggerRule(PS=0, comment='Disabled to save HLT rate',rerun=1,ESValue=0),
                      }])))

HLT_list=['HLT_e24_lhmedium_nod0_L1EM18VH']
egamma_Rules_supporting.update(dict(map(None,HLT_list,len(HLT_list)*[{
                      1000 : TriggerRule(PS=1, comment='Primary'),
                      lumi_disable_l1_em18vh : TriggerRule(rate=0.2, comment='Support'),
                       }])))
# --------------------------------------

# Exceptionally, e24_(lh)medium_L1EM20VH is kept alive until L1_EM20VH is prescaled
# --> tight is also kept alive (was PS=10 before)
# is this needed? would it not be better to keep it PS, low rate, for commissioning only? maybe PS=1 only at start?

HLT_list=['HLT_e24_lhtight_L1EM20VH',]
egamma_Rules_supporting.update(dict(map(None,HLT_list,len(HLT_list)*[{
                      1000 : TriggerRule(PS=1, comment='Primary', ESValue=0),
                      lumi_l1iso_enable_em : TriggerRule(rate=0.2, comment='Support Express', ESValue=1),
                      }])))

HLT_list=['HLT_e24_lhtight_nod0_L1EM20VH',]
egamma_Rules_supporting.update(dict(map(None,HLT_list,len(HLT_list)*[{
                      1000 : TriggerRule(PS=1, comment='Primary'),
                      lumi_l1iso_enable_em : TriggerRule(rate=0.2, comment='Support'),
                      }])))

# ========================================
# other support for electron
# ATR-11040

# --------------------------------------
# Data-driven LH pdf's

# up to 10 Hz, is this really needed?
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
                    1000 : TriggerRule(rate=1.0, comment='Support'), 
                    }])))

# up to 5 Hz, is this really needed?
# all this rerun potentially very CPU expensive. is this needed?

if ElectronRerun:
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
                        1000 : TriggerRule(rate=0.2, comment='Support',rerun=1), 
                        }])))

# 1 Hz SUSY strong-1l request. motivation?
    HLT_list=['HLT_e5_lhvloose']
    egamma_Rules_supporting.update(dict(map(None,HLT_list,len(HLT_list)*[{
                        1000 : TriggerRule(rate=1.0, comment='Support',rerun=1), 
                        }])))

# --------------------------------------
# physics - background estimation
# how much rate does this sum to? is this needed? and why not use the rule with rate=1 Hz instead of these ad-hoc prescales?
# all this rerun potentially very CPU expensive. is this needed?

if ElectronRerun :
    egamma_Rules_supporting.update({
            'HLT_e12_vloose_L1EM10VH'         : {  1000 : TriggerRule(PS=4000, comment='Support, 1 Hz target at 3e33', rerun=1) },
            'HLT_e12_lhvloose_L1EM10VH'       : {  1000 : TriggerRule(PS=2200, comment='Support, 1 Hz target at 3e33', rerun=1) },
            'HLT_e12_lhvloose_nod0_L1EM10VH'  : {  1000 : TriggerRule(PS=2200, comment='Support, 1 Hz target at 3e33', rerun=1) },
            
            'HLT_e15_vloose_L1EM13VH'         : {  1000 : TriggerRule(PS=1350, comment='Support, 1 Hz target at 3e33', rerun=1) },
            'HLT_e15_lhvloose_L1EM13VH'       : {  1000 : TriggerRule(PS=760, comment='Support, 1 Hz target at 3e33', rerun=1) },
            'HLT_e15_lhvloose_nod0_L1EM13VH'  : {  1000 : TriggerRule(PS=760, comment='Support, 1 Hz target at 3e33', rerun=1) },
            
            'HLT_e17_vloose'                  : {  1000 : TriggerRule(PS=820, comment='Support, 1 Hz target at 3e33', rerun=1) },
            'HLT_e17_lhvloose'                : {  1000 : TriggerRule(PS=480, comment='Support, 1 Hz target at 3e33', rerun=1) },
            'HLT_e17_lhvloose_nod0'           : {  1000 : TriggerRule(PS=480, comment='Support, 1 Hz target at 3e33', rerun=1) },
            
            'HLT_e20_vloose'                  : {  1000 : TriggerRule(PS=620, comment='Support, 1 Hz target at 3e33', rerun=1) },
            'HLT_e20_lhvloose'                : {  1000 : TriggerRule(PS=400, comment='Support, 1 Hz target at 3e33', rerun=1) },
            'HLT_e20_lhvloose_nod0'           : {  1000 : TriggerRule(PS=400, comment='Support, 1 Hz target at 3e33', rerun=1) },
            
            'HLT_e40_vloose'                  : {  1000 : TriggerRule(PS=60, comment='Support, 1 Hz target at 3e33', rerun=1) },
            'HLT_e40_lhvloose'                : {  1000 : TriggerRule(PS=45, comment='Support, 1 Hz target at 3e33', rerun=1) },
            'HLT_e40_lhvloose_nod0'           : {  1000 : TriggerRule(PS=45, comment='Support, 1 Hz target at 3e33', rerun=1) },
            
            'HLT_e60_vloose'                  : {  1000 : TriggerRule(PS=15, comment='Support, 1 Hz target at 3e33', rerun=1) },
            'HLT_e60_lhvloose'                : {  1000 : TriggerRule(PS=12, comment='Support, 1 Hz target at 3e33', rerun=1) },
            #
            #
            'HLT_e24_vloose_L1EM18VH'         : {  1000 : TriggerRule(PS=60, comment='Support, 5 Hz target at 3e33', rerun=1) },
            'HLT_e24_lhvloose_L1EM18VH'       : {  1000 : TriggerRule(PS=40, comment='Support, 5 Hz target at 3e33', rerun=1) },
            'HLT_e24_lhvloose_nod0_L1EM18VH'  : {  1000 : TriggerRule(PS=40, comment='Support, 5 Hz target at 3e33', rerun=1) },
            
            'HLT_e24_vloose_L1EM20VH'         : {  1000 : TriggerRule(PS=55, comment='Support, 5 Hz target at 3e33', rerun=1) },
            'HLT_e24_lhvloose_L1EM20VH'       : {  1000 : TriggerRule(PS=35, comment='Support, 5 Hz target at 3e33', rerun=1) },
            'HLT_e24_lhvloose_nod0_L1EM20VH'  : {  1000 : TriggerRule(PS=35, comment='Support, 5 Hz target at 3e33', rerun=1) },
            
            'HLT_e26_vloose_L1EM20VH'         : {  1000 : TriggerRule(PS=190, comment='Support, 5 Hz target at 1.2e34', rerun=1) },
            'HLT_e26_lhvloose_L1EM20VH'       : {  1000 : TriggerRule(PS=125, comment='Support, 5 Hz target at 1.2e34', rerun=1) },
            'HLT_e26_lhvloose_nod0_L1EM20VH'  : {  1000 : TriggerRule(PS=125, comment='Support, 5 Hz target at 1.2e34', rerun=1) },
            })
    
# ========================================
# rerun
# this is potentially very expensive !! is this really needed ?

if ElectronRerun :

    HLT_list=[
        'HLT_e4_etcut',
        'HLT_e9_etcut',
        'HLT_e14_etcut',
        'HLT_e7_medium',                  'HLT_e7_lhmedium',                 'HLT_e7_lhmedium_nod0',    
        'HLT_e9_loose',                   'HLT_e9_lhloose',                  'HLT_e9_lhloose_nod0',
        'HLT_e9_medium',                  'HLT_e9_lhmedium',                 'HLT_e9_lhmedium_nod0',
        'HLT_e12_medium',                 'HLT_e12_lhmedium',                'HLT_e12_lhmedium_nod0',
        'HLT_e17_medium',                 'HLT_e17_lhmedium',                'HLT_e17_lhmedium_nod0',
        'HLT_e17_medium_L1EM15HI',        'HLT_e17_lhmedium_L1EM15HI',       'HLT_e17_lhmedium_nod0_L1EM15HI',
        'HLT_e17_medium_iloose',          'HLT_e17_lhmedium_iloose',         'HLT_e17_lhmedium_nod0_iloose',
        'HLT_e17_medium_iloose_L1EM15HI', 'HLT_e17_lhmedium_iloose_L1EM15HI', 'HLT_e17_lhmedium_nod0_iloose_L1EM15HI',
        'HLT_e17_medium_ivarloose_L1EM15HI', 'HLT_e17_lhmedium_ivarloose_L1EM15HI', 'HLT_e17_lhmedium_nod0_ivarloose_L1EM15HI',
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
        'HLT_e5_loose',                          'HLT_e5_lhloose',                 'HLT_e5_lhloose_nod0',
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
				'HLT_e10_lhvloose_nod0_L1EM7', 
				'HLT_e15_lhvloose_nod0_L1EM7', 
				'HLT_e20_lhvloose_nod0_L1EM12', 
				'HLT_e25_lhvloose_nod0_L1EM15', 
				'HLT_e30_lhvloose_nod0_L1EM15', 
				'HLT_e40_lhvloose_nod0_L1EM15', 
				'HLT_e50_lhvloose_nod0_L1EM15', 
				'HLT_e70_lhvloose_nod0', 
				'HLT_e80_lhvloose_nod0', 
				'HLT_e100_lhvloose_nod0', 
				'HLT_e120_lhvloose_nod0'
        ]
    egamma_Rules_supporting.update(dict(map(None,HLT_list,len(HLT_list)*[{
                        1000 : TriggerRule(PS=0, comment='Support', rerun=1),
                        }])))

# ========================================
# photon prescaled
# 12 Hz just for this. Is this needed?

    HLT_list=[
        'HLT_g15_loose_L1EM7',
        'HLT_g25_loose_L1EM15',
        'HLT_g35_loose_L1EM15',
        'HLT_g40_loose_L1EM15',
        'HLT_g45_loose_L1EM15',
        'HLT_g50_loose_L1EM15',
        ]
    egamma_Rules_supporting.update(dict(map(None,HLT_list,len(HLT_list)*[{
                        1000 : TriggerRule(rate=1.0, comment='Support ATR-13381',rerun=1), 
                        }])))

# another 8 Hz. is this needed?
    HLT_list=[
        'HLT_g60_loose',
        'HLT_g70_loose',
        'HLT_g80_loose',
        'HLT_g100_loose',
        ]
    egamma_Rules_supporting.update(dict(map(None,HLT_list,len(HLT_list)*[{
                        1000 : TriggerRule(rate=1.0, comment='Support ATR-13381',rerun=1),
                        }])))

#another 4 Hz. is this neeeded?
    HLT_list=[
        'HLT_g10_loose',    
        'HLT_g20_loose_L1EM12',
        ]
    egamma_Rules_supporting.update(dict(map(None,HLT_list,len(HLT_list)*[{
                        1000 : TriggerRule(rate=1.0, comment='Support ATR-13381',rerun=1, PTRate=1.0),
                        }])))
# total of 24 Hz for prescaled support after HLT for egamma.

# ========================================
# photon rerun
# all this can be very CPU expensive . Is this really needed?

if ElectronRerun:

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
        'HLT_g22_tight',
        'HLT_g40_tight',
        'HLT_g45_tight',
        
        'HLT_g20_loose_L1EM15',
        
        ]
    egamma_Rules_supporting.update(dict(map(None,HLT_list,len(HLT_list)*[{
                        1000 : TriggerRule(PS=0, comment='Support', rerun=1), 
                        }])))

########################################
# T&P for electron

# --------------------------------------
# Z

egamma_Rules_supporting.update({
    'HLT_e26_lhtight_e15_etcut_Zee'                  : { 1000 : TriggerRule(PS=1, comment='Primary, SM Zee', ESValue=0),
                                                          lumi_l1iso_enable_em : TriggerRule(PS=1, comment='Primary Express, SM Zee', ESRate=0.8) },
    'HLT_e26_tight_e15_etcut_Zee'                    : { 1000 : TriggerRule(PS=1, comment='Primary, SM Zee', ESValue=0) },
    # 
    'HLT_e24_lhtight_L1EM20VH_e15_etcut_Zee'         : { 1000 : TriggerRule(PS=1, comment='Primary Express, SM Zee', ESRate=0.8),
                                                          lumi_l1iso_enable_em : TriggerRule(PS=-1, comment='Disabled', ESValue=0) },
    'HLT_e24_tight_L1EM20VH_e15_etcut_Zee'           : { 1000 : TriggerRule(PS=1, comment='Primary'),
                                                          lumi_l1iso_enable_em : TriggerRule(PS=-1, comment='Disabled') },
		##NEW20.11.0.2 'HLT_e24_lhtight_nod0_L1EM20VH_e15_etcut_Zee' 	 : { 1000 : TriggerRule(PS=1, comment='Primary, ATR-13378'), 
		##NEW20.11.0.2 																											lumi_l1iso_enable_em : TriggerRule(PS=-1, comment='Disabled') },
		##NEW20.11.0.2 'HLT_e26_lhtight_nod0_e15_etcut_Zee' 						 : { 1000 : TriggerRule(PS=1, comment='Primary, ATR-13378') },
    })

# --------------------------------------
# W

L1_list=['L1_EM12_XS20', 'L1_EM15_XS30']
egamma_Rules_supporting.update(dict(map(None,L1_list,len(L1_list)*[{
                    1000 : TriggerRule(rate=500, maxRate=500, comment='L1'),
                    }])))

if ElectronRerun:
    egamma_Rules_supporting.update({
            # 8 Hz for W tag&probe support, all with rerun, is this not CPU expensive?
            #
            # ATR-11746: needs rerun also to W T&P triggers
            #
            # EM10_XS20
            # eff 0.16
            'HLT_e13_etcut_trkcut_j20_perf_xe15_2dphi05'            : {  1000 : TriggerRule(rate=0.5, comment='Support, W tag probe', rerun=1) },
            'HLT_e13_etcut_trkcut_j20_perf_xe15_2dphi05_mt25'       : {  1000 : TriggerRule(rate=0.5, comment='Support, W tag probe', rerun=1) },
            'HLT_e13_etcut_trkcut_j20_perf_xe15_6dphi05'            : {  1000 : TriggerRule(rate=0.5, comment='Support, W tag probe', rerun=1) },
            'HLT_e13_etcut_trkcut_j20_perf_xe15_6dphi05_mt25'       : {  1000 : TriggerRule(rate=0.5, comment='Support, W tag probe', rerun=1) },
            # eff 0.11
            'HLT_e13_etcut_trkcut_xs15_j20_perf_xe15_2dphi05'       : {  1000 : TriggerRule(rate=0.5, comment='Support, W tag probe', rerun=1) },
            'HLT_e13_etcut_trkcut_xs15_j20_perf_xe15_2dphi05_mt25'  : {  1000 : TriggerRule(rate=0.5, comment='Support, W tag probe', rerun=1) },
            'HLT_e13_etcut_trkcut_xs15_j20_perf_xe15_6dphi05'       : {  1000 : TriggerRule(rate=0.5, comment='Support, W tag probe', rerun=1) },
            'HLT_e13_etcut_trkcut_xs15_j20_perf_xe15_6dphi05_mt25'  : {  1000 : TriggerRule(rate=0.5, comment='Support, W tag probe', rerun=1) },
            #
            'HLT_e13_etcut_trkcut_xs15_mt25'                        : {  1000 : TriggerRule(rate=0.5, comment='Support, W tag probe', rerun=1) },
            'HLT_e13_etcut_trkcut_xs15'                             : {  1000 : TriggerRule(rate=0.5, comment='Support, W tag probe', rerun=1) },
            #
            # EM15_XS30    
            # eff 0.05
            'HLT_e18_etcut_trkcut_j20_perf_xe20_6dphi15'            : {  1000 : TriggerRule(rate=0.5, comment='Support, W tag probe', rerun=1) },
            'HLT_e18_etcut_trkcut_j20_perf_xe20_6dphi15_mt35'       : {  1000 : TriggerRule(rate=0.5, comment='Support, W tag probe', rerun=1) },
            # eff 0.03
            'HLT_e18_etcut_trkcut_xs20_j20_perf_xe20_6dphi15'       : {  1000 : TriggerRule(rate=0.5, comment='Support, W tag probe', rerun=1) },
            'HLT_e18_etcut_trkcut_xs20_j20_perf_xe20_6dphi15_mt35'  : {  1000 : TriggerRule(rate=0.5, comment='Support, W tag probe', rerun=1) },
            #
            'HLT_e18_etcut_trkcut_xs20_mt35'                        : {  1000 : TriggerRule(rate=0.5, comment='Support, W tag probe', rerun=1) },
            'HLT_e18_etcut_trkcut_xs20'                             : {  1000 : TriggerRule(rate=0.5, comment='Support, W tag probe', rerun=1) },
            })

    HLT_list=['HLT_e13_etcut_trkcut_L1EM12', 'HLT_e18_etcut_trkcut_L1EM15']
    egamma_Rules_supporting.update(dict(map(None,HLT_list,len(HLT_list)*[{
                        1000 : TriggerRule(rate=0.2, comment='Support', rerun=1),
                        }])))

# is this rerun not very CPU expensive? is this needed?
    HLT_list=['HLT_xs15_L1XS20', 'HLT_xs20_L1XS30']
    egamma_Rules_supporting.update(dict(map(None,HLT_list,len(HLT_list)*[{
                        1000 : TriggerRule(PS=0, comment='Support', rerun=1),
                        }])))

# --------------------------------------
# J/psi 

# PS adjustment by ATR-11886
# rate L1 3000 Hz for Jpsi support

egamma_Rules_supporting.update({
    'L1_2EM3'        : {  1000 : TriggerRule(rate=200, maxRate=200, comment='Support L1') },
    'L1_EM7_2EM3'    : {  1000 : TriggerRule(rate=1200, maxRate=1200, comment='Support L1') },
    'L1_EM12_2EM3'   : {  1000 : TriggerRule(rate=2600, maxRate=2600, comment='Support L1') },
    })

HLT_list=['HLT_e5_lhtight_e4_etcut_Jpsiee']
egamma_Rules_supporting.update(dict(map(None,HLT_list,len(HLT_list)*[{
                    1000 : TriggerRule(PS=1, comment='Support Express, Jpsi ee', ESRate=0.4), 
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
                    1000 : TriggerRule(PS=1, comment='Support, Jpsi ee'), 
                    }])))

#
HLT_list=['HLT_e5_tight_e4_etcut', 'HLT_e5_lhtight_e4_etcut','HLT_e5_lhtight_nod0_e4_etcut']
egamma_Rules_supporting.update(dict(map(None,HLT_list,len(HLT_list)*[{
                    1000 : TriggerRule(PS=1, comment='Support, Jpsi ee'),
                    }])))


########################################
# SM W + nJets

egamma_Rules_supporting.update({
    # why is it done like this and not with rate = xxx command? this gives lots of extra HLT rate possibly. ask ?

    # 12 Hz support
    # 2.0e34
    'HLT_e26_lhvloose_L1EM20VH_3j20noL1'                   : {  1000 : TriggerRule(PS=350, comment='Support, SM W+jets, 1 Hz target at 1.2e34') },
    'HLT_e26_lhvloose_L1EM20VH_4j20noL1'                   : {  1000 : TriggerRule(PS=190, comment='Support, SM W+jets, 1 Hz target at 1.2e34') },
    'HLT_e26_lhvloose_L1EM20VH_5j15noL1'                   : {  1000 : TriggerRule(PS=220, comment='Support, SM W+jets, 1 Hz target at 1.2e34') },
    'HLT_e26_lhvloose_L1EM20VH_6j15noL1'                   : {  1000 : TriggerRule(PS=140, comment='Support, SM W+jets, 1 Hz target at 1.2e34') },
    'HLT_e26_lhvloose_nod0_L1EM20VH_3j20noL1'              : {  1000 : TriggerRule(PS=350, comment='Support, SM W+jets, 1 Hz target at 1.2e34') },
    'HLT_e26_lhvloose_nod0_L1EM20VH_4j20noL1'              : {  1000 : TriggerRule(PS=190, comment='Support, SM W+jets, 1 Hz target at 1.2e34') },
    'HLT_e26_lhvloose_nod0_L1EM20VH_5j15noL1'              : {  1000 : TriggerRule(PS=220, comment='Support, SM W+jets, 1 Hz target at 1.2e34') },
    'HLT_e26_lhvloose_nod0_L1EM20VH_6j15noL1'              : {  1000 : TriggerRule(PS=140, comment='Support, SM W+jets, 1 Hz target at 1.2e34') },
		# PS needs to be computed
    #'HLT_e28_lhvloose_L1EM20VH_3j20noL1'                   : {  1000 : TriggerRule(PS=A, comment='Support, SM W+jets, 1 Hz target at 2e34 (ATR-13381)') },
    #'HLT_e28_lhvloose_L1EM20VH_4j20noL1'                   : {  1000 : TriggerRule(PS=A, comment='Support, SM W+jets, 1 Hz target at 2e34 (ATR-13381)') },
    #'HLT_e28_lhvloose_L1EM20VH_5j15noL1'                   : {  1000 : TriggerRule(PS=A, comment='Support, SM W+jets, 1 Hz target at 2e34 (ATR-13381)') },
    #'HLT_e28_lhvloose_L1EM20VH_6j15noL1'                   : {  1000 : TriggerRule(PS=A, comment='Support, SM W+jets, 1 Hz target at 2e34 (ATR-13381)') },
    #'HLT_e28_lhvloose_nod0_L1EM20VH_3j20noL1'              : {  1000 : TriggerRule(PS=A, comment='Support, SM W+jets, 1 Hz target at 2e34 (ATR-13381)') },
    #'HLT_e28_lhvloose_nod0_L1EM20VH_4j20noL1'              : {  1000 : TriggerRule(PS=A, comment='Support, SM W+jets, 1 Hz target at 2e34 (ATR-13381)') },
    #'HLT_e28_lhvloose_nod0_L1EM20VH_5j15noL1'              : {  1000 : TriggerRule(PS=A, comment='Support, SM W+jets, 1 Hz target at 2e34 (ATR-13381)') },
    #'HLT_e28_lhvloose_nod0_L1EM20VH_6j15noL1'              : {  1000 : TriggerRule(PS=A, comment='Support, SM W+jets, 1 Hz target at 2e34 (ATR-13381)') },

    # 12 Hz support
    # 1.5e34
    'HLT_e24_lhvloose_L1EM20VH_3j20noL1'                   : {  1000 : TriggerRule(PS=100, comment='Support, SM W+jets, 1 Hz target at 3e33'),
                                                               15001 : TriggerRule(PS=-1, comment='Disabled') },
    'HLT_e24_lhvloose_L1EM20VH_4j20noL1'                   : {  1000 : TriggerRule(PS=50, comment='Support, SM W+jets, 1 Hz target at 3e33'),
                                                               15001 : TriggerRule(PS=-1, comment='Disabled') },
    'HLT_e24_lhvloose_L1EM20VH_5j15noL1'                   : {  1000 : TriggerRule(PS=60, comment='Support, SM W+jets, 1 Hz target at 3e33'),
                                                               15001 : TriggerRule(PS=-1, comment='Disabled') },
    'HLT_e24_lhvloose_L1EM20VH_6j15noL1'                   : {  1000 : TriggerRule(PS=35, comment='Support, SM W+jets, 1 Hz target at 3e33'),
                                                               15001 : TriggerRule(PS=-1, comment='Disabled') },
    'HLT_e24_lhvloose_nod0_L1EM20VH_3j20noL1'              : {  1000 : TriggerRule(PS=100, comment='Support, SM W+jets, 1 Hz target at 3e33'),
                                                               15001 : TriggerRule(PS=-1, comment='Disabled') },
    'HLT_e24_lhvloose_nod0_L1EM20VH_4j20noL1'              : {  1000 : TriggerRule(PS=50, comment='Support, SM W+jets, 1 Hz target at 3e33'),
                                                               15001 : TriggerRule(PS=-1, comment='Disabled') },
    'HLT_e24_lhvloose_nod0_L1EM20VH_5j15noL1'              : {  1000 : TriggerRule(PS=60, comment='Support, SM W+jets, 1 Hz target at 3e33'),
                                                               15001 : TriggerRule(PS=-1, comment='Disabled') },
    'HLT_e24_lhvloose_nod0_L1EM20VH_6j15noL1'              : {  1000 : TriggerRule(PS=35, comment='Support, SM W+jets, 1 Hz target at 3e33'),
                                                               15001 : TriggerRule(PS=-1, comment='Disabled') },
    # 12 Hz support
    # 0.3e34
    'HLT_e24_vloose_L1EM18VH_3j20noL1'                     : {  1000 : TriggerRule(PS=170, comment='Support, SM W+jets, 1 Hz target at 3e33'),
                                              lumi_disable_l1_em18vh : TriggerRule(PS=-1, comment='Disabled') },
    'HLT_e24_vloose_L1EM18VH_4j20noL1'                     : {  1000 : TriggerRule(PS=88, comment='Support, SM W+jets, 1 Hz target at 3e33'),
                                              lumi_disable_l1_em18vh : TriggerRule(PS=-1, comment='Disabled') },
    'HLT_e24_vloose_L1EM18VH_5j15noL1'                     : {  1000 : TriggerRule(PS=88, comment='Support, SM W+jets, 1 Hz target at 3e33'),
                                              lumi_disable_l1_em18vh : TriggerRule(PS=-1, comment='Disabled') },
    'HLT_e24_vloose_L1EM18VH_6j15noL1'                     : {  1000 : TriggerRule(PS=64, comment='Support, SM W+jets, 1 Hz target at 3e33'),
                                              lumi_disable_l1_em18vh : TriggerRule(PS=-1, comment='Disabled') },
    'HLT_e24_lhvloose_L1EM18VH_3j20noL1'                   : {  1000 : TriggerRule(PS=110, comment='Support, SM W+jets, 1 Hz target at 3e33', rerun=1),
                                              lumi_disable_l1_em18vh : TriggerRule(PS=-1, comment='Disabled') },
    'HLT_e24_lhvloose_L1EM18VH_4j20noL1'                   : {  1000 : TriggerRule(PS=56, comment='Support, SM W+jets, 1 Hz target at 3e33'),
                                              lumi_disable_l1_em18vh : TriggerRule(PS=-1, comment='Disabled') },
    'HLT_e24_lhvloose_L1EM18VH_5j15noL1'                   : {  1000 : TriggerRule(PS=66, comment='Support, SM W+jets, 1 Hz target at 3e33'),
                                              lumi_disable_l1_em18vh : TriggerRule(PS=-1, comment='Disabled') },
    'HLT_e24_lhvloose_L1EM18VH_6j15noL1'                   : {  1000 : TriggerRule(PS=40, comment='Support, SM W+jets, 1 Hz target at 3e33'),
                                              lumi_disable_l1_em18vh : TriggerRule(PS=-1, comment='Disabled') },
    'HLT_e24_lhvloose_nod0_L1EM18VH_3j20noL1'              : {  1000 : TriggerRule(PS=110, comment='Support, SM W+jets, 1 Hz target at 3e33'),
                                              lumi_disable_l1_em18vh : TriggerRule(PS=-1, comment='Disabled') },
    'HLT_e24_lhvloose_nod0_L1EM18VH_4j20noL1'              : {  1000 : TriggerRule(PS=56, comment='Support, SM W+jets, 1 Hz target at 3e33'),
                                              lumi_disable_l1_em18vh : TriggerRule(PS=-1, comment='Disabled') },
    'HLT_e24_lhvloose_nod0_L1EM18VH_5j15noL1'              : {  1000 : TriggerRule(PS=66, comment='Support, SM W+jets, 1 Hz target at 3e33'),
                                              lumi_disable_l1_em18vh : TriggerRule(PS=-1, comment='Disabled') },
    'HLT_e24_lhvloose_nod0_L1EM18VH_6j15noL1'              : {  1000 : TriggerRule(PS=40, comment='Support, SM W+jets, 1 Hz target at 3e33'),
                                              lumi_disable_l1_em18vh : TriggerRule(PS=-1, comment='Disabled') },
})

########################################
# other rules

egamma_Rules_supporting.update({

    # ----------------------------------------
    # Di-photon
    #
    # for 2g20_tight
    # 7 Hz at 1e34, can we not use rate = 1 command instead ?
    'HLT_2g20_loose'                               : {  1000 : TriggerRule(PS=10, comment='Support') },

    'HLT_2g20_loose_L12EM15'                       : {  1000 : TriggerRule(rate=0.2, comment='Support') },
    'HLT_2g10_loose'                               : {  1000 : TriggerRule(rate=0.2, comment='Support') },
    'HLT_g20_loose_2g15_loose_L12EM13VH'           : {  1000 : TriggerRule(rate=0.2, comment='Support') },

    #
    # what supporting ??? ask
    'HLT_g35_medium_g25_medium_L1EM7_EMPTY'        : {  1000 : TriggerRule(PS=1, comment='Support') },
    'HLT_g35_medium_g25_medium_L1EM7_UNPAIRED_ISO' : {  1000 : TriggerRule(PS=1, comment='Support') },

    ###NEW20.11.0.2 'HLT_2g22_tight_L1EM7_UNPAIRED_ISO' : {  1000 : TriggerRule(PS=1, comment='Support, ATR-13379') },
    ###NEW20.11.0.2 'HLT_2g22_tight_L1EM7_EMPTY' : {  1000 : TriggerRule(PS=1, comment='Support, ATR-13379') },
    ###NEW20.11.0.2 'HLT_2g50_loose_L1EM7_UNPAIRED_ISO' : {  1000 : TriggerRule(PS=1, comment='Support, ATR-13379') },
    ###NEW20.11.0.2 'HLT_2g50_loose_L1EM7_EMPTY' : {  1000 : TriggerRule(PS=1, comment='Support, ATR-13379') },

    # ----------------------------------------
    # idperf
    # rate= 6 Hz
    'HLT_e5_loose_idperf'                          : {  1000 : TriggerRule(rate=0.5, comment='Support') },
    'HLT_e5_lhloose_idperf'                        : {  1000 : TriggerRule(rate=0.5, comment='Support') },
    ###NEW20.11.0.2 'HLT_e5_lhloose_nod0_idperf'                   : {  1000 : TriggerRule(rate=0.5, comment='Support') },
    'HLT_e5_tight_idperf'                          : {  1000 : TriggerRule(rate=0.5, comment='Support') },
    'HLT_e5_lhtight_idperf'                        : {  1000 : TriggerRule(rate=0.5, comment='Support') },
    'HLT_e10_tight_idperf'                         : {  1000 : TriggerRule(rate=0.5, comment='Support') },
    'HLT_e10_lhtight_idperf'                       : {  1000 : TriggerRule(rate=0.5, comment='Support') },
    'HLT_e24_medium_idperf_L1EM20VH'               : {  1000 : TriggerRule(rate=1.5, comment='Support Express', ESRate=0.25) },
    'HLT_e24_lhmedium_idperf_L1EM20VH'             : {  1000 : TriggerRule(rate=1.5, comment='Support Express', ESRate=0.25) },

    # ----------------------------------------
    # perf
    # rate= 2 Hz
    'HLT_g0_perf_L1EM15'    : {  1000 : TriggerRule(rate=1.0, comment='Support') },
    'HLT_e0_perf_L1EM15'    : {  1000 : TriggerRule(rate=1.0, comment='Support') },


})





########################################

egamma_Rules={}

RulesList=[egamma_Rules_2e34, egamma_Rules_1e34, egamma_Rules_0p5e34, egamma_Rules_0p3e34, egamma_Rules_supporting]
if noSupportingTriggers:
    RulesList=[egamma_Rules_2e34, egamma_Rules_1e34, egamma_Rules_0p5e34, egamma_Rules_0p3e34]

for Rules in RulesList:
    for newrule in Rules.keys():
        if egamma_Rules.has_key(newrule):
            print 'FATAL     Physics_pp_v6_rules     Duplicated rule inside physics rule. Cannot be added:',newrule
    egamma_Rules.update(Rules)


#################################################################################################
#
#
# Combined
#
# note: we need to add jet+clean*+xe items
#
#################################################################################################


########################################
# 2e34 

combined_Rules_2e34={}

# for how long do we need to keep cut based too ?

HLT_list=[
    # ATR-9486
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
    # mu + g . ATR-11623
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
                     1000 : TriggerRule(PS=1, comment='Primary'), 
                     }])))

HLT_list=[
    # 2j + xe (+invmass, deta): Higgs invisible, MET+2jet searches, ATR-12151
    'HLT_j40_0eta490_j30_0eta490_deta25_xe80_L1XE70',
    'HLT_2j40_0eta490_invm250_xe80_L1XE70',
    ]
combined_Rules_2e34.update(dict(map(None,HLT_list,len(HLT_list)*[{
               1000 : TriggerRule(PS=1, comment='Primary, Higgs, ATR-12151', PTRate=1.0), 
               }])))

########################################
# 1e34 

combined_Rules_1e34={}

#who asks for this? is it Higgs + photon, Higgs invisible? what about e+xe ?
HLT_list=[
    # e + xe
    'HLT_e60_loose_xe60noL1','HLT_e60_lhloose_xe60noL1','HLT_e60_lhloose_nod0_xe60noL1',
    # g + xe (loose)
    'HLT_g60_loose_xe60noL1',
    ]
combined_Rules_1e34.update(dict(map(None,HLT_list,len(HLT_list)*[{
                      1000 : TriggerRule(PS=1, comment='Primary'), 
#                      10001 : TriggerRule(PS=-1, comment='Disabled'), #SX 090216
                      1001 : TriggerRule(PS=-1, comment='Disabled'),
                      }])))

# who asks for this? 
HLT_list=['HLT_e24_medium_L1EM20VHI_mu8noL1','HLT_e24_lhmedium_L1EM20VHI_mu8noL1','HLT_e24_lhmedium_nod0_L1EM20VHI_mu8noL1',]
combined_Rules_1e34.update(dict(map(None,HLT_list,len(HLT_list)*[{
                       1000 : TriggerRule(PS=1, comment='Primary'), 
                       lumi_disable_l1_em20vhi : TriggerRule(PS=-1, comment='Disabled'),
                       }])))

########################################
# other rules

combined_Rules_other={

    # ----------------------------------------
    # jet + xe

    # ATR-11303, SUSY
    # --> keep both j80/100_xe80 unPS until 1.5e34

    # 1.5e34
#    'HLT_j100_xe80'                      : {  1000 : TriggerRule(PS=1, comment='Primary'), #SX 090216
#                                             15001 : TriggerRule(rate=0.2, comment='Support') },
    'HLT_j100_xe80'                      : {  1000 : TriggerRule(PS=-1, comment='Disabled'),
                                             10001 : TriggerRule(PS=1, comment='Primary') },

#    'HLT_j80_xe80'                       : {  1000 : TriggerRule(PS=1, comment='Primary Express', ESRate=0.2),  #SX 090216
#                                             15001 : TriggerRule(rate=0.2, comment='Support Express', ESValue=1) },


    # ----------------------------------------
    # g + xe (tight)- ATR-12901 Higgs -> photon + dark photon 

    # up to 2e34 w/o PS, after with PS
    'HLT_g45_tight_L1EM22VHI_xe45noL1'   : {  1000 : TriggerRule(PS=1, comment='Primary, ATR-12901'),
#                                              10001 : TriggerRule(PS=5, comment='Primary, ATR-12901') }, #SX 090216
                                              1001 : TriggerRule(PS=5, comment='Primary, ATR-12901') },
    # up to 5e33
    'HLT_g45_tight_xe45noL1'             : {  1000 : TriggerRule(PS=1, comment='Primary, ATR-12901'),
                                              lumi_l1iso_enable_em : TriggerRule(PS=-1, comment='Disabled') },
    # up to 0.3e34
    'HLT_g40_tight_xe40noL1'             : {  1000 : TriggerRule(PS=1, comment='Primary, ATR-12901'),
                                              1001 : TriggerRule(PS=-1, comment='Disabled') },

    # ---------------------
    # hh -> WWbb -> lnu jj bb

    'L1_EM13VH_3J20'      : {  1000 : TriggerRule(PS=1, comment='Primary, ATR-12900') },
    'L1_MU10_3J20'      : {  1000 : TriggerRule(PS=1, comment='Primary, ATR-12900') },
    'HLT_e15_lhtight_iloose_3j20_L1EM13VH_3J20' 				: { 1000 : TriggerRule(PS=1, comment='Primary, ATR-12900') },
		##NEW20.11.0.2 'HLT_e15_lhtight_ivarloose_3j20_L1EM13VH_3J20' 			: { 1000 : TriggerRule(PS=1, comment='Primary, ATR-12900, ATR-13378') },
		##NEW20.11.0.2 'HLT_e15_lhtight_nod0_iloose_3j20_L1EM13VH_3J20' 		: { 1000 : TriggerRule(PS=1, comment='Primary, ATR-12900, ATR-13378') },
		##NEW20.11.0.2 'HLT_e15_lhtight_nod0_ivarloose_3j20_L1EM13VH_3J20' : { 1000 : TriggerRule(PS=1, comment='Primary, ATR-12900, ATR-13378') },
    'HLT_mu14_iloose_3j20_L1MU10_3J20' 		: {  1000 : TriggerRule(PS=1, comment='Primary, ATR-12900') },
    'HLT_mu14_ivarloose_3j20_L1MU10_3J20' : {  1000 : TriggerRule(PS=1, comment='Primary, ATR-12900') },
    # ---------------------
    # VH , H->bb V=W -> mu
    'L1_MU10_2J20'      : {  1000 : TriggerRule(PS=1, comment='Primary, ATR-12900') },
    'HLT_mu20_imedium_L1MU10_2J20' : {  1000 : TriggerRule(PS=1, comment='Primary, ATR-12900') },
    
}

########################################

combined_Rules={}

RulesList=[combined_Rules_2e34, combined_Rules_1e34, combined_Rules_other]
for Rules in RulesList:
    for newrule in Rules.keys():
        if combined_Rules.has_key(newrule):
            print 'FATAL     Physics_pp_v6_rules     Duplicated rule inside physics rule. Cannot be added:',newrule
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

#need to add a g26 or so, when EM22VHI -> EM24VHI, at L>2e34 . ATR-12120
# VBF + 4 j
HLT_list=['HLT_g25_medium_L1EM22VHI_2j35_0eta490_bmv2c2077_2j35_0eta490', 'HLT_g25_medium_L1EM22VHI_j35_0eta490_bmv2c2077_3j35_0eta490_invm700', ]
vbf_Rules_2e34.update(dict(map(None,HLT_list,len(HLT_list)*[{
                      1000 : TriggerRule(PS=1, comment='Primary, VBF H->bb + photon , ATR-12120'), 
                      20001 : TriggerRule(PS=-1, comment='Disabled'),
                      }])))

########################################
# 1e34 

vbf_Rules_1e34={}

# VBF + 5j  - need alternative for higher lumi !!
HLT_list=['HLT_g25_loose_2j40_0eta490_3j25_0eta490_invm700',]
vbf_Rules_1e34.update(dict(map(None,HLT_list,len(HLT_list)*[{
                               1000 : TriggerRule(PS=1, comment='Primary, VBF H +photon, ATR-12120'), 
                               10001 : TriggerRule(PS=-1, comment='Disabled'),
                               }])))


# ----------------------------------------


L1_list=['L1_MU6_J30.0ETA49_2J20.0ETA49',]
vbf_Rules_1e34.update(dict(map(None,L1_list,len(L1_list)*[{
                             1000 : TriggerRule(PS=1, comment='Primary L1'), 
                             lumi_nonl1topo_disable_vbf_mu : TriggerRule(PS=-1, comment='Disabled'),
                             }])))

# 50 Hz at 1e34 !!!! need something better for HLT.
HLT_list=['HLT_mu6_2j40_0eta490_invm1000_L1MU6_J30.0ETA49_2J20.0ETA49',]
vbf_Rules_1e34.update(dict(map(None,HLT_list,len(HLT_list)*[{
                             1000 : TriggerRule(PS=1, comment='Primary Backup for L1Topo, ATR-10979'), 
                             lumi_nonl1topo_disable_vbf_mu : TriggerRule(PS=-1, comment='Disabled'),
                             }])))


# ----------------------------------------

# need alternative here. for what?

#L1_list=['L1_4J20.0ETA49',]
#vbf_Rules_1e34.update(dict(map(None,L1_list,len(L1_list)*[{
#                               1000 : TriggerRule(PS=1, comment='L1'), 
#    lumi_nonl1topo_disable_vbf_bjet : TriggerRule(PS=-1, comment='Disabled'),
#    }])))

#HLT_list=[##YU2016NonExisting 'HLT_2j55_bmedium_L14J20.0ETA49'
#          ]
#vbf_Rules_1e34.update(dict(map(None,HLT_list,len(HLT_list)*[{
#                               1000 : TriggerRule(PS=1, comment=''), 
#    lumi_nonl1topo_disable_vbf_bjet : TriggerRule(PS=-1, comment='Disabled'),
#    }])))

########################################
# 0.75e34 

vbf_Rules_0p75e34={}

#vbf + 4j
HLT_list=['HLT_g25_medium_L1EM22VHI_4j35_0eta490_invm700']
vbf_Rules_0p75e34.update(dict(map(None,HLT_list,len(HLT_list)*[{
                         1000 : TriggerRule(PS=1, comment='Primary'), 
                         1001 : TriggerRule(PS=-1, comment='Disabled'),
                         }])))

########################################
# 0.5e34 

vbf_Rules_0p5e34={}


# vbf + mu, very large rate , can we reduce?
HLT_list=['HLT_mu6_2j40_0eta490_invm800_L1MU6_J30.0ETA49_2J20.0ETA49',]
vbf_Rules_0p5e34.update(dict(map(None,HLT_list,len(HLT_list)*[{
                         1000 : TriggerRule(PS=1, comment='Primary'), 
                         1001 : TriggerRule(PS=-1, comment='Disabled'),
                         }])))

# vbf + 5j
HLT_list=['HLT_g25_loose_L1EM20VH_2j40_0eta490_3j25_0eta490_invm700',]
vbf_Rules_0p5e34.update(dict(map(None,HLT_list,len(HLT_list)*[{
                        1000 : TriggerRule(PS=1, comment='Primary'), 
                        lumi_l1iso_enable_em : TriggerRule(PS=-1, comment='Disabled'),
                        }])))

########################################
# 0.3e34 

vbf_Rules_0p3e34={}

# vbf + mu, very large rate , can we reduce?
HLT_list=['HLT_mu6_2j40_0eta490_invm600_L1MU6_J30.0ETA49_2J20.0ETA49',]
vbf_Rules_0p3e34.update(dict(map(None,HLT_list,len(HLT_list)*[{
                          1000 : TriggerRule(PS=1, comment='Primary'), 
                          1001 : TriggerRule(PS=-1, comment='Disabled'),
                          }])))

HLT_list=['HLT_g20_loose_L1EM18VH_2j40_0eta490_3j25_0eta490_invm700',]
vbf_Rules_0p3e34.update(dict(map(None,HLT_list,len(HLT_list)*[{
                          1000 : TriggerRule(PS=1, comment='Primary'), 
                          lumi_disable_l1_em18vh : TriggerRule(PS=-1, comment='Disabled'),
                          }])))

########################################
# 0.15e34 

vbf_Rules_0p15e34={}

# vbf + mu, very large rate , can we reduce?
HLT_list=['HLT_mu6_2j40_0eta490_invm400_L1MU6_J30.0ETA49_2J20.0ETA49',]
vbf_Rules_0p15e34.update(dict(map(None,HLT_list,len(HLT_list)*[{
                          1000 : TriggerRule(PS=1, comment='Primary'), 
                          1001 : TriggerRule(PS=-1, comment='Disabled'),
                          }])))

########################################
# supporting

vbf_Rules_supporting={}

HLT_list=['HLT_g20_loose_L1EM18VH_2j40_0eta490_3j25_0eta490']
vbf_Rules_supporting.update(dict(map(None,HLT_list,len(HLT_list)*[{
                           1000 : TriggerRule(rate=0.5, comment='Support, ATR-11486'), 
                           lumi_disable_l1_em18vh : TriggerRule(PS=-1, comment='Disabled'),
                           }])))

HLT_list=['HLT_g25_loose_L1EM20VH_4j35_0eta490']
vbf_Rules_supporting.update(dict(map(None,HLT_list,len(HLT_list)*[{
                           1000 : TriggerRule(rate=1., comment='Support, ATR-12120'), 
                           }])))

# ----------------------------------------
# MET + VBF
# 1.8 kHz @ 2e34 for XE55 --> prescaled to 10 Hz
# why ? and why 10 Hz?
vbf_Rules_supporting.update({
     'HLT_2j40_0eta490_invm250_L1XE55'   : {  1000 : TriggerRule(rate=10, comment='Support') },
})

# ----------------------------------------
# b + VBF
# neede donly if 2b trigegrs commented out above become again important
#
#vbf_Rules_supporting.update({
#     'HLT_2j55_bperf_L14J20.0ETA49'      : {  1000 : TriggerRule(rate=1.0, comment='Prescaled') },
#})

########################################

vbf_Rules={}

RulesList=[vbf_Rules_2e34, vbf_Rules_1e34, vbf_Rules_0p75e34, vbf_Rules_0p5e34, vbf_Rules_0p3e34, vbf_Rules_0p15e34, vbf_Rules_supporting] 
if noSupportingTriggers:
    RulesList=[vbf_Rules_2e34, vbf_Rules_1e34, vbf_Rules_0p75e34, vbf_Rules_0p5e34, vbf_Rules_0p3e34, vbf_Rules_0p15e34] 

for Rules in RulesList:
    for newrule in Rules.keys():
        if vbf_Rules.has_key(newrule):
            print 'FATAL     Physics_pp_v6_rules     Duplicated rule inside physics rule. Cannot be added:',newrule
    vbf_Rules.update(Rules)


#################################################################################################
#
#
# Dedicated
#
#
#################################################################################################

dedicated_Rules_supporting = {

    # ========================================
    # Drell-Yan
    
    # supporting
    # PS=10 wrt main
    'HLT_mu6_iloose_mu6_11invm24_noos_novtx'                    : {  1000 : TriggerRule(PS=10, comment='Support'),
                                                          lumi_disable_dy : TriggerRule(PS=-1, comment='Disabled') },
    'HLT_mu6_iloose_mu6_24invm60_noos_novtx'                    : {  1000 : TriggerRule(PS=10, comment='Support'),
                                                          lumi_disable_dy : TriggerRule(PS=-1, comment='Disabled') },
    'HLT_mu4_iloose_mu4_7invm9_noos_novtx'                      : {  1000 : TriggerRule(PS=40, comment='Support'),
                                                          lumi_disable_dy : TriggerRule(PS=-1, comment='Disabled') },
    'HLT_mu4_iloose_mu4_11invm60_noos_novtx'                    : {  1000 : TriggerRule(PS=40, comment='Support'),
                                                          lumi_disable_dy : TriggerRule(PS=-1, comment='Disabled') },

    # ========================================
    # razor

    # supporting
    'HLT_j30_xe10_razor100'                                     : {  1000 : TriggerRule(rate=1.0, comment='Support, razor, ATR-11539/11756') },

    'HLT_j30_xe60_razor100'                                     : {  1000 : TriggerRule(rate=1.0, comment='Support, razor, ATR-11539/11756') },

    # ========================================
    # LLP

    # supporting (background, rerun)
    'HLT_mu20_msonly'                                           : {  1000 : TriggerRule(rate=0.2, comment='Support, LLP', rerun=1) },
 
    # ========================================
    # LFV tau->mu gamma

    # supporting 
    'HLT_g10_etcut_L1EM8I_mu10_iloose_taumass'                   : {  1000 : TriggerRule(rate=1.0, comment='Support') },
    'HLT_g20_etcut_L1EM15I_mu4_iloose_taumass'                   : {  1000 : TriggerRule(rate=1.0, comment='Support') },


    'HLT_g10_etcut_L1EM8I_mu10_taumass'                          : {  1000 : TriggerRule(rate=1.0, comment='Support') },
    'HLT_g20_etcut_L1EM15I_mu4_taumass'                          : {  1000 : TriggerRule(rate=1.0, comment='Support') },

    # ========================================
    # h->phi gamma -> KK gamma

    # supporting
    'HLT_tau25_dikaon_tracktwo'                                 : {  1000 : TriggerRule(PS=0, comment='Support, ATR-11531',rerun=1) },
    'HLT_tau25_dikaontight_tracktwo'                            : {  1000 : TriggerRule(PS=0, comment='Support, ATR-11531',rerun=1) },
    'HLT_tau35_dikaon_tracktwo_L1TAU12'                         : {  1000 : TriggerRule(PS=0, comment='Support, ATR-11531',rerun=1) },
    'HLT_tau35_dikaontight_tracktwo_L1TAU12'                    : {  1000 : TriggerRule(PS=0, comment='Support, ATR-11531',rerun=1) },

    }

dedicated_Rules_2e34 = {

    # ========================================
    # Drell-Yan
    #
    'HLT_mu6_iloose_mu6_24invm60_noos_L1DY-BOX-2MU6'            : {  1000 : TriggerRule(PS=1, comment='Primary, DY, ATR-11516') },

    'HLT_mu6_iloose_mu6_11invm24_noos'                          : {  1000 : TriggerRule(PS=1, comment='Primary, DY, ATR-11516'),
                                                          lumi_disable_dy : TriggerRule(PS=-1, comment='Disabled') },
    'HLT_mu6_iloose_mu6_24invm60_noos'                          : {  1000 : TriggerRule(PS=1, comment='Primary, DY, ATR-11516'),
                                                          lumi_disable_dy : TriggerRule(PS=-1, comment='Disabled') },
    'HLT_mu4_iloose_mu4_7invm9_noos'                            : {  1000 : TriggerRule(PS=4, comment='Primary, DY, ATR-11516'),
                                                          lumi_disable_dy : TriggerRule(PS=-1, comment='Disabled') },
    'HLT_mu4_iloose_mu4_11invm60_noos'                          : {  1000 : TriggerRule(PS=4, comment='Primary, DY, ATR-11516'),
                                                          lumi_disable_dy : TriggerRule(PS=-1, comment='Disabled') },

    'HLT_2mu6_10invm30_pt2_z10'                                 : {  1000 : TriggerRule(PS=1, comment='Primary, DY, ATR-11516'),
                                                                     lumi_disable_l1_2mu6 : TriggerRule(PS=-1, comment='Disabled') },

   

    # ========================================
    # razor

    'L1_2J15_XE55'                                              : {  1000 : TriggerRule(PS=1, comment='Primary L1,razor, ATR-12890') },

    #cannot have 170 unprescaled at 2e34, it's 50 Hz, verify proposed prescale set is ok
    'HLT_j30_xe60_razor170'                                     : {  1000 : TriggerRule(PS=1, comment='Primary, razor, ATR-12890'),
                                                                     10001 : TriggerRule(PS=-1, comment='Disabled') },
    
    'HLT_j30_xe60_razor185'                                     : {  1000 : TriggerRule(PS=1, comment='Primary, razor, ATR-12890'),
                                                                     15001 : TriggerRule(PS=-1, comment='Disabled') },
    
    'HLT_j30_xe60_razor195'                                     : {  1000 : TriggerRule(PS=1, comment='Primary, razor, ATR-12890') },

    # xe10 version: up to 1e34 (for razor185)
    'HLT_j30_xe10_razor170'                                     : {  1000 : TriggerRule(PS=1, comment='Primary, razor, ATR-12890'),
                                                                     1001 : TriggerRule(rate=1, comment='Support 1Hz, ATR-13382') },
    'HLT_j30_xe10_razor185'                                     : {  1000 : TriggerRule(PS=1, comment='Primary, razor, ATR-12890'),
                                                                     1001 : TriggerRule(rate=1, comment='Support 1Hz, ATR-13382') },
    'HLT_j30_xe10_razor195'                                     : {  1000 : TriggerRule(PS=1, comment='Primary, razor, ATR-12890'),
                                                                     1001 : TriggerRule(rate=1, comment='Support 1Hz, ATR-13382') },

    # ========================================
    # LLP

    # ----------------------------------------
    'HLT_j30_muvtx'                                             : {  1000 : TriggerRule(PS=1, comment='Primary, LLP, ATR-10933') },
    # supporting
    'HLT_j30_muvtx_noiso'                                       : {  1000 : TriggerRule(PS=1, comment='Support, LLP, ATR-10933 should not be prescaled') },
    'HLT_j30_muvtx_L1MU4_EMPTY'                                 : {  1000 : TriggerRule(PS=1, comment='Support, LLP, ATR-10933 should not be prescaled') },
    'HLT_j30_muvtx_L1MU4_UNPAIRED_ISO'                          : {  1000 : TriggerRule(PS=1, comment='Support, LLP, ATR-10933 should not be prescaled') },
    ##NEW20.11.0.2 'HLT_j30_muvtx_noiso_L1MU4_EMPTY'                           : {  1000 : TriggerRule(PS=1, comment='Support, LLP, ATR-10933 should not be prescaled, ATR-13379') },
    ##NEW20.11.0.2 'HLT_j30_muvtx_noiso_L1MU4_UNPAIRED_ISO'                    : {  1000 : TriggerRule(PS=1, comment='Support, LLP, ATR-10933 should not be prescaled, ATR-13379') },

    # ----------------------------------------
    #
    'HLT_g15_loose_2mu10_msonly'                                : {  1000 : TriggerRule(PS=1, comment='Primary, LLP, ATR-10933') },
    # supporting
    'HLT_g15_loose_2mu10_msonly_L1MU4_EMPTY'                    : {  1000 : TriggerRule(PS=1, comment='Support, LLP, ATR-10933 should not be prescaled') },
    'HLT_g15_loose_2mu10_msonly_L1MU4_UNPAIRED_ISO'             : {  1000 : TriggerRule(PS=1, comment='Support, LLP, ATR-10933 should not be prescaled') },

    # ----------------------------------------
    #
#    'HLT_3mu6_msonly'                                           : {  1000 : TriggerRule(PS=1, comment='Primary, LLP, ATR-10933') }, # this gets defined with muon triggers, above    
    # supporting
    'HLT_3mu6_msonly_L1MU4_EMPTY'                               : {  1000 : TriggerRule(PS=1, comment='Support, LLP, ATR-10933 should not be prescaled') },
    'HLT_3mu6_msonly_L1MU4_UNPAIRED_ISO'                        : {  1000 : TriggerRule(PS=1, comment='Support, LLP, ATR-10933 should not be prescaled') },

    # ----------------------------------------
    # 
    'HLT_j30_jes_PS_llp_L1TAU60'                                : {  1000 : TriggerRule(PS=1, comment='Primary, LLP, ATR-10933') },
    'HLT_j30_jes_cleanLLP_PS_llp_L1TAU60'                       : {  1000 : TriggerRule(PS=1, comment='Primary, LLP, ATR-10933') },
    'HLT_j30_jes_cleanLLP_PS_llp_L1TAU40'                       : {  1000 : TriggerRule(PS=-1, comment='Primary, LLP, ATR-10933'),
                                                                     lumi_disable_l1_tau40 : TriggerRule(PS=-1, comment='Disabled') },
    'HLT_j30_jes_cleanLLP_PS_llp_L1TAU30'                       : {  1000 : TriggerRule(PS=-1, comment='Primary, LLP, ATR-10933'),
                                                                     lumi_disable_l1_tau30 : TriggerRule(PS=-1, comment='Disabled') },

    'HLT_j30_jes_PS_llp_noiso_L1TAU60'                          : {  1000 : TriggerRule(PS=1, comment='Primary, LLP, ATR-10933') },
    'HLT_j30_jes_cleanLLP_PS_llp_noiso_L1TAU60'                 : {  1000 : TriggerRule(PS=1, comment='Primary, LLP, ATR-10933') },
    # supporting
    'HLT_j30_jes_cleanLLP_PS_llp_L1TAU8_EMPTY'                  : {  1000 : TriggerRule(PS=1, comment='Support, LLP, ATR-10933 should not be prescaled') }, 
    'HLT_j30_jes_cleanLLP_PS_llp_L1TAU8_UNPAIRED_ISO'           : {  1000 : TriggerRule(PS=1, comment='Support, LLP, ATR-10933 should not be prescaled') }, 
 
    ##NEW20.11.0.2 'HLT_j30_jes_cleanLLP_PS_llp_noiso_L1TAU8_EMPTY'                  : {  1000 : TriggerRule(PS=1, comment='Support, LLP, ATR-13379') }, 
    ##NEW20.11.0.2 'HLT_j30_jes_cleanLLP_PS_llp_noiso_L1TAU8_UNPAIRED_ISO'           : {  1000 : TriggerRule(PS=1, comment='Support, LLP, ATR-13379') }, 

    # -------------------------------------------
    # Lepton-Jet - HNL, LLP search (correct??)
    # high rate... need to review carefully. 
    # we need an alternative for higher lumis than 8e33 !!!
    
    'HLT_mu20_msonly_mu6noL1_msonly_nscan05'                    : {  1000 : TriggerRule(PS=1, comment='Primary, LLP, ATR-10933'),
                                                                     lumi_disable_nscan05 : TriggerRule(PS=-1, comment='Disabled') },

    # supporting
    'HLT_mu20_msonly_mu6noL1_msonly_nscan05_L1MU4_EMPTY'        : {  1000 : TriggerRule(PS=1, comment='Support, LLP, ATR-10933 should not be prescaled'),
                                                                     lumi_disable_nscan05 : TriggerRule(PS=-1, comment='Disabled') },
    'HLT_mu20_msonly_mu6noL1_msonly_nscan05_L1MU4_UNPAIRED_ISO' : {  1000 : TriggerRule(PS=1, comment='Support, LLP, ATR-10933 should not be prescaled'),
                                                                     lumi_disable_nscan05 : TriggerRule(PS=-1, comment='Disabled') },


    
    # ========================================
    # LFV tau->3mu 

    'L1_MU11_2MU6'                                          : {  1000 : TriggerRule(PS=1, comment='Primary, LFV tau 3mu') }, 

    'HLT_mu20_l2idonly_mu6noL1_nscan03'                     : {  1000 : TriggerRule(PS=1, comment='Primary, LFV tau 3mu') },            
    'HLT_mu11_nomucomb_2mu4noL1_nscan03_L1MU11_2MU6'        : {  1000 : TriggerRule(PS=1, comment='Primary, LFV tau 3mu') },            
    'HLT_mu11_nomucomb_mu6noL1_nscan03_L1MU11_2MU6_bTau'    : {  1000 : TriggerRule(PS=1, comment='Primary, LFV tau 3mu') },            
    'HLT_2mu10_nomucomb'                                    : {  1000 : TriggerRule(PS=1, comment='Primary, LFV tau 3mu'),
                                                                 lumi_disable_hlt_2mu10: TriggerRule(PS=-1, comment='Disabled') },  #SX 090216          
#    'HLT_mu20_2mu4noL1'                                     : {  1000 : TriggerRule(PS=1, comment='Primary, LFV tau 3mu') },   # this get set in rules with muons above         
#    'HLT_mu22_mu8noL1'                                      : {  1000 : TriggerRule(PS=1, comment='Primary, LFV tau 3mu') },   # this get set in rules with muons above  

    'L1_2MU6_3MU4'                                          : {  1000 : TriggerRule(PS=1, comment='Primary, LFV tau 3mu'),
                                                                 lumi_disable_l1_2mu6_3mu4 : TriggerRule(PS=-1, comment='Disabled')},

    'HLT_2mu6_nomucomb_mu4_nomucomb_L12MU6_3MU4'            : {  1000 : TriggerRule(PS=1, comment='Primary, LFV tau 3mu') },
   

    
    

    # ========================================
    # LFV tau->emu 

    'L1_EM8I_MU10'                                               : {  1000 : TriggerRule(PS=1, comment='Primary, LFV tau mugamma'),
                                                                      10001 : TriggerRule(rate=500, maxRate=500, comment='Support') },

    'L1_EM15I_MU4'                                               : {  1000 : TriggerRule(PS=1, comment='Primary, LFV tau mugamma'),
                                                                      lumi_disable_taumass : TriggerRule(rate=500, maxRate=500, comment='Support') },

    'HLT_g10_loose_L1EM8I_mu10_iloose_taumass'                   : {  1000 : TriggerRule(PS=1, comment='Primary, LFV tau mugamma') },

    'HLT_g20_loose_L1EM15I_mu4_iloose_taumass'                   : {  1000 : TriggerRule(PS=1, comment='Primary, LFV tau mugamma') },




    # ========================================
    # HIP
    # we are missing the version with 24VHI start. remember to add.
    
    'HLT_g0_hiptrt_L1EM22VHI'                                   : {  1000 : TriggerRule(PS=1, comment='Primary, monopole, ATR-9259') },
    'HLT_g0_hiptrt_L1EM20VHI'                                   : {  1000 : TriggerRule(PS=1, comment='Primary, monopole, ATR-9259'),
                                                                     lumi_disable_l1_em20vhi : TriggerRule(PS=-1, comment='Disabled') },
    'HLT_g0_hiptrt_L1EM20VH'                                    : {  1000 : TriggerRule(PS=1, comment='Primary, monopole, ATR-9259'),
                                                                     lumi_l1iso_enable_em : TriggerRule(PS=-1, comment='Disabled') },
    'HLT_g0_hiptrt_L1EM18VH'                                    : {  1000 : TriggerRule(PS=1, comment='Primary, monopole, ATR-9259'),
                                                                     lumi_disable_l1_em18vh : TriggerRule(PS=-1, comment='Disabled') },

    
    # ========================================
    # stopped gluino

    'HLT_j55_0eta240_xe50_L1J30_EMPTY'                          : {  1000 : TriggerRule(PS=1, comment='Primary, stopped gluino, ATR-11193') },
    # don't we need also HLT_j55_0eta240_xe50_L1J12_UNPAIREDISO  and _NONISO ?
    
    # ========================================
    # h->phi gamma -> KK gamma

    # we need to add alternative when EM22VHI gets PS out
    'HLT_g35_medium_tau25_dikaontight_tracktwo_L1TAU12'         : {  1000 : TriggerRule(PS=1, comment='Primary, Higgs, ATR-11531') },
    'HLT_g35_medium_tau35_dikaontight_tracktwo_L1TAU12'         : {  1000 : TriggerRule(PS=1, comment='Primary, Higgs, ATR-11531') },

    'HLT_g35_medium_tau25_dikaon_tracktwo_L1TAU12'              : {  1000 : TriggerRule(PS=1, comment='Primary, Higgs, ATR-11531'),
                                                                     15001 : TriggerRule(PS=-1, comment='Disabled') },
    'HLT_g35_medium_tau35_dikaon_tracktwo_L1TAU12'              : {  1000 : TriggerRule(PS=1, comment='Primary, Higgs, ATR-11531'),
                                                                     20001 : TriggerRule(PS=-1, comment='Disabled') },
}

dedicated_Rules={}

RulesList=[dedicated_Rules_2e34, dedicated_Rules_supporting] 
if noSupportingTriggers:
    RulesList=[dedicated_Rules_2e34] 

for Rules in RulesList:
    for newrule in Rules.keys():
        if dedicated_Rules.has_key(newrule):
            print 'FATAL     Physics_pp_v6_rules     Duplicated rule inside physics rule. Cannot be added:',newrule
    dedicated_Rules.update(Rules)

#################################################################################################
#
#
# Data scouting
#
#
#################################################################################################


datascouting_Rules = {
    'HLT_j0_perf_ds1_L1J100'               : {  1000 : TriggerRule(PS=-1, comment='Disabled'), 
                                                lumi_disable_l1_j75 : TriggerRule(PS=1, comment='Primary, DS, Enabled after J75 is prescaled') },
    'HLT_j0_perf_ds1_L1J75'                : {  1000 : TriggerRule(PS=1, comment='Primary, DS'), 
                                                lumi_disable_l1_j75 : TriggerRule(PS=-1, comment='Disabled') },
    }
# we need to insert here the new DS like HLT_j0_0i1c400m600TLA

#################################################################################################
#
#
# monitoring
#
#
#################################################################################################

import Monitoring_pp_v6_rules

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
    'L1_LAR-EM':         { 1 : TriggerRule(PS=10, comment='LAREM for testing, request max rate 0.5 Hz')},
    'L1_MJJ-100':        { 1 : TriggerRule(PS=25200, comment='L1Topo commissioning, max rate 0.5 Hz')}  ,#  18.4 kHz
    'L1_MJJ-200':        { 1 : TriggerRule(PS=15332, comment='L1Topo commissioning, max rate 0.5 Hz')}   ,# 7.7 kHz
    'L1_MJJ-300':        { 1 : TriggerRule(PS=8400, comment='L1Topo commissioning, max rate 0.5 Hz')}   ,# 4.2 kHz
    'L1_MJJ-400':        { 1 : TriggerRule(PS=5240, comment='L1Topo commissioning, max rate 0.5 Hz')}   ,# 2.6 kHz
    'L1_MJJ-700':        { 1 : TriggerRule(PS=1948, comment='L1Topo commissioning, max rate 0.5 Hz')} ,#   974 Hz
    'L1_MJJ-800':        { 1 : TriggerRule(PS=1362, comment='L1Topo commissioning, max rate 0.5 Hz')} ,#   681 Hz
    'L1_MJJ-900':        { 1 : TriggerRule(PS=1070, comment='L1Topo commissioning, max rate 0.5 Hz')} ,#   535 Hz
    'L1_DPHI-Js2XE50':   { 1 : TriggerRule(PS=188, comment='L1Topo commissioning, max rate 0.5 Hz')} ,#   94 Hz
    'L1_JPSI-1M5':       { 1 : TriggerRule(PS=1848000, comment='L1Topo commissioning, max rate 0.5 Hz')},#    924 kHz
    'L1_JPSI-1M5-EM7':   { 1 : TriggerRule(PS=186000, comment='L1Topo commissioning, max rate 0.5 Hz')} ,#   93 kHz
    'L1_JPSI-1M5-EM12':  { 1 : TriggerRule(PS=68000, comment='L1Topo commissioning, max rate 0.5 Hz')}  ,#   34 kHz
    'L1_W-10DPHI-EMXE-0':{ 1 : TriggerRule(PS=188000, comment='L1Topo commissioning, max rate 0.5 Hz')}  ,#  94 kHz
    'L1_W-15DPHI-EMXE-0':{ 1 : TriggerRule(PS=242000, comment='L1Topo commissioning, max rate 0.5 Hz')} ,#   121 kHz
    'L1_EM12_W-MT25'	:{ 1: TriggerRule(PS=124000 ,comment="L1Topo commissioning, max rate 0.5 Hz")}  ,#6.20E+04
    'L1_EM12_W-MT30'        :{ 1: TriggerRule(PS=94000  ,comment="L1Topo commissioning, max rate 0.5 Hz")}  ,#4.70E+04
    'L1_EM15_W-MT35' 	:{ 1: TriggerRule(PS=37584  ,comment="L1Topo commissioning, max rate 0.5 Hz")}  ,#18792	
    'L1_DPHI-J20s2XE50'	:{ 1: TriggerRule(PS=24   ,comment="L1Topo commissioning, max rate 0.5 Hz")}  ,#6.5	
    'L1_W-05RO-XEEMHT'	:{ 1: TriggerRule(PS=506914 ,comment="L1Topo commissioning, max rate 0.5 Hz")}  ,#130568	
    'L1_W-05DPHI-JXE-0'	:{ 1: TriggerRule(PS=132288 ,comment="L1Topo commissioning, max rate 0.5 Hz")}  ,#66144	
    'L1_W-10DPHI-JXE-0'	:{ 1: TriggerRule(PS=87656  ,comment="L1Topo commissioning, max rate 0.5 Hz")}  ,#43828	
    'L1_W-15DPHI-JXE-0'	:{ 1: TriggerRule(PS=58672  ,comment="L1Topo commissioning, max rate 0.5 Hz")}  ,#29336	
    'L1_DPHI-CJ20XE50'	:{ 1: TriggerRule(PS=21.6   ,comment="L1Topo commissioning, max rate 0.5 Hz")}  ,#10.8	
    'L1_W-05RO-XEHT-0'	:{ 1: TriggerRule(PS=158954 ,comment="L1Topo commissioning, max rate 0.5 Hz")}  ,#79477	
    'L1_W-90RO2-XEHT-0'	:{ 1: TriggerRule(PS=123558 ,comment="L1Topo commissioning, max rate 0.5 Hz")}  ,#61779	
    'L1_W-250RO2-XEHT-0'	:{ 1: TriggerRule(PS=33746  ,comment="L1Topo commissioning, max rate 0.5 Hz")}  ,#16873	
      
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

#grep "has no rules and will be disabled" a.log | awk -F"--> " '{print $2}' | sed "s/ has no rules and will be disabled//g" | sort | awk '{printf("\"%s\", ",$1)}'

HLT_list=[]
if forRateEstim:
    HLT_list=["HLT_alfacalib", "HLT_alfacalib_L1ALFA_ANY", "HLT_beamspot_activeTE_trkfast_pebTRT", "HLT_beamspot_allTE_trkfast_pebTRT", "HLT_beamspot_trkFS_trkfast", "HLT_conej140_320eta490_larpebj", "HLT_conej165_larpebj", "HLT_conej40_larpebj", "HLT_conej75_320eta490_larpebj", "HLT_costmonitor", "HLT_cscmon_L1All", "HLT_g12_loose_larpeb", "HLT_g20_loose_larpeb_L1EM15", "HLT_g3_loose_larpeb", "HLT_g40_loose_larpeb", "HLT_g60_loose_larpeb", "HLT_g80_loose_larpeb", "HLT_id_cosmicid_L1MU11_EMPTY", "HLT_id_cosmicid_L1MU4_EMPTY", "HLT_idcalib_trk16_central", "HLT_idcalib_trk16_fwd", "HLT_idcalib_trk29_central", "HLT_idcalib_trk29_fwd", "HLT_idcalib_trk9_central", "HLT_idcalib_trk9_fwd", "HLT_j0_perf_ds1_L1J100", "HLT_j0_perf_ds1_L1J75", "HLT_j40_L1ZB", "HLT_larcalib_L1EM3_EMPTY", "HLT_larcalib_L1EM7_EMPTY", "HLT_larcalib_L1J12_EMPTY", "HLT_larcalib_L1J3031ETA49_EMPTY", "HLT_larcalib_L1TAU8_EMPTY", "HLT_larhec_L1EM7_FIRSTEMPTY", "HLT_larhec_L1J12_FIRSTEMPTY", "HLT_larhec_L1J30_FIRSTEMPTY", "HLT_larhec_L1TAU8_FIRSTEMPTY", "HLT_larnoiseburst_L1J100", "HLT_larnoiseburst_L1J40_XE50", "HLT_larnoiseburst_L1J50", "HLT_larnoiseburst_L1J75", "HLT_larnoiseburst_L1J75_XE40", "HLT_larnoiseburst_L1XE35", "HLT_larnoiseburst_L1XE50", "HLT_larnoiseburst_L1XE80", "HLT_larnoiseburst_rerun", "HLT_larps_L1EM3_EMPTY", "HLT_larps_L1EM7_EMPTY", "HLT_larps_L1EM7_FIRSTEMPTY", "HLT_larps_L1J12_EMPTY", "HLT_larps_L1J12_FIRSTEMPTY", "HLT_larps_L1J3031ETA49_EMPTY", "HLT_larps_L1J30_EMPTY", "HLT_larps_L1J30_FIRSTEMPTY", "HLT_larps_L1TAU8_EMPTY", "HLT_lumipeb_L1RD0_ABORTGAPNOTCALIB", "HLT_lumipeb_L1RD0_EMPTY", "HLT_lumipeb_L1RD0_FILLED", "HLT_lumipeb_L1RD0_UNPAIRED_ISO", "HLT_mu0_muoncalib", "HLT_mu0_muoncalib_L1MU15", "HLT_mu0_muoncalib_L1MU4_EMPTY", "HLT_mu0_muoncalib_ds3", "HLT_mu0_muoncalib_ds3_L1MU15", "HLT_mu0_muoncalib_ds3_L1MU4_EMPTY", "HLT_robrequest", "HLT_robrequest_L1RD0_EMPTY", "HLT_satu20em_l1satmon_L1EM20VH_FIRSTEMPTY", "HLT_satu20em_l1satmon_L1EM22VHI_FIRSTEMPTY", "HLT_satu20em_l1satmon_L1J100", "HLT_satu20em_l1satmon_L1J100.31ETA49", "HLT_satu20em_l1satmon_L1J100.31ETA49_FIRSTEMPTY", "HLT_satu20em_l1satmon_L1J100_FIRSTEMPTY", "HLT_sct_noise", "HLT_tilecalib_laser", "HLT_timeburner", 
        ]
    disabled_Rules.update(dict(map(None,HLT_list,len(HLT_list)*[{
    1000 : TriggerRule(PS=-1, comment='Disabled Monitoring in RateEstim'), 
    }])))

    
HLT_list=[]
if noSupportingTriggers:
    HLT_list=[
        "HLT_2e12_lhloose_HLTCalo_L12EM10VH", "HLT_2e12_lhloose_L12EM10VH", "HLT_2e12_lhloose_cutd0dphideta_L12EM10VH", "HLT_2e12_lhloose_nod0_L12EM10VH", "HLT_2e12_lhloose_nodeta_L12EM10VH", "HLT_2e12_lhloose_nodphires_L12EM10VH", "HLT_2e12_loose_L12EM10VH", "HLT_2e12_vloose_L12EM10VH", "HLT_2e15_lhloose_HLTCalo_L12EM13VH", "HLT_2e15_lhloose_L12EM13VH", "HLT_2e15_lhloose_cutd0dphideta_L12EM13VH", "HLT_2e15_lhloose_nod0_L12EM13VH", "HLT_2e15_loose_L12EM13VH", "HLT_2e15_vloose_L12EM13VH", "HLT_2e17_lhloose_2j35_boffperf", "HLT_2e17_lhloose_2j35_boffperf_split", "HLT_2e17_lhloose_2j35_bperf", "HLT_2e17_lhloose_2j35_bperf_split", "HLT_2e17_lhloose_HLTCalo", "HLT_2e17_lhloose_L12EM15", "HLT_2e17_lhloose_nod0", "HLT_2e17_loose", "HLT_2e17_loose_2j35_boffperf", "HLT_2e17_loose_2j35_boffperf_split", "HLT_2e17_loose_2j35_bperf", "HLT_2e17_loose_2j35_bperf_split", "HLT_2e17_loose_L12EM15", "HLT_2e17_vloose", "HLT_2g10_loose", "HLT_2g20_loose", "HLT_2g20_loose_L12EM15", "HLT_2g25_tight", "HLT_2j10", "HLT_2j10_320eta490", "HLT_2j15", "HLT_2j15_320eta490", "HLT_2j20", "HLT_2j20_320eta490", "HLT_2j25", "HLT_2j25_320eta490", "HLT_2j25_320eta490_L1TE5", "HLT_2j30_0eta490_deta25_xe80_L1DPHI-AJ20s2XE50", "HLT_2j30_0eta490_deta35_xe60_L1DPHI-AJ20s2XE50", "HLT_2j30_0eta490_deta35_xe80_tc_lcw_L1DPHI-AJ20s2XE50", "HLT_2j30_bmv2c2085_split_L12J15_XE55", "HLT_2j30_boffperf_L12J15_XE55", "HLT_2j30_boffperf_split_L12J15_XE55", "HLT_2j35_320eta490", "HLT_2j35_bmv2c2070_2j35_L13J25.0ETA23", "HLT_2j35_boffperf_2j35_L13J25.0ETA23", "HLT_2j35_boffperf_2j35_L14J15.0ETA25", "HLT_2j35_bperf_2j35_L13J25.0ETA23", "HLT_2j35_bperf_2j35_L14J15.0ETA25", "HLT_2j35_bperf_split_2j35_L13J25.0ETA23", "HLT_2j35_bperf_split_2j35_L14J15.0ETA25", "HLT_2j35_btight_split_2j35_L14J15.0ETA25", "HLT_2j40_0eta490_invm250", "HLT_2j40_0eta490_invm250_L1XE55", "HLT_2j40_0eta490_invm250_xe80", "HLT_2j45_bmedium_split_2j45_L14J15.0ETA25", "HLT_2j45_boffperf_2j45", "HLT_2j45_boffperf_2j45_L13J25.0ETA23", "HLT_2j45_boffperf_2j45_L14J15.0ETA25", "HLT_2j45_boffperf_split_2j45", "HLT_2j45_boffperf_split_2j45_L13J25.0ETA23", "HLT_2j45_bperf_2j45", "HLT_2j45_bperf_2j45_L13J25.0ETA23", "HLT_2j45_bperf_2j45_L14J15.0ETA25", "HLT_2j45_bperf_split_2j45", "HLT_2j45_bperf_split_2j45_L13J25.0ETA23", "HLT_2j45_bperf_split_2j45_L14J15.0ETA25", "HLT_2j45_btight_split_2j45_L13J25.0ETA23", "HLT_2j55_bmedium_ht300_L14J20", "HLT_2j55_bmedium_split_2j55_L13J25.0ETA23", "HLT_2j55_bmv2c2070_ht300_L14J20", "HLT_2j55_bmv2c2070_split_ht300_L14J20", "HLT_2j55_bmv2c2077_split_L13J20_4J20.0ETA49_MJJ-400", "HLT_2j55_bmv2c2077_split_L13J20_4J20.0ETA49_MJJ-700", "HLT_2j55_bmv2c2077_split_L13J20_4J20.0ETA49_MJJ-800", "HLT_2j55_bmv2c2077_split_L13J20_4J20.0ETA49_MJJ-900", "HLT_2j55_bmv2c2077_split_L1J30_2J20_4J20.0ETA49_MJJ-400", "HLT_2j55_bmv2c2077_split_L1J30_2J20_4J20.0ETA49_MJJ-700", "HLT_2j55_bmv2c2077_split_L1J30_2J20_4J20.0ETA49_MJJ-800", "HLT_2j55_bmv2c2077_split_L1J30_2J20_4J20.0ETA49_MJJ-900", "HLT_2j55_bmv2c2077_split_ht300_L14J20", "HLT_2j55_bmv2c2085_split_L13J20_4J20.0ETA49_MJJ-400", "HLT_2j55_bmv2c2085_split_L13J20_4J20.0ETA49_MJJ-700", "HLT_2j55_bmv2c2085_split_L13J20_4J20.0ETA49_MJJ-800", "HLT_2j55_bmv2c2085_split_L13J20_4J20.0ETA49_MJJ-900", "HLT_2j55_bmv2c2085_split_L1J30_2J20_4J20.0ETA49_MJJ-400", "HLT_2j55_bmv2c2085_split_L1J30_2J20_4J20.0ETA49_MJJ-700", "HLT_2j55_bmv2c2085_split_L1J30_2J20_4J20.0ETA49_MJJ-800", "HLT_2j55_bmv2c2085_split_L1J30_2J20_4J20.0ETA49_MJJ-900", "HLT_2j55_boffperf_2j55", "HLT_2j55_boffperf_2j55_L13J25.0ETA23", "HLT_2j55_boffperf_ht300_L14J20", "HLT_2j55_boffperf_split_2j55", "HLT_2j55_boffperf_split_2j55_L13J25.0ETA23", "HLT_2j55_boffperf_split_L13J20_4J20.0ETA49_MJJ-400", "HLT_2j55_boffperf_split_L13J20_4J20.0ETA49_MJJ-700", "HLT_2j55_boffperf_split_L13J20_4J20.0ETA49_MJJ-800", "HLT_2j55_boffperf_split_L13J20_4J20.0ETA49_MJJ-900", "HLT_2j55_boffperf_split_L1J30_2J20_4J20.0ETA49_MJJ-400", "HLT_2j55_boffperf_split_L1J30_2J20_4J20.0ETA49_MJJ-700", "HLT_2j55_boffperf_split_L1J30_2J20_4J20.0ETA49_MJJ-800", "HLT_2j55_boffperf_split_L1J30_2J20_4J20.0ETA49_MJJ-900", "HLT_2j55_boffperf_split_ht300_L14J20", "HLT_2j55_boffperf_split_ht300_L1J100", "HLT_2j55_bperf_2j55", "HLT_2j55_bperf_2j55_L13J25.0ETA23", "HLT_2j55_bperf_L13J20_4J20.0ETA49_MJJ-400", "HLT_2j55_bperf_L13J20_4J20.0ETA49_MJJ-700", "HLT_2j55_bperf_L13J20_4J20.0ETA49_MJJ-800", "HLT_2j55_bperf_L13J20_4J20.0ETA49_MJJ-900", "HLT_2j55_bperf_L1J30_2J20_4J20.0ETA49_MJJ-400", "HLT_2j55_bperf_L1J30_2J20_4J20.0ETA49_MJJ-700", "HLT_2j55_bperf_L1J30_2J20_4J20.0ETA49_MJJ-800", "HLT_2j55_bperf_L1J30_2J20_4J20.0ETA49_MJJ-900", "HLT_2j55_bperf_ht300_L14J20", "HLT_2j55_bperf_split_2j55", "HLT_2j55_bperf_split_2j55_L13J25.0ETA23", "HLT_2j55_bperf_split_L13J20_4J20.0ETA49_MJJ-400", "HLT_2j55_bperf_split_L13J20_4J20.0ETA49_MJJ-700", "HLT_2j55_bperf_split_L13J20_4J20.0ETA49_MJJ-800", "HLT_2j55_bperf_split_L13J20_4J20.0ETA49_MJJ-900", "HLT_2j55_bperf_split_L1J30_2J20_4J20.0ETA49_MJJ-400", "HLT_2j55_bperf_split_L1J30_2J20_4J20.0ETA49_MJJ-700", "HLT_2j55_bperf_split_L1J30_2J20_4J20.0ETA49_MJJ-800", "HLT_2j55_bperf_split_L1J30_2J20_4J20.0ETA49_MJJ-900", "HLT_2j55_bperf_split_ht300_L14J20", "HLT_2j65_boffperf_j65", "HLT_2j65_bperf_j65", "HLT_2j65_bperf_split_j65", "HLT_2j70_boffperf_j70", "HLT_2j70_boffperf_split_j70", "HLT_2j70_bperf_j70", "HLT_2j70_bperf_split_j70", "HLT_2j75_boffperf_j75", "HLT_2j75_boffperf_split_j75", "HLT_2j75_bperf_j75", "HLT_2j75_bperf_split_j75", "HLT_2mu10_bBmumu", "HLT_2mu10_bBmumu_noL2", "HLT_2mu10_bBmumux_BcmumuDsloose", "HLT_2mu10_bBmumux_BcmumuDsloose_noL2", "HLT_2mu10_bBmumux_BpmumuKp", "HLT_2mu10_bBmumux_BpmumuKp_noL2", "HLT_2mu10_bBmumuxv2", "HLT_2mu10_bBmumuxv2_noL2", "HLT_2mu10_bDimu", "HLT_2mu10_bDimu_noEFbph",
#"HLT_2mu10_bDimu_noL2",
"HLT_2mu10_bDimu_noinvm_novtx_ss", "HLT_2mu10_bDimu_novtx_noos", "HLT_2mu10_bDimu_novtx_noos_noL2", "HLT_2mu10_bJpsimumu", "HLT_2mu10_bJpsimumu_noEFbph",
#"HLT_2mu10_bJpsimumu_noL2",
"HLT_2mu10_bUpsimumu", "HLT_2mu10_l2msonly", "HLT_2mu10_l2msonly_bDimu_noL2", "HLT_2mu10_l2msonly_bJpsimumu_noL2", "HLT_2mu14_2j35_boffperf", "HLT_2mu14_2j35_boffperf_split", "HLT_2mu14_2j35_bperf", "HLT_2mu14_2j35_bperf_split", "HLT_2mu14_l2msonly", "HLT_2mu14_noMuCombOvlpRm", "HLT_2mu4", "HLT_2mu4_bBmumu_L12CMU4", "HLT_2mu4_bBmumu_L12MU4-B", "HLT_2mu4_bBmumu_L1BPH-1M19-2MU4-BO_BPH-0DR34-2MU4", "HLT_2mu4_bBmumu_L1BPH-1M19-2MU4-B_BPH-0DR34-2MU4", "HLT_2mu4_bBmumu_L1BPH-1M19-2MU4_BPH-0DR34-2MU4", "HLT_2mu4_bBmumu_L1BPH-2M8-2MU4",
#"HLT_2mu4_bBmumu_L1BPH-2M8-2MU4-BO_BPH-0DR15-2MU4",
#"HLT_2mu4_bBmumu_L1BPH-2M8-2MU4-B_BPH-0DR15-2MU4",
#"HLT_2mu4_bBmumu_L1BPH-2M8-2MU4_BPH-0DR15-2MU4",
"HLT_2mu4_bBmumu_Lxy0", "HLT_2mu4_bBmumu_noL2", "HLT_2mu4_bBmumu_noL2_L12CMU4", "HLT_2mu4_bBmumu_noL2_L12MU4-B", "HLT_2mu4_bBmumu_noL2_L1BPH-1M19-2MU4-BO_BPH-0DR34-2MU4", "HLT_2mu4_bBmumu_noL2_L1BPH-1M19-2MU4-B_BPH-0DR34-2MU4", "HLT_2mu4_bBmumu_noL2_L1BPH-1M19-2MU4_BPH-0DR34-2MU4", "HLT_2mu4_bBmumu_noL2_L1BPH-2M8-2MU4", "HLT_2mu4_bBmumu_noL2_L1BPH-2M8-2MU4-BO_BPH-0DR15-2MU4", "HLT_2mu4_bBmumu_noL2_L1BPH-2M8-2MU4-B_BPH-0DR15-2MU4", "HLT_2mu4_bBmumu_noL2_L1BPH-2M8-2MU4_BPH-0DR15-2MU4", "HLT_2mu4_bBmumu_tightChi2", "HLT_2mu4_bBmumux_BcmumuDsloose_L12CMU4", "HLT_2mu4_bBmumux_BcmumuDsloose_L12MU4-B", "HLT_2mu4_bBmumux_BcmumuDsloose_L1BPH-1M19-2MU4-BO_BPH-0DR34-2MU4", "HLT_2mu4_bBmumux_BcmumuDsloose_L1BPH-1M19-2MU4-B_BPH-0DR34-2MU4", "HLT_2mu4_bBmumux_BcmumuDsloose_L1BPH-1M19-2MU4_BPH-0DR34-2MU4", "HLT_2mu4_bBmumux_BcmumuDsloose_L1BPH-2M8-2MU4", "HLT_2mu4_bBmumux_BcmumuDsloose_L1BPH-2M8-2MU4-BO_BPH-0DR15-2MU4", "HLT_2mu4_bBmumux_BcmumuDsloose_L1BPH-2M8-2MU4-B_BPH-0DR15-2MU4", "HLT_2mu4_bBmumux_BcmumuDsloose_noL2", "HLT_2mu4_bBmumux_BcmumuDsloose_noL2_L12CMU4", "HLT_2mu4_bBmumux_BcmumuDsloose_noL2_L12MU4-B", "HLT_2mu4_bBmumux_BcmumuDsloose_noL2_L1BPH-1M19-2MU4-BO_BPH-0DR34-2MU4", "HLT_2mu4_bBmumux_BcmumuDsloose_noL2_L1BPH-1M19-2MU4-B_BPH-0DR34-2MU4", "HLT_2mu4_bBmumux_BcmumuDsloose_noL2_L1BPH-1M19-2MU4_BPH-0DR34-2MU4", "HLT_2mu4_bBmumux_BcmumuDsloose_noL2_L1BPH-2M8-2MU4", "HLT_2mu4_bBmumux_BcmumuDsloose_noL2_L1BPH-2M8-2MU4-BO_BPH-0DR15-2MU4", "HLT_2mu4_bBmumux_BcmumuDsloose_noL2_L1BPH-2M8-2MU4-B_BPH-0DR15-2MU4", "HLT_2mu4_bBmumux_BcmumuDsloose_noL2_L1BPH-2M8-2MU4_BPH-0DR15-2MU4", "HLT_2mu4_bBmumux_BpmumuKp_noL2", "HLT_2mu4_bBmumuxv2_L12CMU4", "HLT_2mu4_bBmumuxv2_L12MU4-B",
#"HLT_2mu4_bBmumuxv2_L1BPH-1M19-2MU4-BO_BPH-0DR34-2MU4",
#"HLT_2mu4_bBmumuxv2_L1BPH-1M19-2MU4-B_BPH-0DR34-2MU4",
#"HLT_2mu4_bBmumuxv2_L1BPH-1M19-2MU4_BPH-0DR34-2MU4",
"HLT_2mu4_bBmumuxv2_L1BPH-2M8-2MU4", "HLT_2mu4_bBmumuxv2_L1BPH-2M8-2MU4-BO_BPH-0DR15-2MU4","HLT_2mu4_bBmumuxv2_L1BPH-2M8-2MU4-B_BPH-0DR15-2MU4","HLT_2mu4_bBmumuxv2_noL2", "HLT_2mu4_bBmumuxv2_noL2_L12CMU4", "HLT_2mu4_bBmumuxv2_noL2_L12MU4-B", "HLT_2mu4_bBmumuxv2_noL2_L1BPH-1M19-2MU4-BO_BPH-0DR34-2MU4", "HLT_2mu4_bBmumuxv2_noL2_L1BPH-1M19-2MU4-B_BPH-0DR34-2MU4", "HLT_2mu4_bBmumuxv2_noL2_L1BPH-1M19-2MU4_BPH-0DR34-2MU4", "HLT_2mu4_bBmumuxv2_noL2_L1BPH-2M8-2MU4", "HLT_2mu4_bBmumuxv2_noL2_L1BPH-2M8-2MU4-BO_BPH-0DR15-2MU4", "HLT_2mu4_bBmumuxv2_noL2_L1BPH-2M8-2MU4-B_BPH-0DR15-2MU4", "HLT_2mu4_bBmumuxv2_noL2_L1BPH-2M8-2MU4_BPH-0DR15-2MU4", "HLT_2mu4_bBmumuxv3", "HLT_2mu4_bDimu", "HLT_2mu4_bDimu_L12CMU4", "HLT_2mu4_bDimu_L12MU4-B", "HLT_2mu4_bDimu_L1BPH-1M19-2MU4-BO_BPH-0DR34-2MU4", "HLT_2mu4_bDimu_L1BPH-1M19-2MU4-B_BPH-0DR34-2MU4", "HLT_2mu4_bDimu_L1BPH-1M19-2MU4_BPH-0DR34-2MU4", "HLT_2mu4_bDimu_L1BPH-2M8-2MU4", "HLT_2mu4_bDimu_L1BPH-2M8-2MU4-BO_BPH-0DR15-2MU4", "HLT_2mu4_bDimu_L1BPH-2M8-2MU4-B_BPH-0DR15-2MU4", "HLT_2mu4_bDimu_L1BPH-2M8-2MU4_BPH-0DR15-2MU4", "HLT_2mu4_bDimu_L1BPH-7M15-2MU4_BPH-0DR24-2MU4", "HLT_2mu4_bDimu_L1BPH-7M15-2MU4_BPH-0DR24-2MU4-B", "HLT_2mu4_bDimu_L1BPH-7M15-2MU4_BPH-0DR24-2MU4-BO", "HLT_2mu4_bDimu_Lxy0", "HLT_2mu4_bDimu_noEFbph", "HLT_2mu4_bDimu_noL2", "HLT_2mu4_bDimu_noL2_L12CMU4", "HLT_2mu4_bDimu_noL2_L12MU4-B", "HLT_2mu4_bDimu_noL2_L1BPH-1M19-2MU4-BO_BPH-0DR34-2MU4", "HLT_2mu4_bDimu_noL2_L1BPH-1M19-2MU4-B_BPH-0DR34-2MU4", "HLT_2mu4_bDimu_noL2_L1BPH-1M19-2MU4_BPH-0DR34-2MU4", "HLT_2mu4_bDimu_noL2_L1BPH-2M8-2MU4", "HLT_2mu4_bDimu_noL2_L1BPH-2M8-2MU4-BO_BPH-0DR15-2MU4", "HLT_2mu4_bDimu_noL2_L1BPH-2M8-2MU4-B_BPH-0DR15-2MU4", "HLT_2mu4_bDimu_noL2_L1BPH-2M8-2MU4_BPH-0DR15-2MU4", "HLT_2mu4_bDimu_noL2_L1BPH-7M15-2MU4_BPH-0DR24-2MU4", "HLT_2mu4_bDimu_noL2_L1BPH-7M15-2MU4_BPH-0DR24-2MU4-B", "HLT_2mu4_bDimu_noL2_L1BPH-7M15-2MU4_BPH-0DR24-2MU4-BO", "HLT_2mu4_bDimu_noinvm_novtx_ss", "HLT_2mu4_bDimu_novtx_noos", "HLT_2mu4_bDimu_novtx_noos_L12CMU4", "HLT_2mu4_bDimu_novtx_noos_L12MU4-B", "HLT_2mu4_bDimu_novtx_noos_L1BPH-1M19-2MU4-BO_BPH-0DR34-2MU4", "HLT_2mu4_bDimu_novtx_noos_L1BPH-1M19-2MU4-B_BPH-0DR34-2MU4", "HLT_2mu4_bDimu_novtx_noos_L1BPH-1M19-2MU4_BPH-0DR34-2MU4", "HLT_2mu4_bDimu_novtx_noos_L1BPH-2M8-2MU4", "HLT_2mu4_bDimu_novtx_noos_L1BPH-2M8-2MU4-BO_BPH-0DR15-2MU4", "HLT_2mu4_bDimu_novtx_noos_L1BPH-2M8-2MU4-B_BPH-0DR15-2MU4", "HLT_2mu4_bDimu_novtx_noos_L1BPH-2M8-2MU4_BPH-0DR15-2MU4", "HLT_2mu4_bDimu_novtx_noos_L1BPH-7M15-2MU4_BPH-0DR24-2MU4", "HLT_2mu4_bDimu_novtx_noos_L1BPH-7M15-2MU4_BPH-0DR24-2MU4-B", "HLT_2mu4_bDimu_novtx_noos_L1BPH-7M15-2MU4_BPH-0DR24-2MU4-BO", "HLT_2mu4_bDimu_novtx_noos_noL2", "HLT_2mu4_bDimu_novtx_noos_noL2_L12CMU4", "HLT_2mu4_bDimu_novtx_noos_noL2_L12MU4-B", "HLT_2mu4_bDimu_novtx_noos_noL2_L1BPH-1M19-2MU4-BO_BPH-0DR34-2MU4", "HLT_2mu4_bDimu_novtx_noos_noL2_L1BPH-1M19-2MU4-B_BPH-0DR34-2MU4", "HLT_2mu4_bDimu_novtx_noos_noL2_L1BPH-1M19-2MU4_BPH-0DR34-2MU4", "HLT_2mu4_bDimu_novtx_noos_noL2_L1BPH-2M8-2MU4", "HLT_2mu4_bDimu_novtx_noos_noL2_L1BPH-2M8-2MU4-BO_BPH-0DR15-2MU4", "HLT_2mu4_bDimu_novtx_noos_noL2_L1BPH-2M8-2MU4-B_BPH-0DR15-2MU4", "HLT_2mu4_bDimu_novtx_noos_noL2_L1BPH-2M8-2MU4_BPH-0DR15-2MU4", "HLT_2mu4_bDimu_novtx_noos_noL2_L1BPH-7M15-2MU4_BPH-0DR24-2MU4", "HLT_2mu4_bDimu_novtx_noos_noL2_L1BPH-7M15-2MU4_BPH-0DR24-2MU4-B", "HLT_2mu4_bDimu_novtx_noos_noL2_L1BPH-7M15-2MU4_BPH-0DR24-2MU4-BO", "HLT_2mu4_bDimu_tightChi2", "HLT_2mu4_bJpsimumu_L12CMU4", "HLT_2mu4_bJpsimumu_L12MU4-B", "HLT_2mu4_bJpsimumu_L1BPH-1M19-2MU4-BO_BPH-0DR34-2MU4", "HLT_2mu4_bJpsimumu_L1BPH-1M19-2MU4-B_BPH-0DR34-2MU4", "HLT_2mu4_bJpsimumu_L1BPH-1M19-2MU4_BPH-0DR34-2MU4", "HLT_2mu4_bJpsimumu_L1BPH-2M8-2MU4",
#"HLT_2mu4_bJpsimumu_L1BPH-2M8-2MU4-BO_BPH-0DR15-2MU4",
#"HLT_2mu4_bJpsimumu_L1BPH-2M8-2MU4-B_BPH-0DR15-2MU4",
"HLT_2mu4_bJpsimumu_Lxy0", "HLT_2mu4_bJpsimumu_noEFbph", "HLT_2mu4_bJpsimumu_noL2", "HLT_2mu4_bJpsimumu_noL2_L12CMU4", "HLT_2mu4_bJpsimumu_noL2_L12MU4-B", "HLT_2mu4_bJpsimumu_noL2_L1BPH-1M19-2MU4-BO_BPH-0DR34-2MU4", "HLT_2mu4_bJpsimumu_noL2_L1BPH-1M19-2MU4-B_BPH-0DR34-2MU4", "HLT_2mu4_bJpsimumu_noL2_L1BPH-1M19-2MU4_BPH-0DR34-2MU4", "HLT_2mu4_bJpsimumu_noL2_L1BPH-2M8-2MU4", "HLT_2mu4_bJpsimumu_noL2_L1BPH-2M8-2MU4-BO_BPH-0DR15-2MU4", "HLT_2mu4_bJpsimumu_noL2_L1BPH-2M8-2MU4-B_BPH-0DR15-2MU4", "HLT_2mu4_bJpsimumu_noL2_L1BPH-2M8-2MU4_BPH-0DR15-2MU4", "HLT_2mu4_bJpsimumu_tightChi2", "HLT_2mu4_bUpsimumu_L12CMU4", "HLT_2mu4_bUpsimumu_L12MU4-B", "HLT_2mu4_bUpsimumu_L1BPH-1M19-2MU4-BO_BPH-0DR34-2MU4", "HLT_2mu4_bUpsimumu_L1BPH-1M19-2MU4-B_BPH-0DR34-2MU4", "HLT_2mu4_bUpsimumu_L1BPH-1M19-2MU4_BPH-0DR34-2MU4",
#"HLT_2mu4_bUpsimumu_L1BPH-7M15-2MU4_BPH-0DR24-2MU4-B",
#"HLT_2mu4_bUpsimumu_L1BPH-7M15-2MU4_BPH-0DR24-2MU4-BO",
"HLT_2mu4_bUpsimumu_tightChi2", "HLT_2mu4_l2msonly_bDimu_noL2", "HLT_2mu4_l2msonly_bJpsimumu_noL2", "HLT_2mu4_muL2_bBmumuxv3", "HLT_2mu4_wOvlpRm_bBmumuxv2", "HLT_2mu4_wOvlpRm_bDimu", "HLT_2mu4_wOvlpRm_bDimu_noinvm_novtx_ss", "HLT_2mu4_wOvlpRm_bDimu_novtx_noos", "HLT_2mu6",
#"HLT_2mu6_bBmumu_L12CMU6",
"HLT_2mu6_bBmumu_L12MU6-B",
#"HLT_2mu6_bBmumu_Lxy0",
"HLT_2mu6_bBmumu_noL2", "HLT_2mu6_bBmumu_noL2_L12CMU6", "HLT_2mu6_bBmumu_noL2_L12MU6-B", "HLT_2mu6_bBmumu_noL2_L1BPH-2M9-2MU6_BPH-2DR15-2MU6", "HLT_2mu6_bBmumu_tightChi2", "HLT_2mu6_bBmumux_BcmumuDsloose_L12CMU6", "HLT_2mu6_bBmumux_BcmumuDsloose_L12MU6-B", "HLT_2mu6_bBmumux_BcmumuDsloose_noL2", "HLT_2mu6_bBmumux_BcmumuDsloose_noL2_L12CMU6", "HLT_2mu6_bBmumux_BcmumuDsloose_noL2_L12MU6-B", "HLT_2mu6_bBmumux_BcmumuDsloose_noL2_L1BPH-2M9-2MU6_BPH-2DR15-2MU6", "HLT_2mu6_bBmumux_BpmumuKp_noL2", "HLT_2mu6_bBmumuxv2_L12CMU6", "HLT_2mu6_bBmumuxv2_L12MU6-B", "HLT_2mu6_bBmumuxv2_noL2", "HLT_2mu6_bBmumuxv2_noL2_L12CMU6", "HLT_2mu6_bBmumuxv2_noL2_L12MU6-B", "HLT_2mu6_bBmumuxv2_noL2_L1BPH-2M9-2MU6_BPH-2DR15-2MU6", "HLT_2mu6_bBmumuxv3", "HLT_2mu6_bDimu", "HLT_2mu6_bDimu_L12CMU6", "HLT_2mu6_bDimu_L12MU6-B", "HLT_2mu6_bDimu_L1BPH-2M9-2MU6_BPH-2DR15-2MU6", "HLT_2mu6_bDimu_L1BPH-8M15-2MU6", "HLT_2mu6_bDimu_L1BPH-8M15-2MU6_BPH-0DR22-2MU6", "HLT_2mu6_bDimu_Lxy0", "HLT_2mu6_bDimu_noEFbph", "HLT_2mu6_bDimu_noL2", "HLT_2mu6_bDimu_noL2_L12CMU6", "HLT_2mu6_bDimu_noL2_L12MU6-B", "HLT_2mu6_bDimu_noL2_L1BPH-2M9-2MU6_BPH-2DR15-2MU6", "HLT_2mu6_bDimu_noL2_L1BPH-8M15-2MU6", "HLT_2mu6_bDimu_noL2_L1BPH-8M15-2MU6_BPH-0DR22-2MU6", "HLT_2mu6_bDimu_noinvm_novtx_ss", "HLT_2mu6_bDimu_novtx_noos", "HLT_2mu6_bDimu_novtx_noos_L12CMU6", "HLT_2mu6_bDimu_novtx_noos_L12MU6-B", "HLT_2mu6_bDimu_novtx_noos_L1BPH-2M9-2MU6_BPH-2DR15-2MU6", "HLT_2mu6_bDimu_novtx_noos_L1BPH-8M15-2MU6", "HLT_2mu6_bDimu_novtx_noos_L1BPH-8M15-2MU6_BPH-0DR22-2MU6", "HLT_2mu6_bDimu_novtx_noos_noL2", "HLT_2mu6_bDimu_novtx_noos_noL2_L12CMU6", "HLT_2mu6_bDimu_novtx_noos_noL2_L12MU6-B", "HLT_2mu6_bDimu_novtx_noos_noL2_L1BPH-2M9-2MU6_BPH-2DR15-2MU6", "HLT_2mu6_bDimu_novtx_noos_noL2_L1BPH-8M15-2MU6", "HLT_2mu6_bDimu_novtx_noos_noL2_L1BPH-8M15-2MU6_BPH-0DR22-2MU6", "HLT_2mu6_bDimu_tightChi2",
#"HLT_2mu6_bJpsimumu_L12CMU6",
"HLT_2mu6_bJpsimumu_L12MU6-B",
#"HLT_2mu6_bJpsimumu_Lxy0",
"HLT_2mu6_bJpsimumu_noEFbph", "HLT_2mu6_bJpsimumu_noL2", "HLT_2mu6_bJpsimumu_noL2_L12CMU6", "HLT_2mu6_bJpsimumu_noL2_L12MU6-B", "HLT_2mu6_bJpsimumu_noL2_L1BPH-2M9-2MU6_BPH-2DR15-2MU6", "HLT_2mu6_bJpsimumu_tightChi2",
#"HLT_2mu6_bUpsimumu_L12CMU6",
"HLT_2mu6_bUpsimumu_L12MU6-B", "HLT_2mu6_bUpsimumu_L1BPH-8M15-2MU6", "HLT_2mu6_bUpsimumu_tightChi2", "HLT_2mu6_l2msonly_bDimu_noL2", "HLT_2mu6_l2msonly_bJpsimumu_noL2", "HLT_2mu6_l2msonly_mu4_l2msonly_L12MU6_3MU4", "HLT_2mu6_mu4", "HLT_2mu6_mu4_bDimu", "HLT_2mu6_mu4_bJpsi", "HLT_2mu6_mu4_bTau_noL2", "HLT_2mu6_mu4_bUpsi", "HLT_2mu6_muL2_bBmumuxv3", "HLT_3j175_jes_PS", "HLT_3j30", "HLT_3j40", "HLT_3j50", "HLT_3mu4_bDimu", "HLT_3mu4_l2msonly", "HLT_3mu4_noMuCombOvlpRm", "HLT_3mu4_nomucomb", "HLT_3mu6_bDimu", "HLT_3mu6_nomucomb", "HLT_3mu6_wOvlpRm_bDimu", "HLT_4j25", "HLT_4j45", "HLT_4j45_test1", "HLT_4j45_test2", "HLT_5j25", "HLT_5j45", "HLT_5j55", "HLT_6j25", "HLT_7j25", "HLT_e0_L2Star_perf_L1EM15", "HLT_e0_perf_L1EM15", "HLT_e0_perf_L1EM3_EMPTY", "HLT_e100_etcut", "HLT_e100_lhvloose", "HLT_e100_vloose", "HLT_e10_etcut_L1EM7", "HLT_e10_lhtight_idperf", "HLT_e10_lhvloose_L1EM7", "HLT_e10_tight_idperf", "HLT_e10_vloose_L1EM7", "HLT_e120_etcut", "HLT_e120_lhloose_HLTCalo", "HLT_e120_lhvloose", "HLT_e120_loose", "HLT_e120_vloose", "HLT_e12_lhloose", "HLT_e12_lhloose_HLTCalo_L12EM10VH", "HLT_e12_lhloose_L1EM10VH", "HLT_e12_lhloose_cutd0dphideta_L1EM10VH", "HLT_e12_lhloose_nod0", "HLT_e12_lhloose_nod0_L1EM10VH", "HLT_e12_lhloose_nodeta_L1EM10VH", "HLT_e12_lhloose_nodphires_L1EM10VH", "HLT_e12_lhmedium", "HLT_e12_lhmedium_nod0", "HLT_e12_lhvloose_L1EM10VH", "HLT_e12_lhvloose_nod0_L1EM10VH", "HLT_e12_loose", "HLT_e12_loose_L1EM10VH", "HLT_e12_medium", "HLT_e12_vloose_L1EM10VH", "HLT_e13_etcut_L1EM12_W-MT25", "HLT_e13_etcut_L1EM12_W-MT30", "HLT_e13_etcut_L1W-NOMATCH", "HLT_e13_etcut_L1W-NOMATCH_W-05RO-XEEMHT", "HLT_e13_etcut_trkcut", "HLT_e13_etcut_trkcut_L1EM12", "HLT_e13_etcut_trkcut_L1EM12_W-MT25_W-15DPHI-JXE-0_W-15DPHI-EMXE", "HLT_e13_etcut_trkcut_L1EM12_W-MT25_W-15DPHI-JXE-0_W-15DPHI-EMXE_XS20_xe20_mt25", "HLT_e13_etcut_trkcut_j20_perf_xe15_2dphi05", "HLT_e13_etcut_trkcut_j20_perf_xe15_2dphi05_mt25", "HLT_e13_etcut_trkcut_j20_perf_xe15_6dphi05", "HLT_e13_etcut_trkcut_j20_perf_xe15_6dphi05_mt25", "HLT_e13_etcut_trkcut_j20_perf_xe15_6dphi05_mt25_L1EM12_W-MT25_W-15DPHI-JXE-0_W-15DPHI-EMXE_W-90RO2-XEHT-0", "HLT_e13_etcut_trkcut_j20_perf_xe15_6dphi05_mt25_L1EM12_W-MT25_W-15DPHI-JXE-0_W-15DPHI-EMXE_XS20", "HLT_e13_etcut_trkcut_xe20", "HLT_e13_etcut_trkcut_xe20_L1EM12_W-MT25_W-15DPHI-JXE-0_W-15DPHI-EMXE_XS20", "HLT_e13_etcut_trkcut_xe20_mt25", "HLT_e13_etcut_trkcut_xs15", "HLT_e13_etcut_trkcut_xs15_L1EM12_W-MT25_W-15DPHI-JXE-0_W-15DPHI-EMXE_XS20", "HLT_e13_etcut_trkcut_xs15_j20_perf_xe15_2dphi05", "HLT_e13_etcut_trkcut_xs15_j20_perf_xe15_2dphi05_mt25", "HLT_e13_etcut_trkcut_xs15_j20_perf_xe15_6dphi05", "HLT_e13_etcut_trkcut_xs15_j20_perf_xe15_6dphi05_mt25", "HLT_e13_etcut_trkcut_xs15_mt25", "HLT_e13_etcut_trkcut_xs30_j10_perf_xe25_6dphi05_mt35", "HLT_e13_etcut_trkcut_xs30_j10_perf_xe25_6dphi05_mt35_L1EM12_W-MT35_W-250RO2-XEHT-0_W-05DPHI-JXE-0_W-05DPHI-EMXE", "HLT_e13_etcut_trkcut_xs30_j10_perf_xe25_6dphi15_mt35", "HLT_e13_etcut_trkcut_xs30_j10_perf_xe25_6dphi15_mt35_L1EM12_W-MT35_W-90RO2-XEHT-0_W-15DPHI-JXE-0_W-15DPHI-EMXE", "HLT_e13_etcut_trkcut_xs30_j10_perf_xe25_6dphi15_mt35_L1EM12_W-MT35_XS30_W-15DPHI-JXE-0_W-15DPHI-EMXE", "HLT_e13_etcut_trkcut_xs40_j10_perf_xe25_6dphi05_mt35", "HLT_e13_etcut_trkcut_xs40_j10_perf_xe25_6dphi05_mt35_L1EM12_W-MT35_XS40_W-05DPHI-JXE-0_W-05DPHI-EMXE", "HLT_e13_etcut_trkcut_xs50_xe30_mt35", "HLT_e13_etcut_trkcut_xs50_xe30_mt35_L1EM10VH_W-MT35_XS50", "HLT_e13_etcut_trkcut_xs50_xe30_mt35_L1EM12_W-MT35_XS50", "HLT_e140_lhloose_HLTCalo", "HLT_e14_etcut", "HLT_e14_etcut_e5_lhtight_Jpsiee", "HLT_e14_etcut_e5_lhtight_Jpsiee_L1JPSI-1M5-EM12", "HLT_e14_etcut_e5_lhtight_nod0_Jpsiee", "HLT_e14_etcut_e5_lhtight_nod0_Jpsiee_L1JPSI-1M5-EM12", "HLT_e14_etcut_e5_tight_Jpsiee", "HLT_e14_etcut_e5_tight_Jpsiee_L1JPSI-1M5-EM12", "HLT_e14_lhtight", "HLT_e14_lhtight_e4_etcut_Jpsiee", "HLT_e14_lhtight_e4_etcut_Jpsiee_L1JPSI-1M5-EM12", "HLT_e14_lhtight_nod0", "HLT_e14_lhtight_nod0_e4_etcut_Jpsiee", "HLT_e14_lhtight_nod0_e4_etcut_Jpsiee_L1JPSI-1M5-EM12", "HLT_e14_tight", "HLT_e14_tight_e4_etcut_Jpsiee", "HLT_e14_tight_e4_etcut_Jpsiee_L1JPSI-1M5-EM12", "HLT_e15_etcut_L1EM7", "HLT_e15_lhloose_HLTCalo_L12EM13VH", "HLT_e15_lhloose_L1EM13VH", "HLT_e15_lhloose_cutd0dphideta_L1EM13VH", "HLT_e15_lhloose_nod0_L1EM13VH", "HLT_e15_lhvloose_L1EM13VH", "HLT_e15_lhvloose_L1EM7", "HLT_e15_lhvloose_nod0_L1EM13VH", "HLT_e15_loose_L1EM13VH", "HLT_e15_vloose_L1EM13VH", "HLT_e15_vloose_L1EM7", "HLT_e17_etcut_L1EM15", "HLT_e17_etcut_L2StarA_L1EM15", "HLT_e17_etcut_L2StarB_L1EM15", "HLT_e17_etcut_L2StarC_L1EM15", "HLT_e17_etcut_L2Star_idperf_L1EM15", "HLT_e17_etcut_idperf_L1EM15", "HLT_e17_lhloose", "HLT_e17_lhloose_HLTCalo", "HLT_e17_lhloose_L1EM15", "HLT_e17_lhloose_L1EM15VHJJ1523ETA49", "HLT_e17_lhloose_cutd0dphideta", "HLT_e17_lhloose_cutd0dphideta_L1EM15", "HLT_e17_lhloose_nod0", "HLT_e17_lhloose_nod0_L1EM15", "HLT_e17_lhloose_nodeta_L1EM15", "HLT_e17_lhloose_nodphires_L1EM15", "HLT_e17_lhmedium", "HLT_e17_lhmedium_L1EM15HI", "HLT_e17_lhmedium_iloose", "HLT_e17_lhmedium_iloose_2e9_lhmedium", "HLT_e17_lhmedium_iloose_L1EM15HI", "HLT_e17_lhmedium_iloose_tau25_medium1_tracktwo_L1DR-EM15TAU12I-J25", "HLT_e17_lhmedium_iloose_tau25_medium1_tracktwo_L1EM15TAU12I-J25", "HLT_e17_lhmedium_iloose_tau25_perf_ptonly_L1EM15HI_2TAU12IM", "HLT_e17_lhmedium_ivarloose_L1EM15HI", "HLT_e17_lhmedium_ivarloose_tau25_medium1_tracktwo", "HLT_e17_lhmedium_ivarloose_tau25_medium1_tracktwo_L1EM15TAU12I-J25", "HLT_e17_lhmedium_ivarloose_tau25_perf_ptonly_L1EM15HI_2TAU12IM", "HLT_e17_lhmedium_nod0", "HLT_e17_lhmedium_nod0_L1EM15HI", "HLT_e17_lhmedium_nod0_iloose", "HLT_e17_lhmedium_nod0_iloose_2e9_lhmedium_nod0", "HLT_e17_lhmedium_nod0_iloose_L1EM15HI", "HLT_e17_lhmedium_nod0_iloose_tau25_medium1_tracktwo_L1DR-EM15TAU12I", "HLT_e17_lhmedium_nod0_iloose_tau25_medium1_tracktwo_L1DR-EM15TAU12I-J25", "HLT_e17_lhmedium_nod0_iloose_tau25_medium1_tracktwo_L1EM15HI_2TAU12IM", "HLT_e17_lhmedium_nod0_ivarloose", "HLT_e17_lhmedium_nod0_ivarloose_2e9_lhmedium_nod0", "HLT_e17_lhmedium_nod0_ivarloose_L1EM15HI", "HLT_e17_lhmedium_nod0_ivarloose_tau25_medium1_tracktwo", "HLT_e17_lhmedium_nod0_ivarloose_tau25_medium1_tracktwo_L1DR-EM15TAU12I", "HLT_e17_lhmedium_nod0_ivarloose_tau25_medium1_tracktwo_L1DR-EM15TAU12I-J25", "HLT_e17_lhmedium_nod0_ivarloose_tau25_medium1_tracktwo_L1EM15HI_2TAU12IM", "HLT_e17_lhmedium_nod0_ivarloose_tau25_medium1_tracktwo_xe50", "HLT_e17_lhmedium_nod0_tau25_medium1_tracktwo_L1EM15-TAU12I", "HLT_e17_lhmedium_nod0_tau25_medium1_tracktwo_L1EM15HI_2TAU12IM", "HLT_e17_lhmedium_nod0_tau25_medium1_tracktwo_xe50_L1XE35_EM15-TAU12I", "HLT_e17_lhmedium_nod0_tau25_medium1_tracktwo_xe50_L1XE40_EM15-TAU12I", "HLT_e17_lhmedium_nod0_tau80_medium1_tracktwo_L1EM15-TAU40", "HLT_e17_lhmedium_tau25_medium1_tracktwo_L1EM15-TAU12I", "HLT_e17_lhmedium_tau25_medium1_tracktwo_L1EM15HI_2TAU12IM", "HLT_e17_lhmedium_tau25_medium1_tracktwo_xe50_L1XE35_EM15-TAU12I", "HLT_e17_lhmedium_tau25_medium1_tracktwo_xe50_L1XE40_EM15-TAU12I", "HLT_e17_lhmedium_tau80_medium1_tracktwo_L1EM15-TAU40", "HLT_e17_lhvloose", "HLT_e17_lhvloose_nod0", "HLT_e17_loose", "HLT_e17_loose_2e9_loose", "HLT_e17_loose_L1EM15", "HLT_e17_loose_L1EM15VHJJ1523ETA49", "HLT_e17_medium", "HLT_e17_medium_2e9_medium", "HLT_e17_medium_L1EM15HI", "HLT_e17_medium_iloose", "HLT_e17_medium_iloose_2e9_medium", "HLT_e17_medium_iloose_L1EM15HI", "HLT_e17_medium_iloose_tau25_medium1_tracktwo_L1DR-EM15TAU12I-J25", "HLT_e17_medium_iloose_tau25_medium1_tracktwo_L1EM15TAU12I-J25", "HLT_e17_medium_iloose_tau25_perf_ptonly_L1EM15HI_2TAU12IM", "HLT_e17_medium_ivarloose_L1EM15HI", "HLT_e17_medium_ivarloose_tau25_medium1_tracktwo", "HLT_e17_medium_ivarloose_tau25_medium1_tracktwo_L1EM15TAU12I-J25", "HLT_e17_medium_ivarloose_tau25_perf_ptonly_L1EM15HI_2TAU12IM", "HLT_e17_medium_tau25_medium1_tracktwo_L1DR-EM15TAU12I", "HLT_e17_medium_tau25_medium1_tracktwo_L1DR-EM15TAU12I-J25", "HLT_e17_medium_tau25_medium1_tracktwo_L1EM15-TAU12I", "HLT_e17_medium_tau25_medium1_tracktwo_L1EM15HI_2TAU12IM", "HLT_e17_medium_tau25_medium1_tracktwo_L1EM15TAU12I-J25", "HLT_e17_medium_tau25_medium1_tracktwo_xe50_L1XE35_EM15-TAU12I", "HLT_e17_medium_tau25_medium1_tracktwo_xe50_L1XE40_EM15-TAU12I", "HLT_e17_medium_tau80_medium1_tracktwo_L1EM15-TAU40", "HLT_e17_vloose", "HLT_e18_etcut_L1EM15_W-MT35", "HLT_e18_etcut_trkcut", "HLT_e18_etcut_trkcut_L1EM15", "HLT_e18_etcut_trkcut_L1EM15_W-MT35_W-05DPHI-JXE-0_W-05DPHI-EM15XE", "HLT_e18_etcut_trkcut_L1EM15_W-MT35_W-05DPHI-JXE-0_W-05DPHI-EM15XE_XS30_xe35_mt35", "HLT_e18_etcut_trkcut_j20_perf_xe20_6dphi15", "HLT_e18_etcut_trkcut_j20_perf_xe20_6dphi15_mt35", "HLT_e18_etcut_trkcut_j20_perf_xe20_6dphi15_mt35_L1EM15_W-MT35_W-05DPHI-JXE-0_W-05DPHI-EM15XE_XS30", "HLT_e18_etcut_trkcut_j20_perf_xe20_6dphi15_mt35_L1EM15_W-MT35_W-250RO2-XEHT-0_W-05DPHI-JXE-0_W-05DPHI-EM15XE", "HLT_e18_etcut_trkcut_xe35", "HLT_e18_etcut_trkcut_xe35_L1EM15_W-MT35_W-05DPHI-JXE-0_W-05DPHI-EM15XE_XS30", "HLT_e18_etcut_trkcut_xe35_mt35", "HLT_e18_etcut_trkcut_xs20", "HLT_e18_etcut_trkcut_xs20_j20_perf_xe20_6dphi15", "HLT_e18_etcut_trkcut_xs20_j20_perf_xe20_6dphi15_mt35", "HLT_e18_etcut_trkcut_xs20_mt35", "HLT_e18_etcut_trkcut_xs50_j10_perf_xe30_6dphi05_mt35", "HLT_e18_etcut_trkcut_xs50_j10_perf_xe30_6dphi05_mt35_L1EM15_W-MT35_W-250RO2-XEHT-0_W-05DPHI-JXE-0_W-05DPHI-EM15XE", "HLT_e18_etcut_trkcut_xs50_j10_perf_xe30_6dphi05_mt35_L1EM15_W-MT35_XS40_W-05DPHI-JXE-0_W-05DPHI-EM15XE", "HLT_e18_etcut_trkcut_xs50_j10_perf_xe30_6dphi15_mt35", "HLT_e18_etcut_trkcut_xs50_j10_perf_xe30_6dphi15_mt35_L1EM15_W-MT35_W-250RO2-XEHT-0_W-15DPHI-JXE-0_W-15DPHI-EM15XE", "HLT_e18_etcut_trkcut_xs50_j10_perf_xe30_6dphi15_mt35_L1EM15_W-MT35_XS40_W-15DPHI-JXE-0_W-15DPHI-EM15XE", "HLT_e18_etcut_trkcut_xs60_j10_perf_xe30_6dphi05_mt35", "HLT_e18_etcut_trkcut_xs60_j10_perf_xe30_6dphi05_mt35_L1EM15_W-MT35_XS60_W-05DPHI-JXE-0_W-05DPHI-EM15XE", "HLT_e18_etcut_trkcut_xs60_j10_perf_xe30_6dphi15_mt35", "HLT_e18_etcut_trkcut_xs60_j10_perf_xe30_6dphi15_mt35_L1EM15_W-MT35_XS60_W-15DPHI-JXE-0_W-15DPHI-EM15XE", "HLT_e18_etcut_trkcut_xs60_xe30_mt35", "HLT_e18_etcut_trkcut_xs60_xe30_mt35_L1EM15VH_W-MT35_XS60", "HLT_e18_etcut_trkcut_xs60_xe30_mt35_L1EM15_W-MT35_XS60", "HLT_e20_etcut_L1EM12", "HLT_e20_lhmedium", "HLT_e20_lhmedium_nod0", "HLT_e20_lhvloose", "HLT_e20_lhvloose_L1EM12", "HLT_e20_lhvloose_nod0", "HLT_e20_medium", "HLT_e20_vloose", "HLT_e20_vloose_L1EM12", "HLT_e24_etcut_trkcut_xs40_j10_perf_xe35_2dphi05_mt35", "HLT_e24_etcut_trkcut_xs60_xe30_mt35", "HLT_e24_etcut_trkcut_xs60_xe30_mt35_L1EM20VH_W-MT35_XS60", "HLT_e24_lhmedium_L1EM15VH", "HLT_e24_lhmedium_L1EM18VH", "HLT_e24_lhmedium_L1EM20VHI", "HLT_e24_lhmedium_L2Star_idperf_L1EM20VH", "HLT_e24_lhmedium_cutd0dphideta_L1EM18VH", "HLT_e24_lhmedium_cutd0dphideta_iloose_L1EM18VH",  "HLT_e24_lhmedium_e9_lhmedium", "HLT_e24_lhmedium_idperf_L1EM20VH", "HLT_e24_lhmedium_iloose_HLTCalo_L1EM18VH", "HLT_e24_lhmedium_iloose_HLTCalo_L1EM20VH", "HLT_e24_lhmedium_iloose_L2StarA_L1EM20VH", "HLT_e24_lhmedium_nod0_L1EM15VH", "HLT_e24_lhmedium_nod0_L1EM18VH", "HLT_e24_lhmedium_nod0_L1EM20VHI", "HLT_e24_lhmedium_nod0_ringer_L1EM20VH", "HLT_e24_lhmedium_nodeta_L1EM18VH", "HLT_e24_lhmedium_nodeta_iloose_L1EM18VH", "HLT_e24_lhmedium_nodphires_L1EM18VH", "HLT_e24_lhmedium_nodphires_iloose_L1EM18VH", "HLT_e24_lhtight_L1EM20VH", "HLT_e24_lhtight_L1EM20VH_e15_etcut_Zee", "HLT_e24_lhtight_cutd0dphideta_iloose_L1EM20VH", "HLT_e24_lhtight_iloose_HLTCalo", "HLT_e24_lhtight_iloose_HLTCalo_L1EM20VH", "HLT_e24_lhtight_iloose_L1EM20VH", "HLT_e24_lhtight_iloose_L2EFCalo_L1EM20VH", "HLT_e24_lhtight_nod0_L1EM20VH", "HLT_e24_lhtight_nod0_iloose_L1EM20VH", "HLT_e24_lhtight_nod0_ivarloose_L1EM20VH", "HLT_e24_lhtight_nod0_ringer_iloose", "HLT_e24_lhtight_nod0_ringer_ivarloose", "HLT_e24_lhtight_smooth_L1EM20VH", "HLT_e24_lhvloose_L1EM18VH", "HLT_e24_lhvloose_L1EM18VH_3j20noL1", "HLT_e24_lhvloose_L1EM18VH_4j20noL1", "HLT_e24_lhvloose_L1EM18VH_5j15noL1", "HLT_e24_lhvloose_L1EM18VH_6j15noL1", "HLT_e24_lhvloose_L1EM20VH", "HLT_e24_lhvloose_L1EM20VH_3j20noL1", "HLT_e24_lhvloose_L1EM20VH_4j20noL1", "HLT_e24_lhvloose_L1EM20VH_5j15noL1", "HLT_e24_lhvloose_L1EM20VH_6j15noL1", "HLT_e24_lhvloose_nod0_L1EM18VH", "HLT_e24_lhvloose_nod0_L1EM18VH_3j20noL1", "HLT_e24_lhvloose_nod0_L1EM18VH_4j20noL1", "HLT_e24_lhvloose_nod0_L1EM18VH_5j15noL1", "HLT_e24_lhvloose_nod0_L1EM18VH_6j15noL1", "HLT_e24_lhvloose_nod0_L1EM20VH", "HLT_e24_lhvloose_nod0_L1EM20VH_3j20noL1", "HLT_e24_lhvloose_nod0_L1EM20VH_4j20noL1", "HLT_e24_lhvloose_nod0_L1EM20VH_5j15noL1", "HLT_e24_lhvloose_nod0_L1EM20VH_6j15noL1", "HLT_e24_medium_L1EM15VH", "HLT_e24_medium_L1EM18VH", "HLT_e24_medium_L1EM20VH", "HLT_e24_medium_L1EM20VHI", "HLT_e24_medium_L2Star_idperf_L1EM20VH", "HLT_e24_medium_idperf_L1EM20VH", "HLT_e24_medium_iloose_L1EM18VH", "HLT_e24_medium_iloose_L2StarA_L1EM20VH", "HLT_e24_tight_L1EM20VH", "HLT_e24_tight_L1EM20VH_e15_etcut_Zee", "HLT_e24_tight_iloose_HLTCalo_L1EM20VH", "HLT_e24_tight_iloose_L1EM20VH", "HLT_e24_tight_iloose_L2EFCalo_L1EM20VH", "HLT_e24_tight_iloose_etisem_L1EM20VH", "HLT_e24_vloose_L1EM18VH", "HLT_e24_vloose_L1EM18VH_3j20noL1", "HLT_e24_vloose_L1EM18VH_4j20noL1", "HLT_e24_vloose_L1EM18VH_5j15noL1", "HLT_e24_vloose_L1EM18VH_6j15noL1", "HLT_e24_vloose_L1EM20VH",  "HLT_e25_etcut_L1EM15", "HLT_e25_lhvloose_L1EM15", "HLT_e25_vloose_L1EM15", "HLT_e26_etcut_trkcut_xs40_xe30_mt35", "HLT_e26_etcut_trkcut_xs40_xe30_mt35_L1EM22VHI_W-MT35_XS40", "HLT_e26_lhmedium_L1EM22VHI", "HLT_e26_lhmedium_nod0_L1EM22VHI", "HLT_e26_lhtight", "HLT_e26_lhtight_e15_etcut_Zee", "HLT_e26_lhtight_iloose_2j35_boffperf", "HLT_e26_lhtight_iloose_2j35_boffperf_split", "HLT_e26_lhtight_iloose_2j35_bperf", "HLT_e26_lhtight_iloose_2j35_bperf_split", "HLT_e26_lhtight_iloose_HLTCalo", "HLT_e26_lhtight_nod0", "HLT_e26_lhtight_nod0_iloose_2j35_bperf", "HLT_e26_lhtight_nod0_ivarloose_2j35_bperf", "HLT_e26_lhtight_smooth", "HLT_e26_lhvloose_L1EM20VH", "HLT_e26_lhvloose_L1EM20VH_3j20noL1", "HLT_e26_lhvloose_L1EM20VH_4j20noL1", "HLT_e26_lhvloose_L1EM20VH_5j15noL1", "HLT_e26_lhvloose_L1EM20VH_6j15noL1", "HLT_e26_lhvloose_nod0_L1EM20VH", "HLT_e26_lhvloose_nod0_L1EM20VH_3j20noL1", "HLT_e26_lhvloose_nod0_L1EM20VH_4j20noL1", "HLT_e26_lhvloose_nod0_L1EM20VH_5j15noL1", "HLT_e26_lhvloose_nod0_L1EM20VH_6j15noL1", "HLT_e26_medium_L1EM22VHI", "HLT_e26_tight_e15_etcut_Zee", "HLT_e26_tight_iloose_2j35_boffperf", "HLT_e26_tight_iloose_2j35_boffperf_split", "HLT_e26_tight_iloose_2j35_bperf", "HLT_e26_tight_iloose_2j35_bperf_split", "HLT_e26_vloose_L1EM20VH", "HLT_e28_lhtight", "HLT_e28_lhtight_nod0", "HLT_e28_lhtight_smooth", "HLT_e30_etcut_L1EM15", "HLT_e30_lhvloose_L1EM15", "HLT_e30_vloose_L1EM15", "HLT_e40_etcut_L1EM15", "HLT_e40_etcut_trkcut_xs40_xe30", "HLT_e40_lhvloose", "HLT_e40_lhvloose_L1EM15", "HLT_e40_lhvloose_nod0", "HLT_e40_vloose", "HLT_e40_vloose_L1EM15", "HLT_e4_etcut", "HLT_e50_etcut_L1EM15", "HLT_e50_lhvloose_L1EM15", "HLT_e50_vloose_L1EM15", "HLT_e5_etcut", "HLT_e5_etcut_L1W-05DPHI-EMXE-1", "HLT_e5_etcut_L1W-05DPHI-JXE-0", "HLT_e5_etcut_L1W-05RO-XEHT-0", "HLT_e5_etcut_L1W-10DPHI-EMXE-0", "HLT_e5_etcut_L1W-10DPHI-JXE-0", "HLT_e5_etcut_L1W-15DPHI-EMXE-0", "HLT_e5_etcut_L1W-15DPHI-JXE-0", "HLT_e5_etcut_L1W-250RO2-XEHT-0", "HLT_e5_etcut_L1W-90RO2-XEHT-0", "HLT_e5_etcut_L1W-HT20-JJ15.ETA49", "HLT_e5_lhloose", "HLT_e5_lhloose_L2StarA", "HLT_e5_lhloose_L2Star_idperf", "HLT_e5_lhloose_idperf", "HLT_e5_lhtight", "HLT_e5_lhtight_e4_etcut", "HLT_e5_lhtight_e4_etcut_Jpsiee", "HLT_e5_lhtight_e4_etcut_Jpsiee_L1JPSI-1M5", "HLT_e5_lhtight_e4_etcut_L1JPSI-1M5", "HLT_e5_lhtight_idperf", "HLT_e5_lhtight_nod0", "HLT_e5_lhtight_nod0_e4_etcut", "HLT_e5_lhtight_nod0_e4_etcut_Jpsiee", "HLT_e5_lhtight_nod0_e4_etcut_Jpsiee_L1JPSI-1M5", "HLT_e5_lhtight_nod0_e4_etcut_L1JPSI-1M5", "HLT_e5_lhvloose", "HLT_e5_lhvloose_nod0", "HLT_e5_loose", "HLT_e5_loose_L2StarA", "HLT_e5_loose_L2Star_idperf", "HLT_e5_loose_idperf", "HLT_e5_tight", "HLT_e5_tight_e4_etcut", "HLT_e5_tight_e4_etcut_Jpsiee", "HLT_e5_tight_e4_etcut_Jpsiee_L1JPSI-1M5", "HLT_e5_tight_e4_etcut_L1JPSI-1M5", "HLT_e5_tight_idperf", "HLT_e5_vloose", "HLT_e60_etcut", "HLT_e60_lhloose", "HLT_e60_lhloose_L1EM24VHI", "HLT_e60_lhloose_L1EM24VHI_xe60noL1", "HLT_e60_lhloose_nod0", "HLT_e60_lhloose_nod0_L1EM24VHI", "HLT_e60_lhloose_nod0_L1EM24VHI_xe60noL1", "HLT_e60_lhmedium_HLTCalo", "HLT_e60_lhvloose", "HLT_e60_loose", "HLT_e60_vloose", "HLT_e70_etcut", "HLT_e70_lhloose", "HLT_e70_lhloose_L1EM24VHI", "HLT_e70_lhloose_L1EM24VHI_xe70noL1", "HLT_e70_lhloose_nod0", "HLT_e70_lhloose_nod0_L1EM24VHI", "HLT_e70_lhloose_nod0_L1EM24VHI_xe70noL1", "HLT_e70_lhvloose", "HLT_e70_loose", "HLT_e70_vloose", "HLT_e7_lhmedium", "HLT_e7_lhmedium_nod0", "HLT_e7_medium", "HLT_e80_etcut", "HLT_e80_lhvloose", "HLT_e80_vloose", "HLT_e9_etcut", "HLT_e9_etcut_e5_lhtight_Jpsiee", "HLT_e9_etcut_e5_lhtight_Jpsiee_L1JPSI-1M5-EM7", "HLT_e9_etcut_e5_lhtight_nod0_Jpsiee", "HLT_e9_etcut_e5_lhtight_nod0_Jpsiee_L1JPSI-1M5-EM7", "HLT_e9_etcut_e5_tight_Jpsiee", "HLT_e9_etcut_e5_tight_Jpsiee_L1JPSI-1M5-EM7", "HLT_e9_lhloose", "HLT_e9_lhloose_nod0", "HLT_e9_lhmedium", "HLT_e9_lhmedium_nod0", "HLT_e9_lhtight", "HLT_e9_lhtight_e4_etcut_Jpsiee", "HLT_e9_lhtight_e4_etcut_Jpsiee_L1JPSI-1M5-EM7", "HLT_e9_lhtight_nod0", "HLT_e9_lhtight_nod0_e4_etcut_Jpsiee", "HLT_e9_lhtight_nod0_e4_etcut_Jpsiee_L1JPSI-1M5-EM7", "HLT_e9_loose", "HLT_e9_medium", "HLT_e9_tight", "HLT_e9_tight_e4_etcut_Jpsiee", "HLT_e9_tight_e4_etcut_Jpsiee_L1JPSI-1M5-EM7", "HLT_eb_high_L1RD2_FILLED", "HLT_eb_low_L1RD2_FILLED", "HLT_g0_perf_L1EM15", "HLT_g0_perf_L1EM3_EMPTY", "HLT_g100_loose", "HLT_g10_etcut", "HLT_g10_etcut_L1EM8I_mu10_iloose_taumass", "HLT_g10_etcut_L1EM8I_mu10_taumass", "HLT_g10_etcut_mu10_L1LFV-EM8I", "HLT_g10_etcut_mu10_iloose_taumass_L1LFV-EM8I", "HLT_g10_etcut_mu10_taumass", "HLT_g10_loose", "HLT_g10_loose_mu10_iloose_taumass_L1LFV-EM8I", "HLT_g10_medium", "HLT_g140_loose_HLTCalo", "HLT_g140_loose_L1EM24VHI", "HLT_g15_loose", "HLT_g15_loose_2j40_0eta490_3j25_0eta490", "HLT_g15_loose_L1EM3", "HLT_g15_loose_L1EM7", "HLT_g20_etcut_L1EM12", "HLT_g20_etcut_L1EM15I_mu4_iloose_taumass", "HLT_g20_etcut_L1EM15I_mu4_taumass", "HLT_g20_etcut_mu4_L1LFV-EM15I", "HLT_g20_etcut_mu4_iloose_taumass_L1LFV-EM15I", "HLT_g20_etcut_mu4_taumass", "HLT_g20_loose", "HLT_g20_loose_2g15_loose_L12EM13VH", "HLT_g20_loose_2j40_0eta490_3j25_0eta490", "HLT_g20_loose_2j40_0eta490_3j25_0eta490_L1MJJ-700", "HLT_g20_loose_2j40_0eta490_3j25_0eta490_L1MJJ-900", "HLT_g20_loose_L1EM12", "HLT_g20_loose_L1EM15", "HLT_g20_loose_L1EM18VH_2j40_0eta490_3j25_0eta490", "HLT_g20_loose_mu4_iloose_taumass_L1LFV-EM15I", "HLT_g20_tight", "HLT_g25_loose", "HLT_g25_loose_L1EM15", "HLT_g25_loose_L1EM20VH_4j35_0eta490", "HLT_g25_medium", "HLT_g25_medium_HLTCalo", "HLT_g25_medium_L1EM22VHI_2j35_0eta490_bmv2c2077_split_2j35_0eta490", "HLT_g25_medium_L1EM22VHI_2j35_0eta490_boffperf_split_2j35_0eta490", "HLT_g25_medium_L1EM22VHI_j35_0eta490_bmv2c2077_split_3j35_0eta490_invm700", "HLT_g25_medium_L1EM22VHI_j35_0eta490_boffperf_split_3j35_0eta490_invm700", "HLT_g30_loose", "HLT_g30_loose_L1EM15", "HLT_g35_loose", "HLT_g35_loose_L1EM15", "HLT_g35_medium", "HLT_g35_medium_HLTCalo", "HLT_g35_medium_HLTCalo_g25_medium_HLTCalo", "HLT_g35_medium_g25_medium_L1EM7_EMPTY", "HLT_g35_medium_g25_medium_L1EM7_UNPAIRED_ISO", "HLT_g35_medium_tau25_perf_tracktwo_L1TAU12", "HLT_g40_loose_L1EM15", "HLT_g40_tight", "HLT_g45_loose_L1EM15", "HLT_g45_tight", "HLT_g45_tight_3j50noL1", "HLT_g50_loose", "HLT_g50_loose_L1EM15", "HLT_g60_loose", "HLT_g60_loose_L1EM15VH", "HLT_g70_loose", "HLT_g75_tight_3j25noL1", "HLT_g75_tight_3j25noL1_L1EM20VHI", "HLT_g75_tight_3j25noL1_L1EM22VHI", "HLT_g80_loose", "HLT_ht0_L1J12_EMPTY", "HLT_ht1000", "HLT_ht1000_L1HT190-J15.ETA21", "HLT_ht400", "HLT_ht400_L1HT150-J20.ETA31", "HLT_ht550", "HLT_ht550_L1HT150-J20.ETA31", "HLT_ht700", "HLT_ht700_L1HT190-J15.ETA21", "HLT_ht850", "HLT_ht850_L1HT190-J15.ETA21", "HLT_j0_0i1c200m400TLA", "HLT_j0_0i1c400m600TLA", "HLT_j0_0i1c500m900TLA", "HLT_j0_0i1c600m800TLA", "HLT_j0_0i1c800m1000TLA", "HLT_j0_1i2c100m300TLA", "HLT_j0_1i2c100m8000TLA", "HLT_j0_1i2c200m8000TLA", "HLT_j0_1i2c300m500TLA", "HLT_j0_1i2c500m700TLA", "HLT_j0_1i2c500m900TLA", "HLT_j0_1i2c600m800TLA", "HLT_j0_L1J12_EMPTY", "HLT_j0_lcw_jes_L1J12", "HLT_j0_perf_L1RD0_FILLED", "HLT_j0_perf_bperf_L1J12_EMPTY", "HLT_j0_perf_bperf_L1MU10", "HLT_j0_perf_bperf_L1RD0_EMPTY", "HLT_j0_perf_ds1_L1All", "HLT_j10", "HLT_j100", "HLT_j100_2j30", "HLT_j100_2j40", "HLT_j100_2j50", "HLT_j100_2j55_bmedium_split", "HLT_j100_2j55_bmv2c2077", "HLT_j100_2j55_boffperf", "HLT_j100_2j55_boffperf_split", "HLT_j100_2j55_bperf", "HLT_j100_2j55_bperf_split", "HLT_j100_xe80_L1J40_DPHI-CJ20XE50", "HLT_j100_xe80_L1J40_DPHI-J20XE50", "HLT_j100_xe80_L1J40_DPHI-J20s2XE50", "HLT_j100_xe80_L1J40_DPHI-Js2XE50", "HLT_j10_320eta490", "HLT_j110", "HLT_j110_320eta490", "HLT_j110_boffperf", "HLT_j110_boffperf_split", "HLT_j110_bperf", "HLT_j110_bperf_split", "HLT_j120_xe80", "HLT_j15", "HLT_j150", "HLT_j150_2j50", "HLT_j150_boffperf", "HLT_j150_boffperf_j50_boffperf", "HLT_j150_boffperf_split", "HLT_j150_bperf", "HLT_j150_bperf_j50_bperf", "HLT_j150_bperf_split", "HLT_j150_bperf_split_j50_bperf_split", "HLT_j150_xe80", "HLT_j15_320eta490", "HLT_j15_boffperf", "HLT_j15_boffperf_split", "HLT_j15_bperf", "HLT_j15_bperf_split", "HLT_j15_j15_320eta490", "HLT_j175", "HLT_j175_320eta490", "HLT_j175_320eta490_jes", "HLT_j175_320eta490_lcw", "HLT_j175_320eta490_lcw_jes", "HLT_j175_320eta490_lcw_nojcalib", "HLT_j175_320eta490_nojcalib", "HLT_j175_bmedium_split_j60_bmedium_split", "HLT_j175_bmv2c2085", "HLT_j175_bmv2c2085_split", "HLT_j175_boffperf", "HLT_j175_boffperf_j60_boffperf", "HLT_j175_boffperf_split_j60_boffperf_split", "HLT_j175_bperf", "HLT_j175_bperf_j60_bperf", "HLT_j175_bperf_split", "HLT_j175_bperf_split_j60_bperf_split", "HLT_j175_jes", "HLT_j175_lcw", "HLT_j175_lcw_jes", "HLT_j175_lcw_nojcalib", "HLT_j175_nojcalib", "HLT_j20", "HLT_j200", "HLT_j200_jes_PS", "HLT_j225_bloose_split", "HLT_j225_boffperf", "HLT_j225_bperf", "HLT_j225_bperf_split", "HLT_j25", "HLT_j25_320eta490", "HLT_j25_320eta490_L1TE5", "HLT_j25_boffperf", "HLT_j25_boffperf_split", "HLT_j25_bperf", "HLT_j25_bperf_split", "HLT_j25_j25_320eta490", "HLT_j260", "HLT_j260_a10_lcw_sub_L1J75", "HLT_j260_a10_lcw_sub_L1SC85", "HLT_j260_a10_sub_L1J75", "HLT_j260_a10r_L1J75", "HLT_j260_boffperf", "HLT_j260_boffperf_split", "HLT_j260_bperf", "HLT_j260_bperf_split", "HLT_j30", "HLT_j300", "HLT_j300_a10_lcw_sub_L1J75", "HLT_j300_a10_lcw_sub_L1SC85", "HLT_j300_a10_sub_L1J75", "HLT_j300_a10r_L1J75", "HLT_j300_bloose_split", "HLT_j300_boffperf", "HLT_j300_boffperf_split", "HLT_j300_bperf", "HLT_j300_bperf_split", "HLT_j300_lcw_nojcalib", "HLT_j30_0eta490_cleanT_xe100_tc_lcw_L1XE70", "HLT_j30_0eta490_cleanT_xe80_L1XE60", "HLT_j30_L1TE10", "HLT_j30_L1TE5", "HLT_j30_jes_cleanLLP_PS_llp_L1LLP-NOMATCH", "HLT_j30_jes_cleanLLP_PS_llp_noiso_L1LLP-NOMATCH", "HLT_j30_xe10_razor100", "HLT_j30_xe60_razor100", "HLT_j320", "HLT_j320_boffperf", "HLT_j320_boffperf_split", "HLT_j320_bperf", "HLT_j320_bperf_split", "HLT_j35", "HLT_j35_320eta490", "HLT_j35_320eta490_L1TE10", "HLT_j35_boffperf", "HLT_j35_boffperf_3j35", "HLT_j35_boffperf_split", "HLT_j35_bperf", "HLT_j35_bperf_3j35", "HLT_j35_bperf_split", "HLT_j35_j35_320eta490", "HLT_j35_jes", "HLT_j35_lcw", "HLT_j35_lcw_jes", "HLT_j35_lcw_nojcalib", "HLT_j35_nojcalib", "HLT_j360_a10_lcw_nojcalib", "HLT_j360_a10_lcw_nojcalib_L1HT150-J20.ETA31", "HLT_j360_a10_lcw_sub", "HLT_j360_a10_lcw_sub_L1HT150-J20.ETA31", "HLT_j360_a10_lcw_sub_L1SC111", "HLT_j360_a10_nojcalib", "HLT_j360_a10_nojcalib_L1HT150-J20.ETA31", "HLT_j360_a10_sub", "HLT_j360_a10_sub_L1HT150-J20.ETA31", "HLT_j360_a10r", "HLT_j40", "HLT_j400_a10_lcw_sub_L1J100", "HLT_j400_a10_lcw_sub_L1SC111", "HLT_j400_a10_sub_L1J100", "HLT_j400_a10r_L1J100", "HLT_j400_boffperf", "HLT_j400_boffperf_split", "HLT_j400_bperf", "HLT_j400_bperf_split", "HLT_j40_0eta490_j30_0eta490_deta25_xe60", "HLT_j40_0eta490_j30_0eta490_deta25_xe80", "HLT_j40_0eta490_j30_0eta490_deta25_xe80_tc_lcw", "HLT_j40_0eta490_xe80_1dphi10_L1DPHI-AJ20s2XE50", "HLT_j40_L1J12", "HLT_j40_L1TE10", "HLT_j40_L1TE20", "HLT_j40_cleanT_xe75_L1XE60", "HLT_j40_cleanT_xe80_L1XE60", "HLT_j420_a10_lcw_sub_L1J100", "HLT_j420_a10_lcw_sub_L1SC111", "HLT_j420_a10_sub_L1J100", "HLT_j420_a10r_L1J100", "HLT_j45", "HLT_j45_320eta490", "HLT_j45_320eta490_L1TE20", "HLT_j45_L1RD0_FILLED", "HLT_j45_boffperf", "HLT_j45_boffperf_3j45", "HLT_j45_boffperf_3j45_L13J15.0ETA25", "HLT_j45_boffperf_3j45_L13J20", "HLT_j45_boffperf_split", "HLT_j45_boffperf_split_3j45", "HLT_j45_boffperf_split_3j45_L13J15.0ETA25", "HLT_j45_boffperf_split_3j45_L13J20", "HLT_j45_bperf", "HLT_j45_bperf_3j45", "HLT_j45_bperf_3j45_L13J15.0ETA25", "HLT_j45_bperf_3j45_L13J20", "HLT_j45_bperf_3j45_L14J20", "HLT_j45_bperf_split", "HLT_j45_bperf_split_3j45", "HLT_j45_bperf_split_3j45_L13J15.0ETA25", "HLT_j45_bperf_split_3j45_L13J20", "HLT_j45_j45_320eta490", "HLT_j460_a10_lcw_nojcalib", "HLT_j460_a10_lcw_nojcalib_L1HT190-J15.ETA21", "HLT_j460_a10_lcw_sub", "HLT_j460_a10_lcw_sub_L1HT190-J15.ETA21", "HLT_j460_a10_lcw_sub_L1SC111", "HLT_j460_a10_nojcalib", "HLT_j460_a10_nojcalib_L1HT190-J15.ETA21", "HLT_j460_a10_sub", "HLT_j460_a10_sub_L1HT190-J15.ETA21", "HLT_j460_a10r", "HLT_j50_0eta490_j30_0eta490_deta30_xe80_L1J30.0ETA49_XE50", "HLT_j50_0eta490_j30_0eta490_deta30_xe80_L1J40_XE50", "HLT_j50_L1J12", "HLT_j50_L1J15", "HLT_j50_L1TE20", "HLT_j55", "HLT_j55_320eta490", "HLT_j55_320eta490_L1TE20", "HLT_j55_L1RD0_FILLED", "HLT_j55_bmv2c2070_ht500_L14J20", "HLT_j55_bmv2c2070_split_ht500_L14J20", "HLT_j55_bmv2c2077_split_ht500_L14J20", "HLT_j55_boffperf", "HLT_j55_boffperf_ht500_L14J20", "HLT_j55_boffperf_split", "HLT_j55_boffperf_split_ht500_L14J20", "HLT_j55_boffperf_split_ht500_L1J100", "HLT_j55_bperf", "HLT_j55_bperf_3j55", "HLT_j55_bperf_ht500_L14J20", "HLT_j55_bperf_split", "HLT_j55_bperf_split_ht500_L14J20", "HLT_j55_btight_ht500_L14J20", "HLT_j55_j55_320eta490", "HLT_j55_j55_320eta490_L1RD0_FILLED", "HLT_j60", "HLT_j60_280eta320", "HLT_j60_320eta490", "HLT_j60_L1J15", "HLT_j60_L1RD0_FILLED", "HLT_j60_TT", "HLT_j60_bmv2c2077_split_j60_L12J40_XE45", "HLT_j60_boffperf_j60_L12J40_XE45", "HLT_j60_boffperf_split_j60_L12J40_XE45", "HLT_j60_cleanT_xe75_L1J40_XE50", "HLT_j60_cleanT_xe80_L1J40_XE50", "HLT_j60_j60_320eta490", "HLT_j65_0eta490_j40_0eta490_invm250_xe80", "HLT_j65_0eta490_j40_0eta490_invm250_xe80_L1XE70", "HLT_j65_boffperf_3j65_L13J25.0ETA23", "HLT_j65_boffperf_3j65_L14J15.0ETA25", "HLT_j65_bperf_3j65_L13J25.0ETA23", "HLT_j65_bperf_3j65_L14J15.0ETA25", "HLT_j65_bperf_split_3j65_L13J25.0ETA23", "HLT_j65_bperf_split_3j65_L14J15.0ETA25", "HLT_j70_boffperf_3j70", "HLT_j70_boffperf_3j70_L13J25.0ETA23", "HLT_j70_boffperf_3j70_L14J15.0ETA25", "HLT_j70_boffperf_split_3j70", "HLT_j70_boffperf_split_3j70_L13J25.0ETA23", "HLT_j70_bperf_3j70", "HLT_j70_bperf_3j70_L13J25.0ETA23", "HLT_j70_bperf_3j70_L14J15.0ETA25", "HLT_j70_bperf_split_3j70", "HLT_j70_bperf_split_3j70_L13J25.0ETA23", "HLT_j70_bperf_split_3j70_L14J15.0ETA25", "HLT_j75_2j30", "HLT_j75_2j40", "HLT_j75_L1J20", "HLT_j75_boffperf_3j75", "HLT_j75_boffperf_3j75_L13J25.0ETA23", "HLT_j75_boffperf_split_3j75", "HLT_j75_boffperf_split_3j75_L13J25.0ETA23", "HLT_j75_bperf_3j75", "HLT_j75_bperf_3j75_L13J25.0ETA23", "HLT_j75_bperf_split_3j75", "HLT_j75_bperf_split_3j75_L13J25.0ETA23", "HLT_j80_bmv2c2077_split_L12J50_XE40", "HLT_j80_bmv2c2077_split_L1J400ETA25_XE50", "HLT_j80_bmv2c2077_split_L1J40_XE50", "HLT_j80_bmv2c2077_split_L1J40_XE60", "HLT_j80_bmv2c2077_split_L1XE60", "HLT_j80_boffperf_L12J50_XE40", "HLT_j80_boffperf_L1J400ETA25_XE50", "HLT_j80_boffperf_L1J40_XE50", "HLT_j80_boffperf_L1J40_XE60", "HLT_j80_boffperf_L1XE60", "HLT_j80_boffperf_split_L12J50_XE40", "HLT_j80_boffperf_split_L1J400ETA25_XE50", "HLT_j80_boffperf_split_L1J40_XE50", "HLT_j80_boffperf_split_L1J40_XE60", "HLT_j80_boffperf_split_L1XE60", "HLT_j80_xe100", "HLT_j80_xe80", "HLT_j80_xe80_1dphi10_L1J40_DPHI-CJ20XE50", "HLT_j80_xe80_1dphi10_L1J40_DPHI-J20XE50", "HLT_j80_xe80_1dphi10_L1J40_DPHI-J20s2XE50", "HLT_j80_xe80_1dphi10_L1J40_DPHI-Js2XE50", "HLT_j85", "HLT_j85_280eta320", "HLT_j85_280eta320_jes", "HLT_j85_280eta320_lcw", "HLT_j85_280eta320_lcw_jes", "HLT_j85_280eta320_lcw_nojcalib", "HLT_j85_280eta320_nojcalib", "HLT_j85_2j45", "HLT_j85_320eta490", "HLT_j85_L1RD0_FILLED", "HLT_j85_boffperf", "HLT_j85_boffperf_split", "HLT_j85_bperf", "HLT_j85_bperf_split", "HLT_j85_cleanL", "HLT_j85_cleanLLP", "HLT_j85_cleanT", "HLT_j85_j85_320eta490", "HLT_j85_jes", "HLT_j85_jes_test1", "HLT_j85_jes_test2", "HLT_j85_lcw", "HLT_j85_lcw_jes", "HLT_j85_lcw_nojcalib", "HLT_j85_nojcalib", "HLT_j85_test1", "HLT_j85_test1_2j45_test1", "HLT_j85_test2", "HLT_j85_test2_2j45_test2", "HLT_l1calooverflow", "HLT_mb_sp1200_pusup700_trk70_hmt_L1TE5", "HLT_mb_sp1300_hmtperf_L1TE30", "HLT_mb_sp1300_hmtperf_L1TE40", "HLT_mb_sp1300_hmtperf_L1TE40.0ETA24", "HLT_mb_sp1400_pusup550_trk90_hmt_L1TE30", "HLT_mb_sp1400_pusup550_trk90_hmt_L1TE40", "HLT_mb_sp1400_pusup550_trk90_hmt_L1TE40.0ETA24", "HLT_mb_sp1400_pusup550_trk90_hmt_L1TE50", "HLT_mb_sp1400_pusup800_trk80_hmt_L1TE5", "HLT_mb_sp1400_trk100_hmt_L1TE40", "HLT_mb_sp1400_trk100_hmt_L1TE40.0ETA24", "HLT_mb_sp1400_trk90_hmt_L1TE30", "HLT_mb_sp1400_trk90_hmt_L1TE40", "HLT_mb_sp1400_trk90_hmt_L1TE40.0ETA24", "HLT_mb_sp1500_hmtperf_L1TE30", "HLT_mb_sp1500_hmtperf_L1TE40", "HLT_mb_sp1500_hmtperf_L1TE40.0ETA24", "HLT_mb_sp1500_hmtperf_L1TE50", "HLT_mb_sp1500_hmtperf_L1TE60", "HLT_mb_sp1500_pusup700_trk100_hmt_L1TE30", "HLT_mb_sp1500_pusup700_trk100_hmt_L1TE40", "HLT_mb_sp1500_pusup700_trk100_hmt_L1TE40.0ETA24", "HLT_mb_sp1500_pusup700_trk100_hmt_L1TE50", "HLT_mb_sp1500_trk100_hmt_L1TE30", "HLT_mb_sp1500_trk100_hmt_L1TE40", "HLT_mb_sp1500_trk100_hmt_L1TE40.0ETA24", "HLT_mb_sp1600_pusup1100_trk90_hmt_L1TE10", "HLT_mb_sp1600_pusup1100_trk90_hmt_L1TE20", "HLT_mb_sp1600_pusup1100_trk90_hmt_L1TE30", "HLT_mb_sp1600_pusup1100_trk90_hmt_L1TE40", "HLT_mb_sp1600_pusup1100_trk90_hmt_L1TE50", "HLT_mb_sp1600_pusup1100_trk90_hmt_L1TE60", "HLT_mb_sp1700_hmtperf_L1TE30", "HLT_mb_sp1700_hmtperf_L1TE40", "HLT_mb_sp1700_hmtperf_L1TE40.0ETA24", "HLT_mb_sp1800_hmtperf_L1TE40", "HLT_mb_sp1800_pusup800_trk110_hmt_L1TE30", "HLT_mb_sp1800_pusup800_trk110_hmt_L1TE40", "HLT_mb_sp1800_pusup800_trk110_hmt_L1TE40.0ETA24", "HLT_mb_sp1800_pusup800_trk110_hmt_L1TE50", "HLT_mb_sp1800_trk110_hmt_L1TE30", "HLT_mb_sp1800_trk110_hmt_L1TE40", "HLT_mb_sp1800_trk110_hmt_L1TE40.0ETA24", "HLT_mb_sp2000_pusup1200_trk100_hmt_L1TE10", "HLT_mb_sp2000_pusup1200_trk100_hmt_L1TE20", "HLT_mb_sp2000_pusup1200_trk100_hmt_L1TE30", "HLT_mb_sp2000_pusup1200_trk100_hmt_L1TE40", "HLT_mb_sp2000_pusup1200_trk100_hmt_L1TE50", "HLT_mb_sp2000_pusup1200_trk100_hmt_L1TE60", "HLT_mb_sp2000_pusup600_trk70_hmt_L1TE60", "HLT_mb_sp2000_pusup600_trk70_hmt_L1TE70", "HLT_mb_sp2000_pusup700_trk50_sumet110_hmt_L1TE70", "HLT_mb_sp2000_pusup700_trk50_sumet70_hmt_L1TE60", "HLT_mb_sp2000_pusup700_trk60_hmt_L1TE60", "HLT_mb_sp2000_pusup700_trk70_hmt_L1TE60", "HLT_mb_sp2000_pusup700_trk70_hmt_L1TE70", "HLT_mb_sp2100_hmtperf_L1TE10", "HLT_mb_sp2100_hmtperf_L1TE20", "HLT_mb_sp2100_hmtperf_L1TE30", "HLT_mb_sp2100_hmtperf_L1TE40", "HLT_mb_sp2100_hmtperf_L1TE50", "HLT_mb_sp2100_hmtperf_L1TE60", "HLT_mb_sp2100_pusup1000_trk120_hmt_L1TE30", "HLT_mb_sp2100_pusup1000_trk120_hmt_L1TE40", "HLT_mb_sp2100_pusup1000_trk120_hmt_L1TE50", "HLT_mb_sp2200_pusup1300_trk110_hmt_L1TE20", "HLT_mb_sp2200_pusup1300_trk110_hmt_L1TE30", "HLT_mb_sp2200_pusup1300_trk110_hmt_L1TE40", "HLT_mb_sp2200_pusup1300_trk110_hmt_L1TE50", "HLT_mb_sp2200_pusup1300_trk110_hmt_L1TE60", "HLT_mb_sp2400_pusup1100_trk130_hmt_L1TE30", "HLT_mb_sp2400_pusup1100_trk130_hmt_L1TE40", "HLT_mb_sp2400_pusup1100_trk130_hmt_L1TE50", "HLT_mb_sp2400_pusup1400_trk120_hmt_L1TE20", "HLT_mb_sp2400_pusup1400_trk120_hmt_L1TE30", "HLT_mb_sp2400_pusup1400_trk120_hmt_L1TE40", "HLT_mb_sp2400_pusup1400_trk120_hmt_L1TE50", "HLT_mb_sp2400_pusup1400_trk120_hmt_L1TE60", "HLT_mb_sp2500_hmtperf_L1TE40", "HLT_mb_sp2500_pusup750_trk90_hmt_L1TE70", "HLT_mb_sp2700_pusup1300_trk140_hmt_L1TE30", "HLT_mb_sp2700_pusup1300_trk140_hmt_L1TE40", "HLT_mb_sp2700_pusup1300_trk140_hmt_L1TE50", "HLT_mb_sp2_hmtperf_L1TE5", "HLT_mb_sp2_hmtperf_L1TE5.0ETA24", "HLT_mb_sp3000_pusup1400_trk150_hmt_L1TE40", "HLT_mb_sp3000_pusup1400_trk150_hmt_L1TE50", "HLT_mb_sp3000_pusup1400_trk150_hmt_L1TE60", "HLT_mb_sp3000_pusup1500_trk160_hmt_L1TE40", "HLT_mb_sp3000_pusup1500_trk160_hmt_L1TE50", "HLT_mb_sp3000_pusup1500_trk160_hmt_L1TE60", "HLT_mb_sp300_trk10_sumet50_hmt_L1TE20", "HLT_mb_sp300_trk10_sumet60_hmt_L1TE40", "HLT_mb_sp600_trk10_sumet50_hmt_L1TE20.0ETA24", "HLT_mb_sp600_trk10_sumet60_hmt_L1TE40.0ETA24", "HLT_mb_sp700_hmtperf_L1TE5", "HLT_mb_sp800_pusup400_trk50_hmt_L1TE5", "HLT_mb_sp900_pusup350_trk60_hmt_L1TE20", "HLT_mb_sp900_pusup350_trk60_hmt_L1TE20.0ETA24", "HLT_mb_sp900_pusup500_trk60_hmt_L1TE5", "HLT_mb_sp900_trk60_hmt_L1TE20", "HLT_mb_sp900_trk60_hmt_L1TE20.0ETA24", "HLT_mb_sptrk_vetombts2in_L1RD0_FILLED", "HLT_mb_sptrk_vetombts2in_L1ZDC_AND", "HLT_mb_sptrk_vetombts2in_L1ZDC_A_C", "HLT_mu0_perf", "HLT_mu10", "HLT_mu10_bJpsi_Trkloose", "HLT_mu10_idperf", "HLT_mu10_mgonly_L1LATEMU10_J50", "HLT_mu10_mgonly_L1LATEMU10_XE50", "HLT_mu10_msonly", "HLT_mu10_mu10_l2msonly_bDimu_noL2", "HLT_mu10_mu10_l2msonly_bJpsimumu_noL2", "HLT_mu10_mu6_bBmumux_BcmumuDsloose", "HLT_mu10_mu6_bBmumux_BcmumuDsloose_L1LFV-MU", "HLT_mu10_mu6_bBmumux_BcmumuDsloose_noL2", "HLT_mu10_mu6_bBmumux_BcmumuDsloose_noL2_L1LFV-MU", "HLT_mu10_mu6_bBmumux_BpmumuKp", "HLT_mu10_mu6_bBmumux_BpmumuKp_noL2", "HLT_mu10_mu6_bBmumuxv2", "HLT_mu10_mu6_bBmumuxv2_L1LFV-MU",
#"HLT_mu10_mu6_bBmumuxv2_noL2",
#"HLT_mu10_mu6_bBmumuxv2_noL2_L1LFV-MU",
"HLT_mu10_muL2_mu6_muL2_bBmumuxv3", "HLT_mu10_mucombTag_noEF_L1MU40", "HLT_mu10_r1extr", "HLT_mu11", "HLT_mu11_2mu4noL1_nscan03_L1MU11_2MU6", "HLT_mu11_L1MU10_2mu4noL1_nscan03_L1MU10_2MU6", "HLT_mu11_llns_mu6noL1_nscan03_bDimu_noL2_L1MU11_2MU6", "HLT_mu11_llns_mu6noL1_nscan03_bJpsimumu_noL2_L1MU11_2MU6", "HLT_mu11_msonly_mu6noL1_msonly_nscan05_noComb", "HLT_mu11_nomucomb_2mu4noL1_nscan03_L1LFV-MU", "HLT_mu11_nomucomb_2mu4noL1_nscan03_L1LFV-MU_bTau", "HLT_mu11_nomucomb_2mu4noL1_nscan03_L1MU11_2MU6_bTau", "HLT_mu11_nomucomb_mu6noL1_nscan03_L1LFV-MU_bTau", "HLT_mu11_nomucomb_mu6noL1_nscan03_L1MU11_2MU6", "HLT_mu13_mu13_idperf_Zmumu", "HLT_mu14", "HLT_mu14_iloose", "HLT_mu14_iloose_tau25_medium1_tracktwo_L1DR-MU10TAU12I", "HLT_mu14_iloose_tau25_medium1_tracktwo_L1DR-MU10TAU12I_TAU12I-J25", "HLT_mu14_iloose_tau25_medium1_tracktwo_L1MU10_TAU12I-J25", "HLT_mu14_iloose_tau25_medium1_tracktwo_L1MU10_TAU12IM", "HLT_mu14_iloose_tau25_perf_ptonly_L1MU10_TAU12IM", "HLT_mu14_iloose_tau25_perf_tracktwo", "HLT_mu14_imedium_mu6noL1_msonly", "HLT_mu14_ivarloose_tau25_medium1_tracktwo_L1MU10_TAU12IM", "HLT_mu14_ivarloose_tau25_perf_ptonly_L1MU10_TAU12IM", "HLT_mu14_tau25_medium1_tracktwo_L1MU10_TAU12IM", "HLT_mu15_mucombTag_noEF_L1MU40", "HLT_mu18", "HLT_mu18_2mu0noL1_JpsimumuFS", "HLT_mu18_2mu4_JpsimumuL2", "HLT_mu18_bJpsi_Trkloose", "HLT_mu18_iloose_mu8noL1_calotag_0eta010", "HLT_mu18_l2idonly_2mu4noL1", "HLT_mu18_l2idonly_mu8noL1", "HLT_mu18_mu8noL1_calotag_0eta010", "HLT_mu20", "HLT_mu20_2mu0noL1_JpsimumuFS", "HLT_mu20_2mu4_JpsimumuL2", "HLT_mu20_L1MU15", "HLT_mu20_L1MU15_2j20noL1", "HLT_mu20_L1MU15_3j20noL1", "HLT_mu20_L1MU15_4j15noL1", "HLT_mu20_L1MU15_5j15noL1", "HLT_mu20_idperf", "HLT_mu20_iloose_2mu4noL1", "HLT_mu20_iloose_mu8noL1", "HLT_mu20_imedium_mu8noL1", "HLT_mu20_l2idonly_mu6noL1_nscan03_bTau", "HLT_mu20_msonly", "HLT_mu20_msonly_mu6noL1_msonly_nscan05_noComb", "HLT_mu20_mucombTag_noEF_L1MU40", "HLT_mu20_nomucomb_mu6noL1_nscan03", "HLT_mu20_r1extr", "HLT_mu22", "HLT_mu22_iloose_mu8noL1_calotag_0eta010", "HLT_mu22_l2idonly_2mu4noL1", "HLT_mu22_l2idonly_mu8noL1", "HLT_mu22_mu8noL1_calotag_0eta010", "HLT_mu24", "HLT_mu24_2j20noL1", "HLT_mu24_3j20noL1", "HLT_mu24_4j15noL1", "HLT_mu24_5j15noL1", "HLT_mu24_L1MU15", "HLT_mu24_L2Star_idperf", "HLT_mu24_icalo", "HLT_mu24_idperf", "HLT_mu24_iloose_mu8noL1_calotag_0eta010", "HLT_mu24_iloosecalo", "HLT_mu24_imediumcalo", "HLT_mu24_l2idonly_2mu4noL1", "HLT_mu24_l2idonly_mu8noL1", "HLT_mu24_mu8noL1_calotag_0eta010", "HLT_mu25_mucombTag_noEF_L1MU40", "HLT_mu26", "HLT_mu26_2j20noL1", "HLT_mu26_3j20noL1", "HLT_mu26_4j15noL1", "HLT_mu26_5j15noL1", "HLT_mu26_imedium_2j35_boffperf", "HLT_mu26_imedium_2j35_boffperf_split", "HLT_mu26_imedium_2j35_bperf", "HLT_mu26_imedium_2j35_bperf_split", "HLT_mu4", "HLT_mu4_2j35_dr05_j35_bmedium_split_antimatchdr05mu_L13J15_BTAG-MU4J15", "HLT_mu4_2j35_dr05_j35_bmedium_split_antimatchdr05mu_L13J20_BTAG-MU4J20", "HLT_mu4_2j35_dr05_j35_bmedium_split_antimatchdr05mu_L1MU4_3J15", "HLT_mu4_2j35_dr05_j35_bmedium_split_antimatchdr05mu_L1MU4_3J20", "HLT_mu4_2j35_dr05_j35_boffperf_split_antimatchdr05mu_L1MU4_3J15", "HLT_mu4_2j35_dr05_j35_boffperf_split_antimatchdr05mu_L1MU4_3J20", "HLT_mu4_2j40_dr05_j40_bloose_split_antimatchdr05mu_L13J15_BTAG-MU4J15", "HLT_mu4_2j40_dr05_j40_bloose_split_antimatchdr05mu_L13J20_BTAG-MU4J20", "HLT_mu4_2j40_dr05_j40_bloose_split_antimatchdr05mu_L1MU4_3J15", "HLT_mu4_2j40_dr05_j40_bloose_split_antimatchdr05mu_L1MU4_3J20", "HLT_mu4_2j40_dr05_j40_boffperf_split_antimatchdr05mu_L1MU4_3J15", "HLT_mu4_2j40_dr05_j40_boffperf_split_antimatchdr05mu_L1MU4_3J20", "HLT_mu4_3j30_dr05_j30_bmedium_split_antimatchdr05mu_L13J15_BTAG-MU4J15", "HLT_mu4_3j30_dr05_j30_bmedium_split_antimatchdr05mu_L13J20_BTAG-MU4J20", "HLT_mu4_3j30_dr05_j30_bmedium_split_antimatchdr05mu_L14J20", "HLT_mu4_3j30_dr05_j30_bmedium_split_antimatchdr05mu_L1MU4_3J15", "HLT_mu4_3j30_dr05_j30_bmedium_split_antimatchdr05mu_L1MU4_3J20", "HLT_mu4_3j30_dr05_j30_boffperf_split_antimatchdr05mu_L13J15_BTAG-MU4J15", "HLT_mu4_3j30_dr05_j30_boffperf_split_antimatchdr05mu_L14J20", "HLT_mu4_3j30_dr05_j30_boffperf_split_antimatchdr05mu_L1MU4_3J15", "HLT_mu4_3j30_dr05_j30_boffperf_split_antimatchdr05mu_L1MU4_3J20", "HLT_mu4_3j35_dr05_j35_bloose_split_antimatchdr05mu_L13J15_BTAG-MU4J15", "HLT_mu4_3j35_dr05_j35_bloose_split_antimatchdr05mu_L13J20_BTAG-MU4J20", "HLT_mu4_3j35_dr05_j35_bloose_split_antimatchdr05mu_L14J20", "HLT_mu4_3j35_dr05_j35_bloose_split_antimatchdr05mu_L1MU4_3J15", "HLT_mu4_3j35_dr05_j35_bloose_split_antimatchdr05mu_L1MU4_3J20", "HLT_mu4_3j35_dr05_j35_boffperf_antimatchdr05mu_L14J20", "HLT_mu4_3j35_dr05_j35_boffperf_split_antimatchdr05mu_L13J15_BTAG-MU4J15", "HLT_mu4_3j35_dr05_j35_boffperf_split_antimatchdr05mu_L13J20_BTAG-MU4J20", "HLT_mu4_3j35_dr05_j35_boffperf_split_antimatchdr05mu_L14J20", "HLT_mu4_3j35_dr05_j35_boffperf_split_antimatchdr05mu_L1MU4_3J15", "HLT_mu4_3j35_dr05_j35_boffperf_split_antimatchdr05mu_L1MU4_3J20", "HLT_mu4_3j45_dr05_L13J15_BTAG-MU4J15", "HLT_mu4_3j45_dr05_L13J20_BTAG-MU4J20", "HLT_mu4_4j40_dr05_L13J15_BTAG-MU4J15", "HLT_mu4_4j40_dr05_L13J20_BTAG-MU4J20", "HLT_mu4_bJpsi_Trkloose", "HLT_mu4_cosmic_L1MU11_EMPTY", "HLT_mu4_cosmic_L1MU4_EMPTY", "HLT_mu4_idperf", "HLT_mu4_iloose_mu4_11invm60_noos_L1DY-BOX-2MU4", "HLT_mu4_iloose_mu4_11invm60_noos_novtx", "HLT_mu4_iloose_mu4_11invm60_noos_novtx_L1DY-BOX-2MU4", "HLT_mu4_iloose_mu4_7invm9_noos_L1DY-BOX-2MU4", "HLT_mu4_iloose_mu4_7invm9_noos_novtx", "HLT_mu4_iloose_mu4_7invm9_noos_novtx_L1DY-BOX-2MU4", "HLT_mu4_j15_bperf_split_dr05_dz02", "HLT_mu4_j15_dr05", "HLT_mu4_j25_bperf_split_dr05_dz02", "HLT_mu4_j25_dr05", "HLT_mu4_j35_bperf_split_dr05_dz02", "HLT_mu4_j35_bperf_split_dr05_dz02_L1BTAG-MU4J15", "HLT_mu4_j35_dr05", "HLT_mu4_j55_bperf_split_dr05_dz02", "HLT_mu4_j55_bperf_split_dr05_dz02_L1BTAG-MU4J15", "HLT_mu4_j55_dr05", "HLT_mu4_j60_dr05_2j35_L13J15_BTAG-MU4J30", "HLT_mu4_j70_dr05_L1BTAG-MU4J30", "HLT_mu4_msonly", "HLT_mu4_msonly_cosmic_L1MU11_EMPTY", "HLT_mu4_msonly_cosmic_L1MU4_EMPTY", "HLT_mu4_mu4_idperf_bJpsimumu_noid", "HLT_mu4_mu4_l2msonly_bDimu_noL2", "HLT_mu4_mu4_l2msonly_bJpsimumu_noL2", "HLT_mu4_mu4noL1", "HLT_mu4_r1extr", "HLT_mu4noL1", "HLT_mu6", "HLT_mu6_2j40_0eta490_invm1000", "HLT_mu6_2j40_0eta490_invm400", "HLT_mu6_2j40_0eta490_invm600", "HLT_mu6_2j40_0eta490_invm800", "HLT_mu6_2mu4", "HLT_mu6_2mu4_bDimu", "HLT_mu6_2mu4_bJpsi", "HLT_mu6_2mu4_bTau", "HLT_mu6_2mu4_bTau_noL2", "HLT_mu6_2mu4_bUpsi", "HLT_mu6_L2Star_idperf", "HLT_mu6_bJpsi_Trkloose", "HLT_mu6_idperf", "HLT_mu6_iloose_mu6_11invm24_noos_L1DY-BOX-2MU6", "HLT_mu6_iloose_mu6_11invm24_noos_novtx", "HLT_mu6_iloose_mu6_11invm24_noos_novtx_L1DY-BOX-2MU6", "HLT_mu6_iloose_mu6_11invm60_noos", "HLT_mu6_iloose_mu6_11invm60_noos_novtx", "HLT_mu6_iloose_mu6_24invm60_noos_novtx", "HLT_mu6_iloose_mu6_24invm60_noos_novtx_L1DY-BOX-2MU6", "HLT_mu6_j110_bperf_split_dr05_dz02", "HLT_mu6_j110_bperf_split_dr05_dz02_L1BTAG-MU6J20", "HLT_mu6_j110_dr05", "HLT_mu6_j150_bperf_split_dr05_dz02", "HLT_mu6_j150_bperf_split_dr05_dz02_L1BTAG-MU6J20", "HLT_mu6_j150_dr05", "HLT_mu6_j175_bperf_split_dr05_dz02", "HLT_mu6_j175_bperf_split_dr05_dz02_L1BTAG-MU6J20", "HLT_mu6_j175_dr05", "HLT_mu6_j260_bperf_split_dr05_dz02", "HLT_mu6_j260_bperf_split_dr05_dz02_L1BTAG-MU6J20", "HLT_mu6_j260_dr05", "HLT_mu6_j320_bperf_split_dr05_dz02", "HLT_mu6_j320_bperf_split_dr05_dz02_L1BTAG-MU6J20", "HLT_mu6_j320_dr05", "HLT_mu6_j400_bperf_split_dr05_dz02", "HLT_mu6_j400_bperf_split_dr05_dz02_L1BTAG-MU6J20", "HLT_mu6_j400_dr05", "HLT_mu6_j50_dr05_2j35_L13J15_BTAG-MU6J25", "HLT_mu6_j60_dr05_L1BTAG-MU6J25", "HLT_mu6_j85_bperf_split_dr05_dz02", "HLT_mu6_j85_bperf_split_dr05_dz02_L1BTAG-MU6J20", "HLT_mu6_j85_dr05", "HLT_mu6_l2msonly_2mu4_l2msonly_L1MU6_3MU4", "HLT_mu6_l2msonly_mu4_bDimu_noL2", "HLT_mu6_l2msonly_mu4_bJpsimumu_noL2", "HLT_mu6_l2msonly_mu4_l2msonly_bDimu_noL2", "HLT_mu6_l2msonly_mu4_l2msonly_bJpsimumu_noL2", "HLT_mu6_msonly", "HLT_mu6_mu4_bBmumu_L12CMU4", "HLT_mu6_mu4_bBmumu_L12MU4-B", #"HLT_mu6_mu4_bBmumu_L1BPH-2M8-MU6MU4-B_BPH-0DR15-MU6MU4",
#"HLT_mu6_mu4_bBmumu_Lxy0",
"HLT_mu6_mu4_bBmumu_noL2", "HLT_mu6_mu4_bBmumu_noL2_L12CMU4", "HLT_mu6_mu4_bBmumu_noL2_L12MU4-B", "HLT_mu6_mu4_bBmumu_noL2_L1BPH-2M8-MU6MU4-B_BPH-0DR15-MU6MU4", "HLT_mu6_mu4_bBmumu_noL2_L1BPH-2M8-MU6MU4_BPH-0DR15-MU6MU4", "HLT_mu6_mu4_bBmumu_tightChi2", "HLT_mu6_mu4_bBmumux_BcmumuDsloose_L12CMU4", "HLT_mu6_mu4_bBmumux_BcmumuDsloose_L12MU4-B", "HLT_mu6_mu4_bBmumux_BcmumuDsloose_L1BPH-2M8-MU6MU4-B_BPH-0DR15-MU6MU4", "HLT_mu6_mu4_bBmumux_BcmumuDsloose_noL2", "HLT_mu6_mu4_bBmumux_BcmumuDsloose_noL2_L12CMU4", "HLT_mu6_mu4_bBmumux_BcmumuDsloose_noL2_L12MU4-B", "HLT_mu6_mu4_bBmumux_BcmumuDsloose_noL2_L1BPH-2M8-MU6MU4-B_BPH-0DR15-MU6MU4", "HLT_mu6_mu4_bBmumux_BcmumuDsloose_noL2_L1BPH-2M8-MU6MU4_BPH-0DR15-MU6MU4", "HLT_mu6_mu4_bBmumux_BpmumuKp_noL2", "HLT_mu6_mu4_bBmumuxv2_L12CMU4", "HLT_mu6_mu4_bBmumuxv2_L12MU4-B", "HLT_mu6_mu4_bBmumuxv2_L1BPH-2M8-MU6MU4_BPH-0DR15-MU6MU4", "HLT_mu6_mu4_bBmumuxv2_noL2", "HLT_mu6_mu4_bBmumuxv2_noL2_L12CMU4", "HLT_mu6_mu4_bBmumuxv2_noL2_L12MU4-B", "HLT_mu6_mu4_bBmumuxv2_noL2_L1BPH-2M8-MU6MU4-B_BPH-0DR15-MU6MU4", "HLT_mu6_mu4_bBmumuxv2_noL2_L1BPH-2M8-MU6MU4_BPH-0DR15-MU6MU4", "HLT_mu6_mu4_bBmumuxv3", "HLT_mu6_mu4_bDimu", "HLT_mu6_mu4_bDimu_L12CMU4", "HLT_mu6_mu4_bDimu_L12MU4-B", "HLT_mu6_mu4_bDimu_L1BPH-2M8-MU6MU4-B_BPH-0DR15-MU6MU4", "HLT_mu6_mu4_bDimu_L1BPH-2M8-MU6MU4_BPH-0DR15-MU6MU4", "HLT_mu6_mu4_bDimu_L1BPH-8M15-MU6MU4", "HLT_mu6_mu4_bDimu_L1BPH-8M15-MU6MU4_BPH-0DR22-MU6MU4", "HLT_mu6_mu4_bDimu_Lxy0", "HLT_mu6_mu4_bDimu_noEFbph", "HLT_mu6_mu4_bDimu_noL2", "HLT_mu6_mu4_bDimu_noL2_L12CMU4", "HLT_mu6_mu4_bDimu_noL2_L12MU4-B", "HLT_mu6_mu4_bDimu_noL2_L1BPH-2M8-MU6MU4-B_BPH-0DR15-MU6MU4", "HLT_mu6_mu4_bDimu_noL2_L1BPH-2M8-MU6MU4_BPH-0DR15-MU6MU4", "HLT_mu6_mu4_bDimu_noL2_L1BPH-8M15-MU6MU4", "HLT_mu6_mu4_bDimu_noL2_L1BPH-8M15-MU6MU4_BPH-0DR22-MU6MU4", "HLT_mu6_mu4_bDimu_noinvm_novtx_ss", "HLT_mu6_mu4_bDimu_novtx_noos", "HLT_mu6_mu4_bDimu_novtx_noos_L12CMU4", "HLT_mu6_mu4_bDimu_novtx_noos_L12MU4-B", "HLT_mu6_mu4_bDimu_novtx_noos_L1BPH-2M8-MU6MU4-B_BPH-0DR15-MU6MU4", "HLT_mu6_mu4_bDimu_novtx_noos_L1BPH-2M8-MU6MU4_BPH-0DR15-MU6MU4", "HLT_mu6_mu4_bDimu_novtx_noos_L1BPH-8M15-MU6MU4", "HLT_mu6_mu4_bDimu_novtx_noos_L1BPH-8M15-MU6MU4_BPH-0DR22-MU6MU4", "HLT_mu6_mu4_bDimu_novtx_noos_noL2", "HLT_mu6_mu4_bDimu_novtx_noos_noL2_L12CMU4", "HLT_mu6_mu4_bDimu_novtx_noos_noL2_L12MU4-B", "HLT_mu6_mu4_bDimu_novtx_noos_noL2_L1BPH-2M8-MU6MU4-B_BPH-0DR15-MU6MU4", "HLT_mu6_mu4_bDimu_novtx_noos_noL2_L1BPH-2M8-MU6MU4_BPH-0DR15-MU6MU4", "HLT_mu6_mu4_bDimu_novtx_noos_noL2_L1BPH-8M15-MU6MU4", "HLT_mu6_mu4_bDimu_novtx_noos_noL2_L1BPH-8M15-MU6MU4_BPH-0DR22-MU6MU4", "HLT_mu6_mu4_bDimu_tightChi2", "HLT_mu6_mu4_bJpsimumu_L12CMU4", "HLT_mu6_mu4_bJpsimumu_L12MU4-B", "HLT_mu6_mu4_bJpsimumu_L1BPH-2M8-MU6MU4_BPH-0DR15-MU6MU4",
#"HLT_mu6_mu4_bJpsimumu_Lxy0",
        "HLT_mu6_mu4_bJpsimumu_noEFbph", "HLT_mu6_mu4_bJpsimumu_noL2", "HLT_mu6_mu4_bJpsimumu_noL2_L12CMU4", "HLT_mu6_mu4_bJpsimumu_noL2_L12MU4-B", "HLT_mu6_mu4_bJpsimumu_noL2_L1BPH-2M8-MU6MU4-B_BPH-0DR15-MU6MU4", "HLT_mu6_mu4_bJpsimumu_noL2_L1BPH-2M8-MU6MU4_BPH-0DR15-MU6MU4", "HLT_mu6_mu4_bJpsimumu_tightChi2", "HLT_mu6_mu4_bUpsimumu_L12CMU4", "HLT_mu6_mu4_bUpsimumu_L12MU4-B", "HLT_mu6_mu4_bUpsimumu_L1BPH-8M15-MU6MU4", "HLT_mu6_mu4_bUpsimumu_tightChi2", "HLT_mu6_mu4_l2msonly_bDimu_noL2", "HLT_mu6_mu4_l2msonly_bJpsimumu_noL2", "HLT_mu6_mu4noL1", "HLT_mu6_mu6_l2msonly_bDimu_noL2", "HLT_mu6_mu6_l2msonly_bJpsimumu_noL2", "HLT_mu6_muL2_mu4_muL2_bBmumuxv3", "HLT_mu6_nomucomb_2mu4_nomucomb_L1MU6_3MU4", "HLT_mu8", "HLT_mu8_mucombTag_noEF_L1MU40", "HLT_noalg_L1MU10", "HLT_noalg_L1MU11_EMPTY", "HLT_noalg_L1MU15", "HLT_noalg_L1MU20", "HLT_noalg_L1MU20_FIRSTEMPTY", "HLT_noalg_L1MU4_EMPTY", "HLT_noalg_L1MU4_FIRSTEMPTY", "HLT_noalg_L1MU4_UNPAIRED_ISO", "HLT_noalg_L1RD0_EMPTY", "HLT_noalg_L1RD0_FILLED", "HLT_noalg_L1TGC_BURST", "HLT_noalg_L1Topo", "HLT_noalg_L1XE35", "HLT_noalg_bkg_L1Bkg", "HLT_noalg_bkg_L1J12_BGRP12", "HLT_noalg_bkg_L1J30.31ETA49_BGRP12", "HLT_noalg_cosmiccalo_L1EM3_EMPTY", "HLT_noalg_cosmiccalo_L1J12_EMPTY", "HLT_noalg_cosmiccalo_L1J3031ETA49_EMPTY", "HLT_noalg_cosmiccalo_L1J30_EMPTY", "HLT_noalg_cosmiccalo_L1RD1_EMPTY", "HLT_noalg_idmon_L1RD0_EMPTY", "HLT_noalg_idmon_L1RD0_FILLED", "HLT_noalg_idmon_L1RD0_UNPAIRED_ISO", "HLT_noalg_l1calo_L12EM3", "HLT_noalg_l1calo_L12MU4", "HLT_noalg_l1calo_L12MU6", "HLT_noalg_l1calo_L14J15", "HLT_noalg_l1calo_L14J20.0ETA49", "HLT_noalg_l1calo_L1EM12_2EM3", "HLT_noalg_l1calo_L1EM15", "HLT_noalg_l1calo_L1EM15HI", "HLT_noalg_l1calo_L1EM15I_MU4", "HLT_noalg_l1calo_L1EM3", "HLT_noalg_l1calo_L1EM7", "HLT_noalg_l1calo_L1EM7_2EM3", "HLT_noalg_l1calo_L1EM8I_MU10", "HLT_noalg_l1calo_L1J100", "HLT_noalg_l1calo_L1J15", "HLT_noalg_l1calo_L1J20", "HLT_noalg_l1calo_L1J25", "HLT_noalg_l1calo_L1J30.0ETA49_2J20.0ETA49", "HLT_noalg_l1calo_L1J40_XE50", "HLT_noalg_l1calo_L1MU10_2MU6", "HLT_noalg_l1calo_L1MU10_TAU12IM", "HLT_noalg_l1calo_L1MU4", "HLT_noalg_l1calo_L1MU4_J30", "HLT_noalg_l1calo_L1MU6_2MU4", "HLT_noalg_l1calo_L1MU6_J20", "HLT_noalg_l1calo_L1TAU12IM", "HLT_noalg_l1calo_L1TAU20IM_2TAU12IM", "HLT_noalg_l1calo_L1TAU30", "HLT_noalg_l1calo_L1TAU40", "HLT_noalg_l1calo_L1XE35", "HLT_noalg_l1calo_L1XE45", "HLT_noalg_l1calo_L1XE55", "HLT_noalg_l1calo_L1XE60", "HLT_noalg_l1calo_L1XE70", "HLT_noalg_larcells_L1LAR-EM", "HLT_noalg_larcells_L1LAR-J", "HLT_noalg_to_L12MU20_OVERLAY", "HLT_noalg_zb_L1ZB", "HLT_tau0_perf_ptonly_L1TAU12", "HLT_tau0_perf_ptonly_L1TAU60", "HLT_tau125_medium1_calo", "HLT_tau125_perf_ptonly", "HLT_tau125_perf_tracktwo", "HLT_tau125_r1medium1", "HLT_tau125_r1perf", "HLT_tau160_idperf_track", "HLT_tau160_idperf_tracktwo", "HLT_tau1_cosmic_ptonly_L1MU4_EMPTY", "HLT_tau1_cosmic_track_L1MU4_EMPTY", "HLT_tau20_r1_idperf", "HLT_tau20_r1medium1", "HLT_tau20_r1perf", "HLT_tau25_dikaon_tracktwo", "HLT_tau25_dikaontight_tracktwo", "HLT_tau25_idperf_track", "HLT_tau25_idperf_tracktwo", "HLT_tau25_idperf_tracktwo2015", "HLT_tau25_loose1_ptonly", "HLT_tau25_loose1_tracktwo", "HLT_tau25_medium1_calo", "HLT_tau25_medium1_mvonly", "HLT_tau25_medium1_ptonly", "HLT_tau25_medium1_track", "HLT_tau25_medium1_trackcalo", "HLT_tau25_medium1_tracktwo", "HLT_tau25_medium1_tracktwo2015", "HLT_tau25_medium1_tracktwo_L1TAU12", "HLT_tau25_medium1_tracktwo_L1TAU12IL", "HLT_tau25_medium1_tracktwo_L1TAU12IT", "HLT_tau25_medium1_tracktwocalo", "HLT_tau25_perf_calo", "HLT_tau25_perf_calo_L1TAU12", "HLT_tau25_perf_ptonly", "HLT_tau25_perf_ptonly_L1TAU12", "HLT_tau25_perf_track", "HLT_tau25_perf_track_L1TAU12", "HLT_tau25_perf_trackcalo", "HLT_tau25_perf_tracktwo", "HLT_tau25_perf_tracktwo2015", "HLT_tau25_perf_tracktwo_L1TAU12", "HLT_tau25_perf_tracktwocalo", "HLT_tau25_r1_idperf", "HLT_tau25_r1perf", "HLT_tau25_tight1_ptonly", "HLT_tau25_tight1_tracktwo", "HLT_tau29_r1medium1", "HLT_tau29_r1perf", "HLT_tau35_dikaon_tracktwo_L1TAU12", "HLT_tau35_dikaontight_tracktwo_L1TAU12", "HLT_tau35_loose1_ptonly", "HLT_tau35_loose1_tracktwo", "HLT_tau35_loose1_tracktwo_tau25_loose1_tracktwo_L1DR-TAU20ITAU12I", "HLT_tau35_loose1_tracktwo_tau25_loose1_tracktwo_L1DR-TAU20ITAU12I-J25", "HLT_tau35_loose1_tracktwo_tau25_loose1_tracktwo_L1TAU20ITAU12I-J25", "HLT_tau35_medium1_calo", "HLT_tau35_medium1_ptonly", "HLT_tau35_medium1_ptonly_L1TAU20", "HLT_tau35_medium1_track", "HLT_tau35_medium1_tracktwo", "HLT_tau35_medium1_tracktwo_L1TAU20", "HLT_tau35_medium1_tracktwo_L1TAU20IL", "HLT_tau35_medium1_tracktwo_L1TAU20IT", "HLT_tau35_medium1_tracktwo_L1TAU20_xe70_L1XE45_L1XE45_TAU20-J20", "HLT_tau35_medium1_tracktwo_tau25_medium1_tracktwo_L1BOX-TAU20ITAU12I", "HLT_tau35_perf_ptonly", "HLT_tau35_perf_ptonly_tau25_perf_ptonly_L1TAU20IM_2TAU12IM", "HLT_tau35_perf_tracktwo", "HLT_tau35_perf_tracktwo_L1TAU20_tau25_perf_tracktwo_L1TAU12", "HLT_tau35_perf_tracktwo_tau25_perf_tracktwo", "HLT_tau35_perf_tracktwo_tau25_perf_tracktwo_ditauL", "HLT_tau35_perf_tracktwo_tau25_perf_tracktwo_ditauM", "HLT_tau35_perf_tracktwo_tau25_perf_tracktwo_ditauT", "HLT_tau35_tight1_ptonly", "HLT_tau35_tight1_tracktwo", "HLT_tau50_medium1_tracktwo_L1TAU12", "HLT_tau5_perf_ptonly_L1TAU8", "HLT_tau80_medium1_calo", "HLT_tau8_cosmic_ptonly", "HLT_tau8_cosmic_track", "HLT_te20", "HLT_te20_tc_lcw", "HLT_te50_L1TE20", "HLT_te50_L1TE20.0ETA24", "HLT_te60_L1TE40", "HLT_te60_L1TE40.0ETA24", "HLT_xe0noL1_l2fsperf", "HLT_xe0noL1_l2fsperf_mht", "HLT_xe0noL1_l2fsperf_pueta", "HLT_xe0noL1_l2fsperf_pufit", "HLT_xe0noL1_l2fsperf_tc_em", "HLT_xe0noL1_l2fsperf_tc_lcw", "HLT_xe100_L1XE50", "HLT_xe100_L1XE80", "HLT_xe100_mht", "HLT_xe100_mht_L1XE50", "HLT_xe100_mht_L1XE80", "HLT_xe100_mht_wEFMu", "HLT_xe100_mht_wEFMu_L1XE50", "HLT_xe100_mht_wEFMu_L1XE80", "HLT_xe100_pueta", "HLT_xe100_pueta_L1XE80", "HLT_xe100_pueta_wEFMu", "HLT_xe100_pueta_wEFMu_L1XE80", "HLT_xe100_pufit", "HLT_xe100_pufit_L1XE80", "HLT_xe100_pufit_wEFMu", "HLT_xe100_pufit_wEFMu_L1XE80", "HLT_xe100_tc_em", "HLT_xe100_tc_em_L1XE50", "HLT_xe100_tc_em_L1XE80", "HLT_xe100_tc_em_wEFMu", "HLT_xe100_tc_em_wEFMu_L1XE50", "HLT_xe100_tc_em_wEFMu_L1XE80", "HLT_xe100_tc_lcw", "HLT_xe100_tc_lcw_L1XE50", "HLT_xe100_tc_lcw_L1XE80", "HLT_xe100_tc_lcw_wEFMu", "HLT_xe100_tc_lcw_wEFMu_L1XE50", "HLT_xe100_tc_lcw_wEFMu_L1XE80", "HLT_xe100_wEFMu", "HLT_xe100_wEFMu_L1XE50", "HLT_xe100_wEFMu_L1XE80", "HLT_xe110_L1XE60", "HLT_xe110_mht_L1XE60", "HLT_xe110_pueta_L1XE60", "HLT_xe110_pufit_L1XE60", "HLT_xe110_tc_lcw_L1XE60", "HLT_xe35", "HLT_xe35_L2FS", "HLT_xe35_l2fsperf_wEFMuFEB_wEFMu", "HLT_xe35_mht", "HLT_xe35_mht_wEFMu", "HLT_xe35_pueta", "HLT_xe35_pueta_wEFMu", "HLT_xe35_pufit", "HLT_xe35_pufit_wEFMu", "HLT_xe35_tc_em", "HLT_xe35_tc_em_wEFMu", "HLT_xe35_tc_lcw", "HLT_xe35_tc_lcw_wEFMu", "HLT_xe35_wEFMu", "HLT_xe50", "HLT_xe50_mht", "HLT_xe50_pueta", "HLT_xe50_pufit", "HLT_xe50_tc_em", "HLT_xe50_tc_lcw", "HLT_xe60", "HLT_xe60_mht", "HLT_xe60_mht_wEFMu", "HLT_xe60_pueta", "HLT_xe60_pueta_wEFMu", "HLT_xe60_pufit", "HLT_xe60_pufit_wEFMu", "HLT_xe60_tc_em", "HLT_xe60_tc_em_wEFMu", "HLT_xe60_tc_lcw", "HLT_xe60_tc_lcw_wEFMu", "HLT_xe60_wEFMu", "HLT_xe70", "HLT_xe70_L1XE45", "HLT_xe70_mht", "HLT_xe70_mht_wEFMu", "HLT_xe70_pueta", "HLT_xe70_pueta_wEFMu", "HLT_xe70_pufit", "HLT_xe70_pufit_wEFMu", "HLT_xe70_tc_em", "HLT_xe70_tc_em_wEFMu", "HLT_xe70_tc_lcw", "HLT_xe70_tc_lcw_wEFMu", "HLT_xe70_wEFMu", "HLT_xe80_L1XE70", "HLT_xe80_mht", "HLT_xe80_mht_L1XE50", "HLT_xe80_mht_L1XE70", "HLT_xe80_mht_wEFMu", "HLT_xe80_mht_wEFMu_L1XE50", "HLT_xe80_mht_wEFMu_L1XE70", "HLT_xe80_pueta", "HLT_xe80_pueta_L1XE50", "HLT_xe80_pueta_L1XE70", "HLT_xe80_pueta_wEFMu", "HLT_xe80_pueta_wEFMu_L1XE50", "HLT_xe80_pueta_wEFMu_L1XE70", "HLT_xe80_pufit", "HLT_xe80_pufit_L1XE50", "HLT_xe80_pufit_L1XE70", "HLT_xe80_pufit_wEFMu", "HLT_xe80_pufit_wEFMu_L1XE50", "HLT_xe80_pufit_wEFMu_L1XE70", "HLT_xe80_tc_em", "HLT_xe80_tc_em_L1XE50", "HLT_xe80_tc_em_L1XE70", "HLT_xe80_tc_em_wEFMu", "HLT_xe80_tc_em_wEFMu_L1XE50", "HLT_xe80_tc_em_wEFMu_L1XE70", "HLT_xe80_tc_lcw", "HLT_xe80_tc_lcw_L1XE50", "HLT_xe80_tc_lcw_L1XE70", "HLT_xe80_tc_lcw_wEFMu", "HLT_xe80_tc_lcw_wEFMu_L1XE50", "HLT_xe80_tc_lcw_wEFMu_L1XE70", "HLT_xe80_wEFMu", "HLT_xe80_wEFMu_L1XE50", "HLT_xe80_wEFMu_L1XE70", "HLT_xe90_L1XE60", "HLT_xe90_mht", "HLT_xe90_mht_L1XE60", "HLT_xe90_mht_wEFMu", "HLT_xe90_pueta", "HLT_xe90_pueta_L1XE60", "HLT_xe90_pueta_L1XE50", "HLT_xe90_pueta_wEFMu", "HLT_xe90_pufit", "HLT_xe90_pufit_L1XE60", "HLT_xe90_pufit_wEFMu", "HLT_xe90_tc_em", "HLT_xe90_tc_em_wEFMu", "HLT_xe90_tc_lcw", "HLT_xe90_tc_lcw_L1XE60", "HLT_xe90_tc_lcw_wEFMu", "HLT_xe90_wEFMu", "HLT_xs15_L1XS20", "HLT_xs20_L1XS30", "HLT_xs30", "HLT_xs30_tc_lcw", "HLT_zdcpeb_L1ZDC_A", "HLT_zdcpeb_L1ZDC_AND", "HLT_zdcpeb_L1ZDC_A_C", "HLT_zdcpeb_L1ZDC_C", "HLT_e24_vloose_L1EM20VH_3j20noL1", "HLT_e24_vloose_L1EM20VH_4j20noL1", "HLT_e24_vloose_L1EM20VH_5j15noL1", "HLT_e24_vloose_L1EM20VH_6j15noL1", "HLT_e26_vloose_L1EM20VH_3j20noL1", "HLT_e26_vloose_L1EM20VH_4j20noL1", "HLT_e26_vloose_L1EM20VH_5j15noL1", "HLT_e26_vloose_L1EM20VH_6j15noL1", 'HLT_2mu6_nomucomb_mu4_nomucomb_bTau_L12MU6_3MU4', 'HLT_mu6_nomucomb_2mu4_nomucomb_bTau_L1MU6_3MU4', 'HLT_mu20_nomucomb_2mu4noL1', 'HLT_mu20_nomucomb_mu8noL1', 'HLT_mu22_nomucomb_mu8noL1', 'HLT_mu22_nomucomb_2mu4noL1', 'HLT_g22_tight', 'HLT_xe90_pufit_wEFMu_L1XE50', 'HLT_xe90_pueta_wEFMu_L1XE50', 'HLT_xe90_pueta_wEFMu_L1XE50', 'HLT_xe90_pufit_L1XE50', 'HLT_xe80_L1XE50', 'HLT_xe80', 'HLT_xe90', "HLT_2mu6_bBmumu", "HLT_mu6_mu4_bBmumuxv2_L1BPH-2M8-MU6MU4-B_BPH-0DR15-MU6MU4", "HLT_mu6_mu4_bBmumux_BcmumuDsloose_L1BPH-2M8-MU6MU4_BPH-0DR15-MU6MU4", "HLT_2mu4_bBmumuxv2_L1BPH-2M8-2MU4_BPH-0DR15-2MU4", "HLT_2mu4_bBmumux_BcmumuDsloose_L1BPH-2M8-2MU4_BPH-0DR15-2MU4",
        ]

else:
    HLT_list=[
        "HLT_2e12_lhloose_HLTCalo_L12EM10VH", "HLT_2e12_lhloose_L12EM10VH", "HLT_2e12_lhloose_cutd0dphideta_L12EM10VH", "HLT_2e12_lhloose_nod0_L12EM10VH", "HLT_2e12_lhloose_nodeta_L12EM10VH", "HLT_2e12_lhloose_nodphires_L12EM10VH", "HLT_2e12_loose_L12EM10VH", "HLT_2e12_vloose_L12EM10VH", "HLT_2e15_lhloose_HLTCalo_L12EM13VH", "HLT_2e15_lhloose_L12EM13VH", "HLT_2e15_lhloose_cutd0dphideta_L12EM13VH", "HLT_2e15_lhloose_nod0_L12EM13VH", "HLT_2e15_loose_L12EM13VH", "HLT_2e15_vloose_L12EM13VH", "HLT_2e17_lhloose_2j35_boffperf", "HLT_2e17_lhloose_2j35_bperf", "HLT_2e17_lhloose_2j35_bperf_split", "HLT_2e17_lhloose_HLTCalo", "HLT_2e17_lhloose_nod0", "HLT_2e17_loose", "HLT_2e17_loose_2j35_boffperf", "HLT_2e17_loose_2j35_boffperf_split", "HLT_2e17_loose_2j35_bperf", "HLT_2e17_loose_2j35_bperf_split", "HLT_2e17_vloose", "HLT_2g25_tight", "HLT_2j10", "HLT_2j10_320eta490", "HLT_2j15", "HLT_2j15_320eta490", "HLT_2j20", "HLT_2j20_320eta490", "HLT_2j25", "HLT_2j25_320eta490", "HLT_2j25_320eta490_L1TE5", "HLT_2j30_0eta490_deta25_xe80_L1DPHI-AJ20s2XE50", "HLT_2j30_0eta490_deta35_xe60_L1DPHI-AJ20s2XE50", "HLT_2j30_0eta490_deta35_xe80_tc_lcw_L1DPHI-AJ20s2XE50", "HLT_2j30_bmv2c2085_split_L12J15_XE55", "HLT_2j30_boffperf_L12J15_XE55", "HLT_2j30_boffperf_split_L12J15_XE55", "HLT_2j35_320eta490", "HLT_2j35_bmv2c2070_2j35_L13J25.0ETA23", "HLT_2j35_boffperf_2j35_L13J25.0ETA23", "HLT_2j35_boffperf_2j35_L14J15.0ETA25", "HLT_2j35_bperf_2j35_L13J25.0ETA23", "HLT_2j35_bperf_2j35_L14J15.0ETA25", "HLT_2j35_bperf_split_2j35_L13J25.0ETA23", "HLT_2j35_bperf_split_2j35_L14J15.0ETA25", "HLT_2j35_btight_split_2j35_L14J15.0ETA25", "HLT_2j40_0eta490_invm250", "HLT_2j40_0eta490_invm250_xe80", "HLT_2j45_bmedium_split_2j45_L14J15.0ETA25", "HLT_2j45_boffperf_2j45", "HLT_2j45_boffperf_2j45_L13J25.0ETA23", "HLT_2j45_boffperf_2j45_L14J15.0ETA25", "HLT_2j45_boffperf_split_2j45", "HLT_2j45_boffperf_split_2j45_L13J25.0ETA23", "HLT_2j45_bperf_2j45", "HLT_2j45_bperf_2j45_L13J25.0ETA23", "HLT_2j45_bperf_2j45_L14J15.0ETA25", "HLT_2j45_bperf_split_2j45", "HLT_2j45_bperf_split_2j45_L13J25.0ETA23", "HLT_2j45_bperf_split_2j45_L14J15.0ETA25", "HLT_2j45_btight_split_2j45_L13J25.0ETA23", "HLT_2j55_bmedium_ht300_L14J20", "HLT_2j55_bmedium_split_2j55_L13J25.0ETA23", "HLT_2j55_bmv2c2070_ht300_L14J20", "HLT_2j55_bmv2c2070_split_ht300_L14J20", "HLT_2j55_bmv2c2077_split_L13J20_4J20.0ETA49_MJJ-400", "HLT_2j55_bmv2c2077_split_L13J20_4J20.0ETA49_MJJ-700", "HLT_2j55_bmv2c2077_split_L13J20_4J20.0ETA49_MJJ-800", "HLT_2j55_bmv2c2077_split_L13J20_4J20.0ETA49_MJJ-900", "HLT_2j55_bmv2c2077_split_L1J30_2J20_4J20.0ETA49_MJJ-400", "HLT_2j55_bmv2c2077_split_L1J30_2J20_4J20.0ETA49_MJJ-700", "HLT_2j55_bmv2c2077_split_L1J30_2J20_4J20.0ETA49_MJJ-800", "HLT_2j55_bmv2c2077_split_L1J30_2J20_4J20.0ETA49_MJJ-900", "HLT_2j55_bmv2c2077_split_ht300_L14J20", "HLT_2j55_bmv2c2085_split_L13J20_4J20.0ETA49_MJJ-400", "HLT_2j55_bmv2c2085_split_L13J20_4J20.0ETA49_MJJ-700", "HLT_2j55_bmv2c2085_split_L13J20_4J20.0ETA49_MJJ-800", "HLT_2j55_bmv2c2085_split_L13J20_4J20.0ETA49_MJJ-900", "HLT_2j55_bmv2c2085_split_L1J30_2J20_4J20.0ETA49_MJJ-400", "HLT_2j55_bmv2c2085_split_L1J30_2J20_4J20.0ETA49_MJJ-700", "HLT_2j55_bmv2c2085_split_L1J30_2J20_4J20.0ETA49_MJJ-800", "HLT_2j55_bmv2c2085_split_L1J30_2J20_4J20.0ETA49_MJJ-900", "HLT_2j55_boffperf_2j55", "HLT_2j55_boffperf_2j55_L13J25.0ETA23", "HLT_2j55_boffperf_ht300_L14J20", "HLT_2j55_boffperf_split_2j55", "HLT_2j55_boffperf_split_2j55_L13J25.0ETA23", "HLT_2j55_boffperf_split_L13J20_4J20.0ETA49_MJJ-400", "HLT_2j55_boffperf_split_L13J20_4J20.0ETA49_MJJ-700", "HLT_2j55_boffperf_split_L13J20_4J20.0ETA49_MJJ-800", "HLT_2j55_boffperf_split_L13J20_4J20.0ETA49_MJJ-900", "HLT_2j55_boffperf_split_L1J30_2J20_4J20.0ETA49_MJJ-400", "HLT_2j55_boffperf_split_L1J30_2J20_4J20.0ETA49_MJJ-700", "HLT_2j55_boffperf_split_L1J30_2J20_4J20.0ETA49_MJJ-800", "HLT_2j55_boffperf_split_L1J30_2J20_4J20.0ETA49_MJJ-900", "HLT_2j55_boffperf_split_ht300_L14J20", "HLT_2j55_boffperf_split_ht300_L1J100", "HLT_2j55_bperf_2j55", "HLT_2j55_bperf_2j55_L13J25.0ETA23", "HLT_2j55_bperf_L13J20_4J20.0ETA49_MJJ-400", "HLT_2j55_bperf_L13J20_4J20.0ETA49_MJJ-700", "HLT_2j55_bperf_L13J20_4J20.0ETA49_MJJ-800", "HLT_2j55_bperf_L13J20_4J20.0ETA49_MJJ-900", "HLT_2j55_bperf_L1J30_2J20_4J20.0ETA49_MJJ-400", "HLT_2j55_bperf_L1J30_2J20_4J20.0ETA49_MJJ-700", "HLT_2j55_bperf_L1J30_2J20_4J20.0ETA49_MJJ-800", "HLT_2j55_bperf_L1J30_2J20_4J20.0ETA49_MJJ-900", "HLT_2j55_bperf_ht300_L14J20", "HLT_2j55_bperf_split_2j55", "HLT_2j55_bperf_split_2j55_L13J25.0ETA23", "HLT_2j55_bperf_split_L13J20_4J20.0ETA49_MJJ-400", "HLT_2j55_bperf_split_L13J20_4J20.0ETA49_MJJ-700", "HLT_2j55_bperf_split_L13J20_4J20.0ETA49_MJJ-800", "HLT_2j55_bperf_split_L13J20_4J20.0ETA49_MJJ-900", "HLT_2j55_bperf_split_L1J30_2J20_4J20.0ETA49_MJJ-400", "HLT_2j55_bperf_split_L1J30_2J20_4J20.0ETA49_MJJ-700", "HLT_2j55_bperf_split_L1J30_2J20_4J20.0ETA49_MJJ-800", "HLT_2j55_bperf_split_L1J30_2J20_4J20.0ETA49_MJJ-900", "HLT_2j55_bperf_split_ht300_L14J20", "HLT_2j65_boffperf_j65", "HLT_2j65_bperf_j65", "HLT_2j65_bperf_split_j65", "HLT_2j70_boffperf_j70", "HLT_2j70_boffperf_split_j70", "HLT_2j70_bperf_j70", "HLT_2j70_bperf_split_j70", "HLT_2j75_boffperf_j75", "HLT_2j75_boffperf_split_j75", "HLT_2j75_bperf_j75", "HLT_2j75_bperf_split_j75", "HLT_2mu10_bBmumu", "HLT_2mu10_bBmumu_noL2", "HLT_2mu10_bBmumux_BcmumuDsloose", "HLT_2mu10_bBmumux_BcmumuDsloose_noL2", "HLT_2mu10_bBmumux_BpmumuKp", "HLT_2mu10_bBmumux_BpmumuKp_noL2", "HLT_2mu10_bBmumuxv2", "HLT_2mu10_bBmumuxv2_noL2", "HLT_2mu10_bDimu", "HLT_2mu10_bDimu_noEFbph",
#"HLT_2mu10_bDimu_noL2",
"HLT_2mu10_bDimu_noinvm_novtx_ss", "HLT_2mu10_bDimu_novtx_noos", "HLT_2mu10_bDimu_novtx_noos_noL2", "HLT_2mu10_bJpsimumu", "HLT_2mu10_bJpsimumu_noEFbph",
#"HLT_2mu10_bJpsimumu_noL2",
"HLT_2mu10_bUpsimumu", "HLT_2mu10_l2msonly", "HLT_2mu10_l2msonly_bDimu_noL2", "HLT_2mu10_l2msonly_bJpsimumu_noL2", "HLT_2mu14_2j35_boffperf", "HLT_2mu14_2j35_bperf", "HLT_2mu14_2j35_bperf_split", "HLT_2mu14_l2msonly", "HLT_2mu14_noMuCombOvlpRm", "HLT_2mu4_bBmumu_L12CMU4", "HLT_2mu4_bBmumu_L12MU4-B", "HLT_2mu4_bBmumu_L1BPH-1M19-2MU4-BO_BPH-0DR34-2MU4", "HLT_2mu4_bBmumu_L1BPH-1M19-2MU4-B_BPH-0DR34-2MU4", "HLT_2mu4_bBmumu_L1BPH-1M19-2MU4_BPH-0DR34-2MU4", "HLT_2mu4_bBmumu_L1BPH-2M8-2MU4",
#"HLT_2mu4_bBmumu_L1BPH-2M8-2MU4-BO_BPH-0DR15-2MU4",
#"HLT_2mu4_bBmumu_L1BPH-2M8-2MU4-B_BPH-0DR15-2MU4",
#"HLT_2mu4_bBmumu_L1BPH-2M8-2MU4_BPH-0DR15-2MU4",
"HLT_2mu4_bBmumu_Lxy0", "HLT_2mu4_bBmumu_noL2", "HLT_2mu4_bBmumu_noL2_L12CMU4", "HLT_2mu4_bBmumu_noL2_L12MU4-B", "HLT_2mu4_bBmumu_noL2_L1BPH-1M19-2MU4-BO_BPH-0DR34-2MU4", "HLT_2mu4_bBmumu_noL2_L1BPH-1M19-2MU4-B_BPH-0DR34-2MU4", "HLT_2mu4_bBmumu_noL2_L1BPH-1M19-2MU4_BPH-0DR34-2MU4", "HLT_2mu4_bBmumu_noL2_L1BPH-2M8-2MU4", "HLT_2mu4_bBmumu_noL2_L1BPH-2M8-2MU4-BO_BPH-0DR15-2MU4", "HLT_2mu4_bBmumu_noL2_L1BPH-2M8-2MU4-B_BPH-0DR15-2MU4", "HLT_2mu4_bBmumu_noL2_L1BPH-2M8-2MU4_BPH-0DR15-2MU4", "HLT_2mu4_bBmumu_tightChi2", "HLT_2mu4_bBmumux_BcmumuDsloose_L12CMU4", "HLT_2mu4_bBmumux_BcmumuDsloose_L12MU4-B", "HLT_2mu4_bBmumux_BcmumuDsloose_L1BPH-1M19-2MU4-BO_BPH-0DR34-2MU4", "HLT_2mu4_bBmumux_BcmumuDsloose_L1BPH-1M19-2MU4-B_BPH-0DR34-2MU4", "HLT_2mu4_bBmumux_BcmumuDsloose_L1BPH-1M19-2MU4_BPH-0DR34-2MU4", "HLT_2mu4_bBmumux_BcmumuDsloose_L1BPH-2M8-2MU4", "HLT_2mu4_bBmumux_BcmumuDsloose_L1BPH-2M8-2MU4-BO_BPH-0DR15-2MU4", "HLT_2mu4_bBmumux_BcmumuDsloose_L1BPH-2M8-2MU4-B_BPH-0DR15-2MU4", "HLT_2mu4_bBmumux_BcmumuDsloose_noL2", "HLT_2mu4_bBmumux_BcmumuDsloose_noL2_L12CMU4", "HLT_2mu4_bBmumux_BcmumuDsloose_noL2_L12MU4-B", "HLT_2mu4_bBmumux_BcmumuDsloose_noL2_L1BPH-1M19-2MU4-BO_BPH-0DR34-2MU4", "HLT_2mu4_bBmumux_BcmumuDsloose_noL2_L1BPH-1M19-2MU4-B_BPH-0DR34-2MU4", "HLT_2mu4_bBmumux_BcmumuDsloose_noL2_L1BPH-1M19-2MU4_BPH-0DR34-2MU4", "HLT_2mu4_bBmumux_BcmumuDsloose_noL2_L1BPH-2M8-2MU4", "HLT_2mu4_bBmumux_BcmumuDsloose_noL2_L1BPH-2M8-2MU4-BO_BPH-0DR15-2MU4", "HLT_2mu4_bBmumux_BcmumuDsloose_noL2_L1BPH-2M8-2MU4-B_BPH-0DR15-2MU4", "HLT_2mu4_bBmumux_BcmumuDsloose_noL2_L1BPH-2M8-2MU4_BPH-0DR15-2MU4", "HLT_2mu4_bBmumux_BpmumuKp_noL2", "HLT_2mu4_bBmumuxv2_L12CMU4", "HLT_2mu4_bBmumuxv2_L12MU4-B",
#"HLT_2mu4_bBmumuxv2_L1BPH-1M19-2MU4-BO_BPH-0DR34-2MU4",
#"HLT_2mu4_bBmumuxv2_L1BPH-1M19-2MU4-B_BPH-0DR34-2MU4",
#"HLT_2mu4_bBmumuxv2_L1BPH-1M19-2MU4_BPH-0DR34-2MU4",
"HLT_2mu4_bBmumuxv2_L1BPH-2M8-2MU4", "HLT_2mu4_bBmumuxv2_L1BPH-2M8-2MU4-BO_BPH-0DR15-2MU4", "HLT_2mu4_bBmumuxv2_L1BPH-2M8-2MU4-B_BPH-0DR15-2MU4", "HLT_2mu4_bBmumuxv2_noL2", "HLT_2mu4_bBmumuxv2_noL2_L12CMU4", "HLT_2mu4_bBmumuxv2_noL2_L12MU4-B", "HLT_2mu4_bBmumuxv2_noL2_L1BPH-1M19-2MU4-BO_BPH-0DR34-2MU4", "HLT_2mu4_bBmumuxv2_noL2_L1BPH-1M19-2MU4-B_BPH-0DR34-2MU4", "HLT_2mu4_bBmumuxv2_noL2_L1BPH-1M19-2MU4_BPH-0DR34-2MU4", "HLT_2mu4_bBmumuxv2_noL2_L1BPH-2M8-2MU4", "HLT_2mu4_bBmumuxv2_noL2_L1BPH-2M8-2MU4-BO_BPH-0DR15-2MU4", "HLT_2mu4_bBmumuxv2_noL2_L1BPH-2M8-2MU4-B_BPH-0DR15-2MU4", "HLT_2mu4_bBmumuxv2_noL2_L1BPH-2M8-2MU4_BPH-0DR15-2MU4", "HLT_2mu4_bBmumuxv3", "HLT_2mu4_bDimu_L12CMU4", "HLT_2mu4_bDimu_L12MU4-B", "HLT_2mu4_bDimu_L1BPH-1M19-2MU4-BO_BPH-0DR34-2MU4", "HLT_2mu4_bDimu_L1BPH-1M19-2MU4-B_BPH-0DR34-2MU4", "HLT_2mu4_bDimu_L1BPH-1M19-2MU4_BPH-0DR34-2MU4", "HLT_2mu4_bDimu_L1BPH-2M8-2MU4", "HLT_2mu4_bDimu_L1BPH-2M8-2MU4-BO_BPH-0DR15-2MU4", "HLT_2mu4_bDimu_L1BPH-2M8-2MU4-B_BPH-0DR15-2MU4", "HLT_2mu4_bDimu_L1BPH-2M8-2MU4_BPH-0DR15-2MU4", "HLT_2mu4_bDimu_L1BPH-7M15-2MU4_BPH-0DR24-2MU4-B", "HLT_2mu4_bDimu_L1BPH-7M15-2MU4_BPH-0DR24-2MU4-BO", "HLT_2mu4_bDimu_Lxy0", "HLT_2mu4_bDimu_noEFbph", "HLT_2mu4_bDimu_noL2", "HLT_2mu4_bDimu_noL2_L12CMU4", "HLT_2mu4_bDimu_noL2_L12MU4-B", "HLT_2mu4_bDimu_noL2_L1BPH-1M19-2MU4-BO_BPH-0DR34-2MU4", "HLT_2mu4_bDimu_noL2_L1BPH-1M19-2MU4-B_BPH-0DR34-2MU4", "HLT_2mu4_bDimu_noL2_L1BPH-1M19-2MU4_BPH-0DR34-2MU4", "HLT_2mu4_bDimu_noL2_L1BPH-2M8-2MU4", "HLT_2mu4_bDimu_noL2_L1BPH-2M8-2MU4-BO_BPH-0DR15-2MU4", "HLT_2mu4_bDimu_noL2_L1BPH-2M8-2MU4-B_BPH-0DR15-2MU4", "HLT_2mu4_bDimu_noL2_L1BPH-2M8-2MU4_BPH-0DR15-2MU4", "HLT_2mu4_bDimu_noL2_L1BPH-7M15-2MU4_BPH-0DR24-2MU4", "HLT_2mu4_bDimu_noL2_L1BPH-7M15-2MU4_BPH-0DR24-2MU4-B", "HLT_2mu4_bDimu_noL2_L1BPH-7M15-2MU4_BPH-0DR24-2MU4-BO", "HLT_2mu4_bDimu_novtx_noos_L12CMU4", "HLT_2mu4_bDimu_novtx_noos_L12MU4-B", "HLT_2mu4_bDimu_novtx_noos_L1BPH-1M19-2MU4-BO_BPH-0DR34-2MU4", "HLT_2mu4_bDimu_novtx_noos_L1BPH-1M19-2MU4-B_BPH-0DR34-2MU4", "HLT_2mu4_bDimu_novtx_noos_L1BPH-1M19-2MU4_BPH-0DR34-2MU4", "HLT_2mu4_bDimu_novtx_noos_L1BPH-2M8-2MU4", "HLT_2mu4_bDimu_novtx_noos_L1BPH-2M8-2MU4-BO_BPH-0DR15-2MU4", "HLT_2mu4_bDimu_novtx_noos_L1BPH-2M8-2MU4-B_BPH-0DR15-2MU4", "HLT_2mu4_bDimu_novtx_noos_L1BPH-2M8-2MU4_BPH-0DR15-2MU4", "HLT_2mu4_bDimu_novtx_noos_L1BPH-7M15-2MU4_BPH-0DR24-2MU4-B", "HLT_2mu4_bDimu_novtx_noos_L1BPH-7M15-2MU4_BPH-0DR24-2MU4-BO", "HLT_2mu4_bDimu_novtx_noos_noL2", "HLT_2mu4_bDimu_novtx_noos_noL2_L12CMU4", "HLT_2mu4_bDimu_novtx_noos_noL2_L12MU4-B", "HLT_2mu4_bDimu_novtx_noos_noL2_L1BPH-1M19-2MU4-BO_BPH-0DR34-2MU4", "HLT_2mu4_bDimu_novtx_noos_noL2_L1BPH-1M19-2MU4-B_BPH-0DR34-2MU4", "HLT_2mu4_bDimu_novtx_noos_noL2_L1BPH-1M19-2MU4_BPH-0DR34-2MU4", "HLT_2mu4_bDimu_novtx_noos_noL2_L1BPH-2M8-2MU4", "HLT_2mu4_bDimu_novtx_noos_noL2_L1BPH-2M8-2MU4-BO_BPH-0DR15-2MU4", "HLT_2mu4_bDimu_novtx_noos_noL2_L1BPH-2M8-2MU4-B_BPH-0DR15-2MU4", "HLT_2mu4_bDimu_novtx_noos_noL2_L1BPH-2M8-2MU4_BPH-0DR15-2MU4", "HLT_2mu4_bDimu_novtx_noos_noL2_L1BPH-7M15-2MU4_BPH-0DR24-2MU4", "HLT_2mu4_bDimu_novtx_noos_noL2_L1BPH-7M15-2MU4_BPH-0DR24-2MU4-B", "HLT_2mu4_bDimu_novtx_noos_noL2_L1BPH-7M15-2MU4_BPH-0DR24-2MU4-BO", "HLT_2mu4_bDimu_tightChi2", "HLT_2mu4_bJpsimumu_L12CMU4", "HLT_2mu4_bJpsimumu_L12MU4-B", "HLT_2mu4_bJpsimumu_L1BPH-1M19-2MU4-BO_BPH-0DR34-2MU4", "HLT_2mu4_bJpsimumu_L1BPH-1M19-2MU4-B_BPH-0DR34-2MU4", "HLT_2mu4_bJpsimumu_L1BPH-1M19-2MU4_BPH-0DR34-2MU4", "HLT_2mu4_bJpsimumu_L1BPH-2M8-2MU4",
#"HLT_2mu4_bJpsimumu_L1BPH-2M8-2MU4-BO_BPH-0DR15-2MU4",
#"HLT_2mu4_bJpsimumu_L1BPH-2M8-2MU4-B_BPH-0DR15-2MU4",
"HLT_2mu4_bJpsimumu_Lxy0", "HLT_2mu4_bJpsimumu_noEFbph", "HLT_2mu4_bJpsimumu_noL2", "HLT_2mu4_bJpsimumu_noL2_L12CMU4", "HLT_2mu4_bJpsimumu_noL2_L12MU4-B", "HLT_2mu4_bJpsimumu_noL2_L1BPH-1M19-2MU4-BO_BPH-0DR34-2MU4", "HLT_2mu4_bJpsimumu_noL2_L1BPH-1M19-2MU4-B_BPH-0DR34-2MU4", "HLT_2mu4_bJpsimumu_noL2_L1BPH-1M19-2MU4_BPH-0DR34-2MU4", "HLT_2mu4_bJpsimumu_noL2_L1BPH-2M8-2MU4", "HLT_2mu4_bJpsimumu_noL2_L1BPH-2M8-2MU4-BO_BPH-0DR15-2MU4", "HLT_2mu4_bJpsimumu_noL2_L1BPH-2M8-2MU4-B_BPH-0DR15-2MU4", "HLT_2mu4_bJpsimumu_noL2_L1BPH-2M8-2MU4_BPH-0DR15-2MU4", "HLT_2mu4_bJpsimumu_tightChi2", "HLT_2mu4_bUpsimumu_L12CMU4", "HLT_2mu4_bUpsimumu_L12MU4-B", "HLT_2mu4_bUpsimumu_L1BPH-1M19-2MU4-BO_BPH-0DR34-2MU4", "HLT_2mu4_bUpsimumu_L1BPH-1M19-2MU4-B_BPH-0DR34-2MU4", "HLT_2mu4_bUpsimumu_L1BPH-1M19-2MU4_BPH-0DR34-2MU4",
#"HLT_2mu4_bUpsimumu_L1BPH-7M15-2MU4_BPH-0DR24-2MU4-B",
#"HLT_2mu4_bUpsimumu_L1BPH-7M15-2MU4_BPH-0DR24-2MU4-BO",
"HLT_2mu4_bUpsimumu_tightChi2", "HLT_2mu4_l2msonly_bDimu_noL2", "HLT_2mu4_l2msonly_bJpsimumu_noL2", "HLT_2mu4_muL2_bBmumuxv3", "HLT_2mu4_wOvlpRm_bBmumuxv2", "HLT_2mu4_wOvlpRm_bDimu", "HLT_2mu4_wOvlpRm_bDimu_noinvm_novtx_ss", "HLT_2mu4_wOvlpRm_bDimu_novtx_noos",
#"HLT_2mu6_bBmumu_L12CMU6",
"HLT_2mu6_bBmumu_L12MU6-B",
#"HLT_2mu6_bBmumu_Lxy0",
"HLT_2mu6_bBmumu_noL2", "HLT_2mu6_bBmumu_noL2_L12CMU6", "HLT_2mu6_bBmumu_noL2_L12MU6-B", "HLT_2mu6_bBmumu_noL2_L1BPH-2M9-2MU6_BPH-2DR15-2MU6", "HLT_2mu6_bBmumu_tightChi2", "HLT_2mu6_bBmumux_BcmumuDsloose_L12CMU6", "HLT_2mu6_bBmumux_BcmumuDsloose_L12MU6-B", "HLT_2mu6_bBmumux_BcmumuDsloose_noL2", "HLT_2mu6_bBmumux_BcmumuDsloose_noL2_L12CMU6", "HLT_2mu6_bBmumux_BcmumuDsloose_noL2_L12MU6-B","HLT_2mu6_bBmumux_BcmumuDsloose_noL2_L1BPH-2M9-2MU6_BPH-2DR15-2MU6", "HLT_2mu6_bBmumux_BpmumuKp_noL2", "HLT_2mu6_bBmumuxv2_L12CMU6", "HLT_2mu6_bBmumuxv2_L12MU6-B", "HLT_2mu6_bBmumuxv2_noL2", "HLT_2mu6_bBmumuxv2_noL2_L12CMU6", "HLT_2mu6_bBmumuxv2_noL2_L12MU6-B", "HLT_2mu6_bBmumuxv2_noL2_L1BPH-2M9-2MU6_BPH-2DR15-2MU6", "HLT_2mu6_bBmumuxv3", "HLT_2mu6_bDimu_L12CMU6", "HLT_2mu6_bDimu_L12MU6-B", "HLT_2mu6_bDimu_L1BPH-2M9-2MU6_BPH-2DR15-2MU6", "HLT_2mu6_bDimu_L1BPH-8M15-2MU6", "HLT_2mu6_bDimu_L1BPH-8M15-2MU6_BPH-0DR22-2MU6", "HLT_2mu6_bDimu_Lxy0", "HLT_2mu6_bDimu_noEFbph", "HLT_2mu6_bDimu_noL2", "HLT_2mu6_bDimu_noL2_L12CMU6", "HLT_2mu6_bDimu_noL2_L12MU6-B", "HLT_2mu6_bDimu_noL2_L1BPH-2M9-2MU6_BPH-2DR15-2MU6", "HLT_2mu6_bDimu_noL2_L1BPH-8M15-2MU6", "HLT_2mu6_bDimu_noL2_L1BPH-8M15-2MU6_BPH-0DR22-2MU6", "HLT_2mu6_bDimu_novtx_noos_L12CMU6", "HLT_2mu6_bDimu_novtx_noos_L12MU6-B", "HLT_2mu6_bDimu_novtx_noos_L1BPH-2M9-2MU6_BPH-2DR15-2MU6", "HLT_2mu6_bDimu_novtx_noos_L1BPH-8M15-2MU6", "HLT_2mu6_bDimu_novtx_noos_L1BPH-8M15-2MU6_BPH-0DR22-2MU6", "HLT_2mu6_bDimu_novtx_noos_noL2", "HLT_2mu6_bDimu_novtx_noos_noL2_L12CMU6", "HLT_2mu6_bDimu_novtx_noos_noL2_L12MU6-B", "HLT_2mu6_bDimu_novtx_noos_noL2_L1BPH-2M9-2MU6_BPH-2DR15-2MU6", "HLT_2mu6_bDimu_novtx_noos_noL2_L1BPH-8M15-2MU6", "HLT_2mu6_bDimu_novtx_noos_noL2_L1BPH-8M15-2MU6_BPH-0DR22-2MU6", "HLT_2mu6_bDimu_tightChi2",
#"HLT_2mu6_bJpsimumu_L12CMU6",
"HLT_2mu6_bJpsimumu_L12MU6-B",
#"HLT_2mu6_bJpsimumu_Lxy0",
"HLT_2mu6_bJpsimumu_noEFbph", "HLT_2mu6_bJpsimumu_noL2", "HLT_2mu6_bJpsimumu_noL2_L12CMU6", "HLT_2mu6_bJpsimumu_noL2_L12MU6-B", "HLT_2mu6_bJpsimumu_noL2_L1BPH-2M9-2MU6_BPH-2DR15-2MU6", "HLT_2mu6_bJpsimumu_tightChi2",
#"HLT_2mu6_bUpsimumu_L12CMU6",
"HLT_2mu6_bUpsimumu_L12MU6-B", "HLT_2mu6_bUpsimumu_L1BPH-8M15-2MU6", "HLT_2mu6_bUpsimumu_tightChi2", "HLT_2mu6_l2msonly_bDimu_noL2", "HLT_2mu6_l2msonly_bJpsimumu_noL2", "HLT_2mu6_l2msonly_mu4_l2msonly_L12MU6_3MU4", "HLT_2mu6_mu4", "HLT_2mu6_mu4_bDimu", "HLT_2mu6_mu4_bJpsi", "HLT_2mu6_mu4_bTau_noL2", "HLT_2mu6_mu4_bUpsi", "HLT_2mu6_muL2_bBmumuxv3", "HLT_3j175_jes_PS", "HLT_3j30", "HLT_3j40", "HLT_3j50", "HLT_3mu4_l2msonly", "HLT_3mu4_noMuCombOvlpRm", "HLT_3mu4_nomucomb", "HLT_3mu6_nomucomb", "HLT_3mu6_wOvlpRm_bDimu", "HLT_4j45_test1", "HLT_4j45_test2", "HLT_e0_L2Star_perf_L1EM15", "HLT_e0_perf_L1EM3_EMPTY", "HLT_e120_lhloose_HLTCalo", "HLT_e120_loose", "HLT_e13_etcut_L1EM12_W-MT25", "HLT_e13_etcut_L1EM12_W-MT30", "HLT_e13_etcut_L1W-NOMATCH", "HLT_e13_etcut_L1W-NOMATCH_W-05RO-XEEMHT", "HLT_e13_etcut_trkcut", "HLT_e13_etcut_trkcut_L1EM12_W-MT25_W-15DPHI-JXE-0_W-15DPHI-EMXE", "HLT_e13_etcut_trkcut_L1EM12_W-MT25_W-15DPHI-JXE-0_W-15DPHI-EMXE_XS20_xe20_mt25", "HLT_e13_etcut_trkcut_j20_perf_xe15_6dphi05_mt25_L1EM12_W-MT25_W-15DPHI-JXE-0_W-15DPHI-EMXE_W-90RO2-XEHT-0", "HLT_e13_etcut_trkcut_j20_perf_xe15_6dphi05_mt25_L1EM12_W-MT25_W-15DPHI-JXE-0_W-15DPHI-EMXE_XS20", "HLT_e13_etcut_trkcut_xe20", "HLT_e13_etcut_trkcut_xe20_L1EM12_W-MT25_W-15DPHI-JXE-0_W-15DPHI-EMXE_XS20", "HLT_e13_etcut_trkcut_xe20_mt25", "HLT_e13_etcut_trkcut_xs15_L1EM12_W-MT25_W-15DPHI-JXE-0_W-15DPHI-EMXE_XS20", "HLT_e13_etcut_trkcut_xs30_j10_perf_xe25_6dphi05_mt35", "HLT_e13_etcut_trkcut_xs30_j10_perf_xe25_6dphi05_mt35_L1EM12_W-MT35_W-250RO2-XEHT-0_W-05DPHI-JXE-0_W-05DPHI-EMXE", "HLT_e13_etcut_trkcut_xs30_j10_perf_xe25_6dphi15_mt35", "HLT_e13_etcut_trkcut_xs30_j10_perf_xe25_6dphi15_mt35_L1EM12_W-MT35_W-90RO2-XEHT-0_W-15DPHI-JXE-0_W-15DPHI-EMXE", "HLT_e13_etcut_trkcut_xs30_j10_perf_xe25_6dphi15_mt35_L1EM12_W-MT35_XS30_W-15DPHI-JXE-0_W-15DPHI-EMXE", "HLT_e13_etcut_trkcut_xs40_j10_perf_xe25_6dphi05_mt35", "HLT_e13_etcut_trkcut_xs40_j10_perf_xe25_6dphi05_mt35_L1EM12_W-MT35_XS40_W-05DPHI-JXE-0_W-05DPHI-EMXE", "HLT_e13_etcut_trkcut_xs50_xe30_mt35", "HLT_e13_etcut_trkcut_xs50_xe30_mt35_L1EM10VH_W-MT35_XS50", "HLT_e13_etcut_trkcut_xs50_xe30_mt35_L1EM12_W-MT35_XS50", "HLT_e140_lhloose_HLTCalo", "HLT_e14_etcut_e5_lhtight_Jpsiee_L1JPSI-1M5-EM12", "HLT_e14_etcut_e5_lhtight_nod0_Jpsiee_L1JPSI-1M5-EM12", "HLT_e14_etcut_e5_tight_Jpsiee_L1JPSI-1M5-EM12", "HLT_e14_lhtight_e4_etcut_Jpsiee_L1JPSI-1M5-EM12", "HLT_e14_lhtight_nod0_e4_etcut_Jpsiee_L1JPSI-1M5-EM12", "HLT_e14_tight_e4_etcut_Jpsiee_L1JPSI-1M5-EM12", "HLT_e17_etcut_L1EM15", "HLT_e17_etcut_L2StarA_L1EM15", "HLT_e17_etcut_L2StarB_L1EM15", "HLT_e17_etcut_L2StarC_L1EM15", "HLT_e17_etcut_L2Star_idperf_L1EM15", "HLT_e17_etcut_idperf_L1EM15", "HLT_e17_lhloose_L1EM15VHJJ1523ETA49", "HLT_e17_lhmedium_iloose_2e9_lhmedium", "HLT_e17_lhmedium_iloose_tau25_medium1_tracktwo_L1DR-EM15TAU12I-J25", "HLT_e17_lhmedium_iloose_tau25_medium1_tracktwo_L1EM15TAU12I-J25", "HLT_e17_lhmedium_iloose_tau25_perf_ptonly_L1EM15HI_2TAU12IM", "HLT_e17_lhmedium_ivarloose_tau25_medium1_tracktwo", "HLT_e17_lhmedium_ivarloose_tau25_medium1_tracktwo_L1EM15TAU12I-J25", "HLT_e17_lhmedium_ivarloose_tau25_perf_ptonly_L1EM15HI_2TAU12IM", "HLT_e17_lhmedium_nod0_iloose_2e9_lhmedium_nod0", "HLT_e17_lhmedium_nod0_iloose_tau25_medium1_tracktwo_L1DR-EM15TAU12I", "HLT_e17_lhmedium_nod0_iloose_tau25_medium1_tracktwo_L1DR-EM15TAU12I-J25", "HLT_e17_lhmedium_nod0_iloose_tau25_medium1_tracktwo_L1EM15HI_2TAU12IM", "HLT_e17_lhmedium_nod0_ivarloose", "HLT_e17_lhmedium_nod0_ivarloose_2e9_lhmedium_nod0", "HLT_e17_lhmedium_nod0_ivarloose_tau25_medium1_tracktwo", "HLT_e17_lhmedium_nod0_ivarloose_tau25_medium1_tracktwo_L1DR-EM15TAU12I", "HLT_e17_lhmedium_nod0_ivarloose_tau25_medium1_tracktwo_L1DR-EM15TAU12I-J25", "HLT_e17_lhmedium_nod0_ivarloose_tau25_medium1_tracktwo_L1EM15HI_2TAU12IM", "HLT_e17_lhmedium_nod0_ivarloose_tau25_medium1_tracktwo_xe50", "HLT_e17_lhmedium_nod0_tau25_medium1_tracktwo_L1EM15-TAU12I", "HLT_e17_lhmedium_nod0_tau25_medium1_tracktwo_L1EM15HI_2TAU12IM", "HLT_e17_lhmedium_nod0_tau25_medium1_tracktwo_xe50_L1XE35_EM15-TAU12I", "HLT_e17_lhmedium_nod0_tau25_medium1_tracktwo_xe50_L1XE40_EM15-TAU12I", "HLT_e17_lhmedium_nod0_tau80_medium1_tracktwo_L1EM15-TAU40", "HLT_e17_lhmedium_tau25_medium1_tracktwo_L1EM15-TAU12I", "HLT_e17_lhmedium_tau25_medium1_tracktwo_L1EM15HI_2TAU12IM", "HLT_e17_lhmedium_tau25_medium1_tracktwo_xe50_L1XE35_EM15-TAU12I", "HLT_e17_lhmedium_tau25_medium1_tracktwo_xe50_L1XE40_EM15-TAU12I", "HLT_e17_lhmedium_tau80_medium1_tracktwo_L1EM15-TAU40", "HLT_e17_loose_2e9_loose", "HLT_e17_loose_L1EM15VHJJ1523ETA49", "HLT_e17_medium_2e9_medium", "HLT_e17_medium_iloose_2e9_medium", "HLT_e17_medium_iloose_tau25_medium1_tracktwo_L1DR-EM15TAU12I-J25", "HLT_e17_medium_iloose_tau25_medium1_tracktwo_L1EM15TAU12I-J25", "HLT_e17_medium_iloose_tau25_perf_ptonly_L1EM15HI_2TAU12IM", "HLT_e17_medium_ivarloose_tau25_medium1_tracktwo", "HLT_e17_medium_ivarloose_tau25_medium1_tracktwo_L1EM15TAU12I-J25", "HLT_e17_medium_ivarloose_tau25_perf_ptonly_L1EM15HI_2TAU12IM", "HLT_e17_medium_tau25_medium1_tracktwo_L1DR-EM15TAU12I", "HLT_e17_medium_tau25_medium1_tracktwo_L1DR-EM15TAU12I-J25", "HLT_e17_medium_tau25_medium1_tracktwo_L1EM15-TAU12I", "HLT_e17_medium_tau25_medium1_tracktwo_L1EM15HI_2TAU12IM", "HLT_e17_medium_tau25_medium1_tracktwo_L1EM15TAU12I-J25", "HLT_e17_medium_tau25_medium1_tracktwo_xe50_L1XE35_EM15-TAU12I", "HLT_e17_medium_tau25_medium1_tracktwo_xe50_L1XE40_EM15-TAU12I", "HLT_e17_medium_tau80_medium1_tracktwo_L1EM15-TAU40", "HLT_e18_etcut_L1EM15_W-MT35", "HLT_e18_etcut_trkcut", "HLT_e18_etcut_trkcut_L1EM15_W-MT35_W-05DPHI-JXE-0_W-05DPHI-EM15XE", "HLT_e18_etcut_trkcut_L1EM15_W-MT35_W-05DPHI-JXE-0_W-05DPHI-EM15XE_XS30_xe35_mt35", "HLT_e18_etcut_trkcut_j20_perf_xe20_6dphi15_mt35_L1EM15_W-MT35_W-05DPHI-JXE-0_W-05DPHI-EM15XE_XS30", "HLT_e18_etcut_trkcut_j20_perf_xe20_6dphi15_mt35_L1EM15_W-MT35_W-250RO2-XEHT-0_W-05DPHI-JXE-0_W-05DPHI-EM15XE", "HLT_e18_etcut_trkcut_xe35", "HLT_e18_etcut_trkcut_xe35_L1EM15_W-MT35_W-05DPHI-JXE-0_W-05DPHI-EM15XE_XS30", "HLT_e18_etcut_trkcut_xe35_mt35", "HLT_e18_etcut_trkcut_xs50_j10_perf_xe30_6dphi05_mt35", "HLT_e18_etcut_trkcut_xs50_j10_perf_xe30_6dphi05_mt35_L1EM15_W-MT35_W-250RO2-XEHT-0_W-05DPHI-JXE-0_W-05DPHI-EM15XE", "HLT_e18_etcut_trkcut_xs50_j10_perf_xe30_6dphi05_mt35_L1EM15_W-MT35_XS40_W-05DPHI-JXE-0_W-05DPHI-EM15XE", "HLT_e18_etcut_trkcut_xs50_j10_perf_xe30_6dphi15_mt35", "HLT_e18_etcut_trkcut_xs50_j10_perf_xe30_6dphi15_mt35_L1EM15_W-MT35_W-250RO2-XEHT-0_W-15DPHI-JXE-0_W-15DPHI-EM15XE", "HLT_e18_etcut_trkcut_xs50_j10_perf_xe30_6dphi15_mt35_L1EM15_W-MT35_XS40_W-15DPHI-JXE-0_W-15DPHI-EM15XE", "HLT_e18_etcut_trkcut_xs60_j10_perf_xe30_6dphi05_mt35", "HLT_e18_etcut_trkcut_xs60_j10_perf_xe30_6dphi05_mt35_L1EM15_W-MT35_XS60_W-05DPHI-JXE-0_W-05DPHI-EM15XE", "HLT_e18_etcut_trkcut_xs60_j10_perf_xe30_6dphi15_mt35", "HLT_e18_etcut_trkcut_xs60_j10_perf_xe30_6dphi15_mt35_L1EM15_W-MT35_XS60_W-15DPHI-JXE-0_W-15DPHI-EM15XE", "HLT_e18_etcut_trkcut_xs60_xe30_mt35", "HLT_e18_etcut_trkcut_xs60_xe30_mt35_L1EM15VH_W-MT35_XS60", "HLT_e18_etcut_trkcut_xs60_xe30_mt35_L1EM15_W-MT35_XS60", "HLT_e24_etcut_trkcut_xs40_j10_perf_xe35_2dphi05_mt35", "HLT_e24_etcut_trkcut_xs60_xe30_mt35", "HLT_e24_etcut_trkcut_xs60_xe30_mt35_L1EM20VH_W-MT35_XS60", "HLT_e24_lhmedium_L1EM20VHI", "HLT_e24_lhmedium_L2Star_idperf_L1EM20VH", "HLT_e24_lhmedium_cutd0dphideta_L1EM18VH", "HLT_e24_lhmedium_cutd0dphideta_iloose_L1EM18VH", "HLT_e24_lhmedium_e9_lhmedium", "HLT_e24_lhmedium_iloose_HLTCalo_L1EM18VH", "HLT_e24_lhmedium_iloose_HLTCalo_L1EM20VH", "HLT_e24_lhmedium_iloose_L2StarA_L1EM20VH", "HLT_e24_lhmedium_nod0_ringer_L1EM20VH", "HLT_e24_lhmedium_nodeta_L1EM18VH", "HLT_e24_lhmedium_nodeta_iloose_L1EM18VH", "HLT_e24_lhmedium_nodphires_L1EM18VH", "HLT_e24_lhmedium_nodphires_iloose_L1EM18VH", "HLT_e24_lhtight_cutd0dphideta_iloose_L1EM20VH", "HLT_e24_lhtight_iloose_HLTCalo", "HLT_e24_lhtight_iloose_HLTCalo_L1EM20VH", "HLT_e24_lhtight_iloose_L1EM20VH", "HLT_e24_lhtight_iloose_L2EFCalo_L1EM20VH", "HLT_e24_lhtight_nod0_iloose_L1EM20VH", "HLT_e24_lhtight_nod0_ivarloose_L1EM20VH", "HLT_e24_lhtight_nod0_ringer_iloose", "HLT_e24_lhtight_nod0_ringer_ivarloose", "HLT_e24_lhtight_smooth_L1EM20VH", "HLT_e24_medium_L1EM18VH", "HLT_e24_medium_L1EM20VH", "HLT_e24_medium_L1EM20VHI", "HLT_e24_medium_L2Star_idperf_L1EM20VH", "HLT_e24_medium_iloose_L1EM18VH", "HLT_e24_medium_iloose_L2StarA_L1EM20VH", "HLT_e24_tight_L1EM20VH", "HLT_e24_tight_iloose_HLTCalo_L1EM20VH", "HLT_e24_tight_iloose_L1EM20VH", "HLT_e24_tight_iloose_L2EFCalo_L1EM20VH", "HLT_e24_tight_iloose_etisem_L1EM20VH", "HLT_e26_etcut_trkcut_xs40_xe30_mt35", "HLT_e26_etcut_trkcut_xs40_xe30_mt35_L1EM22VHI_W-MT35_XS40", "HLT_e26_lhtight", "HLT_e26_lhtight_iloose_2j35_boffperf", "HLT_e26_lhtight_iloose_2j35_bperf", "HLT_e26_lhtight_iloose_2j35_bperf_split", "HLT_e26_lhtight_iloose_HLTCalo", "HLT_e26_lhtight_nod0", "HLT_e26_lhtight_nod0_iloose_2j35_bperf", "HLT_e26_lhtight_nod0_ivarloose_2j35_bperf", "HLT_e26_lhtight_smooth", "HLT_e26_tight_iloose_2j35_boffperf", "HLT_e26_tight_iloose_2j35_boffperf_split", "HLT_e26_tight_iloose_2j35_bperf", "HLT_e26_tight_iloose_2j35_bperf_split", "HLT_e28_lhtight", "HLT_e28_lhtight_nod0", "HLT_e28_lhtight_smooth", "HLT_e40_etcut_trkcut_xs40_xe30", "HLT_e5_etcut_L1W-05DPHI-EMXE-1", "HLT_e5_etcut_L1W-05DPHI-JXE-0", "HLT_e5_etcut_L1W-05RO-XEHT-0", "HLT_e5_etcut_L1W-10DPHI-EMXE-0", "HLT_e5_etcut_L1W-10DPHI-JXE-0", "HLT_e5_etcut_L1W-15DPHI-EMXE-0", "HLT_e5_etcut_L1W-15DPHI-JXE-0", "HLT_e5_etcut_L1W-250RO2-XEHT-0", "HLT_e5_etcut_L1W-90RO2-XEHT-0", "HLT_e5_etcut_L1W-HT20-JJ15.ETA49", "HLT_e5_lhloose_L2StarA", "HLT_e5_lhloose_L2Star_idperf", "HLT_e5_lhtight_e4_etcut_Jpsiee_L1JPSI-1M5", "HLT_e5_lhtight_e4_etcut_L1JPSI-1M5", "HLT_e5_lhtight_nod0_e4_etcut_Jpsiee_L1JPSI-1M5", "HLT_e5_lhtight_nod0_e4_etcut_L1JPSI-1M5", "HLT_e5_loose_L2StarA", "HLT_e5_loose_L2Star_idperf", "HLT_e5_tight_e4_etcut_Jpsiee_L1JPSI-1M5", "HLT_e5_tight_e4_etcut_L1JPSI-1M5", "HLT_e60_lhloose_L1EM24VHI", "HLT_e60_lhloose_L1EM24VHI_xe60noL1", "HLT_e60_lhloose_nod0_L1EM24VHI", "HLT_e60_lhloose_nod0_L1EM24VHI_xe60noL1", "HLT_e60_lhmedium_HLTCalo", "HLT_e70_lhloose_L1EM24VHI", "HLT_e70_lhloose_L1EM24VHI_xe70noL1", "HLT_e70_lhloose_nod0_L1EM24VHI", "HLT_e70_lhloose_nod0_L1EM24VHI_xe70noL1", "HLT_e9_etcut_e5_lhtight_Jpsiee_L1JPSI-1M5-EM7", "HLT_e9_etcut_e5_lhtight_nod0_Jpsiee_L1JPSI-1M5-EM7", "HLT_e9_etcut_e5_tight_Jpsiee_L1JPSI-1M5-EM7", "HLT_e9_lhtight_e4_etcut_Jpsiee_L1JPSI-1M5-EM7", "HLT_e9_lhtight_nod0_e4_etcut_Jpsiee_L1JPSI-1M5-EM7", "HLT_e9_tight_e4_etcut_Jpsiee_L1JPSI-1M5-EM7", "HLT_eb_high_L1RD2_FILLED", "HLT_eb_low_L1RD2_FILLED", "HLT_g0_perf_L1EM3_EMPTY", "HLT_g10_etcut_mu10_L1LFV-EM8I", "HLT_g10_etcut_mu10_iloose_taumass_L1LFV-EM8I", "HLT_g10_etcut_mu10_taumass", "HLT_g10_loose_mu10_iloose_taumass_L1LFV-EM8I", "HLT_g140_loose_HLTCalo", "HLT_g140_loose_L1EM24VHI", "HLT_g15_loose_2j40_0eta490_3j25_0eta490", "HLT_g20_etcut_mu4_L1LFV-EM15I", "HLT_g20_etcut_mu4_iloose_taumass_L1LFV-EM15I", "HLT_g20_etcut_mu4_taumass", "HLT_g20_loose_2j40_0eta490_3j25_0eta490", "HLT_g20_loose_2j40_0eta490_3j25_0eta490_L1MJJ-700", "HLT_g20_loose_2j40_0eta490_3j25_0eta490_L1MJJ-900", "HLT_g20_loose_mu4_iloose_taumass_L1LFV-EM15I", "HLT_g25_medium_L1EM22VHI_2j35_0eta490_bmv2c2077_split_2j35_0eta490", "HLT_g25_medium_L1EM22VHI_2j35_0eta490_boffperf_split_2j35_0eta490", "HLT_g25_medium_L1EM22VHI_j35_0eta490_bmv2c2077_split_3j35_0eta490_invm700", "HLT_g25_medium_L1EM22VHI_j35_0eta490_boffperf_split_3j35_0eta490_invm700", "HLT_g30_loose", "HLT_g30_loose_L1EM15", "HLT_g35_medium_HLTCalo_g25_medium_HLTCalo", "HLT_g35_medium_tau25_perf_tracktwo_L1TAU12", "HLT_g45_tight_3j50noL1", "HLT_g75_tight_3j25noL1", "HLT_g75_tight_3j25noL1_L1EM20VHI", "HLT_g75_tight_3j25noL1_L1EM22VHI", "HLT_ht0_L1J12_EMPTY", "HLT_ht1000", "HLT_ht1000_L1HT190-J15.ETA21", "HLT_ht400", "HLT_ht400_L1HT150-J20.ETA31", "HLT_ht550", "HLT_ht550_L1HT150-J20.ETA31", "HLT_ht700", "HLT_ht700_L1HT190-J15.ETA21", "HLT_ht850", "HLT_ht850_L1HT190-J15.ETA21", "HLT_j0_0i1c200m400TLA", "HLT_j0_0i1c400m600TLA", "HLT_j0_0i1c500m900TLA", "HLT_j0_0i1c600m800TLA", "HLT_j0_0i1c800m1000TLA", "HLT_j0_1i2c100m300TLA", "HLT_j0_1i2c100m8000TLA", "HLT_j0_1i2c200m8000TLA", "HLT_j0_1i2c300m500TLA", "HLT_j0_1i2c500m700TLA", "HLT_j0_1i2c500m900TLA", "HLT_j0_1i2c600m800TLA", "HLT_j0_L1J12_EMPTY", "HLT_j0_lcw_jes_L1J12", "HLT_j0_perf_bperf_L1J12_EMPTY", "HLT_j0_perf_bperf_L1MU10", "HLT_j0_perf_bperf_L1RD0_EMPTY", "HLT_j0_perf_ds1_L1All", "HLT_j10", "HLT_j100_2j30", "HLT_j100_2j40", "HLT_j100_2j50", "HLT_j100_2j55_bmedium_split", "HLT_j100_2j55_bmv2c2077", "HLT_j100_2j55_boffperf", "HLT_j100_2j55_boffperf_split", "HLT_j100_2j55_bperf", "HLT_j100_2j55_bperf_split", "HLT_j100_xe80_L1J40_DPHI-CJ20XE50", "HLT_j100_xe80_L1J40_DPHI-J20XE50", "HLT_j100_xe80_L1J40_DPHI-J20s2XE50", "HLT_j100_xe80_L1J40_DPHI-Js2XE50", "HLT_j10_320eta490", "HLT_j110_bperf", "HLT_j120_xe80", "HLT_j150_2j50", "HLT_j150_boffperf_j50_boffperf", "HLT_j150_bperf_j50_bperf", "HLT_j150_bperf_split_j50_bperf_split", "HLT_j150_xe80", "HLT_j15_bperf", "HLT_j175_bmedium_split_j60_bmedium_split", "HLT_j175_bmv2c2085", "HLT_j175_bmv2c2085_split", "HLT_j175_boffperf", "HLT_j175_boffperf_j60_boffperf", "HLT_j175_boffperf_split_j60_boffperf_split", "HLT_j175_bperf", "HLT_j175_bperf_j60_bperf", "HLT_j175_bperf_split", "HLT_j175_bperf_split_j60_bperf_split", "HLT_j175_jes", "HLT_j175_lcw", "HLT_j175_lcw_jes", "HLT_j175_lcw_nojcalib", "HLT_j175_nojcalib", "HLT_j20", "HLT_j200_jes_PS", "HLT_j225_bloose_split", "HLT_j225_boffperf", "HLT_j225_bperf", "HLT_j225_bperf_split", "HLT_j25_320eta490_L1TE5", "HLT_j260_a10_lcw_sub_L1J75", "HLT_j260_a10_lcw_sub_L1SC85", "HLT_j260_a10_sub_L1J75", "HLT_j260_a10r_L1J75", "HLT_j30", "HLT_j300_a10_lcw_sub_L1J75", "HLT_j300_a10_lcw_sub_L1SC85", "HLT_j300_a10_sub_L1J75", "HLT_j300_a10r_L1J75", "HLT_j300_bloose_split", "HLT_j300_boffperf", "HLT_j300_boffperf_split", "HLT_j300_bperf", "HLT_j300_bperf_split", "HLT_j300_lcw_nojcalib", "HLT_j30_0eta490_cleanT_xe100_tc_lcw_L1XE70", "HLT_j30_0eta490_cleanT_xe80_L1XE60", "HLT_j30_L1TE10", "HLT_j30_L1TE5", "HLT_j30_jes_cleanLLP_PS_llp_L1LLP-NOMATCH", "HLT_j30_jes_cleanLLP_PS_llp_noiso_L1LLP-NOMATCH", "HLT_j35_320eta490_L1TE10", "HLT_j35_boffperf_3j35", "HLT_j35_bperf_3j35", "HLT_j360_a10_lcw_nojcalib", "HLT_j360_a10_lcw_nojcalib_L1HT150-J20.ETA31", "HLT_j360_a10_lcw_sub", "HLT_j360_a10_lcw_sub_L1HT150-J20.ETA31", "HLT_j360_a10_lcw_sub_L1SC111", "HLT_j360_a10_nojcalib", "HLT_j360_a10_nojcalib_L1HT150-J20.ETA31", "HLT_j360_a10_sub", "HLT_j360_a10_sub_L1HT150-J20.ETA31", "HLT_j360_a10r", "HLT_j40", "HLT_j400_a10_lcw_sub_L1J100", "HLT_j400_a10_lcw_sub_L1SC111", "HLT_j400_a10_sub_L1J100", "HLT_j400_a10r_L1J100", "HLT_j40_0eta490_j30_0eta490_deta25_xe60", "HLT_j40_0eta490_j30_0eta490_deta25_xe80", "HLT_j40_0eta490_j30_0eta490_deta25_xe80_tc_lcw", "HLT_j40_0eta490_xe80_1dphi10_L1DPHI-AJ20s2XE50", "HLT_j40_L1J12", "HLT_j40_L1TE10", "HLT_j40_L1TE20", "HLT_j40_cleanT_xe75_L1XE60", "HLT_j40_cleanT_xe80_L1XE60", "HLT_j420_a10_lcw_sub_L1J100", "HLT_j420_a10_lcw_sub_L1SC111", "HLT_j420_a10_sub_L1J100", "HLT_j420_a10r_L1J100", "HLT_j45_320eta490_L1TE20", "HLT_j460_a10_lcw_nojcalib", "HLT_j460_a10_lcw_nojcalib_L1HT190-J15.ETA21", "HLT_j460_a10_lcw_sub", "HLT_j460_a10_lcw_sub_L1HT190-J15.ETA21", "HLT_j460_a10_lcw_sub_L1SC111", "HLT_j460_a10_nojcalib", "HLT_j460_a10_nojcalib_L1HT190-J15.ETA21", "HLT_j460_a10_sub", "HLT_j460_a10_sub_L1HT190-J15.ETA21", "HLT_j460_a10r", "HLT_j50_0eta490_j30_0eta490_deta30_xe80_L1J30.0ETA49_XE50", "HLT_j50_0eta490_j30_0eta490_deta30_xe80_L1J40_XE50", "HLT_j50_L1J12", "HLT_j50_L1J15", "HLT_j50_L1TE20", "HLT_j55_320eta490_L1TE20", "HLT_j55_bmv2c2070_ht500_L14J20", "HLT_j55_bmv2c2070_split_ht500_L14J20", "HLT_j55_bmv2c2077_split_ht500_L14J20", "HLT_j55_boffperf_ht500_L14J20", "HLT_j55_boffperf_split_ht500_L14J20", "HLT_j55_boffperf_split_ht500_L1J100", "HLT_j55_bperf", "HLT_j55_bperf_ht500_L14J20", "HLT_j55_bperf_split_ht500_L14J20", "HLT_j55_btight_ht500_L14J20", "HLT_j60_L1J15", "HLT_j60_TT", "HLT_j60_bmv2c2077_split_j60_L12J40_XE45", "HLT_j60_boffperf_j60_L12J40_XE45", "HLT_j60_boffperf_split_j60_L12J40_XE45", "HLT_j60_cleanT_xe75_L1J40_XE50", "HLT_j60_cleanT_xe80_L1J40_XE50", "HLT_j65_0eta490_j40_0eta490_invm250_xe80", "HLT_j65_0eta490_j40_0eta490_invm250_xe80_L1XE70", "HLT_j65_boffperf_3j65_L13J25.0ETA23", "HLT_j65_boffperf_3j65_L14J15.0ETA25", "HLT_j65_bperf_3j65_L13J25.0ETA23", "HLT_j65_bperf_3j65_L14J15.0ETA25", "HLT_j65_bperf_split_3j65_L13J25.0ETA23", "HLT_j65_bperf_split_3j65_L14J15.0ETA25", "HLT_j70_boffperf_3j70", "HLT_j70_boffperf_3j70_L13J25.0ETA23", "HLT_j70_boffperf_3j70_L14J15.0ETA25", "HLT_j70_boffperf_split_3j70", "HLT_j70_boffperf_split_3j70_L13J25.0ETA23", "HLT_j70_bperf_3j70", "HLT_j70_bperf_3j70_L13J25.0ETA23", "HLT_j70_bperf_3j70_L14J15.0ETA25", "HLT_j70_bperf_split_3j70", "HLT_j70_bperf_split_3j70_L13J25.0ETA23", "HLT_j70_bperf_split_3j70_L14J15.0ETA25", "HLT_j75_2j30", "HLT_j75_2j40", "HLT_j75_L1J20", "HLT_j75_boffperf_3j75", "HLT_j75_boffperf_3j75_L13J25.0ETA23", "HLT_j75_boffperf_split_3j75", "HLT_j75_boffperf_split_3j75_L13J25.0ETA23", "HLT_j75_bperf_3j75", "HLT_j75_bperf_3j75_L13J25.0ETA23", "HLT_j75_bperf_split_3j75", "HLT_j75_bperf_split_3j75_L13J25.0ETA23", "HLT_j80_bmv2c2077_split_L12J50_XE40", "HLT_j80_bmv2c2077_split_L1J400ETA25_XE50", "HLT_j80_bmv2c2077_split_L1J40_XE50", "HLT_j80_bmv2c2077_split_L1J40_XE60", "HLT_j80_bmv2c2077_split_L1XE60", "HLT_j80_boffperf_L12J50_XE40", "HLT_j80_boffperf_L1J400ETA25_XE50", "HLT_j80_boffperf_L1J40_XE50", "HLT_j80_boffperf_L1J40_XE60", "HLT_j80_boffperf_L1XE60", "HLT_j80_boffperf_split_L12J50_XE40", "HLT_j80_boffperf_split_L1J400ETA25_XE50", "HLT_j80_boffperf_split_L1J40_XE50", "HLT_j80_boffperf_split_L1J40_XE60", "HLT_j80_boffperf_split_L1XE60", "HLT_j80_xe100", "HLT_j80_xe80", "HLT_j80_xe80_1dphi10_L1J40_DPHI-CJ20XE50", "HLT_j80_xe80_1dphi10_L1J40_DPHI-J20XE50", "HLT_j80_xe80_1dphi10_L1J40_DPHI-J20s2XE50", "HLT_j80_xe80_1dphi10_L1J40_DPHI-Js2XE50", "HLT_j85_2j45", "HLT_j85_jes_test1", "HLT_j85_jes_test2", "HLT_j85_lcw", "HLT_j85_lcw_jes", "HLT_j85_lcw_nojcalib", "HLT_j85_nojcalib", "HLT_j85_test1", "HLT_j85_test1_2j45_test1", "HLT_j85_test2", "HLT_j85_test2_2j45_test2", "HLT_l1calooverflow", "HLT_mb_sp1200_pusup700_trk70_hmt_L1TE5", "HLT_mb_sp1300_hmtperf_L1TE30", "HLT_mb_sp1300_hmtperf_L1TE40", "HLT_mb_sp1300_hmtperf_L1TE40.0ETA24", "HLT_mb_sp1400_pusup550_trk90_hmt_L1TE30", "HLT_mb_sp1400_pusup550_trk90_hmt_L1TE40", "HLT_mb_sp1400_pusup550_trk90_hmt_L1TE40.0ETA24", "HLT_mb_sp1400_pusup550_trk90_hmt_L1TE50", "HLT_mb_sp1400_pusup800_trk80_hmt_L1TE5", "HLT_mb_sp1400_trk100_hmt_L1TE40", "HLT_mb_sp1400_trk100_hmt_L1TE40.0ETA24", "HLT_mb_sp1400_trk90_hmt_L1TE30", "HLT_mb_sp1400_trk90_hmt_L1TE40", "HLT_mb_sp1400_trk90_hmt_L1TE40.0ETA24", "HLT_mb_sp1500_hmtperf_L1TE30", "HLT_mb_sp1500_hmtperf_L1TE40", "HLT_mb_sp1500_hmtperf_L1TE40.0ETA24", "HLT_mb_sp1500_hmtperf_L1TE50", "HLT_mb_sp1500_hmtperf_L1TE60", "HLT_mb_sp1500_pusup700_trk100_hmt_L1TE30", "HLT_mb_sp1500_pusup700_trk100_hmt_L1TE40", "HLT_mb_sp1500_pusup700_trk100_hmt_L1TE40.0ETA24", "HLT_mb_sp1500_pusup700_trk100_hmt_L1TE50", "HLT_mb_sp1500_trk100_hmt_L1TE30", "HLT_mb_sp1500_trk100_hmt_L1TE40", "HLT_mb_sp1500_trk100_hmt_L1TE40.0ETA24", "HLT_mb_sp1600_pusup1100_trk90_hmt_L1TE10", "HLT_mb_sp1600_pusup1100_trk90_hmt_L1TE20", "HLT_mb_sp1600_pusup1100_trk90_hmt_L1TE30", "HLT_mb_sp1600_pusup1100_trk90_hmt_L1TE40", "HLT_mb_sp1600_pusup1100_trk90_hmt_L1TE50", "HLT_mb_sp1600_pusup1100_trk90_hmt_L1TE60", "HLT_mb_sp1700_hmtperf_L1TE30", "HLT_mb_sp1700_hmtperf_L1TE40", "HLT_mb_sp1700_hmtperf_L1TE40.0ETA24", "HLT_mb_sp1800_hmtperf_L1TE40", "HLT_mb_sp1800_pusup800_trk110_hmt_L1TE30", "HLT_mb_sp1800_pusup800_trk110_hmt_L1TE40", "HLT_mb_sp1800_pusup800_trk110_hmt_L1TE40.0ETA24", "HLT_mb_sp1800_pusup800_trk110_hmt_L1TE50", "HLT_mb_sp1800_trk110_hmt_L1TE30", "HLT_mb_sp1800_trk110_hmt_L1TE40", "HLT_mb_sp1800_trk110_hmt_L1TE40.0ETA24", "HLT_mb_sp2000_pusup1200_trk100_hmt_L1TE10", "HLT_mb_sp2000_pusup1200_trk100_hmt_L1TE20", "HLT_mb_sp2000_pusup1200_trk100_hmt_L1TE30", "HLT_mb_sp2000_pusup1200_trk100_hmt_L1TE40", "HLT_mb_sp2000_pusup1200_trk100_hmt_L1TE50", "HLT_mb_sp2000_pusup1200_trk100_hmt_L1TE60", "HLT_mb_sp2000_pusup600_trk70_hmt_L1TE60", "HLT_mb_sp2000_pusup600_trk70_hmt_L1TE70", "HLT_mb_sp2000_pusup700_trk50_sumet110_hmt_L1TE70", "HLT_mb_sp2000_pusup700_trk50_sumet70_hmt_L1TE60", "HLT_mb_sp2000_pusup700_trk60_hmt_L1TE60", "HLT_mb_sp2000_pusup700_trk70_hmt_L1TE60", "HLT_mb_sp2000_pusup700_trk70_hmt_L1TE70", "HLT_mb_sp2100_hmtperf_L1TE10", "HLT_mb_sp2100_hmtperf_L1TE20", "HLT_mb_sp2100_hmtperf_L1TE30", "HLT_mb_sp2100_hmtperf_L1TE40", "HLT_mb_sp2100_hmtperf_L1TE50", "HLT_mb_sp2100_hmtperf_L1TE60", "HLT_mb_sp2100_pusup1000_trk120_hmt_L1TE30", "HLT_mb_sp2100_pusup1000_trk120_hmt_L1TE40", "HLT_mb_sp2100_pusup1000_trk120_hmt_L1TE50", "HLT_mb_sp2200_pusup1300_trk110_hmt_L1TE20", "HLT_mb_sp2200_pusup1300_trk110_hmt_L1TE30", "HLT_mb_sp2200_pusup1300_trk110_hmt_L1TE40", "HLT_mb_sp2200_pusup1300_trk110_hmt_L1TE50", "HLT_mb_sp2200_pusup1300_trk110_hmt_L1TE60", "HLT_mb_sp2400_pusup1100_trk130_hmt_L1TE30", "HLT_mb_sp2400_pusup1100_trk130_hmt_L1TE40", "HLT_mb_sp2400_pusup1100_trk130_hmt_L1TE50", "HLT_mb_sp2400_pusup1400_trk120_hmt_L1TE20", "HLT_mb_sp2400_pusup1400_trk120_hmt_L1TE30", "HLT_mb_sp2400_pusup1400_trk120_hmt_L1TE40", "HLT_mb_sp2400_pusup1400_trk120_hmt_L1TE50", "HLT_mb_sp2400_pusup1400_trk120_hmt_L1TE60", "HLT_mb_sp2500_hmtperf_L1TE40", "HLT_mb_sp2500_pusup750_trk90_hmt_L1TE70", "HLT_mb_sp2700_pusup1300_trk140_hmt_L1TE30", "HLT_mb_sp2700_pusup1300_trk140_hmt_L1TE40", "HLT_mb_sp2700_pusup1300_trk140_hmt_L1TE50", "HLT_mb_sp2_hmtperf_L1TE5", "HLT_mb_sp2_hmtperf_L1TE5.0ETA24", "HLT_mb_sp3000_pusup1400_trk150_hmt_L1TE40", "HLT_mb_sp3000_pusup1400_trk150_hmt_L1TE50", "HLT_mb_sp3000_pusup1400_trk150_hmt_L1TE60", "HLT_mb_sp3000_pusup1500_trk160_hmt_L1TE40", "HLT_mb_sp3000_pusup1500_trk160_hmt_L1TE50", "HLT_mb_sp3000_pusup1500_trk160_hmt_L1TE60", "HLT_mb_sp300_trk10_sumet50_hmt_L1TE20", "HLT_mb_sp300_trk10_sumet60_hmt_L1TE40", "HLT_mb_sp600_trk10_sumet50_hmt_L1TE20.0ETA24", "HLT_mb_sp600_trk10_sumet60_hmt_L1TE40.0ETA24", "HLT_mb_sp700_hmtperf_L1TE5", "HLT_mb_sp800_pusup400_trk50_hmt_L1TE5", "HLT_mb_sp900_pusup350_trk60_hmt_L1TE20", "HLT_mb_sp900_pusup350_trk60_hmt_L1TE20.0ETA24", "HLT_mb_sp900_pusup500_trk60_hmt_L1TE5", "HLT_mb_sp900_trk60_hmt_L1TE20", "HLT_mb_sp900_trk60_hmt_L1TE20.0ETA24", "HLT_mb_sptrk_vetombts2in_L1RD0_FILLED", "HLT_mb_sptrk_vetombts2in_L1ZDC_AND", "HLT_mb_sptrk_vetombts2in_L1ZDC_A_C", "HLT_mu10_mgonly_L1LATEMU10_J50", "HLT_mu10_mgonly_L1LATEMU10_XE50", "HLT_mu10_mu10_l2msonly_bDimu_noL2", "HLT_mu10_mu10_l2msonly_bJpsimumu_noL2", "HLT_mu10_mu6_bBmumux_BcmumuDsloose", "HLT_mu10_mu6_bBmumux_BcmumuDsloose_L1LFV-MU", "HLT_mu10_mu6_bBmumux_BcmumuDsloose_noL2", "HLT_mu10_mu6_bBmumux_BcmumuDsloose_noL2_L1LFV-MU", "HLT_mu10_mu6_bBmumux_BpmumuKp", "HLT_mu10_mu6_bBmumux_BpmumuKp_noL2", "HLT_mu10_mu6_bBmumuxv2", "HLT_mu10_mu6_bBmumuxv2_L1LFV-MU",
#"HLT_mu10_mu6_bBmumuxv2_noL2",
#"HLT_mu10_mu6_bBmumuxv2_noL2_L1LFV-MU",
"HLT_mu10_muL2_mu6_muL2_bBmumuxv3", "HLT_mu10_mucombTag_noEF_L1MU40", "HLT_mu10_r1extr", "HLT_mu11_2mu4noL1_nscan03_L1MU11_2MU6", "HLT_mu11_L1MU10_2mu4noL1_nscan03_L1MU10_2MU6", "HLT_mu11_llns_mu6noL1_nscan03_bDimu_noL2_L1MU11_2MU6", "HLT_mu11_llns_mu6noL1_nscan03_bJpsimumu_noL2_L1MU11_2MU6", "HLT_mu11_msonly_mu6noL1_msonly_nscan05_noComb", "HLT_mu11_nomucomb_2mu4noL1_nscan03_L1LFV-MU", "HLT_mu11_nomucomb_2mu4noL1_nscan03_L1LFV-MU_bTau", "HLT_mu11_nomucomb_2mu4noL1_nscan03_L1MU11_2MU6_bTau", "HLT_mu11_nomucomb_mu6noL1_nscan03_L1LFV-MU_bTau", "HLT_mu11_nomucomb_mu6noL1_nscan03_L1MU11_2MU6", "HLT_mu14_iloose_tau25_medium1_tracktwo_L1DR-MU10TAU12I", "HLT_mu14_iloose_tau25_medium1_tracktwo_L1DR-MU10TAU12I_TAU12I-J25", "HLT_mu14_iloose_tau25_medium1_tracktwo_L1MU10_TAU12I-J25", "HLT_mu14_iloose_tau25_medium1_tracktwo_L1MU10_TAU12IM", "HLT_mu14_iloose_tau25_perf_ptonly_L1MU10_TAU12IM", "HLT_mu14_imedium_mu6noL1_msonly", "HLT_mu14_ivarloose_tau25_medium1_tracktwo_L1MU10_TAU12IM", "HLT_mu14_ivarloose_tau25_perf_ptonly_L1MU10_TAU12IM", "HLT_mu14_tau25_medium1_tracktwo_L1MU10_TAU12IM", "HLT_mu15_mucombTag_noEF_L1MU40", "HLT_mu18_iloose_mu8noL1_calotag_0eta010", "HLT_mu18_l2idonly_2mu4noL1", "HLT_mu18_l2idonly_mu8noL1", "HLT_mu18_mu8noL1_calotag_0eta010", "HLT_mu20_iloose_2mu4noL1", "HLT_mu20_iloose_mu8noL1", "HLT_mu20_imedium_mu8noL1", "HLT_mu20_l2idonly_mu6noL1_nscan03_bTau", "HLT_mu20_msonly_mu6noL1_msonly_nscan05_noComb", "HLT_mu20_mucombTag_noEF_L1MU40", "HLT_mu20_nomucomb_mu6noL1_nscan03", "HLT_mu20_r1extr", "HLT_mu22_iloose_mu8noL1_calotag_0eta010", "HLT_mu22_l2idonly_2mu4noL1", "HLT_mu22_l2idonly_mu8noL1", "HLT_mu22_mu8noL1_calotag_0eta010", "HLT_mu24_L2Star_idperf", "HLT_mu24_icalo", "HLT_mu24_iloose_mu8noL1_calotag_0eta010", "HLT_mu24_iloosecalo", "HLT_mu24_imediumcalo", "HLT_mu24_l2idonly_2mu4noL1", "HLT_mu24_l2idonly_mu8noL1", "HLT_mu24_mu8noL1_calotag_0eta010", "HLT_mu25_mucombTag_noEF_L1MU40", "HLT_mu26_imedium_2j35_boffperf", "HLT_mu26_imedium_2j35_bperf", "HLT_mu26_imedium_2j35_bperf_split", "HLT_mu4_2j35_dr05_j35_bmedium_split_antimatchdr05mu_L13J15_BTAG-MU4J15", "HLT_mu4_2j35_dr05_j35_bmedium_split_antimatchdr05mu_L13J20_BTAG-MU4J20", "HLT_mu4_2j35_dr05_j35_bmedium_split_antimatchdr05mu_L1MU4_3J15", "HLT_mu4_2j35_dr05_j35_bmedium_split_antimatchdr05mu_L1MU4_3J20", "HLT_mu4_2j35_dr05_j35_boffperf_split_antimatchdr05mu_L1MU4_3J15", "HLT_mu4_2j35_dr05_j35_boffperf_split_antimatchdr05mu_L1MU4_3J20", "HLT_mu4_2j40_dr05_j40_bloose_split_antimatchdr05mu_L13J15_BTAG-MU4J15", "HLT_mu4_2j40_dr05_j40_bloose_split_antimatchdr05mu_L13J20_BTAG-MU4J20", "HLT_mu4_2j40_dr05_j40_bloose_split_antimatchdr05mu_L1MU4_3J15", "HLT_mu4_2j40_dr05_j40_bloose_split_antimatchdr05mu_L1MU4_3J20", "HLT_mu4_2j40_dr05_j40_boffperf_split_antimatchdr05mu_L1MU4_3J15", "HLT_mu4_2j40_dr05_j40_boffperf_split_antimatchdr05mu_L1MU4_3J20", "HLT_mu4_3j30_dr05_j30_bmedium_split_antimatchdr05mu_L13J15_BTAG-MU4J15", "HLT_mu4_3j30_dr05_j30_bmedium_split_antimatchdr05mu_L13J20_BTAG-MU4J20", "HLT_mu4_3j30_dr05_j30_bmedium_split_antimatchdr05mu_L14J20", "HLT_mu4_3j30_dr05_j30_bmedium_split_antimatchdr05mu_L1MU4_3J15", "HLT_mu4_3j30_dr05_j30_bmedium_split_antimatchdr05mu_L1MU4_3J20", "HLT_mu4_3j30_dr05_j30_boffperf_split_antimatchdr05mu_L13J15_BTAG-MU4J15", "HLT_mu4_3j30_dr05_j30_boffperf_split_antimatchdr05mu_L14J20", "HLT_mu4_3j30_dr05_j30_boffperf_split_antimatchdr05mu_L1MU4_3J15", "HLT_mu4_3j30_dr05_j30_boffperf_split_antimatchdr05mu_L1MU4_3J20", "HLT_mu4_3j35_dr05_j35_bloose_split_antimatchdr05mu_L13J15_BTAG-MU4J15", "HLT_mu4_3j35_dr05_j35_bloose_split_antimatchdr05mu_L13J20_BTAG-MU4J20", "HLT_mu4_3j35_dr05_j35_bloose_split_antimatchdr05mu_L14J20", "HLT_mu4_3j35_dr05_j35_bloose_split_antimatchdr05mu_L1MU4_3J15", "HLT_mu4_3j35_dr05_j35_bloose_split_antimatchdr05mu_L1MU4_3J20", "HLT_mu4_3j35_dr05_j35_boffperf_antimatchdr05mu_L14J20", "HLT_mu4_3j35_dr05_j35_boffperf_split_antimatchdr05mu_L13J15_BTAG-MU4J15", "HLT_mu4_3j35_dr05_j35_boffperf_split_antimatchdr05mu_L13J20_BTAG-MU4J20", "HLT_mu4_3j35_dr05_j35_boffperf_split_antimatchdr05mu_L14J20", "HLT_mu4_3j35_dr05_j35_boffperf_split_antimatchdr05mu_L1MU4_3J15", "HLT_mu4_3j35_dr05_j35_boffperf_split_antimatchdr05mu_L1MU4_3J20", "HLT_mu4_3j45_dr05_L13J15_BTAG-MU4J15", "HLT_mu4_3j45_dr05_L13J20_BTAG-MU4J20", "HLT_mu4_4j40_dr05_L13J15_BTAG-MU4J15", "HLT_mu4_4j40_dr05_L13J20_BTAG-MU4J20", "HLT_mu4_cosmic_L1MU11_EMPTY", "HLT_mu4_cosmic_L1MU4_EMPTY", "HLT_mu4_iloose_mu4_11invm60_noos_L1DY-BOX-2MU4", "HLT_mu4_iloose_mu4_11invm60_noos_novtx_L1DY-BOX-2MU4", "HLT_mu4_iloose_mu4_7invm9_noos_L1DY-BOX-2MU4", "HLT_mu4_iloose_mu4_7invm9_noos_novtx_L1DY-BOX-2MU4", "HLT_mu4_j15_dr05", "HLT_mu4_j25_dr05", "HLT_mu4_j35_bperf_split_dr05_dz02_L1BTAG-MU4J15", "HLT_mu4_j35_dr05", "HLT_mu4_j55_bperf_split_dr05_dz02_L1BTAG-MU4J15", "HLT_mu4_j55_dr05", "HLT_mu4_j60_dr05_2j35_L13J15_BTAG-MU4J30", "HLT_mu4_j70_dr05_L1BTAG-MU4J30", "HLT_mu4_msonly_cosmic_L1MU11_EMPTY", "HLT_mu4_msonly_cosmic_L1MU4_EMPTY", "HLT_mu4_mu4_l2msonly_bDimu_noL2", "HLT_mu4_mu4_l2msonly_bJpsimumu_noL2", "HLT_mu4_mu4noL1", "HLT_mu4_r1extr", "HLT_mu4noL1", "HLT_mu6_2j40_0eta490_invm1000", "HLT_mu6_2j40_0eta490_invm400", "HLT_mu6_2j40_0eta490_invm600", "HLT_mu6_2j40_0eta490_invm800", "HLT_mu6_2mu4", "HLT_mu6_2mu4_bDimu", "HLT_mu6_2mu4_bJpsi", "HLT_mu6_2mu4_bTau", "HLT_mu6_2mu4_bTau_noL2", "HLT_mu6_2mu4_bUpsi", "HLT_mu6_L2Star_idperf", "HLT_mu6_iloose_mu6_11invm24_noos_L1DY-BOX-2MU6", "HLT_mu6_iloose_mu6_11invm24_noos_novtx_L1DY-BOX-2MU6", "HLT_mu6_iloose_mu6_11invm60_noos", "HLT_mu6_iloose_mu6_11invm60_noos_novtx", "HLT_mu6_iloose_mu6_24invm60_noos_novtx_L1DY-BOX-2MU6", "HLT_mu6_j110_bperf_split_dr05_dz02_L1BTAG-MU6J20", "HLT_mu6_j110_dr05", "HLT_mu6_j150_bperf_split_dr05_dz02_L1BTAG-MU6J20", "HLT_mu6_j150_dr05", "HLT_mu6_j175_bperf_split_dr05_dz02_L1BTAG-MU6J20", "HLT_mu6_j175_dr05", "HLT_mu6_j260_bperf_split_dr05_dz02_L1BTAG-MU6J20", "HLT_mu6_j260_dr05", "HLT_mu6_j320_bperf_split_dr05_dz02_L1BTAG-MU6J20", "HLT_mu6_j320_dr05", "HLT_mu6_j400_bperf_split_dr05_dz02_L1BTAG-MU6J20", "HLT_mu6_j400_dr05", "HLT_mu6_j50_dr05_2j35_L13J15_BTAG-MU6J25", "HLT_mu6_j60_dr05_L1BTAG-MU6J25", "HLT_mu6_j85_bperf_split_dr05_dz02_L1BTAG-MU6J20", "HLT_mu6_j85_dr05", "HLT_mu6_l2msonly_2mu4_l2msonly_L1MU6_3MU4", "HLT_mu6_l2msonly_mu4_bDimu_noL2", "HLT_mu6_l2msonly_mu4_bJpsimumu_noL2", "HLT_mu6_l2msonly_mu4_l2msonly_bDimu_noL2", "HLT_mu6_l2msonly_mu4_l2msonly_bJpsimumu_noL2", "HLT_mu6_mu4_bBmumu_L12CMU4", "HLT_mu6_mu4_bBmumu_L12MU4-B",
        #"HLT_mu6_mu4_bBmumu_L1BPH-2M8-MU6MU4-B_BPH-0DR15-MU6MU4",
#"HLT_mu6_mu4_bBmumu_Lxy0",
"HLT_mu6_mu4_bBmumu_noL2", "HLT_mu6_mu4_bBmumu_noL2_L12CMU4", "HLT_mu6_mu4_bBmumu_noL2_L12MU4-B", "HLT_mu6_mu4_bBmumu_noL2_L1BPH-2M8-MU6MU4-B_BPH-0DR15-MU6MU4", "HLT_mu6_mu4_bBmumu_noL2_L1BPH-2M8-MU6MU4_BPH-0DR15-MU6MU4", "HLT_mu6_mu4_bBmumu_tightChi2", "HLT_mu6_mu4_bBmumux_BcmumuDsloose_L12CMU4", "HLT_mu6_mu4_bBmumux_BcmumuDsloose_L12MU4-B", "HLT_mu6_mu4_bBmumux_BcmumuDsloose_L1BPH-2M8-MU6MU4-B_BPH-0DR15-MU6MU4", "HLT_mu6_mu4_bBmumux_BcmumuDsloose_noL2", "HLT_mu6_mu4_bBmumux_BcmumuDsloose_noL2_L12CMU4", "HLT_mu6_mu4_bBmumux_BcmumuDsloose_noL2_L12MU4-B", "HLT_mu6_mu4_bBmumux_BcmumuDsloose_noL2_L1BPH-2M8-MU6MU4-B_BPH-0DR15-MU6MU4", "HLT_mu6_mu4_bBmumux_BcmumuDsloose_noL2_L1BPH-2M8-MU6MU4_BPH-0DR15-MU6MU4", "HLT_mu6_mu4_bBmumux_BpmumuKp_noL2", "HLT_mu6_mu4_bBmumuxv2_L12CMU4", "HLT_mu6_mu4_bBmumuxv2_L12MU4-B", "HLT_mu6_mu4_bBmumuxv2_L1BPH-2M8-MU6MU4_BPH-0DR15-MU6MU4", "HLT_mu6_mu4_bBmumuxv2_noL2", "HLT_mu6_mu4_bBmumuxv2_noL2_L12CMU4", "HLT_mu6_mu4_bBmumuxv2_noL2_L12MU4-B", "HLT_mu6_mu4_bBmumuxv2_noL2_L1BPH-2M8-MU6MU4-B_BPH-0DR15-MU6MU4", "HLT_mu6_mu4_bBmumuxv2_noL2_L1BPH-2M8-MU6MU4_BPH-0DR15-MU6MU4", "HLT_mu6_mu4_bBmumuxv3", "HLT_mu6_mu4_bDimu_L12CMU4", "HLT_mu6_mu4_bDimu_L12MU4-B", "HLT_mu6_mu4_bDimu_L1BPH-2M8-MU6MU4-B_BPH-0DR15-MU6MU4", "HLT_mu6_mu4_bDimu_L1BPH-8M15-MU6MU4", "HLT_mu6_mu4_bDimu_Lxy0", "HLT_mu6_mu4_bDimu_noEFbph", "HLT_mu6_mu4_bDimu_noL2", "HLT_mu6_mu4_bDimu_noL2_L12CMU4", "HLT_mu6_mu4_bDimu_noL2_L12MU4-B", "HLT_mu6_mu4_bDimu_noL2_L1BPH-2M8-MU6MU4-B_BPH-0DR15-MU6MU4", "HLT_mu6_mu4_bDimu_noL2_L1BPH-2M8-MU6MU4_BPH-0DR15-MU6MU4", "HLT_mu6_mu4_bDimu_noL2_L1BPH-8M15-MU6MU4", "HLT_mu6_mu4_bDimu_noL2_L1BPH-8M15-MU6MU4_BPH-0DR22-MU6MU4", "HLT_mu6_mu4_bDimu_novtx_noos_L12CMU4", "HLT_mu6_mu4_bDimu_novtx_noos_L12MU4-B", "HLT_mu6_mu4_bDimu_novtx_noos_L1BPH-2M8-MU6MU4-B_BPH-0DR15-MU6MU4", "HLT_mu6_mu4_bDimu_novtx_noos_L1BPH-8M15-MU6MU4", "HLT_mu6_mu4_bDimu_novtx_noos_noL2", "HLT_mu6_mu4_bDimu_novtx_noos_noL2_L12CMU4", "HLT_mu6_mu4_bDimu_novtx_noos_noL2_L12MU4-B", "HLT_mu6_mu4_bDimu_novtx_noos_noL2_L1BPH-2M8-MU6MU4-B_BPH-0DR15-MU6MU4", "HLT_mu6_mu4_bDimu_novtx_noos_noL2_L1BPH-2M8-MU6MU4_BPH-0DR15-MU6MU4", "HLT_mu6_mu4_bDimu_novtx_noos_noL2_L1BPH-8M15-MU6MU4", "HLT_mu6_mu4_bDimu_novtx_noos_noL2_L1BPH-8M15-MU6MU4_BPH-0DR22-MU6MU4", "HLT_mu6_mu4_bDimu_tightChi2", "HLT_mu6_mu4_bJpsimumu_L12CMU4", "HLT_mu6_mu4_bJpsimumu_L12MU4-B", "HLT_mu6_mu4_bJpsimumu_L1BPH-2M8-MU6MU4_BPH-0DR15-MU6MU4",
#"HLT_mu6_mu4_bJpsimumu_Lxy0",
        "HLT_mu6_mu4_bJpsimumu_noEFbph", "HLT_mu6_mu4_bJpsimumu_noL2", "HLT_mu6_mu4_bJpsimumu_noL2_L12CMU4", "HLT_mu6_mu4_bJpsimumu_noL2_L12MU4-B", "HLT_mu6_mu4_bJpsimumu_noL2_L1BPH-2M8-MU6MU4-B_BPH-0DR15-MU6MU4", "HLT_mu6_mu4_bJpsimumu_noL2_L1BPH-2M8-MU6MU4_BPH-0DR15-MU6MU4", "HLT_mu6_mu4_bJpsimumu_tightChi2", "HLT_mu6_mu4_bUpsimumu_L12CMU4", "HLT_mu6_mu4_bUpsimumu_L12MU4-B", "HLT_mu6_mu4_bUpsimumu_L1BPH-8M15-MU6MU4", "HLT_mu6_mu4_bUpsimumu_tightChi2", "HLT_mu6_mu4_l2msonly_bDimu_noL2", "HLT_mu6_mu4_l2msonly_bJpsimumu_noL2", "HLT_mu6_mu4noL1", "HLT_mu6_mu6_l2msonly_bDimu_noL2", "HLT_mu6_mu6_l2msonly_bJpsimumu_noL2", "HLT_mu6_muL2_mu4_muL2_bBmumuxv3", "HLT_mu6_nomucomb_2mu4_nomucomb_L1MU6_3MU4", "HLT_mu8", "HLT_mu8_mucombTag_noEF_L1MU40", "HLT_noalg_L1TGC_BURST", "HLT_noalg_L1Topo", "HLT_noalg_bkg_L1Bkg", "HLT_noalg_bkg_L1J12_BGRP12", "HLT_noalg_bkg_L1J30.31ETA49_BGRP12", "HLT_noalg_cosmiccalo_L1EM3_EMPTY", "HLT_noalg_cosmiccalo_L1J12_EMPTY", "HLT_noalg_cosmiccalo_L1J3031ETA49_EMPTY", "HLT_noalg_cosmiccalo_L1J30_EMPTY", "HLT_noalg_cosmiccalo_L1RD1_EMPTY", "HLT_noalg_idmon_L1RD0_EMPTY", "HLT_noalg_idmon_L1RD0_FILLED", "HLT_noalg_idmon_L1RD0_UNPAIRED_ISO", "HLT_noalg_l1calo_L12EM3", "HLT_noalg_l1calo_L12MU4", "HLT_noalg_l1calo_L12MU6", "HLT_noalg_l1calo_L14J15", "HLT_noalg_l1calo_L14J20.0ETA49", "HLT_noalg_l1calo_L1EM12_2EM3", "HLT_noalg_l1calo_L1EM15", "HLT_noalg_l1calo_L1EM15HI", "HLT_noalg_l1calo_L1EM15I_MU4", "HLT_noalg_l1calo_L1EM3", "HLT_noalg_l1calo_L1EM7", "HLT_noalg_l1calo_L1EM7_2EM3", "HLT_noalg_l1calo_L1EM8I_MU10", "HLT_noalg_l1calo_L1J100", "HLT_noalg_l1calo_L1J15", "HLT_noalg_l1calo_L1J20", "HLT_noalg_l1calo_L1J25", "HLT_noalg_l1calo_L1J30.0ETA49_2J20.0ETA49", "HLT_noalg_l1calo_L1J40_XE50", "HLT_noalg_l1calo_L1MU10_2MU6", "HLT_noalg_l1calo_L1MU10_TAU12IM", "HLT_noalg_l1calo_L1MU4", "HLT_noalg_l1calo_L1MU4_J30", "HLT_noalg_l1calo_L1MU6_2MU4", "HLT_noalg_l1calo_L1MU6_J20", "HLT_noalg_l1calo_L1TAU12IM", "HLT_noalg_l1calo_L1TAU20IM_2TAU12IM", "HLT_noalg_l1calo_L1TAU30", "HLT_noalg_l1calo_L1TAU40", "HLT_noalg_l1calo_L1XE35", "HLT_noalg_l1calo_L1XE45", "HLT_noalg_l1calo_L1XE55", "HLT_noalg_l1calo_L1XE60", "HLT_noalg_l1calo_L1XE70", "HLT_noalg_larcells_L1LAR-EM", "HLT_noalg_larcells_L1LAR-J", "HLT_noalg_to_L12MU20_OVERLAY", "HLT_noalg_zb_L1ZB", "HLT_tau0_perf_ptonly_L1TAU12", "HLT_tau0_perf_ptonly_L1TAU60", "HLT_tau125_medium1_calo", "HLT_tau125_r1medium1", "HLT_tau125_r1perf", "HLT_tau160_idperf_track", "HLT_tau160_idperf_tracktwo", "HLT_tau1_cosmic_ptonly_L1MU4_EMPTY", "HLT_tau1_cosmic_track_L1MU4_EMPTY", "HLT_tau20_r1medium1", "HLT_tau20_r1perf", "HLT_tau25_idperf_track", "HLT_tau25_idperf_tracktwo", "HLT_tau25_idperf_tracktwo2015", "HLT_tau25_medium1_calo", "HLT_tau25_medium1_trackcalo", "HLT_tau25_medium1_tracktwo2015", "HLT_tau25_medium1_tracktwocalo", "HLT_tau25_perf_calo", "HLT_tau25_perf_calo_L1TAU12", "HLT_tau25_perf_trackcalo", "HLT_tau25_perf_tracktwo", "HLT_tau25_perf_tracktwo2015", "HLT_tau25_perf_tracktwocalo", "HLT_tau25_r1perf", "HLT_tau29_r1medium1", "HLT_tau29_r1perf", "HLT_tau35_loose1_tracktwo_tau25_loose1_tracktwo_L1DR-TAU20ITAU12I", "HLT_tau35_loose1_tracktwo_tau25_loose1_tracktwo_L1DR-TAU20ITAU12I-J25", "HLT_tau35_loose1_tracktwo_tau25_loose1_tracktwo_L1TAU20ITAU12I-J25", "HLT_tau35_medium1_calo", "HLT_tau35_medium1_tracktwo_L1TAU20_xe70_L1XE45_L1XE45_TAU20-J20", "HLT_tau35_medium1_tracktwo_tau25_medium1_tracktwo_L1BOX-TAU20ITAU12I", "HLT_tau35_perf_ptonly_tau25_perf_ptonly_L1TAU20IM_2TAU12IM", "HLT_tau35_perf_tracktwo_L1TAU20_tau25_perf_tracktwo_L1TAU12", "HLT_tau35_perf_tracktwo_tau25_perf_tracktwo_ditauL", "HLT_tau35_perf_tracktwo_tau25_perf_tracktwo_ditauM", "HLT_tau35_perf_tracktwo_tau25_perf_tracktwo_ditauT", "HLT_tau80_medium1_calo", "HLT_tau8_cosmic_ptonly", "HLT_tau8_cosmic_track", "HLT_te20", "HLT_te20_tc_lcw", "HLT_te50_L1TE20", "HLT_te50_L1TE20.0ETA24", "HLT_te60_L1TE40", "HLT_te60_L1TE40.0ETA24", "HLT_xe100_L1XE50", "HLT_xe100_L1XE80", "HLT_xe100_mht", "HLT_xe100_mht_L1XE50", "HLT_xe100_mht_L1XE80", "HLT_xe100_mht_wEFMu", "HLT_xe100_mht_wEFMu_L1XE50", "HLT_xe100_mht_wEFMu_L1XE80", "HLT_xe100_pueta",  "HLT_xe100_pueta_L1XE80", "HLT_xe100_pueta_wEFMu", "HLT_xe100_pueta_wEFMu_L1XE80", "HLT_xe100_pufit", "HLT_xe100_pufit_L1XE80", "HLT_xe100_pufit_wEFMu", "HLT_xe100_pufit_wEFMu_L1XE80", "HLT_xe100_tc_em", "HLT_xe100_tc_em_L1XE50", "HLT_xe100_tc_em_L1XE80", "HLT_xe100_tc_em_wEFMu", "HLT_xe100_tc_em_wEFMu_L1XE50", "HLT_xe100_tc_em_wEFMu_L1XE80", "HLT_xe100_tc_lcw", "HLT_xe100_tc_lcw_L1XE50", "HLT_xe100_tc_lcw_L1XE80", "HLT_xe100_tc_lcw_wEFMu", "HLT_xe100_tc_lcw_wEFMu_L1XE50", "HLT_xe100_tc_lcw_wEFMu_L1XE80", "HLT_xe100_wEFMu", "HLT_xe100_wEFMu_L1XE50", "HLT_xe100_wEFMu_L1XE80", "HLT_xe110_L1XE60", "HLT_xe110_mht_L1XE60", "HLT_xe110_pueta_L1XE60", "HLT_xe110_pufit_L1XE60", "HLT_xe110_tc_lcw_L1XE60", "HLT_xe35_L2FS", "HLT_xe35_l2fsperf_wEFMuFEB_wEFMu", "HLT_xe50_mht", "HLT_xe50_pueta", "HLT_xe50_pufit", "HLT_xe50_tc_em", "HLT_xe50_tc_lcw", "HLT_xe60", "HLT_xe60_mht", "HLT_xe60_mht_wEFMu", "HLT_xe60_pueta", "HLT_xe60_pueta_wEFMu", "HLT_xe60_pufit", "HLT_xe60_pufit_wEFMu", "HLT_xe60_tc_em", "HLT_xe60_tc_em_wEFMu", "HLT_xe60_tc_lcw", "HLT_xe60_tc_lcw_wEFMu", "HLT_xe60_wEFMu", "HLT_xe70", "HLT_xe70_mht", "HLT_xe70_mht_wEFMu", "HLT_xe70_pueta", "HLT_xe70_pueta_wEFMu", "HLT_xe70_pufit", "HLT_xe70_pufit_wEFMu", "HLT_xe70_tc_em", "HLT_xe70_tc_em_wEFMu", "HLT_xe70_tc_lcw", "HLT_xe70_tc_lcw_wEFMu", "HLT_xe70_wEFMu", "HLT_xe80_L1XE70", "HLT_xe80_mht", "HLT_xe80_mht_L1XE50", "HLT_xe80_mht_L1XE70", "HLT_xe80_mht_wEFMu", "HLT_xe80_mht_wEFMu_L1XE50", "HLT_xe80_mht_wEFMu_L1XE70", "HLT_xe80_pueta", "HLT_xe80_pueta_L1XE50", "HLT_xe80_pueta_L1XE70", "HLT_xe80_pueta_wEFMu", "HLT_xe80_pueta_wEFMu_L1XE50", "HLT_xe80_pueta_wEFMu_L1XE70", "HLT_xe80_pufit", "HLT_xe80_pufit_L1XE50", "HLT_xe80_pufit_L1XE70", "HLT_xe80_pufit_wEFMu", "HLT_xe80_pufit_wEFMu_L1XE50", "HLT_xe80_pufit_wEFMu_L1XE70", "HLT_xe80_tc_em", "HLT_xe80_tc_em_L1XE50", "HLT_xe80_tc_em_L1XE70", "HLT_xe80_tc_em_wEFMu", "HLT_xe80_tc_em_wEFMu_L1XE50", "HLT_xe80_tc_em_wEFMu_L1XE70", "HLT_xe80_tc_lcw", "HLT_xe80_tc_lcw_L1XE50", "HLT_xe80_tc_lcw_L1XE70", "HLT_xe80_tc_lcw_wEFMu", "HLT_xe80_tc_lcw_wEFMu_L1XE50", "HLT_xe80_tc_lcw_wEFMu_L1XE70", "HLT_xe80_wEFMu", "HLT_xe80_wEFMu_L1XE50", "HLT_xe80_wEFMu_L1XE70", "HLT_xe90_pueta_L1XE50", "HLT_xe90_L1XE60", "HLT_xe90_mht", "HLT_xe90_mht_L1XE60", "HLT_xe90_mht_wEFMu", "HLT_xe90_pueta", "HLT_xe90_pueta_L1XE60", "HLT_xe90_pueta_wEFMu", "HLT_xe90_pufit", "HLT_xe90_pufit_L1XE60", "HLT_xe90_pufit_wEFMu", "HLT_xe90_tc_em", "HLT_xe90_tc_em_wEFMu", "HLT_xe90_tc_lcw", "HLT_xe90_tc_lcw_L1XE60", "HLT_xe90_tc_lcw_wEFMu", "HLT_xe90_wEFMu", "HLT_xs30", "HLT_xs30_tc_lcw", "HLT_zdcpeb_L1ZDC_A", "HLT_zdcpeb_L1ZDC_AND", "HLT_zdcpeb_L1ZDC_A_C", "HLT_zdcpeb_L1ZDC_C", "HLT_e24_vloose_L1EM20VH_3j20noL1", "HLT_e24_vloose_L1EM20VH_4j20noL1", "HLT_e24_vloose_L1EM20VH_5j15noL1", "HLT_e24_vloose_L1EM20VH_6j15noL1", "HLT_e26_vloose_L1EM20VH_3j20noL1", "HLT_e26_vloose_L1EM20VH_4j20noL1", "HLT_e26_vloose_L1EM20VH_5j15noL1", "HLT_e26_vloose_L1EM20VH_6j15noL1", 'HLT_2mu6_nomucomb_mu4_nomucomb_bTau_L12MU6_3MU4', 'HLT_mu6_nomucomb_2mu4_nomucomb_bTau_L1MU6_3MU4', 'HLT_mu20_nomucomb_2mu4noL1', 'HLT_mu20_nomucomb_mu8noL1', 'HLT_mu22_nomucomb_mu8noL1', 'HLT_mu22_nomucomb_2mu4noL1', 'HLT_xe90_pufit_wEFMu_L1XE50', 'HLT_xe90_pueta_wEFMu_L1XE50', 'HLT_xe90_pueta_wEFMu_L1XE50', 'HLT_xe90_pufit_L1XE50', 'HLT_xe80_L1XE50', 'HLT_xe80', 'HLT_xe90', "HLT_2mu6_bBmumu", "HLT_mu6_mu4_bBmumuxv2_L1BPH-2M8-MU6MU4-B_BPH-0DR15-MU6MU4", "HLT_mu6_mu4_bBmumux_BcmumuDsloose_L1BPH-2M8-MU6MU4_BPH-0DR15-MU6MU4", "HLT_2mu4_bBmumuxv2_L1BPH-2M8-2MU4_BPH-0DR15-2MU4", "HLT_2mu4_bBmumux_BcmumuDsloose_L1BPH-2M8-2MU4_BPH-0DR15-2MU4",
        ]
        
disabled_Rules.update(dict(map(None,HLT_list,len(HLT_list)*[{
    1000 : TriggerRule(PS=-1, comment='Undefined'), 
    }])))


# --------------------------------------
# low mu run chains

HLT_list=[
'HLT_j10_L1ALFA_Jet', 'HLT_j10_L1ALFA_UNPAIRED_ISO_Jet', 'HLT_j15_L1ALFA_Jet', 'HLT_j15_L1ALFA_UNPAIRED_ISO_Jet', 'HLT_j25_L1ALFA_Jet', 'HLT_j25_L1ALFA_UNPAIRED_ISO_Jet', 'HLT_j35_L1ALFA_Jet', 'HLT_j35_L1ALFA_UNPAIRED_ISO_Jet', 'HLT_lhcfpeb', 'HLT_lhcfpeb_L1LHCF_ALFA_ANY_A', 'HLT_lhcfpeb_L1LHCF_ALFA_ANY_A_UNPAIRED_ISO', 'HLT_lhcfpeb_L1LHCF_ALFA_ANY_C', 'HLT_lhcfpeb_L1LHCF_ALFA_ANY_C_UNPAIRED_ISO', 'HLT_lhcfpeb_L1LHCF_EMPTY', 'HLT_lhcfpeb_L1LHCF_UNPAIRED_ISO', 'HLT_te40_L1MBTS_1_1', 'HLT_te50_L1MBTS_1_1', 'HLT_te50_L1RD3_FILLED', 'HLT_te50_L1TE10', 'HLT_te50_L1TE10.0ETA24', 'HLT_te50_L1TE10.0ETA25', 'HLT_te60_L1MBTS_1_1', 'HLT_te60_L1TE20', 'HLT_te60_L1TE20.0ETA24', 'HLT_te60_L1TE20.0ETA25', 'HLT_te70_L1MBTS_1_1', 'HLT_te80_L1MBTS_1_1',]
disabled_Rules.update(dict(map(None,HLT_list,len(HLT_list)*[{
    1000 : TriggerRule(PS=-1, comment='Disabled low-mu'), 
    }])))

# --------------------------------------
# streamers

HLT_list=[
    'HLT_noalg_L12EM10VH', 'HLT_noalg_L12EM15', 'HLT_noalg_L12EM3', 'HLT_noalg_L12EM7', 'HLT_noalg_L12EM8VH_MU10', 'HLT_noalg_L12J15_XE55', 'HLT_noalg_L12MU4', 'HLT_noalg_L12MU6', 'HLT_noalg_L13J25.0ETA23', 'HLT_noalg_L13J40', 'HLT_noalg_L13MU4', 'HLT_noalg_L14J15', 'HLT_noalg_L14J20.0ETA49', 'HLT_noalg_L1ALFA_Diff', 'HLT_noalg_L1ALFA_Phys', 'HLT_noalg_L1ALFA_PhysAny', 'HLT_noalg_L1ALFA_SYS', 'HLT_noalg_L1ALFA_TRT_Diff', 'HLT_noalg_L1ALFA_TRT_UNPAIRED_ISO_Diff', 'HLT_noalg_L1All', 'HLT_noalg_L1CALREQ2', 'HLT_noalg_L1Calo', 'HLT_noalg_L1Calo_EMPTY', 'HLT_noalg_L1EM12', 'HLT_noalg_L1EM15', 'HLT_noalg_L1EM15HI_2TAU12IM', 'HLT_noalg_L1EM15HI_2TAU12IM_J25_3J12', 'HLT_noalg_L1EM15HI_2TAU12IM_XE35', 'HLT_noalg_L1EM15HI_TAU40_2TAU15', 'HLT_noalg_L1EM15VH_3EM7', 'HLT_noalg_L1EM15VH_MU10', 'HLT_noalg_L1EM15_MU4', 'HLT_noalg_L1EM18VH', 'HLT_noalg_L1EM20VH', 'HLT_noalg_L1EM3', 'HLT_noalg_L1EM7', 'HLT_noalg_L1EM7_MU10', 'HLT_noalg_L1J15_J15.31ETA49', 'HLT_noalg_L1J20_J20.31ETA49', 'HLT_noalg_L1J30.0ETA49_2J20.0ETA49', 'HLT_noalg_L1J50', 'HLT_noalg_L1J75', 'HLT_noalg_L1J75.31ETA49', 'HLT_noalg_L1LHCF', 'HLT_noalg_L1LHCF_EMPTY', 'HLT_noalg_L1LHCF_UNPAIRED_ISO', 'HLT_noalg_L1LowLumi', 'HLT_noalg_L1MBTS_1_BGRP11', 'HLT_noalg_L1MBTS_1_BGRP9', 'HLT_noalg_L1MBTS_2_BGRP11', 'HLT_noalg_L1MBTS_2_BGRP9', 'HLT_noalg_L1MJJ-100', 'HLT_noalg_L1MU10_TAU12IM', 'HLT_noalg_L1MU10_TAU12IM_J25_2J12', 'HLT_noalg_L1MU10_TAU12IM_XE35', 'HLT_noalg_L1MU10_TAU12_J25_2J12', 'HLT_noalg_L1MU10_TAU20', 'HLT_noalg_L1MU4', 'HLT_noalg_L1MU4_3J15', 'HLT_noalg_L1MU4_J12', 'HLT_noalg_L1MU4_J30', 'HLT_noalg_L1MU6', 'HLT_noalg_L1MU6_J20', 'HLT_noalg_L1MU6_J30.0ETA49_2J20.0ETA49', 'HLT_noalg_L1MU6_J40', 'HLT_noalg_L1MinBias', 'HLT_noalg_L1RD0_BGRP11', 'HLT_noalg_L1RD0_BGRP9', 'HLT_noalg_L1Standby', 'HLT_noalg_L1TAU12', 'HLT_noalg_L1TAU12IL', 'HLT_noalg_L1TAU12IM', 'HLT_noalg_L1TAU12IT', 'HLT_noalg_L1TAU20', 'HLT_noalg_L1TAU20IL', 'HLT_noalg_L1TAU20IM', 'HLT_noalg_L1TAU20IM_2TAU12IM', 'HLT_noalg_L1TAU20IM_2TAU12IM_J25_2J20_3J12', 'HLT_noalg_L1TAU20IM_2TAU12IM_XE35', 'HLT_noalg_L1TAU20IT', 'HLT_noalg_L1TAU20_2J20_XE45', 'HLT_noalg_L1TAU20_2TAU12', 'HLT_noalg_L1TAU20_2TAU12_XE35', 'HLT_noalg_L1TAU30', 'HLT_noalg_L1TAU60', 'HLT_noalg_L1XE50', 'HLT_noalg_bkg_L1J12', 'HLT_noalg_bkg_L1J12_BGRP0', 'HLT_noalg_bkg_L1J30.31ETA49', 'HLT_noalg_bkg_L1J30.31ETA49_BGRP0', 'HLT_noalg_cosmiccalo_L1J12_FIRSTEMPTY', 'HLT_noalg_cosmiccalo_L1J30_FIRSTEMPTY', 'HLT_noalg_cosmicmuons_L1MU11_EMPTY', 'HLT_noalg_cosmicmuons_L1MU4_EMPTY', 'HLT_noalg_dcmmon_L1RD2_BGRP12', 'HLT_noalg_dcmmon_L1RD2_EMPTY', 'HLT_noalg_eb_L1ABORTGAPNOTCALIB_noPS', 'HLT_noalg_eb_L1EMPTY_noPS', 'HLT_noalg_eb_L1FIRSTEMPTY_noPS', 'HLT_noalg_eb_L1PhysicsHigh_noPS', 'HLT_noalg_eb_L1PhysicsLow_noPS', 'HLT_noalg_eb_L1RD0_EMPTY', 'HLT_noalg_eb_L1RD3_EMPTY', 'HLT_noalg_eb_L1RD3_FILLED', 'HLT_noalg_eb_L1UNPAIRED_ISO_noPS', 'HLT_noalg_eb_L1UNPAIRED_NONISO_noPS', 'HLT_noalg_idcosmic_L1TRT_EMPTY', 'HLT_noalg_idcosmic_L1TRT_FILLED', 'HLT_noalg_L1ALFA_ANY', 'HLT_noalg_L1BPH-2M-2MU4', 'HLT_noalg_L1BPH-2M-2MU6', 'HLT_noalg_L1BPH-2M-MU6MU4', 'HLT_noalg_L1BPH-4M8-2MU4', 'HLT_noalg_L1BPH-4M8-2MU6', 'HLT_noalg_L1BPH-4M8-MU6MU4', 'HLT_noalg_L1BPH-DR-2MU4', 'HLT_noalg_L1BPH-DR-2MU6', 'HLT_noalg_L1BPH-DR-MU6MU4', 'HLT_noalg_L1DR-MU10TAU12I', 'HLT_noalg_L1DR-TAU20ITAU12I', 'HLT_noalg_L1DY-BOX-2MU4', 'HLT_noalg_L1DY-BOX-2MU6', 'HLT_noalg_L1DY-BOX-MU6MU4', 'HLT_noalg_L1DY-DR-2MU4', 'HLT_noalg_L1EM10', 'HLT_noalg_L1EM10VH', 'HLT_noalg_L1EM13VH', 'HLT_noalg_L1EM15VH', 'HLT_noalg_L1EM20VHI', 'HLT_noalg_L1EM22VHI', 'HLT_noalg_L1EM8VH', 'HLT_noalg_L1J100', 'HLT_noalg_L1J12', 'HLT_noalg_L1J120', 'HLT_noalg_L1J15', 'HLT_noalg_L1J20', 'HLT_noalg_L1J25', 'HLT_noalg_L1J30', 'HLT_noalg_L1J40', 'HLT_noalg_L1J85', 'HLT_noalg_L1LFV-MU', 'HLT_noalg_L1TAU40', 'HLT_noalg_L1TAU8', 'HLT_noalg_L1TE10', 'HLT_noalg_L1TE10.0ETA24', 'HLT_noalg_L1TE20', 'HLT_noalg_L1TE20.0ETA24', 'HLT_noalg_L1TE30', 'HLT_noalg_L1TE30.0ETA24', 'HLT_noalg_L1TE40', 'HLT_noalg_L1TE50',  'HLT_noalg_L1XE40', 'HLT_noalg_L1XE45', 'HLT_noalg_L1XE55', 'HLT_noalg_L1XE60', 'HLT_noalg_L1XE70', 'HLT_noalg_L1XE80', 'HLT_noalg_L1XS20', 'HLT_noalg_L1XS30', 'HLT_noalg_L1XS40', 'HLT_noalg_L1XS50', 'HLT_noalg_L1XS60', 'HLT_noalg_L1J100.31ETA49', 'HLT_noalg_L1J15.28ETA31', 'HLT_noalg_L1J15.31ETA49', 'HLT_noalg_L1J20.28ETA31', 'HLT_noalg_L1J20.31ETA49', 'HLT_noalg_L1J30.31ETA49', 'HLT_noalg_L1J50.31ETA49', 'HLT_noalg_L1RD1_FILLED', 'HLT_noalg_L1RD2_FILLED', 'HLT_noalg_L1RD2_EMPTY', 'HLT_noalg_L1RD3_FILLED', 'HLT_noalg_L1RD3_EMPTY', 'HLT_noalg_standby_L1RD0_FILLED', 'HLT_noalg_standby_L1RD0_EMPTY', "HLT_noalg_L1EM15I_MU4", "HLT_noalg_L1TE40.0ETA24", "HLT_noalg_L1TE50.0ETA24", "HLT_noalg_L1TE60", "HLT_noalg_L1TE60.0ETA24", "HLT_noalg_L1TE70", "HLT_noalg_mb_L1TE50.0ETA24", "HLT_noalg_mb_L1TE60", "HLT_noalg_mb_L1TE60.0ETA24", "HLT_noalg_mb_L1TE70", "HLT_noalg_mb_L1TE70.0ETA24", "HLT_noalg_mb_L1ZDC_A", "HLT_noalg_mb_L1ZDC_AND", "HLT_noalg_mb_L1ZDC_A_C", "HLT_noalg_mb_L1ZDC_C",
    ]

disabled_Rules.update(dict(map(None,HLT_list,len(HLT_list)*[{
    1000 : TriggerRule(PS=-1, comment='Disabled Streamer'), 
    }])))

# --------------------------------------
# beam spot

HLT_list=[
'HLT_beamspot_activeTE_L2StarB_peb', 'HLT_beamspot_activeTE_L2StarB_pebTRT_L1TRT_EMPTY', 'HLT_beamspot_activeTE_L2StarB_pebTRT_L1TRT_FILLED', 'HLT_beamspot_activeTE_L2StarB_peb_L1TRT_EMPTY', 'HLT_beamspot_activeTE_L2StarB_peb_L1TRT_FILLED', 'HLT_beamspot_activeTE_trkfast_peb', 'HLT_beamspot_activeTE_trkfast_pebTRT_L1TRT_EMPTY', 'HLT_beamspot_activeTE_trkfast_pebTRT_L1TRT_FILLED', 'HLT_beamspot_activeTE_trkfast_peb_L1TRT_EMPTY', 'HLT_beamspot_activeTE_trkfast_peb_L1TRT_FILLED', 'HLT_beamspot_allTE_L2StarB', 'HLT_beamspot_allTE_L2StarB_L1TRT_EMPTY', 'HLT_beamspot_allTE_L2StarB_L1TRT_FILLED', 'HLT_beamspot_allTE_L2StarB_peb', 'HLT_beamspot_allTE_L2StarB_pebTRT_L1TRT_EMPTY', 'HLT_beamspot_allTE_L2StarB_pebTRT_L1TRT_FILLED', 'HLT_beamspot_allTE_L2StarB_peb_L1TRT_EMPTY', 'HLT_beamspot_allTE_L2StarB_peb_L1TRT_FILLED', 'HLT_beamspot_allTE_trkfast', 'HLT_beamspot_allTE_trkfast_L1TRT_EMPTY', 'HLT_beamspot_allTE_trkfast_L1TRT_FILLED', 'HLT_beamspot_allTE_trkfast_peb', 'HLT_beamspot_allTE_trkfast_pebTRT_L1TRT_EMPTY', 'HLT_beamspot_allTE_trkfast_pebTRT_L1TRT_FILLED', 'HLT_beamspot_allTE_trkfast_peb_L1TRT_EMPTY', 'HLT_beamspot_allTE_trkfast_peb_L1TRT_FILLED', 'HLT_beamspot_trkFS_L2StarB_L1TRT_EMPTY', 'HLT_beamspot_trkFS_L2StarB_L1TRT_FILLED', 'HLT_beamspot_trkFS_trkfast_L1TRT_EMPTY', 'HLT_beamspot_trkFS_trkfast_L1TRT_FILLED', ]
disabled_Rules.update(dict(map(None,HLT_list,len(HLT_list)*[{
    1000 : TriggerRule(PS=-1, comment='Disabled beamspot'), 
    }])))

# --------------------------------------
# calibration

HLT_list=[
'HLT_alfacalib_L1ALFA_A7L1_OD', 'HLT_alfacalib_L1ALFA_A7R1_OD', 'HLT_alfacalib_L1ALFA_B7L1_OD', 'HLT_alfacalib_L1ALFA_B7R1_OD', 'HLT_alfacalib_L1ALFA_ELAS', 'HLT_alfacalib_L1ALFA_ELAST15', 'HLT_alfacalib_L1ALFA_ELAST18', 'HLT_alfacalib_L1ALFA_SYS', 'HLT_alfacalib_L1ALFA_TRT', 'HLT_alfacalib_L1LHCF_ALFA_ANY_A', 'HLT_alfacalib_L1LHCF_ALFA_ANY_A_UNPAIRED_ISO', 'HLT_alfacalib_L1LHCF_ALFA_ANY_C', 'HLT_alfacalib_L1LHCF_ALFA_ANY_C_UNPAIRED_ISO', 'HLT_alfaidpeb_L1TRT_ALFA_ANY', 'HLT_cscmon_L1EM3', 'HLT_cscmon_L1J12', 'HLT_cscmon_L1MU10', 'HLT_ibllumi_L1RD0_ABORTGAPNOTCALIB', 'HLT_ibllumi_L1RD0_FILLED', 'HLT_ibllumi_L1RD0_UNPAIRED_ISO', 'HLT_id_cosmicid', 'HLT_id_cosmicid_trtxk', 'HLT_id_cosmicid_trtxk_central', 'HLT_l1calocalib', 'HLT_l1calocalib_L1BGRP9', 'HLT_larnoiseburst_L1All', 'HLT_larnoiseburst_loose_L1All', 'HLT_larnoiseburst_loose_rerun', 'HLT_lumipeb_vdm_L1MBTS_1', 'HLT_lumipeb_vdm_L1MBTS_1_BGRP11', 'HLT_lumipeb_vdm_L1MBTS_1_BGRP9', 'HLT_lumipeb_vdm_L1MBTS_1_UNPAIRED_ISO', 'HLT_lumipeb_vdm_L1MBTS_2', 'HLT_lumipeb_vdm_L1MBTS_2_BGRP11', 'HLT_lumipeb_vdm_L1MBTS_2_BGRP9', 'HLT_lumipeb_vdm_L1MBTS_2_UNPAIRED_ISO', 'HLT_lumipeb_vdm_L1RD0_BGRP11', 'HLT_lumipeb_vdm_L1RD0_BGRP9', 'HLT_lumipeb_vdm_L1RD0_FILLED', 'HLT_lumipeb_vdm_L1RD0_UNPAIRED_ISO',]
disabled_Rules.update(dict(map(None,HLT_list,len(HLT_list)*[{
    1000 : TriggerRule(PS=-1, comment='Disabled calib'), 
    }])))

# --------------------------------------
# minbias

HLT_list=[
'HLT_mb_mbts_L1MBTS_1', 'HLT_mb_mbts_L1MBTS_1_1', 'HLT_mb_mbts_L1MBTS_1_1_EMPTY', 'HLT_mb_mbts_L1MBTS_1_1_UNPAIRED_ISO', 'HLT_mb_mbts_L1MBTS_1_EMPTY', 'HLT_mb_mbts_L1MBTS_1_UNPAIRED_ISO', 'HLT_mb_mbts_L1MBTS_2', 'HLT_mb_mbts_L1MBTS_2_EMPTY', 'HLT_mb_mbts_L1MBTS_2_UNPAIRED_ISO', 'HLT_mb_perf_L1LUCID', 'HLT_mb_perf_L1LUCID_EMPTY', 'HLT_mb_perf_L1LUCID_UNPAIRED_ISO', 'HLT_mb_perf_L1MBTS_2', 'HLT_mb_perf_L1RD1_FILLED', 'HLT_mb_sp1000_trk70_hmt_L1MBTS_1_1', 'HLT_mb_sp1200_trk75_hmt_L1MBTS_1_1', 'HLT_mb_sp1400_hmtperf_L1TE10', 'HLT_mb_sp1400_hmtperf_L1TE10.0ETA24', 'HLT_mb_sp1400_hmtperf_L1TE20', 'HLT_mb_sp1400_hmtperf_L1TE20.0ETA24', 'HLT_mb_sp1400_trk100_hmt_L1TE20', 'HLT_mb_sp1400_trk100_hmt_L1TE20.0ETA24', 'HLT_mb_sp1400_trk80_hmt_L1MBTS_1_1', 'HLT_mb_sp1400_trk90_hmt_L1TE10', 'HLT_mb_sp1400_trk90_hmt_L1TE10.0ETA24', 'HLT_mb_sp1800_hmtperf', 'HLT_mb_sp1800_hmtperf_L1TE20', 'HLT_mb_sp2000_pusup600_trk70_hmt', 'HLT_mb_sp2000_pusup600_trk70_hmt_L1TE30', 'HLT_mb_sp2000_pusup600_trk70_hmt_L1TE40', 'HLT_mb_sp2000_pusup700_trk50_sumet110_hmt_L1TE40', 'HLT_mb_sp2000_pusup700_trk50_sumet150_hmt_L1TE50', 'HLT_mb_sp2000_pusup700_trk50_sumet70_hmt_L1TE30', 'HLT_mb_sp2000_pusup700_trk60_hmt_L1TE30', 'HLT_mb_sp2000_pusup700_trk70_hmt_L1TE30', 'HLT_mb_sp2000_pusup700_trk70_hmt_L1TE40', 'HLT_mb_sp2000_trk70_hmt', 'HLT_mb_sp2500_hmtperf_L1TE20', 'HLT_mb_sp2500_pusup750_trk90_hmt_L1TE40', 'HLT_mb_sp2_hmtperf', 'HLT_mb_sp2_hmtperf_L1MBTS_1_1', 'HLT_mb_sp3000_pusup800_trk120_hmt_L1TE50', 'HLT_mb_sp300_trk10_sumet40_hmt_L1MBTS_1_1', 'HLT_mb_sp300_trk10_sumet50_hmt_L1MBTS_1_1', 'HLT_mb_sp300_trk10_sumet50_hmt_L1RD3_FILLED', 'HLT_mb_sp300_trk10_sumet50_hmt_L1TE10', 'HLT_mb_sp300_trk10_sumet60_hmt_L1MBTS_1_1', 'HLT_mb_sp300_trk10_sumet60_hmt_L1TE20', 'HLT_mb_sp300_trk10_sumet70_hmt_L1MBTS_1_1', 'HLT_mb_sp300_trk10_sumet80_hmt_L1MBTS_1_1', 'HLT_mb_sp400_trk40_hmt_L1MBTS_1_1', 'HLT_mb_sp500_hmtperf', 'HLT_mb_sp600_trk10_sumet40_hmt_L1MBTS_1_1', 'HLT_mb_sp600_trk10_sumet50_hmt_L1MBTS_1_1', 'HLT_mb_sp600_trk10_sumet50_hmt_L1RD3_FILLED', 'HLT_mb_sp600_trk10_sumet50_hmt_L1TE10.0ETA24', 'HLT_mb_sp600_trk10_sumet60_hmt_L1MBTS_1_1', 'HLT_mb_sp600_trk10_sumet60_hmt_L1TE20.0ETA24', 'HLT_mb_sp600_trk10_sumet70_hmt_L1MBTS_1_1', 'HLT_mb_sp600_trk10_sumet80_hmt_L1MBTS_1_1', 'HLT_mb_sp600_trk45_hmt_L1MBTS_1_1', 'HLT_mb_sp700_trk50_hmt_L1MBTS_1_1', 'HLT_mb_sp700_trk50_hmt_L1RD3_FILLED', 'HLT_mb_sp700_trk55_hmt_L1MBTS_1_1', 'HLT_mb_sp900_trk60_hmt_L1MBTS_1_1', 'HLT_mb_sp900_trk65_hmt_L1MBTS_1_1', 'HLT_mb_sptrk','HLT_mb_sptrk_L1RD0_EMPTY', 'HLT_mb_sptrk_L1RD0_UNPAIRED_ISO', 'HLT_mb_sptrk_L1RD3_FILLED', 'HLT_mb_sptrk_costr', 'HLT_mb_sptrk_costr_L1RD0_EMPTY', 'HLT_mb_sptrk_noisesup', 'HLT_mb_sptrk_noisesup_L1RD0_EMPTY', 'HLT_mb_sptrk_noisesup_L1RD0_UNPAIRED_ISO', 'HLT_mb_sptrk_noisesup_L1RD3_FILLED', 'HLT_mb_sptrk_pt4_L1MBTS_1', 'HLT_mb_sptrk_pt4_L1MBTS_1_1', 'HLT_mb_sptrk_pt4_L1MBTS_2', 'HLT_mb_sptrk_pt4_L1RD3_FILLED', 'HLT_mb_sptrk_pt6_L1MBTS_1', 'HLT_mb_sptrk_pt6_L1MBTS_1_1', 'HLT_mb_sptrk_pt6_L1MBTS_2', 'HLT_mb_sptrk_pt6_L1RD3_FILLED', 'HLT_mb_sptrk_pt8_L1MBTS_1', 'HLT_mb_sptrk_pt8_L1MBTS_1_1', 'HLT_mb_sptrk_pt8_L1MBTS_2', 'HLT_mb_sptrk_pt8_L1RD3_FILLED', 'HLT_mb_sptrk_vetombts2in_L1ALFA_CEP', 'HLT_mb_sptrk_vetombts2in_peb_L1ALFA_ANY', 'HLT_mb_sptrk_vetombts2in_peb_L1ALFA_ANY_UNPAIRED_ISO', 'HLT_noalg_mb_L1LUCID', 'HLT_noalg_mb_L1LUCID_EMPTY', 'HLT_noalg_mb_L1LUCID_UNPAIRED_ISO', 'HLT_noalg_mb_L1MBTS_1', 'HLT_noalg_mb_L1MBTS_1_1', 'HLT_noalg_mb_L1MBTS_1_1_EMPTY', 'HLT_noalg_mb_L1MBTS_1_1_UNPAIRED_ISO', 'HLT_noalg_mb_L1MBTS_1_EMPTY', 'HLT_noalg_mb_L1MBTS_1_UNPAIRED_ISO', 'HLT_noalg_mb_L1MBTS_2', 'HLT_noalg_mb_L1MBTS_2_EMPTY', 'HLT_noalg_mb_L1MBTS_2_UNPAIRED_ISO', 'HLT_noalg_mb_L1RD0_EMPTY', 'HLT_noalg_mb_L1RD0_FILLED', 'HLT_noalg_mb_L1RD0_UNPAIRED_ISO', 'HLT_noalg_mb_L1RD1_FILLED', 'HLT_noalg_mb_L1RD2_EMPTY', 'HLT_noalg_mb_L1RD2_FILLED', 'HLT_noalg_mb_L1RD3_EMPTY', 'HLT_noalg_mb_L1RD3_FILLED', 'HLT_noalg_mb_L1TE10', 'HLT_noalg_mb_L1TE10.0ETA24', 'HLT_noalg_mb_L1TE20', 'HLT_noalg_mb_L1TE20.0ETA24', 'HLT_noalg_mb_L1TE30', 'HLT_noalg_mb_L1TE40', 'HLT_noalg_mb_L1TE50','HLT_mb_sp1800_hmtperf_L1TE30.0ETA24','HLT_noalg_mb_L1TE30.0ETA24','HLT_noalg_L1TE0', 'HLT_noalg_L1TE0.0ETA24', 'HLT_noalg_L1TE15', 'HLT_noalg_L1TE5', 'HLT_noalg_L1TE5.0ETA24', 'HLT_noalg_mb_L1TE0', 'HLT_noalg_mb_L1TE0.0ETA24', 'HLT_noalg_mb_L1TE15', 'HLT_noalg_mb_L1TE5', 'HLT_noalg_mb_L1TE5.0ETA24', 'HLT_mb_sp1300_hmtperf_L1TE10', 'HLT_mb_sp1300_hmtperf_L1TE10.0ETA24', 'HLT_mb_sp1300_hmtperf_L1TE15', 'HLT_mb_sp1300_hmtperf_L1TE20', 'HLT_mb_sp1300_hmtperf_L1TE20.0ETA24', 'HLT_mb_sp1300_hmtperf_L1TE5', 'HLT_mb_sp1300_hmtperf_L1TE5.0ETA24', 'HLT_mb_sp1400_pusup550_trk90_hmt_L1TE10', 'HLT_mb_sp1400_pusup550_trk90_hmt_L1TE10.0ETA24', 'HLT_mb_sp1400_pusup550_trk90_hmt_L1TE15', 'HLT_mb_sp1400_pusup550_trk90_hmt_L1TE20', 'HLT_mb_sp1400_pusup550_trk90_hmt_L1TE20.0ETA24', 'HLT_mb_sp1400_pusup550_trk90_hmt_L1TE5', 'HLT_mb_sp1400_pusup550_trk90_hmt_L1TE5.0ETA24', 'HLT_mb_sp1400_trk90_hmt_L1TE15', 'HLT_mb_sp1400_trk90_hmt_L1TE20', 'HLT_mb_sp1400_trk90_hmt_L1TE20.0ETA24', 'HLT_mb_sp1400_trk90_hmt_L1TE5', 'HLT_mb_sp1400_trk90_hmt_L1TE5.0ETA24', 'HLT_mb_sp1500_hmtperf_L1TE10', 'HLT_mb_sp1500_hmtperf_L1TE10.0ETA24', 'HLT_mb_sp1500_hmtperf_L1TE15', 'HLT_mb_sp1500_hmtperf_L1TE20', 'HLT_mb_sp1500_hmtperf_L1TE20.0ETA24', 'HLT_mb_sp1500_hmtperf_L1TE5', 'HLT_mb_sp1500_hmtperf_L1TE5.0ETA24', 'HLT_mb_sp1600_pusup550_trk100_hmt_L1TE10', 'HLT_mb_sp1600_pusup550_trk100_hmt_L1TE10.0ETA24', 'HLT_mb_sp1600_pusup550_trk100_hmt_L1TE15', 'HLT_mb_sp1600_pusup550_trk100_hmt_L1TE20', 'HLT_mb_sp1600_pusup550_trk100_hmt_L1TE20.0ETA24', 'HLT_mb_sp1600_pusup550_trk100_hmt_L1TE5', 'HLT_mb_sp1600_pusup550_trk100_hmt_L1TE5.0ETA24', 'HLT_mb_sp1600_trk100_hmt_L1TE10', 'HLT_mb_sp1600_trk100_hmt_L1TE10.0ETA24', 'HLT_mb_sp1600_trk100_hmt_L1TE15', 'HLT_mb_sp1600_trk100_hmt_L1TE20', 'HLT_mb_sp1600_trk100_hmt_L1TE20.0ETA24', 'HLT_mb_sp1600_trk100_hmt_L1TE5', 'HLT_mb_sp1600_trk100_hmt_L1TE5.0ETA24', 'HLT_mb_sp1700_hmtperf_L1TE10', 'HLT_mb_sp1700_hmtperf_L1TE10.0ETA24', 'HLT_mb_sp1700_hmtperf_L1TE15', 'HLT_mb_sp1700_hmtperf_L1TE20', 'HLT_mb_sp1700_hmtperf_L1TE20.0ETA24', 'HLT_mb_sp1700_hmtperf_L1TE5', 'HLT_mb_sp1700_hmtperf_L1TE5.0ETA24', 'HLT_mb_sp1800_pusup600_trk110_hmt_L1TE10', 'HLT_mb_sp1800_pusup600_trk110_hmt_L1TE10.0ETA24', 'HLT_mb_sp1800_pusup600_trk110_hmt_L1TE15', 'HLT_mb_sp1800_pusup600_trk110_hmt_L1TE20', 'HLT_mb_sp1800_pusup600_trk110_hmt_L1TE20.0ETA24', 'HLT_mb_sp1800_pusup600_trk110_hmt_L1TE5', 'HLT_mb_sp1800_pusup600_trk110_hmt_L1TE5.0ETA24', 'HLT_mb_sp1800_trk110_hmt_L1TE10', 'HLT_mb_sp1800_trk110_hmt_L1TE10.0ETA24', 'HLT_mb_sp1800_trk110_hmt_L1TE15', 'HLT_mb_sp1800_trk110_hmt_L1TE20', 'HLT_mb_sp1800_trk110_hmt_L1TE20.0ETA24', 'HLT_mb_sp1800_trk110_hmt_L1TE5', 'HLT_mb_sp1800_trk110_hmt_L1TE5.0ETA24', 'HLT_mb_sp2_hmtperf_L1TE0', 'HLT_mb_sp2_hmtperf_L1TE0.0ETA24', 'HLT_mb_sp900_pusup350_trk60_hmt_L1TE10', 'HLT_mb_sp900_pusup350_trk60_hmt_L1TE10.0ETA24', 'HLT_mb_sp900_pusup350_trk60_hmt_L1TE5', 'HLT_mb_sp900_pusup350_trk60_hmt_L1TE5.0ETA24', 'HLT_mb_sp900_trk60_hmt_L1TE10', 'HLT_mb_sp900_trk60_hmt_L1TE10.0ETA24', 'HLT_mb_sp900_trk60_hmt_L1TE5', 'HLT_mb_sp900_trk60_hmt_L1TE5.0ETA24', 'HLT_lumipeb_L1MBTS_2', 'HLT_mb_sptrk_vetombts2in_L1TRT_ALFA_EINE', 'HLT_mb_sptrk_vetombts2in_peb_L1TRT_ALFA_ANY', 'HLT_mb_sptrk_vetombts2in_peb_L1TRT_ALFA_ANY_UNPAIRED_ISO','mb_sptrk_vetombts2in_L1TRT_ALFA_EINE','mb_sptrk_vetombts2in_peb_L1TRT_ALFA_ANY','mb_sptrk_vetombts2in_peb_L1TRT_ALFA_ANY_UNPAIRED_ISO','lumipeb_L1MBTS_2', 'HLT_lumipeb_L1ALFA_BGT', 'HLT_lumipeb_L1ALFA_BGT_BGRP10', 'HLT_lumipeb_L1ALFA_BGT_UNPAIRED_ISO', 'HLT_lumipeb_L1MBTS_1', 'HLT_lumipeb_L1MBTS_1_UNPAIRED_ISO', 'HLT_lumipeb_L1MBTS_2_UNPAIRED_ISO', 'HLT_mb_sp1300_hmtperf_L1TE3', 'HLT_mb_sp1300_hmtperf_L1TE3.0ETA24', 'HLT_mb_sp1400_pusup550_trk90_hmt_L1TE25', 'HLT_mb_sp1500_pusup700_trk100_hmt_L1TE10', 'HLT_mb_sp1500_pusup700_trk100_hmt_L1TE10.0ETA24', 'HLT_mb_sp1500_pusup700_trk100_hmt_L1TE15', 'HLT_mb_sp1500_pusup700_trk100_hmt_L1TE20', 'HLT_mb_sp1500_pusup700_trk100_hmt_L1TE20.0ETA24', 'HLT_mb_sp1500_pusup700_trk100_hmt_L1TE25', 'HLT_mb_sp1500_pusup700_trk100_hmt_L1TE5', 'HLT_mb_sp1500_pusup700_trk100_hmt_L1TE5.0ETA24', 'HLT_mb_sp1500_trk100_hmt_L1TE10', 'HLT_mb_sp1500_trk100_hmt_L1TE10.0ETA24', 'HLT_mb_sp1500_trk100_hmt_L1TE15', 'HLT_mb_sp1500_trk100_hmt_L1TE20', 'HLT_mb_sp1500_trk100_hmt_L1TE20.0ETA24', 'HLT_mb_sp1500_trk100_hmt_L1TE5', 'HLT_mb_sp1500_trk100_hmt_L1TE5.0ETA24', 'HLT_mb_sp1800_pusup800_trk110_hmt_L1TE10', 'HLT_mb_sp1800_pusup800_trk110_hmt_L1TE10.0ETA24', 'HLT_mb_sp1800_pusup800_trk110_hmt_L1TE15', 'HLT_mb_sp1800_pusup800_trk110_hmt_L1TE20', 'HLT_mb_sp1800_pusup800_trk110_hmt_L1TE20.0ETA24', 'HLT_mb_sp1800_pusup800_trk110_hmt_L1TE25', 'HLT_mb_sp1800_pusup800_trk110_hmt_L1TE5', 'HLT_mb_sp1800_pusup800_trk110_hmt_L1TE5.0ETA24', 'HLT_mb_sp2100_pusup1000_trk120_hmt_L1TE10', 'HLT_mb_sp2100_pusup1000_trk120_hmt_L1TE15', 'HLT_mb_sp2100_pusup1000_trk120_hmt_L1TE20', 'HLT_mb_sp2100_pusup1000_trk120_hmt_L1TE25', 'HLT_mb_sp2400_pusup1100_trk130_hmt_L1TE10', 'HLT_mb_sp2400_pusup1100_trk130_hmt_L1TE15', 'HLT_mb_sp2400_pusup1100_trk130_hmt_L1TE20', 'HLT_mb_sp2400_pusup1100_trk130_hmt_L1TE25', 'HLT_mb_sp2700_pusup1300_trk140_hmt_L1TE10', 'HLT_mb_sp2700_pusup1300_trk140_hmt_L1TE15', 'HLT_mb_sp2700_pusup1300_trk140_hmt_L1TE20', 'HLT_mb_sp2700_pusup1300_trk140_hmt_L1TE25', 'HLT_mb_sp2_hmtperf_L1TE3', 'HLT_mb_sp2_hmtperf_L1TE3.0ETA24', 'HLT_mb_sp3000_pusup1400_trk150_hmt_L1TE10', 'HLT_mb_sp3000_pusup1400_trk150_hmt_L1TE15', 'HLT_mb_sp3000_pusup1400_trk150_hmt_L1TE20', 'HLT_mb_sp3000_pusup1400_trk150_hmt_L1TE25', 'HLT_mb_sp3000_pusup1400_trk150_hmt_L1TE30', 'HLT_mb_sp3000_pusup1500_trk160_hmt_L1TE10', 'HLT_mb_sp3000_pusup1500_trk160_hmt_L1TE15', 'HLT_mb_sp3000_pusup1500_trk160_hmt_L1TE20', 'HLT_mb_sp3000_pusup1500_trk160_hmt_L1TE25', 'HLT_mb_sp3000_pusup1500_trk160_hmt_L1TE30', 'HLT_mb_sptrk_vetombts2in_L1ALFA_ANY', 'HLT_mb_sptrk_vetombts2in_L1ALFA_ANY_UNPAIRED_ISO', 'HLT_mb_sptrk_vetombts2in_L1TRT_ALFA_ANY', 'HLT_mb_sptrk_vetombts2in_L1TRT_ALFA_ANY_UNPAIRED_ISO', 'HLT_noalg_L1ALFA_CDiff_Phys', 'HLT_noalg_L1ALFA_Diff_Phys', 'HLT_noalg_L1ALFA_Jet_Phys', 'HLT_noalg_L1TE15.0ETA24', 'HLT_noalg_L1TE25', 'HLT_noalg_L1TE25.0ETA24', 'HLT_noalg_L1TE3', 'HLT_noalg_L1TE3.0ETA24', 'HLT_noalg_mb_L1TE15.0ETA24', 'HLT_noalg_mb_L1TE25', 'HLT_noalg_mb_L1TE25.0ETA24', 'HLT_noalg_mb_L1TE3', 'HLT_noalg_mb_L1TE3.0ETA24', 'HLT_noalg_mb_L1TE40.0ETA24', ]
disabled_Rules.update(dict(map(None,HLT_list,len(HLT_list)*[{
    1000 : TriggerRule(PS=-1, comment='Disabled mb'), 
    }])))




# --------------------------------------
# L1Topo seeded
# need to be all updated.

HLT_list=[
'HLT_mu11_2mu4noL1_nscan03_L1MU11_LFV-MU'
 'HLT_mu11_L1MU10_2mu4noL1_nscan03'
 'HLT_mu14_tau25_medium1_tracktwo_L1DR-MU10TAU12I_TAU12I-J25'
 'HLT_2j40_0eta490_invm250'
 'HLT_2j55_bloose_L13J20_4J20.0ETA49_MJJ-400'
 'HLT_2j55_bloose_L13J20_4J20.0ETA49_MJJ-700'
 'HLT_2j55_bloose_L13J20_4J20.0ETA49_MJJ-800'
 'HLT_2j55_bloose_L13J20_4J20.0ETA49_MJJ-900'
 'HLT_2j55_bloose_L1J30_2J20_4J20.0ETA49_MJJ-400'
 'HLT_2j55_bloose_L1J30_2J20_4J20.0ETA49_MJJ-700'
 'HLT_2j55_bloose_L1J30_2J20_4J20.0ETA49_MJJ-800'
 'HLT_2j55_bloose_L1J30_2J20_4J20.0ETA49_MJJ-900'
 'HLT_2j55_bmedium_L13J20_4J20.0ETA49_MJJ-400'
 'HLT_2j55_bmedium_L13J20_4J20.0ETA49_MJJ-700'
 'HLT_2j55_bmedium_L13J20_4J20.0ETA49_MJJ-800'
 'HLT_2j55_bmedium_L13J20_4J20.0ETA49_MJJ-900'
 'HLT_2j55_bmedium_L1J30_2J20_4J20.0ETA49_MJJ-400'
 'HLT_2j55_bmedium_L1J30_2J20_4J20.0ETA49_MJJ-700'
 'HLT_2j55_bmedium_L1J30_2J20_4J20.0ETA49_MJJ-800'
 'HLT_2j55_bmedium_L1J30_2J20_4J20.0ETA49_MJJ-900'
 'HLT_2j55_bperf_L13J20_4J20.0ETA49_MJJ-400'
 'HLT_2j55_bperf_L13J20_4J20.0ETA49_MJJ-700'
 'HLT_2j55_bperf_L13J20_4J20.0ETA49_MJJ-800'
 'HLT_2j55_bperf_L13J20_4J20.0ETA49_MJJ-900'
 'HLT_2j55_bperf_L1J30_2J20_4J20.0ETA49_MJJ-400'
 'HLT_2j55_bperf_L1J30_2J20_4J20.0ETA49_MJJ-700'
 'HLT_2j55_bperf_L1J30_2J20_4J20.0ETA49_MJJ-800'
 'HLT_2j55_bperf_L1J30_2J20_4J20.0ETA49_MJJ-900'
 'HLT_2mu4_bBmumu_L1BPH-2M-2MU4'
 'HLT_2mu4_bBmumu_L1BPH-2M-2MU4-B'
 'HLT_2mu4_bBmumu_L1BPH-2M-2MU4-BO'
 'HLT_2mu4_bBmumu_L1BPH-4M8-2MU4'
 'HLT_2mu4_bBmumu_L1BPH-4M8-2MU4-B'
 'HLT_2mu4_bBmumu_L1BPH-4M8-2MU4-BO'
 'HLT_2mu4_bBmumu_L1BPH-DR-2MU4'
 'HLT_2mu4_bBmumu_L1BPH-DR-2MU4-B'
# 'HLT_2mu4_bBmumu_L1BPH-DR-2MU4-B-BPH-2M-2MU4-B'
# 'HLT_2mu4_bBmumu_L1BPH-DR-2MU4-B-BPH-4M8-2MU4-B'
 'HLT_2mu4_bBmumu_L1BPH-DR-2MU4-BO'
# 'HLT_2mu4_bBmumu_L1BPH-DR-2MU4-BO-BPH-2M-2MU4-BO'
# 'HLT_2mu4_bBmumu_L1BPH-DR-2MU4-BO-BPH-4M8-2MU4-BO'
# 'HLT_2mu4_bBmumu_L1BPH-DR-2MU4-BPH-2M-2MU4'
# 'HLT_2mu4_bBmumu_L1BPH-DR-2MU4-BPH-4M8-2MU4'
# 'HLT_2mu4_bBmumux_BcmumuDsloose_L1BPH-2M-2MU4'
# 'HLT_2mu4_bBmumux_BcmumuDsloose_L1BPH-2M-2MU4-B'
# 'HLT_2mu4_bBmumux_BcmumuDsloose_L1BPH-2M-2MU4-BO'
# 'HLT_2mu4_bBmumux_BcmumuDsloose_L1BPH-4M8-2MU4'
# 'HLT_2mu4_bBmumux_BcmumuDsloose_L1BPH-4M8-2MU4-B'
# 'HLT_2mu4_bBmumux_BcmumuDsloose_L1BPH-4M8-2MU4-BO'
 'HLT_2mu4_bBmumux_BcmumuDsloose_L1BPH-DR-2MU4'
 'HLT_2mu4_bBmumux_BcmumuDsloose_L1BPH-DR-2MU4-B'
# 'HLT_2mu4_bBmumux_BcmumuDsloose_L1BPH-DR-2MU4-B-BPH-2M-2MU4-B'
# 'HLT_2mu4_bBmumux_BcmumuDsloose_L1BPH-DR-2MU4-B-BPH-4M8-2MU4-B'
 'HLT_2mu4_bBmumux_BcmumuDsloose_L1BPH-DR-2MU4-BO'
# 'HLT_2mu4_bBmumux_BcmumuDsloose_L1BPH-DR-2MU4-BO-BPH-2M-2MU4-BO'
# 'HLT_2mu4_bBmumux_BcmumuDsloose_L1BPH-DR-2MU4-BO-BPH-4M8-2MU4-BO'
# 'HLT_2mu4_bBmumux_BcmumuDsloose_L1BPH-DR-2MU4-BPH-2M-2MU4'
# 'HLT_2mu4_bBmumux_BcmumuDsloose_L1BPH-DR-2MU4-BPH-4M8-2MU4'
 'HLT_2mu4_bBmumuxv2_L1BPH-2M-2MU4'
 'HLT_2mu4_bBmumuxv2_L1BPH-2M-2MU4-B'
 'HLT_2mu4_bBmumuxv2_L1BPH-2M-2MU4-BO'
 'HLT_2mu4_bBmumuxv2_L1BPH-4M8-2MU4'
 'HLT_2mu4_bBmumuxv2_L1BPH-4M8-2MU4-B'
 'HLT_2mu4_bBmumuxv2_L1BPH-4M8-2MU4-BO'
 'HLT_2mu4_bBmumuxv2_L1BPH-DR-2MU4'
 'HLT_2mu4_bBmumuxv2_L1BPH-DR-2MU4-B'
# 'HLT_2mu4_bBmumuxv2_L1BPH-DR-2MU4-B-BPH-2M-2MU4-B'
# 'HLT_2mu4_bBmumuxv2_L1BPH-DR-2MU4-B-BPH-4M8-2MU4-B'
 'HLT_2mu4_bBmumuxv2_L1BPH-DR-2MU4-BO'
# 'HLT_2mu4_bBmumuxv2_L1BPH-DR-2MU4-BO-BPH-2M-2MU4-BO'
# 'HLT_2mu4_bBmumuxv2_L1BPH-DR-2MU4-BO-BPH-4M8-2MU4-BO'
# 'HLT_2mu4_bBmumuxv2_L1BPH-DR-2MU4-BPH-2M-2MU4'
# 'HLT_2mu4_bBmumuxv2_L1BPH-DR-2MU4-BPH-4M8-2MU4'
 'HLT_2mu4_bDimu_L1BPH-2M-2MU4'
 'HLT_2mu4_bDimu_L1BPH-2M-2MU4-B'
 'HLT_2mu4_bDimu_L1BPH-2M-2MU4-BO'
 'HLT_2mu4_bDimu_L1BPH-4M8-2MU4'
 'HLT_2mu4_bDimu_L1BPH-4M8-2MU4-B'
 'HLT_2mu4_bDimu_L1BPH-4M8-2MU4-BO'
 'HLT_2mu4_bDimu_L1BPH-DR-2MU4'
 'HLT_2mu4_bDimu_L1BPH-DR-2MU4-B'
# 'HLT_2mu4_bDimu_L1BPH-DR-2MU4-B-BPH-2M-2MU4-B'
# 'HLT_2mu4_bDimu_L1BPH-DR-2MU4-B-BPH-4M8-2MU4-B'
 'HLT_2mu4_bDimu_L1BPH-DR-2MU4-BO'
# 'HLT_2mu4_bDimu_L1BPH-DR-2MU4-BO-BPH-2M-2MU4-BO'
# 'HLT_2mu4_bDimu_L1BPH-DR-2MU4-BO-BPH-4M8-2MU4-BO'
 'HLT_2mu4_bDimu_L1BPH-DR-2MU4-BPH-2M-2MU4'
 'HLT_2mu4_bDimu_L1BPH-DR-2MU4-BPH-4M8-2MU4'
 'HLT_2mu4_bDimu_novtx_noos_L1BPH-2M-2MU4'
 'HLT_2mu4_bDimu_novtx_noos_L1BPH-2M-2MU4-B'
 'HLT_2mu4_bDimu_novtx_noos_L1BPH-2M-2MU4-BO'
 'HLT_2mu4_bDimu_novtx_noos_L1BPH-4M8-2MU4'
 'HLT_2mu4_bDimu_novtx_noos_L1BPH-4M8-2MU4-B'
 'HLT_2mu4_bDimu_novtx_noos_L1BPH-4M8-2MU4-BO'
 'HLT_2mu4_bDimu_novtx_noos_L1BPH-DR-2MU4'
 'HLT_2mu4_bDimu_novtx_noos_L1BPH-DR-2MU4-B'
# 'HLT_2mu4_bDimu_novtx_noos_L1BPH-DR-2MU4-B-BPH-2M-2MU4-B'
# 'HLT_2mu4_bDimu_novtx_noos_L1BPH-DR-2MU4-B-BPH-4M8-2MU4-B'
 'HLT_2mu4_bDimu_novtx_noos_L1BPH-DR-2MU4-BO'
# 'HLT_2mu4_bDimu_novtx_noos_L1BPH-DR-2MU4-BO-BPH-2M-2MU4-BO'
# 'HLT_2mu4_bDimu_novtx_noos_L1BPH-DR-2MU4-BO-BPH-4M8-2MU4-BO'
# 'HLT_2mu4_bDimu_novtx_noos_L1BPH-DR-2MU4-BPH-2M-2MU4'
# 'HLT_2mu4_bDimu_novtx_noos_L1BPH-DR-2MU4-BPH-4M8-2MU4'
 'HLT_2mu4_bJpsimumu_L1BPH-2M-2MU4'
 'HLT_2mu4_bJpsimumu_L1BPH-2M-2MU4-B'
 'HLT_2mu4_bJpsimumu_L1BPH-2M-2MU4-BO'
 'HLT_2mu4_bJpsimumu_L1BPH-DR-2MU4'
 'HLT_2mu4_bJpsimumu_L1BPH-DR-2MU4-B'
# 'HLT_2mu4_bJpsimumu_L1BPH-DR-2MU4-B-BPH-2M-2MU4-B'
 'HLT_2mu4_bJpsimumu_L1BPH-DR-2MU4-BO'
# 'HLT_2mu4_bJpsimumu_L1BPH-DR-2MU4-BO-BPH-2M-2MU4-BO'
# 'HLT_2mu4_bJpsimumu_L1BPH-DR-2MU4-BPH-2M-2MU4'
 'HLT_2mu4_bUpsimumu_L1BPH-2M-2MU4'
 'HLT_2mu4_bUpsimumu_L1BPH-2M-2MU4-B'
 'HLT_2mu4_bUpsimumu_L1BPH-2M-2MU4-BO'
 'HLT_2mu6_bBmumu_L1BPH-2M-2MU6'
 'HLT_2mu6_bBmumu_L1BPH-4M8-2MU6'
 'HLT_2mu6_bBmumu_L1BPH-DR-2MU6'
# 'HLT_2mu6_bBmumu_L1BPH-DR-2MU6-BPH-2M-2MU6'
# 'HLT_2mu6_bBmumu_L1BPH-DR-2MU6-BPH-4M8-2MU6'
# 'HLT_2mu6_bBmumux_BcmumuDsloose_L1BPH-2M-2MU6'
 'HLT_2mu6_bBmumux_BcmumuDsloose_L1BPH-4M8-2MU6'
 'HLT_2mu6_bBmumux_BcmumuDsloose_L1BPH-DR-2MU6'
# 'HLT_2mu6_bBmumux_BcmumuDsloose_L1BPH-DR-2MU6-BPH-2M-2MU6'
# 'HLT_2mu6_bBmumux_BcmumuDsloose_L1BPH-DR-2MU6-BPH-4M8-2MU6'
# 'HLT_2mu6_bBmumuxv2_L1BPH-2M-2MU6'
 'HLT_2mu6_bBmumuxv2_L1BPH-4M8-2MU6'
 'HLT_2mu6_bBmumuxv2_L1BPH-DR-2MU6'
# 'HLT_2mu6_bBmumuxv2_L1BPH-DR-2MU6-BPH-2M-2MU6'
# 'HLT_2mu6_bBmumuxv2_L1BPH-DR-2MU6-BPH-4M8-2MU6'
 'HLT_2mu6_bDimu_L1BPH-2M-2MU6'
 'HLT_2mu6_bDimu_L1BPH-4M8-2MU6'
 'HLT_2mu6_bDimu_L1BPH-DR-2MU6'
# 'HLT_2mu6_bDimu_L1BPH-DR-2MU6-BPH-2M-2MU6'
# 'HLT_2mu6_bDimu_L1BPH-DR-2MU6-BPH-4M8-2MU6'
 'HLT_2mu6_bDimu_novtx_noos_L1BPH-2M-2MU6'
 'HLT_2mu6_bDimu_novtx_noos_L1BPH-4M8-2MU6'
 'HLT_2mu6_bDimu_novtx_noos_L1BPH-DR-2MU6'
# 'HLT_2mu6_bDimu_novtx_noos_L1BPH-DR-2MU6-BPH-2M-2MU6'
# 'HLT_2mu6_bDimu_novtx_noos_L1BPH-DR-2MU6-BPH-4M8-2MU6'
 'HLT_2mu6_bJpsimumu_L1BPH-2M-2MU6'
 'HLT_2mu6_bJpsimumu_L1BPH-DR-2MU6'
# 'HLT_2mu6_bJpsimumu_L1BPH-DR-2MU6-BPH-2M-2MU6'
 'HLT_2mu6_bUpsimumu_L1BPH-2M-2MU6'
 'HLT_e13_etcut_L1EM10_W-MT25'
 'HLT_e13_etcut_L1EM10_W-MT30'
 'HLT_e13_etcut_L1W-NOMATCH'
 'HLT_e13_etcut_L1W-NOMATCH_W-05RO-XEEMHT'
 'HLT_e13_etcut_trkcut'
 'HLT_e13_etcut_trkcut_L1EM10_W-MT25_W-15DPHI-JXE-0_W-15DPHI-EMXE'
 'HLT_e13_etcut_trkcut_L1EM10_W-MT25_W-15DPHI-JXE-0_W-15DPHI-EMXE_XS20_xe20_mt25'
 'HLT_e13_etcut_trkcut_j20_perf_xe15_6dphi05_mt25_L1EM10_W-MT25_W-15DPHI-JXE-0_W-15DPHI-EMXE_W-90RO2-XEHT-0'
 'HLT_e13_etcut_trkcut_j20_perf_xe15_6dphi05_mt25_L1EM10_W-MT25_W-15DPHI-JXE-0_W-15DPHI-EMXE_XS20'
 'HLT_e13_etcut_trkcut_xe20'
 'HLT_e13_etcut_trkcut_xe20_L1EM10_W-MT25_W-15DPHI-JXE-0_W-15DPHI-EMXE_XS20'
 'HLT_e13_etcut_trkcut_xe20_mt25'
 'HLT_e13_etcut_trkcut_xs15_L1EM10_W-MT25_W-15DPHI-JXE-0_W-15DPHI-EMXE_XS20'
 'HLT_e14_etcut_e5_lhtight_Jpsiee_L1JPSI-1M5-EM12'
 'HLT_e14_etcut_e5_lhtight_nod0_Jpsiee_L1JPSI-1M5-EM12'
 'HLT_e14_etcut_e5_tight_Jpsiee_L1JPSI-1M5-EM12'
 'HLT_e14_lhtight_e4_etcut_Jpsiee_L1JPSI-1M5-EM12'
 'HLT_e14_lhtight_nod0_e4_etcut_Jpsiee_L1JPSI-1M5-EM12'
 'HLT_e14_tight_e4_etcut_Jpsiee_L1JPSI-1M5-EM12'
 'HLT_e17_lhmedium_iloose_tau25_medium1_tracktwo_L1DR-EM15TAU12I-J25'
 'HLT_e17_lhmedium_nod0_iloose_tau25_medium1_tracktwo_L1DR-EM15TAU12I-J25'
 'HLT_e17_lhmedium_nod0_tau25_medium1_tracktwo_L1DR-EM15TAU12I-J25'
 'HLT_e17_lhmedium_nod0_tau25_medium1_tracktwo_L1EM15-TAU12I'
 'HLT_e17_lhmedium_nod0_tau25_medium1_tracktwo_L1EM15TAU12I-J25'
 'HLT_e17_lhmedium_nod0_tau25_medium1_tracktwo_xe50_L1XE35_EM15-TAU12I'
 'HLT_e17_lhmedium_nod0_tau25_medium1_tracktwo_xe50_L1XE40_EM15-TAU12I'
 'HLT_e17_lhmedium_nod0_tau80_medium1_tracktwo_L1EM15-TAU40'
 'HLT_e17_lhmedium_tau25_medium1_tracktwo_L1DR-EM15TAU12I-J25'
 'HLT_e17_lhmedium_tau25_medium1_tracktwo_L1EM15-TAU12I'
 'HLT_e17_lhmedium_tau25_medium1_tracktwo_L1EM15TAU12I-J25'
 'HLT_e17_lhmedium_tau25_medium1_tracktwo_xe50_L1XE35_EM15-TAU12I'
 'HLT_e17_lhmedium_tau25_medium1_tracktwo_xe50_L1XE40_EM15-TAU12I'
 'HLT_e17_lhmedium_tau80_medium1_tracktwo_L1EM15-TAU40'
 'HLT_e17_medium_iloose_tau25_medium1_tracktwo_L1DR-EM15TAU12I-J25'
 'HLT_e17_medium_tau25_medium1_tracktwo_L1DR-EM15TAU12I-J25'
 'HLT_e17_medium_tau25_medium1_tracktwo_L1EM15-TAU12I'
 'HLT_e17_medium_tau25_medium1_tracktwo_L1EM15TAU12I-J25'
 'HLT_e17_medium_tau25_medium1_tracktwo_xe50_L1XE35_EM15-TAU12I'
 'HLT_e17_medium_tau25_medium1_tracktwo_xe50_L1XE40_EM15-TAU12I'
 'HLT_e17_medium_tau80_medium1_tracktwo_L1EM15-TAU40'
 'HLT_e18_etcut_L1EM15_W-MT35'
 'HLT_e18_etcut_trkcut'
 'HLT_e18_etcut_trkcut_L1EM15_W-MT35_W-05DPHI-JXE-0_W-05DPHI-EMXE'
 'HLT_e18_etcut_trkcut_L1EM15_W-MT35_W-05DPHI-JXE-0_W-05DPHI-EMXE_XS30_xe35_mt35'
 'HLT_e18_etcut_trkcut_j20_perf_xe20_6dphi15_mt35_L1EM15_W-MT35_W-05DPHI-JXE-0_W-05DPHI-EMXE_W-250RO2-XEHT-0'
 'HLT_e18_etcut_trkcut_j20_perf_xe20_6dphi15_mt35_L1EM15_W-MT35_W-05DPHI-JXE-0_W-05DPHI-EMXE_XS30'
 'HLT_e18_etcut_trkcut_xe35'
 'HLT_e18_etcut_trkcut_xe35_L1EM15_W-MT35_W-05DPHI-JXE-0_W-05DPHI-EMXE_XS30'
 'HLT_e18_etcut_trkcut_xe35_mt35'
 'HLT_e5_etcut_L1W-05DPHI-EMXE-1'
 'HLT_e5_etcut_L1W-05DPHI-JXE-0'
 'HLT_e5_etcut_L1W-05RO-XEHT-0'
 'HLT_e5_etcut_L1W-10DPHI-EMXE-0'
 'HLT_e5_etcut_L1W-10DPHI-JXE-0'
 'HLT_e5_etcut_L1W-15DPHI-EMXE-0'
 'HLT_e5_etcut_L1W-15DPHI-JXE-0'
 'HLT_e5_etcut_L1W-250RO2-XEHT-0'
 'HLT_e5_etcut_L1W-90RO2-XEHT-0'
 'HLT_e5_etcut_L1W-HT20-JJ15.ETA49'
 'HLT_e5_lhtight_e4_etcut_Jpsiee_L1JPSI-1M5'
 'HLT_e5_lhtight_e4_etcut_L1JPSI-1M5'
 'HLT_e5_lhtight_nod0_e4_etcut_Jpsiee_L1JPSI-1M5'
 'HLT_e5_lhtight_nod0_e4_etcut_L1JPSI-1M5'
 'HLT_e5_tight_e4_etcut_Jpsiee_L1JPSI-1M5'
 'HLT_e5_tight_e4_etcut_L1JPSI-1M5'
 'HLT_e9_etcut_e5_lhtight_Jpsiee_L1JPSI-1M5-EM7'
 'HLT_e9_etcut_e5_lhtight_nod0_Jpsiee_L1JPSI-1M5-EM7'
 'HLT_e9_etcut_e5_tight_Jpsiee_L1JPSI-1M5-EM7'
 'HLT_e9_lhtight_e4_etcut_Jpsiee_L1JPSI-1M5-EM7'
 'HLT_e9_lhtight_nod0_e4_etcut_Jpsiee_L1JPSI-1M5-EM7'
 'HLT_e9_tight_e4_etcut_Jpsiee_L1JPSI-1M5-EM7'
 'HLT_g10_etcut_mu10_L1LFV-EM8I'
 'HLT_g10_etcut_mu10_iloose_taumass_L1LFV-EM8I'
 'HLT_g10_etcut_mu10_taumass'
 'HLT_g10_loose_mu10_iloose_taumass_L1LFV-EM8I'
 'HLT_g15_loose_2j40_0eta490_3j25_0eta490'
 'HLT_g20_etcut_mu4_L1LFV-EM15I'
 'HLT_g20_etcut_mu4_iloose_taumass_L1LFV-EM15I'
 'HLT_g20_etcut_mu4_taumass'
 'HLT_g20_loose_2j40_0eta490_3j25_0eta490'
 'HLT_g20_loose_2j40_0eta490_3j25_0eta490_L1MJJ-700'
 'HLT_g20_loose_2j40_0eta490_3j25_0eta490_L1MJJ-900'
 'HLT_g20_loose_mu4_iloose_taumass_L1LFV-EM15I'
 'HLT_ht1000'
 'HLT_ht1000_L1HT190-J15.ETA21'
 'HLT_ht400'
 'HLT_ht400_L1HT150-J20.ETA31'
 'HLT_ht550'
 'HLT_ht550_L1HT150-J20.ETA31'
 'HLT_ht700'
 'HLT_ht700_L1HT190-J15.ETA21'
 'HLT_ht850'
 'HLT_ht850_L1HT190-J15.ETA21'
 'HLT_j100_xe80_L1J40_DPHI-CJ20XE50'
 'HLT_j100_xe80_L1J40_DPHI-J20XE50'
 'HLT_j100_xe80_L1J40_DPHI-J20s2XE50'
 'HLT_j100_xe80_L1J40_DPHI-Js2XE50'
 'HLT_j360_a10_lcw_nojcalib'
 'HLT_j360_a10_lcw_nojcalib_L1HT150-J20.ETA31'
 'HLT_j360_a10_lcw_sub'
 'HLT_j360_a10_lcw_sub_L1HT150-J20.ETA31'
 'HLT_j360_a10_nojcalib'
 'HLT_j360_a10_nojcalib_L1HT150-J20.ETA31'
 'HLT_j360_a10_sub'
 'HLT_j360_a10_sub_L1HT150-J20.ETA31'
 'HLT_j360_a10r'
 'HLT_j460_a10_lcw_nojcalib'
 'HLT_j460_a10_lcw_nojcalib_L1HT190-J15.ETA21'
 'HLT_j460_a10_lcw_sub'
 'HLT_j460_a10_lcw_sub_L1HT190-J15.ETA21'
 'HLT_j460_a10_nojcalib'
 'HLT_j460_a10_nojcalib_L1HT190-J15.ETA21'
 'HLT_j460_a10_sub'
 'HLT_j460_a10_sub_L1HT190-J15.ETA21'
 'HLT_j460_a10r'
 'HLT_j80_xe80_1dphi10_L1J40_DPHI-CJ20XE50'
 'HLT_j80_xe80_1dphi10_L1J40_DPHI-J20XE50'
 'HLT_j80_xe80_1dphi10_L1J40_DPHI-J20s2XE50'
 'HLT_j80_xe80_1dphi10_L1J40_DPHI-Js2XE50'
 'HLT_mu14_tau25_medium1_tracktwo_L1DR-MU10TAU12I'
 'HLT_mu14_tau25_medium1_tracktwo_L1DR-MU10TAU12I_TAU12-J25'
 'HLT_mu14_tau25_medium1_tracktwo_L1MU10_TAU12I-J25'
 'HLT_mu4_iloose_mu4_11invm60_noos_L1DY-BOX-2MU4'
 'HLT_mu4_iloose_mu4_11invm60_noos_novtx_L1DY-BOX-2MU4'
 'HLT_mu4_iloose_mu4_7invm9_noos_L1DY-BOX-2MU4'
 'HLT_mu4_iloose_mu4_7invm9_noos_novtx_L1DY-BOX-2MU4'
 'HLT_mu4_j60_dr05_2j35_L13J15_BTAG-MU4J30'
 'HLT_mu4_j70_dr05_L1BTAG-MU4J30'
 'HLT_mu6_2j40_0eta490_invm1000'
 'HLT_mu6_2j40_0eta490_invm400'
 'HLT_mu6_2j40_0eta490_invm600'
 'HLT_mu6_2j40_0eta490_invm800'
 'HLT_mu6_iloose_mu6_11invm24_noos_L1DY-BOX-2MU6'
 'HLT_mu6_iloose_mu6_11invm24_noos_novtx_L1DY-BOX-2MU6'
 'HLT_mu6_iloose_mu6_24invm60_noos_L1DY-BOX-2MU6'
 'HLT_mu6_iloose_mu6_24invm60_noos_novtx_L1DY-BOX-2MU6'
 'HLT_mu6_j50_dr05_2j35_L13J15_BTAG-MU6J25'
 'HLT_mu6_j60_dr05_L1BTAG-MU6J25'
 'HLT_mu6_mu4_bBmumu_L1BPH-2M-MU6MU4'
 'HLT_mu6_mu4_bBmumu_L1BPH-2M-MU6MU4-BO'
 'HLT_mu6_mu4_bBmumu_L1BPH-4M8-MU6MU4'
 'HLT_mu6_mu4_bBmumu_L1BPH-4M8-MU6MU4-BO'
 'HLT_mu6_mu4_bBmumu_L1BPH-DR-MU6MU4'
 'HLT_mu6_mu4_bBmumu_L1BPH-DR-MU6MU4-BO'
 'HLT_mu6_mu4_bBmumux_BcmumuDsloose_L1BPH-2M-MU6MU4-B'
 'HLT_mu6_mu4_bBmumux_BcmumuDsloose_L1BPH-2M-MU6MU4-BO'
 'HLT_mu6_mu4_bBmumux_BcmumuDsloose_L1BPH-4M8-MU6MU4-B'
 'HLT_mu6_mu4_bBmumux_BcmumuDsloose_L1BPH-4M8-MU6MU4-BO'
 'HLT_mu6_mu4_bBmumux_BcmumuDsloose_L1BPH-DR-MU6MU4-B'
 'HLT_mu6_mu4_bBmumux_BcmumuDsloose_L1BPH-DR-MU6MU4-BO'
 'HLT_mu6_mu4_bBmumuxv2_L1BPH-2M-MU6MU4'
 'HLT_mu6_mu4_bBmumuxv2_L1BPH-2M-MU6MU4-BO'
 'HLT_mu6_mu4_bBmumuxv2_L1BPH-4M8-MU6MU4'
 'HLT_mu6_mu4_bBmumuxv2_L1BPH-4M8-MU6MU4-BO'
 'HLT_mu6_mu4_bBmumuxv2_L1BPH-DR-MU6MU4'
 'HLT_mu6_mu4_bBmumuxv2_L1BPH-DR-MU6MU4-BO'
 'HLT_mu6_mu4_bDimu_L1BPH-2M-MU6MU4'
 'HLT_mu6_mu4_bDimu_L1BPH-2M-MU6MU4-BO'
 'HLT_mu6_mu4_bDimu_L1BPH-4M8-MU6MU4'
 'HLT_mu6_mu4_bDimu_L1BPH-4M8-MU6MU4-BO'
 'HLT_mu6_mu4_bDimu_L1BPH-BPH-DR-MU6MU4'
 'HLT_mu6_mu4_bDimu_L1BPH-BPH-DR-MU6MU4-BO'
 'HLT_mu6_mu4_bDimu_novtx_noos_L1BPH-2M-MU6MU4-B'
 'HLT_mu6_mu4_bDimu_novtx_noos_L1BPH-2M-MU6MU4-BO'
 'HLT_mu6_mu4_bDimu_novtx_noos_L1BPH-4M8-MU6MU4-B'
 'HLT_mu6_mu4_bDimu_novtx_noos_L1BPH-4M8-MU6MU4-BO'
 'HLT_mu6_mu4_bDimu_novtx_noos_L1BPH-DR-MU6MU4-B'
 'HLT_mu6_mu4_bDimu_novtx_noos_L1BPH-DR-MU6MU4-BO'
 'HLT_mu6_mu4_bJpsimumu_L1BPH-2M-MU6MU4'
 'HLT_mu6_mu4_bJpsimumu_L1BPH-2M-MU6MU4-BO'
 'HLT_mu6_mu4_bJpsimumu_L1BPH-DR-MU6MU4'
 'HLT_mu6_mu4_bJpsimumu_L1BPH-DR-MU6MU4-BO'
 'HLT_mu6_mu4_bUpsimumu_L1BPH-2M-MU6MU4'
 'HLT_mu6_mu4_bUpsimumu_L1BPH-2M-MU6MU4-BO'
 'HLT_mu6_mu4_bUpsimumu_L1BPH-DR-MU6MU4'
 'HLT_mu6_mu4_bUpsimumu_L1BPH-DR-MU6MU4-BO'
 'HLT_tau35_medium1_tracktwo_tau25_medium1_tracktwo_L1BOX-TAU20ITAU12I'
 'HLT_tau35_medium1_tracktwo_tau25_medium1_tracktwo_L1DR-TAU20ITAU12I'
 'HLT_tau35_medium1_tracktwo_tau25_medium1_tracktwo_L1DR-TAU20ITAU12I-J25'
 'HLT_tau35_medium1_tracktwo_tau25_medium1_tracktwo_L1TAU20ITAU12I-J25'
 'HLT_mu14_iloose_tau25_medium1_tracktwo_L1DR-MU10TAU12I'
 'HLT_mu14_iloose_tau25_medium1_tracktwo_L1DR-MU10TAU12I_TAU12I-J25'
 'HLT_mu14_iloose_tau25_medium1_tracktwo_L1MU10_TAU12I-J25'
 'HLT_mu4_j35_bperf_split_dr05_dz02_L1BTAG-MU4J15'
 'HLT_mu4_j55_bperf_split_dr05_dz02_L1BTAG-MU4J15'
 'HLT_mu6_j110_bperf_split_dr05_dz02_L1BTAG-MU6J20'
 'HLT_mu6_j150_bperf_split_dr05_dz02_L1BTAG-MU6J20'
 'HLT_mu6_j175_bperf_split_dr05_dz02_L1BTAG-MU6J20'
 'HLT_mu6_j260_bperf_split_dr05_dz02_L1BTAG-MU6J20'
 'HLT_mu6_j320_bperf_split_dr05_dz02_L1BTAG-MU6J20'
 'HLT_mu6_j400_bperf_split_dr05_dz02_L1BTAG-MU6J20'
 'HLT_mu6_j85_bperf_split_dr05_dz02_L1BTAG-MU6J20'
 'HLT_e17_lhmedium_iloose_tau25_medium1_tracktwo_L1EM15TAU12I-J25'
 'HLT_e17_medium_iloose_tau25_medium1_tracktwo_L1EM15TAU12I-J25'
 ]
disabled_Rules.update(dict(map(None,HLT_list,len(HLT_list)*[{
    1000 : TriggerRule(PS=-1, comment='Disabled L1Topo'), 
    }])))


#################################################################################################
#################################################################################################


rules = {}

# full list
RulesList=[muon_Rules, bphys_Rules, met_Rules, jet_Rules, bjet_Rules, tau_Rules, egamma_Rules, combined_Rules, vbf_Rules, dedicated_Rules, disabled_Rules]

for Rules in RulesList:
    for newrule in Rules.keys():
        if rules.has_key(newrule):
            print 'FATAL     Physics_pp_v6_rules     Duplicated rule inside physics rule. Cannot be added:',newrule
    rules.update(Rules)

# ----------------------------------------

RulesList=[datascouting_Rules]
if forRateEstim:
    RulesList=[]

for Rules in RulesList:
    for newrule in Rules.keys():
        if rules.has_key(newrule):
            print 'FATAL     Physics_pp_v6_rules     Duplicated rule inside calibration rule. Cannot be added:',newrule
    rules.update(Rules)

# ----------------------------------------

RulesList=[zerobias_Rules, beamspot_Rules, tauoverlay_Rules, topo_Rules,l1calo_streamer_Rules]
if forRateEstim:
    RulesList=[]

for Rules in RulesList:
    for newrule in Rules.keys():
        if rules.has_key(newrule):
            print 'FATAL     Physics_pp_v6_rules     Duplicated rule inside calibration rule. Cannot be added:',newrule
    rules.update(Rules)

# ----------------------------------------

RulesList=[Monitoring_pp_v6_rules.physics_rules]
if forRateEstim:
    RulesList=[]

for Rules in RulesList:
    for newrule in Rules.keys():
        if rules.has_key(newrule):
            print 'FATAL     Physics_pp_v6_rules     Duplicated rule in monitoring rule. Cannot be added from monitoring rule for:',newrule
    rules.update(Rules)

#################################################################################################
#################################################################################################

# rules to override already-existing rules on purpose

override_rules = {
    # defined in monitoring rule
    'L1_MU4_UNPAIRED_ISO'                      : {  1000 : TriggerRule(PS=50,    comment='') },
    'L1_TAU8_EMPTY'                            : {  1000 : TriggerRule(PS=1,   scaling='bunches', comment='Support') },
    'L1_TAU8_UNPAIRED_ISO'                     : {  1000 : TriggerRule(PS=1,   scaling='bunches', comment='Support') },
    'L1_EM7_EMPTY'                             : {  1000 : TriggerRule(PS=1,   scaling='bunches', comment='Support') },
    'L1_EM7_UNPAIRED_ISO'                      : {  1000 : TriggerRule(PS=1,   scaling='bunches', comment='Support') },

    #    'HLT_l1calocalib' :  {  1000 : TriggerRule(rate=1000,    comment=' overwritten for 8b4e') },
#    'HLT_noalg_zb_L1ZB'        : { 1 : TriggerRule(PS=40, comment='10 Hz target overwritten for 8b4e') },
    
}


if forRateEstim:
    #HLT_j55_0eta240_xe50_L1J30_EMPTY is known to be duplicated here
    L1_list=['L1_RD0_FILLED', 'L1_RD0_ABORTGAPNOTCALIB', 'L1_RD0_UNPAIRED_ISO', 'HLT_j55_0eta240_xe50_L1J30_EMPTY']
    override_rules.update(dict(map(None,L1_list,len(L1_list)*[{
        1000 : TriggerRule(PS=-1, scaling='bunches', comment='L1'),
        }])))



rules.update(override_rules)


#################################################################################################
#################################################################################################
