# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from RuleClasses import TriggerRule
tag = 'Physics_pp_v5_ppref2'
rules={}

import Physics_pp_v5_rules
rules.update(Physics_pp_v5_rules.rules)

from runOptions import filled_bunches as NumBunches
ZDC_A_C_ps = NumBunches*0.6
ZDC_A_ps   = NumBunches*3

TE5_ps  = NumBunches/5
TE10_ps = NumBunches*0.07
TE20_ps = NumBunches*0.068
TE30_ps = NumBunches*0.018
TE40_ps = -1
TE50_ps = 1
TE60_ps = 1
HLT_ucc = NumBunches*0.05
HLT_zb_L1ZB_ps = NumBunches*0.2
HLT_e5_etcut_ps = 15

if TE10_ps<1:
    TE10_ps=1
if TE20_ps<1:
    TE20_ps=1
if TE30_ps<1:
    TE30_ps=1
#if TE40_ps<1:
#    TE40_ps=1
if TE50_ps<1:
    TE50_ps=1
if TE60_ps<1:
    TE60_ps=1
if HLT_ucc<1:
    HLT_ucc=1
if HLT_zb_L1ZB_ps<1:
    HLT_zb_L1ZB_ps=1
if HLT_e5_etcut_ps<1:
    HLT_e5_etcut_ps=1

jet_250hz=False
jet_400hz=False
jet_600hz=True

doHITreshold=False

########
# muon rules ppref
########

muon_rules={}

# physics
L1_list=['L1_MU6','L1_MU20', 'L1_2MU10', 'L1_3MU6']
muon_rules.update(dict(map(None,L1_list,len(L1_list)*[{
    400 : TriggerRule(PS=1, comment='L1'),
    }])))

#overriding rate from Physics_pp_v5_rules
rules['L1_MU4'] ={
    10 : TriggerRule(rate=15000, comment='L1'),
    50 : TriggerRule(PS=1, comment='L1'),
    200 : TriggerRule(PS=3, comment='L1'),
    }

HLT_list=[ 'HLT_mu14', 'HLT_mu14_iloose', 'HLT_mu18', 'HLT_mu20', 'HLT_mu22', 'HLT_mu26',
           'HLT_mu6_mu4noL1', 'HLT_mu18_mu8noL1', 'HLT_mu20_mu8noL1', 'HLT_mu22_mu8noL1','HLT_mu24_mu8noL1',
           'HLT_2mu4', 'HLT_2mu6', 'HLT_2mu10', 'HLT_2mu14',
           'HLT_mu26_imedium', 'HLT_mu50', 'HLT_mu60_0eta105_msonly',
           'HLT_mu24_2mu4noL1', 'HLT_3mu6_msonly' ]
muon_rules.update(dict(map(None,HLT_list,len(HLT_list)*[{
                    400 : TriggerRule(PS=1, comment=''), 
                    }])))

muon_rules['HLT_mu18'] = {
    400 : TriggerRule(PS=1, comment='',ESRate=0.5),
    }

muon_rules['HLT_mu4_bJpsi_Trkloose'] = {
    400 : TriggerRule(rate=1, comment=''),
    }


# supporting
HLT_list=[  'HLT_mu4_mu4noL1']
muon_rules.update(dict(map(None,HLT_list,len(HLT_list)*[{
                    50 : TriggerRule(PS=1., comment='',rerun=1),
                    200 : TriggerRule(rate=30., comment='',rerun=1), 
                    }])))
muon_rules['HLT_mu4noL1'] = {
                    50 : TriggerRule(PS=50., comment='min 30Hz',rerun=1), 
                    200 : TriggerRule(rate=60., comment='',rerun=1),
                    }
muon_rules['HLT_mu4'] = {
                    50 : TriggerRule(PS=50., comment='',rerun=1),
                    200 : TriggerRule(rate=60., comment='',rerun=1),
                    }
muon_rules['HLT_mu6'] = {
                    50 : TriggerRule(PS=1., comment='',rerun=1),
                    200 : TriggerRule(rate=60., comment='50Hz',rerun=1), #60 12
                    }
muon_rules['HLT_mu8'] = {
                    50 : TriggerRule(PS=1., comment='',rerun=1),
                    200 : TriggerRule(rate=60., comment='50Hz',rerun=1), #60 4
                    }



