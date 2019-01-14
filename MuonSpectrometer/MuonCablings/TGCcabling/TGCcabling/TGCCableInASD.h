/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TGCCABLEINASD_HH
#define TGCCABLEINASD_HH
 
#include <string>
#include "TGCcabling/TGCCable.h"
#include "TGCcabling/TGCId.h"
#include "TGCcabling/TGCDatabaseASDToPP.h" 
#include "TGCcabling/TGCChannelASDIn.h"
#include "TGCcabling/TGCChannelASDOut.h"


namespace LVL1TGCCabling8
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
  TGCDatabase* m_database[TGCIdBase::MaxRegionType][TGCIdBase::MaxModuleType];

};
  
} // end of namespace
 
#endif
