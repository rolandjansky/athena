# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#################################################################################################
#  file: TrigJetMonitoring.py
#  purpose : configure TrigJetMonitoring for 
#            a) Monitoring histograms per chain
#               Basic Quantities, Correlations
#            b) Event selection
#            c) Jet selection / cleaning
#            d) Jet / HLT trigger object matching
#            e) Trigger efficiency estimation 
#
#  author(s) : Venkatesh Kaushik (venkat.kaushik@cern.ch)
#################################################################################################

import math

## offline jet keys 
## internal:StoreGate Key ordered pair
hlt_offlineJetKeys = { "AntiKt4LCTopoJets" : "AntiKt4LCTopoJets", "AntiKt4EMTopoJets" : "AntiKt4EMTopoJets", "AntiKt10LCTopoJEts" : "AntiKt10LCTopoJets" }


## sample selection (orthogonal) triggers
hlt_eventSelectionTriggers = [ "L1_MU0", "L1_MU4", "L1_MU6", "L1_MU10", "L1_MU11", "L1_MU20", "L1_MBTS_1_1", "L1_RD0_FILLED" ]

## Triggers for basic plots
## "Internal:Physical"

## monitoring triggers 
hlt_basicL1Triggers = { 'L1_J100':'L1_J100','L1_FJ100':'L1_FJ100' }
hlt_basicEFTriggers  = { 'HLT_j460':'HLT_j460_a10_L1J100','HLT_j200':'HLT_j200_320eta490','HLT_3j175':'HLT_3j175','HLT_4j100':'HLT_4j100','HLT_5j85':'HLT_5j85' } 

# Binning for NJets
hlt_njetbins = [ 55 ]
hlt_njetbinlo = [ 0.5 ]
hlt_njetbinhi = [ 54.5 ]

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

# Binning for DEt
hlt_jetDEtbins = [ 42 ]
hlt_jetDEtbinlo = [ -70. ]
hlt_jetDEtbinhi = [ 70. ]

# Binning for DeltaEta/DeltaPhi
hlt_jetDepbins = [ 12 ]
hlt_jetDepbinlo = [ -6.0 ]
hlt_jetDepbinhi = [ 6.0 ]

## Items/Chains for Trigger Efficiency 
# NB1: ordered-pair: internal name:chain/item name
# NB2: ordered-pair: internal name:threshold value
# NB3: python lists are alphabetically ordered

## L1 items   
hlt_level1ItemNames           = { 'L1_J100':'L1_J100' }
hlt_level1EtThresholds        = { 'L1_J100':100. }
hlt_level1EtNBins             = [ 40 ]
hlt_level1EtBinsLow           = [ 0. ]
hlt_level1EtBinsHigh          = [ 400. ]

# DC14 jet menu items
hlt_efChainNames              = { 'HLT_j460':'HLT_j460_a10_L1J100','HLT_j200':'HLT_j200_320eta490','HLT_3j175':'HLT_3j175','HLT_4j100':'HLT_4j100','HLT_5j85':'HLT_5j85' } 
hlt_efEtThresholds            = { 'HLT_j460_a10_L1J100':150., 'HLT_j200_a4hi_320eta490':150., 'HLT_3j175':100. }
hlt_efEtNBins                 = [ 50, 50, 50 ]
hlt_efEtBinsLow               = [ 0., 0., 0. ]
hlt_efEtBinsHigh              = [ 500., 500., 500.]


hlt_offlineEtThresholds        = { 'L1_J100':100., 'L2_FJ100':100., 'HLT_j460_a10_L1J100':150., 'HLT_j200_a4hi_320eta490':150., 'HLT_3j175':100. }
            
## implemented but not being used [todo]
hlt_jetchainsbyregexp         = [ "(L2|EF)_[0-9]?[jJ][0-9]+", "(L2|EF)_[0-9]+[fj][j0-9]+"]

## define instance of monitoring tool
def TrigJetMonitoringTool():
  from TrigJetMonitoring.TrigJetMonitoringConf import HLTJetMonTool
  HLTJetMon = HLTJetMonTool ( 
            name                 = 'HLTJetMon',
            histoPathBase        = "/Trigger/HLT",
            JetMonBase           = "/HLT/JetMon",
            DoL1Efficiency       = True,         # w.r.t offline
	    DoOfflineJets        = True,         # fill offline jet plots
            DoEFEfficiency       = True,         # w.r.t offline (EF eff = L1 & L2 & EF)

            # Select events based on any of these passing EvtSelTriggers 
            DoEventSelection     = False,
            EvtSelTriggers       = hlt_eventSelectionTriggers,
                                   
            # SG Keys for L1, EF, OF Jets
            L1xAODJetKey         = "LVL1JetRoIs",
            EFJetKey             = "HLT_xAOD__JetContainer_TrigHLTJetRec",
            OFJetKeys            = hlt_offlineJetKeys,

            # Directories for histograms 
            # Modifying these is not recommended since
            # it requires change in HLTjet han config 
            # Prefix '/' for all *Dir 
            L1JetDir             = "/L1",
            EFJetDir             = "/EF",
            EfficiencyDir        = "/TrigEff",

            ## IF set, offline jet algs will have this prefix
            # Modifying this is not recommended since
            # it requires change in HLTjet han config 
            OFJetDirPrefix       = "OF",
           
            # Define basic L1/EF chains
            BasicL1Chains        = hlt_basicL1Triggers,
            BasicEFChains        = hlt_basicEFTriggers,

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

            # Binning for DEt
            JetDEtNBins               = hlt_jetDEtbins,
            JetDEtBinLo               = hlt_jetDEtbinlo,
            JetDEtBinHi               = hlt_jetDEtbinhi,
            
            # Binning for Delta eta/phi
            JetDepNBins               = hlt_jetDepbins,
            JetDepBinLo               = hlt_jetDepbinlo,
            JetDepBinHi               = hlt_jetDepbinhi,
            
            L1EtThresGeV            = hlt_level1EtThresholds,
            EFEtThresGeV            = hlt_efEtThresholds,

            # L1 Items for trigger efficiency
            L1ItemNames             = hlt_level1ItemNames,
            L1EffNBinsEt            = hlt_level1EtNBins,
            L1EffBinLoEtGeV         = hlt_level1EtBinsLow,
            L1EffBinHiEtGeV         = hlt_level1EtBinsHigh,
            #L1EffEtThresGeV         = hlt_level1EtThresholds,
            
            # EF Chains for trigger efficiency
            EFChainNames            = hlt_efChainNames,
            EFEffNBinsEt            = hlt_efEtNBins,
            EFEffBinLoEtGeV         = hlt_efEtBinsLow,
            EFEffBinHiEtGeV         = hlt_efEtBinsHigh,

            OFEtThresGeV            = hlt_offlineEtThresholds,
            #EFEffEtThresGeV         = hlt_efEtThresholds,

            # chains by regexp
            JetChainsRegex          = hlt_jetchainsbyregexp,
                                  
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
  ToolSvc += HLTJetMon;
  list = [ "HLTJetMonTool/HLTJetMon" ];
  return list
