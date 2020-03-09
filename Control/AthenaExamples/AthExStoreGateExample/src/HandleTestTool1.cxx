/*
 * Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration.
 */
/**
 * @file AthExStoreGateExample/src/HandleTestTool1.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Oct, 2019
 * @brief Test for cross-component circular dependency warning suppression
 *        of WriteDecorHandleKey.
 */


#include "HandleTestTool1.h"


namespace AthEx {


StatusCode HandleTestTool1::initialize()
{
  ATH_CHECK( m_whKey.initialize() );
  return StatusCode::SUCCESS;
}


} // namespace AthEx
