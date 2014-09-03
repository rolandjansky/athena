/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARTPCNV_LARRAWCHANNELCONTAINER_P4_H
#define LARTPCNV_LARRAWCHANNELCONTAINER_P4_H

#include "LArTPCnv/LArRawChannel_p2.h"

#include <vector>


/**
@class LArRawChannelContainer_p4
@brief Persistent LArRawChannelContainer, version p3

*/

class LArRawChannelContainer_p4
{
 public:
  
  /** constructor */
  LArRawChannelContainer_p4() { };
  
  friend class LArRawChannelContainerCnv_p4;

 private:
    std::vector<LArRawChannel_p2>           m_channels;
};

#endif

