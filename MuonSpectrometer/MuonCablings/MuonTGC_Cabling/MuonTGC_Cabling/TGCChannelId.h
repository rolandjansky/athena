/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONTGC_CABLING_TGCCHANNELID_HH
#define MUONTGC_CABLING_TGCCHANNELID_HH
 
#include "MuonTGC_Cabling/TGCId.h"

namespace MuonTGC_Cabling
{
class TGCModuleId;
  
class TGCChannelId : public TGCId
{
public:
  // Constructor & Destructor
  TGCChannelId(ChannelIdType type=NoChannelIdType)
    : TGCId(TGCIdBase::Channel)
  {
    m_channelType = type;
  }
  virtual ~TGCChannelId(void) {}
  
  virtual TGCModuleId* getModule(void) const { return 0; }
  
  virtual bool operator ==(const TGCChannelId& channelId) const;

  virtual bool isValid(void) const { return true; }
  
};

} // end of namespace
 
#endif
