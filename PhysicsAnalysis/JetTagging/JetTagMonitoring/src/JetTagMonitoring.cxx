/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//#include "ITrackToVertex/ITrackToVertex.h"
#include "xAODJet/JetContainer.h"   
#include "xAODJet/JetAttributes.h"
#include "xAODBTagging/BTagging.h"
#include "xAODEventInfo/EventInfo.h"

#include "JetTagMonitoring/JetTagMonitoring.h"
#include "JetTagTools/TrackSelector.h"

#include "xAODTracking/TrackParticle.h"
#include "xAODTracking/TrackParticleContainer.h"    

#include "TrigDecisionTool/TrigDecisionTool.h"
#include "TrkParticleBase/LinkToTrackParticleBase.h"
#include "TrkVertexFitterInterfaces/ITrackToVertexIPEstimator.h"
#include "VxSecVertex/VxSecVKalVertexInfo.h"
#include "VxSecVertex/VxSecVertexInfo.h"

#include "LArRecEvent/LArEventBitInfo.h"

#include "LWHists/LWHist.h"
#include "LWHists/TH1F_LW.h"
#include "LWHists/TH2F_LW.h"

#include "TH1F.h"
#include "TH2F.h"
#include "TMath.h"

#include <vector>
#include <string>
#include <sstream>

//** --------------------------------------------------------------------------------------------------------------- **//

// Private methods

namespace {

template< class T >
bool retrieveCollection(const ServiceHandle<StoreGateSvc>& sg, const DataHandle<T>& coll, std::string key ) {

    StatusCode sc;
    sc = sg->retrieve( coll, key );

    if (!sc.isSuccess())
        return false;

    return true;
}

} // unnamed namespace


//** --------------------------------------------------------------------------------------------------------------- **//


JetTagMonitoring::JetTagMonitoring(const std::string & type, const std::string & name, const IInterface* parent) :
    ManagedMonitorToolBase(type, name, parent),
    m_storeGate( "StoreGateSvc", name ),
    m_trackSelectorTool("Analysis::TrackSelector"),
    m_trackToVertexTool("Reco::TrackToVertex"),
    m_histogramsCreated(false),
    m_switch_off(false),
    m_pixelCondSummarySvc("PixelConditionsSummarySvc",name)
{

    declareProperty("JetContainer",           m_jetName           = "AntiKt4TopoJets");
    declareProperty("TrackParticleContainer", m_trackParticleName = "TrackParticleCandidate");
    declareProperty("PrimaryVertexContainer", m_primaryVertexName = "VxPrimaryCandidate");

    declareProperty("DQcuts", m_do_cuts = true);

    declareProperty("PriVtxTrkMinCut", m_pri_vtx_trk_min_cut = 4 );
    declareProperty("D0_min_cuts", m_trk_d0_min_cut = 0);
    declareProperty("D0_max_cuts", m_trk_d0_max_cut = 1);
    declareProperty("JetPt_cuts", m_jet_pt_cut = 15e3);
    declareProperty("JetEta_cuts", m_jet_eta_cut = 2.5);
    declareProperty("nTrk_cuts", m_trk_n = 1);
    declareProperty("SV0_weight_cut", m_sv0_weight_cut=0.);
    declareProperty("SV1_weight_cut", m_sv1_weight_cut=0.);
    declareProperty("SV2_weight_cut", m_sv2_weight_cut=0.);
    declareProperty("IP1D_weight_cut", m_ip1d_weight_cut=0.);
    declareProperty("IP2D_weight_cut", m_ip2d_weight_cut=0.);
    declareProperty("IP3D_weight_cut", m_ip3d_weight_cut=0.);
    declareProperty("SV1IP3D_weight_cut", m_sv1ip3d_weight_cut=0.);
    declareProperty("JetFitterCOMBNN_weight_cut", m_jfcnn_weight_cut=0.);
    declareProperty("MV1_weight_cut", m_mv1_weight_cut=0.601713);
    declareProperty("UseTrackSelector", m_use_trackselector = true);
    declareProperty("UseTrigDecisionTool", m_use_trigdectool = true);
    declareProperty("doLumiBlock",     m_doLumiBlock=false);
    declareProperty("doStatus",        m_doStatus = false);

    m_isNewLumiBlock = false;
}

//** --------------------------------------------------------------------------------------------------------------- **//


JetTagMonitoring::~JetTagMonitoring() {}


//** --------------------------------------------------------------------------------------------------------------- **//


StatusCode JetTagMonitoring::registerHist(MonGroup& theGroup, TH1* h1) {

    msg(MSG::VERBOSE) << "in JetTagMonitoring::registerHist " << h1->GetName() << endreq;

    StatusCode sc = theGroup.regHist(h1);
    if (!sc.isSuccess())
        msg(MSG::WARNING) << "Could not register histogram " << endreq;

    return sc;
}

//** --------------------------------------------------------------------------------------------------------------- **//


StatusCode JetTagMonitoring::registerHist(MonGroup& theGroup, LWHist* h1) {

    msg(MSG::VERBOSE) << "in JetTagMonitoring::registerHist " << h1->GetName() << endreq;

    StatusCode sc = theGroup.regHist(h1);
    if (!sc.isSuccess())
        msg(MSG::WARNING) << "Could not register histogram " << endreq;

    return sc;
}


//** --------------------------------------------------------------------------------------------------------------- **//





//** --------------------------------------------------------------------------------------------------------------- **//




StatusCode JetTagMonitoring::initialize() {

    msg(MSG::INFO) << "in JetTagMonitoring::initialize" << endreq;

    StatusCode sc = ManagedMonitorToolBase::initialize();
    if (!sc.isSuccess()) {
        msg(MSG::WARNING) << "Unable to initialize ManagedMonitorToolBase" << endreq;
        return sc;
    }

    sc = m_storeGate.retrieve();
    if (!sc.isSuccess()) {
        msg(MSG::WARNING) << "Unable to retrieve StoreGateSvc handle" << endreq;
        m_switch_off = true;
    }

    if ( m_use_trackselector ) {
        if ( m_trackSelectorTool.retrieve().isFailure() ) {
            msg(MSG::WARNING) << "Failed to retrieve tool " << m_trackSelectorTool << endreq;
            m_switch_off = true;
        }
    } else {
        msg(MSG::INFO) << "Analysis::TrackSelector not used" << endreq;
    }

    if ( m_switch_off )
        msg(MSG::WARNING) << "Switching off JetTagMonitoring::fillHistograms()" << endreq;

    m_lumiBlockNum = 0;

    return StatusCode::SUCCESS;
}


//** --------------------------------------------------------------------------------------------------------------- **//


