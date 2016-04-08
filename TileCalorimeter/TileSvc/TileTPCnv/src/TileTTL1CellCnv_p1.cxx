///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// TileTTL1CellCnv_p1.cxx 
// Implementation file for class TileTTL1CellCnv_p1
// Author: Alexander Solodkov <Sanya.Solodkov@cern.ch>
// Date:   June 2009
/////////////////////////////////////////////////////////////////// 

#include "TileEvent/TileTTL1Cell.h"
#include "TileTPCnv/TileTTL1CellCnv_p1.h"


void
TileTTL1CellCnv_p1::persToTrans(const TileTTL1Cell_p1* persObj, TileTTL1Cell* transObj, MsgStream &/*log*/)
{
  *transObj = TileTTL1Cell (Identifier(Identifier32(persObj->m_channelID)),
                            persObj->m_eneTower,
                            persObj->m_timeAve,
                            persObj->m_corrFactor,
                            persObj->m_quality);
}


void
TileTTL1CellCnv_p1::transToPers(const TileTTL1Cell* transObj, TileTTL1Cell_p1* persObj, MsgStream &/*log*/)
{
  persObj->m_channelID = transObj->identify().get_identifier32().get_compact();

  // Tower energy
  persObj->m_eneTower = transObj->eneTower();

  // Time average
  persObj->m_timeAve = transObj->timeAve();

  // Correction factor
  persObj->m_corrFactor = transObj->corrFactor();

  // quality factor
  persObj->m_quality = transObj->qualTower();

}
