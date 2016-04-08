///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// TileTTL1Cnv_p1.cxx 
// Implementation file for class TileTTL1Cnv_p1
// Author: Alexander Solodkov <Sanya.Solodkov@cern.ch>
// Date:   June 2009
/////////////////////////////////////////////////////////////////// 

#include "TileEvent/TileTTL1.h"
#include "TileTPCnv/TileTTL1Cnv_p1.h"


void
TileTTL1Cnv_p1::persToTrans(const TileTTL1_p1* persObj, TileTTL1* transObj, MsgStream &/*log*/)
{
  *transObj = TileTTL1 (Identifier(Identifier32(persObj->m_channelID)),
                        persObj->m_TTL1digits);
}


void
TileTTL1Cnv_p1::transToPers(const TileTTL1* transObj, TileTTL1_p1* persObj, MsgStream &/*log*/)
{
  persObj->m_channelID = transObj->identify().get_identifier32().get_compact();
  persObj->m_TTL1digits = transObj->fsamples();
}