StatusCode JetTagMonitoring::bookHistograms() {

    msg(MSG::DEBUG) << "bookHistograms()" << endreq;

    m_isNewLumiBlock = newLumiBlock;
    if ( !newRun ) {
        // Only book run-length histogram
        return StatusCode::SUCCESS;
    }


    if ( (m_environment != AthenaMonManager::tier0) &&
            (m_environment != AthenaMonManager::tier0ESD) &&
            (m_environment != AthenaMonManager::online) ) {
        // Only produce histograms from ESD, or from RAW if ESD-only monitoring is not enabled
        msg(MSG::INFO) << "Monitoring environment is \'" << m_environment << "\', not booking histograms" << endreq;
        return StatusCode::SUCCESS;
    }

    if ( m_dataType == AthenaMonManager::cosmics ) {
        msg(MSG::INFO) << "Cosmic-ray data, not booking histograms" << endreq;
        return StatusCode::SUCCESS;
    }

    msg(MSG::INFO) << "Monitoring environment is \'" << m_environment << "\', data type is \'" << m_dataType
                   << "\', booking histograms now." << endreq;


    m_monGr_shift = new MonGroup( this, "JetTagging", run );
    m_monGr_LowStat = new MonGroup( this, "JetTagging", lowStat );

    registerHist(*m_monGr_shift, m_tag_ip2d_w  = TH1F_LW::create("tag_IP2D_w","IP2D weight",125,-25.,40.));
    registerHist(*m_monGr_shift, m_tag_ip2d_n  = TH1F_LW::create("tag_IP2D_n","IP2D # of tracks",20,0.,20.));
    registerHist(*m_monGr_shift, m_tag_ip2d_b  = TH1F_LW::create("tag_IP2D_b","IP2D Pb",10,0.,1.));
    registerHist(*m_monGr_shift, m_tag_ip2d_u  = TH1F_LW::create("tag_IP2D_u","IP2D Pu",10,0.,1.));
    registerHist(*m_monGr_shift, m_tag_ip3d_w  = TH1F_LW::create("tag_IP3D_w","IP3D weight",195,-25.,40.));
    registerHist(*m_monGr_shift, m_tag_ip3d_n  = TH1F_LW::create("tag_IP3D_n","IP3D # of tracks",20,0.,20.));
    registerHist(*m_monGr_shift, m_tag_ip3d_b  = TH1F_LW::create("tag_IP3D_b","IP3D Pb",10,0.,1.));
    registerHist(*m_monGr_shift, m_tag_ip3d_u  = TH1F_LW::create("tag_IP3D_u","IP3D Pu",10,0.,1.));
    registerHist(*m_monGr_shift, m_tag_sv0_w   = TH1F_LW::create("tag_SV0_w","SV0 weight",120,-20.,40.));
    registerHist(*m_monGr_shift, m_tag_sv1_w   = TH1F_LW::create("tag_SV1_w","SV1 weight",120,-20.,40.));
    registerHist(*m_monGr_shift, m_tag_sv2_w   = TH1F_LW::create("tag_SV2_w","SV2 weight",120,-20.,40.));
    registerHist(*m_monGr_shift, m_tag_lf2d_w  = TH1F_LW::create("tag_LF2D_w","LifeTime2D weight",120,-20.,40.));
    registerHist(*m_monGr_shift, m_tag_svbu_w  = TH1F_LW::create("tag_SVBU_w","SecVtxTagBU weight",120,-20.,40.));
    registerHist(*m_monGr_shift, m_tag_lhsig_w = TH1F_LW::create("tag_LHSIG_w","lhSig weight",160,-120.,120.));
    registerHist(*m_monGr_shift, m_tag_softm_w = TH1F_LW::create("tag_SOFTM_w","SoftMuonTag weight",120,-20.,40.));
    registerHist(*m_monGr_shift, m_tag_comb_w  = TH1F_LW::create("tag_COMB_w","Combined weight IP3D+SV1",120,-2.,10.));
    registerHist(*m_monGr_shift, m_tag_jfcnn_w  = TH1F_LW::create("tag_JFCNN_w","Combined weight JetFitterCOMBNN",120,-20.,40.));
    registerHist(*m_monGr_shift, m_tag_mv1_w  = TH1F_LW::create("tag_MV1_w","Combined weight MV1",210,-1.,1.1));

    /*
      registerHist(*m_monGr_shift, m_tag_sv0_w_sj   = TH1F_LW::create("tag_SV0_w_sj","SV0 weight Suspect Jets",120,-20.,40.));
      registerHist(*m_monGr_shift, m_tag_comb_w_sj  = TH1F_LW::create("tag_COMB_w_sj","Combined weight IP3D+SV1 Suspect Jets",120,-20.,40.));
      registerHist(*m_monGr_shift, m_tag_jfcnn_w_sj  = TH1F_LW::create("tag_JFCNN_w_sj","Combined weight JetFitterCOMBNN Suspect Jets",120,-20.,40.));

      registerHist(*m_monGr_shift, m_tag_sv0_w_DMZ1    = TH1F_LW::create("tag_SV0_w_DMZ1","SV0 weight DMZ1",120,-20.,40.));
      registerHist(*m_monGr_shift, m_tag_comb_w_DMZ1   = TH1F_LW::create("tag_COMB_w_DMZ1","Combined weight IP3D+SV1 DMZ1",120,-20.,40.));
      registerHist(*m_monGr_shift, m_tag_jfcnn_w_DMZ1  = TH1F_LW::create("tag_JFCNN_w_DMZ1","Combined weight JetFitterCOMBNN DMZ1",120,-20.,40.));
      registerHist(*m_monGr_shift, m_tag_ip3d_w_DMZ1  = TH1F_LW::create("tag_IP3D_w_DMZ1","Combined weight IP3D DMZ1",120,-21.,40.));

      registerHist(*m_monGr_shift, m_tag_sv0_w_DMZ2    = TH1F_LW::create("tag_SV0_w_DMZ2","SV0 weight DMZ2",120,-20.,40.));
      registerHist(*m_monGr_shift, m_tag_comb_w_DMZ2   = TH1F_LW::create("tag_COMB_w_DMZ2","Combined weight IP3D+SV1 DMZ2",120,-20.,40.));
      registerHist(*m_monGr_shift, m_tag_jfcnn_w_DMZ2  = TH1F_LW::create("tag_JFCNN_w_DMZ2","Combined weight JetFitterCOMBNN DMZ2",120,-20.,40.));

      registerHist(*m_monGr_shift, m_tag_ip3d_w_DMZ2  = TH1F_LW::create("tag_IP3D_w_DMZ2","Combined weight IP3D DMZ2",120,-21.,40.));
      registerHist(*m_monGr_shift, m_tag_ip3d_w_DMZ2_x1  = TH1F_LW::create("tag_IP3D_w_DMZ2_x1","Combined weight IP3D DMZ2 x1",120,-21.,40.));
      registerHist(*m_monGr_shift, m_tag_ip3d_w_DMZ2_x2  = TH1F_LW::create("tag_IP3D_w_DMZ2_x2","Combined weight IP3D DMZ2 x2",120,-21.,40.));
      registerHist(*m_monGr_shift, m_tag_ip3d_w_DMZ2_x4  = TH1F_LW::create("tag_IP3D_w_DMZ2_x4","Combined weight IP3D DMZ2 x4",120,-21.,40.));
      registerHist(*m_monGr_shift, m_tag_ip3d_w_DMZ2_x8  = TH1F_LW::create("tag_IP3D_w_DMZ2_x8","Combined weight IP3D DMZ2 x8",120,-21.,40.));

      registerHist(*m_monGr_shift, m_tag_sv0_w_DMZ3    = TH1F_LW::create("tag_SV0_w_DMZ3","SV0 weight DMZ3",120,-20.,40.));
      registerHist(*m_monGr_shift, m_tag_comb_w_DMZ3   = TH1F_LW::create("tag_COMB_w_DMZ3","Combined weight IP3D+SV1 DMZ3",120,-20.,40.));
      registerHist(*m_monGr_shift, m_tag_jfcnn_w_DMZ3  = TH1F_LW::create("tag_JFCNN_w_DMZ3","Combined weight JetFitterCOMBNN DMZ3",120,-20.,40.));

      registerHist(*m_monGr_shift, m_tag_ip3d_w_DMZ3  = TH1F_LW::create("tag_IP3D_w_DMZ3","Combined weight IP3D DMZ3",120,-21.,40.));
      registerHist(*m_monGr_shift, m_tag_ip3d_w_DMZ3_x1  = TH1F_LW::create("tag_IP3D_w_DMZ3_x1","Combined weight IP3D DMZ3 x1",120,-21.,40.));
      registerHist(*m_monGr_shift, m_tag_ip3d_w_DMZ3_x2  = TH1F_LW::create("tag_IP3D_w_DMZ3_x2","Combined weight IP3D DMZ3 x2",120,-21.,40.));
      registerHist(*m_monGr_shift, m_tag_ip3d_w_DMZ3_x4  = TH1F_LW::create("tag_IP3D_w_DMZ3_x4","Combined weight IP3D DMZ3 x4",120,-21.,40.));
      registerHist(*m_monGr_shift, m_tag_ip3d_w_DMZ3_x8  = TH1F_LW::create("tag_IP3D_w_DMZ3_x8","Combined weight IP3D DMZ3 x8",120,-21.,40.));
    */

    registerHist(*m_monGr_shift, m_tag_trkcnt_w =
                     TH1F_LW::create("tag_TrkCnt_w","TrackCounting2D",120,-20.,40.));
    registerHist(*m_monGr_shift, m_tag_jetprob_w =
                     TH1F_LW::create("tag_JetProb_w","JetProb",120,0.,1.01));

    registerHist(*m_monGr_shift, m_jet_n =
                     TH1F_LW::create("jet_n","number of jets",20,0.,20.));

    registerHist(*m_monGr_shift, m_global_nPrimVtx =
                     TH1F_LW::create("global_nPrimVtx","# primary vertex",30,0.,30.));
    registerHist(*m_monGr_shift, m_global_xPrimVtx =
                     TH1F_LW::create("global_xPrimVtx","x primary vertex",100,-0.5,0.5));
    registerHist(*m_monGr_shift, m_global_yPrimVtx =
                     TH1F_LW::create("global_yPrimVtx","y primary vertex",100,-1.2,1.2));
    registerHist(*m_monGr_shift, m_global_zPrimVtx =
                     TH1F_LW::create("global_zPrimVtx","z primary vertex",100,-250.,250.));

    registerHist(*m_monGr_shift, m_global_BLayerHits =
                     TH1F_LW::create("global_BLayerHits","# of BLayer hits on TrackParticle",5,0.,5.));
    registerHist(*m_monGr_shift, m_global_PixelHits =
                     TH1F_LW::create("global_PixelHits","# of Pixel hits on TrackParticle",10,0.,10.));
    registerHist(*m_monGr_shift, m_global_SCTHits =
                     TH1F_LW::create("global_SCTHits","# of SCT hits on TrackParticle",15,0.,15.));
    registerHist(*m_monGr_shift, m_global_SiHits =
                     TH1F_LW::create("global_SiHits","# of pixel+SCT hits on TrackParticle",25,0.,25.));
    registerHist(*m_monGr_shift, m_global_TRTHits =
                     TH1F_LW::create("global_TRTHits","# of TRT hits on TrackParticle",100,0.,100.));

    registerHist(*m_monGr_shift, m_trackParticle_n = TH1F_LW::create("NTrackParticle","number of TrackParticles",100,0.,1000.));

    registerHist(*m_monGr_shift, m_jet_nTag = TH1F_LW::create("jet_nTag","# tags",20,0.,20.));
    registerHist(*m_monGr_shift, m_jet_eta = TH1F_LW::create("jet_eta","Jet eta",100,-5.,5.));
    registerHist(*m_monGr_shift, m_jet_phi = TH1F_LW::create("jet_phi","Jet phi",100,-3.15,3.15));
    registerHist(*m_monGr_shift, m_jet_et  = TH1F_LW::create("jet_et","Jet et",100,0.,500000.));

    registerHist(*m_monGr_shift, m_jet_tracks_n =
                     TH1F_LW::create("jet_tracks_n","# of tracks in a jet",50,0.,50.));
    registerHist(*m_monGr_shift, m_jet_tracks_pt =
                     TH1F_LW::create("jet_tracks_pt","pT of tracks in a jet",100,0.,100.));
    registerHist(*m_monGr_shift, m_jet_tracks_eta =
                     TH1F_LW::create("jet_tracks_eta","#eta of tracks in a jet",100,-2.5,2.5));
    registerHist(*m_monGr_shift, m_jet_tracks_phi =
                     TH1F_LW::create("jet_tracks_phi","#varphi of tracks in a jet",100,-TMath::Pi(),TMath::Pi()));
    registerHist(*m_monGr_shift, m_jet_tracks_BLayerHits =
                     TH1F_LW::create("jet_tracks_hits_BLayer","# of BLayer hits per track in a jet",5,0.,5.));
    registerHist(*m_monGr_shift, m_jet_tracks_PixelHits =
                     TH1F_LW::create("jet_tracks_hits_Pixel","# of Pixel hits per track in a jet",10,0.,10.));
    registerHist(*m_monGr_shift, m_jet_tracks_SCTHits =
                     TH1F_LW::create("jet_tracks_hits_SCT","# of SCT hits per track in a jet",15,0.,15.));
    registerHist(*m_monGr_shift, m_jet_muons_n =
                     TH1F_LW::create("jet_muons_n","# of muons in a jet",10,0.,10.));
    registerHist(*m_monGr_shift, m_jet_muons_pt =
                     TH1F_LW::create("jet_muons_pt","pT of muons in a jet",100,0.,100.));
    registerHist(*m_monGr_shift, m_jet_electrons_n =
                     TH1F_LW::create("jet_electrons_n","# of electrons in a jet",10,0.,10.));
    registerHist(*m_monGr_shift, m_jet_electrons_pt =
                     TH1F_LW::create("jet_electrons_pt","pT of electrons in a jet",100,0.,100.));

    registerHist(*m_monGr_shift, m_trigPassed =
                     TH1F_LW::create("trigPassed","Number of events passed trigger chains",25,0.,25.));

  
        m_trigPassed->GetXaxis()->SetBinLabel(1,"L1_J10") ;
        m_trigPassed->GetXaxis()->SetBinLabel(2,"L1_J15") ;
               m_trigPassed->GetXaxis()->SetBinLabel(3,"L1_J20")  ;
                      m_trigPassed->GetXaxis()->SetBinLabel(4,"L1_J30") ;
                                        m_trigPassed->GetXaxis()->SetBinLabel(5,"L1_J40")  ;
                                        m_trigPassed->GetXaxis()->SetBinLabel(6,"L1_J50")  ;
                      m_trigPassed->GetXaxis()->SetBinLabel(7,"L1_J75")  ;
                                        m_trigPassed->GetXaxis()->SetBinLabel(8,"L1_J100") ;
                                         m_trigPassed->GetXaxis()->SetBinLabel(9,"L1_J350");
    
    m_trigPassed->GetXaxis()->SetBinLabel(10,"L1_3J10");
    m_trigPassed->GetXaxis()->SetBinLabel(11,"L1_3J15");
    m_trigPassed->GetXaxis()->SetBinLabel(12,"L1_3J20");
    m_trigPassed->GetXaxis()->SetBinLabel(13,"L1_3J50");
    m_trigPassed->GetXaxis()->SetBinLabel(14,"L1_3J75");
    m_trigPassed->GetXaxis()->SetBinLabel(15,"L1_4J10");
    m_trigPassed->GetXaxis()->SetBinLabel(16," L1_4J15");
    m_trigPassed->GetXaxis()->SetBinLabel(17,"L1_4J30");
    m_trigPassed->GetXaxis()->SetBinLabel(18,"L1_5J10");
    m_trigPassed->GetXaxis()->SetBinLabel(19,"L1_5J20");
   
    m_trigPassed->GetXaxis()->SetBinLabel(20,"EF_mu4_L1J10_matched");
    m_trigPassed->GetXaxis()->SetBinLabel(21,"EF_e24vh_medium1");
    m_trigPassed->GetXaxis()->SetBinLabel(22,"EF_mu18_IDTrkNoCut_tight");
   
    m_trigPassed->GetXaxis()->SetBinLabel(23,"L1_RD0_FILLED");


    registerHist(*m_monGr_shift, m_cutflow =
                     TH1F_LW::create("DQ_Cutflow","Number of events passing DQcuts",10,-0.5,9.5));

    m_cutflow->GetXaxis()->SetBinLabel(1,"All");
    m_cutflow->GetXaxis()->SetBinLabel(2,"Good LAr");
    m_cutflow->GetXaxis()->SetBinLabel(3,"PV present");
    m_cutflow->GetXaxis()->SetBinLabel(4,"PV ntrk");

    registerHist(*m_monGr_shift, m_priVtx_trks = TH2F_LW::create("priVtx_trks","Tracks PV",21,-0.5,20.5,21,-0.5,20.5));

    m_priVtx_trks->GetXaxis()->SetTitle("Primary Vertex 0");
    m_priVtx_trks->GetYaxis()->SetTitle("Primary Vertex 1");




    registerHist(*m_monGr_shift, m_cutflow_jet =
                     TH1F_LW::create("Jet_Cutflow","Number of jets passed DQcuts",10,-0.5,9.5));

    m_cutflow_jet->GetXaxis()->SetBinLabel(1,"All");
    m_cutflow_jet->GetXaxis()->SetBinLabel(2,"isGoodJet");
    m_cutflow_jet->GetXaxis()->SetBinLabel(3,"Kinematic Cuts");
    m_cutflow_jet->GetXaxis()->SetBinLabel(4,"Taggable");

    registerHist(*m_monGr_shift, m_track_selector_eff = TH2F_LW::create("track_selector_eff", "Efficiency of Track Selector Tool;#eta;#phi", 25, -2.5, 2.5, 25, -TMath::Pi(), TMath::Pi()));
    registerHist(*m_monGr_shift, m_track_selector_all = TH2F_LW::create("track_selector_all", "Efficiency of Track Selector Tool;#eta;#phi", 25, -2.5, 2.5, 25, -TMath::Pi(), TMath::Pi()));
    //registerHist(*m_monGr_shift, m_track_selector_suspect = TH2F_LW::create("track_selector_suspect", "Efficiency of Track Selector Tool (Suspect Jets)", 25, -2.5, 2.5, 25, -TMath::Pi(), TMath::Pi()));

    //2D plots
    //secondary vertex based
    registerHist(*m_monGr_shift, m_jet_2D_all = TH2F_LW::create("jet_2D_all", "Jet 2D plot (No Cuts);#eta;#phi", 25, -2.5, 2.5, 25, -TMath::Pi(), TMath::Pi()));
    registerHist(*m_monGr_shift, m_jet_2D_good = TH2F_LW::create("jet_2D_good", "Jet 2D plot (Good Jets);#eta;#phi", 25, -2.5, 2.5, 25, -TMath::Pi(), TMath::Pi()));
    registerHist(*m_monGr_shift, m_jet_2D_kinematic = TH2F_LW::create("jet_2D_kinematic", "Jet 2D plot (Kinematic Cuts);#eta;#phi", 25, -2.5, 2.5, 25, -TMath::Pi(), TMath::Pi()));
    registerHist(*m_monGr_shift, m_jet_2D_quality = TH2F_LW::create("jet_2D_quality", "Jet 2D plot (Quality Cuts);#eta;#phi", 25, -2.5, 2.5, 25, -TMath::Pi(), TMath::Pi()));
    //registerHist(*m_monGr_shift, m_jet_2D_suspect = TH2F_LW::create("jet_2D_suspect", "Jet 2D plot (Suspect Jets)", 25, -2.5, 2.5, 25, -TMath::Pi(), TMath::Pi()));

    registerHist(*m_monGr_shift, m_sv0_tag_pos_rate_2D = TH2F_LW::create("sv0_tag_pos_rate_2D", "SV0 Tag Positive Rate;#eta;#phi", 25, -2.5, 2.5, 25, -TMath::Pi(), TMath::Pi()));

    registerHist(*m_monGr_shift, m_sv0_tag_neg_rate_2D = TH2F_LW::create("sv0_tag_neg_rate_2D", "SV0 Tag Negative Rate;#eta;#phi", 25, -2.5, 2.5, 25, -TMath::Pi(), TMath::Pi()));

    registerHist(*m_monGr_shift, m_sv1_tag_pos_rate_2D = TH2F_LW::create("sv1_tag_pos_rate_2D", "SV1 Tag Positive Rate;#eta;#phi", 25, -2.5, 2.5, 25, -TMath::Pi(), TMath::Pi()));

    registerHist(*m_monGr_shift, m_sv1_tag_neg_rate_2D = TH2F_LW::create("sv1_tag_neg_rate_2D", "SV1 Tag Negative Rate;#eta;#phi", 25, -2.5, 2.5, 25, -TMath::Pi(), TMath::Pi()));

    registerHist(*m_monGr_shift, m_sv2_tag_pos_rate_2D = TH2F_LW::create("sv2_tag_pos_rate_2D", "SV2 Tag Positive Rate;#eta;#phi", 25, -2.5, 2.5, 25, -TMath::Pi(), TMath::Pi()));

    registerHist(*m_monGr_shift, m_sv2_tag_neg_rate_2D = TH2F_LW::create("sv2_tag_neg_rate_2D", "SV2 Tag Negative Rate;#eta;#phi", 25, -2.5, 2.5, 25, -TMath::Pi(), TMath::Pi()));

    //impact parameter based
    registerHist(*m_monGr_shift, m_ip2d_tag_pos_rate_2D = TH2F_LW::create("ip2d_tag_pos_rate_2D", "IP2D Tag Positive Rate;#eta;#phi", 25, -2.5, 2.5, 25, -TMath::Pi(), TMath::Pi()));
    registerHist(*m_monGr_shift, m_ip2d_tag_neg_rate_2D = TH2F_LW::create("ip2d_tag_neg_rate_2D", "IP2D Tag Negative Rate;#eta;#phi", 25, -2.5, 2.5, 25, -TMath::Pi(), TMath::Pi()));
    registerHist(*m_monGr_shift, m_ip3d_tag_pos_rate_2D = TH2F_LW::create("ip3d_tag_pos_rate_2D", "IP3D Tag Positive Rate;#eta;#phi", 25, -2.5, 2.5, 25, -TMath::Pi(), TMath::Pi()));
    registerHist(*m_monGr_shift, m_ip3d_tag_neg_rate_2D = TH2F_LW::create("ip3d_tag_neg_rate_2D", "IP3D Tag Negative Rate;#eta;#phi", 25, -2.5, 2.5, 25, -TMath::Pi(), TMath::Pi()));

    registerHist(*m_monGr_shift, m_ip3d_tag_def_rate_2D = TH2F_LW::create("ip3d_tag_def_rate_2D", "IP3D Tag Default Rate", 25, -2.5, 2.5, 25, -TMath::Pi(), TMath::Pi()));

    //MV1 based
    registerHist(*m_monGr_shift, m_mv1_tag_pos_rate_2D = TH2F_LW::create("mv1_tag_pos_rate_2D", "MV1 Tag Positive Rate;#eta;#phi", 25, -2.5, 2.5, 25, -TMath::Pi(), TMath::Pi()));
    registerHist(*m_monGr_shift, m_mv1_tag_neg_rate_2D = TH2F_LW::create("mv1_tag_neg_rate_2D", "MV1 Tag Negative Rate;#eta;#phi", 25, -2.5, 2.5, 25, -TMath::Pi(), TMath::Pi()));
    registerHist(*m_monGr_shift, m_mv1_tag_def_rate_2D = TH2F_LW::create("mv1_tag_def_rate_2D", "MV1 Tag Default Rate;#eta;#phi", 25, -2.5, 2.5, 25, -TMath::Pi(), TMath::Pi()));
    registerHist(*m_monGr_shift, m_mv1_tag_def_rate_2D_LS = TH2F_LW::create("mv1_tag_def_rate_2D_LS", "MV1 Tag Default Rate;#eta;#phi", 25, -2.5, 2.5, 25, -TMath::Pi(), TMath::Pi()));

    registerHist(*m_monGr_shift, m_ip3d_tag_def_rate_2D = TH2F_LW::create("ip3d_tag_def_rate_2D", "IP3D Tag Default Rate", 25, -2.5, 2.5, 25, -TMath::Pi(), TMath::Pi()));

    registerHist(*m_monGr_shift, m_sv1ip3d_tag_pos_rate_2D = TH2F_LW::create("sv1ip3d_tag_pos_rate_2D", "SV1+IP3D Tag Positive Rate;#eta;#phi", 25, -2.5, 2.5, 25, -TMath::Pi(), TMath::Pi()));
    registerHist(*m_monGr_shift, m_sv1ip3d_tag_neg_rate_2D = TH2F_LW::create("sv1ip3d_tag_neg_rate_2D", "SV1+IP3D Tag Negative Rate;#eta;#phi", 25, -2.5, 2.5, 25, -TMath::Pi(), TMath::Pi()));

    registerHist(*m_monGr_shift, m_jfcnn_tag_pos_rate_2D = TH2F_LW::create("jfcnn_tag_pos_rate_2D", "JetFitterCOMBNN Tag Positive Rate;#eta;#phi", 25, -2.5, 2.5, 25, -TMath::Pi(), TMath::Pi()));
    registerHist(*m_monGr_shift, m_jfcnn_tag_neg_rate_2D = TH2F_LW::create("jfcnn_tag_neg_rate_2D", "JetFitterCOMBNN Tag Negative Rate;#eta;#phi", 25, -2.5, 2.5, 25, -TMath::Pi(), TMath::Pi()));

    registerHist(*m_monGr_shift, m_tracks_all_2D = TH2F_LW::create("tracks_all_2D", "All Tracks", 25, -2.5, 2.5, 25, -TMath::Pi(), TMath::Pi()));
    registerHist(*m_monGr_shift, m_tracks_pTMin_2D = TH2F_LW::create("tracks_pTMin_2D", "Tracks Failed pTMin Cut;#eta;#phi", 25, -2.5, 2.5, 25, -TMath::Pi(), TMath::Pi()));
    registerHist(*m_monGr_shift, m_tracks_d0Max_2D = TH2F_LW::create("tracks_d0Max_2D", "Tracks Failed d0Max Cut;#eta;#phi", 25, -2.5, 2.5, 25, -TMath::Pi(), TMath::Pi()));
    registerHist(*m_monGr_shift, m_tracks_z0Max_2D = TH2F_LW::create("tracks_z0Max_2D", "Tracks Failed z0Max Cut;#eta;#phi", 25, -2.5, 2.5, 25, -TMath::Pi(), TMath::Pi()));
    registerHist(*m_monGr_shift, m_tracks_sigd0Max_2D = TH2F_LW::create("tracks_sigd0Max_2D", "Tracks Failed sigd0Max Cut;#eta;#phi", 25, -2.5, 2.5, 25, -TMath::Pi(), TMath::Pi()));
    registerHist(*m_monGr_shift, m_tracks_sigz0Max_2D = TH2F_LW::create("tracks_sigz0Max_2D", "Tracks Failed sigz0Max Cut;#eta;#phi", 25, -2.5, 2.5, 25, -TMath::Pi(), TMath::Pi()));
    registerHist(*m_monGr_shift, m_tracks_etaMax_2D = TH2F_LW::create("tracks_etaMax_2D", "Tracks Failed etaMax Cut;#eta;#phi", 25, -2.5, 2.5, 25, -TMath::Pi(), TMath::Pi()));
    registerHist(*m_monGr_shift, m_tracks_nHitBLayer_2D = TH2F_LW::create("tracks_nHitBLayer_2D", "Tracks Failed nHitBLayer Cut;#eta;#phi", 25, -2.5, 2.5, 25, -TMath::Pi(), TMath::Pi()));
    registerHist(*m_monGr_shift, m_tracks_deadBLayer_2D = TH2F_LW::create("tracks_deadBLayer_2D", "Tracks Failed deadBLayer Cut;#eta;#phi", 25, -2.5, 2.5, 25, -TMath::Pi(), TMath::Pi()));
    registerHist(*m_monGr_shift, m_tracks_nHitPix_2D = TH2F_LW::create("tracks_nHitPix_2D", "Tracks Failed nHitPix Cut;#eta;#phi", 25, -2.5, 2.5, 25, -TMath::Pi(), TMath::Pi()));
    registerHist(*m_monGr_shift, m_tracks_nHitSct_2D = TH2F_LW::create("tracks_nHitSct_2D", "Tracks Failed nHitSct Cut;#eta;#phi", 25, -2.5, 2.5, 25, -TMath::Pi(), TMath::Pi()));
    registerHist(*m_monGr_shift, m_tracks_nHitSi_2D = TH2F_LW::create("tracks_nHitSi_2D", "Tracks Failed nHitSi Cut;#eta;#phi", 25, -2.5, 2.5, 25, -TMath::Pi(), TMath::Pi()));
    registerHist(*m_monGr_shift, m_tracks_nHitTrt_2D = TH2F_LW::create("tracks_nHitTrt_2D", "Tracks Failed nHitTrt Cut;#eta;#phi", 25, -2.5, 2.5, 25, -TMath::Pi(), TMath::Pi()));
    registerHist(*m_monGr_shift, m_tracks_nHitTrtHighE_2D = TH2F_LW::create("tracks_nHitTrtHighE_2D", "Tracks Failed nHitTrtHighE Cut;#eta;#phi", 25, -2.5, 2.5, 25, -TMath::Pi(), TMath::Pi()));
    registerHist(*m_monGr_shift, m_tracks_fitChi2_2D = TH2F_LW::create("tracks_fitChi2_2D", "Tracks Failed fitChi2 Cut;#eta;#phi", 25, -2.5, 2.5, 25, -TMath::Pi(), TMath::Pi()));
    registerHist(*m_monGr_shift, m_tracks_fitProb_2D = TH2F_LW::create("tracks_fitProb_2D", "Tracks Failed fitProb Cut;#eta;#phi", 25, -2.5, 2.5, 25, -TMath::Pi(), TMath::Pi()));
    registerHist(*m_monGr_shift, m_tracks_fitChi2OnNdfMax_2D = TH2F_LW::create("tracks_fitChi2OnNdfMax_2D", "Tracks Failed fitChi2OnNdfMax Cut;#eta;#phi", 25, -2.5, 2.5, 25, -TMath::Pi(), TMath::Pi()));


    /*
    Starting to register LowStat histograms:
    */
    if ( newRun || newLowStatInterval ) {
        registerHist(*m_monGr_LowStat, m_tracks_all_2D_LS = TH2F_LW::create("tracks_all_2D_LS", "All Tracks;#eta;#phi", 25, -2.5, 2.5, 25, -TMath::Pi(), TMath::Pi()));
        registerHist(*m_monGr_LowStat, m_tracks_pTMin_2D_LS = TH2F_LW::create("tracks_pTMin_2D_LS", "Tracks Failed pTMin Cut;#eta;#phi", 25, -2.5, 2.5, 25, -TMath::Pi(), TMath::Pi()));
        registerHist(*m_monGr_LowStat, m_tracks_d0Max_2D_LS = TH2F_LW::create("tracks_d0Max_2D_LS", "Tracks Failed d0Max Cut;#eta;#phi", 25, -2.5, 2.5, 25, -TMath::Pi(), TMath::Pi()));
        registerHist(*m_monGr_LowStat, m_tracks_z0Max_2D_LS = TH2F_LW::create("tracks_z0Max_2D_LS", "Tracks Failed z0Max Cut;#eta;#phi", 25, -2.5, 2.5, 25, -TMath::Pi(), TMath::Pi()));
        registerHist(*m_monGr_LowStat, m_tracks_sigd0Max_2D_LS = TH2F_LW::create("tracks_sigd0Max_2D_LS", "Tracks Failed sigd0Max Cut;#eta;#phi", 25, -2.5, 2.5, 25, -TMath::Pi(), TMath::Pi()));
        registerHist(*m_monGr_LowStat, m_tracks_sigz0Max_2D_LS = TH2F_LW::create("tracks_sigz0Max_2D_LS", "Tracks Failed sigz0Max Cut;#eta;#phi", 25, -2.5, 2.5, 25, -TMath::Pi(), TMath::Pi()));
        registerHist(*m_monGr_LowStat, m_tracks_etaMax_2D_LS = TH2F_LW::create("tracks_etaMax_2D_LS", "Tracks Failed etaMax Cut;#eta;#phi", 25, -2.5, 2.5, 25, -TMath::Pi(), TMath::Pi()));
        registerHist(*m_monGr_LowStat, m_tracks_nHitBLayer_2D_LS = TH2F_LW::create("tracks_nHitBLayer_2D_LS", "Tracks Failed nHitBLayer Cut;#eta;#phi", 25, -2.5, 2.5, 25, -TMath::Pi(), TMath::Pi()));
        registerHist(*m_monGr_LowStat, m_tracks_deadBLayer_2D_LS = TH2F_LW::create("tracks_deadBLayer_2D_LS", "Tracks Failed deadBLayer Cut;#eta;#phi", 25, -2.5, 2.5, 25, -TMath::Pi(), TMath::Pi()));
        registerHist(*m_monGr_LowStat, m_tracks_nHitPix_2D_LS = TH2F_LW::create("tracks_nHitPix_2D_LS", "Tracks Failed nHitPix Cut;#eta;#phi", 25, -2.5, 2.5, 25, -TMath::Pi(), TMath::Pi()));
        registerHist(*m_monGr_LowStat, m_tracks_nHitSct_2D_LS = TH2F_LW::create("tracks_nHitSct_2D_LS", "Tracks Failed nHitSct Cut;#eta;#phi", 25, -2.5, 2.5, 25, -TMath::Pi(), TMath::Pi()));
        registerHist(*m_monGr_LowStat, m_tracks_nHitSi_2D_LS = TH2F_LW::create("tracks_nHitSi_2D_LS", "Tracks Failed nHitSi Cut;#eta;#phi", 25, -2.5, 2.5, 25, -TMath::Pi(), TMath::Pi()));
        registerHist(*m_monGr_LowStat, m_tracks_nHitTrt_2D_LS = TH2F_LW::create("tracks_nHitTrt_2D_LS", "Tracks Failed nHitTrt Cut;#eta;#phi", 25, -2.5, 2.5, 25, -TMath::Pi(), TMath::Pi()));
        registerHist(*m_monGr_LowStat, m_tracks_nHitTrtHighE_2D_LS = TH2F_LW::create("tracks_nHitTrtHighE_2D_LS", "Tracks Failed nHitTrtHighE Cut;#eta;#phi", 25, -2.5, 2.5, 25, -TMath::Pi(), TMath::Pi()));
        registerHist(*m_monGr_LowStat, m_tracks_fitChi2_2D_LS = TH2F_LW::create("tracks_fitChi2_2D_LS", "Tracks Failed fitChi2 Cut;#eta;#phi", 25, -2.5, 2.5, 25, -TMath::Pi(), TMath::Pi()));
        registerHist(*m_monGr_LowStat, m_tracks_fitProb_2D_LS = TH2F_LW::create("tracks_fitProb_2D_LS", "Tracks Failed fitProb Cut;#eta;#phi", 25, -2.5, 2.5, 25, -TMath::Pi(), TMath::Pi()));
        registerHist(*m_monGr_LowStat, m_tracks_fitChi2OnNdfMax_2D_LS = TH2F_LW::create("tracks_fitChi2OnNdfMax_2D_LS", "Tracks Failed fitChi2OnNdfMax Cut;#eta;#phi", 25, -2.5, 2.5, 25, -TMath::Pi(), TMath::Pi()));

        registerHist(*m_monGr_LowStat, m_ip3d_tag_def_rate_2D_LS = TH2F_LW::create("ip3d_tag_def_rate_2D_LS", "IP3D Tag Default Rate", 25, -2.5, 2.5, 25, -TMath::Pi(), TMath::Pi()));
        registerHist(*m_monGr_LowStat, m_jet_2D_kinematic_LS     = TH2F_LW::create("jet_2D_kinematic_LS", "Jet 2D plot (Kinematic Cuts);#eta;#phi", 25, -2.5, 2.5, 25, -TMath::Pi(), TMath::Pi()));
        registerHist(*m_monGr_LowStat, m_track_selector_eff_LS   = TH2F_LW::create("track_selector_eff_LS", "Efficiency of Track Selector Tool;#eta;#phi", 25, -2.5, 2.5, 25, -TMath::Pi(), TMath::Pi()));
        registerHist(*m_monGr_LowStat, m_track_selector_all_LS   = TH2F_LW::create("track_selector_all_LS", "Efficiency of Track Selector Tool;#eta;#phi", 25, -2.5, 2.5, 25, -TMath::Pi(), TMath::Pi()));
    }

    /*
    Ended registering LowStat histograms
    */

    m_histogramsCreated = true;

    return StatusCode::SUCCESS;
}


