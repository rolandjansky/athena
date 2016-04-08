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
#include "xAODTracking/Vertex.h"
#include "xAODTracking/VertexContainer.h"

// Disable trigger for now since it produces an error in q-test q221
//#include "TrigDecisionTool/TrigDecisionTool.h"
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
#include "TEfficiency.h"

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

    declareProperty("JetContainer",           m_jetName           = "AntiKt4EMTopoJets");
    declareProperty("TrackParticleContainer", m_trackParticleName = "InDetTrackParticles");
    declareProperty("PrimaryVertexContainer", m_primaryVertexName = "PrimaryVertices");

    declareProperty("DQcuts", m_do_cuts = true);

    declareProperty("PriVtxTrkMinCut", m_pri_vtx_trk_min_cut = 4 );
    declareProperty("D0_min_cuts", m_trk_d0_min_cut = 0);
    declareProperty("D0_max_cuts", m_trk_d0_max_cut = 1);
    declareProperty("JetPt_cuts", m_jet_pt_cut = 15e3);
    declareProperty("JetEta_cuts", m_jet_eta_cut = 2.5);
    declareProperty("nTrk_cuts", m_trk_n = 1);
    declareProperty("SV1IP3D_weight_cut", m_sv1ip3d_weight_cut=0.);
    declareProperty("MV2c20_50_cut", m_mv2c20_50_weight_cut=0.751629);
    declareProperty("MV2c20_70_cut", m_mv2c20_70_weight_cut=-0.10694);
    declareProperty("MV2c20_80_cut", m_mv2c20_80_weight_cut=-0.658252);    
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

    // msg(MSG::VERBOSE) << "in JetTagMonitoring::registerHist " << h1->GetName() << endreq;
    ATH_MSG_VERBOSE("in JetTagMonitoring::registerHist " << h1->GetName());

    StatusCode sc = theGroup.regHist(h1);
    if (! sc.isSuccess())
      ATH_MSG_WARNING("Could not register histogram ");

    return sc;
}

//** --------------------------------------------------------------------------------------------------------------- **//


StatusCode JetTagMonitoring::registerHist(MonGroup& theGroup, LWHist* h1) {

    ATH_MSG_VERBOSE("in JetTagMonitoring::registerHist " << h1->GetName());

    StatusCode sc = theGroup.regHist(h1);
    if (!sc.isSuccess())
      ATH_MSG_WARNING("Could not register histogram ");

    return sc;
}


//** --------------------------------------------------------------------------------------------------------------- **//





//** --------------------------------------------------------------------------------------------------------------- **//




StatusCode JetTagMonitoring::initialize() {

    ATH_MSG_INFO("in JetTagMonitoring::initialize");

    StatusCode sc = ManagedMonitorToolBase::initialize();
    if (! sc.isSuccess()) {
      ATH_MSG_WARNING("Unable to initialize ManagedMonitorToolBase");
      return sc;
    }

    sc = m_storeGate.retrieve();
    if (!sc.isSuccess()) {
      ATH_MSG_WARNING("Unable to retrieve StoreGateSvc handle");
      m_switch_off = true;
    }

    if ( m_use_trackselector ) {
        if ( m_trackSelectorTool.retrieve().isFailure() ) {
	  ATH_MSG_WARNING("Failed to retrieve tool " << m_trackSelectorTool);
	  m_switch_off = true;
        }
    } else {
        ATH_MSG_INFO("Analysis::TrackSelector not used");
    }

    if ( m_switch_off )
      ATH_MSG_WARNING("Switching off JetTagMonitoring::fillHistograms()");

    m_lumiBlockNum = 0;

    return StatusCode::SUCCESS;
}


//** --------------------------------------------------------------------------------------------------------------- **//