if doHITreshold:
    muon_rules['HLT_mu8'] = {
    400 : TriggerRule(PS=1, comment='',rerun=1),
    }
    muon_rules['HLT_mu10'] = {
    400 : TriggerRule(PS=1, comment='',rerun=1),
    }
    muon_rules['HLT_mu11'] = {
    400 : TriggerRule(PS=1, comment='',rerun=1),
    }
    muon_rules['HLT_mu4_mu4noL1'] = {
    400 : TriggerRule(PS=1, comment='',rerun=1),
    }
    


rules.update(muon_rules)

###########
# egamma
##########

egamma_rules={}

L1_list=['L1_EM3']
egamma_rules.update(dict(map(None,L1_list,len(L1_list)*[{
    400 : TriggerRule(PS=160, comment='L1'),
    }])))

#physics
HLT_list=[
    'HLT_e15_lhloose_L1EM13VH', 'HLT_e12_lhmedium', 
    'HLT_e17_lhmedium', 'HLT_e20_lhmedium',
    'HLT_2e12_lhloose_L12EM10VH',
    #
    'HLT_e20_vloose_L1EM12','HLT_e20_lhvloose_L1EM12',
    'HLT_e25_vloose_L1EM15','HLT_e25_lhvloose_L1EM15',
    'HLT_e30_vloose_L1EM15','HLT_e30_lhvloose_L1EM15',
    'HLT_e40_vloose_L1EM15','HLT_e40_lhvloose_L1EM15',
    'HLT_e50_vloose_L1EM15','HLT_e50_lhvloose_L1EM15',
    'HLT_e70_vloose',       'HLT_e70_lhvloose',
    'HLT_e80_vloose',       'HLT_e80_lhvloose',
    'HLT_e100_vloose',      'HLT_e100_lhvloose',
    'HLT_e120_vloose',      'HLT_e120_lhvloose',
    #
    #'HLT_e60_etcut',
    #'HLT_e70_etcut',
    #'HLT_e80_etcut',
    #'HLT_e100_etcut',
    #'HLT_e120_etcut',
    # photons
    'HLT_g30_loose', 'HLT_g30_loose_L1EM15',
    'HLT_g35_loose', 'HLT_g35_loose_L1EM15', 
    'HLT_g40_loose_L1EM15', 'HLT_g45_loose_L1EM15', 
    'HLT_g50_loose_L1EM15', 'HLT_g60_loose_L1EM15VH', 
    'HLT_g50_loose', 'HLT_g60_loose',
    'HLT_g35_medium_HLTCalo', 
    'HLT_g40_tight', 'HLT_g45_tight',
    'HLT_g200_etcut',
    #'HLT_2g10_loose', 'HLT_2g20_loose', 'HLT_2g20_loose_L12EM15', #request from Peter
]

egamma_rules.update(dict(map(None,HLT_list,len(HLT_list)*[{
    400 : TriggerRule(PS=1, comment=''), 
    }])))

HLT_list=[
    'HLT_e10_etcut_L1EM7',
    'HLT_e15_etcut_L1EM7',
    ]
egamma_rules.update(dict(map(None,HLT_list,len(HLT_list)*[{
    400 : TriggerRule(rate=30.0, comment='', rerun=1), 
    }])))

egamma_rules['HLT_e5_etcut'] = {
    400 : TriggerRule(PS=HLT_e5_etcut_ps, comment='20Hz', rerun=1),
    }

HLT_list=[
    'HLT_e12_lhloose_L1EM10VH' ,
    #'HLT_g10_loose',
    #'HLT_g20_loose',
    'HLT_g10_etcut',
    'HLT_g20_etcut_L1EM12',
    ]
egamma_rules.update(dict(map(None,HLT_list,len(HLT_list)*[{
    400 : TriggerRule(rate=20.0, comment='', rerun=1), 
    }])))

egamma_rules['HLT_g20_loose_L1EM15']={
    50 : TriggerRule(PS=1., comment='', rerun=1),
    200 : TriggerRule(PS=10, comment='', rerun=1),
    }
egamma_rules['HLT_g25_loose_L1EM15']={
    50 : TriggerRule(PS=1., comment='', rerun=1),
    200 : TriggerRule(rate=20., comment='', rerun=1),
    }

if doHITreshold:
    egamma_rules['HLT_g20_loose_L1EM15']={
    400 : TriggerRule(PS=1, comment='', rerun=1),
    }
    egamma_rules['HLT_g25_loose_L1EM15']={
    400 : TriggerRule(PS=1, comment='', rerun=1),
    }

