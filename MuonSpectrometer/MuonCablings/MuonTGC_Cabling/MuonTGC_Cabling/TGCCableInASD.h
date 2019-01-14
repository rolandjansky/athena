/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONTGC_CABLING_TGCCABLEINASD_HH
#define MUONTGC_CABLING_TGCCABLEINASD_HH
 
#include "MuonTGC_Cabling/TGCCable.h"

#include <string>

#include "MuonTGC_Cabling/TGCId.h"

namespace MuonTGC_Cabling
{

class TGCDatabase;

class TGCCableInASD : public TGCCable
{
public:
  // Constructor & Destructor
  TGCCableInASD(std::string filename);
  
  virtual ~TGCCableInASD(void);

  virtual TGCChannelId* getChannel(const TGCChannelId* channelId, 
				   bool orChannel=false) const;

private:
  TGCCableInASD(void) {}
  virtual TGCChannelId* getChannelIn(const TGCChannelId* asdout, 
				     bool orChannel=false) const;
  virtual TGCChannelId* getChannelOut(const TGCChannelId* asdin,
				      bool orChannel=false) const;
  TGCDatabase* m_database[TGCIdBase::MaxRegionType][TGCIdBase::MaxModuleType];

};
  
} // end of namespace
 
#endif
