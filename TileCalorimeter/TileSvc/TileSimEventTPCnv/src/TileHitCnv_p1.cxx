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

#include "TileSimEventTPCnv/TileHitCnv_p1.h"


void TileHitCnv_p1::persToTrans(const TileHit_p1* persObj, TileHit* transObj, MsgStream &/*log*/) {

   transObj->m_pmt_id = Identifier(Identifier32(persObj->m_channelID));
   transObj->m_energy.reserve(persObj->m_energy.size());

   for (const float energy : persObj->m_energy) {
     transObj->m_energy.push_back( energy );
   }

   for (const float time : persObj->m_time) {
     transObj->m_time.push_back( time );
   }

}

void TileHitCnv_p1::transToPers(const TileHit* transObj, TileHit_p1* persObj, MsgStream &/*log*/) {

   persObj->m_channelID = transObj->m_pmt_id.get_identifier32().get_compact();


   for (const float energy : transObj->m_energy) {
     persObj->m_energy.push_back( energy );
   }

   for (const float time : transObj->m_time) {
     persObj->m_time.push_back( time );
   }

}
