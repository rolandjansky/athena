/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonGeoModel/TgcComponent.h"

#include <string>

namespace MuonGM {

TgcComponent:: TgcComponent():StandardComponent()
{
}


std::ostream& operator<<(std::ostream& os,const TgcComponent& c)
{
  os << "Component " << c.name << std::endl;
  return os;
}

} // namespace MuonGM
