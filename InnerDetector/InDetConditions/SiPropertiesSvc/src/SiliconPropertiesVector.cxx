/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
//   Source file for class SiliconPropertiesVector for SiPropertiesTool
///////////////////////////////////////////////////////////////////

#include "SiPropertiesSvc/SiliconPropertiesVector.h"

namespace InDet {
  
  SiliconPropertiesVector::SiliconPropertiesVector():
    m_data{},
    m_defaultProperties{}
  {
  }

  SiliconPropertiesVector::~SiliconPropertiesVector() {
  }

  void SiliconPropertiesVector::resize(IdentifierHash::value_type size) {
    m_data.resize(size);
  }

  const InDet::SiliconProperties& SiliconPropertiesVector::getSiProperties(const IdentifierHash& elementHash) const {
    if (m_data.size()<elementHash) return m_defaultProperties;
    return m_data[elementHash];
  }

  void SiliconPropertiesVector::setConditions(const IdentifierHash& elementHash, const double temperature, const double electricField) {
    if (m_data.size()<elementHash) m_data.resize(elementHash);
    m_data[elementHash].setConditions(temperature, electricField);
  }

} // namespace InDet
