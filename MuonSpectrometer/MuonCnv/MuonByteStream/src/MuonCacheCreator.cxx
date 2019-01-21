/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonCacheCreator.h"

#include "MuonIdHelpers/MdtIdHelper.h"
#include "AthViews/View.h"

/// Constructor
MuonCacheCreator::MuonCacheCreator(const std::string &name,ISvcLocator *pSvcLocator):
  AthReentrantAlgorithm(name,pSvcLocator),
  m_MdtCsmCacheKey("")
{
  declareProperty("MdtCsmCacheKey", m_MdtCsmCacheKey);
  declareProperty("DisableViewWarning", m_disableWarning);
}

MuonCacheCreator::~MuonCacheCreator() {

}

StatusCode MuonCacheCreator::initialize() {
  ATH_CHECK( m_MdtCsmCacheKey.initialize(!m_MdtCsmCacheKey.key().empty()) );
  
  ATH_CHECK( detStore()->retrieve(m_mdtIdHelper,"MDTIDHELPER") );
  
  return StatusCode::SUCCESS;
}

bool MuonCacheCreator::isInsideView(const EventContext& context) const
{
   const IProxyDict* proxy = context.getExtension<Atlas::ExtendedEventContext>().proxy();
   const SG::View* view = dynamic_cast<const SG::View*>(proxy);
   return view != nullptr;
}

StatusCode MuonCacheCreator::execute (const EventContext& ctx) const {

  if(!m_disableWarning){
     if(isInsideView(ctx)){
        ATH_MSG_ERROR("CacheCreator is running inside a view, this is probably a misconfiguration");
        return StatusCode::FAILURE;
     }
     m_disableWarning = true; //only check once
  }
  // Create the MDT cache container
  auto maxHashMDTs = m_mdtIdHelper->stationNameIndex("BME") != -1 ?
             m_mdtIdHelper->detectorElement_hash_max() : m_mdtIdHelper->module_hash_max();
  ATH_CHECK(createContainer(m_MdtCsmCacheKey, maxHashMDTs, ctx));

  ATH_MSG_INFO("Created cache container " << m_MdtCsmCacheKey);

  return StatusCode::SUCCESS;
}