# is this ok for pp v5 physics case ? should we give some rate ?
HLT_list=[
    'HLT_e12_vloose_L1EM10VH','HLT_e12_lhvloose_L1EM10VH' ,'HLT_e12_lhvloose_nod0_L1EM10VH'
    ]
egamma_rules.update(dict(map(None,HLT_list,len(HLT_list)*[{
    400 : TriggerRule(rate=0.0, comment='', rerun=1), 
    }])))


rules.update(egamma_rules)

# Jets
jet_rules={}

rules['L1_J20.31ETA49'] = {
    40: TriggerRule(comment='',PS=1,),
    240: TriggerRule(comment='Prescaled',rate=1500.0,maxRate=1500.0,)
    }
rules['L1_J15.31ETA49'] = {
    40: TriggerRule(comment='',PS=1,),
    200: TriggerRule(comment='',PS=2,)
    }

HLT_list=[
    #inclusive
    'HLT_j150', 'HLT_j175', 'HLT_j200', 'HLT_j260',
    #multi-jet
    'HLT_3j50',
    #muon+jet
    'HLT_mu6_j60_dr05_L1MU6_J20',    
    'HLT_mu6_j85_dr05',    
    'HLT_mu6_j110_dr05',
    ]
jet_rules.update(dict(map(None,HLT_list,len(HLT_list)*[{
                    400 : TriggerRule(PS=1, comment=''), 
                    }])))
jet_rules['HLT_mu4_j30'] = {
    50 : TriggerRule(PS=4, comment='18Hz'),
    200 : TriggerRule(rate=15, comment='10Hz'),
    }
jet_rules['HLT_mu6_j30'] = {
    50 : TriggerRule(PS=1, comment='10Hz'),
    200 : TriggerRule(PS=15, comment='10Hz'),
    }

rules.update(jet_rules)

if jet_250hz:
    jet_scale_factor=1.
    bjet_scale_factor=1.
elif jet_400hz:
    jet_scale_factor=2.
    bjet_scale_factor=3.
elif jet_600hz:
    jet_scale_factor=2.
    bjet_scale_factor=3.

rules['L1_J15'] = {
    400 : TriggerRule(rate=1000, maxRate=1000, comment='Prescaled'),
    }
rules['L1_J20'] = {
    400 : TriggerRule(PS=1, comment='Prescaled'),
    }

jet_rules={
    #inclusive jets
    'HLT_j20'                       : {   
                                          10 : TriggerRule(PS=1, comment=''),
                                          240 : TriggerRule(PS=3, comment=''),},
    'HLT_j50_L1J12'                 : {   400 : TriggerRule(rate=jet_scale_factor*100, comment='') },
    'HLT_j60_L1J15'                 : {   
                                          240 : TriggerRule(PS=1, comment=''),
                                          400 : TriggerRule(rate=jet_scale_factor*40, comment='') },
    'HLT_j75_L1J20'                 : {   400 : TriggerRule(rate=jet_scale_factor*50, comment='') },
    'HLT_j110'                      : {   400 : TriggerRule(rate=jet_scale_factor*10, comment='') },
    'HLT_j150'                      : {   400 : TriggerRule(rate=jet_scale_factor*5, comment='') },
    'HLT_j175'                      : {   400 : TriggerRule(PS=1, comment='') },
    #forward
    'HLT_j10_320eta490'                      : {   400 : TriggerRule(rate=10, comment='') },
    'HLT_j15_320eta490'                      : {   400 : TriggerRule(rate=10, comment='') },
    'HLT_2j10_320eta490'                     : {   400 : TriggerRule(PS=1, comment='') },
    'HLT_j25_320eta490_L1TE5'                : {   400 : TriggerRule(PS=1, comment='') },
    'HLT_j35_320eta490_L1TE10'               : {   400 : TriggerRule(PS=1, comment='') },
    'HLT_j45_320eta490_L1TE20'               : {   400 : TriggerRule(PS=1, comment='') },
    'HLT_j60_320eta490'                      : {   400 : TriggerRule(PS=1, comment='') },
    'HLT_j85_320eta490'                      : {   400 : TriggerRule(PS=1, comment='') },
    'HLT_j110_320eta490'                     : {   400 : TriggerRule(PS=1, comment='') },
    #di-jet
    'HLT_2j10'                               : {   200 : TriggerRule(PS=-1, comment=''),},
    'HLT_2j15'                               : {   10  : TriggerRule(PS=-1, comment=''),},
    'HLT_2j20'                               : {   240 : TriggerRule(PS=-1, comment=''),},
    'HLT_2j25'                               : {   240 : TriggerRule(PS=-1, comment=''),},
    'HLT_2j15_320eta490'                     : {   400 : TriggerRule(PS=1, comment='') },
    'HLT_2j20_320eta490'                     : {   400 : TriggerRule(PS=1, comment='') },
    'HLT_2j25_320eta490'                     : {   400 : TriggerRule(PS=1, comment='') },
    'HLT_2j25_320eta490_L1TE5'               : {   400 : TriggerRule(PS=1, comment='') },
    'HLT_2j35_320eta490'                     : {   400 : TriggerRule(rate=5, comment='') },
    #multi-jets
    'HLT_j75_2j30'              : {   400 : TriggerRule(rate=15, comment='') },
    'HLT_j100_2j30'             : {   400 : TriggerRule(rate=5, comment='') },
    #mu+jet
    'HLT_mu4_j25_dr05'             : {   
                                         10 : TriggerRule(PS=1, comment=''),
                                         200 : TriggerRule(PS=2, comment='')},
    'HLT_mu4_j35_dr05'             : {   
                                         10 : TriggerRule(PS=1, comment=''),
                                         200 : TriggerRule(rate=bjet_scale_factor*20, comment='')},
    'HLT_mu4_j55_dr05'             : {   
                                         400 : TriggerRule(PS=1, comment='',ESValue=10),},
    }

