/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TGCcabling/TGCChannelASDIn.h"

namespace LVL1TGCCabling8
{
 
// Constructor
TGCChannelASDIn::TGCChannelASDIn (TGCIdBase::SideType side,
				  TGCIdBase::SignalType signal,
				  TGCIdBase::RegionType region,
				  int sector,
				  int layer,
				  int chamber,
				  int channel)
  : TGCChannelId(TGCChannelId::ASDIn)
{
  setSideType(side);
  setSignalType(signal);
  setRegionType(region);
  setLayer(layer);
  setSector(sector);
  setChamber(chamber);
  setChannel(channel);
}

void TGCChannelASDIn::setSector (int sector) 
{
  if(isEndcap()&&!isInner()){
    TGCChannelId::setSector((sector+1)%48);
  } else {
    TGCChannelId::setSector(sector%24);
  }
}

int TGCChannelASDIn::getSector (void) const
{
  int sector;
  if(isEndcap()&&!isInner()){
    sector = TGCChannelId::getSector()-1;
    if(sector<=0) sector += 48;
  } else {
    sector = TGCChannelId::getSector();
    if(sector<=0) sector += 24;
  }

  return sector;
}

bool TGCChannelASDIn::isValid (void) const
{
  if((getSideType()  >TGCIdBase::NoSideType)   &&
     (getSideType()  <TGCIdBase::MaxSideType)  &&
     (getSignalType()>TGCIdBase::NoSignalType) &&
     (getSignalType()<TGCIdBase::MaxSignalType)&&
     (getRegionType()>TGCIdBase::NoRegionType) &&
     (getRegionType()<TGCIdBase::MaxRegionType)&&
     (getOctant()    >=0)                  &&
     (getOctant()    <8)                   &&
     (getLayer()     >=0)                  &&
     (getChamber()   >=0)                  &&
     (getChannel()   >=0)                  )
    return true;
  return false;
}

} // end of namespace
