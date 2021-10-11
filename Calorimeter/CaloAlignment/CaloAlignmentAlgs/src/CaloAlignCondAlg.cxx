/*
   Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "CaloAlignCondAlg.h"

#include "CaloDetDescrUtils/CaloDetDescrBuilder.h"
#include "AthenaKernel/getMessageSvc.h"

#include <memory>

StatusCode CaloAlignCondAlg::initialize()
{
  ATH_MSG_DEBUG("initialize " << name());

  ATH_CHECK(m_condSvc.retrieve());
  ATH_CHECK(m_readKeyGeoAlign.initialize());
  ATH_CHECK(m_readKeyCellPosShift.initialize());
  ATH_CHECK(m_writeCaloMgrKey.initialize());

  // Register Write Cond Handle
  if(m_condSvc->regHandle(this, m_writeCaloMgrKey).isFailure()) {
    ATH_MSG_ERROR("unable to register WriteCondHandle " << m_writeCaloMgrKey.fullKey() << " with CondSvc");
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

StatusCode CaloAlignCondAlg::execute(const EventContext& ctx) const
{
  // ____________ Construct Write Cond Handle and check its validity ____________
  SG::WriteCondHandle<CaloDetDescrManager> writeCaloMgrHandle{m_writeCaloMgrKey,ctx};
  if (writeCaloMgrHandle.isValid()) {
    ATH_MSG_DEBUG("Found valid write handle");
    return StatusCode::SUCCESS;
  }
  
  // ____________ Get Read Cond Objects ____________
  // 1. GeoAlignmentStore
  SG::ReadCondHandle<GeoAlignmentStore> readHandleGeoAlign{m_readKeyGeoAlign,ctx};
  ATH_CHECK(readHandleGeoAlign.isValid());
  ATH_MSG_DEBUG("Retrieved GeoAlignmentStore object form the Condition Store");
  writeCaloMgrHandle.addDependency(readHandleGeoAlign);

  // 2. CaloCellPositionShift
  SG::ReadCondHandle<CaloRec::CaloCellPositionShift> readHandleCellPosShift{m_readKeyCellPosShift,ctx};
  ATH_CHECK(readHandleCellPosShift.isValid());
  ATH_MSG_DEBUG("Retrieved CaloRec::CaloCellPositionShift object form the Condition Store");
  writeCaloMgrHandle.addDependency(readHandleCellPosShift);

  // ____________ Build new CaloDetDescrManager _________________
  std::unique_ptr<CaloDetDescrManager> caloMgr = buildCaloDetDescr(serviceLocator()
								   , Athena::getMessageSvc()
								   , *readHandleGeoAlign
								   , *readHandleCellPosShift);

  ATH_CHECK(writeCaloMgrHandle.record(std::move(caloMgr)));
  ATH_MSG_INFO("recorded new CaloDetDescr Manager condition object with key " << writeCaloMgrHandle.key() 
	       << " and range " << writeCaloMgrHandle.getRange());

  return StatusCode::SUCCESS;
}
