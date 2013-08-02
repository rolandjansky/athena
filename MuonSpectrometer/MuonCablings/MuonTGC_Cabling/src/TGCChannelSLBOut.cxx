/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonTGC_Cabling/TGCChannelSLBOut.h"

#include "MuonTGC_Cabling/TGCModuleSLB.h"

namespace MuonTGC_Cabling
{
 
// Constructor
TGCChannelSLBOut::TGCChannelSLBOut(TGCIdBase::SideType vside,
				   TGCIdBase::ModuleType vmodule,
				   TGCIdBase::RegionType vregion,
				   int vsector,
				   int vid,
				   int vblock,
				   int vchannel)
  : TGCChannelId(TGCIdBase::SLBOut)
{
  setSideType(vside);
  setModuleType(vmodule);
  setRegionType(vregion);
  setSector(vsector);
  setId(vid);
  setBlock(vblock);
  setChannel(vchannel);
}

TGCModuleId* TGCChannelSLBOut::getModule(void) const 
{
  return (new TGCModuleSLB(getSideType(),
			   getModuleType(),
			   getRegionType(),
			   getSector(),
			   getId()));
}
 

bool TGCChannelSLBOut::isValid(void) const
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


const int TGCChannelSLBOut::numberOfBlockInWD   = 2;
const int TGCChannelSLBOut::numberOfBlockInSD   = 2;
const int TGCChannelSLBOut::numberOfBlockInWT   = 3;
const int TGCChannelSLBOut::numberOfBlockInST   = 8;
const int TGCChannelSLBOut::numberOfLayerInWD   = 2;
const int TGCChannelSLBOut::numberOfLayerInSD   = 2;
const int TGCChannelSLBOut::numberOfLayerInWT   = 3;
const int TGCChannelSLBOut::numberOfLayerInST   = 2;
const int TGCChannelSLBOut::channelInBlockForWD = 32;
const int TGCChannelSLBOut::channelInBlockForSD = 32;
const int TGCChannelSLBOut::channelInBlockForWT = 32;
const int TGCChannelSLBOut::channelInBlockForST = 16;

int TGCChannelSLBOut::getNumberOfBlock(TGCIdBase::ModuleType moduleType) {
  switch(moduleType){
  case TGCIdBase::WD:
    return numberOfBlockInWD;
  case TGCIdBase::SD:
    return numberOfBlockInSD;
  case TGCIdBase::WT:
    return numberOfBlockInWT;
  case TGCIdBase::ST:
    return numberOfBlockInST;
  default:
    break;
  }
  return -1;
}

int TGCChannelSLBOut::getNumberOfLayer(TGCIdBase::ModuleType moduleType) {
  switch(moduleType){
  case TGCIdBase::WD:
    return numberOfLayerInWD;
  case TGCIdBase::SD:
    return numberOfLayerInSD;
  case TGCIdBase::WT:
    return numberOfLayerInWT;
  case TGCIdBase::ST:
    return numberOfLayerInST;
  default:
    break;
  }
  return -1;
}

int TGCChannelSLBOut::getChannelInBlock(TGCIdBase::ModuleType moduleType) {
  switch(moduleType){
  case TGCIdBase::WD:
    return channelInBlockForWD;
  case TGCIdBase::SD:
    return channelInBlockForSD;
  case TGCIdBase::WT:
    return channelInBlockForWT;
  case TGCIdBase::ST:
    return channelInBlockForST;
  default:
    break;
  }
  return -1;
}
 
} // end of namespace
