/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARTPCNV_LARRAWCHANNELCONTAINER_P2_H
#define LARTPCNV_LARRAWCHANNELCONTAINER_P2_H

#include "LArTPCnv/LArRawChannelCollection_p2.h"

#include <vector>


/**
@class LArRawChannelContainer_p2
@brief Persistent LArRawChannelContainer, version p2

*/

class LArRawChannelContainer_p2
{
 public:
  
  /** constructor */
  LArRawChannelContainer_p2() { };
  
  friend class LArRawChannelContainerCnv_p2;

 private:
  
    std::vector<LArRawChannelCollection_p2> m_collections;
    std::vector<LArRawChannel_p1>           m_channels;
};

#endif