//** --------------------------------------------------------------------------------------------------------------- **//


StatusCode JetTagMonitoring::fillHistograms() {

    msg(MSG::DEBUG)<< "fillHistograms()" << endreq;

    if (!m_histogramsCreated)
        return StatusCode::SUCCESS;

    if (m_switch_off)
        return StatusCode::SUCCESS;

    m_cutflow->Fill(0.);

    const xAOD::EventInfo* thisEventInfo;
    if (evtStore()->retrieve(thisEventInfo).isFailure())
        msg(MSG::WARNING)<< "Cannot retrieve EventInfo" << endreq;

    m_lumiBlockNum = thisEventInfo->lumiBlock();

    msg(MSG::DEBUG) << "Lumiblock ID: " << m_lumiBlockNum << endreq;

    ////////////////////////////////
    //* skip events with bad LAr *//
    ////////////////////////////////

    if ( thisEventInfo->isEventFlagBitSet(xAOD::EventInfo::LAr, LArEventBitInfo::BADFEBS) ) {
      msg(MSG::DEBUG) << "Bad LAr event: True" << endreq;
      return StatusCode::SUCCESS;    
    }

    m_cutflow->Fill(1.);



    ///////////////////////////////
    //* Trigger container       *//
    ///////////////////////////////

    msg(MSG::DEBUG) << "TrigDecTool: " << m_trigDecTool << endreq;

    if (m_use_trigdectool && m_trigDecTool != 0) {
      if (m_trigDecTool->isPassed("L1_J10"))  m_trigPassed->Fill(0.);
      if (m_trigDecTool->isPassed("L1_J15"))  m_trigPassed->Fill(1.);
      if (m_trigDecTool->isPassed("L1_J20"))  m_trigPassed->Fill(2.);
      if (m_trigDecTool->isPassed("L1_J30"))  m_trigPassed->Fill(3.);
      if (m_trigDecTool->isPassed("L1_J40"))  m_trigPassed->Fill(4.);
      if (m_trigDecTool->isPassed("L1_J50"))  m_trigPassed->Fill(5.);
      if (m_trigDecTool->isPassed("L1_J75"))  m_trigPassed->Fill(6.);
      if (m_trigDecTool->isPassed("L1_J100"))  m_trigPassed->Fill(7.);
      if (m_trigDecTool->isPassed("L1_J350"))  m_trigPassed->Fill(8.);
      if (m_trigDecTool->isPassed("L1_3J10"))  m_trigPassed->Fill(9.);
      if (m_trigDecTool->isPassed("L1_3J15"))  m_trigPassed->Fill(10.);
      if (m_trigDecTool->isPassed("L1_3J20"))  m_trigPassed->Fill(11.);
      if (m_trigDecTool->isPassed("L1_3J50"))  m_trigPassed->Fill(12.);
      if (m_trigDecTool->isPassed("L1_3J75"))  m_trigPassed->Fill(13.);
      if (m_trigDecTool->isPassed("L1_4J10"))  m_trigPassed->Fill(14.);
      if (m_trigDecTool->isPassed("L1_4J15"))  m_trigPassed->Fill(15.);
      if (m_trigDecTool->isPassed("L1_4J30"))  m_trigPassed->Fill(16.);
      if (m_trigDecTool->isPassed("L1_5J10"))  m_trigPassed->Fill(17.);
      if (m_trigDecTool->isPassed("L1_5J20"))  m_trigPassed->Fill(18.);
      
      
      if (m_trigDecTool->isPassed("EF_mu4_L1J10_matched"))  m_trigPassed->Fill(19.);
      if (m_trigDecTool->isPassed("EF_e24vh_medium1")) m_trigPassed->Fill(20.);
      if (m_trigDecTool->isPassed("EF_mu18_IDTrkNoCut_tight")) m_trigPassed->Fill(21.);
      
      if (m_trigDecTool->isPassed("L1_RD0_FILLED")) m_trigPassed->Fill(22.);
    }

    //////////////////////
    //* Primary vertex *//
    //////////////////////

    unsigned int npv = 0, npv_trk = 0, nsv_trk = 0;
    double xpv = 0., ypv = 0., zpv = 0.;

    const DataHandle<VxContainer> vxContainer;
    bool foundPrimaryVtx = retrieveCollection(m_storeGate, vxContainer, m_primaryVertexName);

    if (!foundPrimaryVtx) {

        msg(MSG::WARNING) << "Unable to retrieve \"" << m_primaryVertexName << "\" from StoreGate" << endreq;

        return StatusCode::SUCCESS;
    }

    msg(MSG::DEBUG)<< "VxContainer \"" << m_primaryVertexName << "\" found with " << vxContainer->size() << " entries" << endreq;

    npv = vxContainer->size();
    m_global_nPrimVtx->Fill((float)npv);

    if (vxContainer->size() < 1) {
        msg(MSG::WARNING) << "No primary vertices reconstructed" << endreq;
        return StatusCode::SUCCESS;
    }

    m_cutflow->Fill(2.);

    m_priVtx = vxContainer->at(0);

    npv_trk = m_priVtx->vxTrackAtVertex()->size();

    msg(MSG::DEBUG) << "PV tracks: " << npv_trk << endreq;

    const Trk::RecVertex& primaryRecVertex = m_priVtx->recVertex();

    xpv = primaryRecVertex.position().x();
    ypv = primaryRecVertex.position().y();
    zpv = primaryRecVertex.position().z();

    m_global_xPrimVtx->Fill(xpv);
    m_global_yPrimVtx->Fill(ypv);
    m_global_zPrimVtx->Fill(zpv);

    msg(MSG::DEBUG)<< "primary vertex: x = " << xpv << ", y = " << ypv << ", z = " << zpv << endreq;

    if (xpv == 0 && ypv == 0 && zpv == 0)
        msg(MSG::WARNING) << "Primary Vertex is (0,0,0)" << endreq;

    /* store the number of tracks of the second PV candidate, if any */
    if ( vxContainer->size() > 1 ) {
        nsv_trk = vxContainer->at(1)->vxTrackAtVertex()->size();
    }

    m_priVtx_trks->Fill(npv_trk, nsv_trk);

    // check first PV > 4 tracks, if not, bail
    if ( npv_trk <  m_pri_vtx_trk_min_cut )
        return StatusCode::SUCCESS;

    m_cutflow->Fill(3.);

//     double vtxChiSq = primaryRecVertex.fitQuality().chiSquared();
//     int    vtxNDoF  = primaryRecVertex.fitQuality().numberDoF();

//     double vtxProb  = vtxChiSq / vtxNDoF;

    /* Fill Jet-based histograms */

    fillJetHistograms();

    ///////////////////////////////
    //* TrackParticle container *//
    ///////////////////////////////

    const DataHandle<xAOD::TrackParticleContainer> trackParticles;
    bool foundTrackPartColl = retrieveCollection(m_storeGate, trackParticles, m_trackParticleName);

    if (!foundTrackPartColl)  {

        msg(MSG::WARNING) << "Unable to retrieve \"" << m_trackParticleName << "\" from StoreGate" << endreq;

        return StatusCode::SUCCESS;

    }

    msg(MSG::DEBUG) << "TrackParticleContainer \"" << m_trackParticleName << "\" found with " << trackParticles->size() << " entries" << endreq;

    xAOD::TrackParticleContainer::const_iterator trackParticleItr = trackParticles->begin();
    xAOD::TrackParticleContainer::const_iterator trackParticleEnd = trackParticles->end();

    // New for Release 19: summary information is filled by reference, with a boolean Pass/Fail check...

    uint8_t nPixHits    = 0;
    uint8_t nSCTHits    = 0;
    uint8_t nSiHits     = 0;
    uint8_t nBLayerHits = 0;
    uint8_t nTRTHits    = 0;

    for ( ; trackParticleItr != trackParticleEnd; trackParticleItr++) { 

	if ((*trackParticleItr)->summaryValue(nBLayerHits, xAOD::numberOfBLayerHits)) { m_global_BLayerHits->Fill((float) nBLayerHits); }
	if ((*trackParticleItr)->summaryValue(nPixHits, xAOD::numberOfPixelHits))     { m_global_PixelHits->Fill((float) nPixHits); }
	if ((*trackParticleItr)->summaryValue(nSCTHits, xAOD::numberOfSCTHits))       { m_global_SCTHits->Fill((float) nSCTHits); }
	if ((*trackParticleItr)->summaryValue(nTRTHits, xAOD::numberOfTRTHits))       { m_global_TRTHits->Fill((float) nTRTHits); }

	// too complicated to set up a check at this point... thank you framework!
	nSiHits = nPixHits + nSCTHits;
        m_global_SiHits->Fill((float)nSiHits);
	nSiHits = 0; // clean up value for next round...
    }

    m_trackParticle_n->Fill((*trackParticles).size());

    msg(MSG::DEBUG) << "end of fillHistograms()" << endreq;

    return StatusCode::SUCCESS;
}


