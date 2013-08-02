/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONTGC_CABLING_TGCCHANNELPPIN_HH
#define MUONTGC_CABLING_TGCCHANNELPPIN_HH
 
#include "MuonTGC_Cabling/TGCChannelId.h"
 
namespace MuonTGC_Cabling
{
 
class TGCChannelPPIn : public TGCChannelId
{
public:
  // Constructor & Destructor
  TGCChannelPPIn(TGCIdBase::SideType side,
		 TGCIdBase::ModuleType module,
		 TGCIdBase::RegionType region,
		 int sector,
		 int id,
		 int block,
		 int channel);

  virtual ~TGCChannelPPIn(void) {}
  
  virtual TGCModuleId* getModule(void) const;

  virtual bool isValid(void) const;

private:
  TGCChannelPPIn(void) {}
};
  
} // end of namespace
 
#endif
