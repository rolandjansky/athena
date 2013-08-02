/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TGCcabling12/TGCModuleROD.h"

namespace LVL1TGCCabling12
{
 
// Constructor
TGCModuleROD::TGCModuleROD (TGCIdBase::SideType vside,
			    int vreadoutSector)
  : TGCModuleId(TGCModuleId::ROD)
{
  setSideType(vside);
  setReadoutSector(vreadoutSector);
  int rodId = vreadoutSector+1;
  setId(rodId);
}
  
bool TGCModuleROD::isValid (void) const
{
  if((getSideType()  >TGCIdBase::NoSideType)      &&
     (getSideType()  <TGCIdBase::MaxSideType)     &&
     (getReadoutSector() >=0)                 &&
     (getReadoutSector() < NumberOfReadoutSector ) )
    return true;
  return false;
}

} // end of namespace
