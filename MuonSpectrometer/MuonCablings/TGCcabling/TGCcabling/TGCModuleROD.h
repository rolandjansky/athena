/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TGCMODULEROD_HH
#define TGCMODULEROD_HH
 
#include "TGCcabling/TGCModuleId.h"
 
namespace LVL1TGCCabling8
{
  
class TGCModuleROD : public TGCModuleId
{
public:
  // Constructor & Destructor
  TGCModuleROD (TGCIdBase::SideType side,
		int octant);

  virtual ~TGCModuleROD (void) {}

  virtual bool isValid (void) const;
  
private:
  TGCModuleROD (void) {}
};
  
} // end of namespace
 
#endif
