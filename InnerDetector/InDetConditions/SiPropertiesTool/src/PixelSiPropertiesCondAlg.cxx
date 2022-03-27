/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "PixelSiPropertiesCondAlg.h"

#include <cmath>
#include <memory>

#include "GaudiKernel/EventIDRange.h"
#include "InDetReadoutGeometry/SiDetectorElement.h"

PixelSiPropertiesCondAlg::PixelSiPropertiesCondAlg(const std::string& name, ISvcLocator* pSvcLocator):
  ::AthReentrantAlgorithm(name, pSvcLocator),
  m_pixid(nullptr)
{
}

StatusCode PixelSiPropertiesCondAlg::initialize() {
  ATH_MSG_DEBUG("PixelSiPropertiesCondAlg::initialize()");

  ATH_CHECK(detStore()->retrieve(m_pixid,"PixelID"));

  ATH_CHECK(m_readKeyTemp.initialize());
  ATH_CHECK(m_readKeyHV.initialize());
  ATH_CHECK(m_pixelDetEleCollKey.initialize());
  ATH_CHECK(m_writeKey.initialize());

  return StatusCode::SUCCESS;
}

StatusCode PixelSiPropertiesCondAlg::execute(const EventContext& ctx) const {
  ATH_MSG_DEBUG("PixelSiPropertiesCondAlg::execute()");

  SG::WriteCondHandle<InDet::SiliconPropertiesVector> writeHandle(m_writeKey, ctx);
  if (writeHandle.isValid()) {
    ATH_MSG_DEBUG("CondHandle " << writeHandle.fullKey() << " is already valid." << " In theory this should not be called, but may happen" << " if multiple concurrent events are being processed out of order.");
    return StatusCode::SUCCESS; 
  }

  // Read Cond Handle (temperature)
  SG::ReadCondHandle<PixelDCSTempData> readHandleTemp(m_readKeyTemp, ctx);
  const PixelDCSTempData* readCdoTemp(*readHandleTemp);
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
  SG::ReadCondHandle<PixelDCSHVData> readHandleHV(m_readKeyHV, ctx);
  const PixelDCSHVData* readCdoHV(*readHandleHV);
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

  // Combined the validity ranges of temp and HV
  EventIDRange rangeW = EventIDRange::intersect(rangeTemp, rangeHV);
  if (rangeW.stop().isValid() and rangeW.start()>rangeW.stop()) {
    ATH_MSG_FATAL("Invalid intersection range: " << rangeW);
    return StatusCode::FAILURE;
  }

  SG::ReadCondHandle<InDetDD::SiDetectorElementCollection> pixelDetEleHandle(m_pixelDetEleCollKey, ctx);
  const InDetDD::SiDetectorElementCollection* elements(*pixelDetEleHandle);
  if (not pixelDetEleHandle.isValid() or elements==nullptr) {
    ATH_MSG_FATAL(m_pixelDetEleCollKey.fullKey() << " is not available.");
    return StatusCode::FAILURE;
  }

  // Construct the output Cond Object and fill it in
  std::unique_ptr<InDet::SiliconPropertiesVector> writeCdo(std::make_unique<InDet::SiliconPropertiesVector>());
  const PixelID::size_type wafer_hash_max = m_pixid->wafer_hash_max();
  writeCdo->resize(wafer_hash_max);
  for (PixelID::size_type hash=0; hash<wafer_hash_max; hash++) {
    const IdentifierHash elementHash = static_cast<IdentifierHash::value_type>(hash);

    double temperature = readCdoTemp->getTemperature(elementHash)+273.15;
    double deplVoltage = 0.0*CLHEP::volt;
    double biasVoltage = readCdoHV->getBiasVoltage(elementHash)*CLHEP::volt;

    const InDetDD::SiDetectorElement* element = elements->getDetectorElement(elementHash);
    double depletionDepth = element->thickness();
    if (std::abs(biasVoltage) < std::abs(deplVoltage)) {
      depletionDepth *= sqrt(std::abs(biasVoltage/deplVoltage));
    }

    double meanElectricField = 0;
    if (depletionDepth) { meanElectricField = biasVoltage/depletionDepth; }

    writeCdo->setConditions(hash, temperature, meanElectricField);
  }

  // Record the output cond object
  if (writeHandle.record(rangeW, std::move(writeCdo)).isFailure()) {
    ATH_MSG_FATAL("Could not record PixelSiliconPropertiesVector " << writeHandle.key() << " with EventRange " << rangeW << " into Conditions Store");
    return StatusCode::FAILURE;
  }
  ATH_MSG_INFO("recorded new CDO " << writeHandle.key() << " with range " << rangeW << " into Conditions Store");

  return StatusCode::SUCCESS;
}

StatusCode PixelSiPropertiesCondAlg::finalize() {
  ATH_MSG_DEBUG("PixelSiPropertiesCondAlg::finalize()");
  return StatusCode::SUCCESS;
}

