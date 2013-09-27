/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Converter from TrigSpacePointCounts_p1_old to TrigSpacePointCounts_p1.
// $Id: TrigSpacePointCounts_p1_old_cnv.cxx,v 1.2 2009-04-01 22:08:46 salvator Exp $

#include "TrigInDetEventTPCnv/TrigSpacePointCounts_p1_old_cnv.h"

void TrigSpacePointCounts_p1_old_cnv::Convert (TrigSpacePointCounts_p1* newobj,
                                   const TrigSpacePointCounts_p1_old* oldobj)
{
  newobj->m_pixSPcnt     = oldobj->m_pixcnt;
  newobj->m_sctSPcnt     = oldobj->m_sctcnt;

  newobj->m_pixCL1cnt    = 0;
  newobj->m_pixCL2cnt    = 0;
  newobj->m_pixCLmin3cnt = 0;
  newobj->m_SPpixBarr_cnt= 0;
  newobj->m_SPpixECAcnt  = 0;
  newobj->m_SPpixECCcnt  = 0;

  newobj->m_SPsctBarr_cnt= 0;
  newobj->m_SPsctECAcnt  = 0;
  newobj->m_SPsctECCcnt  = 0;
}
