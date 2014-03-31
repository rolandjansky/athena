///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// TileHitCnv_p1.cxx 
// Implementation file for class TileHitCnv_p1
// Author: Author: Davide Costanzo
// Date:   March 2007
/////////////////////////////////////////////////////////////////// 

#define private public
#define protected public
#include "TileSimEvent/TileHit.h"
#undef private
#undef protected

#include "TileTPCnv/TileHitCnv_p1.h"


void
TileHitCnv_p1::persToTrans(const TileHit_p1* persObj, TileHit* transObj, MsgStream &/*log*/)
{
   transObj->m_pmt_id          = Identifier(Identifier32(persObj->m_channelID));
   
   transObj->m_energy.reserve(persObj->m_energy.size());
   for (std::vector<float>::const_iterator it = persObj->m_energy.begin(); it != persObj->m_energy.end(); ++it) {
    transObj->m_energy.push_back( *it );
   }
   for (std::vector<float>::const_iterator it = persObj->m_time.begin(); it != persObj->m_time.end(); ++it) {
    transObj->m_time.push_back( *it );
   }

}

void
TileHitCnv_p1::transToPers(const TileHit* transObj, TileHit_p1* persObj, MsgStream &/*log*/)
{
   persObj->m_channelID         = transObj->m_pmt_id.get_identifier32().get_compact();

   for (std::vector<float>::const_iterator it = transObj->m_energy.begin(); it != transObj->m_energy.end(); ++it) {
    persObj->m_energy.push_back( *it );
   }
   for (std::vector<float>::const_iterator it = transObj->m_time.begin(); it != transObj->m_time.end(); ++it) {
    persObj->m_time.push_back( *it );
   }

}
