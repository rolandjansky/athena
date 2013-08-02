/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONTGC_CABLING_TGCMODULEHPB_HH
#define MUONTGC_CABLING_TGCMODULEHPB_HH
 
#include "MuonTGC_Cabling/TGCModuleId.h"
 
namespace MuonTGC_Cabling
{
  
class TGCModuleHPB : public TGCModuleId
{
public:
  // Constructor & Destructor
  TGCModuleHPB(TGCIdBase::SideType side,
	       TGCIdBase::SignalType signal,
	       TGCIdBase::RegionType region,
	       int sector,
	       int id);

  virtual ~TGCModuleHPB(void) {}

  virtual bool isValid(void) const;
  
private:
  TGCModuleHPB(void) {}
};
  
} // end of namespace
 
#endif
