/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TGCMODULESSW_HH
#define TGCMODULESSW_HH
 
#include "TGCcabling12/TGCModuleId.h"
 
namespace LVL1TGCCabling12
{
  
class TGCModuleSSW : public TGCModuleId
{
public:
  // Constructor & Destructor
  TGCModuleSSW (TGCIdBase::SideType side,
		int readoutSector,
		int id);

  virtual ~TGCModuleSSW (void) {}
  
  virtual bool isValid (void) const;

private:
  TGCModuleSSW (void) {}
};
  
} // end of namespace
 
#endif
