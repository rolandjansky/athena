/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "CaloEvent/CaloRecoStatus.h"
#include <cstddef>

bool CaloRecoStatus::getStatus(std::vector<CaloRecoStatus::StatusIndicator>&
			       pStatusList) const
{
  size_t oldSize = pStatusList.size();
  if ( (TAGGEDEM  & m_status) == TAGGEDEM   ) pStatusList.push_back(TAGGEDEM);
  if ( (TAGGEDHAD & m_status) == TAGGEDHAD  ) pStatusList.push_back(TAGGEDHAD);
  if ( (TAGGEDMUON & m_status) == TAGGEDMUON )
    pStatusList.push_back(TAGGEDMUON);
  if ( (TAGGEDUNKNOWN & m_status) == TAGGEDUNKNOWN ) 
    pStatusList.push_back(TAGGEDUNKNOWN);
  return oldSize < pStatusList.size();
}
