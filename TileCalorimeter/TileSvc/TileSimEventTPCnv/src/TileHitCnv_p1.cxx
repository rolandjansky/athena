///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// TileHitCnv_p1.cxx 
// Implementation file for class TileHitCnv_p1
// Author: Author: Davide Costanzo
// Date:   March 2007
/////////////////////////////////////////////////////////////////// 

#include "TileSimEvent/TileHit.h"
#include "TileSimEventTPCnv/TileHitCnv_p1.h"
#include <stdexcept>


void
TileHitCnv_p1::persToTrans(const TileHit_p1* persObj, TileHit* transObj, MsgStream &/*log*/)
{
  size_t sz = persObj->m_energy.size();
   if (sz != persObj->m_time.size())
     throw std::runtime_error ("TileHit_p1 vector size mismatch");

   *transObj = TileHit (Identifier(Identifier32(persObj->m_channelID)));
   transObj->resize (sz);

   for (size_t i = 0; i < sz; i++) {
     transObj->setEnergy (persObj->m_energy[i], i);
     transObj->setTime (persObj->m_time[i], i);
   }
}

void
TileHitCnv_p1::transToPers(const TileHit* transObj, TileHit_p1* persObj, MsgStream &/*log*/)
{
   persObj->m_channelID         = transObj->identify().get_identifier32().get_compact();

   size_t sz = transObj->size();
   persObj->m_energy.resize (sz);
   persObj->m_time.resize (sz);
   for (size_t i = 0; i < sz; i++) {
     persObj->m_energy[i] = transObj->energy(i);
     persObj->m_time[i] = transObj->time(i);
   }
}
