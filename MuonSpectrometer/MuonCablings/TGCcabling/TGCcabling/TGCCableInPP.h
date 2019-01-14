/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TGCCABLEINPP_HH
#define TGCCABLEINPP_HH
 
#include <string>
#include "TGCcabling/TGCCable.h"
#include "TGCcabling/TGCId.h" 
#include "TGCcabling/TGCDatabaseInPP.h" 
#include "TGCcabling/TGCChannelPPIn.h"
#include "TGCcabling/TGCChannelPPOut.h"

namespace LVL1TGCCabling8
{
  
class TGCCableInPP : public TGCCable
{
public:
  // Constructor & Destructor
  TGCCableInPP (std::string filename);

  virtual ~TGCCableInPP (void);
  
  virtual TGCChannelId* getChannel (const TGCChannelId* channelId,
				    bool orChannel=false) const;

private:
  TGCCableInPP (void) {}
  virtual TGCChannelId* getChannelIn (const TGCChannelId* ppout, 
				      bool orChannel=false) const;
  virtual TGCChannelId* getChannelOut (const TGCChannelId* ppin,
				       bool orChannel=false) const;
  TGCDatabase* m_database[TGCIdBase::MaxRegionType][TGCIdBase::MaxModuleType];
};
  
} // end of namespace
 
#endif
