/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

  * Trigger Hypo Tool, that is aimed at triggering displaced vertex
  * author Kunihiro Nagano <kunihiro.nagano@cern.ch> - KEK
*/

#include "TrigCompositeUtils/TrigCompositeUtils.h"
#include "TrigCompositeUtils/HLTIdentifier.h"
#include "TrigCompositeUtils/Combinators.h"
#include "AthenaMonitoringKernel/Monitored.h"
#include "GaudiKernel/PhysicalConstants.h"
#include "PathResolver/PathResolver.h"
#include "TrigHitDVHypoTool.h"

using namespace TrigCompositeUtils;

// ------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------

TrigHitDVHypoTool::TrigHitDVHypoTool( const std::string& type, 
				      const std::string& name, 
				      const IInterface* parent ) 
   : AthAlgTool( type, name, parent ),
     m_decisionId( HLT::Identifier::fromToolName( name ) ) {}

TrigHitDVHypoTool::~TrigHitDVHypoTool() {}

// ------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------

StatusCode TrigHitDVHypoTool::initialize()
{
   if ( !m_monTool.empty() ) CHECK( m_monTool.retrieve() );

   ATH_MSG_INFO("Initialization completed successfully:");
   ATH_MSG_INFO("cutJetPtGeV         = " << m_cutJetPtGeV); 
   ATH_MSG_INFO("cutJetEta           = " << m_cutJetEta);  
   ATH_MSG_INFO("doSPseed            = " << m_doSPseed);  
   ATH_MSG_INFO("Tool configured for chain/id: " << m_decisionId);
   
   m_tmva_reader = new TMVA::Reader( "!Color:!Silent" );
   m_tmva_reader->AddVariable("n_track_qual", &m_tmva_n_track_qual);
   m_tmva_reader->AddVariable("ly0_sp_frac",  &m_tmva_ly0_sp_frac);
   m_tmva_reader->AddVariable("ly1_sp_frac",  &m_tmva_ly1_sp_frac);
   m_tmva_reader->AddVariable("ly2_sp_frac",  &m_tmva_ly2_sp_frac);
   m_tmva_reader->AddVariable("ly3_sp_frac",  &m_tmva_ly3_sp_frac);
   m_tmva_reader->AddVariable("ly4_sp_frac",  &m_tmva_ly4_sp_frac);
   m_tmva_reader->AddVariable("ly5_sp_frac",  &m_tmva_ly5_sp_frac);
   m_tmva_reader->AddVariable("ly6_sp_frac",  &m_tmva_ly6_sp_frac);
   m_tmva_reader->AddVariable("ly7_sp_frac",  &m_tmva_ly7_sp_frac);

   // --- Book the MVA methods
   const std::string tuningVer  = "v21a";
   const std::string methodName = "BDT method";

   std::string file = "TrigHitDVHypo/HitDV.BDT.weights." + tuningVer + ".xml";
   std::string weightfile = PathResolver::find_calib_file(file.c_str());
   ATH_MSG_DEBUG("opening weightfile = " << weightfile);
   m_tmva_reader->BookMVA(methodName, weightfile); 

   return StatusCode::SUCCESS;
}

// ------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------

float TrigHitDVHypoTool::deltaR(float eta_1, float phi_1, float eta_2, float phi_2) const
{
   float dPhi = phi_1 - phi_2;
   if (dPhi < -TMath::Pi()) dPhi += 2*TMath::Pi();
   if (dPhi >  TMath::Pi()) dPhi -= 2*TMath::Pi();
   float dEta = eta_1 - eta_2;
   return std::sqrt(dPhi*dPhi+dEta*dEta);
}

// ------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------

