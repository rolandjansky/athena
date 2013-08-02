/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONTGC_CABLING_TGCMODULESSW_HH
#define MUONTGC_CABLING_TGCMODULESSW_HH
 
#include "MuonTGC_Cabling/TGCModuleId.h"
 
namespace MuonTGC_Cabling
{
  
class TGCModuleSSW : public TGCModuleId
{
public:
  // Constructor & Destructor
  TGCModuleSSW(TGCIdBase::SideType side,
	       int readoutSector,
	       int id);

  virtual ~TGCModuleSSW(void) {}
  
  virtual bool isValid(void) const;

private:
  TGCModuleSSW(void) {}
};
  
} // end of namespace
 
#endif