if jet_600hz:
    jet_rules['HLT_j75_L1J20'] = {   400 : TriggerRule(PS=1, comment='')} 
    jet_rules['HLT_j60_L1J15'] = {   400 : TriggerRule(rate=100, comment='')}
    jet_rules['HLT_j85'] =       {   400 : TriggerRule(PS=1, comment='')}

jet_rules['HLT_j45_320eta490'] =       {   400 : TriggerRule(PS=1, comment='')}
jet_rules['HLT_j55_320eta490'] =       {   400 : TriggerRule(PS=1, comment='')}

rules.update(jet_rules)

jet_rules={
    #inclusive jets
    'HLT_j30_L1TE5'                 : {   
         2 : TriggerRule(PS=1, comment='') ,
         40 : TriggerRule(PS=1, comment='') ,
         240 : TriggerRule(rate=jet_scale_factor*55, comment='min 45Hz') },
    
    'HLT_j40_L1TE10'                : {   
         2 : TriggerRule(PS=1, comment='') ,
         40 : TriggerRule(PS=1, comment='') ,
         240 : TriggerRule(rate=jet_scale_factor*30, comment='min 50Hz') ,
     }
}

rules.update(jet_rules)

#################################################################################################
#
#
# MinBias 
#
#
#################################################################################################

minbias_rules = {

    'L1_RD0_FILLED'                                           : {   2 : TriggerRule(rate=5000, scaling='bunches', comment='monitoring+lumi') },
    'HLT_mb_sptrk'                                            : {   2 : TriggerRule(inputRate=30, comment='Target =15 Hz',ESValue=30) }, 
    'HLT_noalg_zb_L1ZB'                                       : {   2 : TriggerRule(PS=HLT_zb_L1ZB_ps, comment='Target 5 Hz',ESValue=0.1) }, 

    }

# donno what prescale for ZDC, also for HLT
L1_list=['L1_ZDC_A','L1_ZDC_C' ]
minbias_rules.update(dict(map(None,L1_list,len(L1_list)*[{
    400 : TriggerRule(PS=ZDC_A_ps, comment=''),
    }])))

# supposed 10Hz/bunch (mu=0.5)
L1_list=['L1_ZDC_AND' ]
minbias_rules.update(dict(map(None,L1_list,len(L1_list)*[{
    400 : TriggerRule(PS=ZDC_A_C_ps, comment=''),
    }])))

L1_list=['L1_ZDC_A_C' ]
minbias_rules.update(dict(map(None,L1_list,len(L1_list)*[{
    400 : TriggerRule(PS=ZDC_A_C_ps, comment=''),
    }])))



