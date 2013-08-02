/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TGCCHANNELASDIN_HH
#define TGCCHANNELASDIN_HH
 
#include "TGCcabling/TGCChannelId.h"
 
namespace LVL1TGCCabling8
{
  
class TGCChannelASDIn : public TGCChannelId
{
public:
  // Constructor & Destructor
  TGCChannelASDIn (TGCIdBase::SideType side,
		   TGCIdBase::SignalType signal,
		   TGCIdBase::RegionType region,
		   int sector,
		   int layer,
		   int chamber,
		   int channel);

  virtual ~TGCChannelASDIn (void) {}

  virtual void setSector (int sector);
  
  virtual int getSector (void) const;

  virtual bool isValid (void) const;
  
private:
  TGCChannelASDIn (void) {}
};
  
} // end of namespace
 
#endif
