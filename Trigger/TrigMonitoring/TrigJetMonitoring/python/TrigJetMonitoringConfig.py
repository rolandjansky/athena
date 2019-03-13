# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#################################################################################################
#  file: TrigJetMonitoring_physics.py
#  purpose : configure TrigJetMonitoring for 
#            a) Monitoring histograms per chain
#               Basic Quantities, Correlations
#            b) Event selection
#            c) Jet selection / cleaning
#            d) Jet / HLT trigger object matching
#            e) Trigger efficiency estimation 
#
#  author(s) : Lee Sawyer (lee.sawyer@cern.ch), Giulio Cornelio Grossi
#################################################################################################

import math
from TrigHLTMonitoring.HLTMonTriggerList import hltmonList

pp      = hltmonList.pp_mode
hi      = hltmonList.HI_mode
cosmics = hltmonList.cosmic_mode
#pPb     = hltmonList.pPb_mode
mc      = hltmonList.mc_mode

# Binning for NJets
hlt_njetbins = [ 100 ]
hlt_njetbinlo = [ 0 ]
hlt_njetbinhi = [ 100 ]

# Binning for Et
hlt_jetEtbins = [ 50 ]
hlt_jetEtbinlo = [ 0. ]
hlt_jetEtbinhi = [ 750. ]

# Binning for Eta
# custom binning for L1 eta
hlt_jetetabins = [ 48]
hlt_jetetabinlo = [ -6.0]
hlt_jetetabinhi = [ 6.0 ]

# Binning for Phi
# custom binning for L1 phi: 65,-3.1415936-0.098174/2.,3.1415936+0.098174/2.
hlt_jetphibins = [ 35 ]
hlt_jetphibinlo = [ -1.115*math.pi ]
hlt_jetphibinhi = [ 1.115*math.pi ]

# Binning for EMFrac
hlt_jetemfracbins = [ 52]
hlt_jetemfracbinlo = [ -0.02]
hlt_jetemfracbinhi = [ 1.02 ]

# Binning for JVT
hlt_jetJVTbins = [ 62]
hlt_jetJVTbinlo = [ -0.12]
hlt_jetJVTbinhi = [ 1.02 ]

# Binning for SumPtTrk500
hlt_jetSumPtTrk500bins = [ 100 ]
hlt_jetSumPtTrk500binlo = [ 0. ]
hlt_jetSumPtTrk500binhi = [ 500 ]

# Binning for DEt
hlt_jetDEtbins = [ 42 ]
hlt_jetDEtbinlo = [ -70. ]
hlt_jetDEtbinhi = [ 70. ]

# Binning for DeltaEta/DeltaPhi
hlt_jetDepbins = [ 12 ]
hlt_jetDepbinlo = [ -6.0 ]
hlt_jetDepbinhi = [ 6.0 ]

hlt_level1EtNBins             = [ 40 ]
hlt_level1EtBinsLow           = [ 0. ]
hlt_level1EtBinsHigh          = [ 400. ]

hlt_hltEtNBins                 = [ 50, 50 ]
hlt_hltEtBinsLow               = [ 0., 0. ]
hlt_hltEtBinsHigh              = [ 500., 500.]


## Items/Chains for Trigger Efficiency 
# NB1: ordered-pair: internal name:chain/item name used internally
# NB2: ordered-pair: internal name:threshold value
# NB3: python lists are alphabetically ordered
# 4-Feb-2015 Modified to match default naming scheme for menu-aware monitoring



## Default dummy config 

hlt_JetKeys = { "a4tcemsubjesISFS"  : "HLT_xAOD__JetContainer_a4tcemsubjesISFS"}

hlt_offlineJetKeys = {"AntiKt4EMTopoJets":"AntiKt4EMTopoJets", "AntiKt4HIJets":"AntiKt4HIJets"}



# L1 items   
hlt_level1EtThresholds        = {'L1_J75':30.}

hlt_l1EtaLowThresholds        = {'L1_J75':0.}


hlt_l1EtaHighThresholds        = {'L1_J75':4.9}


# HLT items
hlt_hltEtThresholds={'j360':250.}

hlt_hltEtaHighThresholds={'j360':3.2}

hlt_hltEtaLowThresholds={'j360':0.}


hlt_hltJetn={'j360':1}
  
  
hlt_hltContainers={'j360':'a4tcemsubjesISFS'}
  
                
# Offline 
hlt_offlineEtThresholds={'j360':250.}
#Dijet chains
  
l1_DijetChains  = ['L1_J75']
hlt_DijetChains = ['j360']
of_DijetChains  = ['AntiKt4EMTopoJets']


############################################ pp Config ###############################################

