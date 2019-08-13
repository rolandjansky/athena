/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "InDetVKalVxInJetTool/InDetTrkInJetType.h"
#include "TMVA/MethodBDT.h"
#include "TMVA/Reader.h"
#include "PathResolver/PathResolver.h"
#include "TLorentzVector.h"
#include "TrkVKalVrtFitter/TrkVKalVrtFitter.h"

#include "Particle/TrackParticle.h"
#include "MVAUtils/BDT.h" 
#include "GaudiKernel/IChronoStatSvc.h"
//
//-------------------------------------------------
namespace InDet {
//
//Constructor-------------------------------------------------------------- 
InDetTrkInJetType::InDetTrkInJetType(const std::string& type,
                                           const std::string& name,
                                           const IInterface* parent):
  base_class(type,name,parent),
  m_tmvaReader(nullptr),
  m_localBDT(nullptr),
  m_trkSctHitsCut(4),
  m_trkPixelHitsCut(1),
  m_trkChi2Cut(5.),
  m_trkMinPtCut(700.),
  m_jetMaxPtCut(7000000.),
  m_jetMinPtCut(  35000.),
  m_d0_limLow(-3.),
  m_d0_limUpp( 5.),
  m_Z0_limLow(-8.),
  m_Z0_limUpp(12.),
  m_calibFileName("TrackClassif_3cl.v02.xml"),
  m_fitterSvc("Trk::TrkVKalVrtFitter/VertexFitterTool",this)
  {
     declareProperty("trkSctHits",   m_trkSctHitsCut   ,  "Cut on track SCT hits number" );
     declareProperty("trkPixelHits", m_trkPixelHitsCut ,  "Cut on track Pixel hits number" );
     declareProperty("trkChi2",   m_trkChi2Cut   ,  "Cut on track Chi2/Ndf" );
     declareProperty("trkMinPt",  m_trkMinPtCut  ,  "Minimal track Pt cut" );
     declareProperty("jetMaxPt",  m_jetMaxPtCut  ,  "Maximal jet Pt cut" );
     declareProperty("jetMinPt",  m_jetMinPtCut  ,  "Minimal jet Pt cut from training" );
     declareProperty("d0_limLow", m_d0_limLow    ,  "Low d0 impact cut" );
     declareProperty("d0_limUpp", m_d0_limUpp    ,  "Upper d0 impact cut" );
     declareProperty("Z0_limLow", m_Z0_limLow    ,  "Low Z0 impact cut" );
     declareProperty("Z0_limUpp", m_Z0_limUpp    ,  "Upper Z0 impact cut" );
     m_timingProfile=nullptr;
  }

//Destructor---------------------------------------------------------------
  InDetTrkInJetType::~InDetTrkInJetType(){
    if(m_tmvaReader)delete m_tmvaReader;
    if(m_localBDT)delete m_localBDT;
    ATH_MSG_DEBUG("InDetTrkInJetType destructor called");
  }

//Initialize---------------------------------------------------------------
   StatusCode InDetTrkInJetType::initialize(){
     m_initialised = 0;
     m_tmvaReader = new TMVA::Reader();
     //m_tmvaReader->AddVariable( "prbS",  &m_prbS );
     m_tmvaReader->AddVariable( "Sig3D",  &m_Sig3D );
     m_tmvaReader->AddVariable( "prbP",   &m_prbP );
     m_tmvaReader->AddVariable( "pTvsJet",&m_pTvsJet );
     //m_tmvaReader->AddVariable( "prodTJ", &m_prodTJ );
     m_tmvaReader->AddVariable( "d0",     &m_d0 );
     m_tmvaReader->AddVariable( "SigR",   &m_SigR );
     m_tmvaReader->AddVariable( "SigZ",   &m_SigZ );
     m_tmvaReader->AddVariable( "ptjet",  &m_ptjet );
     m_tmvaReader->AddVariable( "ibl"   , &m_ibl );
     m_tmvaReader->AddVariable( "bl"   ,  &m_bl );
     m_tmvaReader->AddVariable( "etatrk", &m_etatrk );
//
//-- Calibration file
//
     std::string fullPathToFile = PathResolverFindCalibFile("InDetVKalVxInJetTool/"+m_calibFileName);
     if(fullPathToFile != ""){
        if(msgLvl(MSG::DEBUG))msg(MSG::DEBUG) <<"TrackClassification calibration file" << fullPathToFile << endmsg;
        m_tmvaReader->BookMVA("BDTG", fullPathToFile);
        TMVA::MethodBDT* method_bdt = dynamic_cast<TMVA::MethodBDT*> (m_tmvaReader->FindMVA("BDTG"));
	if(!method_bdt){    ATH_MSG_DEBUG("Error! No method_BDT for TrackClassification!");
                            return StatusCode::SUCCESS;  }
        bool useYesNoLeaf = false;
        bool isGrad       = false;
        if(method_bdt->GetOptions().Contains("UseYesNoLeaf=True")) useYesNoLeaf = true;
        if(method_bdt->GetOptions().Contains("BoostType=Grad")) isGrad = true;
        m_localBDT = new MVAUtils::BDT( method_bdt, isGrad, useYesNoLeaf);
	if(!m_localBDT){   ATH_MSG_DEBUG("Error! No_BDT from MVAUtils created");
                           return StatusCode::SUCCESS; }
     }else{
        ATH_MSG_DEBUG("Error! No calibration for TrackClassification found.");
        return StatusCode::SUCCESS;
     }    
     //-------
     if (m_fitterSvc.retrieve().isFailure()) {
        ATH_MSG_DEBUG("Could not find Trk::TrkVKalVrtFitter");
        return StatusCode::SUCCESS;
     } else {
        ATH_MSG_DEBUG("InDetTrkInJetTool TrkVKalVrtFitter found");
     }
     m_fitSvc = dynamic_cast<Trk::TrkVKalVrtFitter*>(&(*m_fitterSvc));
     if(!m_fitSvc){
        ATH_MSG_DEBUG(" No implemented Trk::ITrkVKalVrtFitter interface");
        return StatusCode::SUCCESS;
     }
     m_initialised = 1;          // Tool is initialised successfully.
//-----
     if(msgLvl(MSG::DEBUG)) ATH_CHECK(service("ChronoStatSvc", m_timingProfile));
//-----
     return StatusCode::SUCCESS;
   }

