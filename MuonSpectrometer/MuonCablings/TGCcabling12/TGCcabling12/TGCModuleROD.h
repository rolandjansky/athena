/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TGCMODULEROD_HH
#define TGCMODULEROD_HH
 
#include "TGCcabling12/TGCModuleId.h"
 
namespace LVL1TGCCabling12
{
  
class TGCModuleROD : public TGCModuleId
{
public:
  // Constructor & Destructor
  TGCModuleROD (TGCIdBase::SideType side,
		int    readoutSector);

  virtual ~TGCModuleROD (void) {}

  virtual bool isValid (void) const;
  
private:
  TGCModuleROD (void) {}
};
  
} // end of namespace
 
#endif
