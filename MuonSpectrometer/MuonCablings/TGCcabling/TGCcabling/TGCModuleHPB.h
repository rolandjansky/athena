/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TGCMODULEHPB_HH
#define TGCMODULEHPB_HH
 
#include "TGCcabling/TGCModuleId.h"
 
namespace LVL1TGCCabling8
{
  
class TGCModuleHPB : public TGCModuleId
{
public:
  // Constructor & Destructor
  TGCModuleHPB (TGCIdBase::SideType side,
		TGCIdBase::SignalType signal,
		TGCIdBase::RegionType region,
		int sector,
		int id);

  virtual ~TGCModuleHPB (void) {}

  virtual bool isValid (void) const;
  
private:
  TGCModuleHPB (void) {}
};
  
} // end of namespace
 
#endif
