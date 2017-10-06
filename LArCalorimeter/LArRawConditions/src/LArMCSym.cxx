/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArRawConditions/LArMCSym.h"


LArMCSym::LArMCSym(const LArOnlineID* onlId, 
		   std::vector<HWIdentifier>&& oflHashtoSymOnl) :
  m_onlineID(onlId),
  m_oflHashtoSymOnl(oflHashtoSymOnl) 
{}
