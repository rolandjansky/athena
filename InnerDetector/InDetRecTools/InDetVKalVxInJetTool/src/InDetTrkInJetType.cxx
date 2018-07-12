/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "InDetVKalVxInJetTool/InDetTrkInJetType.h"
#include "TMVA/MethodBase.h"
#include "TMVA/Reader.h"
#include "PathResolver/PathResolver.h"
//
//-------------------------------------------------
namespace InDet {
//
//Constructor-------------------------------------------------------------- 
InDetTrkInJetType::InDetTrkInJetType(const std::string& type,
                                           const std::string& name,
                                           const IInterface* parent):
  AthAlgTool(type,name,parent),
  m_tmvaReader(0),
  m_trkMinPtCut(700.),
  m_d0_limLow(-3.),
  m_d0_limUpp( 5.),
  m_Z0_limLow(-8.),
  m_Z0_limUpp(12.),
  m_calibFileName("TrackClassif_3cl.v01.xml"),
  m_fitterSvc("Trk::TrkVKalVrtFitter/VertexFitterTool",this)
  {
     declareInterface<IInDetTrkInJetType>(this);
     declareProperty("trkMinPt",  m_trkMinPtCut  ,  "Minimal track Pt cut" );
     declareProperty("d0_limLow", m_d0_limLow    ,  "Low d0 impact cut" );
     declareProperty("d0_limUpp", m_d0_limUpp    ,  "Upper d0 impact cut" );
     declareProperty("Z0_limLow", m_Z0_limLow    ,  "Low Z0 impact cut" );
     declareProperty("Z0_limUpp", m_Z0_limUpp    ,  "Upper Z0 impact cut" );
  }

//Destructor---------------------------------------------------------------
  InDetTrkInJetType::~InDetTrkInJetType(){
    delete m_tmvaReader;
    if(msgLvl(MSG::DEBUG))msg(MSG::DEBUG)<< "InDetTrkInJetType destructor called" << endmsg;
  }

//Initialize---------------------------------------------------------------
   StatusCode InDetTrkInJetType::initialize(){
     m_initialised = 0;
     m_tmvaReader = new TMVA::Reader();
     //m_tmvaReader->AddVariable( "prbS",  &m_prbS );
     m_tmvaReader->AddVariable( "Sig3D", &m_Sig3D );
     m_tmvaReader->AddVariable( "prbP",  &m_prbP );
     m_tmvaReader->AddVariable( "pTvsJet", &m_pTvsJet );
     //m_tmvaReader->AddVariable( "prodTJ", &m_prodTJ );
     m_tmvaReader->AddVariable( "d0",    &m_d0 );
     m_tmvaReader->AddVariable( "SigR",  &m_SigR );
     m_tmvaReader->AddVariable( "SigZ",  &m_SigZ );
     m_tmvaReader->AddVariable( "ptjet", &m_ptjet );
     m_tmvaReader->AddVariable( "ibl"   , &m_ibl );
     m_tmvaReader->AddVariable( "bl"   ,  &m_bl );
     m_tmvaReader->AddVariable( "etajet", &m_etajet );
     //m_tmvaReader->AddVariable( "vChi2", &m_vChi2 );
//
//-- Calibration file
//
//     std::string fullPathToFile = PathResolverFindCalibFile("InDetVKalVxInJetTool/TrackClassif_3cl.v01.xml");
     std::string fullPathToFile = PathResolverFindCalibFile("InDetVKalVxInJetTool/"+m_calibFileName);
     if(fullPathToFile != ""){
        if(msgLvl(MSG::DEBUG))msg(MSG::DEBUG) <<"TrackClassification calibration file" << fullPathToFile << endmsg;
        m_tmvaReader->BookMVA("BDTG", fullPathToFile);
     }else{
        if(msgLvl(MSG::DEBUG))msg(MSG::DEBUG) <<"Error! No calibration for TrackClassification found." << endmsg;
        return StatusCode::SUCCESS;
     }    
     //-------
     if (m_fitterSvc.retrieve().isFailure()) {
        if(msgLvl(MSG::DEBUG))msg(MSG::DEBUG) << "Could not find Trk::TrkVKalVrtFitter" << endmsg;
        return StatusCode::SUCCESS;
     } else {
        if(msgLvl(MSG::DEBUG))msg(MSG::DEBUG) << "InDetTrkInJetTool TrkVKalVrtFitter found" << endmsg;
     }
     m_fitSvc = dynamic_cast<Trk::TrkVKalVrtFitter*>(&(*m_fitterSvc));
     if(!m_fitSvc){
        if(msgLvl(MSG::DEBUG))msg(MSG::DEBUG)<<" No implemented Trk::ITrkVKalVrtFitter interface" << endmsg;
        return StatusCode::SUCCESS;
     }
     m_initialised = 1;          // Tool is initialised successfully.
     return StatusCode::SUCCESS;
   }

