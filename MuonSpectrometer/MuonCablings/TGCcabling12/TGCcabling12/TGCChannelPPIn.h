/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TGCCHANNELPPIN_HH
#define TGCCHANNELPPIN_HH
 
#include "TGCcabling12/TGCChannelId.h"
#include "TGCcabling12/TGCModulePP.h"
 
namespace LVL1TGCCabling12
{
  
class TGCChannelPPIn : public TGCChannelId
{
public:
  // Constructor & Destructor
  TGCChannelPPIn (TGCIdBase::SideType side,
		  TGCIdBase::ModuleType module,
		  TGCIdBase::RegionType region,
		  int sector,
		  int id,
		  int block,
		  int channel);

  virtual ~TGCChannelPPIn (void) {}
  
  virtual TGCModuleId* getModule (void) const;

  virtual bool isValid (void) const;

private:
  TGCChannelPPIn (void) {}
};
  
} // end of namespace
 
#endif
