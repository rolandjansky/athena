/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TGCMODULESLB_HH
#define TGCMODULESLB_HH
 
#include "TGCcabling/TGCModuleId.h"
 
namespace LVL1TGCCabling8
{
  
class TGCModuleSLB : public TGCModuleId
{
public:
  // Constructor & Destructor
  TGCModuleSLB (TGCIdBase::SideType side,
		TGCIdBase::ModuleType module,
		TGCIdBase::RegionType region,
		int sector,
		int id);

  virtual ~TGCModuleSLB (void) {}

  virtual bool isValid (void) const;
  
private:
  TGCModuleSLB (void) {}
};
  
} // end of namespace
 
#endif
