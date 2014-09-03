/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARTPCNV_LARRAWCHANNELCONTAINER_P1_H
#define LARTPCNV_LARRAWCHANNELCONTAINER_P1_H

#include "LArTPCnv/LArRawChannelCollection_p1.h"

#include <vector>

/**
@class LArRawChannelContainer_p1 
@brief Persistent LArRawChannelContainer, version p1

*/
class LArRawChannelContainer_p1 : public std::vector<LArRawChannelCollection_p1> 
{
 public:
  
  /** constructor */
  LArRawChannelContainer_p1() { };
  
  friend class LArRawChannelContainerCnv_p1;
};

#endif

