/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "egammaValidation/ElectronPlots.h"
#include <typeinfo>

using namespace egammaMonitoring;


void ElectronPlots::initializePlots() {
  
  ParticlePlots::initializePlots();
  
  TrkToEl = new TH1D("TrkToEl", ";N; Events of Number Tracks to Electron", 11, 0, 11);
  
} 


void ElectronPlots::fill(const xAOD::Electron& elrec) {
  
  ParticlePlots::fill(elrec);
  
  m_conversionTracks = xAOD::EgammaHelpers::getTrackParticles(&elrec, false);
  TrkToEl->Fill(m_conversionTracks.size());
  
}
  
