/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TGCCHANNELID_HH
#define TGCCHANNELID_HH
 
#include "TGCcabling/TGCId.h"

namespace LVL1TGCCabling8
{
class TGCModuleId;
  
class TGCChannelId : public TGCId
{
public:
  // Constructor & Destructor
  TGCChannelId (TGCIdBase::ChannelIdType type=NoChannelIdType)
    : TGCId(TGCIdBase::Channel)
  {
    m_channelType = type;
  }
  virtual ~TGCChannelId (void) {}
  
  virtual TGCModuleId* getModule (void) const { return 0; }
  
  virtual bool operator ==(const TGCChannelId& channelId) const;

  virtual bool isValid (void) const { return true; }

  
};

} // end of namespace
 
#endif
