/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonTGC_Cabling/TGCModuleSLB.h"

namespace MuonTGC_Cabling
{
 
// Constructor
TGCModuleSLB::TGCModuleSLB(TGCIdBase::SideType vside,
			   TGCIdBase::ModuleType vmodule,
			   TGCIdBase::RegionType vregion,
			   int vsector,
			   int vid,
			   int vsbLoc,
			   int vslbAddr)
  : TGCModuleId(TGCModuleId::SLB)
{
  setSideType(vside);
  setModuleType(vmodule);
  setRegionType(vregion);
  setSector(vsector);
  setId(vid);
  m_sbLoc = vsbLoc;
  m_slbAddr = vslbAddr;
}
  
bool TGCModuleSLB::isValid(void) const
{
  if((getSideType()  >TGCIdBase::NoSideType)         &&
     (getSideType()  <TGCIdBase::MaxSideType)        &&
     (getModuleType()>TGCIdBase::NoModuleType)       &&
     (getModuleType()<TGCIdBase::MaxModuleType + 1 ) && // add SLB SL
     (getRegionType()>TGCIdBase::NoRegionType)       &&
     (getRegionType()<TGCIdBase::MaxRegionType)      &&
     (getOctant()    >=0)                            &&
     (getOctant()    <8)                             &&
     (getId()        >=0)                            )
    return true;
  return false;
}

} // end of namespace