if (pp) or (mc):

  hlt_JetKeys = { "a10tclcwsubFS"   : "HLT_xAOD__JetContainer_a10tclcwsubFS",
                  "a4tcemsubFS"     : "HLT_xAOD__JetContainer_a4tcemsubFS", 
                  "a4tcemjesFS"     : "HLT_xAOD__JetContainer_a4tcemjesFS", 
                  "a4tcemjesPS"     : "HLT_xAOD__JetContainer_a4tcemjesPS",
                  "a4tcemsubjesISFS"  : "HLT_xAOD__JetContainer_a4tcemsubjesISFS",
                  "a4tclcwjesFS"    : "HLT_xAOD__JetContainer_a4tclcwjesFS",
                  "a4GSC" : "HLT_xAOD__JetContainer_GSCJet",
                  #"a10rtcemsubjesFS"   : "HLT_xAOD__JetContainer_a10r_tcemsubjesFS",
                  "a10tclcwsubjesFS"   : "HLT_xAOD__JetContainer_a10tclcwsubjesFS",
                  "a10ttclcwjesFS"   : "HLT_xAOD__JetContainer_a10ttclcwjesFS",
                  "a4tcemsubjesISFSftk"  : "HLT_xAOD__JetContainer_a4tcemsubjesISFSftk",
                  "a4tcemsubjesISFSftkrefit"  : "HLT_xAOD__JetContainer_a4tcemsubjesISFSftkrefit",
                  }
  
  hlt_offlineJetKeys = {"AntiKt4EMTopoJets":"AntiKt4EMTopoJets",   
                        "AntiKt4LCTopoJets":"AntiKt4LCTopoJets", 
                        "AntiKt10LCTopoJets":"AntiKt10LCTopoJets"}
  
  
  
# L1 items   
  hlt_level1EtThresholds        = { 'L1_J10':0.,
                                    'L1_TE50':0.,
                                    'L1_J15':5.,
                                    'L1_J75':30.,
                                    'L1_J100':50.,
                                    'L1_J100.31ETA49':50.,
                                    'L1_J400':150.}
  
  hlt_l1EtaLowThresholds        = { 'L1_J10':0.,
                                    'L1_TE50':0.,
                                    'L1_J15':0.,                        #Chose eta range for efficiency calculation
                                    'L1_J75':0.,
                                    'L1_J100':0.,
                                    'L1_J100.31ETA49':3.1,
                                    'L1_J400':0.}
  
  
  hlt_l1EtaHighThresholds        = { 'L1_J10':4.9,
                                     'L1_TE50':4.9,
                                     'L1_J15':4.9,
                                     'L1_J75':4.9,
                                     'L1_J100':4.9,
                                     'L1_J100.31ETA49':4.9,
                                     'L1_J400':4.9}
  
  
