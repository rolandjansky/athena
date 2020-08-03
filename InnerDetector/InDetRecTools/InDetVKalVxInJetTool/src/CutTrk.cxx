/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// Header include
#include "InDetVKalVxInJetTool/InDetVKalVxInJetTool.h"
#include  "AnalysisUtils/AnalysisMisc.h"
#include "TrkVertexFitterInterfaces/ITrackToVertexIPEstimator.h"
#include <iostream>
//-------------------------------------------------
namespace InDet{

  StatusCode InDetVKalVxInJetTool::CutTrkRelax(std::unordered_map<std::string,double> TrkVarDouble,
                                               std::unordered_map<std::string,long int> TrkVarLongInt)
  const
  {

    double eta       = TrkVarDouble["eta"];
    double ThetaVert = TrkVarDouble["ThetaVert"];
    double A0Vert    = TrkVarDouble["A0Vert"];
    double ZVert     = TrkVarDouble["ZVert"];
    double ConeDist  = TrkVarDouble["ConeDist"];

    long int PixelHits = TrkVarLongInt["PixelHits"];
    long int SctHits   = TrkVarLongInt["SctHits"];

    if ( ConeDist > m_ConeForTag )                        return StatusCode::FAILURE;

    double z0_cut = m_useEtaDependentCuts ? m_etaDependentCutsSvc->getMaxZImpactAtEta(eta) : m_CutZVrt;
    if(fabs(ZVert*sin(ThetaVert)) > z0_cut)	return StatusCode::FAILURE;

    double d0_cut = m_useEtaDependentCuts ? m_etaDependentCutsSvc->getMaxPrimaryImpactAtEta(eta) : m_CutA0;
    if(fabs(A0Vert) > d0_cut) 		        return StatusCode::FAILURE;

    int pix_cut = m_useEtaDependentCuts ? m_etaDependentCutsSvc->getMinPixelHitsAtEta(eta) : m_CutPixelHits;
    if(!m_useEtaDependentCuts && fabs(eta)>2. && !m_IsPhase2) pix_cut++;
    if(PixelHits < pix_cut) 		        return StatusCode::FAILURE;

    int strip_cut = m_useEtaDependentCuts ? m_etaDependentCutsSvc->getMinStripHitsAtEta(eta) : m_CutSctHits;
    if(!m_useEtaDependentCuts && fabs(eta)>1.65 && !m_IsPhase2) strip_cut++;
    if(SctHits < strip_cut)              	return StatusCode::FAILURE;

    int si_cut = m_useEtaDependentCuts ? m_etaDependentCutsSvc->getMinSiHitsAtEta(eta) : m_CutSiHits;
    if((PixelHits+SctHits) < si_cut) 		return StatusCode::FAILURE;

     return StatusCode::SUCCESS;
  }