StatusCode JetTagMonitoring::bookHistograms() {

    ATH_MSG_DEBUG("bookHistograms()");

    m_isNewLumiBlock = newLumiBlock;
    if ( !newRun ) {
        // Only book run-length histogram
        return StatusCode::SUCCESS;
    }


    if ( (m_environment != AthenaMonManager::tier0) &&
            (m_environment != AthenaMonManager::tier0ESD) &&
            (m_environment != AthenaMonManager::online) ) {
        // Only produce histograms from ESD, or from RAW if ESD-only monitoring is not enabled
        ATH_MSG_INFO("Monitoring environment is \'" << m_environment << "\', not booking histograms");
	//   return StatusCode::SUCCESS;
    }

    if ( m_dataType == AthenaMonManager::cosmics ) {
        ATH_MSG_INFO("Cosmic-ray data, not booking histograms");
        return StatusCode::SUCCESS;
    }

    ATH_MSG_INFO("Monitoring environment is \'" << m_environment << "\', data type is \'" << m_dataType << "\', booking histograms now.");


    m_monGr_shift = new MonGroup( this, "JetTagging", run );
    m_monGr_LowStat = new MonGroup( this, "JetTagging", lowStat );

    registerHist(*m_monGr_shift, m_tag_ip2d_n    = TH1F_LW::create("tag_IP2D_n","IP2D # of tracks (Quality Jets)",20,0.,20.));
    registerHist(*m_monGr_shift, m_tag_ip2d_b    = TH1F_LW::create("tag_IP2D_b","IP2D Pb (Quality Jets)",100,0.,1.));
    registerHist(*m_monGr_shift, m_tag_ip2d_u    = TH1F_LW::create("tag_IP2D_u","IP2D Pu (Quality Jets)",100,0.,1.));
    registerHist(*m_monGr_shift, m_tag_ip2d_c    = TH1F_LW::create("tag_IP2D_c","IP2D Pc (Quality Jets)",100,0.,1.));
    registerHist(*m_monGr_shift, m_tag_ip2d_llr  = TH1F_LW::create("tag_IP2D_llr","IP2D llr (Quality Jets)",160,-40.,40.));

    registerHist(*m_monGr_shift, m_tag_ip2d_n_sj    = TH1F_LW::create("tag_IP2D_n_sj","IP2D # of tracks (Suspect Jets)",20,0.,20.));
    registerHist(*m_monGr_shift, m_tag_ip2d_b_sj    = TH1F_LW::create("tag_IP2D_b_sj","IP2D Pb (Suspect Jets)",100,0.,1.));
    registerHist(*m_monGr_shift, m_tag_ip2d_u_sj    = TH1F_LW::create("tag_IP2D_u_sj","IP2D Pu (Suspect Jets)",100,0.,1.));
    registerHist(*m_monGr_shift, m_tag_ip2d_c_sj    = TH1F_LW::create("tag_IP2D_c_sj","IP2D Pc (Suspect Jets)",100,0.,1.));
    registerHist(*m_monGr_shift, m_tag_ip2d_llr_sj  = TH1F_LW::create("tag_IP2D_llr_sj","IP2D llr (Suspect Jets)",160,-40.,40.));
    
    registerHist(*m_monGr_shift, m_tag_ip3d_n    = TH1F_LW::create("tag_IP3D_n","IP3D # of tracks (Quality Jets)",20,0.,20.));
    registerHist(*m_monGr_shift, m_tag_ip3d_b    = TH1F_LW::create("tag_IP3D_b","IP3D Pb (Quality Jets)",100,0.,1.));
    registerHist(*m_monGr_shift, m_tag_ip3d_u    = TH1F_LW::create("tag_IP3D_u","IP3D Pu (Quality Jets)",100,0.,1.));
    registerHist(*m_monGr_shift, m_tag_ip3d_c    = TH1F_LW::create("tag_IP3D_c","IP3D Pc (Quality Jets)",100,0.,1.)); 
    registerHist(*m_monGr_shift, m_tag_ip3d_llr  = TH1F_LW::create("tag_IP3D_llr","IP3D llr (Quality Jets)",160,-40.,40.));  

    registerHist(*m_monGr_shift, m_tag_ip3d_n_sj    = TH1F_LW::create("tag_IP3D_n_sj","IP3D # of tracks (Suspect Jets)",20,0.,20.));
    registerHist(*m_monGr_shift, m_tag_ip3d_b_sj    = TH1F_LW::create("tag_IP3D_b_sj","IP3D Pb (Suspect Jets)",100,0.,1.));
    registerHist(*m_monGr_shift, m_tag_ip3d_u_sj    = TH1F_LW::create("tag_IP3D_u_sj","IP3D Pu (Suspect Jets)",100,0.,1.));
    registerHist(*m_monGr_shift, m_tag_ip3d_c_sj    = TH1F_LW::create("tag_IP3D_c_sj","IP3D Pc (Suspect Jets)",100,0.,1.)); 
    registerHist(*m_monGr_shift, m_tag_ip3d_llr_sj  = TH1F_LW::create("tag_IP3D_llr_sj","IP3D llr (Suspect Jets)",160,-40.,40.));  
    
    registerHist(*m_monGr_shift, m_tag_sv0_sig3d  = TH1F_LW::create("tag_SV0_sig3d","SV0 sig3d",100, 0.,100.));     
     
    registerHist(*m_monGr_shift, m_tag_sv1_b    = TH1F_LW::create("tag_SV1_b","SV1 Pb (Quality Jets)",100,0.,1.));
    registerHist(*m_monGr_shift, m_tag_sv1_u    = TH1F_LW::create("tag_SV1_u","SV1 Pu (Quality Jets)",100,0.,1.));
    registerHist(*m_monGr_shift, m_tag_sv1_c    = TH1F_LW::create("tag_SV1_c","SV1 Pc (Quality Jets)",100,0.,1.));
    registerHist(*m_monGr_shift, m_tag_sv1_llr  = TH1F_LW::create("tag_SV1_llr","SV1 llr (Quality Jets)",160,-40.,40.));

    registerHist(*m_monGr_shift, m_tag_sv1_b_sj    = TH1F_LW::create("tag_SV1_b_sj","SV1 Pb (Suspect Jets)",100,0.,1.));
    registerHist(*m_monGr_shift, m_tag_sv1_u_sj    = TH1F_LW::create("tag_SV1_u_sj","SV1 Pu (Suspect Jets)",100,0.,1.));
    registerHist(*m_monGr_shift, m_tag_sv1_c_sj    = TH1F_LW::create("tag_SV1_c_sj","SV1 Pc (Suspect Jets)",100,0.,1.));
    registerHist(*m_monGr_shift, m_tag_sv1_llr_sj  = TH1F_LW::create("tag_SV1_llr_sj","SV1 llr (Suspect Jets)",160,-40.,40.));
    
    registerHist(*m_monGr_shift, m_tag_jetfitter_llr = TH1F_LW::create("tag_JetFitter_llr","JetFitter llr (Quality Jets)",160,-40.,40.));
    registerHist(*m_monGr_shift, m_tag_jfcnn_llr   = TH1F_LW::create("tag_JFCNN_llr","Combined weight JetFitterCOMBNN (Quality Jets)",160,-40.,40.));

    registerHist(*m_monGr_shift, m_tag_jetfitter_llr_sj = TH1F_LW::create("tag_JetFitter_llr_sj","JetFitter llr (Suspect Jets)",160,-40.,40.));
    registerHist(*m_monGr_shift, m_tag_jfcnn_llr_sj   = TH1F_LW::create("tag_JFCNN_llr_sj","Combined weight JetFitterCOMBNN (Suspect Jets)",160,-40.,40.));
    
    registerHist(*m_monGr_shift, m_tag_sv1ip3d_w = TH1F_LW::create("tag_SV1IP3D_w","Combined weight SV1IP3D (quality jet)",100,-10.,10.));
    registerHist(*m_monGr_shift, m_tag_mv2c20_w  = TH1F_LW::create("tag_MV2c20_w","Combined weight MV2c20 (quality jet)",100,-1.,1.));    

    registerHist(*m_monGr_shift, m_tag_mv2c20_w_pT10_20   = TH1F_LW::create("tag_MV2c20_w_pT10_20"  ,"Combined weight MV2c20 (quality jet), jet pT = [10,20] GeV"  ,100,-1.,1.));    
    registerHist(*m_monGr_shift, m_tag_mv2c20_w_pT20_50   = TH1F_LW::create("tag_MV2c20_w_pT20_50"  ,"Combined weight MV2c20 (quality jet), jet pT = [20,50] GeV"  ,100,-1.,1.));    
    registerHist(*m_monGr_shift, m_tag_mv2c20_w_pT50_100  = TH1F_LW::create("tag_MV2c20_w_pT50_100" ,"Combined weight MV2c20 (quality jet), jet pT = [50,100] GeV" ,100,-1.,1.));    
    registerHist(*m_monGr_shift, m_tag_mv2c20_w_pT100_200 = TH1F_LW::create("tag_MV2c20_w_pT100_200","Combined weight MV2c20 (quality jet), jet pT = [100,200] GeV",100,-1.,1.));    
    registerHist(*m_monGr_shift, m_tag_mv2c20_w_pT200     = TH1F_LW::create("tag_MV2c20_w_pT200"    ,"Combined weight MV2c20 (quality jet), jet pT > 200 GeV"      ,100,-1.,1.));    

    registerHist(*m_monGr_shift, m_tag_mv2c20_w_eta0_05  = TH1F_LW::create("tag_MV2c20_w_eta0_05" ,"Combined weight MV2c20 (quality jet), jet |eta| = [0.0,0.5]" ,100,-1.,1.));    
    registerHist(*m_monGr_shift, m_tag_mv2c20_w_eta05_10 = TH1F_LW::create("tag_MV2c20_w_eta05_10","Combined weight MV2c20 (quality jet), jet |eta| = [0.5,1.0]" ,100,-1.,1.));    
    registerHist(*m_monGr_shift, m_tag_mv2c20_w_eta10_15 = TH1F_LW::create("tag_MV2c20_w_eta10_15","Combined weight MV2c20 (quality jet), jet |eta| = [1.0,1.5]" ,100,-1.,1.));    
    registerHist(*m_monGr_shift, m_tag_mv2c20_w_eta15_20 = TH1F_LW::create("tag_MV2c20_w_eta15_20","Combined weight MV2c20 (quality jet), jet |eta| = [1.5,2.0]" ,100,-1.,1.));    
    registerHist(*m_monGr_shift, m_tag_mv2c20_w_eta20_25 = TH1F_LW::create("tag_MV2c20_w_eta20_25","Combined weight MV2c20 (quality jet), jet |eta| = [2.0,2.5]" ,100,-1.,1.));    

    registerHist(*m_monGr_shift, m_tag_mv2c20_w_phi0_07  = TH1F_LW::create("tag_MV2c20_w_phi0_07" ,"Combined weight MV2c20 (quality jet), jet |phi| = [0.0,0.7]" ,100,-1.,1.));    
    registerHist(*m_monGr_shift, m_tag_mv2c20_w_phi07_14 = TH1F_LW::create("tag_MV2c20_w_phi07_14","Combined weight MV2c20 (quality jet), jet |phi| = [0.7,1.4]" ,100,-1.,1.));    
    registerHist(*m_monGr_shift, m_tag_mv2c20_w_phi14_21 = TH1F_LW::create("tag_MV2c20_w_phi14_21","Combined weight MV2c20 (quality jet), jet |phi| = [1.4,2.1]" ,100,-1.,1.));    
    registerHist(*m_monGr_shift, m_tag_mv2c20_w_phi21_28 = TH1F_LW::create("tag_MV2c20_w_phi21_28","Combined weight MV2c20 (quality jet), jet |phi| = [2.1,2.8]" ,100,-1.,1.));    
    registerHist(*m_monGr_shift, m_tag_mv2c20_w_phi28    = TH1F_LW::create("tag_MV2c20_w_phi28","Combined weight MV2c20 (quality jet), jet |phi| > 2.8" ,100,-1.,1.));

    registerHist(*m_monGr_shift, m_tag_mv2c20_w_phi_sum85OP = TH1F_LW::create("tag_MV2c20_w_phi_sum85OP","Sum weight MV2c20 > 85% OP (quality jet)" ,14,-TMath::Pi(),TMath::Pi()));
    registerHist(*m_monGr_shift, m_tag_mv2c20_w_phi_sum77OP = TH1F_LW::create("tag_MV2c20_w_phi_sum77OP","Sum weight MV2c20 > 77% OP (quality jet)" ,14,-TMath::Pi(),TMath::Pi()));
    registerHist(*m_monGr_shift, m_tag_mv2c20_w_phi_sum70OP = TH1F_LW::create("tag_MV2c20_w_phi_sum70OP","Sum weight MV2c20 > 70% OP (quality jet)" ,14,-TMath::Pi(),TMath::Pi()));
    registerHist(*m_monGr_shift, m_tag_mv2c20_w_phi_sum50OP = TH1F_LW::create("tag_MV2c20_w_phi_sum50OP","Sum weight MV2c20 > 50% OP (quality jet)" ,14,-TMath::Pi(),TMath::Pi()));
    registerHist(*m_monGr_shift, m_tag_mv2c20_w_phi_sumAll  = TH1F_LW::create("tag_MV2c20_w_phi_sumAll" ,"Sum weight MV2c20 All (quality jet)"      ,14,-TMath::Pi(),TMath::Pi()));
    registerHist(*m_monGr_shift, m_tag_mv2c20_w_phi_frac85OP = TH1F_LW::create("tag_MV2c20_w_phi_frac85OP","Frac weight MV2c20 > 85% OP (quality jet)" ,14,-TMath::Pi(),TMath::Pi()));
    registerHist(*m_monGr_shift, m_tag_mv2c20_w_phi_frac77OP = TH1F_LW::create("tag_MV2c20_w_phi_frac77OP","Frac weight MV2c20 > 77% OP (quality jet)" ,14,-TMath::Pi(),TMath::Pi()));
    registerHist(*m_monGr_shift, m_tag_mv2c20_w_phi_frac70OP = TH1F_LW::create("tag_MV2c20_w_phi_frac70OP","Frac weight MV2c20 > 70% OP (quality jet)" ,14,-TMath::Pi(),TMath::Pi()));
    registerHist(*m_monGr_shift, m_tag_mv2c20_w_phi_frac50OP = TH1F_LW::create("tag_MV2c20_w_phi_frac50OP","Frac weight MV2c20 > 50% OP (quality jet)" ,14,-TMath::Pi(),TMath::Pi()));

    registerHist(*m_monGr_shift, m_tag_sv1ip3d_w_sj = TH1F_LW::create("tag_SV1IP3D_w_sj","Combined weight SV1IP3D (suspect jet)",100,-10.,10.));
    registerHist(*m_monGr_shift, m_tag_mv2c20_w_sj  = TH1F_LW::create("tag_MV2c20_w_sj","Combined weight MV2c20 (suspect jet)",100,-1.,1.));   

    registerHist(*m_monGr_shift, m_tag_mv2c20_w_sj_pT10_20   = TH1F_LW::create("tag_MV2c20_w_sj_pT10_20"  ,"Combined weight MV2c20 (suspect jet), jet pT = [10,20] GeV"  ,100,-1.,1.));    
    registerHist(*m_monGr_shift, m_tag_mv2c20_w_sj_pT20_50   = TH1F_LW::create("tag_MV2c20_w_sj_pT20_50"  ,"Combined weight MV2c20 (suspect jet), jet pT = [20,50] GeV"  ,100,-1.,1.));    
    registerHist(*m_monGr_shift, m_tag_mv2c20_w_sj_pT50_100  = TH1F_LW::create("tag_MV2c20_w_sj_pT50_100" ,"Combined weight MV2c20 (suspect jet), jet pT = [50,100] GeV" ,100,-1.,1.));    
    registerHist(*m_monGr_shift, m_tag_mv2c20_w_sj_pT100_200 = TH1F_LW::create("tag_MV2c20_w_sj_pT100_200","Combined weight MV2c20 (suspect jet), jet pT = [100,200] GeV",100,-1.,1.));    
    registerHist(*m_monGr_shift, m_tag_mv2c20_w_sj_pT200     = TH1F_LW::create("tag_MV2c20_w_sj_pT200"    ,"Combined weight MV2c20 (suspect jet), jet pT > 200 GeV"      ,100,-1.,1.));    

    registerHist(*m_monGr_shift, m_tag_mv2c20_w_sj_eta0_05  = TH1F_LW::create("tag_MV2c20_w_sj_eta0_05" ,"Combined weight MV2c20 (suspect jet), jet |eta| = [0.0,0.5]" ,100,-1.,1.));    
    registerHist(*m_monGr_shift, m_tag_mv2c20_w_sj_eta05_10 = TH1F_LW::create("tag_MV2c20_w_sj_eta05_10","Combined weight MV2c20 (suspect jet), jet |eta| = [0.5,1.0]" ,100,-1.,1.));    
    registerHist(*m_monGr_shift, m_tag_mv2c20_w_sj_eta10_15 = TH1F_LW::create("tag_MV2c20_w_sj_eta10_15","Combined weight MV2c20 (suspect jet), jet |eta| = [1.0,1.5]" ,100,-1.,1.));    
    registerHist(*m_monGr_shift, m_tag_mv2c20_w_sj_eta15_20 = TH1F_LW::create("tag_MV2c20_w_sj_eta15_20","Combined weight MV2c20 (suspect jet), jet |eta| = [1.5,2.0]" ,100,-1.,1.));    
    registerHist(*m_monGr_shift, m_tag_mv2c20_w_sj_eta20_25 = TH1F_LW::create("tag_MV2c20_w_sj_eta20_25","Combined weight MV2c20 (suspect jet), jet |eta| = [2.0,2.5]" ,100,-1.,1.));  

    registerHist(*m_monGr_shift, m_tag_mv2c20_w_sj_phi0_07  = TH1F_LW::create("tag_MV2c20_w_sj_phi0_07" ,"Combined weight MV2c20 (suspect jet), jet |phi| = [0.0,0.7]" ,100,-1.,1.));    
    registerHist(*m_monGr_shift, m_tag_mv2c20_w_sj_phi07_14 = TH1F_LW::create("tag_MV2c20_w_sj_phi07_14","Combined weight MV2c20 (suspect jet), jet |phi| = [0.7,1.4]" ,100,-1.,1.));    
    registerHist(*m_monGr_shift, m_tag_mv2c20_w_sj_phi14_21 = TH1F_LW::create("tag_MV2c20_w_sj_phi14_21","Combined weight MV2c20 (suspect jet), jet |phi| = [1.4,2.1]" ,100,-1.,1.));    
    registerHist(*m_monGr_shift, m_tag_mv2c20_w_sj_phi21_28 = TH1F_LW::create("tag_MV2c20_w_sj_phi21_28","Combined weight MV2c20 (suspect jet), jet |phi| = [2.1,2.8]" ,100,-1.,1.));    
    registerHist(*m_monGr_shift, m_tag_mv2c20_w_sj_phi28    = TH1F_LW::create("tag_MV2c20_w_sj_phi28","Combined weight MV2c20 (suspect jet), jet |phi| > 2.8" ,100,-1.,1.));  
    
    registerHist(*m_monGr_shift, m_tag_mv2c20_w_sj_phi_sum85OP = TH1F_LW::create("tag_MV2c20_w_sj_phi_sum85OP","Sum weight MV2c20 > 85% OP (suspect jet)" ,14,-TMath::Pi(),TMath::Pi()));
    registerHist(*m_monGr_shift, m_tag_mv2c20_w_sj_phi_sum77OP = TH1F_LW::create("tag_MV2c20_w_sj_phi_sum77OP","Sum weight MV2c20 > 77% OP (suspect jet)" ,14,-TMath::Pi(),TMath::Pi()));
    registerHist(*m_monGr_shift, m_tag_mv2c20_w_sj_phi_sum70OP = TH1F_LW::create("tag_MV2c20_w_sj_phi_sum70OP","Sum weight MV2c20 > 70% OP (suspect jet)" ,14,-TMath::Pi(),TMath::Pi()));
    registerHist(*m_monGr_shift, m_tag_mv2c20_w_sj_phi_sum50OP = TH1F_LW::create("tag_MV2c20_w_sj_phi_sum50OP","Sum weight MV2c20 > 50% OP (suspect jet)" ,14,-TMath::Pi(),TMath::Pi()));
    registerHist(*m_monGr_shift, m_tag_mv2c20_w_sj_phi_sumAll  = TH1F_LW::create("tag_MV2c20_w_sj_phi_sumAll" ,"Sum weight MV2c20 All (suspect jet)"      ,14,-TMath::Pi(),TMath::Pi()));
    registerHist(*m_monGr_shift, m_tag_mv2c20_w_sj_phi_frac85OP = TH1F_LW::create("tag_MV2c20_w_sj_phi_frac85OP","Frac weight MV2c20 > 85% OP (suspect jet)" ,14,-TMath::Pi(),TMath::Pi()));
    registerHist(*m_monGr_shift, m_tag_mv2c20_w_sj_phi_frac77OP = TH1F_LW::create("tag_MV2c20_w_sj_phi_frac77OP","Frac weight MV2c20 > 77% OP (suspect jet)" ,14,-TMath::Pi(),TMath::Pi()));
    registerHist(*m_monGr_shift, m_tag_mv2c20_w_sj_phi_frac70OP = TH1F_LW::create("tag_MV2c20_w_sj_phi_frac70OP","Frac weight MV2c20 > 70% OP (suspect jet)" ,14,-TMath::Pi(),TMath::Pi()));
    registerHist(*m_monGr_shift, m_tag_mv2c20_w_sj_phi_frac50OP = TH1F_LW::create("tag_MV2c20_w_sj_phi_frac50OP","Frac weight MV2c20 > 50% OP (suspect jet)" ,14,-TMath::Pi(),TMath::Pi()));

    registerHist(*m_monGr_shift, m_jet_n = TH1F_LW::create("jet_n","number of jets",20,0.,20.));

    registerHist(*m_monGr_shift, m_global_nPrimVtx = TH1F_LW::create("global_nPrimVtx","# primary vertex",30,0.,30.));
    registerHist(*m_monGr_shift, m_global_xPrimVtx = TH1F_LW::create("global_xPrimVtx","x primary vertex",100,-5.0,5.0));
    registerHist(*m_monGr_shift, m_global_yPrimVtx = TH1F_LW::create("global_yPrimVtx","y primary vertex",100,-5.0,5.0));
    registerHist(*m_monGr_shift, m_global_zPrimVtx = TH1F_LW::create("global_zPrimVtx","z primary vertex",100,-250.,250.));

    registerHist(*m_monGr_shift, m_global_BLayerHits = TH1F_LW::create("global_BLayerHits","# of BLayer hits on TrackParticle",5,0.,5.));
    registerHist(*m_monGr_shift, m_global_PixelHits = TH1F_LW::create("global_PixelHits","# of Pixel hits on TrackParticle",10,0.,10.));
    registerHist(*m_monGr_shift, m_global_SCTHits = TH1F_LW::create("global_SCTHits","# of SCT hits on TrackParticle",15,0.,15.));
    registerHist(*m_monGr_shift, m_global_SiHits = TH1F_LW::create("global_SiHits","# of pixel+SCT hits on TrackParticle",25,0.,25.));
    registerHist(*m_monGr_shift, m_global_TRTHits = TH1F_LW::create("global_TRTHits","# of TRT hits on TrackParticle",100,0.,100.));

    registerHist(*m_monGr_shift, m_trackParticle_n = TH1F_LW::create("NTrackParticle","number of TrackParticles",100,0.,1000.));

    registerHist(*m_monGr_shift, m_jet_eta = TH1F_LW::create("jet_eta","Jet eta",100,-5.,5.));
    registerHist(*m_monGr_shift, m_jet_phi = TH1F_LW::create("jet_phi","Jet phi",100,-3.15,3.15));
    registerHist(*m_monGr_shift, m_jet_et  = TH1F_LW::create("jet_et","Jet et",100,0.,500000.));

    registerHist(*m_monGr_shift, m_jet_tracks_n = TH1F_LW::create("jet_tracks_n","# of tracks in a jet",50,0.,50.));
    registerHist(*m_monGr_shift, m_jet_tracks_pt = TH1F_LW::create("jet_tracks_pt","pT of tracks in a jet",100,0.,100.));
    registerHist(*m_monGr_shift, m_jet_tracks_eta = TH1F_LW::create("jet_tracks_eta","#eta of tracks in a jet",100,-2.5,2.5));
    registerHist(*m_monGr_shift, m_jet_tracks_phi = TH1F_LW::create("jet_tracks_phi","#varphi of tracks in a jet",100,-TMath::Pi(),TMath::Pi()));
    registerHist(*m_monGr_shift, m_jet_tracks_d0 = TH1F_LW::create("jet_tracks_d0","d0 of tracks in a jet",100,-5.,5.));
    registerHist(*m_monGr_shift, m_jet_tracks_z0 = TH1F_LW::create("jet_tracks_z0","z0 of tracks in a jet",100,-300.,300.));
			      
    registerHist(*m_monGr_shift, m_jet_tracks_BLayerHits = TH1F_LW::create("jet_tracks_hits_BLayer","# of BLayer hits per track in a jet",5,0.,5.));
    registerHist(*m_monGr_shift, m_jet_tracks_PixelHits = TH1F_LW::create("jet_tracks_hits_Pixel","# of Pixel hits per track in a jet",10,0.,10.));
    registerHist(*m_monGr_shift, m_jet_tracks_SCTHits = TH1F_LW::create("jet_tracks_hits_SCT","# of SCT hits per track in a jet",15,0.,15.));
   // registerHist(*m_monGr_shift, m_jet_muons_n = TH1F_LW::create("jet_muons_n","# of muons in a jet",10,0.,10.));
   // registerHist(*m_monGr_shift, m_jet_muons_pt = TH1F_LW::create("jet_muons_pt","pT of muons in a jet",100,0.,100.));
   // registerHist(*m_monGr_shift, m_jet_electrons_n = TH1F_LW::create("jet_electrons_n","# of electrons in a jet",10,0.,10.));
   // registerHist(*m_monGr_shift, m_jet_electrons_pt = TH1F_LW::create("jet_electrons_pt","pT of electrons in a jet",100,0.,100.));

    registerHist(*m_monGr_shift, m_trigPassed = TH1F_LW::create("trigPassed","Number of events passed trigger chains",25,0.,25.));

  
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
    m_trigPassed->GetXaxis()->SetBinLabel(16,"L1_4J15");
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

    registerHist(*m_monGr_shift, m_priVtx_trks = TH1F_LW::create("priVtx_trks","Tracks PV",100,-0.5,99.5));
    m_priVtx_trks->GetXaxis()->SetTitle("Primary Vertex #trks");


    registerHist(*m_monGr_shift, m_cutflow_jet = TH1F_LW::create("Jet_Cutflow","Number of jets passed DQcuts",10,-0.5,9.5));

    m_cutflow_jet->GetXaxis()->SetBinLabel(1,"All");
    m_cutflow_jet->GetXaxis()->SetBinLabel(2,"isGoodJet");
    m_cutflow_jet->GetXaxis()->SetBinLabel(3,"Kinematic Cuts");
    m_cutflow_jet->GetXaxis()->SetBinLabel(4,"Taggable");

    registerHist(*m_monGr_shift, m_track_selector_eff = TH2F_LW::create("track_selector_eff", "Efficiency of Track Selector Tool;#eta;#phi", 25, -2.5, 2.5, 25, -TMath::Pi(), TMath::Pi()));
    registerHist(*m_monGr_shift, m_track_selector_all = TH2F_LW::create("track_selector_all", "Efficiency of Track Selector Tool;#eta;#phi", 25, -2.5, 2.5, 25, -TMath::Pi(), TMath::Pi()));
    registerHist(*m_monGr_shift, m_track_selector_suspect = TH2F_LW::create("track_selector_suspect", "Efficiency of Track Selector Tool (Suspect Jets);#eta;#phi", 25, -2.5, 2.5, 25, -TMath::Pi(), TMath::Pi()));

    //2D plots
    registerHist(*m_monGr_shift, m_jet_2D_all = TH2F_LW::create("jet_2D_all", "Jet 2D plot (No Cuts);#eta;#phi", 25, -2.5, 2.5, 25, -TMath::Pi(), TMath::Pi()));
    registerHist(*m_monGr_shift, m_jet_2D_good = TH2F_LW::create("jet_2D_good", "Jet 2D plot (Good Jets);#eta;#phi", 25, -2.5, 2.5, 25, -TMath::Pi(), TMath::Pi()));
    registerHist(*m_monGr_shift, m_jet_2D_kinematic = TH2F_LW::create("jet_2D_kinematic", "Jet 2D plot (Kinematic Cuts);#eta;#phi", 25, -2.5, 2.5, 25, -TMath::Pi(), TMath::Pi()));
    registerHist(*m_monGr_shift, m_jet_2D_quality = TH2F_LW::create("jet_2D_quality", "Jet 2D plot (Quality Cuts);#eta;#phi", 25, -2.5, 2.5, 25, -TMath::Pi(), TMath::Pi()));
    registerHist(*m_monGr_shift, m_jet_2D_suspect = TH2F_LW::create("jet_2D_suspect", "Jet 2D plot (Suspect Jets);#eta;#phi", 25, -2.5, 2.5, 25, -TMath::Pi(), TMath::Pi()));
    registerHist(*m_monGr_shift, m_jet_2D_bad = TH2F_LW::create("jet_2D_bad", "Jet 2D plot (Bad Jets);#eta;#phi", 25, -2.5, 2.5, 25, -TMath::Pi(), TMath::Pi()));

    registerHist(*m_monGr_shift, m_sv1ip3d_tag_pos_rate_2D = TH2F_LW::create("sv1ip3d_tag_pos_rate_2D", "SV1+IP3D Tag Positive Rate;#eta;#phi", 25, -2.5, 2.5, 25, -TMath::Pi(), TMath::Pi()));
    registerHist(*m_monGr_shift, m_sv1ip3d_tag_neg_rate_2D = TH2F_LW::create("sv1ip3d_tag_neg_rate_2D", "SV1+IP3D Tag Negative Rate;#eta;#phi", 25, -2.5, 2.5, 25, -TMath::Pi(), TMath::Pi()));
    registerHist(*m_monGr_shift, m_mv2c20_tag_50_2D = TH2F_LW::create("mv2c20_tag_50_rate_2D", "MV2c20 Tag 50%;#eta;#phi", 25, -2.5, 2.5, 25, -TMath::Pi(), TMath::Pi()));
    registerHist(*m_monGr_shift, m_mv2c20_tag_70_2D = TH2F_LW::create("mv2c20_tag_70_rate_2D", "MV2c20 Tag 70%;#eta;#phi", 25, -2.5, 2.5, 25, -TMath::Pi(), TMath::Pi()));
    registerHist(*m_monGr_shift, m_mv2c20_tag_80_2D = TH2F_LW::create("mv2c20_tag_80_rate_2D", "MV2c20 Tag 80%;#eta;#phi", 25, -2.5, 2.5, 25, -TMath::Pi(), TMath::Pi()));

 
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

    registerHist(*m_monGr_shift, m_tracks_passedCuts_2D = TH2F_LW::create("tracks_passedCuts_2D", "Tracks passing all cuts;#eta;#phi", 25, -2.5, 2.5, 25, -TMath::Pi(), TMath::Pi()));

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

        registerHist(*m_monGr_LowStat, m_mv2c20_tag_80_2D_LS = TH2F_LW::create("mv2c20_tag_80_2D_LS", "MV2c20 Tag 80%", 25, -2.5, 2.5, 25, -TMath::Pi(), TMath::Pi()));
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

    ATH_MSG_DEBUG("fillHistograms()");

    if (!m_histogramsCreated)
        return StatusCode::SUCCESS;

    if (m_switch_off)
        return StatusCode::SUCCESS;

    m_cutflow->Fill(0.);

    const xAOD::EventInfo* thisEventInfo;
    if (evtStore()->retrieve(thisEventInfo).isFailure())
      ATH_MSG_WARNING("Cannot retrieve EventInfo");

    m_lumiBlockNum = thisEventInfo->lumiBlock();

    ATH_MSG_DEBUG("Lumiblock ID: " << m_lumiBlockNum);

    ////////////////////////////////
    //* Event cleaning           *//
    ////////////////////////////////

    if ( thisEventInfo->errorState(xAOD::EventInfo::EventFlagSubDet::Tile) == xAOD::EventInfo::Error || 
	 thisEventInfo->errorState(xAOD::EventInfo::EventFlagSubDet::LAr)  == xAOD::EventInfo::Error ||
	 thisEventInfo->isEventFlagBitSet(xAOD::EventInfo::Core, 18) ) { 
      ATH_MSG_DEBUG("Event cleaning : skip event");
      return StatusCode::SUCCESS;    
    }
    
    m_cutflow->Fill(1.);

    ///////////////////////////////
    //* Trigger container       *//
    ///////////////////////////////

    ATH_MSG_DEBUG("TrigDecTool: " << m_trigDecTool);

    // Disable trigger for now since it produces an error in q-test q221
    // if (m_use_trigdectool && m_trigDecTool != 0) {
    //   if (m_trigDecTool->isPassed("L1_J10"))  m_trigPassed->Fill(0.);
    //   if (m_trigDecTool->isPassed("L1_J15"))  m_trigPassed->Fill(1.);
    //   if (m_trigDecTool->isPassed("L1_J20"))  m_trigPassed->Fill(2.);
    //   if (m_trigDecTool->isPassed("L1_J30"))  m_trigPassed->Fill(3.);
    //   if (m_trigDecTool->isPassed("L1_J40"))  m_trigPassed->Fill(4.);
    //   if (m_trigDecTool->isPassed("L1_J50"))  m_trigPassed->Fill(5.);
    //   if (m_trigDecTool->isPassed("L1_J75"))  m_trigPassed->Fill(6.);
    //   if (m_trigDecTool->isPassed("L1_J100"))  m_trigPassed->Fill(7.);
    //   if (m_trigDecTool->isPassed("L1_J350"))  m_trigPassed->Fill(8.);
    //   if (m_trigDecTool->isPassed("L1_3J10"))  m_trigPassed->Fill(9.);
    //   if (m_trigDecTool->isPassed("L1_3J15"))  m_trigPassed->Fill(10.);
    //   if (m_trigDecTool->isPassed("L1_3J20"))  m_trigPassed->Fill(11.);
    //   if (m_trigDecTool->isPassed("L1_3J50"))  m_trigPassed->Fill(12.);
    //   if (m_trigDecTool->isPassed("L1_3J75"))  m_trigPassed->Fill(13.);
    //   if (m_trigDecTool->isPassed("L1_4J10"))  m_trigPassed->Fill(14.);
    //   if (m_trigDecTool->isPassed("L1_4J15"))  m_trigPassed->Fill(15.);
    //   if (m_trigDecTool->isPassed("L1_4J30"))  m_trigPassed->Fill(16.);
    //   if (m_trigDecTool->isPassed("L1_5J10"))  m_trigPassed->Fill(17.);
    //   if (m_trigDecTool->isPassed("L1_5J20"))  m_trigPassed->Fill(18.);
      
      
    //   if (m_trigDecTool->isPassed("EF_mu4_L1J10_matched"))  m_trigPassed->Fill(19.);
    //   if (m_trigDecTool->isPassed("EF_e24vh_medium1")) m_trigPassed->Fill(20.);
    //   if (m_trigDecTool->isPassed("EF_mu18_IDTrkNoCut_tight")) m_trigPassed->Fill(21.);
      
    //   if (m_trigDecTool->isPassed("L1_RD0_FILLED")) m_trigPassed->Fill(22.);
    // }

    //////////////////////
    //* Primary vertex *//
    //////////////////////

    unsigned int npv = 0, npv_trk = 0;
    double xpv = 0., ypv = 0., zpv = 0.;

    const xAOD::VertexContainer* vxContainer(0);
    // const DataHandle<VxContainer> vxContainer;
    bool foundPrimaryVtx = evtStore()->retrieve(vxContainer, m_primaryVertexName);

    if (!foundPrimaryVtx) {
      ATH_MSG_WARNING("Unable to retrieve \"" << m_primaryVertexName << "\" from StoreGate");
      return StatusCode::SUCCESS;
    }

    ATH_MSG_DEBUG("VxContainer \"" << m_primaryVertexName << "\" found with " << vxContainer->size() << " entries");

    npv = vxContainer->size();
    m_global_nPrimVtx->Fill((float)npv);

    if (vxContainer->size() < 2) {
      ATH_MSG_WARNING("No primary vertices reconstructed");
      return StatusCode::SUCCESS;
    }

    m_cutflow->Fill(2.);

    bool foundVxTypePriVtx = false;
    m_priVtx = nullptr;
    for (xAOD::VertexContainer::const_iterator vtx = vxContainer->begin(); vtx != vxContainer->end(); ++vtx) {
      if ((*vtx)->vertexType() == xAOD::VxType::PriVtx && (*vtx)->numberDoF() > 0 ) {
	m_priVtx = *vtx;
	foundVxTypePriVtx = true;
	break;
      }
    }
    if ( ! foundVxTypePriVtx ) {
      ATH_MSG_WARNING("xAOD::Vertex of type xAOD::VxType::PriVtx was not found in vertex container.");
      return StatusCode::SUCCESS;
    }

    // if (m_priVtx->vxTrackAtVertexAvailable()) {
    if (m_priVtx->nTrackParticles()>0) {
      //npv_trk = m_priVtx->vxTrackAtVertex().size()
      npv_trk = m_priVtx->nTrackParticles();
      ATH_MSG_DEBUG("PV tracks: " << npv_trk);
    }

    xpv = m_priVtx->x();
    ypv = m_priVtx->y();
    zpv = m_priVtx->z();



    ATH_MSG_DEBUG("primary vertex: x = " << xpv << ", y = " << ypv << ", z = " << zpv);

    if (xpv == 0 && ypv == 0 && zpv == 0)
      ATH_MSG_WARNING("Primary Vertex is (0,0,0)");
    
    m_global_xPrimVtx->Fill(xpv);
    m_global_yPrimVtx->Fill(ypv);
    m_global_zPrimVtx->Fill(zpv);
    m_priVtx_trks->Fill(npv_trk);


    // check first PV > 4 tracks, if not, fail
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

      ATH_MSG_WARNING("Unable to retrieve \"" << m_trackParticleName << "\" from StoreGate");

        return StatusCode::SUCCESS;

    }

    ATH_MSG_DEBUG("TrackParticleContainer \"" << m_trackParticleName << "\" found with " << trackParticles->size() << " entries");

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
    
    ATH_MSG_DEBUG("end of fillHistograms()");

    return StatusCode::SUCCESS;
}


