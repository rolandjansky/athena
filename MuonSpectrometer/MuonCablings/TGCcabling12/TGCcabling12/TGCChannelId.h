/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TGCCHANNELID_HH
#define TGCCHANNELID_HH
 
#include "TGCcabling12/TGCId.h"

namespace LVL1TGCCabling12
{
class TGCModuleId;
  
class TGCChannelId : public TGCId
{
public:
  // Constructor & Destructor
  TGCChannelId (ChannelIdType type=NoChannelIdType)
    : TGCId(TGCIdBase::Channel)
  {
    channelType = type;
  }
  virtual ~TGCChannelId (void) {}
  
  virtual TGCModuleId* getModule (void) const { return 0; }
  
  virtual bool operator ==(const TGCChannelId& channelId) const;

  virtual bool isValid (void) const { return true; }

  
};

} // end of namespace
 
#endif