# HLT items
  hlt_hltEtThresholds            = { 'j0_perf_L1RD0FILLED': 0.,
                                     'j0_perf_ftk_L1RD0FILLED': 0.,
                                     'j0_perf_ftkrefit_L1RD0FILLED': 0.,
                                     'j0_gsc0_boffperf_split_L1RD0FILLED': 0.,
                                     'j0_gsc0_boffperf_split_ftk_L1RD0FILLED': 0.,
                                     'j0_gsc0_boffperf_split_ftkrefit_L1RD0FILLED': 0.,
                                     'j35':20.,
                                     'j35_jes':20.,
                                     'j35_lcw':20.,
                                     'j35_sub':20.,
                                     'j35_subjes':20.,
                                     'j35_nojcalib':20.,
                                     'j35_320eta490':20.,
                                     'j35_320eta490_lcw':20.,
                                     'j35_320eta490_jes':20.,
                                     'j35_320eta490_sub':20.,
                                     'j35_320eta490_subjes':20.,
                                     'j35_320eta490_nojcalib':20.,
                                     'j60':50.,
                                     'j60_280eta320':50.,
                                     'j60_320eta490':50.,
                                     'j200_jes_PS':100.,
                                     'j260':150.,
                                     'j360':250.,
                                     'j460':250.,
                                     'j460_a10_lcw_sub_L1J100':350.,
                                     'j80_xe80':50.,
                                     '4j45':20.,
                                     '6j45':20.,
                                     '6j45_0eta240':20.,
                                     '10j40_L14J20':0.}
    
  hlt_hltEtaHighThresholds       = { 'j0_perf_L1RD0FILLED': 2.5,
                                     'j0_perf_ftk_L1RD0FILLED': 2.5,
                                     'j0_perf_ftkrefit_L1RD0FILLED': 2.5,
                                     'j0_gsc0_boffperf_split_L1RD0FILLED': 2.5,
                                     'j0_gsc0_boffperf_split_ftk_L1RD0FILLED': 2.5,
                                     'j0_gsc0_boffperf_split_ftkrefit_L1RD0FILLED': 2.5,
                                     'j35':3.2,
                                     'j35_jes':3.2,
                                     'j35_lcw':3.2,
                                     'j35_sub':3.2,
                                     'j35_subjes':3.2,
                                     'j35_nojcalib':3.2,
                                     'j35_320eta490':4.9,
                                     'j35_320eta490_lcw':4.9,
                                     'j35_320eta490_jes':4.9,
                                     'j35_320eta490_sub':4.9,
                                     'j35_320eta490_subjes':4.9,
                                     'j60':3.2,
                                     'j60_280eta320':3.2,
                                     'j60_320eta490':4.9,
                                     'j200_jes_PS':3.2,
                                     'j260':3.2, 
                                     'j360':3.2, 
                                     'j460':3.2, 
                                     'j460_a10_lcw_sub_L1J100':3.2,
                                     'j80_xe80':3.2,
                                     '4j45':3.2,
                                     '6j45':3.2,
                                     '6j45_0eta240':2.4,
                                     '10j40_L14J20':3.2}
  
  hlt_hltEtaLowThresholds        = { 'j0_perf_L1RD0FILLED': 0.,
                                     'j0_perf_ftk_L1RD0FILLED': 0.,
                                     'j0_perf_ftkrefit_L1RD0FILLED': 0.,
                                     'j0_gsc0_boffperf_split_L1RD0FILLED': 0.,
                                     'j0_gsc0_boffperf_split_ftk_L1RD0FILLED': 0.,
                                     'j0_gsc0_boffperf_split_ftkrefit_L1RD0FILLED': 0.,
                                     'j35':0.0,
                                     'j35_jes':0.0,
                                     'j35_lcw':0.0,
                                     'j35_sub':0.0,
                                     'j35_subjes':0.0,
                                     'j35_nojcalib':0.0,
                                     'j35_320eta490':3.2,
                                     'j35_320eta490_lcw':3.2,
                                     'j35_320eta490_jes':3.2,
                                     'j35_320eta490_sub':3.2,
                                     'j35_320eta490_subjes':3.2,
                                     'j60':0.0,
                                     'j60_280eta320':2.8,
                                     'j60_320eta490':3.2,
                                     'j60_L1RD0_FILLED':0.0,
                                     'j260':0.0,
                                     'j360':0.0,
                                     'j420':0.0,
                                     'j225_gsc420_boffperf_split':0.0,
                                     'j460_a10_lcw_subjes_L1SC111':0.0,
                                     'j460_a10t_lcw_jes_L1SC111':0.0,
                                     '2j330_a10t_lcw_jes_35smcINF_SC111':0.0,
                                     'j80_xe80':0.0,
                                     '4j45':0.0,
                                     '6j45':0.0,
                                     '6j45_0eta240':0.0,
                                     '10j40_L14J20':0.0}
  
  
  hlt_hltJetn                    = { 'j0_perf_L1RD0FILLED': 1,
                                     'j0_perf_ftk_L1RD0FILLED': 1,
                                     'j0_perf_ftkrefit_L1RD0FILLED': 1,
                                     'j0_gsc0_boffperf_split_L1RD0FILLED': 1,
                                     'j0_gsc0_boffperf_split_ftk_L1RD0FILLED': 1,
                                     'j0_gsc0_boffperf_split_ftkrefit_L1RD0FILLED': 1,
                                     'j35':1,
                                     'j35_jes':1,
                                     'j35_lcw':1,
                                     'j35_sub':1,
                                     'j35_subjes':1,
                                     'j35_nojcalib':1,
                                     'j35_320eta490':1,
                                     'j35_320eta490_lcw':1,
                                     'j35_320eta490_jes':1,
                                     'j35_320eta490_sub':1,
                                     'j35_320eta490_subjes':1,
                                     'j60':1,
                                     'j60_280eta320':1,
                                     'j60_320eta490':1,
                                     'j200_jes_PS':1,
                                     'j260':1,   
                                     'j360':1,  
                                     'j460':1,
                                     'j460_a10_lcw_sub_L1J100':1,
                                     'j80_xe80':1,
                                     '4j45':4,
                                     '6j45':6,
                                     '6j45_0eta240':6,
                                     '10j40_L14J20':10}
  
  
  hlt_hltContainers              = { 'j0_perf_L1RD0FILLED':'a4tcemsubjesISFS',
                                     'j0_perf_ftk_L1RD0FILLED':'a4tcemsubjesISFSftk',
                                     'j0_perf_ftkrefit_L1RD0FILLED':'a4tcemsubjesISFSftkrefit',
                                     'j0_gsc0_boffperf_split_L1RD0FILLED':'a4tcemsubjesISFS',
                                     'j0_gsc0_boffperf_split_ftk_L1RD0FILLED':'a4tcemsubjesISFSftk',
                                     'j0_gsc0_boffperf_split_ftkrefit_L1RD0FILLED':'a4tcemsubjesISFSftkrefit',
                                     'j35':'a4tcemsubjesISFS',
                                     'j35_sub':'a4tcemsubFS',
                                     'j35_jes':'a4tcemjesFS',
                                     'j35_subjes':'a4tcemsubjesFS',
                                     'j35_nojcalib':'a4tcemnojcalibFS',
                                     'j35_lcw':'a4tclcwsubjesISFS',
                                     'j35_320eta490':'a4tcemsubjesISFS',
                                     'j35_320eta490_sub':'a4tcemsubFS',
                                     'j35_320eta490_jes':'a4tcemjesFS',
                                     'j35_320eta490_subjes':'a4tcemsubjesFS',
                                     'j35_320eta490_nojcalib':'a4tcemnojcalibFS',
                                     'j35_320eta490_lcw':'a4tclcwsubjesISFS',
                                     'j60':'a4tcemsubjesISFS',
                                     'j60_280eta320':'a4tcemsubjesISFS',
                                     'j60_320eta490':'a4tcemsubjesISFS',
                                     'j60_L1RD0_FILLED':'a4tcemsubjesISFS',
                                     'j260':'a4tcemsubjesISFS',
                                     'j360':'a4tcemsubjesISFS',
                                     'j420':'a4tcemsubjesISFS',
                                     'j225_gsc420_boffperf_split':'a4GSC',
                                     'j460_a10_lcw_subjes_L1SC111':'a10tclcwsubjesFS',
                                     'j460_a10t_lcw_jes_L1SC111':'a10ttclcwjesFS',
                                     '2j330_a10t_lcw_jes_35smcINF_SC111':'a10ttclcwjesFS',
                                     'j80_xe80':'a4tcemsubjesISFS',
                                     '4j45':'a4tcemsubjesISFS',
                                     '6j45':'a4tcemsubjesISFS',
                                     '6j45_0eta240':'a4tcemsubjesISFS',
                                     '10j40_L14J20':'a4tcemsubjesISFS'
                                     }
                
