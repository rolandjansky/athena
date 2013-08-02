/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONTGC_CABLING_TGCMODULESL_HH
#define MUONTGC_CABLING_TGCMODULESL_HH
 
#include "MuonTGC_Cabling/TGCModuleId.h"
 
namespace MuonTGC_Cabling
{
  
class TGCModuleSL : public TGCModuleId
{
public:
  // Constructor & Destructor
  TGCModuleSL(TGCIdBase::SideType side,
	      TGCIdBase::RegionType region,
	      int sector);

  virtual ~TGCModuleSL(void) {}

  virtual bool isValid(void) const;
  
private:
  TGCModuleSL(void) {}
};
  
} // end of namespace
 
#endif
