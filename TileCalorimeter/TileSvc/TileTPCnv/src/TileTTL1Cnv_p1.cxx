///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// TileTTL1Cnv_p1.cxx 
// Implementation file for class TileTTL1Cnv_p1
// Author: Alexander Solodkov <Sanya.Solodkov@cern.ch>
// Date:   June 2009
/////////////////////////////////////////////////////////////////// 

#define private public
#define protected public
#include "TileEvent/TileTTL1.h"
#undef private
#undef protected

#include "TileTPCnv/TileTTL1Cnv_p1.h"


void
TileTTL1Cnv_p1::persToTrans(const TileTTL1_p1* persObj, TileTTL1* transObj, MsgStream &/*log*/)
{
  transObj->m_ID = Identifier(Identifier32(persObj->m_channelID));
   
  transObj->m_TTL1digits.reserve(persObj->m_TTL1digits.size());
  for (std::vector<float>::const_iterator it = persObj->m_TTL1digits.begin(); it != persObj->m_TTL1digits.end(); ++it) {
    transObj->m_TTL1digits.push_back( *it );
  }
}


void
TileTTL1Cnv_p1::transToPers(const TileTTL1* transObj, TileTTL1_p1* persObj, MsgStream &/*log*/)
{
  persObj->m_channelID = transObj->m_ID.get_identifier32().get_compact();

  persObj->m_TTL1digits.reserve(transObj->m_TTL1digits.size());
  for (std::vector<float>::const_iterator it = transObj->m_TTL1digits.begin(); it != transObj->m_TTL1digits.end(); ++it) {
    persObj->m_TTL1digits.push_back( *it );
  }
}
