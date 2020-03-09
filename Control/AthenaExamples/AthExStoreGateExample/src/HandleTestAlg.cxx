/*
 * Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration.
 */
/**
 * @file AthExStoreGateExample/src/HandleTestAlg.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Oct, 2019
 * @brief Test for cross-component circular dependency warning suppression
 *        of WriteDecorHandleKey.
 */


#include "HandleTestAlg.h"


namespace AthEx {


StatusCode HandleTestAlg::initialize()
{
  ATH_CHECK( m_tool1.retrieve() );
  ATH_CHECK( m_tool2.retrieve() );
  return StatusCode::SUCCESS;
}


StatusCode HandleTestAlg::execute (const EventContext&) const
{
  return StatusCode::SUCCESS;
}


} // namespace AthEx