HLT_list=['HLT_noalg_mb_L1ZDC_A','HLT_noalg_mb_L1ZDC_C']
minbias_rules.update(dict(map(None,HLT_list,len(HLT_list)*[{
    400 : TriggerRule(PS=50, comment=''),
    }])))

HLT_list=['HLT_noalg_mb_L1ZDC_AND',]
minbias_rules.update(dict(map(None,HLT_list,len(HLT_list)*[{
    400 : TriggerRule(PS=50, comment=''),
    }])))

HLT_list=['HLT_noalg_mb_L1ZDC_A_C',]
minbias_rules.update(dict(map(None,HLT_list,len(HLT_list)*[{
    400 : TriggerRule(PS=50, comment=''),
    }])))

HLT_list=['HLT_zdcpeb_L1ZDC_A','HLT_zdcpeb_L1ZDC_C','HLT_zdcpeb_L1ZDC_A_C','HLT_zdcpeb_L1ZDC_AND']
minbias_rules.update(dict(map(None,HLT_list,len(HLT_list)*[{
    400 : TriggerRule(PS=1, comment=''),
    }])))

#ucc
HLT_list=['HLT_hi_perfzdc_ucc_L1ZDC_AND']
minbias_rules.update(dict(map(None,HLT_list,len(HLT_list)*[{
    400 : TriggerRule(PS=50, comment=''),
    }])))

HLT_list=['HLT_hi_th1_ucc_L1TE70']
minbias_rules.update(dict(map(None,HLT_list,len(HLT_list)*[{
    400 : TriggerRule(PS=1, comment=''),
    }])))

HLT_list=['HLT_hi_perf_ucc_L1TE70']
minbias_rules.update(dict(map(None,HLT_list,len(HLT_list)*[{
    400 : TriggerRule(PS=HLT_ucc, comment=''),
    }])))

HLT_list=['HLT_hi_perfzdc_ucc_L1ZDC_A','HLT_hi_perfzdc_ucc_L1ZDC_C',]
minbias_rules.update(dict(map(None,HLT_list,len(HLT_list)*[{
    400 : TriggerRule(PS=500, comment=''),
    }])))



rules.update(minbias_rules)

#################################################################################################
#
#
# HMT triggers
#
#
#################################################################################################

hmt_rules={}

# --------------------------------------
# HMT

# -- Physics, L1
# TE5 = 6MHz, TE10 = 1.5MHz, TE20=404kHz, TE30=133kHz
L1_list=['L1_TE5']
hmt_rules.update(dict(map(None,L1_list,len(L1_list)*[{
    2 : TriggerRule(PS=1., comment='5000 Hz'),
    3 : TriggerRule(PS=2., comment='5000 Hz'),
    60 : TriggerRule(PS=TE5_ps*0.25, comment='10000 Hz'),
    160 : TriggerRule(PS=TE5_ps*0.5, comment='5000 Hz'),
    240 : TriggerRule(PS=TE5_ps, comment='5000 Hz'),
    }])))

L1_list=['L1_TE10',]
hmt_rules.update(dict(map(None,L1_list,len(L1_list)*[{
    2 : TriggerRule(PS=1., comment=''),
    60 : TriggerRule(PS=TE5_ps*0.25, comment='10000 Hz'),
    160 : TriggerRule(PS=TE10_ps*0.5, comment='5000 Hz'),
    200 : TriggerRule(PS=TE10_ps, comment='5000 Hz'),
    }])))

L1_list=['L1_TE20',]
hmt_rules.update(dict(map(None,L1_list,len(L1_list)*[{
    2 : TriggerRule(PS=1., comment=''),
    10 : TriggerRule(PS=TE20_ps, comment='1000 Hz'),
    }])))

L1_list=['L1_TE30']
hmt_rules.update(dict(map(None,L1_list,len(L1_list)*[{
    2 : TriggerRule(PS=1., comment=''),
    10 : TriggerRule(PS=TE30_ps, comment='1000 Hz'),
    }])))

L1_list=['L1_TE40']
hmt_rules.update(dict(map(None,L1_list,len(L1_list)*[{
    2 : TriggerRule(PS=1., comment=''),
    10 : TriggerRule(PS=TE40_ps, comment=''),
    }])))

L1_list=['L1_TE50']
hmt_rules.update(dict(map(None,L1_list,len(L1_list)*[{
    2 : TriggerRule(PS=1., comment=''),
    10 : TriggerRule(PS=TE50_ps, comment=''),
    }])))

