/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file DataModelTestDataCommon/src/xAODTestShallowCopy.cxx
 * @author snyder@bnl.gov
 * @date Nov 2020, from earlier version Jun, 2014
 * @brief Algorithm to test shallow-copy of xAOD data.
 */


#include "xAODTestShallowCopy.h"
#include "DataModelTestDataCommon/CVec.h"
#include "DataModelTestDataCommon/CVecWithData.h"
#include "DataModelTestDataCommon/C.h"
#include "DataModelTestDataCommon/CAuxContainer.h"
#include "DataModelTestDataCommon/CTrigAuxContainer.h"
#include "DataModelTestDataCommon/CInfoAuxContainer.h"
#include "xAODCore/ShallowCopy.h"
#include "StoreGate/ReadHandle.h"
#include "StoreGate/WriteHandle.h"
#include "StoreGate/WriteDecorHandle.h"
#include "AthContainers/AuxTypeRegistry.h"
#include "AthenaKernel/errorcheck.h"


namespace DMTest {


/**
 * @brief Algorithm initialization; called at the beginning of the job.
 */
StatusCode xAODTestShallowCopy::initialize()
{
  if (m_cvecReadKey.empty()) {
    m_cvecWriteKey = "";
    m_cvecAnInt10Key = "";
  }
  ATH_CHECK( m_cvecReadKey.initialize( !m_cvecReadKey.empty() ) );
  ATH_CHECK( m_cvecWriteKey.initialize( !m_cvecReadKey.empty() ) );
  ATH_CHECK( m_cvecAnInt10Key.initialize( !m_cvecReadKey.empty() ) );
  ATH_CHECK( m_cvecDecorDeps.initialize( m_cvecReadKey, m_cvecWriteKey, !m_cvecReadKey.empty() ) );

  if (m_cvecWDReadKey.empty()) {
    m_cvecWDWriteKey = "";
    m_cvecWDAnInt10Key = "";
  }
  ATH_CHECK( m_cvecWDReadKey.initialize( !m_cvecWDReadKey.empty() ) );
  ATH_CHECK( m_cvecWDWriteKey.initialize( !m_cvecWDReadKey.empty() ) );
  ATH_CHECK( m_cvecWDAnInt10Key.initialize( !m_cvecWDReadKey.empty() ) );
  ATH_CHECK( m_cvecWDDecorDeps.initialize( m_cvecWDReadKey, m_cvecWDWriteKey, !m_cvecWDReadKey.empty() ) );

  if (m_cinfoReadKey.empty()) {
    m_cinfoWriteKey = "";
    m_cinfoAnInt10Key = "";
  }
  ATH_CHECK( m_cinfoReadKey.initialize( !m_cinfoReadKey.empty() ) );
  ATH_CHECK( m_cinfoWriteKey.initialize( !m_cinfoReadKey.empty() ) );
  ATH_CHECK( m_cinfoAnInt10Key.initialize( !m_cinfoReadKey.empty() ) );
  ATH_CHECK( m_cinfoDecorDeps.initialize( m_cinfoReadKey, m_cinfoWriteKey, !m_cinfoReadKey.empty() ) );

  if (m_ctrigReadKey.empty()) {
    m_ctrigWriteKey = "";
    m_ctrigAnInt10Key = "";
  }
  ATH_CHECK( m_ctrigReadKey.initialize( !m_ctrigReadKey.empty() ) );
  ATH_CHECK( m_ctrigWriteKey.initialize( !m_ctrigReadKey.empty() ) );
  ATH_CHECK( m_ctrigAnInt10Key.initialize( !m_ctrigReadKey.empty() ) );
  ATH_CHECK( m_ctrigDecorDeps.initialize( m_ctrigReadKey, m_ctrigWriteKey, !m_ctrigReadKey.empty() ) );

  return StatusCode::SUCCESS;
}


/**
 * @brief Algorithm event processing.
 */
StatusCode xAODTestShallowCopy::execute (const EventContext& ctx) const
{
  int count = ctx.evt() + 1;

  if (!m_cvecReadKey.empty()) {
    SG::ReadHandle<CVec> vec (m_cvecReadKey, ctx);
    auto ret = xAOD::shallowCopyContainer (*vec);

    SG::WriteHandle<DMTest::CVec> copy (m_cvecWriteKey, ctx);
    ATH_CHECK( copy.record (std::unique_ptr<DMTest::CVec>(ret.first),
                            std::unique_ptr<xAOD::ShallowAuxContainer>(ret.second)) );

    ATH_CHECK( m_cvecDecorDeps.linkDecors (m_cvecReadKey, ctx) );

    SG::WriteDecorHandle<DMTest::CVec, int> anInt10 (m_cvecAnInt10Key, ctx);
    for (C* c : *ret.first)
      anInt10(*c) = count * 20000 + c->anInt() * 100;
  }

  if (!m_cvecWDReadKey.empty()) {
    SG::ReadHandle<CVecWithData> vec (m_cvecWDReadKey, ctx);
    auto ret = xAOD::shallowCopyContainer (*vec);
    ret.first->meta1 = vec->meta1;

    SG::WriteHandle<DMTest::CVecWithData> copy (m_cvecWDWriteKey, ctx);
    ATH_CHECK( copy.record (std::unique_ptr<DMTest::CVecWithData>(ret.first),
                            std::unique_ptr<xAOD::ShallowAuxContainer>(ret.second)) );

    ATH_CHECK( m_cvecWDDecorDeps.linkDecors (m_cvecWDReadKey, ctx) );

    SG::WriteDecorHandle<DMTest::CVecWithData, int> anInt10 (m_cvecWDAnInt10Key, ctx);
    for (C* c : *ret.first)
      anInt10(*c) = count * 20000 + c->anInt() * 100;
  }

  if (!m_cinfoReadKey.empty()) {
    SG::ReadHandle<C> cinfo (m_cinfoReadKey, ctx);
    auto ret = xAOD::shallowCopyObject (*cinfo);

    SG::WriteHandle<DMTest::C> copy (m_cinfoWriteKey, ctx);
    ATH_CHECK( copy.record (std::unique_ptr<DMTest::C>(ret.first),
                            std::unique_ptr<xAOD::ShallowAuxInfo>(ret.second)) );

    ATH_CHECK( m_cinfoDecorDeps.linkDecors (m_cinfoReadKey, ctx) );

    SG::WriteDecorHandle<DMTest::C, int> anInt10 (m_cinfoAnInt10Key, ctx);
    anInt10(*ret.first) = count * 20000 + ret.first->anInt() * 200;
  }

  if (!m_ctrigReadKey.empty()) {
    SG::ReadHandle<CVec> ctrig (m_ctrigReadKey, ctx);
    auto ret = xAOD::shallowCopyContainer (*ctrig);

    SG::WriteHandle<DMTest::CVec> copy (m_ctrigWriteKey, ctx);
    ATH_CHECK( copy.record (std::unique_ptr<DMTest::CVec>(ret.first),
                            std::unique_ptr<xAOD::ShallowAuxContainer>(ret.second)) );

    ATH_CHECK( m_ctrigDecorDeps.linkDecors (m_ctrigReadKey, ctx) );

    SG::WriteDecorHandle<DMTest::CVec, int> anInt10 (m_ctrigAnInt10Key, ctx);
    for (C* c : *ret.first)
      anInt10(*c) = count * 20000 + c->anInt() * 300;
  }

  return StatusCode::SUCCESS;
}


} // namespace DMTest

