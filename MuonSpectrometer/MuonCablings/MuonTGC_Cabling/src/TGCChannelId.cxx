/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonTGC_Cabling/TGCChannelId.h"

namespace MuonTGC_Cabling {

bool TGCChannelId::operator ==(const TGCChannelId& channelId) const
{
  if((this->getChannelIdType()==channelId.getChannelIdType())&&
     (this->getSideType()     ==channelId.getSideType())     &&
     (this->getRegionType()   ==channelId.getRegionType())   &&
     (this->getSignalType()   ==channelId.getSignalType())   &&
     (this->getModuleType()   ==channelId.getModuleType())   &&
     (this->getSector()       ==channelId.getSector())       &&
     (this->getLayer()        ==channelId.getLayer())        &&
     (this->getChamber()      ==channelId.getChamber())      &&
     (this->getId()           ==channelId.getId())           &&
     (this->getBlock()        ==channelId.getBlock())        &&
     (this->getChannel()      ==channelId.getChannel())      )
    return true;
  return false;
}

} //end of namespace