HLT_list = ['HLT_mb_sp700_hmtperf_L1TE5']
hmt_rules.update(dict(map(None,HLT_list,len(HLT_list)*[{
    2 : TriggerRule(PS=10, comment='',ESValue=100.,rerun=1),
    60 : TriggerRule(PS=800, comment='',ESValue=20.,rerun=1),
    160 : TriggerRule(PS=400, comment='',ESValue=20.,rerun=1),
    }])))

HLT_list = ['HLT_mb_sp1500_hmtperf_L1TE10']
hmt_rules.update(dict(map(None,HLT_list,len(HLT_list)*[{
    2 : TriggerRule(PS=1, comment='',ESValue=100.,rerun=1),
    60 : TriggerRule(PS=300, comment='',ESValue=20.,rerun=1),
    160 : TriggerRule(PS=150, comment='',ESValue=20.,rerun=1),
    }])))

HLT_list = ['HLT_mb_sp2100_hmtperf_L1TE10']
hmt_rules.update(dict(map(None,HLT_list,len(HLT_list)*[{
    2 : TriggerRule(PS=1, comment='',ESValue=100,rerun=1),
    60 : TriggerRule(PS=70, comment='',ESValue=20,rerun=1),
    160 : TriggerRule(PS=35, comment='',ESValue=20,rerun=1),
    }])))

HLT_list=['HLT_mb_sp2200_pusup1300_trk110_hmt_L1TE20','HLT_mb_sp2400_pusup1400_trk120_hmt_L1TE20',]
hmt_rules.update(dict(map(None,HLT_list,len(HLT_list)*[{
    10 : TriggerRule(PS=-1, comment='not in 200 bunches'), 
    }])))


HLT_list=['HLT_mb_sp800_pusup400_trk50_hmt_L1TE5',]
hmt_rules.update(dict(map(None,HLT_list,len(HLT_list)*[{
    2 : TriggerRule(PS=13, comment='',rerun=1),
    10 : TriggerRule(PS=-1, comment='not in 200 bunches',rerun=1), 
    }])))

HLT_list=['HLT_mb_sp1200_pusup700_trk70_hmt_L1TE5',]
hmt_rules.update(dict(map(None,HLT_list,len(HLT_list)*[{
    2 : TriggerRule(PS=6, comment='',rerun=1),
    10 : TriggerRule(PS=-1, comment='not in 200 bunches',rerun=1), 
    }])))

#for 200 bunches
HLT_list=['HLT_mb_sp900_pusup500_trk60_hmt_L1TE5',]
hmt_rules.update(dict(map(None,HLT_list,len(HLT_list)*[{
    2 : TriggerRule(PS=13, comment='',rerun=1),
    60 : TriggerRule(PS=1.5, comment='',rerun=1),
    160 : TriggerRule(PS=1, comment='',rerun=1),
    }])))

HLT_list=['HLT_mb_sp1600_pusup1100_trk90_hmt_L1TE10']
hmt_rules.update(dict(map(None,HLT_list,len(HLT_list)*[{
    2 : TriggerRule(PS=1, comment='',rerun=1),
    10 : TriggerRule(PS=5, comment='',ESValue=3,rerun=1),
    }])))

HLT_list=['HLT_mb_sp1400_pusup550_trk90_hmt_L1TE10']
hmt_rules.update(dict(map(None,HLT_list,len(HLT_list)*[{
    2 : TriggerRule(PS=1, comment='',rerun=1),
    10 : TriggerRule(PS=1, comment='',rerun=1),
    }])))

HLT_list=['HLT_mb_sp1400_pusup550_trk90_hmt_L1TE20']
hmt_rules.update(dict(map(None,HLT_list,len(HLT_list)*[{
    2 : TriggerRule(PS=1, comment='',rerun=1),
    10 : TriggerRule(PS=2, comment='',rerun=1),
    }])))

HLT_list=['HLT_mb_sp1600_pusup1100_trk90_hmt_L1TE30']
hmt_rules.update(dict(map(None,HLT_list,len(HLT_list)*[{
    2 : TriggerRule(PS=1, comment='',rerun=1),
    10 : TriggerRule(PS=1, comment='',rerun=1),
    }])))

HLT_list=['HLT_mb_sp2000_pusup1200_trk100_hmt_L1TE30']
hmt_rules.update(dict(map(None,HLT_list,len(HLT_list)*[{
    2 : TriggerRule(PS=1, comment='',rerun=1),
    10 : TriggerRule(PS=1, comment='',rerun=1),
    }])))

