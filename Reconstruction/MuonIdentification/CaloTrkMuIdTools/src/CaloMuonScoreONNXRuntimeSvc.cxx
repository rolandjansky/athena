// Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

// Local include(s).
#include "CaloTrkMuIdTools/CaloMuonScoreONNXRuntimeSvc.h"

StatusCode CaloMuonScoreONNXRuntimeSvc::initialize() {
  
  // Create the environment object.
  m_env = std::make_unique< Ort::Env >( ORT_LOGGING_LEVEL_WARNING,
					name().c_str() );
  ATH_MSG_DEBUG( "Ort::Env object created" );
  
  // Return gracefully.
  return StatusCode::SUCCESS;
}

StatusCode CaloMuonScoreONNXRuntimeSvc::finalize() {
  
  // Dekete the environment object.
  m_env.reset();
  ATH_MSG_DEBUG( "Ort::Env object deleted" );
  
  // Return gracefully.
  return StatusCode::SUCCESS;
}

Ort::Env& CaloMuonScoreONNXRuntimeSvc::env() const {
  
  return *m_env;
}
