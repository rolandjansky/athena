/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonTGC_Cabling/TGCChannelASDIn.h"

namespace MuonTGC_Cabling
{
 
// Constructor
TGCChannelASDIn::TGCChannelASDIn(TGCIdBase::SideType vside,
				 TGCIdBase::SignalType vsignal,
				 TGCIdBase::RegionType vregion,
				 int vsector,
				 int vlayer,
				 int vchamber,
				 int vchannel)
  : TGCChannelId(TGCIdBase::ASDIn)
{
  setSideType(vside);
  setSignalType(vsignal);
  setRegionType(vregion);
  setLayer(vlayer);
  TGCChannelASDIn::setSector(vsector);
  setChamber(vchamber);
  setChannel(vchannel);
}

void TGCChannelASDIn::setSector(int sector) 
{
  if(isEndcap()&&!isInner()){
    TGCId::setSector((sector+1)%TGCId::NumberOfEndcapSector);
  } else {
    TGCId::setSector(sector%TGCId::NumberOfForwardSector);
  }
}

int TGCChannelASDIn::getSector(void) const
{
  int sector;
  if(isEndcap()&&!isInner()){
    sector = TGCIdBase::getSector()-1;
    if(sector<=0) sector += TGCId::NumberOfEndcapSector;
  } else {
    sector = TGCIdBase::getSector();
    if(sector<=0) sector += TGCId::NumberOfForwardSector;
  }

  return sector;
}

bool TGCChannelASDIn::isValid(void) const
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
