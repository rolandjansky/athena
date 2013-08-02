/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TGCCABLEPPTOSLB_HH
#define TGCCABLEPPTOSLB_HH
 
#include <string>
#include "TGCcabling12/TGCCable.h"
#include "TGCcabling12/TGCId.h"
#include "TGCcabling12/TGCDatabasePPToSL.h" 
#include "TGCcabling12/TGCChannelPPOut.h"
#include "TGCcabling12/TGCChannelSLBIn.h"
#include "TGCcabling12/TGCModulePP.h"
#include "TGCcabling12/TGCModuleSLB.h"

namespace LVL1TGCCabling12
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
  TGCDatabase* database[TGCIdBase::MaxRegionType][TGCIdBase::MaxModuleType];
};
  
} // end of namespace
 
#endif
