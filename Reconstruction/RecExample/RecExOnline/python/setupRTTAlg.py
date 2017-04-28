# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

output_directory = "./"
#ref_filename = "/afs/cern.ch/user/y/yunju/working/yunju/public/GM_ref_plots/r0000284484_All_ATLAS_Global-MDA_Histogramming-Global.root"
ref_filename = "/afs/cern.ch/user/y/yunju/working/yunju/public/GM_ref_plots/r0000302347_l0420_ATLAS_Global-MDA_Histogramming-Global.root"
ref_treename = "Histogramming-Global-iss/GM-Gatherer-MIG_GlobalMonitoringSegment"
rtt_filename = "/afs/cern.ch/atlas/project/RTT/prod/Results/rtt/chainstore/21.1/x86_64-slc6-gcc62-opt/AthenaP1/Reconstruction/RecExample/RecExOnline/OfflineChainJob/Monitor.root"
#this file is only for debug
#rtt_filename = "/afs/cern.ch/user/y/yunju/ATLAS_Ana/P1MON_20.11.0.19.2/Monitor.root"
#rtt_treename = "run_284484"
rtt_treename = "run_302347"
histnames = [
    'Jets/AntiKt4EMTopoJets/pt',
    'Jets/AntiKt4EMTopoJets/eta',
    'Jets/AntiKt4EMTopoJets/phi',
    'Jets/AntiKt4EMTopoJets/M',
    'Tau/tauEt',
    'Tau/tauEta',
    'Tau/tauPhi',
    'Tau/tauCharge',
    'IDAlignMon/ExtendedTracks_NoTriggerSelection/GenericTracks/ngtracks',
    'IDAlignMon/ExtendedTracks_NoTriggerSelection/GenericTracks/Nhits_per_track',
    'IDAlignMon/ExtendedTracks_NoTriggerSelection/GenericTracks/d0',
    'IDAlignMon/ExtendedTracks_NoTriggerSelection/GenericTracks/P',
    'IDPerfMon/Wenu/NoTriggerSelection/Wenu_transmass_sel',
    'IDPerfMon/Zee/NoTriggerSelection/Zee_invmass',
    'DiMuMon/Zmumu/NoTrig/Zmumu_invmass_All',
    'DiMuMon/Jpsi/NoTrig/Jpsi_invmass_All',
    'MuonPhysics/Muons/CBMuons/Muons_CBMuons_pt',
    'MuonPhysics/Muons/CBMuons/Muons_CBMuons_eta',
    'MuonPhysics/Muons/CBMuons/Muons_CBMuons_phi', 
    'egamma/photons/photonEtCbLoose', 
    'egamma/photons/photonEtaCbLoose',
    'egamma/photons/photonPhiCbLoose',
    'egamma/electrons/electronEtLhLoose',
    'egamma/electrons/electronEtaLhLoose',
    'egamma/electrons/electronPhiLhLoose'    
    ]
size = 1000
tests = [
    'anderson_ksamp',
    'CVM_2samp',
    'ks_2samp',
    'likelihoodratio_ksamp',
    'chi2_2samp']
