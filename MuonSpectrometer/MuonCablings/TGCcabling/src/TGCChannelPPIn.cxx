/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TGCcabling/TGCChannelPPIn.h"

namespace LVL1TGCCabling8
{
 
// Constructor
TGCChannelPPIn::TGCChannelPPIn (TGCIdBase::SideType side,
				TGCIdBase::ModuleType module,
				TGCIdBase::RegionType region,
				int sector,
				int id,
				int block,
				int channel)
  : TGCChannelId(TGCChannelId::PPIn)
{
  setSideType(side);
  setModuleType(module);
  setRegionType(region);
  setSector(sector);
  setId(id);
  setBlock(block);
  setChannel(channel);
}
  
TGCModuleId* TGCChannelPPIn::getModule (void) const 
{
  return (new TGCModulePP(getSideType(),
			  getModuleType(),
			  getRegionType(),
			  getSector(),
			  getId()));
}

bool TGCChannelPPIn::isValid (void) const
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
