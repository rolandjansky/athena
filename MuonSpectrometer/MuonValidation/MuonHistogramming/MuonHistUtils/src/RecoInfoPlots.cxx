/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonHistUtils/RecoInfoPlots.h"
#include "MuonHistUtils/MuonEnumDefs.h"

namespace Muon{
											   
RecoInfoPlots::RecoInfoPlots(PlotBase *pParent, std::string sDir):PlotBase(pParent, sDir)
								 ,m_oTrkRecoInfoPlots(this, "IDTrk")
								 ,m_oMSTrkRecoInfoPlots(this, "MSTrk")
								 ,m_oRecoInfoPlots(this, "MuTrk")
								 ,author(NULL)
								 ,quality(NULL)
								 ,muonType(NULL)
{}

void RecoInfoPlots::initializePlots(){

  author   = Book1D("author", "author;Author;Entries",11,-0.5,10.5);  
  muonType = Book1D("muonType", "muonType;muonType;Entries",6,-0.5,5.5);
  quality  = Book1D("quality", "quality;quality;Entries",4,-0.5,3.5);

  //set labels
  for (int i=1; i<=author->GetNbinsX(); i++) 
    author->GetXaxis()->SetBinLabel(i, EnumDefs::toString( (xAOD::Muon::Author)author->GetBinCenter(i) ));
  
  for (int i=1; i<=muonType->GetNbinsX(); i++)
    muonType->GetXaxis()->SetBinLabel(i, EnumDefs::toString( (xAOD::Muon::MuonType)muonType->GetBinCenter(i) ));
  
  for (int i=1; i<=quality->GetNbinsX(); i++) 
    quality->GetXaxis()->SetBinLabel(i, EnumDefs::toString( (xAOD::Muon::Quality)quality->GetBinCenter(i) ));  

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
  
}

}
