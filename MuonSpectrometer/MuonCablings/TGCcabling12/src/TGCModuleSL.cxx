/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TGCcabling12/TGCModuleSL.h"

namespace LVL1TGCCabling12
{
 
// Constructor
TGCModuleSL::TGCModuleSL (TGCIdBase::SideType vside,
			  TGCIdBase::RegionType vregion,
			  int vsector)
  : TGCModuleId(TGCModuleId::SL)
{
  setSideType(vside);
  setRegionType(vregion);
  setSector(vsector);
}
  
bool TGCModuleSL::isValid (void) const
{
  if((getSideType()  >TGCIdBase::NoSideType)   &&
     (getSideType()  <TGCIdBase::MaxSideType)  &&
     (getRegionType()>TGCIdBase::NoRegionType) &&
     (getRegionType()<TGCIdBase::MaxRegionType)&&
     (getOctant()    >=0)                  &&
     (getOctant()    <8)                   )
    return true;
  return false;
}

} // end of namespace