bool JetTagMonitoring::applyKinematicCuts(const xAOD::Jet *jet) {

    msg(MSG::DEBUG) << "applyKinematicCuts()" << endreq;

    //* get jet basic kinematics *//
    CLHEP::HepLorentzVector p4(jet->px(),jet->py(),jet->pz(),jet->e());

    m_jet_eta->Fill(p4.pseudoRapidity());
    m_jet_phi->Fill(p4.phi());
    m_jet_et->Fill(p4.et());

    msg(MSG::DEBUG) << "BJet kinematics: eta = " << p4.pseudoRapidity()
                    << ", phi= " << p4.phi() << ", pT= " << p4.perp()
                    << endreq;


    // kinematic cuts defined as properties
    if ( jet->pt() < m_jet_pt_cut || fabs(jet->eta()) > m_jet_eta_cut )
        return false;

    return true;
}


JetTagMonitoring::Jet_t JetTagMonitoring::applyQualityCuts(const xAOD::Jet *jet) {

  msg(MSG::DEBUG) << "in applyQualityCuts()" << endreq;

  /* if TrackSelector not used, return true */
  if ( ! m_use_trackselector ) {
    msg(MSG::DEBUG) << "Not running track selection" << endreq;
    return goodJet;
  }

  msg(MSG::DEBUG) << "Running track selection" << endreq;

  unsigned int nTrk = 0, nTrk_JetProb = 0;

  // std::vector<InDet::TrackStateOnBLayerInfo> bInfo;
  m_track_selector_all->Fill(jet->eta(), jet->phi(), 1.);
  m_track_selector_all_LS->Fill(jet->eta(), jet->phi(), 1.);
  //Work around, but compared to _all not needed.
  //   m_track_selector_eff->Fill(jet->eta(), jet->phi(), 1.);

  m_trackSelectorTool->primaryVertex(m_priVtx->recVertex().position());
  m_trackSelectorTool->prepare();

  std::vector<const xAOD::IParticle*> trackVector = jet->getAssociatedObjects<xAOD::IParticle>(xAOD::JetAttribute::GhostTrack); 

  bool isSuspectJet = false;

  std::vector<const xAOD::IParticle*>::const_iterator trkItr;

  for ( trkItr = trackVector.begin(); trkItr != trackVector.end() ; trkItr++ ) {

    const xAOD::TrackParticle* trackPart = dynamic_cast<const xAOD::TrackParticle*>(*trkItr);

    double trk_eta = trackPart->eta();
    double trk_phi = trackPart->phi();

    m_tracks_all_2D->Fill(trk_eta, trk_phi, 1.);
    m_tracks_all_2D_LS->Fill(trk_eta, trk_phi, 1.);

    if ( m_trackSelectorTool->selectTrack(trackPart) ) {

      nTrk++;

      double trk_d0 = trackPart->d0();

      if (trk_d0 > m_trk_d0_min_cut && trk_d0 < m_trk_d0_max_cut) 
	nTrk_JetProb++;

    } else {
      const std::bitset<16> failedCuts = m_trackSelectorTool->currentTrackpassedCuts();
      fillBadTrackBits(failedCuts, trk_eta, trk_phi);

      if ( failedCuts[deadBLayer] || failedCuts[nHitBLayer] || failedCuts[nHitSi] )
	isSuspectJet = true;
    }

  }


  msg(MSG::DEBUG) << "Good tracks: " << nTrk << endreq;

  if ( nTrk_JetProb > m_trk_n ) {

    const xAOD::BTagging* btag = jet->btagging(); 
    
    // get JetFitter b probability
    double myTagValue = btag->JetFitter_pb();
    m_tag_jetprob_w->Fill(myTagValue );
    
  }
  if ( nTrk < m_trk_n )
    return badJet;

  m_track_selector_eff_LS->Fill(jet->eta(), jet->phi(), 1.);

  if ( isSuspectJet )
    return suspectJet;

  return goodJet;

}
  

