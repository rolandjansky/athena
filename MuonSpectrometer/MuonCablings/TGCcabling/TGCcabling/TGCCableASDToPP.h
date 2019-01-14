/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TGCCABLEASDTOPP_HH
#define TGCCABLEASDTOPP_HH
 
#include <string>
#include "TGCcabling/TGCCable.h"
#include "TGCcabling/TGCId.h"
#include "TGCcabling/TGCDatabaseASDToPP.h" 
#include "TGCcabling/TGCChannelASDOut.h"
#include "TGCcabling/TGCChannelPPIn.h"

namespace LVL1TGCCabling8
{
  
class TGCCableASDToPP : public TGCCable
{
public:
  // Constructor & Destructor
  TGCCableASDToPP (std::string filename);
  
  virtual ~TGCCableASDToPP (void);

  virtual TGCChannelId* getChannel (const TGCChannelId* channelId, 
				    bool orChannel=false) const;

private:
  TGCCableASDToPP (void) {}
  virtual TGCChannelId* getChannelIn (const TGCChannelId* ppin, 
				      bool orChannel=false) const;
  virtual TGCChannelId* getChannelOut (const TGCChannelId* asdout,
				       bool orChannel=false) const;
  TGCDatabase* m_database[TGCIdBase::MaxRegionType][TGCIdBase::MaxModuleType];

  static const int s_stripForward[];
};
  
} // end of namespace
 
#endif