int TrigHitDVHypoTool::getSPLayer(int layer, float eta) const
{
   float abseta = std::fabs(eta);

   // Pixel barrel or SCT barrel
   if( 0<=layer && layer <=7 ) {
      ATH_MSG_DEBUG("layer=" << layer << ", eta=" << abseta);
      return layer;
   }

   int base = 0;

   // 
   const float PixBR6limit = 1.29612;
   const float PixBR5limit = 1.45204;
   const float PixBR4limit = 1.64909;
   const float PixBR3limit = 1.90036;
   const float PixBR2limit = 2.2146;

   // Pixel Endcap #1
   base = 8;
   if( layer==base || layer==(base+12) ) {
      ATH_MSG_DEBUG("Pix EC1, eta=" << abseta);
      if( abseta > PixBR2limit ) return 2;
      return 3;
   }

   // Pixel Endcap #2
   base = 9;
   if( layer==base || layer==(base+12) ) {
      ATH_MSG_DEBUG("Pix EC2, eta=" << abseta);
      if( abseta > PixBR2limit ) return 2;
      return 3;
   }

   // Pixel Endcap #3
   base = 10;
   if( layer==base || layer==(base+12) ) {
      ATH_MSG_DEBUG("Pix EC3, eta=" << abseta);
      return 3;
   }

   // SCT Endcap #1
   base = 11;
   if( layer==base || layer==(base+12) ) {
      ATH_MSG_DEBUG("Sct EC1, eta=" << abseta);
      if( abseta < PixBR6limit )      return 7;
      else if( abseta < PixBR5limit ) return 6;
      return 5;
   }

   // SCT Endcap #2
   base = 12;
   if( layer==base || layer==(base+12) ) {
      ATH_MSG_DEBUG("Sct EC2, eta=" << abseta);
      if( abseta < PixBR5limit )      return 7;
      else if( abseta < PixBR4limit ) return 6;
      return 4;
   }

   // SCT Endcap #3
   base = 13;
   if( layer==base || layer==(base+12) ) {
      ATH_MSG_DEBUG("Sct EC3, eta=" << abseta);
      if( abseta < PixBR4limit ) return 7;
      return 5;
   }

   // SCT Endcap #4
   base = 14;
   if( layer==base || layer==(base+12) ) {
      ATH_MSG_DEBUG("Sct EC4, eta=" << abseta);
      if( abseta < PixBR4limit ) return 6;
      else if( abseta < PixBR3limit ) return 6;
      return 4;
   }

   // SCT Endcap #5
   base = 15;
   if( layer==base || layer==(base+12) ) {
      ATH_MSG_DEBUG("Sct EC5, eta=" << abseta);
      if( abseta < PixBR3limit ) return 7;
      return 5;
   }

   // SCT Endcap #6
   base = 16;
   if( layer==base || layer==(base+12) ) {
      ATH_MSG_DEBUG("Sct EC6, eta=" << abseta);
      if( abseta < PixBR3limit ) return 6;
      return 4;
   }

   // SCT Endcap #7
   base = 17;
   if( layer==base || layer==(base+12) ) {
      ATH_MSG_DEBUG("Sct EC7, eta=" << abseta);
      if( abseta < PixBR3limit ) return 7;
      return 5;
   }

   // SCT Endcap #8
   base = 18;
   if( layer==base || layer==(base+12) ) {
      ATH_MSG_DEBUG("Sct EC8, eta=" << abseta);
      if( abseta < PixBR3limit ) return 7;
      return 6;
   }

   // SCT Endcap #9
   base = 19;
   if( layer==base || layer==(base+12) ) {
      ATH_MSG_DEBUG("Sct EC9, eta=" << abseta);
      return 7;
   }

   return 0;
}

// ------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------