StatusCode JetTagMonitoring::procHistograms() {

    msg(MSG::DEBUG) << "in procHistograms()" << endreq;

    /* Finalize the histograms */
    if ( endOfRun && m_histogramsCreated ) {

      m_track_selector_eff->getROOTHist()->Divide(m_track_selector_all->getROOTHist(),m_tracks_all_2D->getROOTHist());
    }
    msg(MSG::DEBUG) << "Exiting from procHistograms()" << endreq;
    return StatusCode::SUCCESS;
}


void JetTagMonitoring::fillJetHistograms() {

    msg(MSG::DEBUG) << "in fillJetHistograms()" << endreq;

    /////////////////////
    //* Jet container *//
    /////////////////////

    const DataHandle<xAOD::JetContainer> jets;
    bool foundJetColl = retrieveCollection(m_storeGate, jets, m_jetName);

    if (!foundJetColl) {

        msg(MSG::WARNING) << "Unable to retrieve \"" << m_jetName << "\" from StoreGate" << endreq;

        return;

    }

    msg(MSG::DEBUG) << "JetContainer \"" << m_jetName << "\" found with " << jets->size() << " entries" << endreq;

    xAOD::JetContainer::const_iterator jetItr = jets->begin();
    xAOD::JetContainer::const_iterator jetEnd = jets->end();

    m_jet_n->Fill((*jets).size());

    for ( ; jetItr != jetEnd; jetItr++) {

        m_cutflow_jet->Fill(0.);

        m_jet_2D_all->Fill((*jetItr)->eta(), (*jetItr)->phi(), 1.);

	if( !isGoodJet(*jetItr) ) continue;

	m_cutflow_jet->Fill(1.);

        m_jet_2D_good->Fill((*jetItr)->eta(), (*jetItr)->phi(), 1.);

        if ( !applyKinematicCuts(*jetItr) ) continue;

        m_jet_2D_kinematic->Fill((*jetItr)->eta(), (*jetItr)->phi(), 1.);
        m_jet_2D_kinematic_LS->Fill((*jetItr)->eta(), (*jetItr)->phi(), 1.);

        m_cutflow_jet->Fill(2.);

        //* get information about Track Constituents *//
        fillTrackInJetHistograms(*jetItr);

	const xAOD::BTagging* btag =(*jetItr)->btagging();

        double ip3d   = btag->IP3D_pb();
        double sv0    = btag->SV0_significance3D();
        double sv1    = btag->SV1_pb();
        double combNN = btag->JetFitter_pb();
        double mv1    = btag->MV1_discriminant();

        /* fill the 2D SV0 rate */
        if ( sv0 > m_sv0_weight_cut ) {
            m_sv0_tag_pos_rate_2D->Fill((*jetItr)->eta(), (*jetItr)->phi(), 1.);
        } else {
            m_sv0_tag_neg_rate_2D->Fill((*jetItr)->eta(), (*jetItr)->phi(), 1.);
        }

        /* fill the 2D SV1 rate */
        if ( sv1 > m_sv1_weight_cut ) {
            m_sv1_tag_pos_rate_2D->Fill((*jetItr)->eta(), (*jetItr)->phi(), 1.);
        } else {
            m_sv1_tag_neg_rate_2D->Fill((*jetItr)->eta(), (*jetItr)->phi(), 1.);
        }

        /* fill the 2D IP3D rate */
        if ( ip3d > m_ip3d_weight_cut ) {
            m_ip3d_tag_pos_rate_2D->Fill((*jetItr)->eta(), (*jetItr)->phi(), 1.);
        } else if ( ip3d < -20 ) {
            m_ip3d_tag_def_rate_2D->Fill((*jetItr)->eta(), (*jetItr)->phi(), 1.);
            m_ip3d_tag_def_rate_2D_LS->Fill((*jetItr)->eta(), (*jetItr)->phi(), 1.);
        } else {
            m_ip3d_tag_neg_rate_2D->Fill((*jetItr)->eta(), (*jetItr)->phi(), 1.);
        }

        /* fill the 2D MV1 rate */
        if ( mv1 > m_mv1_weight_cut ) {
            m_mv1_tag_pos_rate_2D->Fill((*jetItr)->eta(), (*jetItr)->phi(), 1.);
        } else if ( mv1 <= 0. ) {
            m_mv1_tag_def_rate_2D->Fill((*jetItr)->eta(), (*jetItr)->phi(), 1.);
            m_mv1_tag_def_rate_2D_LS->Fill((*jetItr)->eta(), (*jetItr)->phi(), 1.);
        } else {
            m_mv1_tag_neg_rate_2D->Fill((*jetItr)->eta(), (*jetItr)->phi(), 1.);
        }

        /* fill the 2D JetFitterCOMBNN rate */
        if ( combNN > m_jfcnn_weight_cut ) {
            m_jfcnn_tag_pos_rate_2D->Fill((*jetItr)->eta(), (*jetItr)->phi(), 1.);
        } else {
            m_jfcnn_tag_neg_rate_2D->Fill((*jetItr)->eta(), (*jetItr)->phi(), 1.);
        }

        msg(MSG::DEBUG)<< "btagging weights: \n"

                       << " SV0: " << sv0 << " SV1: "  << sv1  << "\n"
                       << " IP3D: "     << ip3d << "\n"
                       << " JetFitterCOMBNN: " << combNN << "\n"
                       << " MV1: "<< mv1 <<endreq;

        Jet_t quality = applyQualityCuts(*jetItr);

        if ( m_do_cuts && quality == badJet ) continue;

        m_cutflow_jet->Fill(3.);

        if ( quality == goodJet )
            fillGoodJetHistos(*jetItr);
        //else if ( quality == suspectJet )
        //  fillSuspectJetHistos(*jetItr);

        //* MOAR PLOTS! *//
        fillDetailedHistograms(*jetItr);

        //* get information about Electron Constituents *//
        //fillElectronHistograms(*jetItr);

        //* get information about Muon Constituents *//
        //fillMuonHistograms(*jetItr);

    }


    return;
}

