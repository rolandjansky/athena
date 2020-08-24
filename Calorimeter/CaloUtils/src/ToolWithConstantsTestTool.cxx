/*
 * Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration.
 */
/**
 * @file CaloUtils/src/ToolWithConstantsTestTool.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Apr, 2020
 * @brief Algorithm used for testing ToolWithConstants.
 */


#include "CaloUtils/ToolWithConstantsTestTool.h"
#include "StoreGate/StoreGateSvc.h"
#include "AthenaKernel/IRCUSvc.h"
#include "AthenaKernel/ClassID_traits.h"
#include "GaudiKernel/ServiceHandle.h"


namespace CaloUtils {


/**
 * @brief Gaudi initialize method.
 *
 * Make some ToolConstants conditions objects for testing.
 */
StatusCode
ToolWithConstantsTestTool::initialize()
{
  ATH_CHECK( base_class::initialize() );
  if (m_condKey.empty()) {
    return StatusCode::SUCCESS;
  }

  ServiceHandle<StoreGateSvc> condStore ("ConditionStore", name());
  ATH_CHECK( condStore.retrieve() );

  ServiceHandle<Athena::IRCUSvc> rcusvc ("Athena::RCUSvc", name());
  ATH_CHECK( rcusvc.retrieve() );

  typedef CondCont<CaloRec::ToolConstants> CondCont_t;
  DataObjID id (ClassID_traits<CondCont_t>::ID(), m_condKey);
  auto cc = std::make_unique<CondCont_t> (*rcusvc.get(), id);

  auto lbn = [&](int l) { return EventIDBase (m_runNumber,
                                              EventIDBase::UNDEFEVT,
                                              EventIDBase::UNDEFNUM,
                                              EventIDBase::UNDEFNUM,
                                              l); };
  auto range = [&](int l1, int l2) { return EventIDRange (lbn(l1), lbn(l2)); };

  {
    auto tc = std::make_unique<CaloRec::ToolConstants>();
    tc->version (2);
    tc->clsname (name());
    tc->newrep (name(), "test.cb1", "0");
    tc->newrep (name(), "test.ca2", "[[4, 5], [10, 9], [2.5, 1]]");
    ATH_CHECK( cc->insert (range(0, 1), std::move (tc)) );
  }

  {
    auto tc = std::make_unique<CaloRec::ToolConstants>();
    tc->version (2);
    tc->clsname (name());
    tc->newrep (name(), "test.cb1", "1");
    tc->newrep (name(), "test.ca1", "[10, 9, 8, 7]");
    tc->newrep (name(), "test.ca2", "[[14, 15], [1, 19], [12.5, 11]]");
    ATH_CHECK( cc->insert (range(1, 2), std::move (tc)) );
  }

  {
    auto tc = std::make_unique<CaloRec::ToolConstants>();
    tc->version (3);
    tc->clsname (name());
    tc->newrep (name(), "test.cb1", "1");
    tc->newrep (name(), "test.caa", "[10, 9, 8, 7]");
    tc->newrep (name(), "test.ca2", "[[14, 15], [1, 19], [12.5, 11]]");
    ATH_CHECK( cc->insert (range(2, 3), std::move (tc)) );
  }

  {
    auto tc = std::make_unique<CaloRec::ToolConstants>();
    tc->version (2);
    tc->clsname (name());
    ATH_CHECK( cc->insert (range(3, 4), std::move (tc)) );
  }

  ATH_CHECK( condStore->record (std::move (cc), m_condKey) );

  return StatusCode::SUCCESS;
}


/**
 * @brief Gaudi execute method.
 * 
 * Dump out current values of the constants.
 */
StatusCode
ToolWithConstantsTestTool::execute (const EventContext& ctx) const
{
  Context myctx = context (ctx);
#define CHECKCONST(V)                           \
  do {                                          \
    try {                                       \
      auto val = V (myctx);                     \
      ATH_MSG_INFO (#V << " " << val);          \
    }                                           \
    catch (const std::exception& e) {           \
      ATH_MSG_INFO (#V << " " << e.what());     \
    }                                           \
  } while(0)
  CHECKCONST (m_cf1);
  CHECKCONST (m_ci1);
  CHECKCONST (m_cb1);
  CHECKCONST (m_ca1);
  CHECKCONST (m_ca2);
  return StatusCode::SUCCESS;
}


/**
 * @brief A non-default tool version, for testing.
 */
int ToolWithConstantsTestTool::toolVersion() const
{
  return 2;
}


/**
 * @brief For testing writeConstants().
 */
void ToolWithConstantsTestTool::testWriteConstants (const EventContext& ctx) const
{
  std::ostringstream ss;
  this->writeConstants (ss, name(), ctx);
  ATH_MSG_INFO (ss.str());
}


} // namespace CaloUtils
