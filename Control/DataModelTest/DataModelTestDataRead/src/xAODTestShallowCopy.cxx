/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file  src/xAODTestShallowCopy.cxx
 * @author snyder@bnl.gov
 * @date Jun, 2014
 * @brief Algorithm to test shallow-copy of xAOD data.
 */


#include "xAODTestShallowCopy.h"
#include "DataModelTestDataCommon/CVec.h"
#include "DataModelTestDataCommon/C.h"
#include "DataModelTestDataCommon/CAuxContainer.h"
#include "DataModelTestDataCommon/CTrigAuxContainer.h"
#include "DataModelTestDataCommon/CInfoAuxContainer.h"
#include "xAODCore/ShallowCopy.h"
#include "AthContainers/AuxTypeRegistry.h"
#include "AthenaKernel/errorcheck.h"


namespace DMTest {


/**
 * @brief Constructor.
 * @param name The algorithm name.
 * @param svc The service locator.
 */
xAODTestShallowCopy::xAODTestShallowCopy (const std::string &name,
                            ISvcLocator *pSvcLocator)
  : AthAlgorithm (name, pSvcLocator),
    m_count(0)
{
  declareProperty ("ReadPrefix",  m_readPrefix);
  declareProperty ("WritePrefix", m_writePrefix);
}
  

/**
 * @brief Algorithm initialization; called at the beginning of the job.
 */
StatusCode xAODTestShallowCopy::initialize()
{
  return StatusCode::SUCCESS;
}


/**
 * @brief Algorithm event processing.
 */
StatusCode xAODTestShallowCopy::execute()
{
  ++m_count;

  static C::Accessor<int> anInt10 ("anInt10");

  {
    const CVec* vec = 0;
    CHECK( evtStore()->retrieve (vec, m_readPrefix + "cvec") );
    auto ret = xAOD::shallowCopyContainer (*vec);

    for (C* c : *ret.first)
      anInt10(*c) = m_count * 20000 + c->anInt() * 100;

    CHECK (evtStore()->record (ret.first, m_writePrefix + "cvec"));
    CHECK (evtStore()->record (ret.second, m_writePrefix + "cvecAux."));
    CHECK (evtStore()->setConst (ret.first));
    CHECK (evtStore()->setConst (ret.second));
  }

  {
    const C* cinfo = 0;
    CHECK( evtStore()->retrieve (cinfo, m_readPrefix + "cinfo") );
    auto ret = xAOD::shallowCopyObject (*cinfo);

    anInt10(*ret.first) = m_count * 20000 + ret.first->anInt() * 200;

    CHECK (evtStore()->record (ret.first, m_writePrefix + "cinfo"));
    CHECK (evtStore()->record (ret.second, m_writePrefix + "cinfoAux."));
    CHECK (evtStore()->setConst (ret.first));
    CHECK (evtStore()->setConst (ret.second));
  }

  {
    const CVec* ctrig = 0;
    CHECK( evtStore()->retrieve (ctrig, m_readPrefix + "ctrig") );
    auto ret = xAOD::shallowCopyContainer (*ctrig);

    for (C* c : *ret.first)
      anInt10(*c) = m_count * 20000 + c->anInt() * 300;

    CHECK (evtStore()->record (ret.first, m_writePrefix + "ctrig"));
    CHECK (evtStore()->record (ret.second, m_writePrefix + "ctrigAux."));
    CHECK (evtStore()->setConst (ret.first));
    CHECK (evtStore()->setConst (ret.second));
  }

  return StatusCode::SUCCESS;
}


/**
 * @brief Algorithm finalization; called at the end of the job.
 */
StatusCode xAODTestShallowCopy::finalize()
{
  return StatusCode::SUCCESS;
}


} // namespace DMTest

