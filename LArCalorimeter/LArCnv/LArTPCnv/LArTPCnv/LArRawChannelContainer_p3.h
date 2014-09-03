/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARTPCNV_LARRAWCHANNELCONTAINER_P3_H
#define LARTPCNV_LARRAWCHANNELCONTAINER_P3_H

#include "LArTPCnv/LArRawChannel_p1.h"

#include <vector>


/**
@class LArRawChannelContainer_p3
@brief Persistent LArRawChannelContainer, version p3

*/

class LArRawChannelContainer_p3
{
 public:
  
  /** constructor */
  LArRawChannelContainer_p3() { };
  
  friend class LArRawChannelContainerCnv_p3;

 private:
    std::vector<LArRawChannel_p1>           m_channels;
};

#endif

