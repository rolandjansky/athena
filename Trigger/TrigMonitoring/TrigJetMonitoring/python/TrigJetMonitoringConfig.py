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
#  author(s) : Lee Sawyer (lee.sawyer@cern.ch)
#################################################################################################

import math

## HLT & offline jet keys 
## internal:StoreGate Key ordered pair
hlt_JetKeys = { "a10tcemsubFS" : "HLT_xAOD__JetContainer_a10tcemsubFS",
                "a4tcemsubFS" : "HLT_xAOD__JetContainer_a4tcemsubFS", 
                "a4tcemjesFS" : "HLT_xAOD__JetContainer_a4tcemjesFS", 
                "a4tcemjesPS" : "HLT_xAOD__JetContainer_a4tcemjesPS",
                "a4tcemsubjesFS" : "HLT_xAOD__JetContainer_a4tcemsubjesFS",
		"a4tclcwjesFS" : "HLT_xAOD__JetContainer_a4tclcwjesFS" }

hlt_offlineJetKeys = { "AntiKt4LCTopoJets" : "AntiKt4LCTopoJets", 
                       "AntiKt4EMTopoJets" : "AntiKt4EMTopoJets", 
                       "AntiKt10LCTopoJets" : "AntiKt10LCTopoJets"}

# Binning for NJets
hlt_njetbins = [ 55 ]
hlt_njetbinlo = [ 0 ]
hlt_njetbinhi = [ 55 ]

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
hlt_level1EtThresholds        = { 'L1_J15':5.,'L1_RD0_FILLED':0.,'L1_J400':150.}
hlt_level1EtNBins             = [ 40 ]
hlt_level1EtBinsLow           = [ 0. ]
hlt_level1EtBinsHigh          = [ 400. ]

# HLT items
hlt_hltEtThresholds            = { 'j25':20.,'j25_320eta490':20,'j60':50. , 'j60_L1RD0':50.,'j60_280eta320':50.,'j60_320eta490':50.,'j200_jes_PS':100.,'j260':150.,'j360':250.,'j460_a10_sub_L1J100':350.,'j80_xe80':50.,'4j45':20.}
hlt_hltEtaHighThresholds       = { 'j25':3.2,'j25_320eta490':4.9,'j60':3.2 , 'j60_L1RD0':3.2,'j60_280eta320':3.2,'j60_320eta490':4.9,'j200_jes_PS':3.2,'j260':3.2, 'j360':3.2, 'j460_a10_sub_L1J100':3.2,'j80_xe80':3.2,'4j45':3.2}
hlt_hltEtaLowThresholds        = { 'j25':0.,'j25_320eta490':3.2, 'j60':0.,   'j60_L1RD0':0., 'j60_280eta320':2.8,'j60_320eta490':3.2,'j200_jes_PS':0.,'j260':0.,  'j360':0.,  'j460_a10_sub_L1J100':0.,'j80_xe80':0.,'4j45':0.}
hlt_hltContainers              = {'j25':'a4tcemsubjesFS','j25_320eta490':'a4tcemsubjesFS','j60':'a4tcemsubjesFS','j60_L1RD0':'a4tcemsubjesFS','j60_280eta320':'a4tcemsubjesFS','j60_320eta490':'a4tcemsubjesFS','j200_jes_PS':'a4tcemjesPS','j260':'a4tcemsubjesFS','j360':'a4tcemsubjesFS','j460_a10_sub_L1J100':'a10tcemsubFS','j80_xe80':'a4tcemsubjesFS','4j45':'a4tcemsubjesFS'}
hlt_hltEtNBins                 = [ 50, 50 ]
hlt_hltEtBinsLow               = [ 0., 0. ]
hlt_hltEtBinsHigh              = [ 500., 500.]
# Offline 
hlt_offlineEtThresholds        = { 'L1_J15':10., 'j25':20.,'j25_320eta490':20., 'j60':50.  , 'j60_L1RD0':50.,'j60_280eta320':50.,'j60_320eta490':50.,'j200_jes_PS':100.,'j260':150.,'j360':250.,'j460_a10_sub_L1J100':350.,'j80_xe80':50.,'4j45':20.}
#Dijet chains

l1_DijetChains  = ['L1_J400']
hlt_DijetChains = ['j25']
of_DijetChains  = ['AntiKt4EMTopoJets']
            
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

            HLTEtaHighThres           = hlt_hltEtaHighThresholds,
            HLTEtaLowThres            = hlt_hltEtaLowThresholds,

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

  
  ToolSvc += HLTJetMon;

  # Set up the trigger configuration tool:
  #ToolSvc += CfgMgr.TrigConf__xAODConfigTool( "xAODConfigTool",
                                            #OutputLevel = 2 )

  # Set up the TDT:
  #ToolSvc += CfgMgr.Trig__TrigDecisionTool( "TrigDecisionTool",
                                          #OutputLevel = 2,
                                          #ConfigTool = ToolSvc.xAODConfigTool,
                                          #TrigDecisionKey = "xTrigDecision" )

  list = [ "HLTJetMonTool/HLTJetMon" ];
  return list


