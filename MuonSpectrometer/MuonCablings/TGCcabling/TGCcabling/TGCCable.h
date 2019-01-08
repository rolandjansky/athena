/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TGCCABLE_HH
#define TGCCABLE_HH

#include <string>
#include <map>

#include "TGCcabling/TGCId.h"
#include "TGCcabling/TGCChannelId.h"
#include "TGCcabling/TGCModuleId.h"
#include "TGCcabling/TGCModuleMap.h"

namespace LVL1TGCCabling8
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
    this->m_type = type;
  }
  virtual ~TGCCable (void) {}
  
  CableType getCableType (void) const { return m_type; }

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
  CableType m_type;
  
};
  
} // end of namespace
 
#endif
