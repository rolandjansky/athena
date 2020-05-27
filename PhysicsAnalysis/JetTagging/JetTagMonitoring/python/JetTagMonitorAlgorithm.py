#
#  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#

'''@file JetTagMonitorAlgorithm.py
@author A. Lapertosa
@author S. Strandberg
@date 2019-06-17
@brief JetTagging python configuration for the Run III AthenaMonitoring package, based on the example by C Burton and P Onyisi'''

import math

def JetTagMonitorConfig(inputFlags):
    '''Function to configures some algorithms in the monitoring system.'''

    ### STEP 1 ###
    # Define one top-level monitoring algorithm. The new configuration 
    # framework uses a component accumulator.
    #from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
    #result = ComponentAccumulator()

    # The following class will make a sequence, configure algorithms, and link
    # them to GenericMonitoringTools
    from AthenaMonitoring import AthMonitorCfgHelper
    helper = AthMonitorCfgHelper(inputFlags,'JetTagAthMonitorCfg')

    ### STEP 2 ###
    # Adding an algorithm to the helper. Here, we will use the jetTagging 
    # algorithm in the AthenaMonitoring package. Just pass the type to the 
    # helper. Then, the helper will instantiate an instance and set up the 
    # base class configuration following the inputFlags. The returned object 
    # is the algorithm.
    # You can actually make multiple instances of the same algorithm and give 
    # them different configurations
    from AthenaConfiguration.ComponentFactory import CompFactory
    #jetTagGeneral = helper.addAlgorithm(JetTagMonitorAlgorithm,'JetTagGeneral')
    jetTagMonAlg = helper.addAlgorithm(CompFactory.JetTagMonitorAlgorithm,'JetTagMonAlg')
    #anotherJetTagMonAlg = helper.addAlgorithm(JetTagMonitorAlgorithm,'AnotherJetTagMonAlg')

    # If for some really obscure reason you need to instantiate an algorithm
    # yourself, the AddAlgorithm method will still configure the base 
    # properties and add the algorithm to the monitoring sequence.
    # helper.AddAlgorithm(myExistingAlg)
    
    ### STEP 3 ###
    # Edit properties of an algorithm
    # Declare properties
    jetTagMonAlg.ElectronTrigger_201X = "HLT_e[2-9][0-9]_.*" # electrons 20-99 GeV
    jetTagMonAlg.MuonTrigger_201X = "HLT_mu[2-9][0-9].*" # muons 20-99 GeV

    jetTagMonAlg.TriggerChain = ''
    #jetTagMonAlg.TriggerChain = 'HLT_mu26_ivarmedium'
    #jetTagMonAlg.TriggerChain = 'HLT_e24_lhtight_nod0'
    #jetTagMonAlg.RandomHist = True
    
    ### STEP 4 ###
    # Add some tools. N.B. Do not use your own trigger decision tool. Use the
    # standard one that is included with AthMonitorAlgorithm.

    # First, add a tool that's set up by a different configuration function. 
    # In this case, CaloNoiseToolCfg returns its own component accumulator, 
    # which must be merged with the one from this function.
    # from CaloTools.CaloNoiseToolConfig import CaloNoiseToolCfg
    # caloNoiseAcc, caloNoiseTool = CaloNoiseToolCfg(inputFlags)
    # result.merge(caloNoiseAcc)
    # jetTagMonAlg.CaloNoiseTool = caloNoiseTool

    # Then, add a tool that doesn't have its own configuration function. In
    # this jetTagging, no accumulator is returned, so no merge is necessary.
    # from MyDomainPackage.MyDomainPackageConf import MyDomainTool
    # jetTagMonAlg.MyDomainTool = MyDomainTool()

    # Add a generic monitoring tool (a "group" in old language). The returned 
    # object here is the standard GenericMonitoringTool.
    #GeneralGroup = helper.addGroup(jetTagGeneral,'JetTagMonitor')
    GeneralGroup = helper.addGroup(jetTagMonAlg,'JetTagMonitor','JetTagging')
    #JetGroup = helper.addGroup(jetTagMonAlg,'JetTagMonitor')
    #AnotherGroup = helper.addGroup(anotherJetTagMonAlg,'JetTagMonitor')

    # Add a GMT for the other jetTagging monitor algorithm
    #shifterGroup = helper.addGroup(shifterTrigEgammaMonAlg,'TrigEgammaMonitor','HLT/EgammaMon/Shifter/')

    ### STEP 5 ### 
    #title is for plot, x-axis, y-axis
    #type is type of plot?
    #path is for folder structure of root output file 

    ### General histograms <-----
    GeneralGroup.defineHistogram('Event_CutFlow',title='Event CutFlow;Pass Event CutFlow;Events',path='Cutflow_plots',xbins=10,xmin=-0.5,xmax=9.5)
    GeneralGroup.defineHistogram('Trigger_CutFlow',title='Trigger CutFlow;Pass Trigger CutFlow;Events',path='Cutflow_plots',xbins=10,xmin=-0.5,xmax=9.5)
    GeneralGroup.defineHistogram('Jet_CutFlow',title='Jet CutFlow;Pass Jet CutFlow;Jets',path='Cutflow_plots',xbins=10,xmin=-0.5,xmax=9.5)

    GeneralGroup.defineHistogram('run',title='Run number;Run;Event',path='Run_plots',xbins=500000,xmin=-0.5,xmax=499999.5)
    GeneralGroup.defineHistogram('lb',title='Lumi Block;LB;Events',path='Run_plots',xbins=1000,xmin=-0.5,xmax=999.5)
    GeneralGroup.defineHistogram('mu',title='Mu;<mu>;Events',path='Run_plots',xbins=100,xmin=0,xmax=100.0)
    GeneralGroup.defineHistogram('nPrimVtx',title='Primary vertices;PV;Events',path='Run_plots',xbins=100,xmin=0,xmax=100.0)
    GeneralGroup.defineHistogram('xPrimVtx',title='Primary vertices X position;X;Events',path='Run_plots',xbins=40,xmin=-2.0,xmax=2.0)
    GeneralGroup.defineHistogram('yPrimVtx',title='Primary vertices Y position;Y;Events',path='Run_plots',xbins=40,xmin=-2.0,xmax=2.0)
    GeneralGroup.defineHistogram('zPrimVtx',title='Primary vertices Z position;Z;Events',path='Run_plots',xbins=100,xmin=-250,xmax=250.0)
    GeneralGroup.defineHistogram('nTracks_PV',title='Number of tracks in PV;Tracks in PV;Number of tracks',path='Run_plots',xbins=150,xmin=-0.5,xmax=149.5)

    GeneralGroup.defineHistogram('nTracks',title='Track multiplicity;Tracks per event;Number of events',path='Run_plots',xbins=100,xmin=0,xmax=1000)
    GeneralGroup.defineHistogram('nHitsBLayer',title='Number of IBL hits;Hits on track;Number of IBL hits on track',path='Run_plots',xbins=5,xmin=0,xmax=5)
    GeneralGroup.defineHistogram('nHitsPixel',title='Number of Pixel hits;Hits on track;Number of Pixel hits on track',path='Run_plots',xbins=10,xmin=0,xmax=10)
    GeneralGroup.defineHistogram('nHitsSCT',title='Number of SCT hits;Hits on track;Number of SCT hits on track',path='Run_plots',xbins=15,xmin=0,xmax=15)
    GeneralGroup.defineHistogram('nHitsSi',title='Number of PIX+SCT hits;Hits on track;Number of PIX+SCT hits on track',path='Run_plots',xbins=25,xmin=0,xmax=25)
    GeneralGroup.defineHistogram('nHitsTRT',title='Number of TRT hits;Hits on track;Number of TRT hits on track',path='Run_plots',xbins=100,xmin=0,xmax=100)
    GeneralGroup.defineHistogram('nHitsID',title='Number of ID hits;Hits on track;Number of ID hits on track',path='Run_plots',xbins=150,xmin=0,xmax=150)

    GeneralGroup.defineHistogram('JetTracks_n',title='Track multiplicity in jet;Tracks;Number of tracks per jet',path='Run_plots',xbins=50,xmin=0,xmax=50)
    GeneralGroup.defineHistogram('JetTracks_pt',title='pT of tracks in jet;pT;Tracks in jets',path='Run_plots',xbins=100,xmin=0,xmax=100)
    GeneralGroup.defineHistogram('JetTracks_eta',title='#eta of tracks in jet;#eta;Tracks in jets',path='Run_plots',xbins=100,xmin=-2.5,xmax=2.5)
    GeneralGroup.defineHistogram('JetTracks_phi',title='#phi of tracks in jet;#phi;Tracks in jets',path='Run_plots',xbins=100,xmin=-1*math.pi,xmax=math.pi)
    GeneralGroup.defineHistogram('JetTracks_d0',title='d_0 of tracks in jet;d_0;Tracks in jets',path='Run_plots',xbins=100,xmin=-5,xmax=5)
    GeneralGroup.defineHistogram('JetTracks_z0',title='z_0 of tracks in jet;z_0;Tracks in jets',path='Run_plots',xbins=100,xmin=-300,xmax=300)
    GeneralGroup.defineHistogram('JetTracks_nHitsBLayer',title='Number of IBL hits on tracks in jets;Hits on track;Number of IBL hits on track',path='Run_plots',xbins=5,xmin=0,xmax=5)
    GeneralGroup.defineHistogram('JetTracks_nHitsPixel',title='Number of Pixel hits on tracks in jets;Hits on track;Number of Pixel hits on track',path='Run_plots',xbins=10,xmin=0,xmax=10)
    GeneralGroup.defineHistogram('JetTracks_nHitsSCT',title='Number of SCT hits on tracks in jets;Hits on track;Number of SCT hits on track',path='Run_plots',xbins=15,xmin=0,xmax=15)

    ### is ttbar histograms <-----
    #registerHist(*m_monGr_shift, m_jet_mv_w_top = TH1F_LW::create("jet_MV_top",(m_mv_algorithmName+" Tag Weight of Jets in Top Events; "+m_mv_algorithmName+" tag weight").c_str(),int(m_mv_nBins/2),m_mv_rangeStart,m_mv_rangeStop));
    #registerHist(*m_monGr_shift, m_jet_top_eff = TH1F_LW::create("jet_top_eff","Fraction of b-tagged Jets in Top Events",1,-0.5,0.5));
    #registerHist(*m_monGr_shift, m_jet_pt_top_eff = TH1F_LW::create("jet_pt_top_eff","Fraction of b-tagged Jets in Top Events vs pT; pT [GeV]",10,0.,200.));
    
    GeneralGroup.defineHistogram('muon_PT',title='Muon pT;Muon pT;Events',path='TTbar_plots',xbins=100,xmin=0.0,xmax=200000.0)
    GeneralGroup.defineHistogram('electron_PT',title='Electron pT;Electron pT;Events',path='TTbar_plots',xbins=100,xmin=0.0,xmax=200000.0)
    
    GeneralGroup.defineHistogram('nIsoMuons',title='Number of isolated Muons;Isolated muons;Number of events',path='TTbar_plots',xbins=3,xmin=-0.5,xmax=2.5)
    GeneralGroup.defineHistogram('nIsoElectrons',title='Number of isolated Electrons;Isolated electrons;Number of events',path='TTbar_plots',xbins=3,xmin=-0.5,xmax=2.5)

    GeneralGroup.defineHistogram('nTTbarGoodJets',title='Number of good jets in ttbar events;Good jets per event;Number of events',path='TTbar_plots',xbins=10,xmin=-0.5,xmax=9.5)
    GeneralGroup.defineHistogram('mvTTbarJets',title='MV of jets in ttbar events;Jet MV;Number of jets',path='TTbar_plots',xbins=50,xmin=-1,xmax=1)

    GeneralGroup.defineHistogram('nTTbarJets',title='Total number of ttbar jets;Number of jets;Number of events',path='TTbar_plots',xbins=1,xmin=-0.5,xmax=0.5)
    GeneralGroup.defineHistogram('nTTbarJets_60tag',title='Number of ttbar jets passing 60 tag WP; Jets passing 60 tag WP;Number of ttbar jets',path='TTbar_plots',xbins=1,xmin=-0.5,xmax=0.5)
    GeneralGroup.defineHistogram('nTTbarJets_70tag',title='Number of ttbar jets passing 70 tag WP; Jets passing 70 tag WP;Number of ttbar jets',path='TTbar_plots',xbins=1,xmin=-0.5,xmax=0.5)
    GeneralGroup.defineHistogram('nTTbarJets_77tag',title='Number of ttbar jets passing 77 tag WP; Jets passing 77 tag WP;Number of ttbar jets',path='TTbar_plots',xbins=1,xmin=-0.5,xmax=0.5)
    GeneralGroup.defineHistogram('nTTbarJets_85tag',title='Number of ttbar jets passing 85 tag WP; Jets passing 60 tag WP;Number of ttbar jets',path='TTbar_plots',xbins=1,xmin=-0.5,xmax=0.5)
    GeneralGroup.defineHistogram('pass60n,nTTbarJets',type='TEfficiency',title='TTbar jets 60 tag WP Efficiency;ttbar jets;60 tag WP Efficiency',path='Jet_Eff',xbins=1,xmin=-0.5,xmax=0.5)
    GeneralGroup.defineHistogram('pass70n,nTTbarJets',type='TEfficiency',title='TTbar jets 70 tag WP Efficiency;ttbar jets;70 tag WP Efficiency',path='Jet_Eff',xbins=1,xmin=-0.5,xmax=0.5)
    GeneralGroup.defineHistogram('pass77n,nTTbarJets',type='TEfficiency',title='TTbar jets 77 tag WP Efficiency;ttbar jets;77 tag WP Efficiency',path='Jet_Eff',xbins=1,xmin=-0.5,xmax=0.5)
    GeneralGroup.defineHistogram('pass85n,nTTbarJets',type='TEfficiency',title='TTbar jets 85 tag WP Efficiency;ttbar jets;85 tag WP Efficiency',path='Jet_Eff',xbins=1,xmin=-0.5,xmax=0.5)

    GeneralGroup.defineHistogram('ptTTbarJets',title='Number of ttbar jets vs pT;ttbar jet pT;Number of ttbar jets',path='TTbar_plots',xbins=40,xmin=0,xmax=200)
    GeneralGroup.defineHistogram('ptTTbarJets_60tag',title='Number of ttbar jets passing 60 tag WP vs pT;ttbar jet pT;Number of ttbar jets',path='TTbar_plots',xbins=40,xmin=0,xmax=200)
    GeneralGroup.defineHistogram('ptTTbarJets_70tag',title='Number of ttbar jets passing 70 tag WP vs pT;ttbar jet pT;Number of ttbar jets',path='TTbar_plots',xbins=40,xmin=0,xmax=200)
    GeneralGroup.defineHistogram('ptTTbarJets_77tag',title='Number of ttbar jets passing 77 tag WP vs pT;ttbar jet pT;Number of ttbar jets',path='TTbar_plots',xbins=40,xmin=0,xmax=200)
    GeneralGroup.defineHistogram('ptTTbarJets_85tag',title='Number of ttbar jets passing 85 tag WP vs pT;ttbar jet pT;Number of ttbar jets',path='TTbar_plots',xbins=40,xmin=0,xmax=200)
    GeneralGroup.defineHistogram('pass60p,ptTTbarJets',type='TEfficiency',title='TTbar jets 60 tag WP Efficiency vs pT;ttbar jet pT;60 tag WP Efficiency',path='Jet_Eff',xbins=40,xmin=0.0,xmax=200.0)
    GeneralGroup.defineHistogram('pass70p,ptTTbarJets',type='TEfficiency',title='TTbar jets 70 tag WP Efficiency vs pT;ttbar jet pT;70 tag WP Efficiency',path='Jet_Eff',xbins=40,xmin=0.0,xmax=200.0)
    GeneralGroup.defineHistogram('pass77p,ptTTbarJets',type='TEfficiency',title='TTbar jets 77 tag WP Efficiency vs pT;ttbar jet pT;77 tag WP Efficiency',path='Jet_Eff',xbins=40,xmin=0.0,xmax=200.0)
    GeneralGroup.defineHistogram('pass85p,ptTTbarJets',type='TEfficiency',title='TTbar jets 85 tag WP Efficiency vs pT;ttbar jet pT;85 tag WP Efficiency',path='Jet_Eff',xbins=40,xmin=0.0,xmax=200.0)

    ### is SMT jet histograms <-----
    GeneralGroup.defineHistogram('nSoftMuons',title='Number of Soft Muons;Muons per event;Number of muons',path='SMT_plots',xbins=3,xmin=-0.5,xmax=2.5)
    GeneralGroup.defineHistogram('nSMTJets',title='Number of SMT Jets;Jets per event;Number of jets',path='SMT_plots',xbins=3,xmin=-0.5,xmax=2.5)

    GeneralGroup.defineHistogram('drSMT',title='DeltaR between jet and soft muon;DR;Number of SMT candidates',path='SMT_plots',xbins=40,xmin=0,xmax=0.4)
    GeneralGroup.defineHistogram('ptSMT_mu',title='pT of SMT Muons;Muon pT [GeV];Number of muons',path='SMT_plots',xbins=50,xmin=0,xmax=100)
    GeneralGroup.defineHistogram('ptSMT_jet',title='pT of SMT Jets;Jet pT [GeV];Number of jets',path='SMT_plots',xbins=100,xmin=0,xmax=300)
    GeneralGroup.defineHistogram('ptSMT_ratio',title='pT ratio of SMT Jets (mu/jet);pT ratio;Number of SMT jets',path='SMT_plots',xbins=50,xmin=0,xmax=1)
    GeneralGroup.defineHistogram('pTrelSMT',title='pTrel of SMT Jets;pTrel;Number of jets',path='SMT_plots',xbins=50,xmin=0,xmax=5)
    GeneralGroup.defineHistogram('mvSMT',title='MV of SMT Jets;MV;Number of jets',path='SMT_plots',xbins=50,xmin=-1,xmax=1)

    ### jet quality cutflow histograms <-----

    #Distributions before selection (all jets from selected events)
    GeneralGroup.defineHistogram('nJets',title='Jet multiplicity;Number of jets;Events',path='Jet_quality',xbins=50,xmin=0,xmax=50)
    GeneralGroup.defineHistogram('jet_PT',title='Jet pT;Jet pT [GeV];Jets',path='Jet_quality',xbins=100,xmin=0.0,xmax=200.0)
    GeneralGroup.defineHistogram('jet_ETA',title='Jet eta;Jet #eta;Jets',path='Jet_quality',xbins=100,xmin=-2.5,xmax=2.5)
    GeneralGroup.defineHistogram('jet_PHI',title='Jet phi;Jet #phi;Jets',path='Jet_quality',xbins=100,xmin=-1*math.pi,xmax=math.pi)
    GeneralGroup.defineHistogram('jet_MV',title='Jet MV;Jet MV;Jets',path='Jet_quality',xbins=100,xmin=-1.0,xmax=1.0)

    #2D maps during selection
    GeneralGroup.defineHistogram('jet_eta_all,jet_phi_all;jet_MAP_all',title='2D MAP of all jets;Jet #eta;Jet #phi',type='TH2F',path='Jet_quality',xbins=100,xmin=-2.5,xmax=2.5,ybins=200,ymin=-1*math.pi,ymax=math.pi)
    GeneralGroup.defineHistogram('jet_eta_quality,jet_phi_quality;jet_MAP_quality',title='2D MAP of quality jets;Jet #eta;Jet #phi',type='TH2F',path='Jet_quality',xbins=100,xmin=-2.5,xmax=2.5,ybins=200,ymin=-1*math.pi,ymax=math.pi)
    GeneralGroup.defineHistogram('jet_eta_kin,jet_phi_kin;jet_MAP_kin',title='2D MAP of jets passing kinematic selection;Jet #eta;Jet #phi',type='TH2F',path='Jet_quality',xbins=100,xmin=-2.5,xmax=2.5,ybins=200,ymin=-1*math.pi,ymax=math.pi)
    GeneralGroup.defineHistogram('jet_eta_jvt,jet_phi_jvt;jet_MAP_jvt',title='2D MAP of jets passing JVT;Jet #eta;Jet #phi',type='TH2F',path='Jet_quality',xbins=100,xmin=-2.5,xmax=2.5,ybins=200,ymin=-1*math.pi,ymax=math.pi)
    GeneralGroup.defineHistogram('jet_eta_overlap,jet_phi_overlap;jet_MAP_overlap',title='2D MAP of not overlapping jets;Jet #eta;Jet #phi',type='TH2F',path='Jet_quality',xbins=100,xmin=-2.5,xmax=2.5,ybins=200,ymin=-1*math.pi,ymax=math.pi)
    GeneralGroup.defineHistogram('jet_eta_good,jet_phi_good;jet_MAP_good',title='2D MAP of good jets;Jet #eta;Jet #phi',type='TH2F',path='Jet_quality',xbins=100,xmin=-2.5,xmax=2.5,ybins=200,ymin=-1*math.pi,ymax=math.pi)
    GeneralGroup.defineHistogram('jet_eta_suspect,jet_phi_suspect;jet_MAP_suspect',title='2D MAP of suspect jets;Jet #eta;Jet #phi',type='TH2F',path='Jet_quality',xbins=100,xmin=-2.5,xmax=2.5,ybins=200,ymin=-1*math.pi,ymax=math.pi)
    GeneralGroup.defineHistogram('jet_eta_bad,jet_phi_bad;jet_MAP_bad',title='2D MAP of bad jets;Jet #eta;Jet #phi',type='TH2F',path='Jet_quality',xbins=100,xmin=-2.5,xmax=2.5,ybins=200,ymin=-1*math.pi,ymax=math.pi)
    GeneralGroup.defineHistogram('jet_eta_smt,jet_phi_smt;jet_MAP_smt',title='2D MAP of SMT jets;Jet #eta;Jet #phi',type='TH2F',path='Jet_quality',xbins=100,xmin=-2.5,xmax=2.5,ybins=200,ymin=-1*math.pi,ymax=math.pi)

    #GeneralGroup.defineHistogram('jet_MV_JVT_bef',title='Jet MV before JVT;Jet MV;Jets',path='Jet_quality',xbins=100,xmin=-1.0,xmax=1.0)
    #GeneralGroup.defineHistogram('jet_MV_JVT_aft',title='Jet MV after JVT;Jet MV;Jets',path='Jet_quality',xbins=100,xmin=-1.0,xmax=1.0)
    
    ###Distributions for good jets (passing Good/Suspect/Bad selection)
    GeneralGroup.defineHistogram('jet_pT_good',title='Good jets pT;Good Jet pT [GeV];Jets',path='Jet_MV',xbins=100,xmin=0.0,xmax=200.0)
    GeneralGroup.defineHistogram('jet_eta_good',title='Good jets #eta;Good Jet #eta;Jets',path='Jet_MV',xbins=100,xmin=-2.5,xmax=2.5)
    GeneralGroup.defineHistogram('jet_phi_good',title='Good jets #phi;Good Jet #phi;Jets',path='Jet_MV',xbins=100,xmin=-1*math.pi,xmax=math.pi)
    GeneralGroup.defineHistogram('jet_MV_good',title='Good jets MV;Good Jet MV;Jets',path='Jet_MV',xbins=100,xmin=-1.0,xmax=1.0)

    ### tag weight histograms <-----
    
    GeneralGroup.defineHistogram('jet_MV_mu_0_30',title='Jet MV in mu bin 1 : <mu> = [0,30];Jet MV;Jets',path='Jet_MV',xbins=100,xmin=-1.0,xmax=1.0)
    GeneralGroup.defineHistogram('jet_MV_mu_30_50',title='Jet MV in mu bin 2 : <mu> = [30,50];Jet MV;Jets',path='Jet_MV',xbins=100,xmin=-1.0,xmax=1.0)
    GeneralGroup.defineHistogram('jet_MV_mu_50_100',title='Jet MV in mu bin 3 : <mu> = [50,100];Jet MV;Jets',path='Jet_MV',xbins=100,xmin=-1.0,xmax=1.0)

    GeneralGroup.defineHistogram('jet_MV_pt_0_20',title='Jet MV in pT bin 1 : pT = [0,20];Jet MV;Jets',path='Jet_MV',xbins=100,xmin=-1.0,xmax=1.0)
    GeneralGroup.defineHistogram('jet_MV_pt_20_40',title='Jet MV in pT bin 2 : pT = [20,40];Jet MV;Jets',path='Jet_MV',xbins=100,xmin=-1.0,xmax=1.0)
    GeneralGroup.defineHistogram('jet_MV_pt_40_70',title='Jet MV in pT bin 3 : pT = [40,70];Jet MV;Jets',path='Jet_MV',xbins=100,xmin=-1.0,xmax=1.0)
    GeneralGroup.defineHistogram('jet_MV_pt_70_100',title='Jet MV in pT bin 4 : pT = [70,100];Jet MV;Jets',path='Jet_MV',xbins=100,xmin=-1.0,xmax=1.0)
    GeneralGroup.defineHistogram('jet_MV_pt_100_150',title='Jet MV in pT bin 5 : pT = [100,150];Jet MV;Jets',path='Jet_MV',xbins=100,xmin=-1.0,xmax=1.0)
    GeneralGroup.defineHistogram('jet_MV_pt_150_200',title='Jet MV in pT bin 6 : pT = [150,200];Jet MV;Jets',path='Jet_MV',xbins=100,xmin=-1.0,xmax=1.0)
    GeneralGroup.defineHistogram('jet_MV_pt_200_1000',title='Jet MV in pT bin 7 : pT = [200,1000];Jet MV;Jets',path='Jet_MV',xbins=100,xmin=-1.0,xmax=1.0)

    GeneralGroup.defineHistogram('jet_MV_eta_00_05',title='Jet MV in #eta bin 1 : #eta = [0.0,0.5];Jet MV;Jets',path='Jet_MV',xbins=100,xmin=-1.0,xmax=1.0)
    GeneralGroup.defineHistogram('jet_MV_eta_05_10',title='Jet MV in #eta bin 2 : #eta = [0.5,1.0];Jet MV;Jets',path='Jet_MV',xbins=100,xmin=-1.0,xmax=1.0)
    GeneralGroup.defineHistogram('jet_MV_eta_10_15',title='Jet MV in #eta bin 3 : #eta = [1.0,1.5];Jet MV;Jets',path='Jet_MV',xbins=100,xmin=-1.0,xmax=1.0)
    GeneralGroup.defineHistogram('jet_MV_eta_15_20',title='Jet MV in #eta bin 4 : #eta = [1.5,2.0];Jet MV;Jets',path='Jet_MV',xbins=100,xmin=-1.0,xmax=1.0)
    GeneralGroup.defineHistogram('jet_MV_eta_20_25',title='Jet MV in #eta bin 5 : #eta = [2.0,2.5];Jet MV;Jets',path='Jet_MV',xbins=100,xmin=-1.0,xmax=1.0)

    GeneralGroup.defineHistogram('jet_MV_phi_00_05',title='Jet MV in #phi bin 1 : #phi = [0.0,0.5];Jet MV;Jets',path='Jet_MV',xbins=100,xmin=-1.0,xmax=1.0)
    GeneralGroup.defineHistogram('jet_MV_phi_05_10',title='Jet MV in #phi bin 2 : #phi = [0.5,1.0];Jet MV;Jets',path='Jet_MV',xbins=100,xmin=-1.0,xmax=1.0)
    GeneralGroup.defineHistogram('jet_MV_phi_10_15',title='Jet MV in #phi bin 3 : #phi = [1.0,1.5];Jet MV;Jets',path='Jet_MV',xbins=100,xmin=-1.0,xmax=1.0)
    GeneralGroup.defineHistogram('jet_MV_phi_15_20',title='Jet MV in #phi bin 4 : #phi = [1.5,2.0];Jet MV;Jets',path='Jet_MV',xbins=100,xmin=-1.0,xmax=1.0)
    GeneralGroup.defineHistogram('jet_MV_phi_20_25',title='Jet MV in #phi bin 5 : #phi = [2.0,2.5];Jet MV;Jets',path='Jet_MV',xbins=100,xmin=-1.0,xmax=1.0)
    GeneralGroup.defineHistogram('jet_MV_phi_25_31',title='Jet MV in #phi bin 6 : #phi = [2.5,3.1];Jet MV;Jets',path='Jet_MV',xbins=100,xmin=-1.0,xmax=1.0)

    #jets passing WPs in eta and phi bins, 2D map and efficiency
    GeneralGroup.defineHistogram('jet_eta_60tag',title='Number of jets passing 60 tag WP vs #eta;Jet #eta;Number of jets',path='Jet_MV',xbins=10,xmin=-2.5,xmax=2.5)
    GeneralGroup.defineHistogram('jet_eta_70tag',title='Number of jets passing 70 tag WP vs #eta;Jet #eta;Number of jets',path='Jet_MV',xbins=10,xmin=-2.5,xmax=2.5)
    GeneralGroup.defineHistogram('jet_eta_77tag',title='Number of jets passing 77 tag WP vs #eta;Jet #eta;Number of jets',path='Jet_MV',xbins=10,xmin=-2.5,xmax=2.5)
    GeneralGroup.defineHistogram('jet_eta_85tag',title='Number of jets passing 85 tag WP vs #eta;Jet #eta;Number of jets',path='Jet_MV',xbins=10,xmin=-2.5,xmax=2.5)
    GeneralGroup.defineHistogram('jet_phi_60tag',title='Number of jets passing 60 tag WP vs #phi;Jet #phi;Number of jets',path='Jet_MV',xbins=12,xmin=-1*math.pi,xmax=math.pi)
    GeneralGroup.defineHistogram('jet_phi_70tag',title='Number of jets passing 70 tag WP vs #phi;Jet #phi;Number of jets',path='Jet_MV',xbins=12,xmin=-1*math.pi,xmax=math.pi)
    GeneralGroup.defineHistogram('jet_phi_77tag',title='Number of jets passing 77 tag WP vs #phi;Jet #phi;Number of jets',path='Jet_MV',xbins=12,xmin=-1*math.pi,xmax=math.pi)
    GeneralGroup.defineHistogram('jet_phi_85tag',title='Number of jets passing 85 tag WP vs #phi;Jet #phi;Number of jets',path='Jet_MV',xbins=12,xmin=-1*math.pi,xmax=math.pi)
    GeneralGroup.defineHistogram('jet_eta_60tag,jet_phi_60tag;jet_MAP_60tag',title='2D MAP of jets passing 60 tag WP;Jet #eta;Jet #phi',type='TH2F',path='Jet_MV',xbins=100,xmin=-2.5,xmax=2.5,ybins=200,ymin=-1*math.pi,ymax=math.pi)
    GeneralGroup.defineHistogram('jet_eta_70tag,jet_phi_70tag;jet_MAP_70tag',title='2D MAP of jets passing 70 tag WP;Jet #eta;Jet #phi',type='TH2F',path='Jet_MV',xbins=100,xmin=-2.5,xmax=2.5,ybins=200,ymin=-1*math.pi,ymax=math.pi)
    GeneralGroup.defineHistogram('jet_eta_77tag,jet_phi_77tag;jet_MAP_77tag',title='2D MAP of jets passing 77 tag WP;Jet #eta;Jet #phi',type='TH2F',path='Jet_MV',xbins=100,xmin=-2.5,xmax=2.5,ybins=200,ymin=-1*math.pi,ymax=math.pi)
    GeneralGroup.defineHistogram('jet_eta_85tag,jet_phi_85tag;jet_MAP_85tag',title='2D MAP of jets passing 85 tag WP;Jet #eta;Jet #phi',type='TH2F',path='Jet_MV',xbins=100,xmin=-2.5,xmax=2.5,ybins=200,ymin=-1*math.pi,ymax=math.pi)

    GeneralGroup.defineHistogram('jet_eta',title='Number of jets before passing tag vs #eta;Jet #eta;Number of jets',path='Jet_MV',xbins=10,xmin=-2.5,xmax=2.5)
    GeneralGroup.defineHistogram('pass60e,jet_eta',type='TEfficiency',title='Jets 60 tag WP Efficiency vs #eta;Jet #eta;60 tag WP Efficiency',path='Jet_Eff',xbins=10,xmin=-2.5,xmax=2.5)
    GeneralGroup.defineHistogram('pass70e,jet_eta',type='TEfficiency',title='Jets 70 tag WP Efficiency vs #eta;Jet #eta;70 tag WP Efficiency',path='Jet_Eff',xbins=10,xmin=-2.5,xmax=2.5)
    GeneralGroup.defineHistogram('pass77e,jet_eta',type='TEfficiency',title='Jets 77 tag WP Efficiency vs #eta;Jet #eta;77 tag WP Efficiency',path='Jet_Eff',xbins=10,xmin=-2.5,xmax=2.5)
    GeneralGroup.defineHistogram('pass85e,jet_eta',type='TEfficiency',title='Jets 85 tag WP Efficiency vs #eta;Jet #eta;85 tag WP Efficiency',path='Jet_Eff',xbins=10,xmin=-2.5,xmax=2.5)

    GeneralGroup.defineHistogram('jet_phi',title='Number of jets before passing tag vs #phi;Jet #phi;Number of jets',path='Jet_MV',xbins=12,xmin=-1*math.pi,xmax=math.pi)
    GeneralGroup.defineHistogram('pass60f,jet_phi',type='TEfficiency',title='Jets 60 tag WP Efficiency vs #phi;Jet #phi;60 tag WP Efficiency',path='Jet_Eff',xbins=12,xmin=-1*math.pi,xmax=math.pi)
    GeneralGroup.defineHistogram('pass70f,jet_phi',type='TEfficiency',title='Jets 70 tag WP Efficiency vs #phi;Jet #phi;70 tag WP Efficiency',path='Jet_Eff',xbins=12,xmin=-1*math.pi,xmax=math.pi)
    GeneralGroup.defineHistogram('pass77f,jet_phi',type='TEfficiency',title='Jets 77 tag WP Efficiency vs #phi;Jet #phi;77 tag WP Efficiency',path='Jet_Eff',xbins=12,xmin=-1*math.pi,xmax=math.pi)
    GeneralGroup.defineHistogram('pass85f,jet_phi',type='TEfficiency',title='Jets 85 tag WP Efficiency vs #phi;Jet #phi;85 tag WP Efficiency',path='Jet_Eff',xbins=12,xmin=-1*math.pi,xmax=math.pi)

    #2D TEfficiency for 2D MAP of fraction of good jets
    GeneralGroup.defineHistogram('passGood,jet_eta_all,jet_phi_all',type='TEfficiency',title='TEfficiency in 2D;Jet #eta;Jet #phi;Good Jet Efficiency',path='Jet_Eff',xbins=100,xmin=-2.5,xmax=2.5,ybins=200,ymin=-1*math.pi,ymax=math.pi)


    ### the same but with suspect jets
    GeneralGroup.defineHistogram('sus_jet_MV',title='Suspect Jet MV;Suspect Jet MV;Jets',path='Jet_quality',xbins=100,xmin=-1.0,xmax=1.0)

    GeneralGroup.defineHistogram('sus_jet_MV_pt_0_20',title='Suspect Jet MV in pT bin 1 : pT = [0,20];Suspect Jet MV;Jets',path='Jet_MV',xbins=100,xmin=-1.0,xmax=1.0)
    GeneralGroup.defineHistogram('sus_jet_MV_pt_20_40',title='Suspect Jet MV in pT bin 2 : pT = [20,40];Suspect Jet MV;Jets',path='Jet_MV',xbins=100,xmin=-1.0,xmax=1.0)
    GeneralGroup.defineHistogram('sus_jet_MV_pt_40_80',title='Suspect Jet MV in pT bin 3 : pT = [40,80];Suspect Jet MV;Jets',path='Jet_MV',xbins=100,xmin=-1.0,xmax=1.0)
    GeneralGroup.defineHistogram('sus_jet_MV_pt_80_100',title='Suspect Jet MV in pT bin 4 : pT = [80,100];Suspect Jet MV;Jets',path='Jet_MV',xbins=100,xmin=-1.0,xmax=1.0)
    GeneralGroup.defineHistogram('sus_jet_MV_pt_100_150',title='Suspect Jet MV in pT bin 5 : pT = [100,150];Suspect Jet MV;Jets',path='Jet_MV',xbins=100,xmin=-1.0,xmax=1.0)
    GeneralGroup.defineHistogram('sus_jet_MV_pt_150_200',title='Suspect Jet MV in pT bin 6 : pT = [150,200];Suspect Jet MV;Jets',path='Jet_MV',xbins=100,xmin=-1.0,xmax=1.0)
    GeneralGroup.defineHistogram('sus_jet_MV_pt_200_1000',title='Suspect Jet MV in pT bin 7 : pT = [200,1000];Suspect Jet MV;Jets',path='Jet_MV',xbins=100,xmin=-1.0,xmax=1.0)

    GeneralGroup.defineHistogram('sus_jet_MV_eta_0_05',title='Suspect Jet MV in #eta bin 1 : #eta = [0,0.5];Suspect Jet MV;Jets',path='Jet_MV',xbins=100,xmin=-1.0,xmax=1.0)
    GeneralGroup.defineHistogram('sus_jet_MV_eta_05_10',title='Suspect Jet MV in #eta bin 2 : #eta = [0.5,1.0];Suspect Jet MV;Jets',path='Jet_MV',xbins=100,xmin=-1.0,xmax=1.0)
    GeneralGroup.defineHistogram('sus_jet_MV_eta_10_15',title='Suspect Jet MV in #eta bin 3 : #eta = [1.0,1.5];Suspect Jet MV;Jets',path='Jet_MV',xbins=100,xmin=-1.0,xmax=1.0)
    GeneralGroup.defineHistogram('sus_jet_MV_eta_15_20',title='Suspect Jet MV in #eta bin 4 : #eta = [1.5,2.0];Suspect Jet MV;Jets',path='Jet_MV',xbins=100,xmin=-1.0,xmax=1.0)
    GeneralGroup.defineHistogram('sus_jet_MV_eta_20_25',title='Suspect Jet MV in #eta bin 5 : #eta = [2.0,2.5];Suspect Jet MV;Jets',path='Jet_MV',xbins=100,xmin=-1.0,xmax=1.0)

    GeneralGroup.defineHistogram('sus_jet_MV_phi_0_05',title='Suspect Jet MV in #phi bin 1 : #phi = [0,0.5];Suspect Jet MV;Jets',path='Jet_MV',xbins=100,xmin=-1.0,xmax=1.0)
    GeneralGroup.defineHistogram('sus_jet_MV_phi_05_10',title='Suspect Jet MV in #phi bin 2 : #phi = [0.5,1.0];Suspect Jet MV;Jets',path='Jet_MV',xbins=100,xmin=-1.0,xmax=1.0)
    GeneralGroup.defineHistogram('sus_jet_MV_phi_10_15',title='Suspect Jet MV in #phi bin 3 : #phi = [1.0,1.5];Suspect Jet MV;Jets',path='Jet_MV',xbins=100,xmin=-1.0,xmax=1.0)
    GeneralGroup.defineHistogram('sus_jet_MV_phi_15_20',title='Suspect Jet MV in #phi bin 4 : #phi = [1.5,2.0];Suspect Jet MV;Jets',path='Jet_MV',xbins=100,xmin=-1.0,xmax=1.0)
    GeneralGroup.defineHistogram('sus_jet_MV_phi_20_25',title='Suspect Jet MV in #phi bin 5 : #phi = [2.0,2.5];Suspect Jet MV;Jets',path='Jet_MV',xbins=100,xmin=-1.0,xmax=1.0)
    GeneralGroup.defineHistogram('sus_jet_MV_phi_25_31',title='Suspect Jet MV in #phi bin 6 : #phi = [2.5,3.1];Suspect Jet MV;Jets',path='Jet_MV',xbins=100,xmin=-1.0,xmax=1.0)

    GeneralGroup.defineHistogram('sus_jet_eta_pretag',title='Number of suspect jets before passing tag vs #eta;Suspect Jet #eta;Number of suspect jets',path='Jet_MV',xbins=10,xmin=-2.5,xmax=2.5)
    GeneralGroup.defineHistogram('sus_jet_eta_60tag',title='Number of suspect jets passing 60 tag WP vs #eta;Suspect Jet #eta;Number of suspect jets',path='Jet_MV',xbins=10,xmin=-2.5,xmax=2.5)
    GeneralGroup.defineHistogram('sus_jet_eta_70tag',title='Number of suspect jets passing 70 tag WP vs #eta;Suspect Jet #eta;Number of suspect jets',path='Jet_MV',xbins=10,xmin=-2.5,xmax=2.5)
    GeneralGroup.defineHistogram('sus_jet_eta_77tag',title='Number of suspect jets passing 77 tag WP vs #eta;Suspect Jet #eta;Number of suspect jets',path='Jet_MV',xbins=10,xmin=-2.5,xmax=2.5)
    GeneralGroup.defineHistogram('sus_jet_eta_85tag',title='Number of suspect jets passing 85 tag WP vs #eta;Suspect Jet #eta;Number of suspect jets',path='Jet_MV',xbins=10,xmin=-2.5,xmax=2.5)
    GeneralGroup.defineHistogram('sus_jet_eta_60tag,sus_jet_eta_pretag',type='TEfficiency',title='Jets 60 tag WP Efficiency vs #eta;Suspect Jet #eta;60 tag WP Efficiency',path='Jet_MV',xbins=10,xmin=-2.5,xmax=2.5)
    GeneralGroup.defineHistogram('sus_jet_eta_70tag,sus_jet_eta_pretag',type='TEfficiency',title='Jets 70 tag WP Efficiency vs #eta;Suspect Jet #eta;70 tag WP Efficiency',path='Jet_MV',xbins=10,xmin=-2.5,xmax=2.5)
    GeneralGroup.defineHistogram('sus_jet_eta_77tag,sus_jet_eta_pretag',type='TEfficiency',title='Jets 77 tag WP Efficiency vs #eta;Suspect Jet #eta;77 tag WP Efficiency',path='Jet_MV',xbins=10,xmin=-2.5,xmax=2.5)
    GeneralGroup.defineHistogram('sus_jet_eta_85tag,sus_jet_eta_pretag',type='TEfficiency',title='Jets 85 tag WP Efficiency vs #eta;Suspect Jet #eta;85 tag WP Efficiency',path='Jet_MV',xbins=10,xmin=-2.5,xmax=2.5)

    GeneralGroup.defineHistogram('sus_jet_phi_pretag',title='Number of suspect jets before passing tag vs #phi;Suspect Jet #phi;Number of suspect jets',path='Jet_MV',xbins=12,xmin=-1*math.pi,xmax=math.pi)
    GeneralGroup.defineHistogram('sus_jet_phi_60tag',title='Number of suspect jets passing 60 tag WP vs #phi;Suspect Jet #phi;Number of suspect jets',path='Jet_MV',xbins=12,xmin=-1*math.pi,xmax=math.pi)
    GeneralGroup.defineHistogram('sus_jet_phi_70tag',title='Number of suspect jets passing 70 tag WP vs #phi;Suspect Jet #phi;Number of suspect jets',path='Jet_MV',xbins=12,xmin=-1*math.pi,xmax=math.pi)
    GeneralGroup.defineHistogram('sus_jet_phi_77tag',title='Number of suspect jets passing 77 tag WP vs #phi;Suspect Jet #phi;Number of suspect jets',path='Jet_MV',xbins=12,xmin=-1*math.pi,xmax=math.pi)
    GeneralGroup.defineHistogram('sus_jet_phi_85tag',title='Number of suspect jets passing 85 tag WP vs #phi;Suspect Jet #phi;Number of suspect jets',path='Jet_MV',xbins=12,xmin=-1*math.pi,xmax=math.pi)
    GeneralGroup.defineHistogram('sus_jet_phi_60tag,sus_jet_phi_pretag',type='TEfficiency',title='Jets 60 tag WP Efficiency vs #phi;Suspect Jet #phi;60 tag WP Efficiency',path='Jet_MV',xbins=12,xmin=-1*math.pi,xmax=math.pi)
    GeneralGroup.defineHistogram('sus_jet_phi_70tag,sus_jet_phi_pretag',type='TEfficiency',title='Jets 70 tag WP Efficiency vs #phi;Suspect Jet #phi;70 tag WP Efficiency',path='Jet_MV',xbins=12,xmin=-1*math.pi,xmax=math.pi)
    GeneralGroup.defineHistogram('sus_jet_phi_77tag,sus_jet_phi_pretag',type='TEfficiency',title='Jets 77 tag WP Efficiency vs #phi;Suspect Jet #phi;77 tag WPEfficiency',path='Jet_MV',xbins=12,xmin=-1*math.pi,xmax=math.pi)
    GeneralGroup.defineHistogram('sus_jet_phi_85tag,sus_jet_phi_pretag',type='TEfficiency',title='Jets 85 tag WP Efficiency vs #phi;Suspect Jet #phi;85 tag WP Efficiency',path='Jet_MV',xbins=12,xmin=-1*math.pi,xmax=math.pi)

    GeneralGroup.defineHistogram('sus_jet_eta_60tag,sus_jet_phi_60tag;sus_jet_MAP_60tag',title='2D MAP of jets passing 60 tag WP;Jet #eta;Jet #phi',type='TH2F',path='Jet_MV',xbins=100,xmin=-2.5,xmax=2.5,ybins=200,ymin=-1*math.pi,ymax=math.pi)
    GeneralGroup.defineHistogram('sus_jet_eta_70tag,sus_jet_phi_70tag;sus_jet_MAP_70tag',title='2D MAP of jets passing 70 tag WP;Jet #eta;Jet #phi',type='TH2F',path='Jet_MV',xbins=100,xmin=-2.5,xmax=2.5,ybins=200,ymin=-1*math.pi,ymax=math.pi)
    GeneralGroup.defineHistogram('sus_jet_eta_77tag,sus_jet_phi_77tag;sus_jet_MAP_77tag',title='2D MAP of jets passing 77 tag WP;Jet #eta;Jet #phi',type='TH2F',path='Jet_MV',xbins=100,xmin=-2.5,xmax=2.5,ybins=200,ymin=-1*math.pi,ymax=math.pi)
    GeneralGroup.defineHistogram('sus_jet_eta_85tag,sus_jet_phi_85tag;sus_jet_MAP_85tag',title='2D MAP of jets passing 85 tag WP;Jet #eta;Jet #phi',type='TH2F',path='Jet_MV',xbins=100,xmin=-2.5,xmax=2.5,ybins=200,ymin=-1*math.pi,ymax=math.pi)

    #1D and 2D Tefficiency examples
    #aGroup.defineHistogram('pT_passed,pT',type='TEfficiency',title='Test TEfficiency;x;Eff',path='AndInTheDarkness',xbins=100,xmin=0.0,xmax=50.0)
    #aGroup.defineHistogram('pT_passed,pT,random',type='TEfficiency',title='Test TEfficiency 2D;x;y;Eff',path='AndInTheDarkness',xbins=100,xmin=0.0,xmax=50.0,ybins=10, ymin=0.0, ymax=2.0)


    ### STEP 6 ###
    # Finalize. The return value should be a tuple of the ComponentAccumulator
    # and the sequence containing the created algorithms. If we haven't called
    # any configuration other than the AthMonitorCfgHelper here, then we can 
    # just return directly (and not create "result" above)
    return helper.result()
    
    # # Otherwise, merge with result object and return
    # acc, seq = helper.result()
    # result.merge(acc)
    # return result, seq


