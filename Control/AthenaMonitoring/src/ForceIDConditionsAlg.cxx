/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "./ForceIDConditionsAlg.h"

StatusCode ForceIDConditionsAlg::initialize() {
  ATH_CHECK( m_pixelDetEleCollKey.initialize() );
  ATH_CHECK( m_SCTDetEleCollKey.initialize() );
  ATH_CHECK( m_trtDetEleContKey.initialize() );
  return StatusCode::SUCCESS;
}
