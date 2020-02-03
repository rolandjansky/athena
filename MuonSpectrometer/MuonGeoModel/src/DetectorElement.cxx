/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonGeoModel/DetectorElement.h"

namespace MuonGM {

  const AbsMaterialManager * DetectorElement::s_matManager = nullptr;
  std::mutex DetectorElement::s_mutex;

} // namespace MuonGM
