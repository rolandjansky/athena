/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "TileG4DetDescr/DetectorDescription.h"
#include "TileG4DetDescr/DetectorDescriptionStore.h"

namespace FADS {

  DetectorDescription::DetectorDescription(std::string n)
      : m_name(n) {
    DetectorDescriptionStore& dd = DetectorDescriptionStore::GetDetectorDescriptionStore();
    dd.AddDetectorDescription(this);
    m_isPointed = false;
  }

  DetectorDescription::DetectorDescription(const DetectorDescription& d) {
    DetectorDescriptionStore& dd = DetectorDescriptionStore::GetDetectorDescriptionStore();
    dd.ReplaceDetectorDescription(d.GetName(), this);
    m_isPointed = false;
  }

  std::string DetectorDescription::GetName() const {
    return m_name;
  }

}	// end namespace
