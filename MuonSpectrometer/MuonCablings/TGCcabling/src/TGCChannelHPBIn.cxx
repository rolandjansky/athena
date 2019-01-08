/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
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

const int TGCChannelHPBIn::s_numberOfBlock  = 2;
const int TGCChannelHPBIn::s_channelInBlock = 12;
const int TGCChannelHPBIn::s_slbInBlock     = 3;

int TGCChannelHPBIn::getNumberOfBlock (void) {
  return s_numberOfBlock;
}

int TGCChannelHPBIn::getChannelInBlock (void) {
  return s_channelInBlock;
}

int TGCChannelHPBIn::getSlbInBlock (void) {
  return s_slbInBlock;
}


} // end of namespace
