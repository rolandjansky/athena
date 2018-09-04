/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "SiPropertiesTool.h"

SiPropertiesTool::SiPropertiesTool(const std::string& type, const std::string& name, const IInterface* parent) : 
  base_class(type, name, parent),
  m_propertiesVector{"SCTSiliconPropertiesVector"}
{
  declareProperty("DetectorName", m_detectorName="SCT", "Dectector name: Pixel or SCT");
  declareProperty("ReadKey", m_propertiesVector, "Key of SiliconPropertiesVector");
}

StatusCode 
SiPropertiesTool::initialize()
{ 
  ATH_MSG_INFO("SiPropertiesTool Initialized");
  
  if ((m_detectorName != "Pixel") and (m_detectorName != "SCT")) {
    ATH_MSG_FATAL("Invalid detector name: " << m_detectorName << ". Must be Pixel or SCT.");
    return StatusCode::FAILURE;
  }
  ATH_CHECK(m_propertiesVector.initialize());

  return StatusCode::SUCCESS;
}

StatusCode 
SiPropertiesTool::finalize()
{
  return StatusCode::SUCCESS;
}

const InDet::SiliconProperties&
SiPropertiesTool::getSiProperties(const IdentifierHash& elementHash) const {
  SG::ReadCondHandle<InDet::SiliconPropertiesVector> handle{m_propertiesVector};
  if (handle.isValid()) {
    const InDet::SiliconPropertiesVector* vector{*handle};
    if (vector) {
      return vector->getSiProperties(elementHash);
    }
  }

  ATH_MSG_WARNING("SG::ReadCondHandle<InDet::SiliconPropertiesVector> is not valid. " << 
                  "Return default InDet::SiliconProperties for IdentifierHash for " <<
                  elementHash);
  return s_defaultProperties;
}

const InDet::SiliconProperties SiPropertiesTool::s_defaultProperties{};
