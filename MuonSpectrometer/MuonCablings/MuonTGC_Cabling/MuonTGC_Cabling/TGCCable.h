/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONTGC_CABLING_TGCCABLE_HH
#define MUONTGC_CABLING_TGCCABLE_HH

#include "MuonTGC_Cabling/TGCChannelId.h"
#include "MuonTGC_Cabling/TGCModuleId.h"
#include "MuonTGC_Cabling/TGCModuleMap.h"

namespace MuonTGC_Cabling
{
 
class TGCCable
{
public:
  enum CableType {NoCableType=-1,
		  InASD, ASDToPP, InPP, PPToSLB, InSLB,
		  SLBToHPB, HPBToSL, SLBToSSW, SSWToROD,
		  MaxCableType};
		  
  // Constructor & Destructor
  TGCCable(CableType type=NoCableType)
    {
      this->m_type = type;
    }
  virtual ~TGCCable(void) {}
  
  CableType getCableType(void) const { return m_type; }

protected:
  // channel connection
  virtual TGCChannelId* getChannelIn(const TGCChannelId* ,
				      bool /*orChannel=false*/) const { return 0; }
  virtual TGCChannelId* getChannelOut(const TGCChannelId* ,
				       bool /*orChannel=false*/) const { return 0; }
  // module connection
  virtual TGCModuleMap* getModuleIn(const TGCModuleId* ) const { return 0; }
  virtual TGCModuleMap* getModuleOut(const TGCModuleId* ) const { return 0; }
  
private:
  CableType m_type;
  
};
  
} // end of namespace
 
#endif