bool JetTagMonitoring::applyKinematicCuts(const xAOD::Jet *jet) {

    ATH_MSG_DEBUG("applyKinematicCuts()");

    //* get jet basic kinematics *//
    m_jet_eta->Fill(jet->eta());
    m_jet_phi->Fill(jet->phi());
    m_jet_et->Fill(jet->pt());

    ATH_MSG_DEBUG("Jet kinematics: eta = " << jet->eta() << ", phi= " << jet->phi() << ", pT= " << jet->pt());


    // kinematic cuts defined as properties
    if ( jet->pt() < m_jet_pt_cut || fabs(jet->eta()) > m_jet_eta_cut )
        return false;

    return true;
}


JetTagMonitoring::Jet_t JetTagMonitoring::applyQualityCuts(const xAOD::Jet *jet) {

  ATH_MSG_DEBUG("in applyQualityCuts()");

  /* if TrackSelector not used, return true */
  if ( ! m_use_trackselector ) {
    ATH_MSG_DEBUG("Not running track selection");
    return goodJet;
  }

  ATH_MSG_DEBUG("Running track selection");

  unsigned int nTrk = 0;

  // std::vector<InDet::TrackStateOnBLayerInfo> bInfo;
  m_track_selector_all->Fill(jet->eta(), jet->phi(), 1.);
  m_track_selector_all_LS->Fill(jet->eta(), jet->phi(), 1.);
  //Work around, but compared to _all not needed.
  //   m_track_selector_eff->Fill(jet->eta(), jet->phi(), 1.);

  m_trackSelectorTool->primaryVertex(m_priVtx->position());
  m_trackSelectorTool->prepare();

  std::vector<const xAOD::IParticle*> trackVector = jet->getAssociatedObjects<xAOD::IParticle>(xAOD::JetAttribute::GhostTrack); 

  bool isSuspectJet = false;

  std::vector<const xAOD::IParticle*>::const_iterator trkItr;

  for ( trkItr = trackVector.begin(); trkItr != trackVector.end() ; trkItr++ ) {

    const xAOD::TrackParticle* trackPart = dynamic_cast<const xAOD::TrackParticle*>(*trkItr);
    if ( ! trackPart ) continue;

    double trk_eta = trackPart->eta();
    double trk_phi = trackPart->phi();

    m_tracks_all_2D->Fill(trk_eta, trk_phi, 1.);
    m_tracks_all_2D_LS->Fill(trk_eta, trk_phi, 1.);

    if ( m_trackSelectorTool->selectTrack(trackPart) ) {

      m_tracks_passedCuts_2D->Fill(trk_eta, trk_phi);
      nTrk++;

    } else {
      const std::bitset<17> failedCuts = m_trackSelectorTool->currentTrackpassedCuts();
      fillBadTrackBits(failedCuts, trk_eta, trk_phi);

      if ( !failedCuts[deadBLayer] || !failedCuts[nHitBLayer] || !failedCuts[nHitSi] )
	isSuspectJet = true;
    }

  }
  ATH_MSG_DEBUG("Good tracks: " << nTrk);

  if ( nTrk < m_trk_n ) return badJet;

  m_track_selector_eff_LS->Fill(jet->eta(), jet->phi(), 1.);

  if ( isSuspectJet ) return suspectJet;

  return goodJet;

}
  

