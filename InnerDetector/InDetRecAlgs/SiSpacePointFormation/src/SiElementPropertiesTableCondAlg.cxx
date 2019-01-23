/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "SiElementPropertiesTableCondAlg.h"

#include "InDetIdentifier/SCT_ID.h"
#include "InDetReadoutGeometry/SiDetectorElement.h"

#include <memory>

namespace InDet {

  SiElementPropertiesTableCondAlg::SiElementPropertiesTableCondAlg(const std::string& name, ISvcLocator* pSvcLocator)
    : ::AthReentrantAlgorithm(name, pSvcLocator)
    , m_condSvc{"CondSvc", name}
    , m_idHelper{nullptr}
{
}

  StatusCode SiElementPropertiesTableCondAlg::initialize()
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

    ATH_CHECK(detStore()->retrieve(m_idHelper, "SCT_ID"));

    return StatusCode::SUCCESS;
  }

  StatusCode SiElementPropertiesTableCondAlg::execute(const EventContext& ctx) const
  {
    ATH_MSG_DEBUG("execute " << name());

    // ____________ Construct Write Cond Handle and check its validity ____________
    SG::WriteCondHandle<InDet::SiElementPropertiesTable> writeHandle{m_writeKey, ctx};

    // Do we have a valid Write Cond Handle for current time?
    if (writeHandle.isValid()) {
      ATH_MSG_DEBUG("CondHandle " << writeHandle.fullKey() << " is already valid."
                    << ". In theory this should not be called, but may happen"
                    << " if multiple concurrent events are being processed out of order.");
      return StatusCode::SUCCESS;
    }

    // ____________ Get Read Cond Object ____________
    SG::ReadCondHandle<InDetDD::SiDetectorElementCollection> readHandle{m_readKey, ctx};
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

    // ____________ Construct new Write Cond Object ____________
    std::unique_ptr<InDet::SiElementPropertiesTable> writeCdo{std::make_unique<InDet::SiElementPropertiesTable>(*m_idHelper, *readCdo, m_epsWidth)};

    // ____________ Fill writeCdo using readCdo ____________

    // Record WriteCondHandle
    if (writeHandle.record(rangeW, std::move(writeCdo)).isFailure()) {
      ATH_MSG_FATAL("Could not record " << writeHandle.key()
                    << " with EventRange " << rangeW
                    << " into Conditions Store");
      return StatusCode::FAILURE;
    }
    ATH_MSG_INFO("recorded new CDO " << writeHandle.key() << " with range " << rangeW << " into ConditionStore");

    return StatusCode::SUCCESS;
  }

  StatusCode SiElementPropertiesTableCondAlg::finalize()
  {
    ATH_MSG_DEBUG("finalize " << name());

    return StatusCode::SUCCESS;
  }

}
