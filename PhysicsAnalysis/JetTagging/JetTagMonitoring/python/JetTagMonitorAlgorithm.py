#
#  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
#

'''@file JetTagMonitorAlgorithm.py
@author A. Lapertosa
@author M. Tanasini
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
    jetTagMonAlg.MuonsCollection = "Muons"
    jetTagMonAlg.ElectronsCollection = "Electrons"

    #Skip jet filter selection in case of ion-ion or proton-ion collisions
    if inputFlags.DQ.DataType == 'heavyioncollisions':
        jetTagMonAlg.SkipPreSelection = True
        jetTagMonAlg.SkipJetFilter = True
        jetTagMonAlg.JetsCollection = "AntiKt4HIJets" #Heavy Ion jet collection
    else:
        jetTagMonAlg.SkipPreSelection = False
        jetTagMonAlg.SkipJetFilter = False
        jetTagMonAlg.JetsCollection = "AntiKt4EMPFlowJets" #Standard jet collection

    #Additional low-level taggers and extra plots (i.e. pu, pb, pc)
    jetTagMonAlg.DoExtraTaggerHistos = True

    #general pT / eta cuts for jets
    jetTagMonAlg.JetEtaCut = 2.5
    jetTagMonAlg.JetPtCut = 20.0
    
    #soft muon pT cut for SMT jets 
    jetTagMonAlg.SoftMuonPtMin = 5.0
    jetTagMonAlg.SoftMuonPtMax = 25.0

    #Track selection Tool, Loose WP: https://twiki.cern.ch/twiki/bin/viewauth/AtlasProtected/TrackingCPRecsEarly2018#Track_Selection
    jetTagMonAlg.TrackSelectionTool = CompFactory.InDet.InDetTrackSelectionTool('jetTagMonAlg_TrackSelectionTool')
    jetTagMonAlg.TrackSelectionTool.CutLevel = "Loose"

    #Additional track selection for jet quality assessment
    jetTagMonAlg.MinGoodTrackCut = 1
    jetTagMonAlg.TrackPtCut = 1.0 #GeV
    jetTagMonAlg.Trackd0Cut = 1.0 #mm
    jetTagMonAlg.Trackz0sinCut = 1.5 #mm
    jetTagMonAlg.TrackHitIBLCut = 1

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

    #JVT r22 recommendations: https://twiki.cern.ch/twiki/bin/viewauth/AtlasProtected/BTagRecommendationsRelease22#Default_Object_Recommendations_f
    jetTagMonAlg.JVTCut = 0.5
    jetTagMonAlg.JVTpTCut = 60.0
    jetTagMonAlg.JVTetaCut = 2.4
     
    #Main b-tagging algorithm r22: DL1dv00
    #r22 benchmarks from: https://ftag.docs.cern.ch/recommendations/r22-preliminary/#working-point-definition-for-dl1dv00
    jetTagMonAlg.TaggerName = "DL1dv00"
    
    if jetTagMonAlg.TaggerName == "DL1dv00" :
     jetTagMonAlg.WP60Cut = 4.884
     jetTagMonAlg.WP70Cut = 3.494
     jetTagMonAlg.WP77Cut = 2.443
     jetTagMonAlg.WP85Cut = 0.930
     jetTagMonAlg.cFraction = 0.018
     MV_bins = 100
     MV_start = -6.0
     MV_stop = 14.0
    else : #r21 DL1r WPs
     jetTagMonAlg.WP60Cut = 2.75
     jetTagMonAlg.WP70Cut = 2.01
     jetTagMonAlg.WP77Cut = 1.42
     jetTagMonAlg.WP85Cut = 0.41
     jetTagMonAlg.cFraction = 0.08
     MV_bins = 100
     MV_start = -6.0
     MV_stop = 14.0
    
    ### STEP 4 ###
    # Add some tools. N.B. Do not use your own trigger decision tool. Use the
    # standard one that is included with AthMonitorAlgorithm.

    # Add a tool that doesn't have its own configuration function. In
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

    #General histograms: cutflow, run, PV, tracks, hits
    GeneralGroup.defineHistogram('Cutflow_Event',
                                 title='Event CutFlow;Pass Event CutFlow;Events',
                                 path='Shifter',
                                 xbins=8,
                                 xmin=-0.5,
                                 xmax=7.5,
                                 xlabels=["All", "Good LAr", "PV present", 
                                          "PV Tracks", "Iso lepton", ">= 1 El", 
                                          ">=1 Mu", ""]
                                )

    GeneralGroup.defineHistogram('Cutflow_Jet',
                                 title='Jet CutFlow;Pass Jet CutFlow;Jets',
                                 path='Shifter',
                                 xbins=9,
                                 xmin=-0.5,
                                 xmax=8.5,
                                 xlabels=["All", "Filter", "Kinematic", 
                                          "JVT", "Overlap", "Good", 
                                          "Suspect", "Bad", ""]
                                )

    GeneralGroup.defineHistogram('Run_lb',title='Lumi Block;LB;Events',path='Expert',xbins=1000,xmin=-0.5,xmax=999.5)
    GeneralGroup.defineHistogram('Run_mu',title='Mu;<mu>;Events',path='Expert',xbins=100,xmin=0,xmax=100.0)
    GeneralGroup.defineHistogram('PV_x',title='Primary vertices X position;X;Events',path='Expert',xbins=40,xmin=-2.0,xmax=2.0)
    GeneralGroup.defineHistogram('PV_y',title='Primary vertices Y position;Y;Events',path='Expert',xbins=40,xmin=-2.0,xmax=2.0)
    GeneralGroup.defineHistogram('PV_z',title='Primary vertices Z position;Z;Events',path='Expert',xbins=100,xmin=-250,xmax=250.0)

    GeneralGroup.defineHistogram('PV_n',title='Primary vertices;PV;Events',path='Shifter',xbins=100,xmin=0,xmax=100.0)
    GeneralGroup.defineHistogram('PV_tracks_n',title='Number of tracks in PV;Tracks in PV;Number of tracks',path='Shifter',xbins=100,xmin=0,xmax=200)

    GeneralGroup.defineHistogram('Tracks_n',title='Track multiplicity;Tracks per event;Number of events',path='Shifter',xbins=150,xmin=0,xmax=1500)
    GeneralGroup.defineHistogram('Hits_IBL',title='Number of IBL hits;Hits on track;Number of IBL hits on track',path='Shifter',xbins=5,xmin=0,xmax=5)
    GeneralGroup.defineHistogram('Hits_Pixel',title='Number of Pixel hits;Hits on track;Number of Pixel hits on track',path='Shifter',xbins=10,xmin=0,xmax=10)
    GeneralGroup.defineHistogram('Hits_SCT',title='Number of SCT hits;Hits on track;Number of SCT hits on track',path='Shifter',xbins=15,xmin=0,xmax=15)
    GeneralGroup.defineHistogram('Hits_Si',title='Number of PIX+SCT hits;Hits on track;Number of PIX+SCT hits on track',path='Shifter',xbins=25,xmin=0,xmax=25)
    GeneralGroup.defineHistogram('Hits_TRT',title='Number of TRT hits;Hits on track;Number of TRT hits on track',path='Shifter',xbins=100,xmin=0,xmax=100)
    GeneralGroup.defineHistogram('Hits_ID',title='Number of ID hits;Hits on track;Number of ID hits on track',path='Shifter',xbins=150,xmin=0,xmax=150)

    #Jet tracks hits and impact parameters (loose tracks)
    GeneralGroup.defineHistogram('JetTracks_eta_loose_IBL,JetTracks_phi_loose_IBL;JetTracks_MAP_loose_IBL',title='2D MAP of loose tracks from jets with IBL hit;Track #eta;Track #phi',type='TH2F',path='JetTracks',xbins=50,xmin=-2.5,xmax=2.5,ybins=100,ymin=-1*math.pi,ymax=math.pi)
    GeneralGroup.defineHistogram('JetTracks_eta_loose_BL,JetTracks_phi_loose_BL;JetTracks_MAP_loose_BL',title='2D MAP of loose tracks from jets with BL hit;Track #eta;Track #phi',type='TH2F',path='JetTracks',xbins=50,xmin=-2.5,xmax=2.5,ybins=100,ymin=-1*math.pi,ymax=math.pi)

    GeneralGroup.defineHistogram('JetTracks_Hits_IBL',title='Number of IBL hits on tracks in jets;Hits on track;Number of IBL hits on track',path='JetTracks',xbins=5,xmin=0,xmax=5)
    GeneralGroup.defineHistogram('JetTracks_Hits_IBL_expect',title='Number of IBL hits on tracks in jets;Hits on track;Number of IBL hits on track',path='JetTracks',xbins=5,xmin=0,xmax=5)
    GeneralGroup.defineHistogram('JetTracks_Hits_BL',title='Number of B-Layer hits on tracks in jets;Hits on track;Number of B-Layer hits on track',path='JetTracks',xbins=5,xmin=0,xmax=5)
    GeneralGroup.defineHistogram('JetTracks_Hits_BL_expect',title='Number of B-Layer hits on tracks in jets;Hits on track;Number of B-Layer hits on track',path='JetTracks',xbins=5,xmin=0,xmax=5)

    GeneralGroup.defineHistogram('JetTracks_Hits_PIX',title='Number of Pixel hits on tracks in jets;Hits on track;Number of Pixel hits on track',path='JetTracks',xbins=10,xmin=0,xmax=10)
    GeneralGroup.defineHistogram('JetTracks_Holes_PIX',title='Number of Pixel holes on tracks in jets;Holes on track;Number of Pixel holes on track',path='JetTracks',xbins=5,xmin=0,xmax=5)
    GeneralGroup.defineHistogram('JetTracks_Hits_SCT',title='Number of SCT hits on tracks in jets;Hits on track;Number of SCT hits on track',path='JetTracks',xbins=15,xmin=0,xmax=15)
    GeneralGroup.defineHistogram('JetTracks_Holes_SCT',title='Number of SCT holes on tracks in jets;Holes on track;Number of SCT holes on track',path='JetTracks',xbins=5,xmin=0,xmax=5)
    GeneralGroup.defineHistogram('JetTracks_Hits_TRT',title='Number of TRT hits on tracks in jets;Hits on track;Number of TRT hits on track',path='JetTracks',xbins=100,xmin=0,xmax=100)
    GeneralGroup.defineHistogram('JetTracks_Hits_Si',title='Number of Si (PIX+SCT) hits on tracks in jets;Hits on track;Number of Si (PIX+SCT) hits on track',path='JetTracks',xbins=20,xmin=0,xmax=20)
    GeneralGroup.defineHistogram('JetTracks_Holes_Si',title='Number of Si (PIX+SCT) holes on tracks in jets;Holes on track;Number of Si (PIX+SCT) holes on track',path='JetTracks',xbins=5,xmin=0,xmax=5)

    GeneralGroup.defineHistogram('JetTracks_IP_d0',title='d_0 of tracks in jet;d_0;Tracks in jets',path='JetTracks',xbins=100,xmin=-5,xmax=5)
    GeneralGroup.defineHistogram('JetTracks_IP_d0s',title='sigma d_0 of tracks in jet;sigma d_0;Tracks in jets',path='JetTracks',xbins=50,xmin=0,xmax=5)
    GeneralGroup.defineHistogram('JetTracks_IP_d0si',title='significance d_0 of tracks in jet;significance d_0;Tracks in jets',path='JetTracks',xbins=100,xmin=-5,xmax=5)
    GeneralGroup.defineHistogram('JetTracks_IP_z0',title='z_0 of tracks in jet;z_0;Tracks in jets',path='JetTracks',xbins=100,xmin=-5,xmax=5)
    GeneralGroup.defineHistogram('JetTracks_IP_z0s',title='sigma z_0 of tracks in jet;sigma z_0;Tracks in jets',path='JetTracks',xbins=50,xmin=0,xmax=5)
    GeneralGroup.defineHistogram('JetTracks_IP_z0si',title='significance z_0 of tracks in jet;significance z_0;Tracks in jets',path='JetTracks',xbins=100,xmin=-5,xmax=5)
    GeneralGroup.defineHistogram('JetTracks_IP_z0sin',title='z0 #times sin (#theta) of tracks in jet;z0 #times sin (#theta);Tracks in jets',path='JetTracks',xbins=100,xmin=-5,xmax=5)

    #Jet quality selection (good, suspect, bad) based on its tracks
    GeneralGroup.defineHistogram('JetTracks_n_0_all',title='Jet track multiplicity (all);Tracks;Number of tracks per jet',path='TracksInJetSelection',xbins=50,xmin=0,xmax=50)
    GeneralGroup.defineHistogram('JetTracks_n_1_loose',title='Jet track multiplicity (loose);Tracks;Number of tracks per jet',path='TracksInJetSelection',xbins=50,xmin=0,xmax=50)
    GeneralGroup.defineHistogram('JetTracks_n_2_kin',title='Jet track multiplicity (>1 GeV);Tracks;Number of tracks per jet',path='TracksInJetSelection',xbins=25,xmin=0,xmax=25)
    GeneralGroup.defineHistogram('JetTracks_n_3_IP',title='Jet track multiplicity (IP cut);Tracks;Number of tracks per jet',path='TracksInJetSelection',xbins=25,xmin=0,xmax=25)
    GeneralGroup.defineHistogram('JetTracks_n_4_IBL',title='Jet track multiplicity (I-BL hit);Tracks;Number of tracks per jet',path='TracksInJetSelection',xbins=25,xmin=0,xmax=25)

    GeneralGroup.defineHistogram('JetTracks_pT_0_all',title='pT of tracks in jet (all);pT [GeV];Tracks in jets',path='TracksInJetSelection',xbins=60,xmin=0,xmax=30)
    GeneralGroup.defineHistogram('JetTracks_eta_0_all',title='#eta of tracks in jet (all);#eta;Tracks in jets',path='TracksInJetSelection',xbins=100,xmin=-2.5,xmax=2.5)
    GeneralGroup.defineHistogram('JetTracks_phi_0_all',title='#phi of tracks in jet (all);#phi;Tracks in jets',path='TracksInJetSelection',xbins=100,xmin=-1*math.pi,xmax=math.pi)
    GeneralGroup.defineHistogram('JetTracks_DR_0_all',title='DeltaR of tracks in jet (all);DeltaR;Tracks in jets',path='TracksInJetSelection',xbins=100,xmin=0,xmax=1)

    GeneralGroup.defineHistogram('JetTracks_pT_1_loose',title='pT of tracks in jet (loose);pT [GeV];Tracks in jets',path='TracksInJetSelection',xbins=60,xmin=0,xmax=30)
    GeneralGroup.defineHistogram('JetTracks_eta_1_loose',title='#eta of tracks in jet (loose);#eta;Tracks in jets',path='TracksInJetSelection',xbins=100,xmin=-2.5,xmax=2.5)
    GeneralGroup.defineHistogram('JetTracks_phi_1_loose',title='#phi of tracks in jet (loose);#phi;Tracks in jets',path='TracksInJetSelection',xbins=100,xmin=-1*math.pi,xmax=math.pi)

    #2D Map of selection of tracks in jets
    GeneralGroup.defineHistogram('JetTracks_eta_0_all,JetTracks_phi_0_all;JetTracks_MAP_0_all',title='2D MAP of tracks from jets (all);Track #eta;Track #phi',type='TH2F',path='TracksInJetSelection',xbins=50,xmin=-2.5,xmax=2.5,ybins=100,ymin=-1*math.pi,ymax=math.pi)
    GeneralGroup.defineHistogram('JetTracks_eta_1_loose,JetTracks_phi_1_loose;JetTracks_MAP_1_loose',title='2D MAP of tracks from jets (loose);Track #eta;Track #phi',type='TH2F',path='TracksInJetSelection',xbins=50,xmin=-2.5,xmax=2.5,ybins=100,ymin=-1*math.pi,ymax=math.pi)
    GeneralGroup.defineHistogram('JetTracks_eta_2_kin,JetTracks_phi_2_kin;JetTracks_MAP_2_kin',title='2D MAP of tracks from jets (>1 GeV);Track #eta;Track #phi',type='TH2F',path='TracksInJetSelection',xbins=50,xmin=-2.5,xmax=2.5,ybins=100,ymin=-1*math.pi,ymax=math.pi)
    GeneralGroup.defineHistogram('JetTracks_eta_3_IP,JetTracks_phi_3_IP;JetTracks_MAP_3_IP',title='2D MAP of tracks from jets passed (IP sel);Track #eta;Track #phi',type='TH2F',path='TracksInJetSelection',xbins=50,xmin=-2.5,xmax=2.5,ybins=100,ymin=-1*math.pi,ymax=math.pi)
    GeneralGroup.defineHistogram('JetTracks_eta_4_IBL,JetTracks_phi_4_IBL;JetTracks_MAP_4_IBL',title='2D MAP of tracks from jets passed (I-BL hit);Track #eta;Track #phi',type='TH2F',path='TracksInJetSelection',xbins=50,xmin=-2.5,xmax=2.5,ybins=100,ymin=-1*math.pi,ymax=math.pi)

    #Jets from ttbar events histograms
    GeneralGroup.defineHistogram('Muon_pT',title='Muon pT;Muon pT;Events',path='TTbarEventSelection',xbins=100,xmin=0.0,xmax=200000.0)
    GeneralGroup.defineHistogram('Electron_pT',title='Electron pT;Electron pT;Events',path='TTbarEventSelection',xbins=100,xmin=0.0,xmax=200000.0)
    
    GeneralGroup.defineHistogram('IsolatedMuons_n',title='Number of isolated Muons;Isolated muons;Number of events',path='TTbarEventSelection',xbins=3,xmin=-0.5,xmax=2.5)
    GeneralGroup.defineHistogram('IsolatedElectrons_n',title='Number of isolated Electrons;Isolated electrons;Number of events',path='TTbarEventSelection',xbins=3,xmin=-0.5,xmax=2.5)

    GeneralGroup.defineHistogram('nTTbarGoodJets',title='Number of good jets in ttbar events;Good jets per event;Number of events',path='TTbarEventSelection',xbins=10,xmin=-0.5,xmax=9.5)

    GeneralGroup.defineHistogram('TTbarJets_n',title='Total number of ttbar jets;Number of jets;Number of events',path='TTbarEventSelection',xbins=1,xmin=-0.5,xmax=0.5)
    GeneralGroup.defineHistogram('TTbarJets_n_60tag',title='Number of ttbar jets passing 60 tag WP; Jets passing 60 tag WP;Number of ttbar jets',path='TTbarEventSelection',xbins=1,xmin=-0.5,xmax=0.5)
    GeneralGroup.defineHistogram('TTbarJets_n_70tag',title='Number of ttbar jets passing 70 tag WP; Jets passing 70 tag WP;Number of ttbar jets',path='TTbarEventSelection',xbins=1,xmin=-0.5,xmax=0.5)
    GeneralGroup.defineHistogram('TTbarJets_n_77tag',title='Number of ttbar jets passing 77 tag WP; Jets passing 77 tag WP;Number of ttbar jets',path='TTbarEventSelection',xbins=1,xmin=-0.5,xmax=0.5)
    GeneralGroup.defineHistogram('TTbarJets_n_85tag',title='Number of ttbar jets passing 85 tag WP; Jets passing 60 tag WP;Number of ttbar jets',path='TTbarEventSelection',xbins=1,xmin=-0.5,xmax=0.5)
    GeneralGroup.defineHistogram('pass60n,TTbarJets_n',type='TEfficiency',title='TTbar jets 60 tag WP Efficiency;ttbar jets;60 tag WP Efficiency',path='TTbarEventSelection',xbins=1,xmin=-0.5,xmax=0.5)
    GeneralGroup.defineHistogram('pass70n,TTbarJets_n',type='TEfficiency',title='TTbar jets 70 tag WP Efficiency;ttbar jets;70 tag WP Efficiency',path='TTbarEventSelection',xbins=1,xmin=-0.5,xmax=0.5)
    GeneralGroup.defineHistogram('pass77n,TTbarJets_n',type='TEfficiency',title='TTbar jets 77 tag WP Efficiency;ttbar jets;77 tag WP Efficiency',path='TTbarEventSelection',xbins=1,xmin=-0.5,xmax=0.5)
    GeneralGroup.defineHistogram('pass85n,TTbarJets_n',type='TEfficiency',title='TTbar jets 85 tag WP Efficiency;ttbar jets;85 tag WP Efficiency',path='TTbarEventSelection',xbins=1,xmin=-0.5,xmax=0.5)

    GeneralGroup.defineHistogram('TTbarJets_MV',title='MV of jets in ttbar events;Jet MV;Number of jets',path='JetTTbarEvents',xbins=int(MV_bins/2),xmin=MV_start,xmax=MV_stop)
    GeneralGroup.defineHistogram('TTbarJets_pT',title='Number of ttbar jets vs pT;ttbar jet pT;Number of ttbar jets',path='JetTTbarEvents',xbins=40,xmin=0,xmax=200)
    GeneralGroup.defineHistogram('TTbarJets_pT_60tag',title='Number of ttbar jets passing 60 tag WP vs pT;ttbar jet pT;Number of ttbar jets',path='JetTTbarEvents',xbins=40,xmin=0,xmax=200)
    GeneralGroup.defineHistogram('TTbarJets_pT_70tag',title='Number of ttbar jets passing 70 tag WP vs pT;ttbar jet pT;Number of ttbar jets',path='JetTTbarEvents',xbins=40,xmin=0,xmax=200)
    GeneralGroup.defineHistogram('TTbarJets_pT_77tag',title='Number of ttbar jets passing 77 tag WP vs pT;ttbar jet pT;Number of ttbar jets',path='JetTTbarEvents',xbins=40,xmin=0,xmax=200)
    GeneralGroup.defineHistogram('TTbarJets_pT_85tag',title='Number of ttbar jets passing 85 tag WP vs pT;ttbar jet pT;Number of ttbar jets',path='JetTTbarEvents',xbins=40,xmin=0,xmax=200)
    GeneralGroup.defineHistogram('pass60p,TTbarJets_pT',type='TEfficiency',title='TTbar jets 60 tag WP Efficiency vs pT;ttbar jet pT;60 tag WP Efficiency',path='JetTTbarEvents',xbins=40,xmin=0.0,xmax=200.0)
    GeneralGroup.defineHistogram('pass70p,TTbarJets_pT',type='TEfficiency',title='TTbar jets 70 tag WP Efficiency vs pT;ttbar jet pT;70 tag WP Efficiency',path='JetTTbarEvents',xbins=40,xmin=0.0,xmax=200.0)
    GeneralGroup.defineHistogram('pass77p,TTbarJets_pT',type='TEfficiency',title='TTbar jets 77 tag WP Efficiency vs pT;ttbar jet pT;77 tag WP Efficiency',path='JetTTbarEvents',xbins=40,xmin=0.0,xmax=200.0)
    GeneralGroup.defineHistogram('pass85p,TTbarJets_pT',type='TEfficiency',title='TTbar jets 85 tag WP Efficiency vs pT;ttbar jet pT;85 tag WP Efficiency',path='JetTTbarEvents',xbins=40,xmin=0.0,xmax=200.0)

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
    GeneralGroup.defineHistogram('jet_n_all',title='Jet multiplicity (before selection);Number of jets;Events',path='Shifter',xbins=50,xmin=0,xmax=50)
    GeneralGroup.defineHistogram('jet_pT_all',title='Jet pT (before selection);Jet pT [GeV];Jets',path='JetSelection',xbins=100,xmin=0.0,xmax=200.0)
    GeneralGroup.defineHistogram('jet_eta_all',title='Jet eta (before selection);Jet #eta;Jets',path='JetSelection',xbins=100,xmin=-2.5,xmax=2.5)
    GeneralGroup.defineHistogram('jet_phi_all',title='Jet phi (before selection);Jet #phi;Jets',path='JetSelection',xbins=100,xmin=-1*math.pi,xmax=math.pi)
    GeneralGroup.defineHistogram('jet_MV_all',title='Jet MV (before selection);Jet MV;Jets',path='JetSelection',xbins=MV_bins,xmin=MV_start,xmax=MV_stop)

    #2D maps during jet selection
    GeneralGroup.defineHistogram('jet_eta_all,jet_phi_all;jet_MAP_0_all',title='2D MAP of all jets;Jet #eta;Jet #phi',type='TH2F',path='JetSelection',xbins=50,xmin=-2.5,xmax=2.5,ybins=100,ymin=-1*math.pi,ymax=math.pi)
    GeneralGroup.defineHistogram('jet_eta_filter,jet_phi_filter;jet_MAP_1_filter',title='2D MAP of jets passing basic filter;Jet #eta;Jet #phi',type='TH2F',path='JetSelection',xbins=50,xmin=-2.5,xmax=2.5,ybins=100,ymin=-1*math.pi,ymax=math.pi)
    GeneralGroup.defineHistogram('jet_eta_kin,jet_phi_kin;jet_MAP_2_kin',title='2D MAP of jets passing kinematic selection;Jet #eta;Jet #phi',type='TH2F',path='JetSelection',xbins=25,xmin=-2.5,xmax=2.5,ybins=50,ymin=-1*math.pi,ymax=math.pi)
    GeneralGroup.defineHistogram('jet_eta_jvt,jet_phi_jvt;jet_MAP_3_jvt',title='2D MAP of jets passing JVT;Jet #eta;Jet #phi',type='TH2F',path='JetSelection',xbins=25,xmin=-2.5,xmax=2.5,ybins=50,ymin=-1*math.pi,ymax=math.pi)
    GeneralGroup.defineHistogram('jet_eta_overlap,jet_phi_overlap;jet_MAP_4_overlap',title='2D MAP of not overlapping jets;Jet #eta;Jet #phi',type='TH2F',path='JetSelection',xbins=25,xmin=-2.5,xmax=2.5,ybins=50,ymin=-1*math.pi,ymax=math.pi)
    GeneralGroup.defineHistogram('jet_eta_good,jet_phi_good;jet_MAP_5_good',title='2D MAP of good jets;Jet #eta;Jet #phi',type='TH2F',path='JetSelection',xbins=25,xmin=-2.5,xmax=2.5,ybins=50,ymin=-1*math.pi,ymax=math.pi)
    GeneralGroup.defineHistogram('jet_eta_suspect,jet_phi_suspect;jet_MAP_6_suspect',title='2D MAP of suspect jets;Jet #eta;Jet #phi',type='TH2F',path='JetSelection',xbins=25,xmin=-2.5,xmax=2.5,ybins=50,ymin=-1*math.pi,ymax=math.pi)
    GeneralGroup.defineHistogram('jet_eta_bad,jet_phi_bad;jet_MAP_7_bad',title='2D MAP of bad jets;Jet #eta;Jet #phi',type='TH2F',path='JetSelection',xbins=25,xmin=-2.5,xmax=2.5,ybins=50,ymin=-1*math.pi,ymax=math.pi)
    GeneralGroup.defineHistogram('jet_eta_smt,jet_phi_smt;jet_MAP_X_smt',title='2D MAP of SMT jets;Jet #eta;Jet #phi',type='TH2F',path='JetSelection',xbins=25,xmin=-2.5,xmax=2.5,ybins=50,ymin=-1*math.pi,ymax=math.pi)

    GeneralGroup.defineHistogram('jet_MV_2_beforeJVTCut',title='Jet MV before JVT cut;Jet MV;Jets',path='JetSelection',xbins=MV_bins,xmin=MV_start,xmax=MV_stop)
    GeneralGroup.defineHistogram('jet_MV_3_afterJVTCut',title='Jet MV after JVT cut;Jet MV;Jets',path='JetSelection',xbins=MV_bins,xmin=MV_start,xmax=MV_stop)
    
    #Distributions for good jets (passing Good/Suspect/Bad selection)
    GeneralGroup.defineHistogram('jet_pT_good',title='Good jets pT;Good Jet pT [GeV];Jets',path='Shifter',xbins=100,xmin=0.0,xmax=200.0)
    GeneralGroup.defineHistogram('jet_eta_good',title='Good jets #eta;Good Jet #eta;Jets',path='Shifter',xbins=100,xmin=-2.5,xmax=2.5)
    GeneralGroup.defineHistogram('jet_phi_good',title='Good jets #phi;Good Jet #phi;Jets',path='Shifter',xbins=100,xmin=-1*math.pi,xmax=math.pi)
    GeneralGroup.defineHistogram('jet_MV_good',title='Good jets MV;Good Jet MV;Jets',path='Shifter',xbins=MV_bins,xmin=MV_start,xmax=MV_stop)
   
    #Extra Taggers plots
    GeneralGroup.defineHistogram('jet_MV_pu_good',title='Good jets MV pu;Good Jet MV pu;Jets',path='Shifter',xbins=100,xmin=0,xmax=1.0)
    GeneralGroup.defineHistogram('jet_MV_pc_good',title='Good jets MV pc;Good Jet MV pc;Jets',path='Shifter',xbins=100,xmin=0,xmax=1.0)
    GeneralGroup.defineHistogram('jet_MV_pb_good',title='Good jets MV pb;Good Jet MV pb;Jets',path='Shifter',xbins=100,xmin=0,xmax=1.0)


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
    GeneralGroup.defineHistogram('jet_eta',title='Number of jets before passing tag vs #eta;Jet #eta;Number of jets',path='JetEtaPhi',xbins=20,xmin=-2.5,xmax=2.5)
    GeneralGroup.defineHistogram('jet_eta_60tag',title='Number of jets passing 60 tag WP vs #eta;Jet #eta;Number of jets',path='JetEtaPhi',xbins=20,xmin=-2.5,xmax=2.5)
    GeneralGroup.defineHistogram('jet_eta_70tag',title='Number of jets passing 70 tag WP vs #eta;Jet #eta;Number of jets',path='JetEtaPhi',xbins=20,xmin=-2.5,xmax=2.5)
    GeneralGroup.defineHistogram('jet_eta_77tag',title='Number of jets passing 77 tag WP vs #eta;Jet #eta;Number of jets',path='JetEtaPhi',xbins=20,xmin=-2.5,xmax=2.5)
    GeneralGroup.defineHistogram('jet_eta_85tag',title='Number of jets passing 85 tag WP vs #eta;Jet #eta;Number of jets',path='JetEtaPhi',xbins=20,xmin=-2.5,xmax=2.5)
    GeneralGroup.defineHistogram('pass60e,jet_eta',type='TEfficiency',title='Jets 60 tag WP Efficiency vs #eta;Jet #eta;60 tag WP Efficiency',path='JetEtaPhi',xbins=20,xmin=-2.5,xmax=2.5)
    GeneralGroup.defineHistogram('pass70e,jet_eta',type='TEfficiency',title='Jets 70 tag WP Efficiency vs #eta;Jet #eta;70 tag WP Efficiency',path='JetEtaPhi',xbins=20,xmin=-2.5,xmax=2.5)
    GeneralGroup.defineHistogram('pass77e,jet_eta',type='TEfficiency',title='Jets 77 tag WP Efficiency vs #eta;Jet #eta;77 tag WP Efficiency',path='JetEtaPhi',xbins=20,xmin=-2.5,xmax=2.5)
    GeneralGroup.defineHistogram('pass85e,jet_eta',type='TEfficiency',title='Jets 85 tag WP Efficiency vs #eta;Jet #eta;85 tag WP Efficiency',path='JetEtaPhi',xbins=20,xmin=-2.5,xmax=2.5)

    GeneralGroup.defineHistogram('jet_phi',title='Number of jets before passing tag vs #phi;Jet #phi;Number of jets',path='JetEtaPhi',xbins=24,xmin=-1*math.pi,xmax=math.pi)
    GeneralGroup.defineHistogram('jet_phi_60tag',title='Number of jets passing 60 tag WP vs #phi;Jet #phi;Number of jets',path='JetEtaPhi',xbins=24,xmin=-1*math.pi,xmax=math.pi)
    GeneralGroup.defineHistogram('jet_phi_70tag',title='Number of jets passing 70 tag WP vs #phi;Jet #phi;Number of jets',path='JetEtaPhi',xbins=24,xmin=-1*math.pi,xmax=math.pi)
    GeneralGroup.defineHistogram('jet_phi_77tag',title='Number of jets passing 77 tag WP vs #phi;Jet #phi;Number of jets',path='JetEtaPhi',xbins=24,xmin=-1*math.pi,xmax=math.pi)
    GeneralGroup.defineHistogram('jet_phi_85tag',title='Number of jets passing 85 tag WP vs #phi;Jet #phi;Number of jets',path='JetEtaPhi',xbins=24,xmin=-1*math.pi,xmax=math.pi)
    GeneralGroup.defineHistogram('pass60f,jet_phi',type='TEfficiency',title='Jets 60 tag WP Efficiency vs #phi;Jet #phi;60 tag WP Efficiency',path='JetEtaPhi',xbins=24,xmin=-1*math.pi,xmax=math.pi)
    GeneralGroup.defineHistogram('pass70f,jet_phi',type='TEfficiency',title='Jets 70 tag WP Efficiency vs #phi;Jet #phi;70 tag WP Efficiency',path='JetEtaPhi',xbins=24,xmin=-1*math.pi,xmax=math.pi)
    GeneralGroup.defineHistogram('pass77f,jet_phi',type='TEfficiency',title='Jets 77 tag WP Efficiency vs #phi;Jet #phi;77 tag WP Efficiency',path='JetEtaPhi',xbins=24,xmin=-1*math.pi,xmax=math.pi)
    GeneralGroup.defineHistogram('pass85f,jet_phi',type='TEfficiency',title='Jets 85 tag WP Efficiency vs #phi;Jet #phi;85 tag WP Efficiency',path='JetEtaPhi',xbins=24,xmin=-1*math.pi,xmax=math.pi)

    #Good jet: 2D MAP (eta/phi) and 2D TEfficiency of fraction of good jets
    GeneralGroup.defineHistogram('jet_eta_60tag,jet_phi_60tag;jet_MAP_60tag',title='2D MAP of jets passing 60 tag WP;Jet #eta;Jet #phi',type='TH2F',path='JetEtaPhi',xbins=25,xmin=-2.5,xmax=2.5,ybins=50,ymin=-1*math.pi,ymax=math.pi)
    GeneralGroup.defineHistogram('jet_eta_70tag,jet_phi_70tag;jet_MAP_70tag',title='2D MAP of jets passing 70 tag WP;Jet #eta;Jet #phi',type='TH2F',path='JetEtaPhi',xbins=25,xmin=-2.5,xmax=2.5,ybins=50,ymin=-1*math.pi,ymax=math.pi)
    GeneralGroup.defineHistogram('jet_eta_77tag,jet_phi_77tag;jet_MAP_77tag',title='2D MAP of jets passing 77 tag WP;Jet #eta;Jet #phi',type='TH2F',path='JetEtaPhi',xbins=25,xmin=-2.5,xmax=2.5,ybins=50,ymin=-1*math.pi,ymax=math.pi)
    GeneralGroup.defineHistogram('jet_eta_85tag,jet_phi_85tag;jet_MAP_85tag',title='2D MAP of jets passing 85 tag WP;Jet #eta;Jet #phi',type='TH2F',path='JetEtaPhi',xbins=25,xmin=-2.5,xmax=2.5,ybins=50,ymin=-1*math.pi,ymax=math.pi)

    #Suspect jet plots (same definition of good jet plots) --> jet quality from Suspect/Suspect/Bad selection)
    GeneralGroup.defineHistogram('jet_pT_suspect',title='Suspect jets pT;Suspect Jet pT [GeV];Jets',path='JetSuspect',xbins=100,xmin=0.0,xmax=200.0)
    GeneralGroup.defineHistogram('jet_eta_suspect',title='Suspect jets #eta;Suspect Jet #eta;Jets',path='JetSuspect',xbins=100,xmin=-2.5,xmax=2.5)
    GeneralGroup.defineHistogram('jet_phi_suspect',title='Suspect jets #phi;Suspect Jet #phi;Jets',path='JetSuspect',xbins=100,xmin=-1*math.pi,xmax=math.pi)
    GeneralGroup.defineHistogram('jet_MV_suspect',title='Suspect jets MV;Suspect Jet MV;Jets',path='JetSuspect',xbins=MV_bins,xmin=MV_start,xmax=MV_stop)

    GeneralGroup.defineHistogram('sus_jet_MV_pt_0_20',title='Suspect jet MV in pT bin 1 : pT = [0,20];Jet MV;Jets',path='JetSuspect',xbins=int(MV_bins/2),xmin=MV_start,xmax=MV_stop)
    GeneralGroup.defineHistogram('sus_jet_MV_pt_20_40',title='Suspect jet MV in pT bin 2 : pT = [20,40];Jet MV;Jets',path='JetSuspect',xbins=int(MV_bins/2),xmin=MV_start,xmax=MV_stop)
    GeneralGroup.defineHistogram('sus_jet_MV_pt_40_70',title='Suspect jet MV in pT bin 3 : pT = [40,70];Jet MV;Jets',path='JetSuspect',xbins=int(MV_bins/2),xmin=MV_start,xmax=MV_stop)
    GeneralGroup.defineHistogram('sus_jet_MV_pt_70_100',title='Suspect jet MV in pT bin 4 : pT = [70,100];Jet MV;Jets',path='JetSuspect',xbins=int(MV_bins/2),xmin=MV_start,xmax=MV_stop)
    GeneralGroup.defineHistogram('sus_jet_MV_pt_100_150',title='Suspect jet MV in pT bin 5 : pT = [100,150];Jet MV;Jets',path='JetSuspect',xbins=int(MV_bins/2),xmin=MV_start,xmax=MV_stop)
    GeneralGroup.defineHistogram('sus_jet_MV_pt_150_200',title='Suspect jet MV in pT bin 6 : pT = [150,200];Jet MV;Jets',path='JetSuspect',xbins=int(MV_bins/2),xmin=MV_start,xmax=MV_stop)
    GeneralGroup.defineHistogram('sus_jet_MV_pt_200_1000',title='Suspect jet MV in pT bin 7 : pT = [200,1000];Jet MV;Jets',path='JetSuspect',xbins=int(MV_bins/2),xmin=MV_start,xmax=MV_stop)

    GeneralGroup.defineHistogram('sus_jet_MV_eta_00_05',title='Suspect jet MV in #eta bin 1 : #eta = [0.0,0.5];Jet MV;Jets',path='JetSuspect',xbins=int(MV_bins/2),xmin=MV_start,xmax=MV_stop)
    GeneralGroup.defineHistogram('sus_jet_MV_eta_05_10',title='Suspect jet MV in #eta bin 2 : #eta = [0.5,1.0];Jet MV;Jets',path='JetSuspect',xbins=int(MV_bins/2),xmin=MV_start,xmax=MV_stop)
    GeneralGroup.defineHistogram('sus_jet_MV_eta_10_15',title='Suspect jet MV in #eta bin 3 : #eta = [1.0,1.5];Jet MV;Jets',path='JetSuspect',xbins=int(MV_bins/2),xmin=MV_start,xmax=MV_stop)
    GeneralGroup.defineHistogram('sus_jet_MV_eta_15_20',title='Suspect jet MV in #eta bin 4 : #eta = [1.5,2.0];Jet MV;Jets',path='JetSuspect',xbins=int(MV_bins/2),xmin=MV_start,xmax=MV_stop)
    GeneralGroup.defineHistogram('sus_jet_MV_eta_20_25',title='Suspect jet MV in #eta bin 5 : #eta = [2.0,2.5];Jet MV;Jets',path='JetSuspect',xbins=int(MV_bins/2),xmin=MV_start,xmax=MV_stop)

    GeneralGroup.defineHistogram('sus_jet_MV_phi_00_05',title='Suspect jet MV in #phi bin 1 : #phi = [0.0,0.5];Jet MV;Jets',path='JetSuspect',xbins=int(MV_bins/2),xmin=MV_start,xmax=MV_stop)
    GeneralGroup.defineHistogram('sus_jet_MV_phi_05_10',title='Suspect jet MV in #phi bin 2 : #phi = [0.5,1.0];Jet MV;Jets',path='JetSuspect',xbins=int(MV_bins/2),xmin=MV_start,xmax=MV_stop)
    GeneralGroup.defineHistogram('sus_jet_MV_phi_10_15',title='Suspect jet MV in #phi bin 3 : #phi = [1.0,1.5];Jet MV;Jets',path='JetSuspect',xbins=int(MV_bins/2),xmin=MV_start,xmax=MV_stop)
    GeneralGroup.defineHistogram('sus_jet_MV_phi_15_20',title='Suspect jet MV in #phi bin 4 : #phi = [1.5,2.0];Jet MV;Jets',path='JetSuspect',xbins=int(MV_bins/2),xmin=MV_start,xmax=MV_stop)
    GeneralGroup.defineHistogram('sus_jet_MV_phi_20_25',title='Suspect jet MV in #phi bin 5 : #phi = [2.0,2.5];Jet MV;Jets',path='JetSuspect',xbins=int(MV_bins/2),xmin=MV_start,xmax=MV_stop)
    GeneralGroup.defineHistogram('sus_jet_MV_phi_25_31',title='Suspect jet MV in #phi bin 6 : #phi = [2.5,3.1];Jet MV;Jets',path='JetSuspect',xbins=int(MV_bins/2),xmin=MV_start,xmax=MV_stop)

    GeneralGroup.defineHistogram('sus_jet_eta',title='Number of Suspect jets before passing tag vs #eta;Jet #eta;Number of jets',path='JetSuspect',xbins=20,xmin=-2.5,xmax=2.5)
    GeneralGroup.defineHistogram('sus_jet_eta_60tag',title='Number of Suspect jets passing 60 tag WP vs #eta;Jet #eta;Number of jets',path='JetSuspect',xbins=20,xmin=-2.5,xmax=2.5)
    GeneralGroup.defineHistogram('sus_jet_eta_70tag',title='Number of Suspect jets passing 70 tag WP vs #eta;Jet #eta;Number of jets',path='JetSuspect',xbins=20,xmin=-2.5,xmax=2.5)
    GeneralGroup.defineHistogram('sus_jet_eta_77tag',title='Number of Suspect jets passing 77 tag WP vs #eta;Jet #eta;Number of jets',path='JetSuspect',xbins=20,xmin=-2.5,xmax=2.5)
    GeneralGroup.defineHistogram('sus_jet_eta_85tag',title='Number of Suspect jets passing 85 tag WP vs #eta;Jet #eta;Number of jets',path='JetSuspect',xbins=20,xmin=-2.5,xmax=2.5)
    GeneralGroup.defineHistogram('pass60e,sus_jet_eta',type='TEfficiency',title='Suspect jets 60 tag WP Efficiency vs #eta;Jet #eta;60 tag WP Efficiency',path='JetSuspect',xbins=20,xmin=-2.5,xmax=2.5)
    GeneralGroup.defineHistogram('pass70e,sus_jet_eta',type='TEfficiency',title='Suspect jets 70 tag WP Efficiency vs #eta;Jet #eta;70 tag WP Efficiency',path='JetSuspect',xbins=20,xmin=-2.5,xmax=2.5)
    GeneralGroup.defineHistogram('pass77e,sus_jet_eta',type='TEfficiency',title='Suspect jets 77 tag WP Efficiency vs #eta;Jet #eta;77 tag WP Efficiency',path='JetSuspect',xbins=20,xmin=-2.5,xmax=2.5)
    GeneralGroup.defineHistogram('pass85e,sus_jet_eta',type='TEfficiency',title='Suspect jets 85 tag WP Efficiency vs #eta;Jet #eta;85 tag WP Efficiency',path='JetSuspect',xbins=20,xmin=-2.5,xmax=2.5)

    GeneralGroup.defineHistogram('sus_jet_phi',title='Number of Suspect jets before passing tag vs #phi;Jet #phi;Number of jets',path='JetSuspect',xbins=24,xmin=-1*math.pi,xmax=math.pi)
    GeneralGroup.defineHistogram('sus_jet_phi_60tag',title='Number of Suspect jets passing 60 tag WP vs #phi;Jet #phi;Number of jets',path='JetSuspect',xbins=24,xmin=-1*math.pi,xmax=math.pi)
    GeneralGroup.defineHistogram('sus_jet_phi_70tag',title='Number of Suspect jets passing 70 tag WP vs #phi;Jet #phi;Number of jets',path='JetSuspect',xbins=24,xmin=-1*math.pi,xmax=math.pi)
    GeneralGroup.defineHistogram('sus_jet_phi_77tag',title='Number of Suspect jets passing 77 tag WP vs #phi;Jet #phi;Number of jets',path='JetSuspect',xbins=24,xmin=-1*math.pi,xmax=math.pi)
    GeneralGroup.defineHistogram('sus_jet_phi_85tag',title='Number of Suspect jets passing 85 tag WP vs #phi;Jet #phi;Number of jets',path='JetSuspect',xbins=24,xmin=-1*math.pi,xmax=math.pi)
    GeneralGroup.defineHistogram('pass60f,sus_jet_phi',type='TEfficiency',title='Suspect jets 60 tag WP Efficiency vs #phi;Jet #phi;60 tag WP Efficiency',path='JetSuspect',xbins=24,xmin=-1*math.pi,xmax=math.pi)
    GeneralGroup.defineHistogram('pass70f,sus_jet_phi',type='TEfficiency',title='Suspect jets 70 tag WP Efficiency vs #phi;Jet #phi;70 tag WP Efficiency',path='JetSuspect',xbins=24,xmin=-1*math.pi,xmax=math.pi)
    GeneralGroup.defineHistogram('pass77f,sus_jet_phi',type='TEfficiency',title='Suspect jets 77 tag WP Efficiency vs #phi;Jet #phi;77 tag WP Efficiency',path='JetSuspect',xbins=24,xmin=-1*math.pi,xmax=math.pi)
    GeneralGroup.defineHistogram('pass85f,sus_jet_phi',type='TEfficiency',title='Suspect jets 85 tag WP Efficiency vs #phi;Jet #phi;85 tag WP Efficiency',path='JetSuspect',xbins=24,xmin=-1*math.pi,xmax=math.pi)

    GeneralGroup.defineHistogram('sus_jet_eta_60tag,sus_jet_phi_60tag;sus_jet_MAP_60tag',title='2D MAP of Suspect jets passing 60 tag WP;Jet #eta;Jet #phi',type='TH2F',path='JetSuspect',xbins=25,xmin=-2.5,xmax=2.5,ybins=50,ymin=-1*math.pi,ymax=math.pi)
    GeneralGroup.defineHistogram('sus_jet_eta_70tag,sus_jet_phi_70tag;sus_jet_MAP_70tag',title='2D MAP of Suspect jets passing 70 tag WP;Jet #eta;Jet #phi',type='TH2F',path='JetSuspect',xbins=25,xmin=-2.5,xmax=2.5,ybins=50,ymin=-1*math.pi,ymax=math.pi)
    GeneralGroup.defineHistogram('sus_jet_eta_77tag,sus_jet_phi_77tag;sus_jet_MAP_77tag',title='2D MAP of Suspect jets passing 77 tag WP;Jet #eta;Jet #phi',type='TH2F',path='JetSuspect',xbins=25,xmin=-2.5,xmax=2.5,ybins=50,ymin=-1*math.pi,ymax=math.pi)
    GeneralGroup.defineHistogram('sus_jet_eta_85tag,sus_jet_phi_85tag;sus_jet_MAP_85tag',title='2D MAP of Suspect jets passing 85 tag WP;Jet #eta;Jet #phi',type='TH2F',path='JetSuspect',xbins=25,xmin=-2.5,xmax=2.5,ybins=50,ymin=-1*math.pi,ymax=math.pi)

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
    #ConfigFlags.Input.Files = ["/afs/cern.ch/work/a/alaperto/dq_test/dq_r22_ORD22/run/DQ_ARTs/myESD.data15HI.15Mar.root"] #ESD from ART test, 15 Mar 22, data15_heavy_ion
    ConfigFlags.Input.Files = ["/afs/cern.ch/work/a/alaperto/dq_test/dq_r22_ORD22/run/DQ_ARTs/myESD.data18.14Mar.root"] #ESD from ART test, 14 Mar 22, data18_13TeV
    #ConfigFlags.Input.Files = ["/eos/user/m/mtanasin/DQ_art/myESD.pool.root"] #ESD from ART test, 15 Feb 22, data18
    ConfigFlags.Input.isMC = False

    #MC r22 ART input working:
    #ConfigFlags.Input.Files = [""/afs/cern.ch/user/m/mtanasin/public/DQ/dq_devel/myAOD.pool.root""] #AOD from ART test, 15 Feb 22, data18
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
    #jetTagMonitorAcc.getEventAlgo('JetTagMonAlg').OutputLevel = 2 # DEBUG
    cfg.printConfig(withDetails=True) # set True for exhaustive info

    #Select how many events to run on 
    #use cfg.run() empty for all events
    #use cfg.run(300) to only run on first 300 events
    cfg.run(1000)
