/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONTGC_CABLING_TGCCABLEINPP_HH
#define MUONTGC_CABLING_TGCCABLEINPP_HH
 
#include "MuonTGC_Cabling/TGCCable.h"

#include <string>

#include "TGCcablingInterface/TGCIdBase.h"

namespace MuonTGC_Cabling
{

class TGCDatabase;
  
class TGCCableInPP : public TGCCable
{
public:
  // Constructor & Destructor
  TGCCableInPP(std::string filename);

  virtual ~TGCCableInPP(void);
  
  virtual TGCChannelId* getChannel(const TGCChannelId* channelId,
				   bool orChannel=false) const;

private:
  TGCCableInPP(void) {}
  virtual TGCChannelId* getChannelIn(const TGCChannelId* ppout, 
				     bool orChannel=false) const;
  virtual TGCChannelId* getChannelOut(const TGCChannelId* ppin,
				      bool orChannel=false) const;
  TGCDatabase* m_database[TGCIdBase::MaxRegionType][TGCIdBase::MaxModuleType];
};
  
} // end of namespace
 
#endif
