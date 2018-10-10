/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "TruthElectronHistograms.h"
#include <typeinfo>

#include "GaudiKernel/ServiceHandle.h"
#include "AsgTools/AnaToolHandle.h"

using namespace egammaMonitoring;


StatusCode TruthElectronHistograms::initializePlots() {
  
  ATH_CHECK(ParticleHistograms::initializePlots());

  return StatusCode::SUCCESS;


} 


void TruthElectronHistograms::fill(const xAOD::Electron& elrec) {

  ParticleHistograms::fill(elrec);

  
}
  
