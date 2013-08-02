/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonTGC_Cabling/TGCChannelPPIn.h"

#include "MuonTGC_Cabling/TGCModulePP.h"

namespace MuonTGC_Cabling
{
 
// Constructor
TGCChannelPPIn::TGCChannelPPIn(TGCIdBase::SideType vside,
			       TGCIdBase::ModuleType vmodule,
			       TGCIdBase::RegionType vregion,
			       int vsector,
			       int vid,
			       int vblock,
			       int vchannel)
  : TGCChannelId(TGCIdBase::PPIn)
{
  setSideType(vside);
  setModuleType(vmodule);
  setRegionType(vregion);
  setSector(vsector);
  setId(vid);
  setBlock(vblock);
  setChannel(vchannel);
}
  
TGCModuleId* TGCChannelPPIn::getModule(void) const 
{
  return (new TGCModulePP(getSideType(),
			  getModuleType(),
			  getRegionType(),
			  getSector(),
			  getId()));
}

bool TGCChannelPPIn::isValid(void) const
{
  if((getSideType()  >TGCIdBase::NoSideType)   &&
     (getSideType()  <TGCIdBase::MaxSideType)  &&
     (getModuleType()>TGCIdBase::NoModuleType) &&
     (getModuleType()<TGCIdBase::MaxModuleType)&&
     (getRegionType()>TGCIdBase::NoRegionType) &&
     (getRegionType()<TGCIdBase::MaxRegionType)&&
     (getOctant()    >=0)                  &&
     (getOctant()    <8)                   &&
     (getId()        >=0)                  &&
     (getBlock()     >=0)                  &&
     (getChannel()   >=0)                  )
    return true;
  return false;
}


} // end of namespace