# Offline 
  hlt_offlineEtThresholds        = { 'j0_perf_L1RD0FILLED': 0.,
                                     'j0_perf_ftk_L1RD0FILLED': 0.,
                                     'j0_perf_ftkrefit_L1RD0FILLED': 0.,
                                     'j0_gsc0_boffperf_split_L1RD0FILLED': 0.,
                                     'j0_gsc0_boffperf_split_ftk_L1RD0FILLED': 0.,
                                     'j0_gsc0_boffperf_split_ftkrefit_L1RD0FILLED': 0.,
                                     'L1_J15':0., 
                                     'j35_jes':20.,
                                     'j35_lcw':20.,
                                     'j35_sub':20.,
                                     'j35_subjes':20.,
                                     'j35_nojcalib':20.,
                                     'j35_320eta490':20.,
                                     'j35_320eta490_lcw':20.,
                                     'j35_320eta490_jes':20.,
                                     'j35_320eta490_sub':20.,
                                     'j35_320eta490_subjes':20.,
                                     'j35_320eta490_nojcalib':20.,
                                     'j60':50.,
                                     'j60_280eta320':50.,
                                     'j60_320eta490':50.,
                                     'j200_jes_PS':100.,
                                     'j260':150.,
                                     'j360':250.,
                                     'j460':250.,
                                     'j460_a10_lcw_sub_L1J100':350.,
                                     'j80_xe80':50.,
                                     '4j45':20.,
                                     '6j60':20.,
                                     '5j60':30.,
                                     '10j40_L14J20':0.,
                                     'j0_perf_ds1_L1J75':0.,
                                     'j0_perf_ds1_L1J100':0.,
                                     'ht850_L1J100':50.,
                                     'j60_TT':20,
                                     'j85_cleanLLP':30,
                                     'j85_cleanL':30,
                                     'j85_cleanT':30,
                                     'j0_1i2c300m500TLA':0.,
                                     'j0_0i1c500m900TLA':0.}
#Dijet chains
  
  l1_DijetChains  = ['L1_J400']
  hlt_DijetChains = ['j0_perf_ds1_L1J75','j0_perf_ds1_L1J100']
  of_DijetChains  = ['AntiKt4EMTopoJets']



############################################# HI Config #########################################################
  
if (hi):

  hlt_JetKeys = { "a10tclcwsubFS"   : "HLT_xAOD__JetContainer_a10tclcwsubFS",
                  "a4tcemsubFS"     : "HLT_xAOD__JetContainer_a4tcemsubFS", 
                  "a4tcemjesFS"     : "HLT_xAOD__JetContainer_a4tcemjesFS", 
                  "a4tcemjesPS"     : "HLT_xAOD__JetContainer_a4tcemjesPS",
                  "a4tcemsubjesISFS"  : "HLT_xAOD__JetContainer_a4tcemsubjesISFS",
                  "a4tclcwjesFS"    : "HLT_xAOD__JetContainer_a4tclcwjesFS",
                  "a4ionemsubjesFS" : "HLT_xAOD__JetContainer_a4ionemsubjesFS",  # Start HI chains
                  "a3ionemsubjesFS" : "HLT_xAOD__JetContainer_a3ionemsubjesFS" }
  
  hlt_offlineJetKeys = {"AntiKt4EMTopoJets":"AntiKt4EMTopoJets",  
                        "AntiKt4HIJets"      : "AntiKt4HIJets"}

  hlt_level1EtThresholds        = { 'L1_J10':0.,
                                    'L1_J20':0.,
                                    'L1_TE50':0.,
                                    'L1_J15':5.,
                                    'L1_J75':30.,
                                    'L1_J100':50.,
                                    'L1_J100.31ETA49':50.,
                                    'L1_J400':150.}

  hlt_l1EtaLowThresholds        = { 'L1_J10':0.,
                                    'L1_J20':0.,
                                    'L1_TE50':0.,
                                    'L1_J15':0.,                        #Chose eta range for efficiency calculation
                                    'L1_J75':0.,
                                    'L1_J100':0.,
                                    'L1_J100.31ETA49':3.1,
                                    'L1_J400':0.}
  
  
  hlt_l1EtaHighThresholds        = { 'L1_J10':4.9,
                                     'L1_J20':4.9,
                                     'L1_TE50':4.9,
                                     'L1_J15':4.9,
                                     'L1_J75':4.9,
                                     'L1_J100':4.9,
                                     'L1_J100.31ETA49':4.9,
                                     'L1_J400':4.9}

