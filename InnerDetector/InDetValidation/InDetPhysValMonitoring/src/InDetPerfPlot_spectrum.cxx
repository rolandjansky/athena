/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file InDetPerfPlot_spectrum.cxx
 * @author Stewart Swift
**/

#include "InDetPerfPlot_spectrum.h"
#include <math.h> //for M_PI




InDetPerfPlot_spectrum::InDetPerfPlot_spectrum(InDetPlotBase* pParent, const std::string & sDir):InDetPlotBase(pParent, sDir){
  //nop
}


void 
InDetPerfPlot_spectrum::initializePlots() {  
  //

  book(m_nSCTHits, "nSCTHits");
  book(m_nPixHits, "nPixHits");
  book(m_nTotHits, "nTotHits");
 
 //masking plots hits+deadsensors
  book(m_nSCTDeadSensors, "nSCTDeadSensors");
  book(m_nPixDeadSensors,"nPixDeadSensors");
  book(m_nTotDeadSensors, "nTotDeadSensors");
  //
  book(m_nSCTHits_vs_eta, "nSCTHits_vs_eta");
  book(m_nPixHits_vs_eta, "nPixHits_vs_eta");
  book(m_nTotHits_vs_eta, "nTotHits_vs_eta");
  
  book(m_nSCTDeadSensors_vs_eta, "nSCTDeadSensors_vs_eta");
  book(m_nPixDeadSensors_vs_eta, "nPixDeadSensors_vs_eta");
  book(m_nTotDeadSensors_vs_eta, "nTotDeadSensors_vs_eta");

  book(m_recoEta,"recoEtaSpectrum");
  book(m_recoPt,"recoPtSpectrum");
  book(m_recoPhi,"recoPhiSpectrum");
  book(m_recoMatchProb,"recoMatchProbabilitySpectrum");
 
  book(m_truthEta,"truthEtaSpectrum");
  book(m_truthPt,"truthPtSpectrum");
  book(m_truthPhi,"truthPhiSpectrum");

}

void 
InDetPerfPlot_spectrum::fillSpectrum(const xAOD::TrackParticle& trkprt,Float_t prob){
	
		double pt = trkprt.pt()/1000.;
		double eta = 0.0;
		if(trkprt.pt() > 1e-7) eta = trkprt.eta();
		double phi(trkprt.phi());
		uint8_t iPixHits,iSCTHits,iPixDead,iSCTDead;                                                                                                        
		int pixHits = 0;                                                                                                                  
		int sctHits = 0;                                                                                                                  
		int pixDead = 0;
		int sctDead = 0;

		if (trkprt.summaryValue(iPixHits,xAOD::numberOfPixelHits)) pixHits = iPixHits;                                             
		if (trkprt.summaryValue(iSCTHits,xAOD::numberOfSCTHits)) sctHits = iSCTHits;     
		if (trkprt.summaryValue(iPixDead,xAOD::numberOfPixelDeadSensors)) pixDead = iPixDead;
    if (trkprt.summaryValue(iSCTDead,xAOD::numberOfSCTDeadSensors)) sctDead = iSCTDead;	
		m_recoMatchProb->Fill(prob);
		m_recoPt->Fill(pt);
		m_recoEta->Fill(eta);
		m_recoPhi->Fill(phi);
		m_nSCTHits->Fill(sctHits);
		m_nPixHits->Fill(pixHits);
		m_nTotHits->Fill(sctHits+pixHits);
		m_nSCTDeadSensors->Fill(sctDead);
		m_nPixDeadSensors->Fill(pixDead);
		m_nTotDeadSensors->Fill(sctDead+pixDead);


		m_nSCTHits_vs_eta->Fill(eta,sctHits);
		m_nPixHits_vs_eta->Fill(eta,pixHits);
		m_nTotHits_vs_eta->Fill(eta,sctHits+pixHits);
		m_nSCTDeadSensors_vs_eta->Fill(eta,sctDead);
		m_nPixDeadSensors_vs_eta->Fill(eta,pixDead);
		m_nTotDeadSensors_vs_eta->Fill(eta,sctDead+pixDead);
}


void
InDetPerfPlot_spectrum::fillSpectrum(const xAOD::TruthParticle& particle){

  double pt = particle.pt()/1000;
 // double eta = particle.eta();
  double eta = 0.0;
	if(particle.pt() > 1e-7) eta = particle.eta();

	double phi = particle.phi();

  m_truthPt->Fill(pt);
  m_truthEta->Fill(eta);
  m_truthPhi->Fill(phi);
}

