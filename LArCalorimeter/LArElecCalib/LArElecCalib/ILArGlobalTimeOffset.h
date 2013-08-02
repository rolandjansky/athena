/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ILARGLOBALTIMEOFFSET_H
#define ILARGLOBALTIMEOFFSET_H

#include "CLIDSvc/CLASS_DEF.h" 

class ILArGlobalTimeOffset {
 public:
  virtual ~ILArGlobalTimeOffset() {};

  virtual void setTimeOffset(const float offset)=0;    //Setter
  virtual float TimeOffset() const=0;                  //Getter
};


CLASS_DEF(ILArGlobalTimeOffset, 112363537, 1) 
#endif 
