/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonHistUtils/RecoInfoPlots.h"

namespace Muon{

RecoInfoPlots::RecoInfoPlots(PlotBase *pParent, std::string sDir):PlotBase(pParent, sDir)
						  ,m_oTrkRecoInfoPlots(this, "IDTrk")
						  ,m_oMSTrkRecoInfoPlots(this, "MSTrk")
						  ,m_oRecoInfoPlots(this, "MuTrk")
						  ,author(NULL)
						  ,quality(NULL)
						  ,muonType(NULL)
						  ,innerMatchingChi2(NULL)
						  ,innerMatchingNdof(NULL)
						  ,innerMatchingCon(NULL)
						  ,outerMatchingChi2(NULL)
						  ,outerMatchingNdof(NULL)
						  ,outerMatchingCon(NULL)
{}

void RecoInfoPlots::initializePlots(){

  author   = Book1D("author", "author;Author;Entries",21,-0.5,20.5);
  muonType = Book1D("muonType", "muonType;muonType;Entries",6,-0.5,5.5);
  quality  = Book1D("quality", "quality;quality;Entries",4,-0.5,3.5);

  // innerMatchingChi2 = Book1D("innermchi2","inner matching chi2;inner matching Chi2;Entries",100,0.,100.);
  // innerMatchingNdof = Book1D("innermndof","inner matching ndof;inner matching ndof;Entries",51,-0.5,50.5);
  // innerMatchingCon  = Book1D("innermchi2ndof", "inner matching chi2/ndof;inner matching chi2/ndof;Entries", 100, 0, 5);

  // outerMatchingChi2 = Book1D("outermchi2","outer matching chi2;outer matching Chi2;Entries",100,0.,100.);
  // outerMatchingNdof = Book1D("outermndof","outer matching ndof;outer matching ndof;Entries",51,-0.5,50.5);
  // outerMatchingCon  = Book1D("outermchi2ndof", "outer matching chi2/ndof;outer matching chi2/ndof;Entries", 100, 0, 5);

}

void RecoInfoPlots::fill(const xAOD::Muon& mu) {

  const xAOD::TrackParticle* primaryTrk = mu.trackParticle(xAOD::Muon::Primary);
  if (primaryTrk) {
    m_oRecoInfoPlots.fill(*primaryTrk);
  }

  const xAOD::TrackParticle* inDetTrk = mu.trackParticle(xAOD::Muon::InnerDetectorTrackParticle);
  if (inDetTrk) {
    m_oTrkRecoInfoPlots.fill(*inDetTrk);
  }
  const xAOD::TrackParticle* msTrk = mu.trackParticle(xAOD::Muon::MuonSpectrometerTrackParticle);
  if (msTrk) {
    m_oMSTrkRecoInfoPlots.fill(*msTrk);
  }
  
  author->Fill(mu.author());
  muonType->Fill(mu.muonType());
  quality->Fill(mu.quality());
//  fillMatchingPlots(innerMatchingChi2, innerMatchingNdof, innerMatchingCon, xAOD::Muon::msInnerMatchChi2, xAOD::Muon::msInnerMatchDOF, mu);
//  fillMatchingPlots(outerMatchingChi2, outerMatchingNdof, outerMatchingCon, xAOD::Muon::msOuterMatchChi2, xAOD::Muon::msOuterMatchDOF, mu);

}

void RecoInfoPlots::fillMatchingPlots(TH1* matchingChi2, TH1* matchingNdof, TH1* matchingCon, xAOD::Muon::ParamDef eChi2, xAOD::Muon::ParamDef eNdof, const xAOD::Muon& mu){
  float chi2 = 0; 
  float ndof = 0;
  if (!mu.parameter(chi2, eChi2)) return;
  if (!mu.parameter(ndof, eNdof)) return;
  matchingChi2->Fill(chi2);
  matchingNdof->Fill(ndof);
  matchingCon->Fill(chi2/ndof);
}

}