HLT_list=['HLT_mb_sp2200_pusup1300_trk110_hmt_L1TE30']
hmt_rules.update(dict(map(None,HLT_list,len(HLT_list)*[{
    2 : TriggerRule(PS=1, comment='',rerun=1),
    10 : TriggerRule(PS=1, comment='',rerun=1),
    }])))

HLT_list=['HLT_mb_sp2400_pusup1400_trk120_hmt_L1TE30']
hmt_rules.update(dict(map(None,HLT_list,len(HLT_list)*[{
    2 : TriggerRule(PS=1, comment='',rerun=1),
    10 : TriggerRule(PS=1, comment='',rerun=1),
    }])))

#L1TE40
HLT_list=['HLT_mb_sp1600_pusup1100_trk90_hmt_L1TE40']
hmt_rules.update(dict(map(None,HLT_list,len(HLT_list)*[{
    2 : TriggerRule(PS=1, comment='',rerun=1),
    10 : TriggerRule(PS=1, comment='',rerun=1),
    }])))

HLT_list=['HLT_mb_sp2000_pusup1200_trk100_hmt_L1TE40']
hmt_rules.update(dict(map(None,HLT_list,len(HLT_list)*[{
    2 : TriggerRule(PS=1, comment='',rerun=1),
    10 : TriggerRule(PS=1, comment='',rerun=1),
    }])))

HLT_list=['HLT_mb_sp2100_pusup1000_trk120_hmt_L1TE40']
hmt_rules.update(dict(map(None,HLT_list,len(HLT_list)*[{
    2 : TriggerRule(PS=1, comment='',rerun=1),
    10 : TriggerRule(PS=1, comment='',rerun=1),
    }])))

HLT_list=['HLT_mb_sp2400_pusup1100_trk130_hmt_L1TE40']
hmt_rules.update(dict(map(None,HLT_list,len(HLT_list)*[{
    2 : TriggerRule(PS=1, comment='',rerun=1),
    10 : TriggerRule(PS=1, comment='',rerun=1),
    }])))

HLT_list=['HLT_mb_sp2700_pusup1300_trk140_hmt_L1TE40']
hmt_rules.update(dict(map(None,HLT_list,len(HLT_list)*[{
    2 : TriggerRule(PS=1, comment='',rerun=1),
    10 : TriggerRule(PS=1, comment='',rerun=1),
    }])))

HLT_list=['HLT_mb_sp3000_pusup1400_trk150_hmt_L1TE40']
hmt_rules.update(dict(map(None,HLT_list,len(HLT_list)*[{
    2 : TriggerRule(PS=1, comment='',rerun=1),
    10 : TriggerRule(PS=1, comment='',rerun=1),
    }])))

#L1TE50
HLT_list=['HLT_mb_sp1600_pusup1100_trk90_hmt_L1TE50']
hmt_rules.update(dict(map(None,HLT_list,len(HLT_list)*[{
    2 : TriggerRule(PS=1, comment='',rerun=1),
    10 : TriggerRule(PS=1, comment='',rerun=1),
    }])))

HLT_list=['HLT_mb_sp2000_pusup1200_trk100_hmt_L1TE50']
hmt_rules.update(dict(map(None,HLT_list,len(HLT_list)*[{
    2 : TriggerRule(PS=1, comment='',rerun=1),
    10 : TriggerRule(PS=1, comment='',rerun=1),
    }])))

HLT_list=['HLT_mb_sp2100_pusup1000_trk120_hmt_L1TE50']
hmt_rules.update(dict(map(None,HLT_list,len(HLT_list)*[{
    2 : TriggerRule(PS=1, comment='',rerun=1),
    10 : TriggerRule(PS=1, comment='',rerun=1),
    }])))

HLT_list=['HLT_mb_sp2400_pusup1100_trk130_hmt_L1TE50']
hmt_rules.update(dict(map(None,HLT_list,len(HLT_list)*[{
    2 : TriggerRule(PS=1, comment='',rerun=1),
    10 : TriggerRule(PS=1, comment='',rerun=1),
    }])))

HLT_list=['HLT_mb_sp2700_pusup1300_trk140_hmt_L1TE50']
hmt_rules.update(dict(map(None,HLT_list,len(HLT_list)*[{
    2 : TriggerRule(PS=1, comment='',rerun=1),
    10 : TriggerRule(PS=1, comment='',rerun=1),
    }])))

