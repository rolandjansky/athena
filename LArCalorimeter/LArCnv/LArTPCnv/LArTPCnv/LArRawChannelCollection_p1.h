/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef LARTPCNV_LARRAWCHANNELCOLLECTION_P1_H
#define LARTPCNV_LARRAWCHANNELCOLLECTION_P1_H

#include "LArTPCnv/LArRawChannel_p1.h"

#include <vector>

/**
@class LArRawChannelCollection_p1 
@brief Persistent LArRawChannelCollection, version p1

*/
class LArRawChannelCollection_p1 : public std::vector<LArRawChannel_p1> {
  
 public:
  
  LArRawChannelCollection_p1() : m_id(0) { } ;
  
  friend class LArRawChannelCollectionCnv_p1;
  
 private:
  
  // Identifier of this collection
  unsigned int m_id;
  
};

#endif