   StatusCode InDetTrkInJetType::finalize()
   {
    if(msgLvl(MSG::DEBUG))msg(MSG::DEBUG) <<"InDetTrkInJetType finalize()" << endmsg;
    return StatusCode::SUCCESS; 
   }

   std::vector<float> InDetTrkInJetType::trkTypeWgts(const Rec::TrackParticle *, const xAOD::Vertex &, const TLorentzVector &)
   {   return std::vector<float>(3,0.); }

   std::vector<float> InDetTrkInJetType::trkTypeWgts(const xAOD::TrackParticle * Trk, const xAOD::Vertex & PV, const TLorentzVector & Jet)
   {  
      std::vector<float> safeReturn(3,0.);
      if( !m_initialised ) return safeReturn;
      if(Jet.Perp()>2500000.)return safeReturn;
      std::vector<double> Impact,ImpactError;
      m_Sig3D=m_fitSvc->VKalGetImpact(Trk, PV.position(), 1, Impact, ImpactError);
      AmgVector(5) tmpPerigee = Trk->perigeeParameters().parameters(); 
      if( sin(tmpPerigee[2]-Jet.Phi())*Impact[0] < 0 ){ Impact[0] = -fabs(Impact[0]);}
                                                  else{ Impact[0] =  fabs(Impact[0]);}
      if(  (tmpPerigee[3]-Jet.Theta())*Impact[1] < 0 ){ Impact[1] = -fabs(Impact[1]);}
                                                  else{ Impact[1] =  fabs(Impact[1]);}
      double SignifR = Impact[0]/ sqrt(ImpactError[0]);
      double SignifZ = Impact[1]/ sqrt(ImpactError[2]);
      double trkSignif = sqrt(  (SignifR+0.6)*(SignifR+0.6) + (SignifZ+0.0)*(SignifZ+0.0) );
//---
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
     m_prbP= pfrac/(coeffPt+pfrac);
//---
     double coeffSig=1.0;
     if(trkSignif<coeffSig) return safeReturn;
     m_prbS=(trkSignif-coeffSig)/trkSignif;
     if(m_prbS<0.) return safeReturn;
//---
     m_d0=Impact[0];
     m_SigZ=SignifZ;
     m_SigR=SignifR;
//---
     m_ptjet=Jet.Perp();
     m_etajet=fabs(Jet.Eta());
//---
     m_ibl = (float)hitIBL;
     m_bl  = (float)hitBL;
//---
     TLorentzVector TLV; 
     TLV.SetPtEtaPhiE(Trk->pt(),Trk->eta(),Trk->phi(),Trk->e());
     m_pTvsJet=TLV.Perp(Jet.Vect());
//---
     TLorentzVector normJ;  normJ.SetPtEtaPhiM(1.,Jet.Eta(),Jet.Phi(),0.);
     m_prodTJ=sqrt(TLV.Dot(normJ));
     return m_tmvaReader->EvaluateMulticlass("BDTG");

   }
   
}// close namespace