/* Soft Electron tagger no longer maintained
void JetTagMonitoring::fillElectronHistograms(const xAOD::Jet *jet) {

    // get information about Electron Constituents //

    msg(MSG::DEBUG) << "in fillElectronHistograms()" << endreq;

    int nElInJet = 0;

    const Analysis::ElectronAssociation *elInJet = jet->getAssociation<Analysis::ElectronAssociation>("Electrons");

    // loop over electrons in a jet //
    if (elInJet) {
        nElInJet  = elInJet->size();

        Navigable<ElectronContainer,double>::object_iter elItr = elInJet->begin();
        for ( ; elItr != elInJet->end(); ++elItr) {
            m_jet_electrons_pt->Fill((*elItr)->pt()/1000.);
        }
    }

    m_jet_electrons_n->Fill((float)nElInJet);

    return;
}
*/

/* Soft Muon tagger no longer maintained
void JetTagMonitoring::fillMuonHistograms(const xAOD::Jet *jet) {

    msg(MSG::DEBUG) << "in fillMuonHistograms()" << endreq;

    // get information about Muon Constituents //
    int nMuInJet = 0;

    const Analysis::MuonAssociation *muInJet = jet->getAssociation<Analysis::MuonAssociation>("Muons");

    // loop over muons in a jet //
    if (muInJet) {

        nMuInJet  = muInJet->size();

        Navigable<Analysis::MuonContainer,double>::object_iter muItr = muInJet->begin();
        for ( ; muItr != muInJet->end(); ++muItr) {
            m_jet_muons_pt->Fill((*muItr)->pt()/1000.);
        }
    }


    m_jet_muons_n->Fill((float)nMuInJet);

    return;
}
*/

