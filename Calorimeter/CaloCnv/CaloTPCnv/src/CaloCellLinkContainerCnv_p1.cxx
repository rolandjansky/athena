/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "CaloTPCnv/CaloCellLinkContainerCnv_p1.h"

#include "CaloEvent/CaloCellLinkContainer.h"
#include "CaloTPCnv/CaloCellLinkContainer_p1.h"
#include "AthenaKernel/getThinningCache.h"
     

void CaloCellLinkContainerCnv_p1::persToTransWithKey (const CaloCellLinkContainer_p1* pers,
                                                      CaloCellLinkContainer* trans,
                                                      const std::string& /*key*/,
                                                      MsgStream &log) const
{
  trans->reserve(pers->m_links.size());
  for (const CaloCellLinkContainer_p1::CellLink& l : pers->m_links) {
    auto lnk = std::make_unique<CaloCellLink>();
    m_converter.persToTrans (l, *lnk, log);
    trans->push_back (std::move (lnk));
  }
}


void CaloCellLinkContainerCnv_p1::transToPersWithKey (const CaloCellLinkContainer* trans,
                                                      CaloCellLinkContainer_p1* pers,
                                                      const std::string& /*key*/,
                                                      MsgStream &log) const
{
  pers->m_links.resize(trans->size());
  const SG::ThinningCache* cache = SG::getThinningCache();
  std::vector<CaloCellLinkContainer_p1::CellLink>::size_type i=0;
  for (const CaloCellLink* l : *trans) {
    m_converter.transToPers (*l, pers->m_links[i], cache, log);
    ++i;
  }
}
