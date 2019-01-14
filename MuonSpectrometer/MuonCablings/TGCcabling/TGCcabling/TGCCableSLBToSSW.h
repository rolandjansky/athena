/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TGCCABLESLBTOSSW_HH
#define TGCCABLESLBTOSSW_HH
 
#include <string>
#include "TGCcabling/TGCCable.h"
#include "TGCcabling/TGCId.h"
#include "TGCcabling/TGCDatabaseSLBToROD.h" 
#include "TGCcabling/TGCModuleSLB.h"
#include "TGCcabling/TGCModuleSSW.h"

namespace LVL1TGCCabling8
{
  
class TGCCableSLBToSSW : public TGCCable
{
public:
  // Constructor & Destructor
  TGCCableSLBToSSW (std::string filename);

  virtual ~TGCCableSLBToSSW (void);
  
  virtual TGCModuleMap* getModule (const TGCModuleId* moduleId) const;

private:
  TGCCableSLBToSSW (void) {}
  virtual TGCModuleMap* getModuleIn (const TGCModuleId* ssw) const;
  virtual TGCModuleMap* getModuleOut (const TGCModuleId* slb) const;
  TGCDatabase* m_database[TGCIdBase::MaxRegionType][TGCIdBase::MaxModuleType];
};
  
} // end of namespace
 
#endif
