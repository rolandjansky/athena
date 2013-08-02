/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonTGC_Cabling/TGCModuleHPB.h"

namespace MuonTGC_Cabling
{
 
// Constructor
TGCModuleHPB::TGCModuleHPB(TGCIdBase::SideType side,
			   TGCIdBase::SignalType signal,
			   TGCIdBase::RegionType region,
			   int sector,
			   int id)
  : TGCModuleId(TGCModuleId::HPB)
{
  setSideType(side);
  setSignalType(signal);
  setRegionType(region);
  setSector(sector);
  setId(id);
}
  
bool TGCModuleHPB::isValid(void) const
{
  if((getSideType()  >TGCIdBase::NoSideType)   &&
     (getSideType()  <TGCIdBase::MaxSideType)  &&
     (getSignalType()>TGCIdBase::NoSignalType) &&
     (getSignalType()<TGCIdBase::MaxSignalType)&&
     (getRegionType()>TGCIdBase::NoRegionType) &&
     (getRegionType()<TGCIdBase::MaxRegionType)&&
     (getOctant()    >=0)                      &&
     (getOctant()    <8)                       &&
     (getId()        >=0)                      )
    return true;
  return false;
}

} // end of namespace