if __name__=='__main__':
    # Setup the Run III behavior
    from AthenaCommon.Configurable import Configurable
    Configurable.configurableRun3Behavior = 1

    # Setup logs
    from AthenaCommon.Logging import log
    from AthenaCommon.Constants import INFO
    log.setLevel(INFO)

    # Set the Athena configuration flags
    from AthenaConfiguration.AllConfigFlags import ConfigFlags

    #Select the input (data or MC) and output files

    #Data input working:
    #ConfigFlags.Input.Files = ["/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/CommonInputs/data16_13TeV.00311321.physics_Main.recon.AOD.r9264/AOD.11038520._000001.pool.root.1"] #2157
    #ConfigFlags.Input.Files = ["/afs/cern.ch/work/a/alaperto/dq_test/dq_r22_MAR/run/DQ_test_output/r21_AOD.root"] #22996 events
    #ConfigFlags.Input.isMC = False

    #MC input for DQr22 first tests
    #ConfigFlags.Input.Files = ["/afs/cern.ch/work/e/enagy/public/GenerateAOD/Gen_ST_250919_100/AOD.pool.root"]#tmp.ESD"]
    ConfigFlags.Input.Files = ["/afs/cern.ch/work/a/alaperto/dq_test/dq_r22_MAR/run/DQ_APR_dev/my_new_AOD.root"] #r21 AOD, ttbar, with MANY events
    ConfigFlags.Input.isMC = True

    #Output file
    ConfigFlags.Output.HISTFileName = 'JetTagMonitorOutput.root'
    ConfigFlags.lock()

    # Initialize configuration object, add accumulator, merge, and run.
    from AthenaConfiguration.MainServicesConfig import MainServicesCfg 
    from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg
    cfg = MainServicesCfg(ConfigFlags)
    cfg.merge(PoolReadCfg(ConfigFlags))

    jetTagMonitorAcc = JetTagMonitorConfig(ConfigFlags)
    cfg.merge(jetTagMonitorAcc)
    
    # If you want to turn on more detailed messages ...
    #trigEgammaMonitorAcc.getEventAlgo('TrigEgammaMonAlg').OutputLevel = 2 # DEBUG
    cfg.printConfig(withDetails=True) # set True for exhaustive info

    #Select how many events to run on 
    #use cfg.run() empty for all events
    #use cfg.run(20) to only run on first 20 events
    cfg.run(2000) #22697 for new data, 2157 for data, >20k for ttbar MC
