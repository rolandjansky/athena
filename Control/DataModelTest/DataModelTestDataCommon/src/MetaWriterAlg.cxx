/*
 * Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration.
 */
// $Id$
/**
 * @file DataModelTestDataCommon/MetaWriterAlg.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Dec, 2017
 * @brief Write some metadata objects for testing purposes.
 */


#include "MetaWriterAlg.h"
#include "DataModelTestDataCommon/S1.h"
#include "DataModelTestDataCommon/C.h"
#include "DataModelTestDataCommon/CInfoAuxContainer.h"


namespace DMTest {


/**
 * @brief Constructor.
 * @param name The algorithm name.
 * @param pSvcLocator The service locator.
 */
MetaWriterAlg::MetaWriterAlg (const std::string &name, ISvcLocator *pSvcLocator)
  : AthAlgorithm (name, pSvcLocator),
    m_metaStore ("MetaDataStore", name)
{
  declareProperty ("MetaDataStore", m_metaStore);
}


/**
 * @brief Algorithm initialization; called at the beginning of the job.
 */
StatusCode MetaWriterAlg::initialize()
{
  ATH_CHECK( m_metaStore.retrieve() );
  ATH_CHECK( m_metaStore->record (std::make_unique<DMTest::S1>(24), "MetaS1") );

  auto c = std::make_unique<DMTest::C>();
  auto cstore = std::make_unique<DMTest::CInfoAuxContainer>();
  c->setStore (cstore.get());
  c->setAFloat (2.5);
  c->setAnInt (3);
  ATH_CHECK( m_metaStore->record (std::move(cstore), "MetaCAux.") );
  ATH_CHECK( m_metaStore->record (std::move(c), "MetaC") );
  
  return StatusCode::SUCCESS;
}


/**
 * @brief Algorithm execute method.
 */
StatusCode MetaWriterAlg::execute()
{
  return StatusCode::SUCCESS;
}


} // namespace DMTest
