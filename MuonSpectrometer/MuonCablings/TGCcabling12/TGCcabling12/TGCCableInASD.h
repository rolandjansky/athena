/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TGCCABLEINASD_HH
#define TGCCABLEINASD_HH
 
#include <string>
#include "TGCcabling12/TGCCable.h"
#include "TGCcabling12/TGCId.h"
#include "TGCcabling12/TGCDatabaseASDToPP.h" 
#include "TGCcabling12/TGCChannelASDIn.h"
#include "TGCcabling12/TGCChannelASDOut.h"


namespace LVL1TGCCabling12
{
  
class TGCCableInASD : public TGCCable
{
public:
  // Constructor & Destructor
  TGCCableInASD (std::string filename);
  
  virtual ~TGCCableInASD (void);

  virtual TGCChannelId* getChannel (const TGCChannelId* channelId, 
				    bool orChannel=false) const;

private:
  TGCCableInASD (void) {}
  virtual TGCChannelId* getChannelIn (const TGCChannelId* asdout, 
				      bool orChannel=false) const;
  virtual TGCChannelId* getChannelOut (const TGCChannelId* asdin,
				       bool orChannel=false) const;
  TGCDatabase* database[TGCIdBase::MaxRegionType][TGCIdBase::MaxModuleType];

};
  
} // end of namespace
 
#endif