void JetTagMonitoring::fillTrackInJetHistograms(const xAOD::Jet *jet) {

    msg(MSG::DEBUG) << "in fillInJetTrackHistograms()" << endreq;

    //* get information about Track Constituents *//
    int nTrkInJet = 0;

    std::vector<const xAOD::IParticle*> trackVector = jet->getAssociatedObjects<xAOD::IParticle>(xAOD::JetAttribute::GhostTrack);

    std::vector<const xAOD::IParticle*>::const_iterator trkItr;

    nTrkInJet = trackVector.size();
    msg(MSG::DEBUG) << "examining " << nTrkInJet << " tracks." << endreq;

    // loop over tracks in a jet // 
    for ( trkItr = trackVector.begin(); trkItr != trackVector.end() ; trkItr++ ) {

      const xAOD::TrackParticle* trackPart = dynamic_cast<const xAOD::TrackParticle*>(*trkItr);

      uint8_t nBLayerHits = 0;
      uint8_t nPixHits    = 0;
      uint8_t nSCTHits    = 0;

      m_jet_tracks_pt->Fill(trackPart->pt()/1000.);
      m_jet_tracks_eta->Fill(trackPart->eta());
      m_jet_tracks_phi->Fill(trackPart->phi());

      if (trackPart->summaryValue(nBLayerHits, xAOD::numberOfBLayerHits)) { m_jet_tracks_BLayerHits->Fill((float) nBLayerHits); }
      if (trackPart->summaryValue(nPixHits, xAOD::numberOfPixelHits))     { m_jet_tracks_PixelHits->Fill((float)  nPixHits); }
      if (trackPart->summaryValue(nSCTHits, xAOD::numberOfSCTHits))       { m_jet_tracks_SCTHits->Fill((float)    nSCTHits); }
    
    }

    m_jet_tracks_n->Fill((float)nTrkInJet);

    return;
}

void JetTagMonitoring::fillDetailedHistograms(const xAOD::Jet *jet) {

    msg(MSG::DEBUG) << "in fillDetailedHistograms()" << endreq;

    //* get detailed information *//
    const xAOD::BTagging* btag = jet->btagging(); 


    //* detailed information for impact parameter-based informations *//
    unsigned int ntrk  = btag->nIP3D_TrackParticles();                  // number of tracks used for tagging in the jet
    double pb          = btag->IP3D_pb();                               // b likelihood
    double pu          = btag->IP3D_pu();                               // u likelihood
        
    m_tag_ip3d_n->Fill((float)ntrk); 
    m_tag_ip3d_b->Fill(pb);
    m_tag_ip3d_u->Fill(pu);

    msg(MSG::VERBOSE) << "InfoBase for IP3D: #tracks = " << ntrk
		      << ", Pb = " << pb << ", Pu = " << pu << endreq;
           
    return;
}


