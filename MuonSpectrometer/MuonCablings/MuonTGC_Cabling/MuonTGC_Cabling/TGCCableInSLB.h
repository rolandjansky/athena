/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONTGC_CABLING_TGCCABLEINSLB_HH
#define MUONTGC_CABLING_TGCCABLEINSLB_HH
 
#include "MuonTGC_Cabling/TGCCable.h"

namespace MuonTGC_Cabling
{
  
class TGCCableInSLB : public TGCCable
{
public:
  // Constructor & Destructor
  TGCCableInSLB(void)
    : TGCCable(TGCCable::InSLB) {}

  virtual ~TGCCableInSLB(void) {}

   
  virtual TGCChannelId* getChannel(const TGCChannelId* channelId, 
				   bool orChannel=false) const;

private:
  virtual TGCChannelId* getChannelIn(const TGCChannelId* slbout, 
				     bool orChannel=false) const;
  virtual TGCChannelId* getChannelOut(const TGCChannelId* slbin, 
				      bool orChannel=false) const;

};
  
} // end of namespace
 
#endif