# HLT items
  hlt_hltEtThresholds            = { 'j25':20.,
                                     'j25_320eta490':20.,
                                     'j60':50., 
                                     'j60_L1RD0_FILLED':50.,
                                     'j60_280eta320':50.,
                                     'j60_320eta490':50.,
                                     'j200_jes_PS':100.,
                                     'j260':150.,
                                     'j360':250.,
                                     'j460_a10_lcw_sub_L1J100':350.,
                                     'j80_xe80':50.,
                                     '4j45':20.,
                                     '6j60':20.,
                                     '5j60':30.,
                                     'j0_perf_ds1_L1J75':0.,
                                     'j0_perf_ds1_L1J100':0.,
                                     'ht850_L1J100':50.,
                                     'j30_ion_L1TE20':0., #start HI chains
                                     'j30_L1TE20':0.,
                                     'j75_ion_L1J20':0.,
                                     'j85_ion_L1J20':0.,
                                     'j100_ion_L1J20':0.,
                                     'j150_ion_L1J30':0.,
                                     'j75_L1J20':0.,
                                     'j85_L1J20':0.,
                                     'j100_L1J20':0.,
                                     'j150_L1J30':0.,
                                     'j45_320eta490_ion':0.,
                                     'j45_320eta490':0.,
                                     'j50_ion_2j30_ion_0eta490_L1J10':0.,
                                     'j50_2j30_0eta490_L1J10':0.}
  
  hlt_hltEtaHighThresholds       = { 'j25':3.2,                          #Chose eta range for efficiency calculation
                                     'j25_320eta490':4.9,
                                     'j60':3.2 , 
                                     'j60_L1RD0_FILLED':3.2,
                                     'j60_280eta320':3.2,
                                     'j60_320eta490':4.9,
                                     'j200_jes_PS':3.2,'j260':3.2, 
                                     'j260_320eta490':4.9,'j360':3.2, 
                                     'j460_a10_lcw_sub_L1J100':3.2,
                                     'j80_xe80':3.2,
                                     '4j45':3.2,
                                     '6j60':3.2,
                                     '5j60':3.2,
                                     'j0_perf_ds1_L1J75':3.2,
                                     'j0_perf_ds1_L1J100':3.2,
                                     'ht850_L1J100':3.2,
                                     'j30_ion_L1TE20':3.2, #start HI chains
                                     'j30_L1TE20':3.2,
                                     'j75_ion_L1J20':3.2,
                                     'j85_ion_L1J20':3.2,
                                     'j100_ion_L1J20':3.2,
                                     'j150_ion_L1J30':3.2,
                                     'j75_L1J20':3.2,
                                     'j85_L1J20':3.2,
                                     'j100_L1J20':3.2,
                                     'j150_L1J30':3.2,
                                     'j45_320eta490_ion':4.9,
                                     'j45_320eta490':4.9,
                                     'j50_ion_2j30_ion_0eta490_L1J10':4.9,
                                     'j50_2j30_0eta490_L1J10':4.9}
  
  hlt_hltEtaLowThresholds        = { 'j25':0.,
                                     'j25_320eta490':3.2, 
                                     'j60':0.,   
                                     'j60_L1RD0_FILLED':0., 
                                     'j60_280eta320':2.8,
                                     'j60_320eta490':3.2,
                                     'j200_jes_PS':0.,
                                     'j260':0.,   
                                     'j360':0.,  
                                     'j460_a10_lcw_sub_L1J100':0.,
                                     'j80_xe80':0.,
                                     '4j45':0.,
                                     '6j60':0.,
                                     '5j60':0.,
                                     'j0_perf_ds1_L1J75':0.,
                                     'j0_perf_ds1_L1J100':0.,
                                     'ht850_L1J100':0.,
                                     'j30_ion_L1TE20':0., #start HI chains
                                     'j30_L1TE20':0.,
                                     'j75_ion_L1J20':0.,
                                     'j85_ion_L1J20':0.,
                                     'j100_ion_L1J20':0.,
                                     'j150_ion_L1J30':0.,
                                     'j75_L1J20':0.,
                                     'j85_L1J20':0.,
                                     'j100_L1J20':0.,
                                     'j150_L1J30':0.,
                                     'j45_320eta490_ion':3.2,
                                     'j45_320eta490':3.2,
                                     'j50_ion_2j30_ion_0eta490_L1J10':0.,
                                     'j50_2j30_0eta490_L1J10':0.}
  
  
  hlt_hltJetn                    = { 'j25':1,                           #Select the nth jet for efficiency calcultaion of single/multijet HLT chains
                                     'j25_320eta490':1, 
                                     'j60':1,   
                                     'j60_L1RD0_FILLED':1, 
                                     'j60_280eta320':1,
                                     'j60_320eta490':1,
                                     'j200_jes_PS':1,
                                     'j260':1,   
                                     'j360':1,  
                                     'j460_a10_lcw_sub_L1J100':1,
                                     'j80_xe80':1,
                                     '4j45':4,
                                     '6j60':6,
                                     '5j60':5,
                                     'j0_perf_ds1_L1J75':1,
                                     'j0_perf_ds1_L1J100':1,
                                     'ht850_L1J100':1,
                                     'j30_ion_L1TE20':1, #start HI chains
                                     'j30_L1TE20':1,
                                     'j75_ion_L1J20':1,
                                     'j85_ion_L1J20':1,
                                     'j100_ion_L1J20':1,
                                     'j150_ion_L1J30':1,
                                     'j75_L1J20':1,
                                     'j85_L1J20':1,
                                     'j100_L1J20':1,
                                     'j150_L1J30':1,
                                     'j45_320eta490_ion':1,
                                     'j45_320eta490':1,
                                     'j50_ion_2j30_ion_0eta490_L1J10':1,
                                     'j50_2j30_0eta490_L1J10':1}


  hlt_hltContainers              = {'j25':'a4tcemsubjesISFS',                  #Chose container to retrieve the features of a given chain
                                    'j25_320eta490':'a4tcemsubjesISFS',
                                    'j60':'a4tcemsubjesISFS',
                                    'j60_L1RD0_FILLED':'a4tcemsubjesISFS',
                                    'j60_280eta320':'a4tcemsubjesISFS',
                                    'j60_320eta490':'a4tcemsubjesISFS',
                                    'j200_jes_PS':'a4tcemjesPS',
                                    'j260':'a4tcemsubjesISFS',
                                    'j360':'a4tcemsubjesISFS',
                                    'j460_a10_lcw_sub_L1J100':'a10tclcwsubFS',
                                    'j80_xe80':'a4tcemsubjesISFS',
                                    '4j45':'a4tcemsubjesISFS',
                                    '6j60':'a4tcemsubjesISFS',
                                    '5j60':'a4tcemsubjesISFS',
                                    'j0_perf_ds1_L1J75':'a4tcemsubjesISFS',
                                    'j0_perf_ds1_L1J100':'a4tcemsubjesISFS',
                                    'ht850_L1J100':'a4tcemsubjesISFS',
                                    'j30_ion_L1TE20':'a4ionemsubjesFS', #start HI chains
                                    'j30_L1TE20':'a4tcemsubjesISFS',
                                    'j75_ion_L1J20':'a4ionemsubjesFS',
                                    'j85_ion_L1J20':'a4ionemsubjesFS',
                                    'j100_ion_L1J20':'a4ionemsubjesFS',
                                    'j150_ion_L1J30':'a4ionemsubjesFS',
                                    'j75_L1J20':'a4tcemsubjesISFS',
                                    'j85_L1J20':'a4tcemsubjesISFS',
                                    'j100_L1J20':'a4tcemsubjesISFS',
                                    'j150_L1J30':'a4tcemsubjesISFS',
                                    'j45_320eta490_ion':'a4ionemsubjesFS',
                                    'j45_320eta490':'a4tcemsubjesISFS',
                                    'j50_ion_2j30_ion_0eta490_L1J10':'a4ionemsubjesFS',
                                    'j50_2j30_0eta490_L1J10':'a4tcemsubjesISFS'}



  hlt_offlineEtThresholds        = { 'L1_J15':0., 
                                     'L1_J20':0.,
                                     'j25':20.,
                                     'j25_320eta490':20., 
                                     'j60':50.  , 
                                     'j60_L1RD0_FILLED':50.,
                                     'j60_280eta320':50.,
                                     'j60_320eta490':50.,
                                     'j200_jes_PS':100.,
                                     'j260':150.,
                                     'j360':250.,
                                     'j460_a10_lcw_sub_L1J100':350.,
                                     'j80_xe80':50.,
                                     '4j45':20.,
                                     '6j60':20.,
                                     '5j60':30.,
                                     'j0_perf_ds1_L1J75':0.,
                                     'j0_perf_ds1_L1J100':0.,
                                     'ht850_L1J100':50.,
                                     'j30_ion_L1TE20':0., #start HI chains
                                     'j30_L1TE20':0.,
                                     'j75_ion_L1J20':0.,
                                     'j85_ion_L1J20':0.,
                                     'j100_ion_L1J20':0.,
                                     'j150_ion_L1J30':0.,
                                     'j75_L1J20':0.,
                                     'j85_L1J20':0.,
                                     'j100_L1J20':0.,
                                     'j150_L1J30':0.,
                                     'j45_320eta490_ion':0.,
                                     'j45_320eta490':0.,
                                     'j50_ion_2j30_ion_0eta490_L1J10':0.,
                                     'j50_2j30_0eta490_L1J10':0.}
