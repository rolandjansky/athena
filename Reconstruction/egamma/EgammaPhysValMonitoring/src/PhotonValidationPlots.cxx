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
m_oTruthPlots(this, "Truth/All/", "Truth Photon"),
m_oTruthIsoPlots(this, "Truth/Iso/all/", "Truth Iso photon"),
m_oTruthIsoConvPlots(this, "Truth/Iso/Conv/", "Truth Iso Conv"),
m_oTruthIsoUncPlots(this, "Truth/Iso/Unc/",  "Truth Iso Unc")
{}	

void PhotonValidationPlots::initializePlots(){
  author = Book1D("author", "Author of "+ m_sParticleType +"; author;Events", 150, -0.5, 149.5);
}

void PhotonValidationPlots::fill(const xAOD::Photon& photon, bool isPrompt){
  
  author->Fill(photon.author());
  m_oAllPlots.fill(photon, isPrompt);
  
  if (photon.author()&xAOD::EgammaParameters::AuthorPhoton) m_oPhotPlots.fill(photon, isPrompt); 
  if (photon.author()&xAOD::EgammaParameters::AuthorCaloTopo35) m_oTopoPhotPlots.fill(photon, isPrompt);    
  if (photon.author()&xAOD::EgammaParameters::AuthorAmbiguous) m_oAmbPhotPlots.fill(photon, isPrompt); 

  //Select converted photons 
  m_oConvPhotPlots.fill(photon,isPrompt);
}
