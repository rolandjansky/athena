/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "./ForceMSConditionsAlg.h"

StatusCode ForceMSConditionsAlg::initialize() {
  ATH_CHECK( m_ALineKey.initialize() );
  ATH_CHECK( m_BLineKey.initialize() );
  ATH_CHECK( m_ILineKey.initialize() );
  ATH_CHECK( m_MdtAsBuiltKey.initialize() );
  return StatusCode::SUCCESS;
}