void JetTagMonitoring::fillEffHist(TH1 * h_passed, TH1 * h_all, TH1F_LW * effHist) {

  if ( TEfficiency::CheckConsistency(*h_passed, *h_all) ) {

    TEfficiency * eff = new TEfficiency(*h_passed, *h_all);
    eff->SetStatisticOption(TEfficiency::kBJeffrey);

    for (int i = 1; i <= h_all->GetNbinsX(); ++i) {
      effHist->SetBinContent( i , eff->GetEfficiency(i) );
      effHist->SetBinError( i , (eff->GetEfficiencyErrorUp(i) + eff->GetEfficiencyErrorLow(i))/2. );
    }

    delete eff;

  }
  
}


StatusCode JetTagMonitoring::procHistograms() {

    ATH_MSG_DEBUG("in procHistograms()");

    /* Finalize the histograms */
    if ( m_histogramsCreated && (endOfRun || AthenaMonManager::environment() == AthenaMonManager::online) ) {
      
      m_track_selector_eff->getROOTHist()->Divide(m_track_selector_all->getROOTHist(),m_tracks_all_2D->getROOTHist());
      
      TH1 * qualityJets_sum85OP = m_tag_mv2c20_w_phi_sum85OP->getROOTHistBase();     
      TH1 * qualityJets_sum77OP = m_tag_mv2c20_w_phi_sum77OP->getROOTHistBase();     
      TH1 * qualityJets_sum70OP = m_tag_mv2c20_w_phi_sum70OP->getROOTHistBase();     
      TH1 * qualityJets_sum50OP = m_tag_mv2c20_w_phi_sum50OP->getROOTHistBase();     
      TH1 * qualityJets_sumAll  = m_tag_mv2c20_w_phi_sumAll->getROOTHistBase();     

      fillEffHist(qualityJets_sum85OP, qualityJets_sumAll, m_tag_mv2c20_w_phi_frac85OP);
      fillEffHist(qualityJets_sum77OP, qualityJets_sumAll, m_tag_mv2c20_w_phi_frac77OP);
      fillEffHist(qualityJets_sum70OP, qualityJets_sumAll, m_tag_mv2c20_w_phi_frac70OP);
      fillEffHist(qualityJets_sum50OP, qualityJets_sumAll, m_tag_mv2c20_w_phi_frac50OP);

      TH1 * suspectJets_sum85OP = m_tag_mv2c20_w_sj_phi_sum85OP->getROOTHistBase();     
      TH1 * suspectJets_sum77OP = m_tag_mv2c20_w_sj_phi_sum77OP->getROOTHistBase();     
      TH1 * suspectJets_sum70OP = m_tag_mv2c20_w_sj_phi_sum70OP->getROOTHistBase();     
      TH1 * suspectJets_sum50OP = m_tag_mv2c20_w_sj_phi_sum50OP->getROOTHistBase();     
      TH1 * suspectJets_sumAll  = m_tag_mv2c20_w_sj_phi_sumAll->getROOTHistBase();     
      
      fillEffHist(suspectJets_sum85OP, suspectJets_sumAll, m_tag_mv2c20_w_sj_phi_frac85OP);
      fillEffHist(suspectJets_sum77OP, suspectJets_sumAll, m_tag_mv2c20_w_sj_phi_frac77OP);
      fillEffHist(suspectJets_sum70OP, suspectJets_sumAll, m_tag_mv2c20_w_sj_phi_frac70OP);
      fillEffHist(suspectJets_sum50OP, suspectJets_sumAll, m_tag_mv2c20_w_sj_phi_frac50OP);

    }
    ATH_MSG_DEBUG("Exiting from procHistograms()");
    return StatusCode::SUCCESS;
}


