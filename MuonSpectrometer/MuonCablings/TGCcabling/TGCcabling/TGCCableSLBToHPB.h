/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TGCCABLESLBTOHPB_HH
#define TGCCABLESLBTOHPB_HH
 
#include <string>
#include "TGCcabling/TGCCable.h"
#include "TGCcabling/TGCId.h"
#include "TGCcabling/TGCDatabasePPToSL.h" 
#include "TGCcabling/TGCChannelSLBOut.h"
#include "TGCcabling/TGCChannelHPBIn.h"
#include "TGCcabling/TGCModuleSLB.h"
#include "TGCcabling/TGCModuleHPB.h"

namespace LVL1TGCCabling8
{
  
class TGCCableSLBToHPB : public TGCCable
{
public:

  // Constructor & Destructor
  TGCCableSLBToHPB (std::string filename);

  virtual ~TGCCableSLBToHPB (void);
  
  virtual TGCChannelId* getChannel (const TGCChannelId* channelId,
                                    bool orChannel=false) const;
  virtual TGCModuleMap* getModule (const TGCModuleId* moduleId) const;

  TGCChannelId* getChannelInforHPB (const TGCChannelId* hpbin,
				    TGCIdBase::ModuleType moduleType,
				    bool orChannel=false) const;

private:
  TGCCableSLBToHPB (void) {}
  virtual TGCChannelId* getChannelIn (const TGCChannelId* hpbin, 
                                      bool orChannel=false) const;
  virtual TGCChannelId* getChannelOut (const TGCChannelId* slbout,
                                       bool orChannel=false) const;
  virtual TGCModuleMap* getModuleIn (const TGCModuleId* hpb) const;
  virtual TGCModuleMap* getModuleInforHPB (const TGCModuleId* hpb, TGCIdBase::ModuleType moduleType) const;
  virtual TGCModuleMap* getModuleOut (const TGCModuleId* slb) const;
  TGCDatabase* m_database[TGCIdBase::MaxRegionType][TGCIdBase::MaxModuleType];
};
  
} // end of namespace
 
#endif
