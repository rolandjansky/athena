/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Header include
#include "TrigInDetVxInJetTool/TrigInDetVxInJetTool.h"
#include "TrigInDetEvent/TrigInDetTrack.h"
#include "AnalysisUtils/AnalysisMisc.h"
#include <iostream>
#include "TMath.h"


int  TrigInDetVxInJetTool::SelGoodTrigTrack( const std::vector<const TrigInDetTrack*>& InpTrk,
                                             const Trk::RecVertex                    & PrimVrt,
                                             const CLHEP::HepLorentzVector                  & JetDir,
                                             std::vector<const TrigInDetTrack*>& SelectedTracks) {
  
  std::vector<const TrigInDetTrack*>::const_iterator i_ntrk = InpTrk.begin();
  const TrigInDetTrackFitPar* mPer=0;
  std::vector<double> Impact,ImpactError;
  double ImpactSignif=0;
  int NPrimTrk=0;

  for (unsigned int j=0; i_ntrk < InpTrk.end(); ++i_ntrk, ++j) {

    mPer=(*i_ntrk)->param();

    if (mPer->ea0() > m_A0TrkErrorCut) {
      if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "  track # " << j+1 << " not selected: ea0=" << mPer->ea0() << " > " << m_A0TrkErrorCut << endmsg; 
      continue;
    }

    if (mPer->ez0() > m_ZTrkErrorCut) {
      if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "  track # " << j+1 << " not selected: ez0=" << mPer->ez0() << " > " << m_ZTrkErrorCut << endmsg; 
      continue;
    }

    double  adeta = JetDir.pseudoRapidity() - mPer->eta();
    double  adphi = fabs(JetDir.phi() - mPer->phi0());

    if (adphi < -TMath::Pi()) adphi += 2*TMath::Pi();
    if (adphi >  TMath::Pi()) adphi -= 2*TMath::Pi();

    double dist = sqrt(adphi*adphi + adeta*adeta);
 
    if(dist > m_ConeForTag) {
      if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "  track # " << j+1 << " not selected: jetDistance=" << dist << " > " << m_ConeForTag << endmsg; 
      continue;
    }

    long int PixelHits     = 3;
    long int SctHits       = 9; 
    long int SharedHits    = 0; //Always 0 now
    long int BLayHits      = 1;

    PixelHits = (long int)(*i_ntrk)->NPixelSpacePoints();
    SctHits   = (long int)(*i_ntrk)->NSCT_SpacePoints();
    BLayHits = 0;
    if ((*i_ntrk)->HitPattern()) BLayHits = ((*i_ntrk)->HitPattern() & 0x1) ? 1 : 0;
    if(PixelHits < 0 ) PixelHits=0; 
    if(SctHits   < 0 ) SctHits=0; 
    //if(BLayHits  < 0 ) BLayHits=0;
    ImpactSignif = m_fitSvc->VKalGetImpact((*i_ntrk), PrimVrt.position(), 1, Impact, ImpactError);
    if (ImpactSignif == -9999.0) continue;
    double ImpactA0=Impact[0];  
    double ImpactZ=Impact[1];   

    //Using CutChi2Ndof instead of CutProbChi2
    //double trkProbChi2=TMath::Prob((*i_ntrk)->chi2(),(PixelHits+SctHits)*3-5);
    double Dof = ((PixelHits+SctHits)*3-5)==0?-0.1:(PixelHits+SctHits)*3-5;
    double trkChi2Ndof = (*i_ntrk)->chi2()/Dof;

    double ThetaVert = 2.0*atan(exp(-mPer->eta())); 
    if( !std::isfinite(ThetaVert) ) continue;

    if(fabs(mPer->pT()) < m_CutPt) {
      if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "  track # " << j+1 << " not selected: pT=" << mPer->pT() << " < " << m_CutPt << endmsg; 
      continue;
    }
    if(fabs(ImpactZ) > m_CutZVrt/sin(ThetaVert)) {
      if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "  track # " << j+1 << " not selected: impactZ=" << fabs(ImpactZ) << " > " << m_CutZVrt/sin(ThetaVert) << endmsg; 
      continue;
    }
    if(trkChi2Ndof > m_CutChi2Ndof) {
      if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "  track # " << j+1 << " not selected: Chi2/Ndof=" << trkChi2Ndof << " > " << m_CutChi2Ndof << endmsg; 
      continue;
    }
    if(fabs(ImpactA0) > m_CutA0) {
      if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "  track # " << j+1 << " not selected: impactA0=" << fabs(ImpactA0) << " > " << m_CutA0 << endmsg; 
      continue;
    }
    if(PixelHits < m_CutPixelHits) {
      if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "  track # " << j+1 << " not selected: pixelHits=" << PixelHits << " < " << m_CutPixelHits << endmsg; 
      continue;
    }
    if(SctHits < m_CutSctHits) {
      if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "  track # " << j+1 << " not selected: sctHits=" << SctHits << " < " << m_CutSctHits << endmsg; 
      continue;
    }
    if((PixelHits+SctHits) < m_CutSiHits) {
      if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "  track # " << j+1 << " not selected: siHits=" << PixelHits+SctHits << " < " << m_CutSiHits << endmsg; 
      continue;
    }
    if(BLayHits < m_CutBLayHits) {
      if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "  track # " << j+1 << " not selected: bLayer=" << BLayHits << " < " << m_CutBLayHits << endmsg; 
      continue;
    }
    if(SharedHits > m_CutSharedHits) {
      if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "  track # " << j+1 << " not selected: sharedHits=" << SharedHits << " < " << m_CutSharedHits << endmsg; 
      continue;
    }
    
    if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "  track # " << j+1 << " selected" << endmsg; 
					  
    if(ImpactSignif < 3.)NPrimTrk += 1;
    SelectedTracks.push_back(*i_ntrk);
  }
  return NPrimTrk;
}
 
