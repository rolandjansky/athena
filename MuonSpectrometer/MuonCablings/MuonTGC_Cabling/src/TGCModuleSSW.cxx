/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonTGC_Cabling/TGCModuleSSW.h"

namespace MuonTGC_Cabling
{
 
// Constructor
TGCModuleSSW::TGCModuleSSW(TGCIdBase::SideType vside,
			   int vreadoutSector,
			   int vid)
  : TGCModuleId(TGCModuleId::SSW)
{
  setSideType(vside);
  setReadoutSector(vreadoutSector);
  setId(vid);
}
  
bool TGCModuleSSW::isValid(void) const
{
  if((getSideType()      >TGCIdBase::NoSideType)    &&
     (getSideType()      <TGCIdBase::MaxSideType)   &&
     (getReadoutSector() >=0)                       &&
     (getReadoutSector() < NumberOfReadoutSector )  &&
     (getId()            >=0)                       )
    return true;
  return false;
}

} // end of namespace
