/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TGCcabling/TGCModuleSL.h"

namespace LVL1TGCCabling8
{
 
// Constructor
TGCModuleSL::TGCModuleSL (TGCIdBase::SideType side,
			  TGCIdBase::RegionType region,
			  int sector)
  : TGCModuleId(TGCModuleId::SL)
{
  setSideType(side);
  setRegionType(region);
  setSector(sector);
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
