/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TGCcabling12/TGCModulePP.h"

namespace LVL1TGCCabling12
{
 
// Constructor
TGCModulePP::TGCModulePP (TGCIdBase::SideType vside,
			  TGCIdBase::ModuleType vmodule,
			  TGCIdBase::RegionType vregion,
			  int vsector,
			  int vid)
  : TGCModuleId(TGCModuleId::PP)
{
  setSideType(vside);
  setModuleType(vmodule);
  setRegionType(vregion);
  setSector(vsector);
  setId(vid);
}
  
bool TGCModulePP::isValid (void) const
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
