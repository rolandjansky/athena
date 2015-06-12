///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// TileTTL1CellCnv_p1.cxx 
// Implementation file for class TileTTL1CellCnv_p1
// Author: Alexander Solodkov <Sanya.Solodkov@cern.ch>
// Date:   June 2009
/////////////////////////////////////////////////////////////////// 

#define private public
#define protected public
#include "TileEvent/TileTTL1Cell.h"
#undef private
#undef protected

#include "TileTPCnv/TileTTL1CellCnv_p1.h"


void
TileTTL1CellCnv_p1::persToTrans(const TileTTL1Cell_p1* persObj, TileTTL1Cell* transObj, MsgStream &/*log*/)
{
  transObj->m_ID = Identifier(Identifier32(persObj->m_channelID));
   
  // Tower energy
  transObj->m_eneTower = persObj->m_eneTower;

  // Time average
  transObj->m_timeAve = persObj->m_timeAve;

  // Correction factor
  transObj->m_corrFactor = persObj->m_corrFactor;

  // quality factor
  transObj->m_quality = persObj->m_quality;

}


void
TileTTL1CellCnv_p1::transToPers(const TileTTL1Cell* transObj, TileTTL1Cell_p1* persObj, MsgStream &/*log*/)
{
  persObj->m_channelID = transObj->m_ID.get_identifier32().get_compact();

  // Tower energy
  persObj->m_eneTower = transObj->m_eneTower;

  // Time average
  persObj->m_timeAve = transObj->m_timeAve;

  // Correction factor
  persObj->m_corrFactor = transObj->m_corrFactor;

  // quality factor
  persObj->m_quality = transObj->m_quality;

}
