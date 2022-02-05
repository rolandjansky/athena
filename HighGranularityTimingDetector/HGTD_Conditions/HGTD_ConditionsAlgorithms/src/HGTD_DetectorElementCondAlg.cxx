/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "HGTD_DetectorElementCondAlg.h"

#include "HGTD_ReadoutGeometry/HGTD_DetectorManager.h"
#include "HGTD_ReadoutGeometry/HGTD_DetectorElement.h"
#include "TrkGeometry/Layer.h"
#include "TrkSurfaces/Surface.h"
#include "AthenaKernel/IOVInfiniteRange.h"

#include <map>

HGTD_DetectorElementCondAlg::HGTD_DetectorElementCondAlg(const std::string& name, ISvcLocator* pSvcLocator)
  : ::AthReentrantAlgorithm(name, pSvcLocator)
{
}

StatusCode HGTD_DetectorElementCondAlg::initialize()
{
  ATH_MSG_DEBUG("initialize " << name());

  // Write Handle
  ATH_CHECK(m_writeKey.initialize());

  // CondSvc
  ATH_CHECK(m_condSvc.retrieve());

  // Register write handle
  ATH_CHECK(m_condSvc->regHandle(this, m_writeKey));

  // We need the detector manager
  ATH_CHECK(detStore()->retrieve(m_detManager, m_detManagerName));

  return StatusCode::SUCCESS;
}

StatusCode HGTD_DetectorElementCondAlg::execute(const EventContext& ctx) const
{
  ATH_MSG_DEBUG("execute " << name());

  // ____________ Construct Write Cond Handle and check its validity ____________
  SG::WriteCondHandle<InDetDD::HGTD_DetectorElementCollection> writeHandle{m_writeKey, ctx};

  // Do we have a valid Write Cond Handle for current time?
  if (writeHandle.isValid()) {
    ATH_MSG_DEBUG("CondHandle " << writeHandle.fullKey() << " is already valid."
                  << ". In theory this should not be called, but may happen"
                  << " if multiple concurrent events are being processed out of order.");
    return StatusCode::SUCCESS;
  }

  const InDetDD::HGTD_DetectorElementCollection* oldColl{m_detManager->getDetectorElementCollection()};
  if (oldColl==nullptr) {
    ATH_MSG_FATAL("Null pointer is returned by getDetectorElementCollection()");
    return StatusCode::FAILURE;
  }

  // ____________ Construct new Write Cond Object ____________
  std::unique_ptr<InDetDD::HGTD_DetectorElementCollection> writeCdo{std::make_unique<InDetDD::HGTD_DetectorElementCollection>()};
 
  // Make sure we make a mixed IOV.
  writeHandle.addDependency (IOVInfiniteRange::infiniteMixed());
  
  // ____________ Update writeCdo ____________
  std::map<const InDetDD::HGTD_DetectorElement*, const InDetDD::HGTD_DetectorElement*> oldToNewMap;
  oldToNewMap[nullptr] = nullptr;
  writeCdo->resize(oldColl->size(), nullptr);
  InDetDD::HGTD_DetectorElementCollection::iterator newEl{writeCdo->begin()};
  for (const InDetDD::HGTD_DetectorElement* oldEl: *oldColl) {
    *newEl = new InDetDD::HGTD_DetectorElement(oldEl->identify(),     
                                               &(oldEl->design()),
                                               oldEl->GeoVDetectorElement::getMaterialGeom(),
                                               oldEl->getCommonItems());
    oldToNewMap[oldEl] = *newEl;
    ++newEl;
  }

  // Set layer to surface
  InDetDD::HGTD_DetectorElementCollection::const_iterator oldIt{oldColl->begin()};
  for (InDetDD::HGTD_DetectorElement* newEl: *writeCdo) {
    if (oldToNewMap[(*oldIt)]!=newEl) {
      ATH_MSG_ERROR("Old and new elements are not synchronized!");
    }
    // Layer of old element is set by HGTD_LayerBuilderCond::registerSurfacesToLayer.
    const Trk::Layer* layer{(*oldIt)->surface().associatedLayer()};
    if (layer) {
      newEl->surface().associateLayer(*layer);
    }
    ++oldIt;
  }

  // Apply alignment using readCdo passed to HGTD_DetectorElement
  for (InDetDD::HGTD_DetectorElement* newEl: *writeCdo) {
    newEl->setCache();
  }

  // Record WriteCondHandle
  const std::size_t size{writeCdo->size()};
  if (writeHandle.record(std::move(writeCdo)).isFailure()) {
    ATH_MSG_FATAL("Could not record " << writeHandle.key() 
                  << " with EventRange " << writeHandle.getRange()
                  << " into Conditions Store");
    return StatusCode::FAILURE;
  }
  ATH_MSG_INFO("recorded new CDO " << writeHandle.key() << " with range " <<  writeHandle.getRange()
      << " with size of " << size << " into Conditions Store");

  return StatusCode::SUCCESS;
}
