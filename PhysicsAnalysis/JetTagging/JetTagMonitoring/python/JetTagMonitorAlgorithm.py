#
#  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
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
    from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
    result = ComponentAccumulator()

    # do not run monitoring in RAWtoESD
    if inputFlags.DQ.Environment == 'tier0Raw':
        return result

    # do not run in cosmics
    if inputFlags.DQ.DataType == 'cosmics':
        return result

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
    jetTagMonAlg = helper.addAlgorithm(CompFactory.JetTagMonitorAlgorithm,'JetTagMonAlg')
    #anotherJetTagMonAlg = helper.addAlgorithm(JetTagMonitorAlgorithm,'AnotherJetTagMonAlg')

    # If for some really obscure reason you need to instantiate an algorithm
    # yourself, the AddAlgorithm method will still configure the base 
    # properties and add the algorithm to the monitoring sequence.
    # helper.AddAlgorithm(myExistingAlg)
    
    ### STEP 3 ###
    # Edit properties of an algorithm
    # Declare properties

    #objects collections
    jetTagMonAlg.JetsCollection = "AntiKt4EMTopoJets"
    jetTagMonAlg.MuonsCollection = "Muons"
    jetTagMonAlg.ElectronsCollection = "Electrons"

    #trigger chain
    jetTagMonAlg.TriggerChain = ''
    #jetTagMonAlg.TriggerChain = 'HLT_mu26_ivarmedium'
    #jetTagMonAlg.TriggerChain = 'HLT_e24_lhtight_nod0'

    #skip jet filter selection if ion-ion or proton-ion collisions
    jetTagMonAlg.SkipJetFilter = False

    jetTagMonAlg.DoExtraTaggerHistos = True

    #trigger pre-selection wild-cards
    jetTagMonAlg.ElectronTrigger_201X = "HLT_e[2-9][0-9]_.*" # electrons 20-99 GeV
    jetTagMonAlg.MuonTrigger_201X = "HLT_mu[2-9][0-9].*" # muons 20-99 GeV

    #general pT / eta cuts for jets
    jetTagMonAlg.JetEtaCut = 2.5
    jetTagMonAlg.JetPtCut = 25.0
    
    #soft muon pT cut for SMT jets 
    jetTagMonAlg.SoftMuonPtCut = 5.0

    #track selection for jet quality 
    jetTagMonAlg.MinGoodTrackCut = 1
    jetTagMonAlg.TrackPtCut = 1.0 #GeV
    jetTagMonAlg.TrackEtaCut = 2.5
    jetTagMonAlg.Trackd0Cut = 1.0 #mm
    jetTagMonAlg.Trackz0sinCut = 1.5 #mm
    jetTagMonAlg.TrackChi2ndfCut = 999.0
    jetTagMonAlg.TrackHitIBLCut = 1
    jetTagMonAlg.TrackHitPixCut = 1
    jetTagMonAlg.TrackHolePixCut = 1
    jetTagMonAlg.TrackHitSCTCut = 0
    jetTagMonAlg.TrackHoleSCTCut = 999
    jetTagMonAlg.TrackHitSiCut = 7
    jetTagMonAlg.TrackHoleSiCut = 2
    
    #lepton pT / eta cuts for TTbar events
    jetTagMonAlg.ElectronPtCut = 25.0
    jetTagMonAlg.MuonPtCut = 25.0
    jetTagMonAlg.ElectronEtaCut = 2.47
    jetTagMonAlg.MuonEtaCut = 2.5
    jetTagMonAlg.ElectronEtaCrackLowCut = 1.37
    jetTagMonAlg.ElectronEtaCrackHighCut = 1.52
    
    #isolation cuts corresponding to r21 FixedTightCut working point
    jetTagMonAlg.ElectronTopoEtCone20Cut = 0.06
    jetTagMonAlg.MuonTopoEtCone20Cut = 0.06
    jetTagMonAlg.ElectronPtVarCone20Cut = 0.06
    jetTagMonAlg.MuonPtVarCone30Cut = 0.06

    #JVT cut r21 WP, update eventually to r22
    jetTagMonAlg.JVTCut = 0.59
    jetTagMonAlg.JVTpTCut = 60.0
    jetTagMonAlg.JVTetaCut = 2.4

    if jetTagMonAlg.JetsCollection == "AntiKt4EMPFlowJets" :
        jetTagMonAlg.JVTCut = 0.2
    elif jetTagMonAlg.JetsCollection == "AntiKt4EMTopoJets" :
        jetTagMonAlg.JVTCut = 0.59
    else :
        jetTagMonAlg.JVTCut = 0.59
     
    #Main b-tagging algorithm: MV2c10 (r21) or DL1r (r22)
    #Benchmarks from: https://twiki.cern.ch/twiki/bin/view/AtlasProtected/BTaggingBenchmarksRelease21
    jetTagMonAlg.TaggerName = "DL1r"
    
    if jetTagMonAlg.TaggerName == "MV2c10" :
     jetTagMonAlg.WP60Cut = 0.94
     jetTagMonAlg.WP70Cut = 0.83
     jetTagMonAlg.WP77Cut = 0.64
     jetTagMonAlg.WP85Cut = 0.11
     MV_bins = 100
     MV_start = -1.0
     MV_stop = 1.0
    elif jetTagMonAlg.TaggerName == "DL1r" :
     jetTagMonAlg.WP60Cut = 2.74
     jetTagMonAlg.WP70Cut = 2.02
     jetTagMonAlg.WP77Cut = 1.45
     jetTagMonAlg.WP85Cut = 0.46
     jetTagMonAlg.cFraction = 0.08
     jetTagMonAlg.bFraction = 0.08
     MV_bins = 100
     MV_start = -7.0
     MV_stop = 13.0
    else :
     jetTagMonAlg.WP60Cut = 2.74
     jetTagMonAlg.WP70Cut = 2.02
     jetTagMonAlg.WP77Cut = 1.45
     jetTagMonAlg.WP85Cut = 0.46
     jetTagMonAlg.cFraction = 0.08
     jetTagMonAlg.bFraction = 0.08
     MV_bins = 100
     MV_start = -7.0
     MV_stop = 13.0
    
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
    GeneralGroup = helper.addGroup(jetTagMonAlg,'JetTagMonitor','JetTagging')
    #AnotherGroup = helper.addGroup(anotherJetTagMonAlg,'JetTagMonitor')

    ### STEP 5 ### 
    #title is for plot, x-axis, y-axis
    #type is for TH2F or TEfficiency
    #path is for folder structure of output file 

    #1D and 2D Tefficiency examples
    #aGroup.defineHistogram('pT_passed,pT',type='TEfficiency',title='Test TEfficiency;x;Eff',path='AndInTheDarkness',xbins=100,xmin=0.0,xmax=50.0)
    #aGroup.defineHistogram('pT_passed,pT,random',type='TEfficiency',title='Test TEfficiency 2D;x;y;Eff',path='AndInTheDarkness',xbins=100,xmin=0.0,xmax=50.0,ybins=10, ymin=0.0, ymax=2.0)

    #General histograms: cutflow, run, PV, tracks, hits
    GeneralGroup.defineHistogram('Cutflow_Event',title='Event CutFlow;Pass Event CutFlow;Events',path='Cutflow',xbins=10,xmin=-0.5,xmax=9.5)
    GeneralGroup.defineHistogram('Cutflow_Trigger',title='Trigger CutFlow;Pass Trigger CutFlow;Events',path='Cutflow',xbins=10,xmin=-0.5,xmax=9.5)
    GeneralGroup.defineHistogram('Cutflow_Jet',title='Jet CutFlow;Pass Jet CutFlow;Jets',path='Cutflow',xbins=10,xmin=-0.5,xmax=9.5)

    GeneralGroup.defineHistogram('Run_number',title='Run number;Run;Event',path='Run',xbins=500000,xmin=-0.5,xmax=499999.5)
    GeneralGroup.defineHistogram('Run_event',title='Event number;Event;Event',path='Run',xbins=500000,xmin=-0.5,xmax=499999.5)
    GeneralGroup.defineHistogram('Run_lb',title='Lumi Block;LB;Events',path='Run',xbins=1000,xmin=-0.5,xmax=999.5)
    GeneralGroup.defineHistogram('Run_mu',title='Mu;<mu>;Events',path='Run',xbins=100,xmin=0,xmax=100.0)
    GeneralGroup.defineHistogram('PV_n',title='Primary vertices;PV;Events',path='Run',xbins=100,xmin=0,xmax=100.0)
    GeneralGroup.defineHistogram('PV_x',title='Primary vertices X position;X;Events',path='Run',xbins=40,xmin=-2.0,xmax=2.0)
    GeneralGroup.defineHistogram('PV_y',title='Primary vertices Y position;Y;Events',path='Run',xbins=40,xmin=-2.0,xmax=2.0)
    GeneralGroup.defineHistogram('PV_z',title='Primary vertices Z position;Z;Events',path='Run',xbins=100,xmin=-250,xmax=250.0)
    GeneralGroup.defineHistogram('PV_tracks_n',title='Number of tracks in PV;Tracks in PV;Number of tracks',path='Run',xbins=150,xmin=-0.5,xmax=149.5)

    GeneralGroup.defineHistogram('Tracks_n',title='Track multiplicity;Tracks per event;Number of events',path='Run',xbins=100,xmin=0,xmax=1000)
    GeneralGroup.defineHistogram('Hits_IBL',title='Number of IBL hits;Hits on track;Number of IBL hits on track',path='Run',xbins=5,xmin=0,xmax=5)
    GeneralGroup.defineHistogram('Hits_Pixel',title='Number of Pixel hits;Hits on track;Number of Pixel hits on track',path='Run',xbins=10,xmin=0,xmax=10)
    GeneralGroup.defineHistogram('Hits_SCT',title='Number of SCT hits;Hits on track;Number of SCT hits on track',path='Run',xbins=15,xmin=0,xmax=15)
    GeneralGroup.defineHistogram('Hits_Si',title='Number of PIX+SCT hits;Hits on track;Number of PIX+SCT hits on track',path='Run',xbins=25,xmin=0,xmax=25)
    GeneralGroup.defineHistogram('Hits_TRT',title='Number of TRT hits;Hits on track;Number of TRT hits on track',path='Run',xbins=100,xmin=0,xmax=100)
    GeneralGroup.defineHistogram('Hits_ID',title='Number of ID hits;Hits on track;Number of ID hits on track',path='Run',xbins=150,xmin=0,xmax=150)

    #Jet tracks
    GeneralGroup.defineHistogram('JetTracks_n',title='Track multiplicity in jet;Tracks;Number of tracks per jet',path='JetTracks',xbins=50,xmin=0,xmax=50)
    GeneralGroup.defineHistogram('JetTracks_pT',title='pT of tracks in jet;pT;Tracks in jets',path='JetTracks',xbins=100,xmin=0,xmax=100)
    GeneralGroup.defineHistogram('JetTracks_eta',title='#eta of tracks in jet;#eta;Tracks in jets',path='JetTracks',xbins=100,xmin=-2.5,xmax=2.5)
    GeneralGroup.defineHistogram('JetTracks_phi',title='#phi of tracks in jet;#phi;Tracks in jets',path='JetTracks',xbins=100,xmin=-1*math.pi,xmax=math.pi)
    GeneralGroup.defineHistogram('JetTracks_DR',title='DeltaR of tracks in jet;DeltaR;Tracks in jets',path='JetTracks',xbins=100,xmin=0,xmax=1)
    GeneralGroup.defineHistogram('JetTracks_d0',title='d_0 of tracks in jet;d_0;Tracks in jets',path='JetTracks',xbins=100,xmin=-5,xmax=5)
    GeneralGroup.defineHistogram('JetTracks_d0s',title='sigma d_0 of tracks in jet;sigma d_0;Tracks in jets',path='JetTracks',xbins=50,xmin=0,xmax=5)
    GeneralGroup.defineHistogram('JetTracks_d0si',title='significance d_0 of tracks in jet;significance d_0;Tracks in jets',path='JetTracks',xbins=100,xmin=-5,xmax=5)
    GeneralGroup.defineHistogram('JetTracks_z0',title='z_0 of tracks in jet;z_0;Tracks in jets',path='JetTracks',xbins=100,xmin=-5,xmax=5)
    GeneralGroup.defineHistogram('JetTracks_z0s',title='sigma z_0 of tracks in jet;sigma z_0;Tracks in jets',path='JetTracks',xbins=50,xmin=0,xmax=5)
    GeneralGroup.defineHistogram('JetTracks_z0si',title='significance z_0 of tracks in jet;significance z_0;Tracks in jets',path='JetTracks',xbins=100,xmin=-5,xmax=5)
    GeneralGroup.defineHistogram('JetTracks_Hits_IBL',title='Number of IBL hits on tracks in jets;Hits on track;Number of IBL hits on track',path='JetTracks',xbins=5,xmin=0,xmax=5)
    GeneralGroup.defineHistogram('JetTracks_Hits_Pixel',title='Number of Pixel hits on tracks in jets;Hits on track;Number of Pixel hits on track',path='JetTracks',xbins=10,xmin=0,xmax=10)
    GeneralGroup.defineHistogram('JetTracks_Hits_SCT',title='Number of SCT hits on tracks in jets;Hits on track;Number of SCT hits on track',path='JetTracks',xbins=15,xmin=0,xmax=15)
    GeneralGroup.defineHistogram('JetTracks_Hits_TRT',title='Number of TRT hits on tracks in jets;Hits on track;Number of TRT hits on track',path='JetTracks',xbins=100,xmin=0,xmax=100)
    GeneralGroup.defineHistogram('JetTracks_nSV',title='Number of SVs;Number of SVs;Jets',path='JetTracks',xbins=3,xmin=0.0,xmax=3.0)

    #Jet quality selection (good, suspect, bad) based on its tracks
    GeneralGroup.defineHistogram('SelTracks_n_all',title='Jet track multiplicity;Tracks;Number of tracks per jet',path='TracksInJetSelection',xbins=50,xmin=0,xmax=50)
    GeneralGroup.defineHistogram('SelTracks_n_pass',title='Jet track multiplicity (passed selection);Tracks;Number of tracks per jet',path='TracksInJetSelection',xbins=50,xmin=0,xmax=50)
    GeneralGroup.defineHistogram('SelTracks_pT_all',title='pT of jet tracks;pT [GeV];Jet tracks',path='TracksInJetSelection',xbins=60,xmin=0,xmax=30)
    GeneralGroup.defineHistogram('SelTracks_eta_all',title='#eta of jet tracks;#eta;Jet tracks',path='TracksInJetSelection',xbins=100,xmin=-2.5,xmax=2.5)
    GeneralGroup.defineHistogram('SelTracks_phi_all',title='#phi of jet tracks;#phi;Jet tracks',path='TracksInJetSelection',xbins=100,xmin=-1*math.pi,xmax=math.pi)
    GeneralGroup.defineHistogram('SelTracks_chi2ndof',title='Jet tracks fit #chi^{2}/ndf;#chi^{2}/ndf;Jet tracks',path='TracksInJetSelection',xbins=50,xmin=0,xmax=50)
    GeneralGroup.defineHistogram('SelTracks_d0',title='d0 of jet tracks;d0;Jet tracks',path='TracksInJetSelection',xbins=100,xmin=-5,xmax=5)
    GeneralGroup.defineHistogram('SelTracks_z0sin',title='z0 #times sin (#theta) of jet tracks;z0 #times sin(#theta);Jet tracks',path='TracksInJetSelection',xbins=100,xmin=-5,xmax=5)
    GeneralGroup.defineHistogram('SelTracks_HitIBL',title='Number of IBL hits on tracks in jets;Hits on track;Number of IBL hits on track',path='TracksInJetSelection',xbins=5,xmin=0,xmax=5)
    GeneralGroup.defineHistogram('SelTracks_HitPix',title='Number of Pixel hits on tracks in jets;Hits on track;Number of Pixel hits on track',path='TracksInJetSelection',xbins=10,xmin=0,xmax=10)
    GeneralGroup.defineHistogram('SelTracks_HitSCT',title='Number of SCT hits on tracks in jets;Hits on track;Number of SCT hits on track',path='TracksInJetSelection',xbins=15,xmin=0,xmax=15)
    GeneralGroup.defineHistogram('SelTracks_HitSi',title='Number of Si (Pix+SCT) hits on tracks in jets;Hits on track;Number of Si (Pix+SCT) hits on track',path='TracksInJetSelection',xbins=25,xmin=0,xmax=25)
    GeneralGroup.defineHistogram('SelTracks_HolePix',title='Number of Pixel holes on tracks in jets;Holes on track;Number of Pixel holes on track',path='TracksInJetSelection',xbins=5,xmin=0,xmax=5)
    GeneralGroup.defineHistogram('SelTracks_HoleSCT',title='Number of SCT holes on tracks in jets;Holes on track;Number of SCT holes on track',path='TracksInJetSelection',xbins=5,xmin=0,xmax=5)
    GeneralGroup.defineHistogram('SelTracks_HoleSi',title='Number of Si (Pix+SCT) holes on tracks in jets;Holes on track;Number of Si (Pix+SCT) holes on track',path='TracksInJetSelection',xbins=5,xmin=0,xmax=5)

    #2D Map of selection of tracks in jets
    GeneralGroup.defineHistogram('SelTracks_eta_all,SelTracks_phi_all;SelTracks_MAP_0_all',title='2D MAP of all tracks from jets;Track #eta;Track #phi',type='TH2F',path='TracksInJetSelection',xbins=100,xmin=-2.5,xmax=2.5,ybins=200,ymin=-1*math.pi,ymax=math.pi)
    GeneralGroup.defineHistogram('SelTracks_eta_pT,SelTracks_phi_pT;SelTracks_MAP_1_pT',title='2D MAP of selected tracks (pT and eta) from jets;Track #eta;Track #phi',type='TH2F',path='TracksInJetSelection',xbins=100,xmin=-2.5,xmax=2.5,ybins=200,ymin=-1*math.pi,ymax=math.pi)
    GeneralGroup.defineHistogram('SelTracks_eta_d0,SelTracks_phi_d0;SelTracks_MAP_2_d0',title='2D MAP of selected tracks (d0) from jets;Track #eta;Track #phi',type='TH2F',path='TracksInJetSelection',xbins=100,xmin=-2.5,xmax=2.5,ybins=200,ymin=-1*math.pi,ymax=math.pi)
    GeneralGroup.defineHistogram('SelTracks_eta_z0sin,SelTracks_phi_z0sin;SelTracks_MAP_3_z0sin',title='2D MAP of selected tracks (z0sin#theta) from jets;Track #eta;Track #phi',type='TH2F',path='TracksInJetSelection',xbins=100,xmin=-2.5,xmax=2.5,ybins=200,ymin=-1*math.pi,ymax=math.pi)
    GeneralGroup.defineHistogram('SelTracks_eta_fit,SelTracks_phi_fit;SelTracks_MAP_4_fit',title='2D MAP of selected tracks (fit #chi^{2}/ndf) from jets;Track #eta;Track #phi',type='TH2F',path='TracksInJetSelection',xbins=100,xmin=-2.5,xmax=2.5,ybins=200,ymin=-1*math.pi,ymax=math.pi)
    GeneralGroup.defineHistogram('SelTracks_eta_IBL,SelTracks_phi_IBL;SelTracks_MAP_5_IBL',title='2D MAP of selected tracks (IBL hits) from jets;Track #eta;Track #phi',type='TH2F',path='TracksInJetSelection',xbins=100,xmin=-2.5,xmax=2.5,ybins=200,ymin=-1*math.pi,ymax=math.pi)
    GeneralGroup.defineHistogram('SelTracks_eta_Pix,SelTracks_phi_Pix;SelTracks_MAP_6_Pix',title='2D MAP of selected tracks (Pixel hits and holes) from jets;Track #eta;Track #phi',type='TH2F',path='TracksInJetSelection',xbins=100,xmin=-2.5,xmax=2.5,ybins=200,ymin=-1*math.pi,ymax=math.pi)
    GeneralGroup.defineHistogram('SelTracks_eta_SCT,SelTracks_phi_SCT;SelTracks_MAP_7_SCT',title='2D MAP of selected tracks (SCT hits and holes) from jets;Track #eta;Track #phi',type='TH2F',path='TracksInJetSelection',xbins=100,xmin=-2.5,xmax=2.5,ybins=200,ymin=-1*math.pi,ymax=math.pi)
    GeneralGroup.defineHistogram('SelTracks_eta_Si,SelTracks_phi_Si;SelTracks_MAP_8_Si',title='2D MAP of selected tracks (Pix+SCT hits and holes) from jets;Track #eta;Track #phi',type='TH2F',path='TracksInJetSelection',xbins=100,xmin=-2.5,xmax=2.5,ybins=200,ymin=-1*math.pi,ymax=math.pi)
    GeneralGroup.defineHistogram('SelTracks_eta_pass,SelTracks_phi_pass;SelTracks_MAP_9_pass',title='2D MAP of tracks from jets that passed selection;Track #eta;Track #phi',type='TH2F',path='TracksInJetSelection',xbins=100,xmin=-2.5,xmax=2.5,ybins=200,ymin=-1*math.pi,ymax=math.pi)

    #Jets from ttbar events histograms
    GeneralGroup.defineHistogram('Muon_pT',title='Muon pT;Muon pT;Events',path='TTbarEventSelection',xbins=100,xmin=0.0,xmax=200000.0)
    GeneralGroup.defineHistogram('Electron_pT',title='Electron pT;Electron pT;Events',path='TTbarEventSelection',xbins=100,xmin=0.0,xmax=200000.0)
    
    GeneralGroup.defineHistogram('IsolatedMuons_n',title='Number of isolated Muons;Isolated muons;Number of events',path='TTbarEventSelection',xbins=3,xmin=-0.5,xmax=2.5)
    GeneralGroup.defineHistogram('IsolatedElectrons_n',title='Number of isolated Electrons;Isolated electrons;Number of events',path='TTbarEventSelection',xbins=3,xmin=-0.5,xmax=2.5)

    GeneralGroup.defineHistogram('nTTbarGoodJets',title='Number of good jets in ttbar events;Good jets per event;Number of events',path='TTbarJets',xbins=10,xmin=-0.5,xmax=9.5)
    GeneralGroup.defineHistogram('TTbarJets_MV',title='MV of jets in ttbar events;Jet MV;Number of jets',path='TTbarJets',xbins=int(MV_bins/2),xmin=MV_start,xmax=MV_stop)

    GeneralGroup.defineHistogram('TTbarJets_n',title='Total number of ttbar jets;Number of jets;Number of events',path='TTbarJets',xbins=1,xmin=-0.5,xmax=0.5)
    GeneralGroup.defineHistogram('TTbarJets_n_60tag',title='Number of ttbar jets passing 60 tag WP; Jets passing 60 tag WP;Number of ttbar jets',path='TTbarJets',xbins=1,xmin=-0.5,xmax=0.5)
    GeneralGroup.defineHistogram('TTbarJets_n_70tag',title='Number of ttbar jets passing 70 tag WP; Jets passing 70 tag WP;Number of ttbar jets',path='TTbarJets',xbins=1,xmin=-0.5,xmax=0.5)
    GeneralGroup.defineHistogram('TTbarJets_n_77tag',title='Number of ttbar jets passing 77 tag WP; Jets passing 77 tag WP;Number of ttbar jets',path='TTbarJets',xbins=1,xmin=-0.5,xmax=0.5)
    GeneralGroup.defineHistogram('TTbarJets_n_85tag',title='Number of ttbar jets passing 85 tag WP; Jets passing 60 tag WP;Number of ttbar jets',path='TTbarJets',xbins=1,xmin=-0.5,xmax=0.5)
    GeneralGroup.defineHistogram('pass60n,TTbarJets_n',type='TEfficiency',title='TTbar jets 60 tag WP Efficiency;ttbar jets;60 tag WP Efficiency',path='JetEff',xbins=1,xmin=-0.5,xmax=0.5)
    GeneralGroup.defineHistogram('pass70n,TTbarJets_n',type='TEfficiency',title='TTbar jets 70 tag WP Efficiency;ttbar jets;70 tag WP Efficiency',path='JetEff',xbins=1,xmin=-0.5,xmax=0.5)
    GeneralGroup.defineHistogram('pass77n,TTbarJets_n',type='TEfficiency',title='TTbar jets 77 tag WP Efficiency;ttbar jets;77 tag WP Efficiency',path='JetEff',xbins=1,xmin=-0.5,xmax=0.5)
    GeneralGroup.defineHistogram('pass85n,TTbarJets_n',type='TEfficiency',title='TTbar jets 85 tag WP Efficiency;ttbar jets;85 tag WP Efficiency',path='JetEff',xbins=1,xmin=-0.5,xmax=0.5)

    GeneralGroup.defineHistogram('TTbarJets_pT',title='Number of ttbar jets vs pT;ttbar jet pT;Number of ttbar jets',path='TTbar',xbins=40,xmin=0,xmax=200)
    GeneralGroup.defineHistogram('TTbarJets_pT_60tag',title='Number of ttbar jets passing 60 tag WP vs pT;ttbar jet pT;Number of ttbar jets',path='TTbarJets',xbins=40,xmin=0,xmax=200)
    GeneralGroup.defineHistogram('TTbarJets_pT_70tag',title='Number of ttbar jets passing 70 tag WP vs pT;ttbar jet pT;Number of ttbar jets',path='TTbarJets',xbins=40,xmin=0,xmax=200)
    GeneralGroup.defineHistogram('TTbarJets_pT_77tag',title='Number of ttbar jets passing 77 tag WP vs pT;ttbar jet pT;Number of ttbar jets',path='TTbarJets',xbins=40,xmin=0,xmax=200)
    GeneralGroup.defineHistogram('TTbarJets_pT_85tag',title='Number of ttbar jets passing 85 tag WP vs pT;ttbar jet pT;Number of ttbar jets',path='TTbarJets',xbins=40,xmin=0,xmax=200)
    GeneralGroup.defineHistogram('pass60p,TTbarJets_pT',type='TEfficiency',title='TTbar jets 60 tag WP Efficiency vs pT;ttbar jet pT;60 tag WP Efficiency',path='JetEff',xbins=40,xmin=0.0,xmax=200.0)
    GeneralGroup.defineHistogram('pass70p,TTbarJets_pT',type='TEfficiency',title='TTbar jets 70 tag WP Efficiency vs pT;ttbar jet pT;70 tag WP Efficiency',path='JetEff',xbins=40,xmin=0.0,xmax=200.0)
    GeneralGroup.defineHistogram('pass77p,TTbarJets_pT',type='TEfficiency',title='TTbar jets 77 tag WP Efficiency vs pT;ttbar jet pT;77 tag WP Efficiency',path='JetEff',xbins=40,xmin=0.0,xmax=200.0)
    GeneralGroup.defineHistogram('pass85p,TTbarJets_pT',type='TEfficiency',title='TTbar jets 85 tag WP Efficiency vs pT;ttbar jet pT;85 tag WP Efficiency',path='JetEff',xbins=40,xmin=0.0,xmax=200.0)

    # SMT jets histograms
    GeneralGroup.defineHistogram('SoftMuons_n',title='Number of Soft Muons;Muons per event;Number of muons',path='SMTJetSelection',xbins=3,xmin=-0.5,xmax=2.5)
    GeneralGroup.defineHistogram('SMTJets_n',title='Number of SMT Jets;Jets per event;Number of jets',path='SMTJetSelection',xbins=3,xmin=-0.5,xmax=2.5)

    GeneralGroup.defineHistogram('SMTJets_dr',title='DeltaR between jet and soft muon;DR;Number of SMT candidates',path='SMTJetSelection',xbins=40,xmin=0,xmax=0.4)
    GeneralGroup.defineHistogram('SoftMuons_pT',title='pT of SMT Muons;Muon pT [GeV];Number of muons',path='SMTJetSelection',xbins=50,xmin=0,xmax=100)
    GeneralGroup.defineHistogram('SMTJets_pT',title='pT of SMT Jets;Jet pT [GeV];Number of jets',path='SMTJetSelection',xbins=100,xmin=0,xmax=300)
    GeneralGroup.defineHistogram('SMTJets_pTratio',title='pT ratio of SMT Jets (mu/jet);pT ratio;Number of SMT jets',path='SMTJetSelection',xbins=50,xmin=0,xmax=1)
    GeneralGroup.defineHistogram('SMTJets_pTrel',title='pTrel of SMT Jets;pTrel;Number of jets',path='SMTJetSelection',xbins=50,xmin=0,xmax=5)
    GeneralGroup.defineHistogram('SMTJets_MV',title='MV of SMT Jets;MV;Number of jets',path='SMTJetSelection',xbins=50,xmin=MV_start,xmax=MV_stop)

    #Distributions before jet selection (all jets from selected events)
    GeneralGroup.defineHistogram('jet_n_all',title='Jet multiplicity (before selection);Number of jets;Events',path='JetSelection',xbins=50,xmin=0,xmax=50)
    GeneralGroup.defineHistogram('jet_pT_all',title='Jet pT (before selection);Jet pT [GeV];Jets',path='JetSelection',xbins=100,xmin=0.0,xmax=200.0)
    GeneralGroup.defineHistogram('jet_eta_all',title='Jet eta (before selection);Jet #eta;Jets',path='JetSelection',xbins=100,xmin=-2.5,xmax=2.5)
    GeneralGroup.defineHistogram('jet_phi_all',title='Jet phi (before selection);Jet #phi;Jets',path='JetSelection',xbins=100,xmin=-1*math.pi,xmax=math.pi)
    GeneralGroup.defineHistogram('jet_MV_all',title='Jet MV (before selection);Jet MV;Jets',path='JetSelection',xbins=MV_bins,xmin=MV_start,xmax=MV_stop)

    #2D maps during jet selection
    GeneralGroup.defineHistogram('jet_eta_all,jet_phi_all;jet_MAP_0_all',title='2D MAP of all jets;Jet #eta;Jet #phi',type='TH2F',path='JetSelection',xbins=100,xmin=-2.5,xmax=2.5,ybins=200,ymin=-1*math.pi,ymax=math.pi)
    GeneralGroup.defineHistogram('jet_eta_filter,jet_phi_filter;jet_MAP_1_filter',title='2D MAP of jets passing basic filter;Jet #eta;Jet #phi',type='TH2F',path='JetSelection',xbins=100,xmin=-2.5,xmax=2.5,ybins=200,ymin=-1*math.pi,ymax=math.pi)
    GeneralGroup.defineHistogram('jet_eta_kin,jet_phi_kin;jet_MAP_2_kin',title='2D MAP of jets passing kinematic selection;Jet #eta;Jet #phi',type='TH2F',path='JetSelection',xbins=100,xmin=-2.5,xmax=2.5,ybins=200,ymin=-1*math.pi,ymax=math.pi)
    GeneralGroup.defineHistogram('jet_eta_jvt,jet_phi_jvt;jet_MAP_3_jvt',title='2D MAP of jets passing JVT;Jet #eta;Jet #phi',type='TH2F',path='JetSelection',xbins=100,xmin=-2.5,xmax=2.5,ybins=200,ymin=-1*math.pi,ymax=math.pi)
    GeneralGroup.defineHistogram('jet_eta_overlap,jet_phi_overlap;jet_MAP_4_overlap',title='2D MAP of not overlapping jets;Jet #eta;Jet #phi',type='TH2F',path='JetSelection',xbins=100,xmin=-2.5,xmax=2.5,ybins=200,ymin=-1*math.pi,ymax=math.pi)
    GeneralGroup.defineHistogram('jet_eta_good,jet_phi_good;jet_MAP_5_good',title='2D MAP of good jets;Jet #eta;Jet #phi',type='TH2F',path='JetSelection',xbins=100,xmin=-2.5,xmax=2.5,ybins=200,ymin=-1*math.pi,ymax=math.pi)
    GeneralGroup.defineHistogram('jet_eta_suspect,jet_phi_suspect;jet_MAP_6_suspect',title='2D MAP of suspect jets;Jet #eta;Jet #phi',type='TH2F',path='JetSelection',xbins=100,xmin=-2.5,xmax=2.5,ybins=200,ymin=-1*math.pi,ymax=math.pi)
    GeneralGroup.defineHistogram('jet_eta_bad,jet_phi_bad;jet_MAP_7_bad',title='2D MAP of bad jets;Jet #eta;Jet #phi',type='TH2F',path='JetSelection',xbins=100,xmin=-2.5,xmax=2.5,ybins=200,ymin=-1*math.pi,ymax=math.pi)
    GeneralGroup.defineHistogram('jet_eta_smt,jet_phi_smt;jet_MAP_X_smt',title='2D MAP of SMT jets;Jet #eta;Jet #phi',type='TH2F',path='JetSelection',xbins=100,xmin=-2.5,xmax=2.5,ybins=200,ymin=-1*math.pi,ymax=math.pi)

    GeneralGroup.defineHistogram('jet_MV_beforeJVTCut',title='Jet MV before JVT cut;Jet MV;Jets',path='JetSelection',xbins=MV_bins,xmin=MV_start,xmax=MV_stop)
    GeneralGroup.defineHistogram('jet_MV_afterJVTCut',title='Jet MV after JVT cut;Jet MV;Jets',path='JetSelection',xbins=MV_bins,xmin=MV_start,xmax=MV_stop)
    
    #Distributions for good jets (passing Good/Suspect/Bad selection)
    GeneralGroup.defineHistogram('jet_pT_good',title='Good jets pT;Good Jet pT [GeV];Jets',path='JetGood',xbins=100,xmin=0.0,xmax=200.0)
    GeneralGroup.defineHistogram('jet_eta_good',title='Good jets #eta;Good Jet #eta;Jets',path='JetGood',xbins=100,xmin=-2.5,xmax=2.5)
    GeneralGroup.defineHistogram('jet_phi_good',title='Good jets #phi;Good Jet #phi;Jets',path='JetGood',xbins=100,xmin=-1*math.pi,xmax=math.pi)
    GeneralGroup.defineHistogram('jet_MV_good',title='Good jets MV;Good Jet MV;Jets',path='JetGood',xbins=MV_bins,xmin=MV_start,xmax=MV_stop)
    GeneralGroup.defineHistogram('jet_MV_pu_good',title='Good jets MV pu;Good Jet MV pu;Jets',path='JetGood',xbins=100,xmin=0,xmax=1.0)
    GeneralGroup.defineHistogram('jet_MV_pc_good',title='Good jets MV pc;Good Jet MV pc;Jets',path='JetGood',xbins=100,xmin=0,xmax=1.0)
    GeneralGroup.defineHistogram('jet_MV_pb_good',title='Good jets MV pb;Good Jet MV pb;Jets',path='JetGood',xbins=100,xmin=0,xmax=1.0)

    #Extra Taggers plots
    GeneralGroup.defineHistogram('jet_IP2D_good',title='Jet IP2D;Jet IP2D;Jets',path='JetTaggerExtra',xbins=80,xmin=-40.0,xmax=40.0)
    GeneralGroup.defineHistogram('jet_IP3D_good',title='Jet IP3D;Jet IP3D;Jets',path='JetTaggerExtra',xbins=80,xmin=-40.0,xmax=40.0)
    GeneralGroup.defineHistogram('jet_SV1_good',title='Jet SV1;Jet SV1;Jets',path='JetTaggerExtra',xbins=80,xmin=-40.0,xmax=40.0)
    GeneralGroup.defineHistogram('jet_JetFitter_good',title='Jet JetFitter;Jet JetFitter;Jets',path='JetTaggerExtra',xbins=80,xmin=-40.0,xmax=40.0)
    GeneralGroup.defineHistogram('jet_RNNIP_good',title='Jet RNNIP;Jet RNNIP;Jets',path='JetTaggerExtra',xbins=80,xmin=-40.0,xmax=40.0)
    GeneralGroup.defineHistogram('jet_IP3D_nTrack_good',title='Number of IP3D tracks;IP3D tracks per jet;Jets',path='JetTaggerExtra',xbins=30,xmin=0.0,xmax=30.0)
    GeneralGroup.defineHistogram('jet_SV1_nTrack_good',title='Number of SV tracks;Tracks per SV;SV',path='JetTaggerExtra',xbins=20,xmin=0.0,xmax=20.0)

    #Good jet: tag weight histograms in bins: mu, pt, eta, phi
    GeneralGroup.defineHistogram('jet_MV_mu_0_30',title='Jet MV in mu bin 1 : <mu> = [0,30];Jet MV;Jets',path='JetTagger',xbins=MV_bins,xmin=MV_start,xmax=MV_stop)
    GeneralGroup.defineHistogram('jet_MV_mu_30_50',title='Jet MV in mu bin 2 : <mu> = [30,50];Jet MV;Jets',path='JetTagger',xbins=MV_bins,xmin=MV_start,xmax=MV_stop)
    GeneralGroup.defineHistogram('jet_MV_mu_50_100',title='Jet MV in mu bin 3 : <mu> = [50,100];Jet MV;Jets',path='JetTagger',xbins=MV_bins,xmin=MV_start,xmax=MV_stop)

    GeneralGroup.defineHistogram('jet_MV_pt_0_20',title='Jet MV in pT bin 1 : pT = [0,20];Jet MV;Jets',path='JetTagger',xbins=MV_bins,xmin=MV_start,xmax=MV_stop)
    GeneralGroup.defineHistogram('jet_MV_pt_20_40',title='Jet MV in pT bin 2 : pT = [20,40];Jet MV;Jets',path='JetTagger',xbins=MV_bins,xmin=MV_start,xmax=MV_stop)
    GeneralGroup.defineHistogram('jet_MV_pt_40_70',title='Jet MV in pT bin 3 : pT = [40,70];Jet MV;Jets',path='JetTagger',xbins=MV_bins,xmin=MV_start,xmax=MV_stop)
    GeneralGroup.defineHistogram('jet_MV_pt_70_100',title='Jet MV in pT bin 4 : pT = [70,100];Jet MV;Jets',path='JetTagger',xbins=MV_bins,xmin=MV_start,xmax=MV_stop)
    GeneralGroup.defineHistogram('jet_MV_pt_100_150',title='Jet MV in pT bin 5 : pT = [100,150];Jet MV;Jets',path='JetTagger',xbins=MV_bins,xmin=MV_start,xmax=MV_stop)
    GeneralGroup.defineHistogram('jet_MV_pt_150_200',title='Jet MV in pT bin 6 : pT = [150,200];Jet MV;Jets',path='JetTagger',xbins=MV_bins,xmin=MV_start,xmax=MV_stop)
    GeneralGroup.defineHistogram('jet_MV_pt_200_1000',title='Jet MV in pT bin 7 : pT = [200,1000];Jet MV;Jets',path='JetTagger',xbins=MV_bins,xmin=MV_start,xmax=MV_stop)

    GeneralGroup.defineHistogram('jet_MV_eta_00_05',title='Jet MV in #eta bin 1 : #eta = [0.0,0.5];Jet MV;Jets',path='JetTagger',xbins=MV_bins,xmin=MV_start,xmax=MV_stop)
    GeneralGroup.defineHistogram('jet_MV_eta_05_10',title='Jet MV in #eta bin 2 : #eta = [0.5,1.0];Jet MV;Jets',path='JetTagger',xbins=MV_bins,xmin=MV_start,xmax=MV_stop)
    GeneralGroup.defineHistogram('jet_MV_eta_10_15',title='Jet MV in #eta bin 3 : #eta = [1.0,1.5];Jet MV;Jets',path='JetTagger',xbins=MV_bins,xmin=MV_start,xmax=MV_stop)
    GeneralGroup.defineHistogram('jet_MV_eta_15_20',title='Jet MV in #eta bin 4 : #eta = [1.5,2.0];Jet MV;Jets',path='JetTagger',xbins=MV_bins,xmin=MV_start,xmax=MV_stop)
    GeneralGroup.defineHistogram('jet_MV_eta_20_25',title='Jet MV in #eta bin 5 : #eta = [2.0,2.5];Jet MV;Jets',path='JetTagger',xbins=MV_bins,xmin=MV_start,xmax=MV_stop)

    GeneralGroup.defineHistogram('jet_MV_phi_00_05',title='Jet MV in #phi bin 1 : #phi = [0.0,0.5];Jet MV;Jets',path='JetTagger',xbins=MV_bins,xmin=MV_start,xmax=MV_stop)
    GeneralGroup.defineHistogram('jet_MV_phi_05_10',title='Jet MV in #phi bin 2 : #phi = [0.5,1.0];Jet MV;Jets',path='JetTagger',xbins=MV_bins,xmin=MV_start,xmax=MV_stop)
    GeneralGroup.defineHistogram('jet_MV_phi_10_15',title='Jet MV in #phi bin 3 : #phi = [1.0,1.5];Jet MV;Jets',path='JetTagger',xbins=MV_bins,xmin=MV_start,xmax=MV_stop)
    GeneralGroup.defineHistogram('jet_MV_phi_15_20',title='Jet MV in #phi bin 4 : #phi = [1.5,2.0];Jet MV;Jets',path='JetTagger',xbins=MV_bins,xmin=MV_start,xmax=MV_stop)
    GeneralGroup.defineHistogram('jet_MV_phi_20_25',title='Jet MV in #phi bin 5 : #phi = [2.0,2.5];Jet MV;Jets',path='JetTagger',xbins=MV_bins,xmin=MV_start,xmax=MV_stop)
    GeneralGroup.defineHistogram('jet_MV_phi_25_31',title='Jet MV in #phi bin 6 : #phi = [2.5,3.1];Jet MV;Jets',path='JetTagger',xbins=MV_bins,xmin=MV_start,xmax=MV_stop)

    #Good jet: eta and phi distributions of jets passing WPs and efficiency
    GeneralGroup.defineHistogram('jet_eta',title='Number of jets before passing tag vs #eta;Jet #eta;Number of jets',path='JetEtaPhi',xbins=10,xmin=-2.5,xmax=2.5)
    GeneralGroup.defineHistogram('jet_eta_60tag',title='Number of jets passing 60 tag WP vs #eta;Jet #eta;Number of jets',path='JetEtaPhi',xbins=10,xmin=-2.5,xmax=2.5)
    GeneralGroup.defineHistogram('jet_eta_70tag',title='Number of jets passing 70 tag WP vs #eta;Jet #eta;Number of jets',path='JetEtaPhi',xbins=10,xmin=-2.5,xmax=2.5)
    GeneralGroup.defineHistogram('jet_eta_77tag',title='Number of jets passing 77 tag WP vs #eta;Jet #eta;Number of jets',path='JetEtaPhi',xbins=10,xmin=-2.5,xmax=2.5)
    GeneralGroup.defineHistogram('jet_eta_85tag',title='Number of jets passing 85 tag WP vs #eta;Jet #eta;Number of jets',path='JetEtaPhi',xbins=10,xmin=-2.5,xmax=2.5)
    GeneralGroup.defineHistogram('pass60e,jet_eta',type='TEfficiency',title='Jets 60 tag WP Efficiency vs #eta;Jet #eta;60 tag WP Efficiency',path='JetEtaPhi',xbins=10,xmin=-2.5,xmax=2.5)
    GeneralGroup.defineHistogram('pass70e,jet_eta',type='TEfficiency',title='Jets 70 tag WP Efficiency vs #eta;Jet #eta;70 tag WP Efficiency',path='JetEtaPhi',xbins=10,xmin=-2.5,xmax=2.5)
    GeneralGroup.defineHistogram('pass77e,jet_eta',type='TEfficiency',title='Jets 77 tag WP Efficiency vs #eta;Jet #eta;77 tag WP Efficiency',path='JetEtaPhi',xbins=10,xmin=-2.5,xmax=2.5)
    GeneralGroup.defineHistogram('pass85e,jet_eta',type='TEfficiency',title='Jets 85 tag WP Efficiency vs #eta;Jet #eta;85 tag WP Efficiency',path='JetEtaPhi',xbins=10,xmin=-2.5,xmax=2.5)

    GeneralGroup.defineHistogram('jet_phi',title='Number of jets before passing tag vs #phi;Jet #phi;Number of jets',path='JetEtaPhi',xbins=12,xmin=-1*math.pi,xmax=math.pi)
    GeneralGroup.defineHistogram('jet_phi_60tag',title='Number of jets passing 60 tag WP vs #phi;Jet #phi;Number of jets',path='JetEtaPhi',xbins=12,xmin=-1*math.pi,xmax=math.pi)
    GeneralGroup.defineHistogram('jet_phi_70tag',title='Number of jets passing 70 tag WP vs #phi;Jet #phi;Number of jets',path='JetEtaPhi',xbins=12,xmin=-1*math.pi,xmax=math.pi)
    GeneralGroup.defineHistogram('jet_phi_77tag',title='Number of jets passing 77 tag WP vs #phi;Jet #phi;Number of jets',path='JetEtaPhi',xbins=12,xmin=-1*math.pi,xmax=math.pi)
    GeneralGroup.defineHistogram('jet_phi_85tag',title='Number of jets passing 85 tag WP vs #phi;Jet #phi;Number of jets',path='JetEtaPhi',xbins=12,xmin=-1*math.pi,xmax=math.pi)
    GeneralGroup.defineHistogram('pass60f,jet_phi',type='TEfficiency',title='Jets 60 tag WP Efficiency vs #phi;Jet #phi;60 tag WP Efficiency',path='JetEtaPhi',xbins=12,xmin=-1*math.pi,xmax=math.pi)
    GeneralGroup.defineHistogram('pass70f,jet_phi',type='TEfficiency',title='Jets 70 tag WP Efficiency vs #phi;Jet #phi;70 tag WP Efficiency',path='JetEtaPhi',xbins=12,xmin=-1*math.pi,xmax=math.pi)
    GeneralGroup.defineHistogram('pass77f,jet_phi',type='TEfficiency',title='Jets 77 tag WP Efficiency vs #phi;Jet #phi;77 tag WP Efficiency',path='JetEtaPhi',xbins=12,xmin=-1*math.pi,xmax=math.pi)
    GeneralGroup.defineHistogram('pass85f,jet_phi',type='TEfficiency',title='Jets 85 tag WP Efficiency vs #phi;Jet #phi;85 tag WP Efficiency',path='JetEtaPhi',xbins=12,xmin=-1*math.pi,xmax=math.pi)

    #Good jet: 2D MAP (eta/phi) and 2D TEfficiency of fraction of good jets
    GeneralGroup.defineHistogram('jet_eta_60tag,jet_phi_60tag;jet_MAP_60tag',title='2D MAP of jets passing 60 tag WP;Jet #eta;Jet #phi',type='TH2F',path='JetGood',xbins=100,xmin=-2.5,xmax=2.5,ybins=200,ymin=-1*math.pi,ymax=math.pi)
    GeneralGroup.defineHistogram('jet_eta_70tag,jet_phi_70tag;jet_MAP_70tag',title='2D MAP of jets passing 70 tag WP;Jet #eta;Jet #phi',type='TH2F',path='JetGood',xbins=100,xmin=-2.5,xmax=2.5,ybins=200,ymin=-1*math.pi,ymax=math.pi)
    GeneralGroup.defineHistogram('jet_eta_77tag,jet_phi_77tag;jet_MAP_77tag',title='2D MAP of jets passing 77 tag WP;Jet #eta;Jet #phi',type='TH2F',path='JetGood',xbins=100,xmin=-2.5,xmax=2.5,ybins=200,ymin=-1*math.pi,ymax=math.pi)
    GeneralGroup.defineHistogram('jet_eta_85tag,jet_phi_85tag;jet_MAP_85tag',title='2D MAP of jets passing 85 tag WP;Jet #eta;Jet #phi',type='TH2F',path='JetGood',xbins=100,xmin=-2.5,xmax=2.5,ybins=200,ymin=-1*math.pi,ymax=math.pi)

    GeneralGroup.defineHistogram('passGood,jet_eta_all,jet_phi_all',type='TEfficiency',title='TEfficiency in 2D;Jet #eta;Jet #phi;Good Jet Efficiency',path='JetGood',xbins=100,xmin=-2.5,xmax=2.5,ybins=200,ymin=-1*math.pi,ymax=math.pi)

    #Suspect jet plots (same definition of good jet plots) --> jet quality from Suspect/Suspect/Bad selection)
    GeneralGroup.defineHistogram('jet_pT_suspect',title='Suspect jets pT;Suspect Jet pT [GeV];Jets',path='JetSuspect',xbins=100,xmin=0.0,xmax=200.0)
    GeneralGroup.defineHistogram('jet_eta_suspect',title='Suspect jets #eta;Suspect Jet #eta;Jets',path='JetSuspect',xbins=100,xmin=-2.5,xmax=2.5)
    GeneralGroup.defineHistogram('jet_phi_suspect',title='Suspect jets #phi;Suspect Jet #phi;Jets',path='JetSuspect',xbins=100,xmin=-1*math.pi,xmax=math.pi)
    GeneralGroup.defineHistogram('jet_MV_suspect',title='Suspect jets MV;Suspect Jet MV;Jets',path='JetSuspect',xbins=MV_bins,xmin=MV_start,xmax=MV_stop)

    GeneralGroup.defineHistogram('sus_jet_MV_pt_0_20',title='Suspect jet MV in pT bin 1 : pT = [0,20];Jet MV;Jets',path='JetSuspect',xbins=MV_bins,xmin=MV_start,xmax=MV_stop)
    GeneralGroup.defineHistogram('sus_jet_MV_pt_20_40',title='Suspect jet MV in pT bin 2 : pT = [20,40];Jet MV;Jets',path='JetSuspect',xbins=MV_bins,xmin=MV_start,xmax=MV_stop)
    GeneralGroup.defineHistogram('sus_jet_MV_pt_40_70',title='Suspect jet MV in pT bin 3 : pT = [40,70];Jet MV;Jets',path='JetSuspect',xbins=MV_bins,xmin=MV_start,xmax=MV_stop)
    GeneralGroup.defineHistogram('sus_jet_MV_pt_70_100',title='Suspect jet MV in pT bin 4 : pT = [70,100];Jet MV;Jets',path='JetSuspect',xbins=MV_bins,xmin=MV_start,xmax=MV_stop)
    GeneralGroup.defineHistogram('sus_jet_MV_pt_100_150',title='Suspect jet MV in pT bin 5 : pT = [100,150];Jet MV;Jets',path='JetSuspect',xbins=MV_bins,xmin=MV_start,xmax=MV_stop)
    GeneralGroup.defineHistogram('sus_jet_MV_pt_150_200',title='Suspect jet MV in pT bin 6 : pT = [150,200];Jet MV;Jets',path='JetSuspect',xbins=MV_bins,xmin=MV_start,xmax=MV_stop)
    GeneralGroup.defineHistogram('sus_jet_MV_pt_200_1000',title='Suspect jet MV in pT bin 7 : pT = [200,1000];Jet MV;Jets',path='JetSuspect',xbins=MV_bins,xmin=MV_start,xmax=MV_stop)

    GeneralGroup.defineHistogram('sus_jet_MV_eta_00_05',title='Suspect jet MV in #eta bin 1 : #eta = [0.0,0.5];Jet MV;Jets',path='JetSuspect',xbins=MV_bins,xmin=MV_start,xmax=MV_stop)
    GeneralGroup.defineHistogram('sus_jet_MV_eta_05_10',title='Suspect jet MV in #eta bin 2 : #eta = [0.5,1.0];Jet MV;Jets',path='JetSuspect',xbins=MV_bins,xmin=MV_start,xmax=MV_stop)
    GeneralGroup.defineHistogram('sus_jet_MV_eta_10_15',title='Suspect jet MV in #eta bin 3 : #eta = [1.0,1.5];Jet MV;Jets',path='JetSuspect',xbins=MV_bins,xmin=MV_start,xmax=MV_stop)
    GeneralGroup.defineHistogram('sus_jet_MV_eta_15_20',title='Suspect jet MV in #eta bin 4 : #eta = [1.5,2.0];Jet MV;Jets',path='JetSuspect',xbins=MV_bins,xmin=MV_start,xmax=MV_stop)
    GeneralGroup.defineHistogram('sus_jet_MV_eta_20_25',title='Suspect jet MV in #eta bin 5 : #eta = [2.0,2.5];Jet MV;Jets',path='JetSuspect',xbins=MV_bins,xmin=MV_start,xmax=MV_stop)

    GeneralGroup.defineHistogram('sus_jet_MV_phi_00_05',title='Suspect jet MV in #phi bin 1 : #phi = [0.0,0.5];Jet MV;Jets',path='JetSuspect',xbins=MV_bins,xmin=MV_start,xmax=MV_stop)
    GeneralGroup.defineHistogram('sus_jet_MV_phi_05_10',title='Suspect jet MV in #phi bin 2 : #phi = [0.5,1.0];Jet MV;Jets',path='JetSuspect',xbins=MV_bins,xmin=MV_start,xmax=MV_stop)
    GeneralGroup.defineHistogram('sus_jet_MV_phi_10_15',title='Suspect jet MV in #phi bin 3 : #phi = [1.0,1.5];Jet MV;Jets',path='JetSuspect',xbins=MV_bins,xmin=MV_start,xmax=MV_stop)
    GeneralGroup.defineHistogram('sus_jet_MV_phi_15_20',title='Suspect jet MV in #phi bin 4 : #phi = [1.5,2.0];Jet MV;Jets',path='JetSuspect',xbins=MV_bins,xmin=MV_start,xmax=MV_stop)
    GeneralGroup.defineHistogram('sus_jet_MV_phi_20_25',title='Suspect jet MV in #phi bin 5 : #phi = [2.0,2.5];Jet MV;Jets',path='JetSuspect',xbins=MV_bins,xmin=MV_start,xmax=MV_stop)
    GeneralGroup.defineHistogram('sus_jet_MV_phi_25_31',title='Suspect jet MV in #phi bin 6 : #phi = [2.5,3.1];Jet MV;Jets',path='JetSuspect',xbins=MV_bins,xmin=MV_start,xmax=MV_stop)

    GeneralGroup.defineHistogram('sus_jet_eta',title='Number of Suspect jets before passing tag vs #eta;Jet #eta;Number of jets',path='JetSuspect',xbins=10,xmin=-2.5,xmax=2.5)
    GeneralGroup.defineHistogram('sus_jet_eta_60tag',title='Number of Suspect jets passing 60 tag WP vs #eta;Jet #eta;Number of jets',path='JetSuspect',xbins=10,xmin=-2.5,xmax=2.5)
    GeneralGroup.defineHistogram('sus_jet_eta_70tag',title='Number of Suspect jets passing 70 tag WP vs #eta;Jet #eta;Number of jets',path='JetSuspect',xbins=10,xmin=-2.5,xmax=2.5)
    GeneralGroup.defineHistogram('sus_jet_eta_77tag',title='Number of Suspect jets passing 77 tag WP vs #eta;Jet #eta;Number of jets',path='JetSuspect',xbins=10,xmin=-2.5,xmax=2.5)
    GeneralGroup.defineHistogram('sus_jet_eta_85tag',title='Number of Suspect jets passing 85 tag WP vs #eta;Jet #eta;Number of jets',path='JetSuspect',xbins=10,xmin=-2.5,xmax=2.5)
    GeneralGroup.defineHistogram('pass60e,sus_jet_eta',type='TEfficiency',title='Suspect jets 60 tag WP Efficiency vs #eta;Jet #eta;60 tag WP Efficiency',path='JetSuspect',xbins=10,xmin=-2.5,xmax=2.5)
    GeneralGroup.defineHistogram('pass70e,sus_jet_eta',type='TEfficiency',title='Suspect jets 70 tag WP Efficiency vs #eta;Jet #eta;70 tag WP Efficiency',path='JetSuspect',xbins=10,xmin=-2.5,xmax=2.5)
    GeneralGroup.defineHistogram('pass77e,sus_jet_eta',type='TEfficiency',title='Suspect jets 77 tag WP Efficiency vs #eta;Jet #eta;77 tag WP Efficiency',path='JetSuspect',xbins=10,xmin=-2.5,xmax=2.5)
    GeneralGroup.defineHistogram('pass85e,sus_jet_eta',type='TEfficiency',title='Suspect jets 85 tag WP Efficiency vs #eta;Jet #eta;85 tag WP Efficiency',path='JetSuspect',xbins=10,xmin=-2.5,xmax=2.5)

    GeneralGroup.defineHistogram('sus_jet_phi',title='Number of Suspect jets before passing tag vs #phi;Jet #phi;Number of jets',path='JetSuspect',xbins=12,xmin=-1*math.pi,xmax=math.pi)
    GeneralGroup.defineHistogram('sus_jet_phi_60tag',title='Number of Suspect jets passing 60 tag WP vs #phi;Jet #phi;Number of jets',path='JetSuspect',xbins=12,xmin=-1*math.pi,xmax=math.pi)
    GeneralGroup.defineHistogram('sus_jet_phi_70tag',title='Number of Suspect jets passing 70 tag WP vs #phi;Jet #phi;Number of jets',path='JetSuspect',xbins=12,xmin=-1*math.pi,xmax=math.pi)
    GeneralGroup.defineHistogram('sus_jet_phi_77tag',title='Number of Suspect jets passing 77 tag WP vs #phi;Jet #phi;Number of jets',path='JetSuspect',xbins=12,xmin=-1*math.pi,xmax=math.pi)
    GeneralGroup.defineHistogram('sus_jet_phi_85tag',title='Number of Suspect jets passing 85 tag WP vs #phi;Jet #phi;Number of jets',path='JetSuspect',xbins=12,xmin=-1*math.pi,xmax=math.pi)
    GeneralGroup.defineHistogram('pass60f,sus_jet_phi',type='TEfficiency',title='Suspect jets 60 tag WP Efficiency vs #phi;Jet #phi;60 tag WP Efficiency',path='JetSuspect',xbins=12,xmin=-1*math.pi,xmax=math.pi)
    GeneralGroup.defineHistogram('pass70f,sus_jet_phi',type='TEfficiency',title='Suspect jets 70 tag WP Efficiency vs #phi;Jet #phi;70 tag WP Efficiency',path='JetSuspect',xbins=12,xmin=-1*math.pi,xmax=math.pi)
    GeneralGroup.defineHistogram('pass77f,sus_jet_phi',type='TEfficiency',title='Suspect jets 77 tag WP Efficiency vs #phi;Jet #phi;77 tag WP Efficiency',path='JetSuspect',xbins=12,xmin=-1*math.pi,xmax=math.pi)
    GeneralGroup.defineHistogram('pass85f,sus_jet_phi',type='TEfficiency',title='Suspect jets 85 tag WP Efficiency vs #phi;Jet #phi;85 tag WP Efficiency',path='JetSuspect',xbins=12,xmin=-1*math.pi,xmax=math.pi)

    GeneralGroup.defineHistogram('sus_jet_eta_60tag,sus_jet_phi_60tag;sus_jet_MAP_60tag',title='2D MAP of Suspect jets passing 60 tag WP;Jet #eta;Jet #phi',type='TH2F',path='JetSuspect',xbins=100,xmin=-2.5,xmax=2.5,ybins=200,ymin=-1*math.pi,ymax=math.pi)
    GeneralGroup.defineHistogram('sus_jet_eta_70tag,sus_jet_phi_70tag;sus_jet_MAP_70tag',title='2D MAP of Suspect jets passing 70 tag WP;Jet #eta;Jet #phi',type='TH2F',path='JetSuspect',xbins=100,xmin=-2.5,xmax=2.5,ybins=200,ymin=-1*math.pi,ymax=math.pi)
    GeneralGroup.defineHistogram('sus_jet_eta_77tag,sus_jet_phi_77tag;sus_jet_MAP_77tag',title='2D MAP of Suspect jets passing 77 tag WP;Jet #eta;Jet #phi',type='TH2F',path='JetSuspect',xbins=100,xmin=-2.5,xmax=2.5,ybins=200,ymin=-1*math.pi,ymax=math.pi)
    GeneralGroup.defineHistogram('sus_jet_eta_85tag,sus_jet_phi_85tag;sus_jet_MAP_85tag',title='2D MAP of Suspect jets passing 85 tag WP;Jet #eta;Jet #phi',type='TH2F',path='JetSuspect',xbins=100,xmin=-2.5,xmax=2.5,ybins=200,ymin=-1*math.pi,ymax=math.pi)

    ### STEP 6 ###
    # Finalize. The return value should be a tuple of the ComponentAccumulator
    # and the sequence containing the created algorithms. If we haven't called
    # any configuration other than the AthMonitorCfgHelper here, then we can 
    # just return directly (and not create "result" above)
    result.merge(helper.result())
    return result
    
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
    
    #Data r22 ART input working:
    ConfigFlags.Input.Files = ["/afs/cern.ch/work/a/alaperto/dq_test/dq_r22_FEB21/run/DQ_ARTs/myAOD.data18.25Feb.root"] #AOD from ART test, 25 Feb 2021, data18
    ConfigFlags.Input.isMC = False

    #MC r22 ART input working:
    #ConfigFlags.Input.Files = ["/afs/cern.ch/work/a/alaperto/dq_test/dq_r22_NOV20/run/DQ_ART_26Oct_mc16/myAOD.pool.root"] #AOD from ART test, 26 October, mc16
    #ConfigFlags.Input.isMC = True

    #Output file
    ConfigFlags.Output.HISTFileName = 'data18.00348885.physics_Main.HIST.root'
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