   StatusCode InDetTrkInJetType::finalize()
   {
    if(m_timingProfile)m_timingProfile->chronoPrint("InDet_TrkInJetType");
    ATH_MSG_DEBUG("InDetTrkInJetType finalize()");
    return StatusCode::SUCCESS; 
   }

   std::vector<float> InDetTrkInJetType::trkTypeWgts(const Rec::TrackParticle *, const xAOD::Vertex &, const TLorentzVector &) const
   {   return std::vector<float>(3,0.); }

   std::vector<float> InDetTrkInJetType::trkTypeWgts(const xAOD::TrackParticle * Trk, const xAOD::Vertex & PV, const TLorentzVector & Jet) const
   {  
//-- Track quality checks
      std::vector<float> safeReturn(3,0.);
      if( !m_initialised )          return safeReturn;
      if(Jet.Perp() > m_jetMaxPtCut)return safeReturn;
      if(Trk->pt() < m_trkMinPtCut) return safeReturn;
      if(Trk->pt() > Jet.Pt())      return safeReturn;
      if(Trk->numberDoF() == 0)                             return safeReturn; //Safety
      if(Trk->chiSquared()/Trk->numberDoF() > m_trkChi2Cut) return safeReturn;
      uint8_t PixelHits,SctHits;
      if( !(Trk->summaryValue(PixelHits,xAOD::numberOfPixelHits)) ) return safeReturn; // No Pixel hits. Bad.
      if( !(Trk->summaryValue(  SctHits,xAOD::numberOfSCTHits))   ) return safeReturn; // No SCT hits. Bad.
      if( PixelHits < m_trkPixelHitsCut ) return safeReturn;
      if( SctHits   < m_trkSctHitsCut )   return safeReturn;
 
      std::vector<double> Impact,ImpactError;
      float Sig3D=m_fitSvc->VKalGetImpact(Trk, PV.position(), 1, Impact, ImpactError);
      AmgVector(5) tmpPerigee = Trk->perigeeParameters().parameters(); 
      if( sin(tmpPerigee[2]-Jet.Phi())*Impact[0] < 0 ){ Impact[0] = -fabs(Impact[0]);}
                                                  else{ Impact[0] =  fabs(Impact[0]);}
      if(  (tmpPerigee[3]-Jet.Theta())*Impact[1] < 0 ){ Impact[1] = -fabs(Impact[1]);}
                                                  else{ Impact[1] =  fabs(Impact[1]);}
      double SignifR = Impact[0]/ sqrt(ImpactError[0]);
      double SignifZ = Impact[1]/ sqrt(ImpactError[2]);
      double trkSignif = sqrt(  (SignifR+0.6)*(SignifR+0.6) + (SignifZ+0.0)*(SignifZ+0.0) );
//---Calibrated range selection
      if(Impact[0]<m_d0_limLow || Impact[0]>m_d0_limUpp) return safeReturn;
      if(Impact[0]<m_Z0_limLow || Impact[0]>m_Z0_limUpp) return safeReturn;
      if( sqrt(SignifR*SignifR +SignifZ*SignifZ) < 1.)   return safeReturn;
//---IBL/BL hits
      int hitIBL=0, hitBL=0; 
      uint8_t IBLhit,BLhit,IBLexp,BLexp;
      if(!Trk->summaryValue( IBLhit,  xAOD::numberOfInnermostPixelLayerHits) )        IBLhit = 0;
      if(!Trk->summaryValue(  BLhit,  xAOD::numberOfNextToInnermostPixelLayerHits) )   BLhit = 0;
      if(!Trk->summaryValue( IBLexp,  xAOD::expectInnermostPixelLayerHit) )           IBLexp = 0;
      if(!Trk->summaryValue(  BLexp,  xAOD::expectNextToInnermostPixelLayerHit) )      BLexp = 0;
      hitIBL=IBLhit; if( IBLexp==0 ) hitIBL=-1;
      hitBL = BLhit; if(  BLexp==0 ) hitBL =-1;
/*---PV constraint (doesn't improve rejection in first try)
      Amg::Vector3D     FitVrt;
      TLorentzVector    Momentum;
      long int  Charge=0;
      std::vector<double> ErrorMatrix, Chi2PerTrk;
      std::vector< std::vector<double> > TrkAtVrt;
      std::vector<const xAOD::TrackParticle *>  TrkForFit(1,Trk);
      std::vector<const xAOD::NeutralParticle*> netralDummy(0);
      m_fitSvc->setDefault();                                  //Reset VKalVrt settings
      std::vector<float> covPV=PV.covariance();
      m_fitSvc->setVertexForConstraint(PV.x(),PV.y(),PV.z());
      m_fitSvc->setCovVrtForConstraint(covPV[0],covPV[1],covPV[2],covPV[3],covPV[4],covPV[5]);
      m_fitSvc->setCnstType(6);                                // Set primary vertex constraint
      StatusCode sc=m_fitSvc->VKalVrtFit( TrkForFit, netralDummy, FitVrt, Momentum, Charge, ErrorMatrix, Chi2PerTrk, TrkAtVrt, Chi2);
      if(sc.isFailure())Chi2=exp(11.);
      if(Chi2>exp(11.))Chi2=exp(11.);
*/
//====================== BDT weights
     double coeffPt=10.;
     double pfrac=(Trk->pt()-m_trkMinPtCut)/sqrt(Jet.Perp());
     float prbP= pfrac/(coeffPt+pfrac);
     float etatrk=Trk->eta();
//---
     double coeffSig=1.0;
     if(trkSignif<coeffSig) return safeReturn;
     float prbS=(trkSignif-coeffSig)/trkSignif;
     if(prbS<0.) return safeReturn;
//---
     float d0=Impact[0];
     float SigZ=SignifZ;
     float SigR=SignifR;
//---
     float ptjet=Jet.Perp();
     if(ptjet<m_jetMinPtCut)ptjet=m_jetMinPtCut; //Very low jet pt is replaced by Pt=35GeV
//---
     float ibl = (float)hitIBL;
     float bl  = (float)hitBL;
//---
     TLorentzVector TLV; 
     TLV.SetPtEtaPhiE(Trk->pt(),Trk->eta(),Trk->phi(),Trk->e());
     float pTvsJet=TLV.Perp(Jet.Vect());
//---
     TLorentzVector normJ;  normJ.SetPtEtaPhiM(1.,Jet.Eta(),Jet.Phi(),0.);
//---
     if(m_timingProfile)m_timingProfile->chronoStart("InDet_TrkInJetType");
     //std::vector<float> weights=m_tmvaReader->EvaluateMulticlass("BDTG");
     //-----Use MVAUtils to save CPU
     std::vector<float> bdt_vars={Sig3D, prbP, pTvsJet, d0, SigR, SigZ, ptjet, ibl, bl, etatrk};
     std::vector<float> weights=m_localBDT->GetMultiResponse(bdt_vars,3);
     //-----
     if(m_timingProfile)m_timingProfile->chronoStop("InDet_TrkInJetType");
     return weights;
   }
   
}// close namespace
