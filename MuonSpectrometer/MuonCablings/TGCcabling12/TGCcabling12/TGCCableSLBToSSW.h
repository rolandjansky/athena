/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TGCCABLESLBTOSSW_HH
#define TGCCABLESLBTOSSW_HH
 
#include <string>
#include "TGCcabling12/TGCCable.h"
#include "TGCcabling12/TGCId.h"
#include "TGCcabling12/TGCDatabaseSLBToROD.h" 
#include "TGCcabling12/TGCModuleSLB.h"
#include "TGCcabling12/TGCModuleSSW.h"

namespace LVL1TGCCabling12
{
  
class TGCCableSLBToSSW : public TGCCable
{
public:
  // Constructor & Destructor
  TGCCableSLBToSSW (std::string filename);

  virtual ~TGCCableSLBToSSW (void);
  
  virtual TGCModuleMap* getModule (const TGCModuleId* moduleId) const;

  enum {
    SL            = TGCIdBase::MaxModuleType, 
    MaxModuleType = TGCIdBase::MaxModuleType + 1
  };

private:
  TGCCableSLBToSSW (void) {}
  virtual TGCModuleMap* getModuleIn (const TGCModuleId* ssw) const;
  virtual TGCModuleMap* getModuleOut (const TGCModuleId* slb) const;
  TGCDatabase* database[TGCIdBase::MaxRegionType][MaxModuleType];
};
  
} // end of namespace
 
#endif
