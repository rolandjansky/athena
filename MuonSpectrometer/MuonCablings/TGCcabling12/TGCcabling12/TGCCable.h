/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TGCCABLE_HH
#define TGCCABLE_HH

#include <string>
#include <map>

#include "TGCcabling12/TGCId.h"
#include "TGCcabling12/TGCChannelId.h"
#include "TGCcabling12/TGCModuleId.h"
#include "TGCcabling12/TGCModuleMap.h"

namespace LVL1TGCCabling12
{
 
class TGCCable
{
public:
  enum CableType {NoCableType=-1,
		  InASD,ASDToPP,InPP,PPToSLB,InSLB,
		  SLBToHPB,HPBToSL,SLBToSSW,SSWToROD,
		  MaxCableType};
		  
  // Constructor & Destructor
  TGCCable (CableType type=NoCableType)
  {
    this->type = type;
  }
  virtual ~TGCCable (void) {}
  
  CableType getCableType (void) const { return type; }

protected:
  // channel connection
  virtual TGCChannelId* getChannelIn (const TGCChannelId* ,
				      bool /*orChannel=false*/) const { return 0; }
  virtual TGCChannelId* getChannelOut (const TGCChannelId* ,
				       bool /*orChannel=false*/) const { return 0; }
  // module connection
  virtual TGCModuleMap* getModuleIn (const TGCModuleId* ) const 
  { return 0; }
  virtual TGCModuleMap* getModuleOut (const TGCModuleId* ) const 
  { return 0; }
  
private:
  CableType type;
  
};
  
} // end of namespace
 
#endif
