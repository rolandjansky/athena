/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TGCcabling/TGCModuleSLB.h"

namespace LVL1TGCCabling8
{
 
// Constructor
TGCModuleSLB::TGCModuleSLB (TGCIdBase::SideType side,
			    TGCIdBase::ModuleType module,
			    TGCIdBase::RegionType region,
			    int sector,
			    int id)
  : TGCModuleId(TGCModuleId::SLB)
{
  setSideType(side);
  setModuleType(module);
  setRegionType(region);
  setSector(sector);
  setId(id);
}
  
bool TGCModuleSLB::isValid (void) const
{
  if((getSideType()  >TGCIdBase::NoSideType)   &&
     (getSideType()  <TGCIdBase::MaxSideType)  &&
     (getModuleType()>TGCIdBase::NoModuleType) &&
     (getModuleType()<TGCIdBase::MaxModuleType)&&
     (getRegionType()>TGCIdBase::NoRegionType) &&
     (getRegionType()<TGCIdBase::MaxRegionType)&&
     (getOctant()    >=0)                  &&
     (getOctant()    <8)                   &&
     (getId()        >=0)                  )
    return true;
  return false;
}

} // end of namespace
