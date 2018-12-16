/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonCacheCreator.h"

#include "MuonIdHelpers/MdtIdHelper.h"

/// Constructor
MuonCacheCreator::MuonCacheCreator(const std::string &name,ISvcLocator *pSvcLocator):
  AthReentrantAlgorithm(name,pSvcLocator),
  m_MdtCsmCacheKey("")
{
  declareProperty("MdtCsmCacheKey", m_MdtCsmCacheKey);
}

MuonCacheCreator::~MuonCacheCreator() {

}

StatusCode MuonCacheCreator::initialize() {
  ATH_CHECK( m_MdtCsmCacheKey.initialize(!m_MdtCsmCacheKey.key().empty()) );
  
  ATH_CHECK( detStore()->retrieve(m_mdtIdHelper,"MDTIDHELPER") );
  
  return StatusCode::SUCCESS;
}

StatusCode MuonCacheCreator::execute (const EventContext& ctx) const {

  // Create the MDT cache container
  auto maxHashMDTs = m_mdtIdHelper->stationNameIndex("BME") != -1 ?
             m_mdtIdHelper->detectorElement_hash_max() : m_mdtIdHelper->module_hash_max();
  ATH_CHECK(CreateContainer(m_MdtCsmCacheKey, maxHashMDTs, ctx));

  ATH_MSG_INFO("Created cache container " << m_MdtCsmCacheKey);

  return StatusCode::SUCCESS;
}
