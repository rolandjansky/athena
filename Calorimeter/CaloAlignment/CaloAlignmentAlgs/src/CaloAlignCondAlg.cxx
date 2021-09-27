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
  ATH_CHECK(m_writeKey.initialize());

  // Register Write Cond Handle
  if(m_condSvc->regHandle(this, m_writeKey).isFailure()) {
    ATH_MSG_ERROR("unable to register WriteCondHandle " << m_writeKey.fullKey() << " with CondSvc");
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

StatusCode CaloAlignCondAlg::execute(const EventContext& ctx) const
{
  // ____________ Construct Write Cond Handle and check its validity ____________
  SG::WriteCondHandle<CaloDetDescrManager> writeHandle{m_writeKey,ctx};
  if (writeHandle.isValid()) {
    ATH_MSG_DEBUG("Found valid write handle");
    return StatusCode::SUCCESS;
  }
  
  // ____________ Get Read Cond Objects ____________
  // 1. GeoAlignmentStore
  SG::ReadCondHandle<GeoAlignmentStore> readHandleGeoAlign{m_readKeyGeoAlign,ctx};
  const GeoAlignmentStore* readCdoGeoAlign{*readHandleGeoAlign};
  if(!readCdoGeoAlign) {
    ATH_MSG_ERROR("Null pointer to the read conditions object of type GeoAlignmentStore");
    return StatusCode::FAILURE;
  }
  ATH_MSG_DEBUG("Retrieved GeoAlignmentStore object form the Condition Store");

  writeHandle.addDependency(readHandleGeoAlign);

  // 2. CaloCellPositionShift
  SG::ReadCondHandle<CaloRec::CaloCellPositionShift> readHandleCellPosShift{m_readKeyCellPosShift,ctx};
  const CaloRec::CaloCellPositionShift* readCdoCellPosShift{*readHandleCellPosShift};
  if(!readCdoCellPosShift) {
    ATH_MSG_ERROR("Null pointer to the read conditions object of type CaloRec::CaloCellPositionShift");
    return StatusCode::FAILURE;
  }
  ATH_MSG_DEBUG("Retrieved CaloRec::CaloCellPositionShift object form the Condition Store");

  writeHandle.addDependency(readHandleCellPosShift);

  // ____________ Build new CaloDetDescrManager _________________
  std::unique_ptr<CaloDetDescrManager> caloMgr = buildCaloDetDescr(serviceLocator()
								   , Athena::getMessageSvc()
								   , readCdoGeoAlign
								   , readCdoCellPosShift);

  ATH_CHECK(writeHandle.record(std::move(caloMgr)));
  ATH_MSG_INFO("recorded new CaloDetDescr Manager condition object with key " << writeHandle.key() 
	       << " and range " << writeHandle.getRange());

  return StatusCode::SUCCESS;
}
