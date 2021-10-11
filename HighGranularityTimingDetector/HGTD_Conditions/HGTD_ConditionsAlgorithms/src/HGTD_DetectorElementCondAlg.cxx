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

  // used only if they exist
  ATH_CHECK(m_trtDetElContKey.initialize());
  ATH_CHECK(m_muonManagerKey.initialize());
  ATH_CHECK(m_pixelReadKey.initialize());
  ATH_CHECK(m_SCT_readKey.initialize());

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

  // Add dependency for IOV range
  // Additional dependencies for IOV range to limit lifetime to TrackingGeometry lifetime
  for (const SG::ReadCondHandleKey<MuonGM::MuonDetectorManager> &key :m_muonManagerKey ) {
    SG::ReadCondHandle<MuonGM::MuonDetectorManager> muonDependency{key, ctx};
    if (*muonDependency != nullptr){
       writeHandle.addDependency(muonDependency);
    } else {
       ATH_MSG_WARNING("MuonManager not found, ignoring Muons for PixelDetElement lifetime");
    }
  }
  for (const SG::ReadCondHandleKey<InDetDD::TRT_DetElementContainer> &key :m_trtDetElContKey ) {
    SG::ReadCondHandle<InDetDD::TRT_DetElementContainer> trtDependency{key, ctx};
    if (*trtDependency != nullptr){
      writeHandle.addDependency(trtDependency);
    } else {
      ATH_MSG_WARNING("TRT DetEls not found, ignoring TRT for PixelDetElement lifetime");
    }
  }
  for (const SG::ReadCondHandleKey<GeoAlignmentStore> &key :m_pixelReadKey ) {
    SG::ReadCondHandle<GeoAlignmentStore> pixelDependency{key, ctx};
    if (*pixelDependency != nullptr){
      writeHandle.addDependency(pixelDependency);
    } else {
      ATH_MSG_WARNING("Pixel AlignmentStore not found, ignoring Pixels for SCT_DetElement lifetime");
    }
  }
  for (const SG::ReadCondHandleKey<GeoAlignmentStore> &key :m_SCT_readKey ) {
    SG::ReadCondHandle<GeoAlignmentStore> sctDependency{key, ctx};
    if (*sctDependency != nullptr){
      writeHandle.addDependency(sctDependency);
    } else {
      ATH_MSG_WARNING("SCT AlignmentStore not found, ignoring SCT for PixelDetElement lifetime");
    }
  }

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
    // Layer of old element is set by HGTDet::HGTD_LayerBuilderCond::registerSurfacesToLayer.
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
