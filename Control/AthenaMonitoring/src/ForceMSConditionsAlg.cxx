/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "./ForceMSConditionsAlg.h"

StatusCode ForceMSConditionsAlg::initialize() {
  ATH_CHECK( m_ALineKey.initialize() );
  ATH_CHECK( m_BLineKey.initialize() );
  return StatusCode::SUCCESS;
}
