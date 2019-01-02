/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONTGC_CABLING_TGCCABLEPPTOSLB_HH
#define MUONTGC_CABLING_TGCCABLEPPTOSLB_HH
 
#include "MuonTGC_Cabling/TGCCable.h"

#include <string>

#include "TGCcablingInterface/TGCIdBase.h"

namespace MuonTGC_Cabling
{

class TGCDatabase;
  
class TGCCablePPToSLB : public TGCCable
{
public:
  // Constructor & Destructor
  TGCCablePPToSLB(std::string filename);

  virtual ~TGCCablePPToSLB(void);
  
  virtual TGCChannelId* getChannel(const TGCChannelId* channelId,
				   bool orChannel=false) const;
  virtual TGCModuleMap* getModule(const TGCModuleId* moduleId) const;
  
private:
  TGCCablePPToSLB(void) {}
  virtual TGCChannelId* getChannelIn(const TGCChannelId* slbin, 
				     bool orChannel=false) const;
  virtual TGCChannelId* getChannelOut(const TGCChannelId* ppout,
				      bool orChannel=false) const;
  virtual TGCModuleMap* getModuleIn(const TGCModuleId* slb) const;
  virtual TGCModuleMap* getModuleOut(const TGCModuleId* pp) const;
  TGCDatabase* m_database[TGCIdBase::MaxRegionType][TGCIdBase::MaxModuleType];
};
  
} // end of namespace
 
#endif