#Dijet chains
  
  l1_DijetChains  = ['L1_J400']
  hlt_DijetChains = ['j0_perf_ds1_L1J75','j0_perf_ds1_L1J100']
  of_DijetChains  = ['AntiKt4HIJets']



################################### Cosmics config ######################################################


if (cosmics):

  hlt_JetKeys = { "a10tclcwsubFS"    : "HLT_xAOD__JetContainer_a10tclcwsubFS",
                  "a4tcemsubFS"     : "HLT_xAOD__JetContainer_a4tcemsubFS", 
                  "a4tcemjesFS"     : "HLT_xAOD__JetContainer_a4tcemjesFS", 
                  "a4tcemjesPS"     : "HLT_xAOD__JetContainer_a4tcemjesPS",
                  "a4tcemsubjesISFS"  : "HLT_xAOD__JetContainer_a4tcemsubjesISFS",
                  "a4tclcwjesFS"    : "HLT_xAOD__JetContainer_a4tclcwjesFS",
                  "a4TTemnojcalibFS": "HLT_xAOD__JetContainer_a4TTemnojcalibFS"
                  }
  
  hlt_offlineJetKeys = {"AntiKt4EMTopoJets":"AntiKt4EMTopoJets",   
                        "AntiKt4LCTopoJets":"AntiKt4LCTopoJets", 
                        "AntiKt10LCTopoJets":"AntiKt10LCTopoJets"}
  


  hlt_level1EtThresholds        = { 'L1_J10':0.,
                                    'L1_J15':5.}
  
  hlt_l1EtaLowThresholds        = { 'L1_J10':0.,
                                    'L1_J15':0.}
  
  
  hlt_l1EtaHighThresholds        = { 'L1_J10':4.9,
                                     'L1_J15':4.9}
  
  