void JetTagMonitoring::fillJetHistograms() {

    ATH_MSG_DEBUG("in fillJetHistograms()");

    /////////////////////
    //* Jet container *//
    /////////////////////

    const DataHandle<xAOD::JetContainer> jets;
    bool foundJetColl = retrieveCollection(m_storeGate, jets, m_jetName);

    if (!foundJetColl) {
      ATH_MSG_WARNING("Unable to retrieve \"" << m_jetName << "\" from StoreGate");
      return;
    }

    ATH_MSG_DEBUG("JetContainer \"" << m_jetName << "\" found with " << jets->size() << " entries");

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

        if(!btag) continue; 

	double sv1ip3d = btag->SV1plusIP3D_discriminant();
        double mv2c20 = btag->auxdata<double>("MV2c20_discriminant"); 
    
        /* fill the 2D SV1IP3D rate */
        if ( sv1ip3d > m_sv1ip3d_weight_cut ) {
            m_sv1ip3d_tag_pos_rate_2D->Fill((*jetItr)->eta(), (*jetItr)->phi(), 1.);
        } else {
            m_sv1ip3d_tag_neg_rate_2D->Fill((*jetItr)->eta(), (*jetItr)->phi(), 1.);
        }

        /* fill the 2D MV2c20 histos */
        if ( mv2c20 > m_mv2c20_50_weight_cut ) m_mv2c20_tag_50_2D->Fill((*jetItr)->eta(), (*jetItr)->phi(), 1.);
        if ( mv2c20 > m_mv2c20_70_weight_cut ) m_mv2c20_tag_70_2D->Fill((*jetItr)->eta(), (*jetItr)->phi(), 1.);
        if ( mv2c20 > m_mv2c20_80_weight_cut ) m_mv2c20_tag_80_2D->Fill((*jetItr)->eta(), (*jetItr)->phi(), 1.);	
        
        ATH_MSG_DEBUG("btagging weights: " << "SV1IP3D: " << sv1ip3d << ", MV2c20: "  << mv2c20);

        Jet_t quality = applyQualityCuts(*jetItr);
	
        if ( m_do_cuts && quality == badJet ) continue;

        m_cutflow_jet->Fill(3.);
	
        if ( quality == goodJet )
           fillGoodJetHistos(*jetItr);

        else if ( quality == suspectJet )
           fillSuspectJetHistos(*jetItr);

        else if ( quality == badJet )
           fillBadJetHistos(*jetItr);

        //* MORE PLOTS! *//
        fillDetailedHistograms(*jetItr, quality);

    }

    ATH_MSG_DEBUG("... done fillJetHistograms()");
    return;
}

