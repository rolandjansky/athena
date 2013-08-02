/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonTGC_Cabling/TGCChannelASDOut.h"

namespace MuonTGC_Cabling
{
 
// Constructor
TGCChannelASDOut::TGCChannelASDOut(TGCIdBase::SideType vside,
				   TGCIdBase::SignalType vsignal,
				   TGCIdBase::RegionType vregion,
				   int vsector,
				   int vlayer,
				   int vchamber,
				   int vchannel)
  : TGCChannelId(TGCIdBase::ASDOut)
{
  setSideType(vside);
  setSignalType(vsignal);
  setRegionType(vregion);
  setSector(vsector);
  setLayer(vlayer);
  setChamber(vchamber);
  setChannel(vchannel);
}

TGCChannelASDOut::TGCChannelASDOut(TGCIdBase::SideType vside,
				   TGCIdBase::SignalType vsignal,
				   int voctant,
				   int vsectorModule,
				   int vlayer,
				   int vchamber,
				   int vchannel)
  : TGCChannelId(TGCIdBase::ASDOut)
{
  setSideType(vside);
  setSignalType(vsignal);
  setOctant(voctant);
  setSectorModule(vsectorModule);// after setOctant() method
  setLayer(vlayer);
  setChamber(vchamber);
  setChannel(vchannel);
}

bool TGCChannelASDOut::isValid(void) const
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
