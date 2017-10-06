/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArRawConditions/LArMCSym.h"


LArMCSym::LArMCSym(const LArOnlineID* onlId, 
		   const CaloCell_ID* caloId,
		   std::vector<HWIdentifier>&& oflHashtoSymOnl) :
  m_onlineID(onlId),
  m_caloCellID(caloId),
  m_oflHashtoSymOnl(oflHashtoSymOnl) 
{}