void JetTagMonitoring::fillTrackInJetHistograms(const xAOD::Jet *jet) {

    ATH_MSG_DEBUG("in fillInJetTrackHistograms()");

    //* get information about Track Constituents *//
    int nTrkInJet = 0;

    std::vector<const xAOD::IParticle*> trackVector = jet->getAssociatedObjects<xAOD::IParticle>(xAOD::JetAttribute::GhostTrack);

    std::vector<const xAOD::IParticle*>::const_iterator trkItr;

    nTrkInJet = trackVector.size();
    ATH_MSG_DEBUG("examining " << nTrkInJet << " tracks.");

    // loop over tracks in a jet // 
    for ( trkItr = trackVector.begin(); trkItr != trackVector.end() ; trkItr++ ) {

      const xAOD::TrackParticle* trackPart = dynamic_cast<const xAOD::TrackParticle*>(*trkItr);
      if ( ! trackPart ) continue;

      uint8_t nBLayerHits = 0;
      uint8_t nPixHits    = 0;
      uint8_t nSCTHits    = 0;

      m_jet_tracks_pt->Fill(trackPart->pt()/1000.);
      m_jet_tracks_eta->Fill(trackPart->eta());
      m_jet_tracks_phi->Fill(trackPart->phi());
      m_jet_tracks_d0->Fill(trackPart->d0());
      m_jet_tracks_z0->Fill(trackPart->z0());


      if (trackPart->summaryValue(nBLayerHits, xAOD::numberOfBLayerHits)) { m_jet_tracks_BLayerHits->Fill((float) nBLayerHits); }
      if (trackPart->summaryValue(nPixHits, xAOD::numberOfPixelHits))     { m_jet_tracks_PixelHits->Fill((float)  nPixHits); }
      if (trackPart->summaryValue(nSCTHits, xAOD::numberOfSCTHits))       { m_jet_tracks_SCTHits->Fill((float)    nSCTHits); }
    
    }

    m_jet_tracks_n->Fill((float)nTrkInJet);

    return;
}

