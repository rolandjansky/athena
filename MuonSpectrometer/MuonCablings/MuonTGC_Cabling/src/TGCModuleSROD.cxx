/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonTGC_Cabling/TGCModuleSROD.h"

namespace MuonTGC_Cabling
{
 
// Constructor
TGCModuleSROD::TGCModuleSROD(TGCIdBase::SideType vside,
			   int vreadoutSector)
  : TGCModuleId(TGCModuleId::SROD)
{
  setSideType(vside);
  setReadoutSector(vreadoutSector);
  int srodId = vreadoutSector+1;
  setId(srodId);
}
  
bool TGCModuleSROD::isValid(void) const
{
  return (getSideType()  >TGCIdBase::NoSideType) &&
    (getSideType()  <TGCIdBase::MaxSideType)     &&
    (getReadoutSector() >=0)                     &&
    (getReadoutSector() < NumberOfReadoutSector );
}

} // end of namespace
