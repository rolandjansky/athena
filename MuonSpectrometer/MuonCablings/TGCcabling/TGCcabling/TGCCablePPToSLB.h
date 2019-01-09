/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TGCCABLEPPTOSLB_HH
#define TGCCABLEPPTOSLB_HH
 
#include <string>
#include "TGCcabling/TGCCable.h"
#include "TGCcabling/TGCId.h"
#include "TGCcabling/TGCDatabasePPToSL.h" 
#include "TGCcabling/TGCChannelPPOut.h"
#include "TGCcabling/TGCChannelSLBIn.h"
#include "TGCcabling/TGCModulePP.h"
#include "TGCcabling/TGCModuleSLB.h"

namespace LVL1TGCCabling8
{
  
class TGCCablePPToSLB : public TGCCable
{
public:
  // Constructor & Destructor
  TGCCablePPToSLB (std::string filename);

  virtual ~TGCCablePPToSLB (void);
  
  virtual TGCChannelId* getChannel (const TGCChannelId* channelId,
				    bool orChannel=false) const;
  virtual TGCModuleMap* getModule (const TGCModuleId* moduleId) const;
  
private:
  TGCCablePPToSLB (void) {}
  virtual TGCChannelId* getChannelIn (const TGCChannelId* slbin, 
				      bool orChannel=false) const;
  virtual TGCChannelId* getChannelOut (const TGCChannelId* ppout,
				       bool orChannel=false) const;
  virtual TGCModuleMap* getModuleIn (const TGCModuleId* slb) const;
  virtual TGCModuleMap* getModuleOut (const TGCModuleId* pp) const;
  TGCDatabase* m_database[TGCIdBase::MaxRegionType][TGCIdBase::MaxModuleType];
};
  
} // end of namespace
 
#endif
