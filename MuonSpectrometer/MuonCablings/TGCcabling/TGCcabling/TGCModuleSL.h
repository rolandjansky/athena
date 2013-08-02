/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TGCMODULESL_HH
#define TGCMODULESL_HH
 
#include "TGCcabling/TGCModuleId.h"
 
namespace LVL1TGCCabling8
{
  
class TGCModuleSL : public TGCModuleId
{
public:
  // Constructor & Destructor
  TGCModuleSL (TGCIdBase::SideType side,
	       TGCIdBase::RegionType region,
	       int sector);

  virtual ~TGCModuleSL (void) {}

  virtual bool isValid (void) const;
  
private:
  TGCModuleSL (void) {}
};
  
} // end of namespace
 
#endif