HLT_list=['HLT_mb_sp3000_pusup1400_trk150_hmt_L1TE50']
hmt_rules.update(dict(map(None,HLT_list,len(HLT_list)*[{
    2 : TriggerRule(PS=1, comment='',rerun=1),
    10 : TriggerRule(PS=1, comment='',rerun=1),
    }])))

#TE60
HLT_list=['HLT_mb_sp1600_pusup1100_trk90_hmt_L1TE60']
hmt_rules.update(dict(map(None,HLT_list,len(HLT_list)*[{
    2 : TriggerRule(PS=1, comment='',rerun=1),
    10 : TriggerRule(PS=1, comment='',rerun=1),
    }])))

HLT_list=['HLT_mb_sp2000_pusup1200_trk100_hmt_L1TE60']
hmt_rules.update(dict(map(None,HLT_list,len(HLT_list)*[{
    2 : TriggerRule(PS=1, comment='',rerun=1),
    10 : TriggerRule(PS=1, comment='',rerun=1),
    }])))

HLT_list=['HLT_mb_sp2200_pusup1300_trk110_hmt_L1TE60']
hmt_rules.update(dict(map(None,HLT_list,len(HLT_list)*[{
    2 : TriggerRule(PS=1, comment='',rerun=1),
    10 : TriggerRule(PS=1, comment='',rerun=1),
    }])))

HLT_list=['HLT_mb_sp2400_pusup1400_trk120_hmt_L1TE60']
hmt_rules.update(dict(map(None,HLT_list,len(HLT_list)*[{
    2 : TriggerRule(PS=1, comment='',rerun=1),
    10 : TriggerRule(PS=1, comment='',rerun=1),
    }])))

HLT_list=['HLT_mb_sp3000_pusup1400_trk150_hmt_L1TE60']
hmt_rules.update(dict(map(None,HLT_list,len(HLT_list)*[{
    2 : TriggerRule(PS=1, comment='',rerun=1),
    10 : TriggerRule(PS=1, comment='',rerun=1),
    }])))


rules.update(hmt_rules)


#################################################################################################
#
#
# Disabled triggers 
#
#
#################################################################################################

disabled_rules = {}

HLT_list={'HLT_noalg_L1TE60','HLT_noalg_mb_L1TE60',
          'HLT_mb_sp3000_pusup1500_trk160_hmt_L1TE60','HLT_mb_sp2100_hmtperf_L1TE60',
          'HLT_mb_sp1500_hmtperf_L1TE60',
          'HLT_noalg_mb_L1TE50.0ETA24','HLT_noalg_L1TE50.0ETA24','HLT_mb_sp1400_pusup550_trk90_hmt_L1TE40.0ETA24',
          'HLT_mb_sp1800_trk110_hmt_L1TE40.0ETA24','HLT_mb_sp1800_pusup800_trk110_hmt_L1TE40.0ETA24',
          'HLT_mb_sp600_trk10_sumet60_hmt_L1TE40.0ETA24','HLT_mb_sp1300_hmtperf_L1TE40.0ETA24','HLT_noalg_L1TE40.0ETA24',
          'HLT_mb_sp1500_trk100_hmt_L1TE40.0ETA24','HLT_mb_sp1400_trk90_hmt_L1TE40.0ETA24',
          'HLT_mb_sp1400_trk100_hmt_L1TE40.0ETA24','HLT_mb_sp1700_hmtperf_L1TE40.0ETA24','HLT_mb_sp1500_hmtperf_L1TE40.0ETA24',
          'HLT_mb_sp1500_pusup700_trk100_hmt_L1TE40.0ETA24','HLT_noalg_L1TE60.0ETA24','HLT_noalg_mb_L1TE60.0ETA24',
          'HLT_mb_sp900_trk60_hmt_L1TE20.0ETA24','HLT_mb_sp900_pusup350_trk60_hmt_L1TE20.0ETA24',
          'HLT_mb_sp600_trk10_sumet50_hmt_L1TE20.0ETA24','HLT_mb_sp2_hmtperf_L1TE5.0ETA24',}

disabled_rules.update(dict(map(None,HLT_list,len(HLT_list)*[{
    1000 : TriggerRule(PS=-1, comment=''), 
    }])))

rules.update(disabled_rules)

