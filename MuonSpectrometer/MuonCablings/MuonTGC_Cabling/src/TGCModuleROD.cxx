/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonTGC_Cabling/TGCModuleROD.h"

namespace MuonTGC_Cabling
{
 
// Constructor
TGCModuleROD::TGCModuleROD(TGCIdBase::SideType vside,
			   int vreadoutSector)
  : TGCModuleId(TGCModuleId::ROD)
{
  setSideType(vside);
  setReadoutSector(vreadoutSector);
  int rodId = vreadoutSector+1;
  setId(rodId);
}
  
bool TGCModuleROD::isValid(void) const
{
  if((getSideType()  >TGCIdBase::NoSideType)      &&
     (getSideType()  <TGCIdBase::MaxSideType)     &&
     (getReadoutSector() >=0)                     &&
     (getReadoutSector() < NumberOfReadoutSector ) )
    return true;
  return false;
}

} // end of namespace
