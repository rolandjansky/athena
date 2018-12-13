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
  declareProperty ("S1Key", m_s1Key = "S2");
  declareProperty ("S2Key", m_s2Key = "S2");
  declareProperty ("AliasKey", m_aliasKey = "S2alias");
}


StatusCode xAODTestReadSymlink::initialize()
{
  ATH_CHECK( m_objKey.initialize() );
  ATH_CHECK( m_s1Key.initialize() );
  ATH_CHECK( m_s2Key.initialize() );
  ATH_CHECK( m_aliasKey.initialize() );
  return StatusCode::SUCCESS;
}


StatusCode xAODTestReadSymlink::execute (const EventContext& ctx) const
{
  SG::ReadHandle<SG::AuxElement> c (m_objKey, ctx);
  SG::ReadHandle<DMTest::S1> s1 (m_s1Key, ctx);
  SG::ReadHandle<DMTest::S2> s2 (m_s2Key, ctx);
  SG::ReadHandle<DMTest::S2> alias (m_aliasKey, ctx);

  static const SG::AuxElement::Accessor<int> anInt ("anInt");
  ATH_MSG_INFO( "C (as AuxElement): " << anInt (*c)
                << "; S " << s2->m_x );
  if (s2.cptr() != s1.cptr() || s2.cptr() != alias.cptr())
    ATH_MSG_ERROR( " S objects mismatch! ");
  return StatusCode::SUCCESS;
}


} // namespace DMTest
