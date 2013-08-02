/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONTGC_CABLING_TGCCHANNELPPOUT_HH
#define MUONTGC_CABLING_TGCCHANNELPPOUT_HH
 
#include "MuonTGC_Cabling/TGCChannelId.h"

namespace MuonTGC_Cabling
{

class TGCChannelPPOut : public TGCChannelId
{
public:
  // Constructor & Destructor
  TGCChannelPPOut(TGCIdBase::SideType side,
		  TGCIdBase::ModuleType module,
		  TGCIdBase::RegionType region,
		  int sector,
		  int id,
		  int block,
		  int channel);

  virtual ~TGCChannelPPOut(void) {}

  virtual TGCModuleId* getModule(void) const;

  virtual bool isValid(void) const;
  
private:
  TGCChannelPPOut(void) {}
};
  
} // end of namespace
 
#endif
