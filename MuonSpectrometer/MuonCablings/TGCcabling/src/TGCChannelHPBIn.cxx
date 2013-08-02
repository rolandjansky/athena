/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TGCcabling/TGCChannelHPBIn.h"

namespace LVL1TGCCabling8
{
 
// Constructor
TGCChannelHPBIn::TGCChannelHPBIn (TGCIdBase::SideType side,
				  TGCIdBase::SignalType signal,
				  TGCIdBase::RegionType region,
				  int sector,
				  int id,
				  int block,
				  int channel)
  : TGCChannelId(TGCChannelId::HPBIn)
{
  setSideType(side);
  setSignalType(signal);
  setRegionType(region);
  setSector(sector);
  setId(id);
  setBlock(block);
  setChannel(channel);
}

TGCModuleId* TGCChannelHPBIn::getModule (void) const 
{
  return (new TGCModuleHPB(getSideType(),
			   getSignalType(),
			   getRegionType(),
			   getSector(),
			   getId()));
}


bool TGCChannelHPBIn::isValid (void) const
{
  if((getSideType()  >TGCIdBase::NoSideType)   &&
     (getSideType()  <TGCIdBase::MaxSideType)  &&
     (getSignalType()>TGCIdBase::NoSignalType) &&
     (getSignalType()<TGCIdBase::MaxSignalType)&&
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

const int TGCChannelHPBIn::numberOfBlock  = 2;
const int TGCChannelHPBIn::channelInBlock = 12;
const int TGCChannelHPBIn::slbInBlock     = 3;

int TGCChannelHPBIn::getNumberOfBlock (void) {
  return numberOfBlock;
}

int TGCChannelHPBIn::getChannelInBlock (void) {
  return channelInBlock;
}

int TGCChannelHPBIn::getSlbInBlock (void) {
  return slbInBlock;
}


} // end of namespace