StatusCode TrigHitDVHypoTool::calculateBDT(const xAOD::TrigCompositeContainer* spsContainer,
					   const xAOD::TrigCompositeContainer* trksContainer,
					   const std::vector<float>& seeds_eta, const std::vector<float>& seeds_phi,
					   const float cutBDTthreshold, const int seed_type,
					   xAOD::TrigCompositeContainer* dvContainer, int& n_passed) const
{
   if( seeds_eta.size() != seeds_phi.size() ) return StatusCode::SUCCESS;
   n_passed = 0;

   // monitoring
   std::vector<float> mnt_eta1_ly0_spfr;
   std::vector<float> mnt_eta1_ly1_spfr;
   std::vector<float> mnt_eta1_ly2_spfr;
   std::vector<float> mnt_eta1_ly3_spfr;
   std::vector<float> mnt_eta1_ly4_spfr;
   std::vector<float> mnt_eta1_ly5_spfr;
   std::vector<float> mnt_eta1_ly6_spfr;
   std::vector<float> mnt_eta1_ly7_spfr;
   std::vector<int>   mnt_eta1_n_qtrk;
   std::vector<float> mnt_eta1_bdtscore;
   std::vector<float> mnt_1eta2_ly0_spfr;
   std::vector<float> mnt_1eta2_ly1_spfr;
   std::vector<float> mnt_1eta2_ly2_spfr;
   std::vector<float> mnt_1eta2_ly3_spfr;
   std::vector<float> mnt_1eta2_ly4_spfr;
   std::vector<float> mnt_1eta2_ly5_spfr;
   std::vector<float> mnt_1eta2_ly6_spfr;
   std::vector<float> mnt_1eta2_ly7_spfr;
   std::vector<int>   mnt_1eta2_n_qtrk;
   std::vector<float> mnt_1eta2_bdtscore;
   auto mon_eta1_ly0_spfr  = Monitored::Collection("eta1_ly0_spfr",   mnt_eta1_ly0_spfr);
   auto mon_eta1_ly1_spfr  = Monitored::Collection("eta1_ly1_spfr",   mnt_eta1_ly1_spfr);
   auto mon_eta1_ly2_spfr  = Monitored::Collection("eta1_ly2_spfr",   mnt_eta1_ly2_spfr);
   auto mon_eta1_ly3_spfr  = Monitored::Collection("eta1_ly3_spfr",   mnt_eta1_ly3_spfr);
   auto mon_eta1_ly4_spfr  = Monitored::Collection("eta1_ly4_spfr",   mnt_eta1_ly4_spfr);
   auto mon_eta1_ly5_spfr  = Monitored::Collection("eta1_ly5_spfr",   mnt_eta1_ly5_spfr);
   auto mon_eta1_ly6_spfr  = Monitored::Collection("eta1_ly6_spfr",   mnt_eta1_ly6_spfr);
   auto mon_eta1_ly7_spfr  = Monitored::Collection("eta1_ly7_spfr",   mnt_eta1_ly7_spfr);
   auto mon_eta1_n_qtrk    = Monitored::Collection("eta1_n_qtrk",     mnt_eta1_n_qtrk);
   auto mon_eta1_bdtscore  = Monitored::Collection("eta1_bdtscore",   mnt_eta1_bdtscore);
   auto mon_1eta2_ly0_spfr = Monitored::Collection("1eta2_ly0_spfr",  mnt_1eta2_ly0_spfr);
   auto mon_1eta2_ly1_spfr = Monitored::Collection("1eta2_ly1_spfr",  mnt_1eta2_ly1_spfr);
   auto mon_1eta2_ly2_spfr = Monitored::Collection("1eta2_ly2_spfr",  mnt_1eta2_ly2_spfr);
   auto mon_1eta2_ly3_spfr = Monitored::Collection("1eta2_ly3_spfr",  mnt_1eta2_ly3_spfr);
   auto mon_1eta2_ly4_spfr = Monitored::Collection("1eta2_ly4_spfr",  mnt_1eta2_ly4_spfr);
   auto mon_1eta2_ly5_spfr = Monitored::Collection("1eta2_ly5_spfr",  mnt_1eta2_ly5_spfr);
   auto mon_1eta2_ly6_spfr = Monitored::Collection("1eta2_ly6_spfr",  mnt_1eta2_ly6_spfr);
   auto mon_1eta2_ly7_spfr = Monitored::Collection("1eta2_ly7_spfr",  mnt_1eta2_ly7_spfr);
   auto mon_1eta2_n_qtrk   = Monitored::Collection("1eta2_n_qtrk",    mnt_1eta2_n_qtrk);
   auto mon_1eta2_bdtscore = Monitored::Collection("1eta2_bdtscore",  mnt_1eta2_bdtscore);
   auto monitorIt = Monitored::Group( m_monTool, 
				      mon_eta1_ly0_spfr, mon_eta1_ly1_spfr, mon_eta1_ly2_spfr, mon_eta1_ly3_spfr,
				      mon_eta1_ly4_spfr, mon_eta1_ly5_spfr, mon_eta1_ly6_spfr, mon_eta1_ly7_spfr,
				      mon_eta1_n_qtrk, mon_eta1_bdtscore,
				      mon_1eta2_ly0_spfr, mon_1eta2_ly1_spfr, mon_1eta2_ly2_spfr, mon_1eta2_ly3_spfr,
				      mon_1eta2_ly4_spfr, mon_1eta2_ly5_spfr, mon_1eta2_ly6_spfr, mon_1eta2_ly7_spfr,
				      mon_1eta2_n_qtrk, mon_1eta2_bdtscore);

   for(unsigned int iseed=0; iseed<seeds_eta.size(); iseed++) {
      
      float seed_eta = seeds_eta[iseed];
      float seed_phi = seeds_phi[iseed];

      ATH_MSG_DEBUG("+++++ seed eta: " << seed_eta << ", phi:" << seed_phi << " +++++");

      // loop on space points
      const int   N_LAYER = 8;
      const float DR_TO_REF_CUT = 0.4;

      int n_sp_injet = 0;           int n_pixsp_injet = 0;           int n_sctsp_injet = 0;
      int n_sp_injet_usedByTrk = 0; int n_pixsp_injet_usedByTrk = 0; int n_sctsp_injet_usedByTrk = 0;
      int v_n_sp_injet[N_LAYER];
      int v_n_sp_injet_usedByTrk[N_LAYER];
      for(int i=0; i<N_LAYER; i++) { v_n_sp_injet[i]=0; v_n_sp_injet_usedByTrk[i]=0; }
      
      for ( auto spData : *spsContainer ) {
	 // match within dR
	 float sp_eta = spData->getDetail<float>("sp_eta");
	 float sp_phi = spData->getDetail<float>("sp_phi");
	 float dr = deltaR(sp_eta,sp_phi,seed_eta,seed_phi);
	 if( dr > DR_TO_REF_CUT ) continue;

	 // 
	 int sp_isPix = spData->getDetail<int>("sp_isPix");
	 int sp_isSct = spData->getDetail<int>("sp_isSct");
	 bool isPix = (sp_isPix == 1);
	 bool isSct = (sp_isSct == 1);
	 
	 int sp_layer = spData->getDetail<int>("sp_layer");
	 int sp_trkid = spData->getDetail<int>("sp_usedTrkId");
	 bool isUsedByTrk = (sp_trkid != -1);

	 int ilayer = getSPLayer(sp_layer,sp_eta);
	 
	 if( ilayer<=7  ) { // Pixel barrel or Sct barrel
	    n_sp_injet++;
	    v_n_sp_injet[ilayer]++;
	    if( isPix ) n_pixsp_injet++;
	    if( isSct ) n_sctsp_injet++;
	    if( isUsedByTrk ) {
	       n_sp_injet_usedByTrk++;
	       v_n_sp_injet_usedByTrk[ilayer]++;
	       if( isPix ) n_pixsp_injet_usedByTrk++;
	       if( isSct ) n_sctsp_injet_usedByTrk++;
	    }
	 }
      }
      ATH_MSG_DEBUG("nr of SPs in jet: usedByTrk / all = " << n_sp_injet_usedByTrk << " / " << n_sp_injet);
      float v_ly_sp_frac[N_LAYER];
      for(int i=0; i<N_LAYER; i++) {
	 float frac = 0.;
	 if( v_n_sp_injet[i] > 0 ) frac = 1.0 - (float)v_n_sp_injet_usedByTrk[i] / (float)v_n_sp_injet[i];
	 v_ly_sp_frac[i] = frac;
	 if( std::abs(seed_eta) < 1.0 ) {
	    if     ( i==0 ) { mnt_eta1_ly0_spfr.push_back(frac); }
	    else if( i==1 ) { mnt_eta1_ly1_spfr.push_back(frac); }
	    else if( i==2 ) { mnt_eta1_ly2_spfr.push_back(frac); }
	    else if( i==3 ) { mnt_eta1_ly3_spfr.push_back(frac); }
	    else if( i==4 ) { mnt_eta1_ly4_spfr.push_back(frac); }
	    else if( i==5 ) { mnt_eta1_ly5_spfr.push_back(frac); }
	    else if( i==6 ) { mnt_eta1_ly6_spfr.push_back(frac); }
	    else if( i==7 ) { mnt_eta1_ly7_spfr.push_back(frac); }
	 }
	 else if( std::abs(seed_eta) < 2.0 ) {
	    if     ( i==0 ) { mnt_1eta2_ly0_spfr.push_back(frac); }
	    else if( i==1 ) { mnt_1eta2_ly1_spfr.push_back(frac); }
	    else if( i==2 ) { mnt_1eta2_ly2_spfr.push_back(frac); }
	    else if( i==3 ) { mnt_1eta2_ly3_spfr.push_back(frac); }
	    else if( i==4 ) { mnt_1eta2_ly4_spfr.push_back(frac); }
	    else if( i==5 ) { mnt_1eta2_ly5_spfr.push_back(frac); }
	    else if( i==6 ) { mnt_1eta2_ly6_spfr.push_back(frac); }
	    else if( i==7 ) { mnt_1eta2_ly7_spfr.push_back(frac); }
	 }
	 ATH_MSG_DEBUG("Layer " << i << ": frac=" << v_ly_sp_frac[i] << ", n used / all = " << v_n_sp_injet_usedByTrk[i] << " / " << v_n_sp_injet[i]);
      }

      // loop on tracks
      const float TRK_PT_CUT = 2.0;

      unsigned int n_qtrk_injet = 0;
      for ( auto trk : *trksContainer ) {
	 float trk_pt  = trk->getDetail<float>("dvtrk_pt");
	 trk_pt /= 1000;
	 if( trk_pt < TRK_PT_CUT ) continue;
	 float trk_eta = trk->getDetail<float>("dvtrk_eta");
	 float trk_phi = trk->getDetail<float>("dvtrk_phi");
	 float dr = deltaR(trk_eta,trk_phi,seed_eta,seed_phi);
	 if( dr > DR_TO_REF_CUT )  continue;
	 n_qtrk_injet++;
      }
      ATH_MSG_DEBUG("nr of quality tracks in jet = " << n_qtrk_injet);
      if     ( std::abs(seed_eta) < 1.0 ) { mnt_eta1_n_qtrk.push_back(n_qtrk_injet);  }
      else if( std::abs(seed_eta) < 2.0 ) { mnt_1eta2_n_qtrk.push_back(n_qtrk_injet); }

      // evaluate BDT
      m_tmva_n_track_qual = (float)n_qtrk_injet;
      m_tmva_ly0_sp_frac  = v_ly_sp_frac[0];
      m_tmva_ly1_sp_frac  = v_ly_sp_frac[1];
      m_tmva_ly2_sp_frac  = v_ly_sp_frac[2];
      m_tmva_ly3_sp_frac  = v_ly_sp_frac[3];
      m_tmva_ly4_sp_frac  = v_ly_sp_frac[4];
      m_tmva_ly5_sp_frac  = v_ly_sp_frac[5];
      m_tmva_ly6_sp_frac  = v_ly_sp_frac[6];
      m_tmva_ly7_sp_frac  = v_ly_sp_frac[7];
      float bdt_score = m_tmva_reader->EvaluateMVA("BDT method");
      if     ( std::abs(seed_eta) < 1.0 ) { mnt_eta1_bdtscore.push_back(bdt_score); }
      else if( std::abs(seed_eta) < 2.0 ) { mnt_1eta2_bdtscore.push_back(bdt_score); }

      // BDT threshold
      if( bdt_score < cutBDTthreshold ) continue;

      // passed selection
      ATH_MSG_DEBUG("Passed selection");
      n_passed++;

      // create EDM object
      xAOD::TrigComposite *dv = new xAOD::TrigComposite();
      dv->makePrivateStore();
      dvContainer->push_back(dv);
      
      dv->setDetail<float>("hitdv_seed_eta",     seed_eta);
      dv->setDetail<float>("hitdv_seed_phi",     seed_phi);
      dv->setDetail<int>  ("hitdv_seed_type",    seed_type);
      dv->setDetail<int>  ("hitdv_n_track_qual", n_qtrk_injet);
      dv->setDetail<float>("hitdv_ly0_sp_frac",  v_ly_sp_frac[0]);
      dv->setDetail<float>("hitdv_ly1_sp_frac",  v_ly_sp_frac[1]);
      dv->setDetail<float>("hitdv_ly2_sp_frac",  v_ly_sp_frac[2]);
      dv->setDetail<float>("hitdv_ly3_sp_frac",  v_ly_sp_frac[3]);
      dv->setDetail<float>("hitdv_ly4_sp_frac",  v_ly_sp_frac[4]);
      dv->setDetail<float>("hitdv_ly5_sp_frac",  v_ly_sp_frac[5]);
      dv->setDetail<float>("hitdv_ly6_sp_frac",  v_ly_sp_frac[6]);
      dv->setDetail<float>("hitdv_ly7_sp_frac",  v_ly_sp_frac[7]);
      dv->setDetail<float>("hitdv_bdt_score",    bdt_score);

      ATH_MSG_DEBUG("Created a new entry EDM");
   }
   ATH_MSG_DEBUG("nr of BDT passed = " << n_passed);

   // 
   return StatusCode::SUCCESS;
}