  StatusCode InDetVKalVxInJetTool::CutTrk(std::unordered_map<std::string,double> TrkVarDouble,
                                          std::unordered_map<std::string,long int> TrkVarLongInt)
  const
  {

    double eta         = TrkVarDouble["eta"];
    double PInvVert    = TrkVarDouble["PInvVert"];
    double ThetaVert   = TrkVarDouble["ThetaVert"];
    double A0Vert      = TrkVarDouble["A0Vert"];
    double ZVert       = TrkVarDouble["ZVert"];
    double Chi2        = TrkVarDouble["Chi2"];
    double ConeDist    = TrkVarDouble["ConeDist"];
    double CovTrkMtx11 = TrkVarDouble["CovTrkMtx11"];
    double CovTrkMtx22 = TrkVarDouble["CovTrkMtx22"];
    double trkP        = TrkVarDouble["trkP"];
    double trkPErr     = TrkVarDouble["trkPErr"];

    long int PixelHits = TrkVarLongInt["PixelHits"];
    long int SctHits   = TrkVarLongInt["SctHits"];
    long int BLayHits  = TrkVarLongInt["BLayHits"];
    long int badHits   = TrkVarLongInt["badHits"];

    if ( CovTrkMtx11 > m_A0TrkErrorCut*m_A0TrkErrorCut )  return StatusCode::FAILURE;
    if ( CovTrkMtx22 > m_ZTrkErrorCut*m_ZTrkErrorCut )    return StatusCode::FAILURE;
    if ( ConeDist > m_ConeForTag )                        return StatusCode::FAILURE;
    if(trkP>10000.){
      if(m_FillHist)m_hb_trkPErr->Fill( trkPErr , m_w_1);
      if(trkPErr>0.5) return StatusCode::FAILURE;
    }

     double Pt = sin(ThetaVert)/fabs(PInvVert);
     //- Track quality

     double pT_cut = m_useEtaDependentCuts ? m_etaDependentCutsSvc->getMinPtAtEta(eta) : m_CutPt;
     if(Pt               < pT_cut) 		return StatusCode::FAILURE;

     if(!m_MultiWithPrimary){           //Must not be used for primary vertex search
       double z0_cut = m_useEtaDependentCuts ? m_etaDependentCutsSvc->getMaxZImpactAtEta(eta) : m_CutZVrt;
       if(fabs(ZVert*sin(ThetaVert)) > z0_cut)	return StatusCode::FAILURE;
     }

     double chi2_cut = m_useEtaDependentCuts ? m_etaDependentCutsSvc->getMaxChi2AtEta(eta) : m_CutChi2;
     if(Chi2 	         > chi2_cut) 		return StatusCode::FAILURE;

     double d0_cut = m_useEtaDependentCuts ? m_etaDependentCutsSvc->getMaxPrimaryImpactAtEta(eta) : m_CutA0;
     if(fabs(A0Vert)     > d0_cut) 		return StatusCode::FAILURE;

     int pix_cut = m_useEtaDependentCuts ? m_etaDependentCutsSvc->getMinPixelHitsAtEta(eta) : m_CutPixelHits;
     if(!m_useEtaDependentCuts && fabs(eta)>2. && !m_IsPhase2) pix_cut++;
     if(PixelHits	    < pix_cut) 		return StatusCode::FAILURE;
     if(fabs(eta)>2 && !m_IsPhase2 && badHits && PixelHits<=3) return StatusCode::FAILURE;

     int strip_cut = m_useEtaDependentCuts ? m_etaDependentCutsSvc->getMinStripHitsAtEta(eta) : m_CutSctHits;
     if(!m_useEtaDependentCuts && fabs(eta)>2. && !m_IsPhase2 && m_existIBL) strip_cut++;
     if(!m_useEtaDependentCuts && fabs(eta)>1.65 && !m_IsPhase2) strip_cut++;
     if(SctHits		    < strip_cut) 	return StatusCode::FAILURE;

     int si_cut = m_useEtaDependentCuts ? m_etaDependentCutsSvc->getMinSiHitsAtEta(eta) : m_CutSiHits;
     if((PixelHits+SctHits) < si_cut) 		return StatusCode::FAILURE;

     int inpix_cut = m_useEtaDependentCuts ? m_etaDependentCutsSvc->getMinInnermostPixelHitsAtEta(eta) : m_CutBLayHits;
     if(BLayHits	    < inpix_cut) 	return StatusCode::FAILURE;

     return StatusCode::SUCCESS;
  }
 