# HLT items
  hlt_hltEtThresholds            = {'j25':20.}
                                     
  
  hlt_hltEtaHighThresholds       = {'j25':3.2}
  
  hlt_hltEtaLowThresholds        = { 'j25':0.}
  
  
  hlt_hltJetn                    = { 'j25':1}
  
  
  hlt_hltContainers              = {'j25':'a4tcemsubjesISFS'}
                
                
# Offline 
  hlt_offlineEtThresholds        = { 'L1_J15':10., 
                                     'j25':20.}
#Dijet chains
  
  l1_DijetChains  = ['L1_J400']
  hlt_DijetChains = ['j0_perf_ds1_L1J75','j0_perf_ds1_L1J100']
  of_DijetChains  = ['AntiKt4EMTopoJets']


##################################################################################################################

## implemented but not being used [todo]
##hlt_jetchainsbyregexp         = [ "(L2|EF)_[0-9]?[jJ][0-9]+", "(L2|EF)_[0-9]+[fj][j0-9]+"]

## define instance of monitoring tool
def TrigJetMonitoringTool():
  from TrigJetMonitoring.TrigJetMonitoringConf import HLTJetMonTool
  from TrigHLTMonitoring.HLTMonTriggerList import hltmonList  # access to central tool

  HLTJetMon = HLTJetMonTool ( 
            name                 = 'HLTJetMon',
            histoPathBase        = "/Trigger/HLT",
            JetMonBase           = "/HLT/JetMon",
            DoL1Efficiency       = True,         # w.r.t offline
	    DoOfflineJets        = True,         # fill offline jet plots
            DoHLTEfficiency      = True,         # w.r.t offline (HLT eff = L1 & HLT)
 #           EnableLumi           = True,        # Enable Luminosity Tool
 #           DoLumiWeight         = True,        # Decide to apply luminosity weights                       
            # SG Keys for L1, EF, OF Jets
            L1xAODJetKey         = "LVL1JetRoIs",
            HLTJetKeys           = hlt_JetKeys,
            OFJetKeys            = hlt_offlineJetKeys,

            # Directories for histograms 
            # Modifying these is not recommended since
            # it requires change in HLTjet han config 
            # Prefix '/' for all *Dir 
            L1JetDir             = "/L1",
            HLTJetDir            = "/HLT",
            EfficiencyDir        = "/TrigEff",

            ## IF set, offline jet algs will have this prefix
            # Modifying this is not recommended since
            # it requires change in HLTjet han config 
            HLTJetDirPrefix      = "HLT",

            ## IF set, offline jet algs will have this prefix
            # Modifying this is not recommended since
            # it requires change in HLTjet han config 
            OFJetDirPrefix       = "OF",
           
            # Define basic L1/HLT chains
            monitoring_l1jet       = hltmonList.monitoring_l1jet,
            monitoring_jet         = hltmonList.monitoring_jet,

            L1DijetChains          = l1_DijetChains,
            HLTDijetChains         = hlt_DijetChains,
            OFDijetChains          = of_DijetChains,

            # Binning for NJets
            NJetNBins               = hlt_njetbins,
            NJetBinLo               = hlt_njetbinlo,
            NJetBinHi               = hlt_njetbinhi,
            
            # Binning for Jet Et
            JetEtNBins              = hlt_jetEtbins,
            JetEtBinLo              = hlt_jetEtbinlo,
            JetEtBinHi              = hlt_jetEtbinhi,

            #JetEtNBins              = hlt_efEtNBins,
            #JetEtBinLo              = hlt_efEtBinsLow,
            #JetEtBinHi              = hlt_efEtBinsHigh,

            # Binning for eta
            JetetaNBins               = hlt_jetetabins,
            JetetaBinLo               = hlt_jetetabinlo,
            JetetaBinHi               = hlt_jetetabinhi,
            
            # Binning for phi
            JetphiNBins               = hlt_jetphibins,
            JetphiBinLo               = hlt_jetphibinlo,
            JetphiBinHi               = hlt_jetphibinhi,

            # Binning for emfrac
            JetemfracNBins            = hlt_jetemfracbins,
            JetemfracBinLo            = hlt_jetemfracbinlo,
            JetemfracBinHi            = hlt_jetemfracbinhi,

            # Binning for emfrac
            JetJVTNBins               = hlt_jetJVTbins,
            JetJVTBinLo               = hlt_jetJVTbinlo,
            JetJVTBinHi               = hlt_jetJVTbinhi,

            # Binning for emfrac
            JetSumPtTrk500NBins       = hlt_jetSumPtTrk500bins,
            JetSumPtTrk500BinLo       = hlt_jetSumPtTrk500binlo,
            JetSumPtTrk500BinHi       = hlt_jetSumPtTrk500binhi,

            # Binning for DEt
            JetDEtNBins               = hlt_jetDEtbins,
            JetDEtBinLo               = hlt_jetDEtbinlo,
            JetDEtBinHi               = hlt_jetDEtbinhi,
            
            # Binning for Delta eta/phi
            JetDepNBins               = hlt_jetDepbins,
            JetDepBinLo               = hlt_jetDepbinlo,
            JetDepBinHi               = hlt_jetDepbinhi,
            
            L1EtThresGeV              = hlt_level1EtThresholds,
            HLTEtThresGeV             = hlt_hltEtThresholds,

            L1EtaHighThres            = hlt_l1EtaHighThresholds,
            L1EtaLowThres             = hlt_l1EtaLowThresholds,
            HLTEtaHighThres           = hlt_hltEtaHighThresholds,
            HLTEtaLowThres            = hlt_hltEtaLowThresholds,

            HLTJetn                   =hlt_hltJetn,
            
            # collision mode flags

            isPP=pp,
            isHI=hi,
            isCosmic=cosmics,
            isMC=mc,
           # isPPb=pPb,

            # L1 Items for trigger efficiency
            primary_l1jet           = hltmonList.primary_l1jet,
            L1EffNBinsEt            = hlt_level1EtNBins,
            L1EffBinLoEtGeV         = hlt_level1EtBinsLow,
            L1EffBinHiEtGeV         = hlt_level1EtBinsHigh,
            #L1EffEtThresGeV         = hlt_level1EtThresholds,
            
            # HLT Chains for trigger efficiency
            primary_jet              = hltmonList.primary_jet,
            hltContainers            = hlt_hltContainers,
            HLTEffNBinsEt            = hlt_hltEtNBins,
            HLTEffBinLoEtGeV         = hlt_hltEtBinsLow,
            HLTEffBinHiEtGeV         = hlt_hltEtBinsHigh,

            OFEtThresGeV            = hlt_offlineEtThresholds,

            # chains by regexp
            ##JetChainsRegex          = hlt_jetchainsbyregexp,
                                  
            ## HLT Jet EMFraction > X 
            EMFractionCut        = 0.9,
                                   
            # for matching offline jet to L1,L2,EF
            DeltaRCut            = 0.3,
            
            # Selection cuts for OF jets used for basic histograms
            DoOFJetSelectionForBasicHists = False,
                                   
            # Selection cuts for OF jets used for trigger efficiency
            # This flag will override all others below
            DoOFJetSelectionForTrigEff = True,
            
            ## Offline Jet EMFraction > X 
            # 0 < EM Fraction < 1 
            #DoOFJetEMFCut = True,
            #OFEMFracCut = OFEMfractionCuts,
        
            ## Offline Jet pT > X 
            DoOFMinJetPtCut = True,
            OFMinJetPtGeV = 30. , 
                                   
            ## Offline Jet |eta| < X 
            DoOFAbsJetEtaCut = True,
            OFAbsJetEta = 3.2 ,
                                   
            ## Num of jets > N (N = -1 if all jets are required)
            DoOFMaxNJetCut = True,
            OFMaxNJet = -1, 
            
            ## Remove Jets with Bad Quality (select from: LooseBad, MediumBad, TightBad)
            DoOFJetQualityCut = False,
            OFJetQuality = "LooseBad",
            JetCategory =  "LooseBad",

            ## Use jet energy at this scale
            ## JETEMSCALE (same as UNCALIBRATED), JETFINAL (same as CALIBRATED)
            DoOFSignalState = False,
            OFSignalState = "JETFINAL",
            
            ## n90 => no. of constituents having 90% of jet energy
            #DoOFMinN90JetCut = True,
            #OFMinN90Jet = 5,    

            ## Out-of-time Calo Jet Energy (not to exceed X ns)
            #DoOFMaxJetTimeCut = True,
            #OFMaxTimens = 50.
        );
  from AthenaCommon.AppMgr import ToolSvc
  #from AthenaCommon import CfgMgr

 # from LumiBlockComps.LuminosityToolDefault import LuminosityToolDefault
 # ToolSvc+=LuminosityToolDefault()
  
 # from LumiBlockComps.TrigLivefractionToolDefault import TrigLivefractionToolDefault
 # ToolSvc+=TrigLivefractionToolDefault()

 # from TrigHLTMonitoring.HLTMonFlags import HLTMonFlags
 # HLTMonFlags.doEgamma.set_Value_and_Lock(False)

  # Set up the trigger configuration tool:
  #ToolSvc += CfgMgr.TrigConf__xAODConfigTool( "xAODConfigTool",
                                            #OutputLevel = 2 )

  # Set up the TDT:
  #ToolSvc += CfgMgr.Trig__TrigDecisionTool( "TrigDecisionTool",
                                          #OutputLevel = 2,
                                          #ConfigTool = ToolSvc.xAODConfigTool,
                                          #TrigDecisionKey = "xTrigDecision" )

  list = [ HLTJetMon ];
  return list


