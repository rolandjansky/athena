/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "SCTSiPropertiesCondAlg.h"

#include "Identifier/IdentifierHash.h"
#include "InDetIdentifier/SCT_ID.h"
#include "InDetReadoutGeometry/SiDetectorElement.h"

#include "GaudiKernel/EventIDRange.h"

#include <cmath>
#include <memory>

SCTSiPropertiesCondAlg::SCTSiPropertiesCondAlg(const std::string& name, ISvcLocator* pSvcLocator)
  : ::AthReentrantAlgorithm(name, pSvcLocator)
  , m_condSvc{"CondSvc", name}
  , m_pHelper{nullptr}
{
}

StatusCode SCTSiPropertiesCondAlg::initialize() {
  ATH_MSG_DEBUG("initialize " << name());

  // SCT silicon conditions tool
  ATH_CHECK(m_siCondTool.retrieve());

  // SCT ID helper
  ATH_CHECK(detStore()->retrieve(m_pHelper, "SCT_ID"));

  // CondSvc
  ATH_CHECK(m_condSvc.retrieve());
  // Read Cond Handles
  ATH_CHECK(m_readKeyTemp.initialize());
  ATH_CHECK(m_readKeyHV.initialize());
  ATH_CHECK(m_SCTDetEleCollKey.initialize());
  // Write Cond Handle
  ATH_CHECK(m_writeKey.initialize());
  if (m_condSvc->regHandle(this, m_writeKey).isFailure()) {
    ATH_MSG_FATAL("unable to register WriteCondHandle " << m_writeKey.fullKey() << " with CondSvc");
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

StatusCode SCTSiPropertiesCondAlg::execute(const EventContext& ctx) const {
  ATH_MSG_DEBUG("execute " << name());

  // Write Cond Handle
  SG::WriteCondHandle<InDet::SiliconPropertiesVector> writeHandle{m_writeKey, ctx};
  // Do we have a valid Write Cond Handle for current time?
  if (writeHandle.isValid()) {
    ATH_MSG_DEBUG("CondHandle " << writeHandle.fullKey() << " is already valid."
                  << " In theory this should not be called, but may happen"
                  << " if multiple concurrent events are being processed out of order.");
    return StatusCode::SUCCESS; 
  }

  // Read Cond Handle (temperature)
  SG::ReadCondHandle<SCT_DCSFloatCondData> readHandleTemp{m_readKeyTemp, ctx};
  const SCT_DCSFloatCondData* readCdoTemp{*readHandleTemp};
  if (readCdoTemp==nullptr) {
    ATH_MSG_FATAL("Null pointer to the read conditions object");
    return StatusCode::FAILURE;
  }
  EventIDRange rangeTemp;
  if (not readHandleTemp.range(rangeTemp)) {
    ATH_MSG_FATAL("Failed to retrieve validity range for " << readHandleTemp.key());
    return StatusCode::FAILURE;
  }
  ATH_MSG_INFO("Input is " << readHandleTemp.fullKey() << " with the range of " << rangeTemp);

  // Read Cond Handle (HV)
  SG::ReadCondHandle<SCT_DCSFloatCondData> readHandleHV{m_readKeyHV, ctx};
  const SCT_DCSFloatCondData* readCdoHV{*readHandleHV};
  if (readCdoHV==nullptr) {
    ATH_MSG_FATAL("Null pointer to the read conditions object");
    return StatusCode::FAILURE;
  }
  EventIDRange rangeHV;
  if (not readHandleHV.range(rangeHV)) {
    ATH_MSG_FATAL("Failed to retrieve validity range for " << readHandleHV.key());
    return StatusCode::FAILURE;
  }
  ATH_MSG_INFO("Input is " << readHandleHV.fullKey() << " with the range of " << rangeHV);

  // Combined the validity ranges of temp and HV (timestamp IOV)
  EventIDRange rangeW{EventIDRange::intersect(rangeTemp, rangeHV)};
  if (rangeW.stop().isValid() and rangeW.start()>rangeW.stop()) {
    ATH_MSG_FATAL("Invalid intersection range: " << rangeW);
    return StatusCode::FAILURE;
  }

  // Get SCT_DetectorElementCollection
  SG::ReadCondHandle<InDetDD::SiDetectorElementCollection> sctDetEle(m_SCTDetEleCollKey, ctx);
  const InDetDD::SiDetectorElementCollection* elements(sctDetEle.retrieve());
  if (elements==nullptr) {
    ATH_MSG_FATAL(m_SCTDetEleCollKey.fullKey() << " could not be retrieved");
    return StatusCode::FAILURE;
  }
  EventIDRange rangeDetEle; // Run-LB IOV
  if (not sctDetEle.range(rangeDetEle)) {
    ATH_MSG_FATAL("Failed to retrieve validity range for " << m_SCTDetEleCollKey.key());
    return StatusCode::FAILURE;
  }
  
  // Construct the output Cond Object and fill it in
  std::unique_ptr<InDet::SiliconPropertiesVector> writeCdo{std::make_unique<InDet::SiliconPropertiesVector>()};
  const SCT_ID::size_type wafer_hash_max{m_pHelper->wafer_hash_max()};
  writeCdo->resize(wafer_hash_max);
  for (SCT_ID::size_type hash{0}; hash<wafer_hash_max; hash++) {
    const IdentifierHash elementHash{static_cast<IdentifierHash::value_type>(hash)};

    double temperatureC{m_siCondTool->temperature(elementHash)};

    if (not ((temperatureC>m_temperatureMin) and (temperatureC<m_temperatureMax))) {
      ATH_MSG_DEBUG("Invalid temperature: "  
                    << temperatureC << " C. " 
                    << "Setting to " << m_temperatureDefault << " C. " 
                    << "Detector element hash: " << elementHash);
      temperatureC = m_temperatureDefault;
    }

    double temperature{temperatureC + 273.15};
    double deplVoltage{m_siCondTool->depletionVoltage(elementHash) * CLHEP::volt};
    double biasVoltage{m_siCondTool->biasVoltage(elementHash) * CLHEP::volt};

    const InDetDD::SiDetectorElement* element{elements->getDetectorElement(elementHash)};
    double depletionDepth{element->thickness()};
    if (std::abs(biasVoltage)<std::abs(deplVoltage)) {
      depletionDepth *= sqrt(std::abs(biasVoltage/deplVoltage));
    }
    double meanElectricField{0.};
    if (depletionDepth>0.) {
      meanElectricField = biasVoltage/depletionDepth;
    }

    writeCdo->setConditions(hash, temperature, meanElectricField);
  }

  // Record the output cond object
  if (writeHandle.record(rangeW, std::move(writeCdo)).isFailure()) {
    ATH_MSG_FATAL("Could not record SCT_DCSFloatCondData " << writeHandle.key() 
                  << " with EventRange " << rangeW
                  << " into Conditions Store");
    return StatusCode::FAILURE;
  }
  ATH_MSG_INFO("recorded new CDO " << writeHandle.key() << " with range " << rangeW << " into Conditions Store");

  return StatusCode::SUCCESS;
}

StatusCode SCTSiPropertiesCondAlg::finalize()
{
  ATH_MSG_DEBUG("finalize " << name());
  return StatusCode::SUCCESS;
}
