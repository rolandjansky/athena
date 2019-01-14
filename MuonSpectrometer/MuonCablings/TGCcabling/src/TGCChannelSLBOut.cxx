/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "TGCcabling/TGCChannelSLBOut.h"

namespace LVL1TGCCabling8
{
 
// Constructor
TGCChannelSLBOut::TGCChannelSLBOut (TGCIdBase::SideType side,
				    TGCIdBase::ModuleType module,
				    TGCIdBase::RegionType region,
				    int sector,
				    int id,
				    int block,
				    int channel)
  : TGCChannelId(TGCChannelId::SLBOut)
{
  setSideType(side);
  setModuleType(module);
  setRegionType(region);
  setSector(sector);
  setId(id);
  setBlock(block);
  setChannel(channel);
}

TGCModuleId* TGCChannelSLBOut::getModule (void) const 
{
  return (new TGCModuleSLB(getSideType(),
			   getModuleType(),
			   getRegionType(),
			   getSector(),
			   getId()));
}
 

bool TGCChannelSLBOut::isValid (void) const
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


const int TGCChannelSLBOut::s_numberOfBlockInWD   = 2;
const int TGCChannelSLBOut::s_numberOfBlockInSD   = 2;
const int TGCChannelSLBOut::s_numberOfBlockInWT   = 3;
const int TGCChannelSLBOut::s_numberOfBlockInST   = 8;
const int TGCChannelSLBOut::s_numberOfLayerInWD   = 2;
const int TGCChannelSLBOut::s_numberOfLayerInSD   = 2;
const int TGCChannelSLBOut::s_numberOfLayerInWT   = 3;
const int TGCChannelSLBOut::s_numberOfLayerInST   = 2;
const int TGCChannelSLBOut::s_channelInBlockForWD = 32;
const int TGCChannelSLBOut::s_channelInBlockForSD = 32;
const int TGCChannelSLBOut::s_channelInBlockForWT = 32;
const int TGCChannelSLBOut::s_channelInBlockForST = 16;

int TGCChannelSLBOut::getNumberOfBlock (TGCIdBase::ModuleType moduleType) {
  switch(moduleType){
  case TGCIdBase::WD:
    return s_numberOfBlockInWD;
  case TGCIdBase::SD:
    return s_numberOfBlockInSD;
  case TGCIdBase::WT:
    return s_numberOfBlockInWT;
  case TGCIdBase::ST:
    return s_numberOfBlockInST;
  default:
    break;
  }
  return -1;
}

int TGCChannelSLBOut::getNumberOfLayer (TGCIdBase::ModuleType moduleType) {
  switch(moduleType){
  case TGCIdBase::WD:
    return s_numberOfLayerInWD;
  case TGCIdBase::SD:
    return s_numberOfLayerInSD;
  case TGCIdBase::WT:
    return s_numberOfLayerInWT;
  case TGCIdBase::ST:
    return s_numberOfLayerInST;
  default:
    break;
  }
  return -1;
}

int TGCChannelSLBOut::getChannelInBlock (TGCIdBase::ModuleType moduleType) {
  switch(moduleType){
  case TGCIdBase::WD:
    return s_channelInBlockForWD;
  case TGCIdBase::SD:
    return s_channelInBlockForSD;
  case TGCIdBase::WT:
    return s_channelInBlockForWT;
  case TGCIdBase::ST:
    return s_channelInBlockForST;
  default:
    break;
  }
  return -1;
}
 
} // end of namespace