void JetTagMonitoring::fillBadTrackBits(const std::bitset<16> failedCuts, double eta, double phi) {

    if ( failedCuts[pTMin] ) m_tracks_pTMin_2D->Fill(eta, phi, 1.);
    if ( failedCuts[d0Max] ) m_tracks_d0Max_2D->Fill(eta, phi, 1.);
    if ( failedCuts[z0Max] ) m_tracks_z0Max_2D->Fill(eta, phi, 1.);
    if ( failedCuts[sigd0Max] ) m_tracks_sigd0Max_2D->Fill(eta, phi, 1.);
    if ( failedCuts[sigz0Max] ) m_tracks_sigz0Max_2D->Fill(eta, phi, 1.);
    if ( failedCuts[etaMax] ) m_tracks_etaMax_2D->Fill(eta, phi, 1.);
    if ( failedCuts[nHitBLayer] ) m_tracks_nHitBLayer_2D->Fill(eta, phi, 1.);
    if ( failedCuts[deadBLayer] ) m_tracks_deadBLayer_2D->Fill(eta, phi, 1.);
    if ( failedCuts[nHitPix] ) m_tracks_nHitPix_2D->Fill(eta, phi, 1.);
    if ( failedCuts[nHitSct] ) m_tracks_nHitSct_2D->Fill(eta, phi, 1.);
    if ( failedCuts[nHitSi] ) m_tracks_nHitSi_2D->Fill(eta, phi, 1.);
    if ( failedCuts[nHitTrt] ) m_tracks_nHitTrt_2D->Fill(eta, phi, 1.);
    if ( failedCuts[nHitTrtHighE] ) m_tracks_nHitTrtHighE_2D->Fill(eta, phi, 1.);
    if ( failedCuts[fitChi2] ) m_tracks_fitChi2_2D->Fill(eta, phi, 1.);
    if ( failedCuts[fitProb] ) m_tracks_fitProb_2D->Fill(eta, phi, 1.);
    if ( failedCuts[fitChi2OnNdfMax] ) m_tracks_fitChi2OnNdfMax_2D->Fill(eta, phi, 1.);


    if ( failedCuts[pTMin] ) m_tracks_pTMin_2D_LS->Fill(eta, phi, 1.);
    if ( failedCuts[d0Max] ) m_tracks_d0Max_2D_LS->Fill(eta, phi, 1.);
    if ( failedCuts[z0Max] ) m_tracks_z0Max_2D_LS->Fill(eta, phi, 1.);
    if ( failedCuts[sigd0Max] ) m_tracks_sigd0Max_2D_LS->Fill(eta, phi, 1.);
    if ( failedCuts[sigz0Max] ) m_tracks_sigz0Max_2D_LS->Fill(eta, phi, 1.);
    if ( failedCuts[etaMax] ) m_tracks_etaMax_2D_LS->Fill(eta, phi, 1.);
    if ( failedCuts[nHitBLayer] ) m_tracks_nHitBLayer_2D_LS->Fill(eta, phi, 1.);
    if ( failedCuts[deadBLayer] ) m_tracks_deadBLayer_2D_LS->Fill(eta, phi, 1.);
    if ( failedCuts[nHitPix] ) m_tracks_nHitPix_2D_LS->Fill(eta, phi, 1.);
    if ( failedCuts[nHitSct] ) m_tracks_nHitSct_2D_LS->Fill(eta, phi, 1.);
    if ( failedCuts[nHitSi] ) m_tracks_nHitSi_2D_LS->Fill(eta, phi, 1.);
    if ( failedCuts[nHitTrt] ) m_tracks_nHitTrt_2D_LS->Fill(eta, phi, 1.);
    if ( failedCuts[nHitTrtHighE] ) m_tracks_nHitTrtHighE_2D_LS->Fill(eta, phi, 1.);
    if ( failedCuts[fitChi2] ) m_tracks_fitChi2_2D_LS->Fill(eta, phi, 1.);
    if ( failedCuts[fitProb] ) m_tracks_fitProb_2D_LS->Fill(eta, phi, 1.);
    if ( failedCuts[fitChi2OnNdfMax] ) m_tracks_fitChi2OnNdfMax_2D_LS->Fill(eta, phi, 1.);


    return;

}

void JetTagMonitoring::fillGoodJetHistos(const xAOD::Jet *jet) {

    m_jet_2D_quality->Fill(jet->eta(), jet->phi(), 1.);

    const xAOD::BTagging* btag = jet->btagging();

    double ip3d   = btag->IP3D_pb();
    double sv0    = btag->SV0_significance3D();
    double sv1    = btag->SV1_pb();
    double combNN = btag->JetFitter_pb();
    double mv1    = btag->MV1_discriminant();

    m_tag_ip3d_w->Fill(ip3d);
    m_tag_mv1_w->Fill(mv1);
    m_tag_sv0_w->Fill(sv0);
    m_tag_sv1_w->Fill(sv1);
    m_tag_jfcnn_w->Fill(combNN);

    return;

}

/* should mimick as close as possible the same function from Reconstruction/Jet/JetMonitoring/src/JetRecMonToolBase.cxx */

bool JetTagMonitoring::isGoodJet(const xAOD::Jet *jet){

  if (msgLvl(MSG::DEBUG))
    msg(MSG::DEBUG) << "in dq cleaning cuts JetTagMon" << endreq;

  bool pass_cuts = false;
  	
  float jetQuality      = jet->getAttribute<float>(xAOD::JetAttribute::LArQuality);
  float jetTime         = jet->getAttribute<float>(xAOD::JetAttribute::Timing);
  float hecq            = jet->getAttribute<float>(xAOD::JetAttribute::HECQuality);
  float negE            = jet->getAttribute<float>(xAOD::JetAttribute::NegativeE);
  float chf             = jet->getAttribute<float>(xAOD::JetAttribute::SumPtTrkPt1000)/jet->pt();
  float emf             = jet->getAttribute<float>(xAOD::JetAttribute::EMFrac);
  float hecf            = jet->getAttribute<float>(xAOD::JetAttribute::HECFrac); 
  //int   SamplingMax     = CaloSampling::Unknown; 
  float fracSamplingMax = jet->getAttribute<float>(xAOD::JetAttribute::FracSamplingMax); 
 	
  if ( 
      !(
	(hecf>0.5 && fabs(hecq)>0.5) || (fabs(negE) > 60*CLHEP::GeV) ||
	(emf>0.95 && fabs(jetQuality)>0.8 && fabs(jet->eta()) < 2.8) || 
	//(n90<=5 && hecf>0.8) || (hecf>0.5 && fabs(jetQuality)>0.5) || (emf<0.05) ||
	(fabs(jetTime)>25) ||
	(emf<0.05 && chf<0.05 && fabs(jet->eta())<2) ||
	(emf<0.05 && fabs(jet->eta())>= 2) ||
	(fracSamplingMax>0.99 && fabs(jet->eta())<2)
	) 
      ){
    pass_cuts = true; 
  }
	
  return pass_cuts;
	         
}

/*
void JetTagMonitoring::fillSuspectJetHistos(const xAOD::Jet *jet) {

  float sv0   = jet->getFlavourTagWeight("SV0");
  float comb  = jet->getFlavourTagWeight(); // combination of IP3D and SV1
  float combNN = jet->getFlavourTagWeight("JetFitterCOMBNN");

  m_jet_2D_suspect->Fill(jet->eta(), jet->phi(), 1.);

  m_track_selector_suspect->Fill(jet->eta(), jet->phi(), 1.);

  m_tag_sv0_w_sj->Fill(sv0);
  m_tag_comb_w_sj->Fill(comb);
  m_tag_jfcnn_w_sj->Fill(combNN);

  return;

}


void JetTagMonitoring::fillBadZone(int zone, double ip3d) {

  if( zone == 1 ) {
    m_tag_ip3d_w_DMZ2_x1->Fill(ip3d, 1.);
    m_tag_ip3d_w_DMZ2_x2->Fill(ip3d, 1.);
    m_tag_ip3d_w_DMZ2_x4->Fill(ip3d, 1.);
    m_tag_ip3d_w_DMZ2_x8->Fill(ip3d, 1.);

    m_tag_ip3d_w_DMZ3_x1->Fill(ip3d, 1.);
    m_tag_ip3d_w_DMZ3_x2->Fill(ip3d, 1.);
    m_tag_ip3d_w_DMZ3_x4->Fill(ip3d, 1.);
    m_tag_ip3d_w_DMZ3_x8->Fill(ip3d, 1.);
  }
  else if ( zone == 2 ) {
    m_tag_ip3d_w_DMZ2_x1->Fill(ip3d, 1.);
    m_tag_ip3d_w_DMZ2_x2->Fill(ip3d, 2.);
    m_tag_ip3d_w_DMZ2_x4->Fill(ip3d, 4.);
    m_tag_ip3d_w_DMZ2_x8->Fill(ip3d, 8.);

    m_tag_ip3d_w_DMZ3_x1->Fill(ip3d, 1.);
    m_tag_ip3d_w_DMZ3_x2->Fill(ip3d, 1.);
    m_tag_ip3d_w_DMZ3_x4->Fill(ip3d, 1.);
    m_tag_ip3d_w_DMZ3_x8->Fill(ip3d, 1.);
  }
  else if ( zone == 3 ) {
    m_tag_ip3d_w_DMZ2_x1->Fill(ip3d, 1.);
    m_tag_ip3d_w_DMZ2_x2->Fill(ip3d, 1.);
    m_tag_ip3d_w_DMZ2_x4->Fill(ip3d, 1.);
    m_tag_ip3d_w_DMZ2_x8->Fill(ip3d, 1.);

    m_tag_ip3d_w_DMZ3_x1->Fill(ip3d, 1.);
    m_tag_ip3d_w_DMZ3_x2->Fill(ip3d, 2.);
    m_tag_ip3d_w_DMZ3_x4->Fill(ip3d, 4.);
    m_tag_ip3d_w_DMZ3_x8->Fill(ip3d, 8.);
  }

  return;

}
*/
