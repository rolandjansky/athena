/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TGCCABLEINPP_HH
#define TGCCABLEINPP_HH
 
#include <string>
#include "TGCcabling12/TGCCable.h"
#include "TGCcabling12/TGCId.h" 
#include "TGCcabling12/TGCDatabaseInPP.h" 
#include "TGCcabling12/TGCChannelPPIn.h"
#include "TGCcabling12/TGCChannelPPOut.h"

namespace LVL1TGCCabling12
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
  TGCDatabase* database[TGCIdBase::MaxRegionType][TGCIdBase::MaxModuleType];
};
  
} // end of namespace
 
#endif
