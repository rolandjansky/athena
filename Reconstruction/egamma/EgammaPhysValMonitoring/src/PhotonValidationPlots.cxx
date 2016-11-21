/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "PhotonValidationPlots.h"
#include "xAODEgamma/EgammaDefs.h"

PhotonValidationPlots::PhotonValidationPlots(PlotBase* pParent, std::string sDir):PlotBase(pParent, sDir),
										  m_oAllPlots(this, "PhotAll/", "Reco All"),
										  m_oPhotPlots(this, "Phot/", "Reco Phot"),
										  m_oTopoPhotPlots(this, "PhotTopo/", "Reco Topo Phot"),
										  m_oAmbPhotPlots(this, "PhotAmb/", "Reco Ambig Phot"),
										  m_oConvPhotPlots(this, "Conv/", "Reco Converted"),
										  m_oTruthAllPlots(this, "TruthAll/All/", "Truth Photon"),
										  m_oTruthAllIsoPlots(this, "TruthAll/Iso/all/", "Truth Iso photon"),
										  m_oTruthAllIsoConvPlots(this, "TruthAll/Iso/Conv/", "Truth Iso Conv"),
										  m_oTruthAllIsoUncPlots(this, "TruthAll/Iso/Unc/",  "Truth Iso Unc"),
										  m_oTruthRecoPlots(this, "Truth/Reco/all/", "Reco Truth Photon"),
										  m_oTruthRecoConvPlots(this, "Truth/Reco/Conv/", "Reco Truth Conv Photon"),
										  m_oTruthRecoUncPlots(this, "Truth/Reco/Unc/", "Reco Truth Unc Photon"),
										  m_oTruthRecoLoosePlots(this, "Truth/RecoLoose/all/", "Reco Truth as loose Photon"),
										  m_oTruthRecoLooseConvPlots(this, "Truth/RecoLoose/Conv/", "Reco Truth Conv as loose Photon"),
										  m_oTruthRecoLooseUncPlots(this, "Truth/RecoLoose/Unc/", "Reco Truth Unc as loose Photon"),
										  m_oTruthRecoTightPlots(this, "Truth/RecoTight/all/", "Reco Truth as tight Photon"),
										  m_oTruthRecoTightConvPlots(this, "Truth/RecoTight/Conv/", "Reco Truth Conv as tight Photon"),
										  m_oTruthRecoTightUncPlots(this, "Truth/RecoTight/Unc/", "Reco Truth Unc as tight Photon"),
										  m_oTruthIsoPlots(this, "Truth/Iso/all/", "Truth Iso photon"),
										  m_oTruthIsoConvPlots(this, "Truth/Iso/Conv/", "Truth Iso Conv"),
										  m_oTruthIsoUncPlots(this, "Truth/Iso/Unc/",  "Truth Iso Unc"),
										  author(0),
										  convTruthR(0),
										  convTruthMatchedR(0),
										  convTruthRvsEta(0),
										  convTruthMatchedRvsEta(0),
										  res_et(0),
										  res_eta(0),
										  res_et_cut(0),
										  res_eta_cut(0)
										  
{}	
void PhotonValidationPlots::initializePlots(){
  author = Book1D("author", "Author of "+ m_sParticleType +"; author;Events", 150, -0.5, 149.5);
  convTruthR = Book1D("convTruthR", "Radius Of conversion vertex for truth" + m_sParticleType + ";convR;Conversions", 1200,0.,1200.);
  convTruthMatchedR = Book1D("convTruthMatchedR", "Radius Of conversion vertex for truth matched to reco" + m_sParticleType + ";convR;Conversions", 1200,0.,1200.);
  convTruthRvsEta = Book2D("convTruthRvsEta", "Radius Of conversion vertex for truth vs #eta" + m_sParticleType + ";convR;#eta", 1200,0.,1200., 1000,-5.,5.);
  convTruthMatchedRvsEta = Book2D("convTruthMatchedRvsEta", "Radius Of conversion vertex for truth matched to reco vs #eta" + m_sParticleType + ";convR;#eta", 1200,0.,1200., 1000,-5.,5.);
  res_et  = BookTProfile("res_et"," IsoPhoton;E_{T}^{truth}, [GeV];(E_{T} - E_{T}^{truth})/E_{T}^{truth}",100, 0., 200.);
  res_eta = BookTProfile("res_eta"," IsoPhoton;#eta;(E_{T} - E_{T}^{truth})/E_{T}^{truth}",60, -3., 3.);
  res_et_cut  = BookTProfile("res_et_cut"," IsoPhoton;E_{T}^{truth}, [GeV];(E_{T} - E_{T}^{truth})/E_{T}^{truth}",100, 0., 200.);
  res_eta_cut = BookTProfile("res_eta_cut"," IsoPhoton;#eta;(E_{T} - E_{T}^{truth})/E_{T}^{truth}",60, -3., 3.);
}

void PhotonValidationPlots::fill(const xAOD::Photon& photon, bool isPrompt){
  
  author->Fill(photon.author());
  m_oAllPlots.fill(photon, isPrompt);

  double photon_pt = photon.pt()*0.001;
  if (photon.author()&xAOD::EgammaParameters::AuthorPhoton&&photon_pt>7.) m_oPhotPlots.fill(photon, isPrompt); 
  if (photon_pt<7.) m_oTopoPhotPlots.fill(photon, isPrompt);    
  if (photon.author()&xAOD::EgammaParameters::AuthorAmbiguous&&photon_pt>7.) m_oAmbPhotPlots.fill(photon, isPrompt); 

  //Select converted photons 
  if (photon_pt>7.)m_oConvPhotPlots.fill(photon,isPrompt);
}
