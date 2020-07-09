# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

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
#  author(s) : Lee Sawyer (lee.sawyer@cern.ch)
#################################################################################################

import math

## HLT & offline jet keys 
## internal:StoreGate Key ordered pair
hlt_JetKeys = { "a10tcemsubFS" : "HLT_xAOD__JetContainer_a10tcemsubFS",
                "a4tcemsubFS" : "HLT_xAOD__JetContainer_a4tcemsubFS", 
                "a4tcemjesFS" : "HLT_xAOD__JetContainer_a4tcemjesFS", 
                "a4tcemsubjesFS" : "HLT_xAOD__JetContainer_a4tcemsubjesFS",
                "a4tclcwjesFS" : "HLT_xAOD__JetContainer_a4tclcwjesFS" }

hlt_offlineJetKeys = { "AntiKt4LCTopoJets" : "AntiKt4LCTopoJets", 
                       "AntiKt4EMTopoJets" : "AntiKt4EMTopoJets", 
                       "AntiKt10LCTopoJets" : "AntiKt10LCTopoJets"}

## Monitoring triggers
## Modify to the default naming scheme for menu-aware montioring.  
hlt_monitoring_l1jet = ['L1_RD0', 'L1_J15']
hlt_monitoring_jet = ['j25', 'j60'] 

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

# Binning for EMFrac
hlt_jetemfracbins = [ 52]
hlt_jetemfracbinlo = [ -0.02]
hlt_jetemfracbinhi = [ 1.02 ]

# Binning for DEt
hlt_jetDEtbins = [ 42 ]
hlt_jetDEtbinlo = [ -70. ]
hlt_jetDEtbinhi = [ 70. ]

# Binning for DeltaEta/DeltaPhi
hlt_jetDepbins = [ 12 ]
hlt_jetDepbinlo = [ -6.0 ]
hlt_jetDepbinhi = [ 6.0 ]

## Items/Chains for Trigger Efficiency 
# NB1: ordered-pair: internal name:chain/item name used internally
# NB2: ordered-pair: internal name:threshold value
# NB3: python lists are alphabetically ordered
# 4-Feb-2015 Modified to match default naming scheme for menu-aware monitoring

# L1 items   
hlt_primary_l1jet                 = [ 'L1_J15' ]
hlt_level1EtThresholds        = { 'L1_J15':50. }
hlt_level1EtNBins             = [ 40 ]
hlt_level1EtBinsLow           = [ 0. ]
hlt_level1EtBinsHigh          = [ 400. ]

# HLT items
hlt_primary_jet                    = ['j25', 'j60' ] 
hlt_hltEtThresholds            = { 'j25':20., 'j60':50. }
hlt_hltEtNBins                 = [ 50, 50 ]
hlt_hltEtBinsLow               = [ 0., 0. ]
hlt_hltEtBinsHigh              = [ 500., 500.]

# Offline 
hlt_offlineEtThresholds        = { 'L1_J15':10., 'j25':20., 'j60':50. }
            
## implemented but not being used [todo]
##hlt_jetchainsbyregexp         = [ "(L2|EF)_[0-9]?[jJ][0-9]+", "(L2|EF)_[0-9]+[fj][j0-9]+"]

## define instance of monitoring tool
def TrigJetMonitoringTool():
  from TrigJetMonitoring.TrigJetMonitoringConf import HLTJetMonTool
  HLTJetMon = HLTJetMonTool ( 
            name                 = 'HLTJetMon',
            histoPathBase        = "/Trigger/HLT",
            JetMonBase           = "/HLT/JetMon",
            DoL1Efficiency       = True,         # w.r.t offline
            DoOfflineJets        = True,         # fill offline jet plots
            DoHLTEfficiency      = True,         # w.r.t offline (HLT eff = L1 & HLT)
                                   
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
            monitoring_l1jet       = hlt_monitoring_l1jet,
            monitoring_jet         = hlt_monitoring_jet,

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

            # Binning for DEt
            JetDEtNBins               = hlt_jetDEtbins,
            JetDEtBinLo               = hlt_jetDEtbinlo,
            JetDEtBinHi               = hlt_jetDEtbinhi,
            
            # Binning for Delta eta/phi
            JetDepNBins               = hlt_jetDepbins,
            JetDepBinLo               = hlt_jetDepbinlo,
            JetDepBinHi               = hlt_jetDepbinhi,
            
            L1EtThresGeV            = hlt_level1EtThresholds,
            HLTEtThresGeV           = hlt_hltEtThresholds,

            # L1 Items for trigger efficiency
            primary_l1jet           = hlt_primary_l1jet,
            L1EffNBinsEt            = hlt_level1EtNBins,
            L1EffBinLoEtGeV         = hlt_level1EtBinsLow,
            L1EffBinHiEtGeV         = hlt_level1EtBinsHigh,
            #L1EffEtThresGeV         = hlt_level1EtThresholds,
            
            # HLT Chains for trigger efficiency
            primary_jet              = hlt_primary_jet,
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
        )

  list = [ HLTJetMon ]
  return list
