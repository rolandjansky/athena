/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TGCcabling/TGCChannelASDOut.h"

namespace LVL1TGCCabling8
{
 
// Constructor
TGCChannelASDOut::TGCChannelASDOut (TGCIdBase::SideType side,
				    TGCIdBase::SignalType signal,
				    TGCIdBase::RegionType region,
				    int sector,
				    int layer,
				    int chamber,
				    int channel)
  : TGCChannelId(TGCChannelId::ASDOut)
{
  setSideType(side);
  setSignalType(signal);
  setRegionType(region);
  setSector(sector);
  setLayer(layer);
  setChamber(chamber);
  setChannel(channel);
}

TGCChannelASDOut::TGCChannelASDOut (TGCIdBase::SideType side,
				    TGCIdBase::SignalType signal,
				    int octant,
				    int sectorModule,
				    int layer,
				    int chamber,
				    int channel)
  : TGCChannelId(TGCChannelId::ASDOut)
{
  setSideType(side);
  setSignalType(signal);
  setOctant(octant);
  setSectorModule(sectorModule);// after setOctant() method
  setLayer(layer);
  setChamber(chamber);
  setChannel(channel);
}

bool TGCChannelASDOut::isValid (void) const
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
