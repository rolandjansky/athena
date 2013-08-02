/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TGCCHANNELASDOUT_HH
#define TGCCHANNELASDOUT_HH
 
#include "TGCcabling/TGCChannelId.h"
 
namespace LVL1TGCCabling8
{
  
class TGCChannelASDOut : public TGCChannelId
{
public:
  // Constructor & Destructor
  TGCChannelASDOut (TGCIdBase::SideType side,
		    TGCIdBase::SignalType signal,
		    TGCIdBase::RegionType region,
		    int sector,
		    int layer,
		    int chamber,
		    int channel);

  TGCChannelASDOut (TGCIdBase::SideType side,
		    TGCIdBase::SignalType signal,
		    int octant,
		    int moduleSector,
		    int layer,
		    int chamber,
		    int channel);

  virtual ~TGCChannelASDOut (void) {}

  virtual bool isValid (void) const;
  
private:
  TGCChannelASDOut (void) {}
};
  
} // end of namespace
 
#endif