   int  InDetVKalVxInJetTool::SelGoodTrkParticle( const std::vector<const Rec::TrackParticle*>& InpTrk,
                                                  const xAOD::Vertex                          & PrimVrt,
	                                          const TLorentzVector                        & JetDir,
                                                  std::vector<const Rec::TrackParticle*>   & SelectedTracks)
   const
   {    

    std::vector<const Rec::TrackParticle*>::const_iterator   i_ntrk;
    AmgVector(5) VectPerig; VectPerig<<0.,0.,0.,0.,0.;
    const Trk::Perigee* mPer;
    const Trk::FitQuality*     TrkQual;
    std::vector<double> Impact,ImpactError;
    int NPrimTrk=0;
    for (i_ntrk = InpTrk.begin(); i_ntrk < InpTrk.end(); ++i_ntrk) {
//
//-- Perigee in TrackParticle
//
          mPer=GetPerigee( (*i_ntrk) ) ;
          if( mPer == NULL )  continue;
          VectPerig = mPer->parameters(); 
          TrkQual   = (*i_ntrk)->fitQuality();
          if(TrkQual && TrkQual->numberDoF()== 0) continue; //Protection
          double trkChi2=1.; if(TrkQual) trkChi2=TrkQual->chiSquared() / TrkQual->numberDoF();
          double CovTrkMtx11 = (*(mPer->covariance()))(0,0);
          double CovTrkMtx22 = (*(mPer->covariance()))(1,1);
          double coneDist = ConeDist(VectPerig,JetDir);

          double trkP=1./fabs(VectPerig[4]);         
          double CovTrkMtx55 = (*(mPer->covariance()))(4,4);
          double trkPErr=sqrt(CovTrkMtx55)*trkP;

          //----------------------------------- Summary tools
          const Trk::TrackSummary* testSum = (*i_ntrk)->trackSummary();
          long int PixelHits = (long int) testSum->get(Trk::numberOfPixelHits);
          long int SctHits   = (long int) testSum->get(Trk::numberOfSCTHits);
          long int BLayHits  = (long int) testSum->get(Trk::numberOfBLayerHits);
	  if(PixelHits < 0 ) PixelHits=0; 
	  if(SctHits   < 0 ) SctHits=0; 
	  if(BLayHits  < 0 ) BLayHits=0; 

          double ImpactSignif = m_fitSvc->VKalGetImpact((*i_ntrk), PrimVrt.position(), 1, Impact, ImpactError);
          double ImpactA0=Impact[0];
          double ImpactZ=Impact[1];

          long int splSCTHits = (long int) testSum->get(Trk::numberOfSCTSpoiltHits);
          long int outSCTHits = (long int) testSum->get(Trk::numberOfSCTOutliers);
          long int splPixHits = (long int) testSum->get(Trk::numberOfPixelSpoiltHits);
          long int outPixHits = (long int) testSum->get(Trk::numberOfPixelOutliers);
          if(splSCTHits < 0 ) splSCTHits = 0;
          if(outSCTHits < 0 ) outSCTHits = 0;
          if(splPixHits < 0 ) splPixHits = 0;
          if(outPixHits < 0 ) outPixHits = 0;
          bool badHits = ( splSCTHits || outSCTHits || outPixHits || splPixHits );

          double eta = (*i_ntrk)->eta();

	  std::unordered_map<std::string,double> TrkVarDouble;
	  std::unordered_map<std::string,long int> TrkVarLongInt;

	  TrkVarDouble["eta"]         = eta;
	  TrkVarDouble["PInvVert"]    = VectPerig[4];
	  TrkVarDouble["ThetaVert"]   = VectPerig[3];
	  TrkVarDouble["A0Vert"]      = ImpactA0;
	  TrkVarDouble["ZVert"]       = ImpactZ;
	  TrkVarDouble["Chi2"]        = trkChi2;
	  TrkVarDouble["ConeDist"]    = coneDist;
	  TrkVarDouble["CovTrkMtx11"] = CovTrkMtx11;
	  TrkVarDouble["CovTrkMtx22"] = CovTrkMtx22;
	  TrkVarDouble["trkP"]        = trkP;
	  TrkVarDouble["trkPErr"]     = trkPErr;

	  TrkVarLongInt["PixelHits"] = PixelHits;
	  TrkVarLongInt["SctHits"]   = SctHits;
	  TrkVarLongInt["BLayHits"]  = BLayHits;
	  TrkVarLongInt["badHits"]   = (long int) badHits;

          StatusCode sc = CutTrk(TrkVarDouble, TrkVarLongInt);
          if( sc.isFailure() )                 continue;

	  if(ImpactSignif < 3.)NPrimTrk += 1;
	  SelectedTracks.push_back(*i_ntrk);
      }
      AnalysisUtils::Sort::pT (&SelectedTracks); // no equivalent for TrkTrack yet...
      return NPrimTrk;
   }
//
//  Simplified TrackParticle selection to get reliably reconstructed tracks for jet momentum estimation 
// 
   int  InDetVKalVxInJetTool::SelGoodTrkParticleRelax( const std::vector<const Rec::TrackParticle*>& InpTrk,
                                         const xAOD::Vertex                                        & PrimVrt,
	                                 const TLorentzVector                                      & JetDir,
                                               std::vector<const Rec::TrackParticle*>    & SelectedTracks)
   const
   {    

    std::vector<const Rec::TrackParticle*>::const_iterator   i_ntrk;
    AmgVector(5) VectPerig; VectPerig<<0.,0.,0.,0.,0.;
    const Trk::Perigee* mPer;
    std::vector<double> Impact,ImpactError;
    int NPrimTrk=0;
    for (i_ntrk = InpTrk.begin(); i_ntrk < InpTrk.end(); ++i_ntrk) {
//
//-- MeasuredPerigee in TrackParticle
//
          mPer=GetPerigee( (*i_ntrk) ) ;
          if( mPer == NULL ){ continue; } 
          VectPerig = mPer->parameters(); 
          double coneDist = ConeDist(VectPerig,JetDir);

          //----------------------------------- Summary tools
          const Trk::TrackSummary* testSum = (*i_ntrk)->trackSummary();
          long int PixelHits = (long int) testSum->get(Trk::numberOfPixelHits);
          long int SctHits   = (long int) testSum->get(Trk::numberOfSCTHits);
	  if(PixelHits < 0 ) PixelHits=0; 
	  if(SctHits   < 0 ) SctHits=0; 

          double ImpactSignif = m_fitSvc->VKalGetImpact((*i_ntrk), PrimVrt.position(), 1, Impact, ImpactError);
          double ImpactA0=Impact[0];
          double ImpactZ=Impact[1];

          double eta = (*i_ntrk)->eta();

	  std::unordered_map<std::string,double> TrkVarDouble;
	  std::unordered_map<std::string,long int> TrkVarLongInt;

	  TrkVarDouble["eta"]         = eta;
	  TrkVarDouble["ThetaVert"]   = VectPerig[3];
	  TrkVarDouble["A0Vert"]      = ImpactA0;
	  TrkVarDouble["ZVert"]       = ImpactZ;
	  TrkVarDouble["ConeDist"]    = coneDist;

	  TrkVarLongInt["PixelHits"] = PixelHits;
	  TrkVarLongInt["SctHits"]   = SctHits;

          StatusCode sc = CutTrkRelax(TrkVarDouble, TrkVarLongInt);
          if( sc.isFailure() )                 continue;

	  if(ImpactSignif < 3.)NPrimTrk += 1;
	  SelectedTracks.push_back(*i_ntrk);
      }
      AnalysisUtils::Sort::pT (&SelectedTracks); // no equivalent for TrkTrack yet...
      return NPrimTrk;
  }
//==============================================================================================================
//          xAOD based stuff
//
   int  InDetVKalVxInJetTool::SelGoodTrkParticle( const std::vector<const xAOD::TrackParticle*>& InpTrk,
                                                  const xAOD::Vertex                           & PrimVrt,
	                                          const TLorentzVector                         & JetDir,
                                                        std::vector<const xAOD::TrackParticle*>& SelectedTracks)
   const
   {    

    std::vector<const xAOD::TrackParticle*>::const_iterator   i_ntrk;
    std::vector<double> Impact,ImpactError;
    int NPrimTrk=0;
    for (i_ntrk = InpTrk.begin(); i_ntrk < InpTrk.end(); ++i_ntrk) {
//
//-- Perigee in TrackParticle
//
          const Trk::Perigee mPer=(*i_ntrk)->perigeeParameters() ;
          AmgVector(5) VectPerig = mPer.parameters();
          double coneDist = ConeDist(VectPerig,JetDir);

          if((*i_ntrk)->numberDoF() == 0) continue; //Protection
          double trkChi2 = (*i_ntrk)->chiSquared() / (*i_ntrk)->numberDoF();

          double CovTrkMtx11 = (*i_ntrk)->definingParametersCovMatrix()(0,0);
          double CovTrkMtx22 = (*i_ntrk)->definingParametersCovMatrix()(1,1);
          double CovTrkMtx55 = (*i_ntrk)->definingParametersCovMatrix()(4,4);

          double trkP=1./fabs(VectPerig[4]);         
          double trkPErr=sqrt(CovTrkMtx55)*trkP;

          uint8_t PixelHits,SctHits,BLayHits;
          if( !((*i_ntrk)->summaryValue(PixelHits,xAOD::numberOfPixelHits)) )   continue; // Track is 
          if( !((*i_ntrk)->summaryValue(  SctHits,xAOD::numberOfSCTHits))   )   continue; // definitely
          if( !((*i_ntrk)->summaryValue(BLayHits,xAOD::numberOfInnermostPixelLayerHits)))  BLayHits=0;

          uint8_t splSCTHits,outSCTHits,splPixHits,outPixHits;
          if( !((*i_ntrk)->summaryValue(splSCTHits,xAOD::numberOfSCTSpoiltHits)))  splSCTHits=0;
          if( !((*i_ntrk)->summaryValue(outSCTHits,xAOD::numberOfSCTOutliers)))    outSCTHits=0;
          if( !((*i_ntrk)->summaryValue(splPixHits,xAOD::numberOfPixelSpoiltHits)))splPixHits=0;
          if( !((*i_ntrk)->summaryValue(outPixHits,xAOD::numberOfPixelOutliers)))  outPixHits=0;
          bool badHits = ( splSCTHits || outSCTHits || outPixHits || splPixHits );

          double ImpactSignif = m_fitSvc->VKalGetImpact((*i_ntrk), PrimVrt.position(), 1, Impact, ImpactError);
	  double ImpactA0=Impact[0];
	  double ImpactZ=Impact[1];

	  double eta = (*i_ntrk)->eta();

	  std::unordered_map<std::string,double> TrkVarDouble;
	  std::unordered_map<std::string,long int> TrkVarLongInt;

	  TrkVarDouble["eta"]         = eta;
	  TrkVarDouble["PInvVert"]    = VectPerig[4];
	  TrkVarDouble["ThetaVert"]   = VectPerig[3];
	  TrkVarDouble["A0Vert"]      = ImpactA0;
	  TrkVarDouble["ZVert"]       = ImpactZ;
	  TrkVarDouble["Chi2"]        = trkChi2;
	  TrkVarDouble["ConeDist"]    = coneDist;
	  TrkVarDouble["CovTrkMtx11"] = CovTrkMtx11;
	  TrkVarDouble["CovTrkMtx22"] = CovTrkMtx22;
	  TrkVarDouble["trkP"]        = trkP;
	  TrkVarDouble["trkPErr"]     = trkPErr;

	  TrkVarLongInt["PixelHits"] = PixelHits;
	  TrkVarLongInt["SctHits"]   = SctHits;
	  TrkVarLongInt["BLayHits"]  = BLayHits;
	  TrkVarLongInt["badHits"]   = (long int) badHits;

          StatusCode sc = CutTrk(TrkVarDouble, TrkVarLongInt);
          if( sc.isFailure() )                 continue;

	  if(ImpactSignif < 3.)NPrimTrk += 1;
	  SelectedTracks.push_back(*i_ntrk);
      }
      AnalysisUtils::Sort::pT (&SelectedTracks); // no equivalent for TrkTrack yet...
      return NPrimTrk;
   }
//
//  Simplified xAOD::TrackParticle selection to get reliably reconstructed tracks for jet momentum estimation 
// 
   int  InDetVKalVxInJetTool::SelGoodTrkParticleRelax( const std::vector<const xAOD::TrackParticle*>& InpTrk,
                                                       const xAOD::Vertex                           & PrimVrt,
	                                               const TLorentzVector                         & JetDir,
                                                             std::vector<const xAOD::TrackParticle*>& SelectedTracks)
   const
   {    
    std::vector<const xAOD::TrackParticle*>::const_iterator   i_ntrk;
    std::vector<double> Impact,ImpactError;
    int NPrimTrk=0;
    for (i_ntrk = InpTrk.begin(); i_ntrk < InpTrk.end(); ++i_ntrk) {
//
//-- MeasuredPerigee in xAOD::TrackParticle
//
          const Trk::Perigee mPer=(*i_ntrk)->perigeeParameters() ;
          AmgVector(5) VectPerig = mPer.parameters();
          double coneDist = ConeDist(VectPerig,JetDir);

	  //----------------------------------- Summary tools
          uint8_t PixelHits,SctHits;
          if( !((*i_ntrk)->summaryValue(PixelHits,xAOD::numberOfPixelHits)) ) PixelHits=0;
          if( !((*i_ntrk)->summaryValue(  SctHits,xAOD::numberOfSCTHits))   )   SctHits=0;
 

          double ImpactSignif = m_fitSvc->VKalGetImpact((*i_ntrk), PrimVrt.position(), 1, Impact, ImpactError);
          double ImpactA0=Impact[0];
          double ImpactZ=Impact[1];

          double eta = (*i_ntrk)->eta();

	  std::unordered_map<std::string,double> TrkVarDouble;
	  std::unordered_map<std::string,long int> TrkVarLongInt;

	  TrkVarDouble["eta"]         = eta;
	  TrkVarDouble["ThetaVert"]   = VectPerig[3];
	  TrkVarDouble["A0Vert"]      = ImpactA0;
	  TrkVarDouble["ZVert"]       = ImpactZ;
	  TrkVarDouble["ConeDist"]    = coneDist;

	  TrkVarLongInt["PixelHits"] = PixelHits;
	  TrkVarLongInt["SctHits"]   = SctHits;

          StatusCode sc = CutTrkRelax(TrkVarDouble, TrkVarLongInt);
	  if( sc.isFailure() )                 continue;

	  if(ImpactSignif < 3.)NPrimTrk += 1;
	  SelectedTracks.push_back(*i_ntrk);
      }
      AnalysisUtils::Sort::pT (&SelectedTracks); // no equivalent for TrkTrack yet...
      return NPrimTrk;
  }



}//end namespace
