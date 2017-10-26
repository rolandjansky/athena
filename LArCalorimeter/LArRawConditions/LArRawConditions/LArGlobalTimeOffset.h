/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARGLOBALTIMEOFFSET_H
#define LARGLOBALTIMEOFFSET_H

#include "AthenaKernel/CLASS_DEF.h" 
#include "LArElecCalib/ILArGlobalTimeOffset.h"

class LArGlobalTimeOffset: public ILArGlobalTimeOffset {
 public:
  LArGlobalTimeOffset();                                              //Constructor
  virtual ~LArGlobalTimeOffset() {};                                  //Destructor 
  inline void setTimeOffset(const float offset) {m_offset=offset;}    //Setter
  inline float TimeOffset() const {return m_offset;}                  //Getter

 private:
  float m_offset;
};


CLASS_DEF( LArGlobalTimeOffset, 213690101, 1) 
#endif 
