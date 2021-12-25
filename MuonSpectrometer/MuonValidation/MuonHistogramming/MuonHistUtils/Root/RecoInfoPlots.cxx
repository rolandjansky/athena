/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonHistUtils/RecoInfoPlots.h"
#include "MuonHistUtils/MuonEnumDefs.h"

namespace Muon{
											   
RecoInfoPlots::RecoInfoPlots(PlotBase *pParent, const std::string& sDir):PlotBase(pParent, sDir)
								 ,m_oTrkRecoInfoPlots(this, "","IDTrk")
								 ,m_oMSTrkRecoInfoPlots(this, "", "MSTrk")
								 ,m_oRecoInfoPlots(this, "")
								 ,author(nullptr)
								 ,quality(nullptr)
								 ,quality_cutflow(nullptr)
								 ,muonType(nullptr)
{}

void RecoInfoPlots::initializePlots(){

  author   = Book1D("author", "author;Author;Entries",11,-0.5,10.5);  
  muonType = Book1D("muonType", "muonType;muonType;Entries",6,-0.5,5.5);
  quality  = Book1D("quality", "quality;quality;Entries",4,-0.5,3.5);
  quality_cutflow  = Book1D("quality_cutflow", "quality cut flow;quality;Entries",4,-0.5,3.5);

  //set labels
  for (int i=1; i<=author->GetNbinsX(); i++) 
    author->GetXaxis()->SetBinLabel(i, EnumDefs::toString( (xAOD::Muon::Author)author->GetBinCenter(i) ));
  
  for (int i=1; i<=muonType->GetNbinsX(); i++)
    muonType->GetXaxis()->SetBinLabel(i, EnumDefs::toString( (xAOD::Muon::MuonType)muonType->GetBinCenter(i) ));
  
  for (int i=1; i<=quality->GetNbinsX(); i++) {
    int iQuality = quality->GetBinCenter(i);
    const char* sQuality = EnumDefs::toString( (xAOD::Muon::Quality) iQuality );
    quality->GetXaxis()->SetBinLabel(i, sQuality );
    quality_cutflow->GetXaxis()->SetBinLabel(quality->GetNbinsX()-i+1, sQuality );
  }
}

  void RecoInfoPlots::fill(const xAOD::Muon& mu, float weight) {

  const xAOD::TrackParticle* primaryTrk = mu.trackParticle(xAOD::Muon::Primary);
  if (primaryTrk) {
    m_oRecoInfoPlots.fill(*primaryTrk, weight);
  }

  const xAOD::TrackParticle* inDetTrk = mu.trackParticle(xAOD::Muon::InnerDetectorTrackParticle);
  if (inDetTrk) {
    m_oTrkRecoInfoPlots.fill(*inDetTrk, weight);
  }

  //muon extrapolated to IP
  ////////////////// @@@ sorting out the mess with the link to the extrapolated muon
  //for 20.1.0...
  /// const xAOD::TrackParticle* msExtrapTrk = mu.trackParticle(xAOD::Muon::MuonSpectrometerTrackParticle); // points to the ExtrapolatedMuonSpectrometerTrackParticle, the ExtrapolatedMuonSpectrometerTrackParticle link doesn't exist

  //for 20.1.3...
  //const xAOD::TrackParticle* msExtrapTrk = mu.trackParticle(xAOD::Muon::ExtrapolatedMuonSpectrometerTrackParticle);

  //trying to accomodate both in a way that the code compiles in both releases
  int correctEnum = (int) xAOD::Muon::MuonSpectrometerTrackParticle;
  if (mu.isAvailable< ElementLink<xAOD::TrackParticleContainer> >("extrapolatedMuonSpectrometerTrackParticleLink") && (mu.auxdata< ElementLink<xAOD::TrackParticleContainer> >("extrapolatedMuonSpectrometerTrackParticleLink")).isValid()) correctEnum+=2; //check correct numbering in Muon.h
  const xAOD::TrackParticle* msExtrapTrk = mu.trackParticle((xAOD::Muon::TrackParticleType) correctEnum);
  if (msExtrapTrk) {
    m_oMSTrkRecoInfoPlots.fill(*msExtrapTrk, weight);
  }
  
  author->Fill(mu.author(), weight);
  muonType->Fill(mu.muonType(), weight);

  xAOD::Muon::Quality muqual = mu.quality();
  quality->Fill(muqual, weight);
  for (unsigned int i=0; i<Muon::EnumDefs::nMuonQualities(); i++) {    
    if ( muqual <= (xAOD::Muon::Quality)(Muon::EnumDefs::nMuonQualities()-i-1) ) 
      quality_cutflow->Fill(i, weight);
  } 
}
  
} //close namespace Muon
