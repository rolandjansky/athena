/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TGCCHANNELHPBIN_HH
#define TGCCHANNELHPBIN_HH

#include "TGCcabling/TGCChannelId.h"
#include "TGCcabling/TGCModuleHPB.h"

namespace LVL1TGCCabling8
{
  
class TGCChannelHPBIn : public TGCChannelId
{
public:
  // Constructor & Destructor
  TGCChannelHPBIn (TGCIdBase::SideType side,
		   TGCIdBase::SignalType signal,
		   TGCIdBase::RegionType region,
		   int sector,
		   int id,
		   int block,
		   int channel);

  virtual ~TGCChannelHPBIn (void) {}
  
  virtual TGCModuleId* getModule (void) const;

  virtual bool isValid (void) const;

private:
  static const int s_numberOfBlock;
  static const int s_channelInBlock;
  static const int s_slbInBlock;
  
public:
  static int getNumberOfBlock (void);
  static int getChannelInBlock (void);
  static int getSlbInBlock (void);

private:
  TGCChannelHPBIn (void) {}

};

} // end of namespace
 
#endif
