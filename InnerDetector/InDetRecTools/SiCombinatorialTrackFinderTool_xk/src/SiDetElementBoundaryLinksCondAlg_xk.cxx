/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "SiDetElementBoundaryLinksCondAlg_xk.h"

#include "InDetReadoutGeometry/SCT_DetectorManager.h"
#include "InDetReadoutGeometry/SiDetectorElement.h"
#include "SiCombinatorialTrackFinderTool_xk/SiDetElementBoundaryLink_xk.h"
#include "StoreGate/ReadCondHandle.h"
#include "StoreGate/WriteCondHandle.h"

namespace InDet {

  SiDetElementBoundaryLinksCondAlg_xk::SiDetElementBoundaryLinksCondAlg_xk(const std::string& name, ISvcLocator* pSvcLocator)
    : ::AthAlgorithm(name, pSvcLocator)
    , m_condSvc{"CondSvc", name}
    , m_detManager{nullptr}
{
}

  StatusCode SiDetElementBoundaryLinksCondAlg_xk::initialize()
  {
    ATH_MSG_DEBUG("initialize " << name());

    // Read Handle
    ATH_CHECK(m_readKey.initialize());

    // Write Handle
    ATH_CHECK(m_writeKey.initialize());
    // CondSvc
    ATH_CHECK(m_condSvc.retrieve());
    // Register write handle
    ATH_CHECK(m_condSvc->regHandle(this, m_writeKey));

    ATH_CHECK(detStore()->retrieve(m_detManager, "SCT"));

    return StatusCode::SUCCESS;
  }

  StatusCode SiDetElementBoundaryLinksCondAlg_xk::execute()
  {
    ATH_MSG_DEBUG("execute " << name());

    // ____________ Construct Write Cond Handle and check its validity ____________
    SG::WriteCondHandle<InDet::SiDetElementBoundaryLinks_xk> writeHandle{m_writeKey};

    // Do we have a valid Write Cond Handle for current time?
    if (writeHandle.isValid()) {
      ATH_MSG_DEBUG("CondHandle " << writeHandle.fullKey() << " is already valid."
                    << ". In theory this should not be called, but may happen"
                    << " if multiple concurrent events are being processed out of order.");
      return StatusCode::SUCCESS;
    }

    // ____________ Get Read Cond Object ____________
    SG::ReadCondHandle<InDetDD::SiDetectorElementCollection> readHandle{m_readKey};
    const InDetDD::SiDetectorElementCollection* readCdo{*readHandle};
    if (readCdo==nullptr) {
      ATH_MSG_FATAL("Null pointer to the read conditions object of " << m_readKey.key());
      return StatusCode::FAILURE;
    }

    // Define validity of the output cond object and record it
    EventIDRange rangeW;
    if (not readHandle.range(rangeW)) {
      ATH_MSG_FATAL("Failed to retrieve validity range for " << readHandle.key());
      return StatusCode::FAILURE;
    }

    if (m_useDetectorManager) { // For debugging: use SiDetectorElementCollection from SCT_DetectorManager
      readCdo = m_detManager->getDetectorElementCollection();
    }

    // ____________ Construct new Write Cond Object ____________
    // Copied from
    // InnerDetector/InDetRecTools/SiCombinatorialTrackFinderTool_xk/src/SiCombinatorialTrackFinder_xk.cxx
    std::unique_ptr<InDet::SiDetElementBoundaryLinks_xk> writeCdo{std::make_unique<InDet::SiDetElementBoundaryLinks_xk>()};
    // ____________ Fill writeCdo using readCdo ____________
    for (const InDetDD::SiDetectorElement* newEl: *readCdo) {
      InDet::SiDetElementBoundaryLink_xk dl{newEl};
      writeCdo->push_back(dl);
    }

    // Record WriteCondHandle
    if (writeHandle.record(rangeW, std::move(writeCdo)).isFailure()) {
      ATH_MSG_ERROR("Could not record " << writeHandle.key()
                    << " with EventRange " << rangeW
                    << " into Conditions Store");
      return StatusCode::FAILURE;
    }
    ATH_MSG_INFO("recorded new CDO " << writeHandle.key() << " with range " << rangeW << " into ConditionStore");

    return StatusCode::SUCCESS;
  }

  StatusCode SiDetElementBoundaryLinksCondAlg_xk::finalize()
  {
    ATH_MSG_DEBUG("finalize " << name());

    return StatusCode::SUCCESS;
  }

}
