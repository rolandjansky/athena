/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ILARFEBTIMEOFFSET_H
#define ILARFEBTIMEOFFSET_H

#include "Identifier/HWIdentifier.h"
#include "CLIDSvc/CLASS_DEF.h" 

class ILArFEBTimeOffset {
 public:
  virtual ~ILArFEBTimeOffset() {};
  virtual void setTimeOffset(const HWIdentifier fId, const float offset)=0; //Setter  
  virtual float TimeOffset(const HWIdentifier fId) const=0;                  //Getter
};
          
CLASS_DEF(ILArFEBTimeOffset,241986974,1)
#endif

