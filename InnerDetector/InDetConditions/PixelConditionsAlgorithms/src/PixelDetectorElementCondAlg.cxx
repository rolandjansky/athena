/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "PixelDetectorElementCondAlg.h"

#include "PixelReadoutGeometry/PixelDetectorManager.h"
#include "InDetReadoutGeometry/SiDetectorElement.h"
#include "TrkGeometry/Layer.h"
#include "TrkSurfaces/Surface.h"
#include "AthenaKernel/IOVInfiniteRange.h"

#include <map>

PixelDetectorElementCondAlg::PixelDetectorElementCondAlg(const std::string& name, ISvcLocator* pSvcLocator)
  : ::AthReentrantAlgorithm(name, pSvcLocator)
{
  declareProperty("ReadKey", m_readKey);
}

StatusCode PixelDetectorElementCondAlg::initialize()
{
  ATH_MSG_DEBUG("initialize " << name());

  // Read Handle
  ATH_CHECK(m_readKey.initialize());

  // Write Handle
  ATH_CHECK(m_writeKey.initialize());

  // We need the detector manager
  ATH_CHECK(detStore()->retrieve(m_detManager, m_detManagerName));

  // used only if they exist
  ATH_CHECK(m_trtDetElContKey.initialize(SG::AllowEmpty));
  ATH_CHECK(m_muonManagerKey.initialize(SG::AllowEmpty));
  ATH_CHECK(m_SCT_readKey.initialize(SG::AllowEmpty));

  return StatusCode::SUCCESS;
}

StatusCode PixelDetectorElementCondAlg::execute(const EventContext& ctx) const
{
  ATH_MSG_DEBUG("execute " << name());

  // ____________ Construct Write Cond Handle and check its validity ____________
  SG::WriteCondHandle<InDetDD::SiDetectorElementCollection> writeHandle{m_writeKey, ctx};

  // Do we have a valid Write Cond Handle for current time?
  if (writeHandle.isValid()) {
    ATH_MSG_DEBUG("CondHandle " << writeHandle.fullKey() << " is already valid."
                  << ". In theory this should not be called, but may happen"
                  << " if multiple concurrent events are being processed out of order.");
    return StatusCode::SUCCESS;
  }

  const InDetDD::SiDetectorElementCollection* oldColl{m_detManager->getDetectorElementCollection()};
  if (oldColl==nullptr) {
    ATH_MSG_FATAL("Null pointer is returned by getDetectorElementCollection()");
    return StatusCode::FAILURE;
  }

  // ____________ Construct new Write Cond Object ____________
  std::unique_ptr<InDetDD::SiDetectorElementCollection> writeCdo{std::make_unique<InDetDD::SiDetectorElementCollection>()};

  // ____________ Get Read Cond Object ____________
  SG::ReadCondHandle<GeoAlignmentStore> readHandle{m_readKey, ctx};
  const GeoAlignmentStore* readCdo{*readHandle};
  if (readCdo==nullptr) {
    ATH_MSG_FATAL("Null pointer to the read conditions object of " << m_readKey.key());
    return StatusCode::FAILURE;
  }
  
  // Make sure we make a mixed IOV.
  writeHandle.addDependency (IOVInfiniteRange::infiniteMixed());

  // Add dependency for IOV range
  writeHandle.addDependency(readHandle);
  // Additional dependencies for IOV range to limit lifetime to TrackingGeometry lifetime
  if (!m_muonManagerKey.empty()) {
    SG::ReadCondHandle<MuonGM::MuonDetectorManager> muonDependency{m_muonManagerKey, ctx};
    if (*muonDependency != nullptr) {
       writeHandle.addDependency(muonDependency);
    } else {
       ATH_MSG_ERROR("MuonManager not found but configured");
       return StatusCode::FAILURE;
    }
  }
  if (!m_trtDetElContKey.empty()) {
    SG::ReadCondHandle<InDetDD::TRT_DetElementContainer> trtDependency{m_trtDetElContKey, ctx};
    if (*trtDependency != nullptr) {
      writeHandle.addDependency(trtDependency);
    } else {
      ATH_MSG_ERROR("TRT DetEls not found but configured");
      return StatusCode::FAILURE;
    }
  }
  if (!m_SCT_readKey.empty()) {
    SG::ReadCondHandle<GeoAlignmentStore> sctDependency{m_SCT_readKey, ctx};
    if (*sctDependency != nullptr) {
      writeHandle.addDependency(sctDependency);
    } else {
      ATH_MSG_ERROR("SCT AlignmentStore not found but configured");
      return StatusCode::FAILURE;
    }
  }

  // ____________ Update writeCdo using readCdo ____________
  std::map<const InDetDD::SiDetectorElement*, const InDetDD::SiDetectorElement*> oldToNewMap;
  oldToNewMap[nullptr] = nullptr;
  writeCdo->resize(oldColl->size(), nullptr);
  InDetDD::SiDetectorElementCollection::iterator newEl{writeCdo->begin()};
  for (const InDetDD::SiDetectorElement* oldEl: *oldColl) {
    *newEl = new InDetDD::SiDetectorElement(oldEl->identify(),
                                            &(oldEl->design()),
                                            oldEl->GeoVDetectorElement::getMaterialGeom(),
                                            oldEl->getCommonItems(),
                                            readCdo);
    oldToNewMap[oldEl] = *newEl;
    ++newEl;
  }

  // Set neighbours and other side
  // Set layer to surface
  InDetDD::SiDetectorElementCollection::const_iterator oldIt{oldColl->begin()};
  for (InDetDD::SiDetectorElement* newEl: *writeCdo) {
    if (oldToNewMap[(*oldIt)]!=newEl) {
      ATH_MSG_ERROR("Old and new elements are not synchronized!");
    }
    newEl->setNextInEta(oldToNewMap[(*oldIt)->nextInEta()]);
    newEl->setPrevInEta(oldToNewMap[(*oldIt)->prevInEta()]);
    newEl->setNextInPhi(oldToNewMap[(*oldIt)->nextInPhi()]);
    newEl->setPrevInPhi(oldToNewMap[(*oldIt)->prevInPhi()]);

    // Layer of old element is set by InDet::SiLayerBuilder::registerSurfacesToLayer.
    const Trk::Layer* layer{(*oldIt)->surface().associatedLayer()};
    if (layer) {
      newEl->surface().associateLayer(*layer);
    }
    ++oldIt;
  }

  // Apply alignment using readCdo passed to SiDetectorElement
  for (InDetDD::SiDetectorElement* newEl: *writeCdo) {
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
