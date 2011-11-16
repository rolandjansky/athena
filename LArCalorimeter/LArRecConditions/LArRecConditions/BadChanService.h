/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef BadChanService_H
#define BadChanService_H

#include "LArRecConditions/LArBadChannel.h"
#include "LArRecConditions/BadChanContainer.h"

class BadChanService {
public:

	typedef unsigned int                                    ChannelType;
	typedef BadChanContainer< ChannelType, LArBadChannel>   BadChanInfo;

  static BadChanInfo& info() {
    static BadChanInfo* m_info = 0;
    if (m_info == 0) m_info = new BadChanInfo;
    return *m_info;
  }

};

#endif
