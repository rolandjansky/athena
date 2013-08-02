/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONTGC_CABLING_TGCMODULEPP_HH
#define MUONTGC_CABLING_TGCMODULEPP_HH
 
#include "MuonTGC_Cabling/TGCModuleId.h"

namespace MuonTGC_Cabling
{
 
class TGCModulePP : public TGCModuleId
{
public:
  // Constructor & Destructor
  TGCModulePP(TGCIdBase::SideType side,
	       TGCIdBase::ModuleType module,
	       TGCIdBase::RegionType region,
	       int sector,
	       int id);

  virtual ~TGCModulePP(void) {}  

  virtual bool isValid(void) const;
  
private:
  TGCModulePP(void) {}
};
  
} // end of namespace
 
#endif
