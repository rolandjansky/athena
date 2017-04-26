/*
 * Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration.
 */

// $Id$
/**
 * @file DataModelTestDataCommon/src/xAODTestReadSymlink.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Apr, 2017
 * @brief Test reading via a symlink.
 */


#include "xAODTestReadSymlink.h"
#include "StoreGate/ReadHandle.h"


namespace DMTest {


xAODTestReadSymlink::xAODTestReadSymlink (const std::string &name, ISvcLocator *pSvcLocator)
  : AthReentrantAlgorithm (name, pSvcLocator)
{
  declareProperty ("Key", m_objKey);
}


StatusCode xAODTestReadSymlink::initialize()
{
  ATH_CHECK( m_objKey.initialize() );
  return StatusCode::SUCCESS;
}


StatusCode xAODTestReadSymlink::execute_r (const EventContext& ctx) const
{
  SG::ReadHandle<SG::AuxElement> c (m_objKey, ctx);

  static const SG::AuxElement::Accessor<int> anInt ("anInt");
  ATH_MSG_INFO( "C (as AuxElement): " << anInt (*c) );
  return StatusCode::SUCCESS;
}


} // namespace DMTest