void JetTagMonitoring::fillDetailedHistograms(const xAOD::Jet *jet, Jet_t quality) {

    ATH_MSG_DEBUG("in fillDetailedHistograms()");

    //* get detailed information *//
    const xAOD::BTagging* btag = jet->btagging(); 


    //* detailed information for impact parameter-based informations *//
    unsigned int ntrk  = btag->nIP3D_TrackParticles();                  // number of tracks used for tagging in the jet
    double pb          = btag->IP3D_pb();                               // b likelihood
    double pu          = btag->IP3D_pu();                               // u likelihood
    double pc          = btag->IP3D_pc();                               // c likelihood
    double llr         = btag->IP3D_loglikelihoodratio();               // log likelihood ratio 
        
    if ( quality == goodJet ) {
      m_tag_ip3d_n->Fill((float)ntrk); 
      m_tag_ip3d_b->Fill(pb);
      m_tag_ip3d_u->Fill(pu);
      m_tag_ip3d_c->Fill(pc);
      m_tag_ip3d_llr->Fill(llr);    
    }
    else if ( quality == suspectJet ) {
      m_tag_ip3d_n_sj->Fill((float)ntrk); 
      m_tag_ip3d_b_sj->Fill(pb);
      m_tag_ip3d_u_sj->Fill(pu);
      m_tag_ip3d_c_sj->Fill(pc);
      m_tag_ip3d_llr_sj->Fill(llr);    
    }

    ATH_MSG_VERBOSE("InfoBase for IP3D: #tracks = " << ntrk << ", Pb = " << pb 
    << ", Pu = " << pu<< ", Pc = " << pc << ", llr = " << llr);

    ntrk  = btag->nIP2D_TrackParticles();  // number of tracks used for tagging in the jet
    pb          = btag->IP2D_pb();        // b likelihood
    pu          = btag->IP2D_pu();        // u likelihood
    pc          = btag->IP2D_pc();        // c likelihood
    llr         = btag->IP2D_loglikelihoodratio(); // log likelihood ratio 
    
    if ( quality == goodJet ) {
      m_tag_ip2d_n->Fill((float)ntrk); 
      m_tag_ip2d_b->Fill(pb);
      m_tag_ip2d_u->Fill(pu);
      m_tag_ip2d_c->Fill(pc);
      m_tag_ip2d_llr->Fill(llr);  
    }
    else if ( quality == suspectJet ) {
      m_tag_ip2d_n_sj->Fill((float)ntrk); 
      m_tag_ip2d_b_sj->Fill(pb);
      m_tag_ip2d_u_sj->Fill(pu);
      m_tag_ip2d_c_sj->Fill(pc);
      m_tag_ip2d_llr_sj->Fill(llr);  
    }
     
    ATH_MSG_VERBOSE("InfoBase for IP2D: #tracks = " << ntrk << ", Pb = " << pb 
    << ", Pu = " << pu<< ", Pc = " << pc << ", llr = " << llr);

    m_tag_sv0_sig3d->Fill(btag->SV0_significance3D());
    
    pb          = btag->SV1_pb();        // b likelihood
    pu          = btag->SV1_pu();        // u likelihood
    pc          = btag->SV1_pc();        // c likelihood
    llr         = btag->SV1_loglikelihoodratio();// log likelihood ratio 
    
    if ( quality == goodJet ) {
      m_tag_sv1_b->Fill(pb);
      m_tag_sv1_u->Fill(pu);
      m_tag_sv1_c->Fill(pc);
      m_tag_sv1_llr->Fill(llr);
    }
    else if ( quality == suspectJet ) {
      m_tag_sv1_b_sj->Fill(pb);
      m_tag_sv1_u_sj->Fill(pu);
      m_tag_sv1_c_sj->Fill(pc);
      m_tag_sv1_llr_sj->Fill(llr);
    }

    ATH_MSG_VERBOSE("InfoBase for SV1: Pb = " << pb << ", Pu = " << pu<< ", Pc = " << pc<< ", llr = " << llr);
    
    llr = btag->JetFitter_loglikelihoodratio(); // log likelihood ratio 
    if ( quality == goodJet ) {
      m_tag_jetfitter_llr->Fill(llr);
    }
    else if ( quality == suspectJet ) {
      m_tag_jetfitter_llr_sj->Fill(llr);
    }
 
    llr = btag->JetFitterCombNN_loglikelihoodratio(); // log likelihood ratio 
    if ( quality == goodJet ) {
      m_tag_jfcnn_llr->Fill(llr);
    }
    else if ( quality == suspectJet ) {
      m_tag_jfcnn_llr_sj->Fill(llr);
    }  
  
    return;
}


void JetTagMonitoring::fillBadTrackBits(const std::bitset<17> failedCuts, double eta, double phi) {

    if ( failedCuts[pTMin]           == 0 ) m_tracks_pTMin_2D->Fill(eta, phi, 1.);
    if ( failedCuts[d0Max]           == 0 ) m_tracks_d0Max_2D->Fill(eta, phi, 1.);
    if ( failedCuts[z0Max]           == 0 ) m_tracks_z0Max_2D->Fill(eta, phi, 1.);
    if ( failedCuts[sigd0Max]        == 0 ) m_tracks_sigd0Max_2D->Fill(eta, phi, 1.);
    if ( failedCuts[sigz0Max]        == 0 ) m_tracks_sigz0Max_2D->Fill(eta, phi, 1.);
    if ( failedCuts[etaMax]          == 0 ) m_tracks_etaMax_2D->Fill(eta, phi, 1.);
    if ( failedCuts[nHitBLayer]      == 0 ) m_tracks_nHitBLayer_2D->Fill(eta, phi, 1.);
    if ( failedCuts[deadBLayer]      == 0 ) m_tracks_deadBLayer_2D->Fill(eta, phi, 1.);
    if ( failedCuts[nHitPix]         == 0 ) m_tracks_nHitPix_2D->Fill(eta, phi, 1.);
    if ( failedCuts[nHitSct]         == 0 ) m_tracks_nHitSct_2D->Fill(eta, phi, 1.);
    if ( failedCuts[nHitSi]          == 0 ) m_tracks_nHitSi_2D->Fill(eta, phi, 1.);
    if ( failedCuts[nHitTrt]         == 0 ) m_tracks_nHitTrt_2D->Fill(eta, phi, 1.);
    if ( failedCuts[nHitTrtHighE]    == 0 ) m_tracks_nHitTrtHighE_2D->Fill(eta, phi, 1.);
    if ( failedCuts[fitChi2]         == 0 ) m_tracks_fitChi2_2D->Fill(eta, phi, 1.);
    if ( failedCuts[fitProb]         == 0 ) m_tracks_fitProb_2D->Fill(eta, phi, 1.);
    if ( failedCuts[fitChi2OnNdfMax] == 0 ) m_tracks_fitChi2OnNdfMax_2D->Fill(eta, phi, 1.);


    if ( failedCuts[pTMin]           == 0 ) m_tracks_pTMin_2D_LS->Fill(eta, phi, 1.);
    if ( failedCuts[d0Max]           == 0 ) m_tracks_d0Max_2D_LS->Fill(eta, phi, 1.);
    if ( failedCuts[z0Max]           == 0 ) m_tracks_z0Max_2D_LS->Fill(eta, phi, 1.);
    if ( failedCuts[sigd0Max]        == 0 ) m_tracks_sigd0Max_2D_LS->Fill(eta, phi, 1.);
    if ( failedCuts[sigz0Max]        == 0 ) m_tracks_sigz0Max_2D_LS->Fill(eta, phi, 1.);
    if ( failedCuts[etaMax]          == 0 ) m_tracks_etaMax_2D_LS->Fill(eta, phi, 1.);
    if ( failedCuts[nHitBLayer]      == 0 ) m_tracks_nHitBLayer_2D_LS->Fill(eta, phi, 1.);
    if ( failedCuts[deadBLayer]      == 0 ) m_tracks_deadBLayer_2D_LS->Fill(eta, phi, 1.);
    if ( failedCuts[nHitPix]         == 0 ) m_tracks_nHitPix_2D_LS->Fill(eta, phi, 1.);
    if ( failedCuts[nHitSct]         == 0 ) m_tracks_nHitSct_2D_LS->Fill(eta, phi, 1.);
    if ( failedCuts[nHitSi]          == 0 ) m_tracks_nHitSi_2D_LS->Fill(eta, phi, 1.);
    if ( failedCuts[nHitTrt]         == 0 ) m_tracks_nHitTrt_2D_LS->Fill(eta, phi, 1.);
    if ( failedCuts[nHitTrtHighE]    == 0 ) m_tracks_nHitTrtHighE_2D_LS->Fill(eta, phi, 1.);
    if ( failedCuts[fitChi2]         == 0 ) m_tracks_fitChi2_2D_LS->Fill(eta, phi, 1.);
    if ( failedCuts[fitProb]         == 0 ) m_tracks_fitProb_2D_LS->Fill(eta, phi, 1.);
    if ( failedCuts[fitChi2OnNdfMax] == 0 ) m_tracks_fitChi2OnNdfMax_2D_LS->Fill(eta, phi, 1.);


    return;

}