// ------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------

StatusCode TrigHitDVHypoTool::findJetSeeds(const xAOD::JetContainer* jetsContainer, const float cutJetPt, const float cutJetEta,
					   std::vector<float>& jetSeeds_eta, std::vector<float>& jetSeeds_phi) const
{
   std::vector<float> mnt_jet_pt;
   std::vector<float> mnt_jet_eta;
   auto mon_jet_pt  = Monitored::Collection("jet_pt",  mnt_jet_pt);
   auto mon_jet_eta = Monitored::Collection("jet_eta", mnt_jet_eta);
   auto monitorIt   = Monitored::Group( m_monTool, mon_jet_pt, mon_jet_eta );

   ATH_MSG_DEBUG("looking for jet seed with pt cut=" << cutJetPt << ", eta cut=" << cutJetEta);
   for ( const xAOD::Jet* jet : *jetsContainer ) {
      float jet_pt  = (float)jet->pt() / 1000.0;
      mnt_jet_pt.push_back(jet_pt);
      if( jet_pt < cutJetPt ) {
	 ATH_MSG_DEBUG("Fails jet pt cut, pt = " << jet_pt);
	 continue;
      }
      float jet_eta = (float)jet->eta();
      mnt_jet_eta.push_back(jet_eta);
      if( std::fabs(jet_eta) > cutJetEta ) {
	 ATH_MSG_DEBUG("Fails jet eta cut, eta = " << jet_eta);
	 continue;
      }
      float jet_phi = (float)jet->phi();
      jetSeeds_eta.push_back(jet_eta);
      jetSeeds_phi.push_back(jet_phi);
   }
   ATH_MSG_DEBUG("nr of jet seeds=" << jetSeeds_eta.size());

   return StatusCode::SUCCESS;
}

