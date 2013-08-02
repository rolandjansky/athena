/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONTGC_CABLING_TGCCHANNELHPBIN_HH
#define MUONTGC_CABLING_TGCCHANNELHPBIN_HH

#include "MuonTGC_Cabling/TGCChannelId.h"

namespace MuonTGC_Cabling
{
  
class TGCChannelHPBIn : public TGCChannelId
{
public:
  // Constructor & Destructor
  TGCChannelHPBIn(TGCIdBase::SideType side,
		  TGCIdBase::SignalType signal,
		  TGCIdBase::RegionType region,
		  int sector,
		  int id,
		  int block,
		  int channel);
  
  virtual ~TGCChannelHPBIn(void) {}
  
  virtual TGCModuleId* getModule(void) const;

  virtual bool isValid(void) const;

private:
  static const int numberOfBlock;
  static const int channelInBlock;
  static const int slbInBlock;
  
public:
  static int getNumberOfBlock(void);
  static int getChannelInBlock(void);
  static int getSlbInBlock(void);

private:
  TGCChannelHPBIn(void) {}

};

} // end of namespace
 
#endif