void JetTagMonitoring::fillGoodJetHistos(const xAOD::Jet *jet) {

    m_jet_2D_quality->Fill(jet->eta(), jet->phi(), 1.);

    const xAOD::BTagging* btag = jet->btagging();

    double sv1ip3d = btag->SV1plusIP3D_discriminant();
    double mv2c20 = btag->auxdata<double>("MV2c20_discriminant");    
      
    m_tag_sv1ip3d_w->Fill(sv1ip3d);
    m_tag_mv2c20_w->Fill(mv2c20);   

    if      ( jet->pt() > 200000. ) m_tag_mv2c20_w_pT200->Fill(mv2c20);   
    else if ( jet->pt() > 100000. ) m_tag_mv2c20_w_pT100_200->Fill(mv2c20);   
    else if ( jet->pt() >  50000. ) m_tag_mv2c20_w_pT50_100->Fill(mv2c20);   
    else if ( jet->pt() >  20000. ) m_tag_mv2c20_w_pT20_50->Fill(mv2c20);   
    else if ( jet->pt() >  10000. ) m_tag_mv2c20_w_pT10_20->Fill(mv2c20);   

    if      ( fabs(jet->eta()) > 2.0 ) m_tag_mv2c20_w_eta20_25->Fill(mv2c20);   
    else if ( fabs(jet->eta()) > 1.5 ) m_tag_mv2c20_w_eta15_20->Fill(mv2c20);   
    else if ( fabs(jet->eta()) > 1.0 ) m_tag_mv2c20_w_eta10_15->Fill(mv2c20);   
    else if ( fabs(jet->eta()) > 0.5 ) m_tag_mv2c20_w_eta05_10->Fill(mv2c20);   
    else                               m_tag_mv2c20_w_eta0_05->Fill(mv2c20);   

    if      ( fabs(jet->phi()) > 2.8 ) m_tag_mv2c20_w_phi28->Fill(mv2c20);
    else if ( fabs(jet->phi()) > 2.1 ) m_tag_mv2c20_w_phi21_28->Fill(mv2c20);
    else if ( fabs(jet->phi()) > 1.4 ) m_tag_mv2c20_w_phi14_21->Fill(mv2c20);   
    else if ( fabs(jet->phi()) > 0.7 ) m_tag_mv2c20_w_phi07_14->Fill(mv2c20);   
    else                               m_tag_mv2c20_w_phi0_07->Fill(mv2c20);   

    if ( mv2c20 > -0.7887 ) m_tag_mv2c20_w_phi_sum85OP->Fill(jet->phi());
    if ( mv2c20 > -0.4434 ) m_tag_mv2c20_w_phi_sum77OP->Fill(jet->phi());
    if ( mv2c20 > -0.0436 ) m_tag_mv2c20_w_phi_sum70OP->Fill(jet->phi());
    if ( mv2c20 >  0.7535 ) m_tag_mv2c20_w_phi_sum50OP->Fill(jet->phi());
    m_tag_mv2c20_w_phi_sumAll->Fill(jet->phi());

    return;

}

/* should mimick as close as possible the same function from Reconstruction/Jet/JetMonitoring/src/JetRecMonToolBase.cxx */

bool JetTagMonitoring::isGoodJet(const xAOD::Jet *jet){

  if (msgLvl(MSG::DEBUG))
    ATH_MSG_DEBUG("in dq cleaning cuts JetTagMon");

  bool pass_cuts = false;
  	
  float jetQuality      = jet->getAttribute<float>(xAOD::JetAttribute::LArQuality);
  float jetTime         = jet->getAttribute<float>(xAOD::JetAttribute::Timing);
  float hecq            = jet->getAttribute<float>(xAOD::JetAttribute::HECQuality);
  float negE            = jet->getAttribute<float>(xAOD::JetAttribute::NegativeE);
  std::vector<float> SumPtTrkPt1000;
  jet->getAttribute(xAOD::JetAttribute::SumPtTrkPt1000,SumPtTrkPt1000);
  float chf             = SumPtTrkPt1000.size() > 0 ? SumPtTrkPt1000.at(0)/jet->pt() : -1;
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


void JetTagMonitoring::fillSuspectJetHistos(const xAOD::Jet *jet) {

    m_jet_2D_suspect->Fill(jet->eta(), jet->phi(), 1.);
    m_track_selector_suspect->Fill(jet->eta(), jet->phi(), 1.);
  
    const xAOD::BTagging* btag = jet->btagging();

    double sv1ip3d = btag->SV1plusIP3D_discriminant();
    double mv2c20 = btag->auxdata<double>("MV2c20_discriminant");    
      
    m_tag_sv1ip3d_w_sj->Fill(sv1ip3d);
    m_tag_mv2c20_w_sj->Fill(mv2c20);   

    if      ( jet->pt() > 200000. ) m_tag_mv2c20_w_sj_pT200->Fill(mv2c20);   
    else if ( jet->pt() > 100000. ) m_tag_mv2c20_w_sj_pT100_200->Fill(mv2c20);   
    else if ( jet->pt() >  50000. ) m_tag_mv2c20_w_sj_pT50_100->Fill(mv2c20);   
    else if ( jet->pt() >  20000. ) m_tag_mv2c20_w_sj_pT20_50->Fill(mv2c20);   
    else if ( jet->pt() >  10000. ) m_tag_mv2c20_w_sj_pT10_20->Fill(mv2c20);   

    if      ( fabs(jet->eta()) > 2.0 ) m_tag_mv2c20_w_sj_eta20_25->Fill(mv2c20);   
    else if ( fabs(jet->eta()) > 1.5 ) m_tag_mv2c20_w_sj_eta15_20->Fill(mv2c20);   
    else if ( fabs(jet->eta()) > 1.0 ) m_tag_mv2c20_w_sj_eta10_15->Fill(mv2c20);   
    else if ( fabs(jet->eta()) > 0.5 ) m_tag_mv2c20_w_sj_eta05_10->Fill(mv2c20);   
    else                               m_tag_mv2c20_w_sj_eta0_05->Fill(mv2c20);   

    if      ( fabs(jet->phi()) > 2.8 ) m_tag_mv2c20_w_sj_phi28->Fill(mv2c20);
    else if ( fabs(jet->phi()) > 2.1 ) m_tag_mv2c20_w_sj_phi21_28->Fill(mv2c20);
    else if ( fabs(jet->phi()) > 1.4 ) m_tag_mv2c20_w_sj_phi14_21->Fill(mv2c20);   
    else if ( fabs(jet->phi()) > 0.7 ) m_tag_mv2c20_w_sj_phi07_14->Fill(mv2c20);   
    else                               m_tag_mv2c20_w_sj_phi0_07->Fill(mv2c20); 

    if ( mv2c20 > -0.7887 ) m_tag_mv2c20_w_sj_phi_sum85OP->Fill(jet->phi());
    if ( mv2c20 > -0.4434 ) m_tag_mv2c20_w_sj_phi_sum77OP->Fill(jet->phi());
    if ( mv2c20 > -0.0436 ) m_tag_mv2c20_w_sj_phi_sum70OP->Fill(jet->phi());
    if ( mv2c20 >  0.7535 ) m_tag_mv2c20_w_sj_phi_sum50OP->Fill(jet->phi());
    m_tag_mv2c20_w_sj_phi_sumAll->Fill(jet->phi());

    return;

}


void JetTagMonitoring::fillBadJetHistos(const xAOD::Jet *jet) {

    m_jet_2D_bad->Fill(jet->eta(), jet->phi(), 1.);

    return;

}

/*
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