// ------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------

StatusCode TrigHitDVHypoTool::findSPSeeds( const xAOD::TrigCompositeContainer* spsContainer,
					   std::vector<float>& seeds_eta, std::vector<float>& seeds_phi ) const
{
   seeds_eta.clear();
   seeds_phi.clear();

   const int   NBINS_ETA = 50;
   const float ETA_MIN   = -2.5; 
   const float ETA_MAX   =  2.5;

   const int   NBINS_PHI = 80;
   const float PHI_MIN   = -4.0; 
   const float PHI_MAX   =  4.0;

   std::string hname;

   hname = "ly6_h2_nsp";
   TH2F*    ly6_h2_nsp = new TH2F(hname.c_str(),hname.c_str(),NBINS_ETA,ETA_MIN,ETA_MAX,NBINS_PHI,PHI_MIN,PHI_MAX);
   hname = "ly7_h2_nsp";
   TH2F*    ly7_h2_nsp = new TH2F(hname.c_str(),hname.c_str(),NBINS_ETA,ETA_MIN,ETA_MAX,NBINS_PHI,PHI_MIN,PHI_MAX);

   hname = "ly6_h2_nsp_notrk";
   TH2F*    ly6_h2_nsp_notrk = new TH2F(hname.c_str(),hname.c_str(),NBINS_ETA,ETA_MIN,ETA_MAX,NBINS_PHI,PHI_MIN,PHI_MAX);
   hname = "ly7_h2_nsp_notrk";
   TH2F*    ly7_h2_nsp_notrk = new TH2F(hname.c_str(),hname.c_str(),NBINS_ETA,ETA_MIN,ETA_MAX,NBINS_PHI,PHI_MIN,PHI_MAX);

   for ( auto spData : *spsContainer ) {
      int sp_layer = spData->getDetail<int>  ("sp_layer");
      float sp_eta = spData->getDetail<float>("sp_eta");
      int ilayer = getSPLayer(sp_layer,sp_eta);
      if( ilayer<6 ) continue;

      int sp_trkid = spData->getDetail<int>("sp_usedTrkId");
      bool isUsedByTrk = (sp_trkid != -1);
      float sp_phi = spData->getDetail<float>("sp_phi");

      bool fill_out_of_pi = false;
      float sp_phi2 = 0;
      if( sp_phi < 0 ) {
	 sp_phi2 = 2*TMath::Pi() + sp_phi;
	 if( sp_phi2 < PHI_MAX ) fill_out_of_pi = true;
      }
      else {
	 sp_phi2 = -2*TMath::Pi() + sp_phi;
	 if( PHI_MIN < sp_phi2 ) fill_out_of_pi = true;
      }
      if( ilayer==6 ) {
	                      ly6_h2_nsp->Fill(sp_eta,sp_phi);
	 if( fill_out_of_pi ) ly6_h2_nsp->Fill(sp_eta,sp_phi2);
	 if( ! isUsedByTrk )                  ly6_h2_nsp_notrk->Fill(sp_eta,sp_phi);
	 if( ! isUsedByTrk && fill_out_of_pi) ly6_h2_nsp_notrk->Fill(sp_eta,sp_phi2);
      }
      if( ilayer==7 ) {
	                      ly7_h2_nsp->Fill(sp_eta,sp_phi);
	 if( fill_out_of_pi ) ly7_h2_nsp->Fill(sp_eta,sp_phi2);
	 if( ! isUsedByTrk )                  ly7_h2_nsp_notrk->Fill(sp_eta,sp_phi);
	 if( ! isUsedByTrk && fill_out_of_pi) ly7_h2_nsp_notrk->Fill(sp_eta,sp_phi2);
      }
   }

   ATH_MSG_DEBUG("looking for ly6/ly6 doublet seeds");

   // (idx, sort/weight, eta, phi)
   std::vector<std::tuple<int,float,float,float>> QT;

   for(int ly6_ieta=1; ly6_ieta<=NBINS_ETA; ly6_ieta++) {
      float ly6_eta = (ly6_h2_nsp->GetXaxis()->GetBinLowEdge(ly6_ieta) + ly6_h2_nsp->GetXaxis()->GetBinUpEdge(ly6_ieta))/2.0;
      for(int ly6_iphi=1; ly6_iphi<=NBINS_PHI; ly6_iphi++) {
	 float ly6_phi = (ly6_h2_nsp->GetYaxis()->GetBinLowEdge(ly6_iphi) + ly6_h2_nsp->GetYaxis()->GetBinUpEdge(ly6_iphi))/2.0;

	 float ly6_nsp       = ly6_h2_nsp      ->GetBinContent(ly6_ieta,ly6_iphi);
	 float ly6_nsp_notrk = ly6_h2_nsp_notrk->GetBinContent(ly6_ieta,ly6_iphi);
	 float ly6_frac      = ( ly6_nsp > 0 ) ? ly6_nsp_notrk / ly6_nsp : 0;
	 if( ly6_nsp < 10 || ly6_frac < 0.85 ) continue;

	 float ly7_frac_max = 0;
	 float ly7_eta_max  = 0;
	 float ly7_phi_max  = 0;
	 for(int ly7_ieta=std::max(1,ly6_ieta-1); ly7_ieta<std::min(NBINS_ETA,ly6_ieta+1); ly7_ieta++) {
	    for(int ly7_iphi=std::max(1,ly6_iphi-1); ly7_iphi<=std::min(NBINS_PHI,ly6_iphi+1); ly7_iphi++) {
	       float ly7_nsp       = ly7_h2_nsp      ->GetBinContent(ly7_ieta,ly7_iphi);
	       float ly7_nsp_notrk = ly7_h2_nsp_notrk->GetBinContent(ly7_ieta,ly7_iphi);
	       float ly7_frac      = ( ly7_nsp > 0 ) ? ly7_nsp_notrk / ly7_nsp : 0;
	       if( ly7_nsp < 10 ) continue;
	       if( ly7_frac > ly7_frac_max ) {
		  ly7_frac_max = ly7_frac;
		  ly7_eta_max  = (ly7_h2_nsp->GetXaxis()->GetBinLowEdge(ly7_ieta) + ly7_h2_nsp->GetXaxis()->GetBinUpEdge(ly7_ieta))/2.0;
		  ly7_phi_max  = (ly7_h2_nsp->GetXaxis()->GetBinLowEdge(ly7_iphi) + ly7_h2_nsp->GetXaxis()->GetBinUpEdge(ly7_iphi))/2.0;
	       }
	    }
	 }
	 if( ly7_frac_max < 0.85 ) continue;
	 //
	 float wsum = ly6_frac + ly7_frac_max;
	 float weta = (ly6_eta*ly6_frac + ly7_eta_max*ly7_frac_max) / wsum;
	 float wphi = (ly6_phi*ly6_frac + ly7_phi_max*ly7_frac_max) / wsum;
	 float w = wsum / 2.0;
	 QT.push_back(std::make_tuple(-1,w,weta,wphi));
      }
   }
   delete ly6_h2_nsp;
   delete ly7_h2_nsp;
   delete ly6_h2_nsp_notrk;
   delete ly7_h2_nsp_notrk;
   ATH_MSG_DEBUG("nr of ly6/ly7 doublet candidate seeds=" << QT.size() << ", doing clustering...");

   // sort
   std::sort(QT.begin(), QT.end(),
	     [](const std::tuple<int,float,float,float>& lhs, const std::tuple<int,float,float,float>& rhs) {
		return std::get<1>(lhs) > std::get<1>(rhs); } );
   
   // clustering
   const double CLUSTCUT_DIST      = 0.2;
   const double CLUSTCUT_SEED_FRAC = 0.9;

   std::vector<float> seeds_wsum;

   for(unsigned int i=0; i<QT.size(); i++) {
      float phi  = std::get<3>(QT[i]);
      float eta  = std::get<2>(QT[i]);
      float w    = std::get<1>(QT[i]);
      if(i==0) {
	 seeds_eta.push_back(w*eta); seeds_phi.push_back(w*phi);
	 seeds_wsum.push_back(w);
	 continue;
      }
      const int IDX_INITIAL = 100;
      float dist_min = 100.0;
      int idx_min     = IDX_INITIAL;
      for(unsigned j=0; j<seeds_eta.size(); j++) {
	 float ceta = seeds_eta[j]/seeds_wsum[j];
	 float cphi = seeds_phi[j]/seeds_wsum[j];
	 // intentionally calculate in this way as phi is defined beyond -Pi/Pi (no boundary)
	 float deta = std::fabs(ceta-eta);
	 float dphi = std::fabs(cphi-phi);
	 float dist = std::sqrt(dphi*dphi+deta*deta);
	 if( dist < dist_min ) {
	    dist_min = dist;
	    idx_min  = j; 
	 }
      }
      int match_idx = IDX_INITIAL;
      if( idx_min != IDX_INITIAL ) {
	 if( dist_min < CLUSTCUT_DIST ) { match_idx = idx_min; }
      }
      if( match_idx == IDX_INITIAL ) {
	 if( w > CLUSTCUT_SEED_FRAC && dist_min > CLUSTCUT_DIST ) {
	    seeds_eta.push_back(w*eta); seeds_phi.push_back(w*phi);
	    seeds_wsum.push_back(w);
	 }
	 continue;
      }
      float new_eta   = seeds_eta[match_idx]  + w*eta;
      float new_phi   = seeds_phi[match_idx]  + w*phi;
      float new_wsum  = seeds_wsum[match_idx] + w;
      seeds_eta[match_idx]   = new_eta;
      seeds_phi[match_idx]   = new_phi;
      seeds_wsum[match_idx]  = new_wsum;
   }
   QT.clear();
   for(unsigned int i=0; i<seeds_eta.size(); i++) {
      float eta = seeds_eta[i] / seeds_wsum[i];
      float phi = seeds_phi[i] / seeds_wsum[i];
      seeds_eta[i] = eta;
      seeds_phi[i] = phi;
      if( phi < -TMath::Pi() ) phi =  2*TMath::Pi() + phi;
      if( phi >  TMath::Pi() ) phi = -2*TMath::Pi() + phi;
      seeds_phi[i] = phi;
   }
   ATH_MSG_DEBUG("after clustering, nr of seeds = " << seeds_eta.size());

   // delete overlap (can happen at phi=-Pi/Pi bounadry)
   std::vector<unsigned int> idx_to_delete;
   for(unsigned int i=0; i<seeds_eta.size(); i++) {
      if( std::find(idx_to_delete.begin(),idx_to_delete.end(),i) != idx_to_delete.end() ) continue;
      float eta_i = seeds_eta[i];
      float phi_i = seeds_phi[i];
      for(unsigned int j=i+1; j<seeds_eta.size(); j++) {
	 if( std::find(idx_to_delete.begin(),idx_to_delete.end(),j) != idx_to_delete.end() ) continue;
	 float eta_j = seeds_eta[j];
	 float phi_j = seeds_phi[j];
	 float dr = deltaR(eta_i,phi_i,eta_j,phi_j);
	 if( dr < CLUSTCUT_DIST ) idx_to_delete.push_back(j);
      }
   }
   ATH_MSG_DEBUG("nr of duplicated seeds to be removed = " << idx_to_delete.size());
   if( idx_to_delete.size() > 0 ) {
      std::sort(idx_to_delete.begin(),idx_to_delete.end());
      for(unsigned int j=idx_to_delete.size(); j>0; j--) {
	 unsigned int idx = idx_to_delete[j-1];
	 seeds_eta.erase(seeds_eta.begin()+idx);
	 seeds_phi.erase(seeds_phi.begin()+idx);
      }
   }

   ATH_MSG_DEBUG("nr of ly6/ly7 seeds=" << seeds_eta.size());

   // return
   return StatusCode::SUCCESS;
}

