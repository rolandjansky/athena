/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TGCMODULEPP_HH
#define TGCMODULEPP_HH
 
#include "TGCcabling/TGCModuleId.h"

namespace LVL1TGCCabling8
{
 
class TGCModulePP : public TGCModuleId
{
public:
  // Constructor & Destructor
  TGCModulePP (TGCIdBase::SideType side,
	       TGCIdBase::ModuleType module,
	       TGCIdBase::RegionType region,
	       int sector,
	       int id);

  virtual ~TGCModulePP (void) {}  

  virtual bool isValid (void) const;
  
private:
  TGCModulePP (void) {}
};
  
} // end of namespace
 
#endif
