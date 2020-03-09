/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonGeoModel/TgcComponent.h"
#include "MuonGeoModel/Technology.h"
#include "MuonGeoModel/Tgc.h"
#include "MuonGeoModel/TGC_Technology.h"

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
