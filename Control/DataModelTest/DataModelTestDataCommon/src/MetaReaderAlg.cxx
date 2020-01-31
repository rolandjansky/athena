/*
 * Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration.
 */
// $Id$
/**
 * @file DataModelTestDataCommon/MetaReaderAlg.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Dec, 2017
 * @brief Test reading metadata objects.
 */


#include "MetaReaderAlg.h"
#include "DataModelTestDataCommon/S1.h"
#include "PersistentDataModel/DataHeader.h"

namespace DMTest {


/**
 * @brief Constructor.
 * @param name The algorithm name.
 * @param pSvcLocator The service locator.
 */
MetaReaderAlg::MetaReaderAlg (const std::string &name, ISvcLocator *pSvcLocator)
  : AthReentrantAlgorithm (name, pSvcLocator),
    m_s1Key ("MetaS1"),
    m_cKey ("MetaC")
{
}


/**
 * @brief Algorithm initialization; called at the beginning of the job.
 */
StatusCode MetaReaderAlg::initialize()
{
  ATH_CHECK( m_s1Key.initialize() );
  ATH_CHECK( m_cKey.initialize() );
  return StatusCode::SUCCESS;
}


/**
 * @brief Algorithm execute method.
 * @param ctx Event context.
 */
StatusCode MetaReaderAlg::execute (const EventContext& ctx) const
{
  // Get proper dbkey.
  const DataHeader* thisDH = nullptr;
  ATH_CHECK( evtStore()->retrieve(thisDH, "EventSelector") );
  std::string sid = thisDH->begin()->getToken()->dbID().toString();

  // Make a new key with the correct dbkey.
  SG::ReadMetaHandleKey<DMTest::S1> s1Key (m_s1Key.key(), sid);
  ATH_CHECK( s1Key.initialize() );
  SG::ReadMetaHandle<DMTest::S1> s1 (s1Key, ctx);
  ATH_MSG_INFO ("MetaS1: " << s1->m_x);

  // Make a new key with the correct dbkey.
  SG::ReadMetaHandleKey<DMTest::C> cKey (m_cKey.key(), sid);
  ATH_CHECK( cKey.initialize() );
  SG::ReadMetaHandle<DMTest::C> c (cKey, ctx);
  ATH_MSG_INFO ("MetaC: " << c->anInt() << " " << c->aFloat());
  return StatusCode::SUCCESS;
}


} // namespace DMTest
