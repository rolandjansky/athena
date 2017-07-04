/*
 * Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration.
 */

// $Id$
/**
 * @file DataModelTestDataCommon/src/xAODTestReadSymlinkTool.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Jun, 2017
 * @brief Test reading via a symlink, as a tool.
 */


#include "xAODTestReadSymlinkTool.h"
#include "StoreGate/ReadHandle.h"


namespace DMTest {


xAODTestReadSymlinkTool::xAODTestReadSymlinkTool (const std::string& type,
                                                  const std::string& name,
                                                  const IInterface* parent)
  : base_class (type, name, parent)
{
  declareProperty ("Key", m_objKey);
  declareProperty ("S1Key", m_s1Key = "S2");
  declareProperty ("S2Key", m_s2Key = "S2");
  declareProperty ("AliasKey", m_aliasKey = "S2alias");
}


StatusCode xAODTestReadSymlinkTool::initialize()
{
  ATH_CHECK( m_objKey.initialize() );
  ATH_CHECK( m_s1Key.initialize() );
  ATH_CHECK( m_s2Key.initialize() );
  ATH_CHECK( m_aliasKey.initialize() );
  return StatusCode::SUCCESS;
}


StatusCode xAODTestReadSymlinkTool::doit (const EventContext& ctx) const
{
  SG::ReadHandle<SG::AuxElement> c (m_objKey, ctx);
  SG::ReadHandle<DMTest::S1> s1 (m_s1Key, ctx);
  SG::ReadHandle<DMTest::S2> s2 (m_s2Key, ctx);
  SG::ReadHandle<DMTest::S2> alias (m_aliasKey, ctx);

  static const SG::AuxElement::Accessor<int> anInt ("anInt");
  ATH_MSG_INFO( "From tool: C (as AuxElement): " << anInt (*c)
                << "; S " << s2->m_x );
  if (s2.cptr() != s1.cptr() || s2.cptr() != alias.cptr())
    ATH_MSG_ERROR( " S objects mismatch! ");
  return StatusCode::SUCCESS;
}


} // namespace DMTest
