/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "ReadoutGeometryBase/SiCommonItems.h"

namespace InDetDD {

SiCommonItems::SiCommonItems(const AtlasDetectorID* const idHelper)
  :  m_msg("SiDetectorElement"),
     m_idHelper(idHelper), 
     m_lorentzAngleTool(nullptr)
{}


void SiCommonItems::setLorentzAngleTool(const ISiLorentzAngleTool* lorentzAngleTool) {
  m_lorentzAngleTool = lorentzAngleTool;
}

const ISiLorentzAngleTool* SiCommonItems::lorentzAngleTool() const {
  return m_lorentzAngleTool;
}

} // End namespace InDetDD
