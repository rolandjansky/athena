/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TGCCHANNELSLBOUT_HH
#define TGCCHANNELSLBOUT_HH

#include "TGCcabling/TGCChannelId.h"
#include "TGCcabling/TGCModuleSLB.h"

namespace LVL1TGCCabling8
{
  
class TGCChannelSLBOut : public TGCChannelId
{
public:
  // Constructor & Destructor
  TGCChannelSLBOut (TGCIdBase::SideType side,
		    TGCIdBase::ModuleType module,
		    TGCIdBase::RegionType region,
		    int sector,
		    int id,
		    int block,
		    int channel);

  virtual ~TGCChannelSLBOut (void) {}

  virtual TGCModuleId* getModule (void) const;

  virtual bool isValid (void) const;

private:
  static const int numberOfBlockInWD;
  static const int numberOfBlockInSD;
  static const int numberOfBlockInWT;
  static const int numberOfBlockInST;
  static const int numberOfLayerInWD;
  static const int numberOfLayerInSD;
  static const int numberOfLayerInWT;
  static const int numberOfLayerInST;
  static const int channelInBlockForWD;
  static const int channelInBlockForSD;
  static const int channelInBlockForWT;
  static const int channelInBlockForST;

public:
  static int getNumberOfBlock (TGCIdBase::ModuleType moduleType);
  static int getNumberOfLayer (TGCIdBase::ModuleType moduleType);
  static int getChannelInBlock (TGCIdBase::ModuleType moduleType);

private:
  TGCChannelSLBOut (void) {}

};

} // end of namespace
 
#endif
