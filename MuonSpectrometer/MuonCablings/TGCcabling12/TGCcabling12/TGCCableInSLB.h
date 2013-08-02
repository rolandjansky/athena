/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TGCCABLEINSLB_HH
#define TGCCABLEINSLB_HH
 
#include "TGCcabling12/TGCCable.h"
#include "TGCcabling12/TGCId.h" 
#include "TGCcabling12/TGCChannelSLBIn.h"
#include "TGCcabling12/TGCChannelSLBOut.h"

namespace LVL1TGCCabling12
{
  
class TGCCableInSLB : public TGCCable
{
public:
  // Constructor & Destructor
  TGCCableInSLB (void)
    : TGCCable(TGCCable::InSLB) {}

  virtual ~TGCCableInSLB (void) {}

   
  virtual TGCChannelId* getChannel (const TGCChannelId* channelId, 
				    bool orChannel=false) const;

private:
  virtual TGCChannelId* getChannelIn (const TGCChannelId* slbout, 
				      bool orChannel=false) const;
  virtual TGCChannelId* getChannelOut (const TGCChannelId* slbin, 
				       bool orChannel=false) const;

};
  
} // end of namespace
 
#endif