// ------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------

float TrigHitDVHypoTool::getBDTthreshold(float mu) const
{
   const float defaultBDTthreshold = 0.03773; // 70% effi at mu=60
   
   if( mu<10 || 60<mu ) return defaultBDTthreshold;

   // pol4 fitting
   const float par0 =  0.404727;
   const float par1 = -0.00344154;
   const float par2 = -6.54218e-05;
   const float par3 = -3.39841e-07;
   const float par4 =  9.93062e-09;

   float thres = par0 + par1*mu + par2*mu*mu + par3*mu*mu*mu + par4*mu*mu*mu*mu;
   return thres;
}

// ------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------

StatusCode TrigHitDVHypoTool::decide( TrackInfo& input ) const
{
   // monitoring
   auto mon_n_jetseeds   = Monitored::Scalar( "n_jetseeds",   -999. );
   auto mon_n_spseeds    = Monitored::Scalar( "n_spseeds",    -999. );
   auto mon_n_passed_jet = Monitored::Scalar( "n_passed_jet", -999. );
   auto mon_n_passed_sp  = Monitored::Scalar( "n_passed_sp",  -999. );
   auto mon_average_mu   = Monitored::Scalar( "average_mu",   -999. );
   auto monitorIt        = Monitored::Group( m_monTool, mon_n_jetseeds, mon_n_spseeds, mon_n_passed_jet, mon_n_passed_sp, mon_average_mu );

   // input containers
   float                               averageMu      = input.averageMu;
   const xAOD::JetContainer*           jetsContainer  = input.jetsContainer;
   const xAOD::TrigCompositeContainer* spsContainer   = input.spsContainer;
   const xAOD::TrigCompositeContainer* trksContainer  = input.trksContainer;
   xAOD::TrigCompositeContainer*       dvContainer    = input.dvContainer;
   mon_average_mu = averageMu;

   // BDT selection
   const int   cutIndex = 0;

   // find seeds based on HLT jets
   std::vector<float> jetSeeds_eta;
   std::vector<float> jetSeeds_phi;
   ATH_CHECK( findJetSeeds(jetsContainer, m_cutJetPtGeV[cutIndex], m_cutJetEta[cutIndex], jetSeeds_eta, jetSeeds_phi) );
   mon_n_jetseeds = jetSeeds_eta.size();

   // jet presence is required also for SP seed
   if( jetSeeds_eta.size() < 1 ) {
      mon_n_passed_jet = 0;
      return StatusCode::SUCCESS;
   }

   // find seeds based on SP frac itself
   std::vector<float> spSeeds_eta;
   std::vector<float> spSeeds_phi;
   if( m_doSPseed[cutIndex] ) {
      ATH_CHECK( findSPSeeds(spsContainer, spSeeds_eta, spSeeds_phi) );
      mon_n_spseeds = spSeeds_eta.size();
   }

   // BDT 
   float BDTthreshold = getBDTthreshold(averageMu);

   int n_passed_jet = 0;
   int seed_type = 0;
   ATH_CHECK( calculateBDT(spsContainer, trksContainer, jetSeeds_eta, jetSeeds_phi, BDTthreshold, seed_type, dvContainer, n_passed_jet) );
   mon_n_passed_jet = n_passed_jet;

   int n_passed_sp = 0;
   if( m_doSPseed[cutIndex] ) {
      seed_type = 1;
      ATH_CHECK( calculateBDT(spsContainer, trksContainer, spSeeds_eta, spSeeds_phi,BDTthreshold, seed_type, dvContainer, n_passed_sp) );
      mon_n_passed_sp = n_passed_sp;
   }

   // final
   if( (n_passed_jet+n_passed_sp) > 0 ) {
      addDecisionID( m_decisionId.numeric(), input.decision );
      ATH_MSG_DEBUG("REGTEST event accepted");
      return StatusCode::SUCCESS;
   }
   else {
      return StatusCode::SUCCESS;
   }
}

// ------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------
