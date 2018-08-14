///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ZdcRawChannelCnv_p1.cxx
// Implementation file for class ZdcRawChannelCnv_p1
// Author: Peter Steinberg <peter.steinberg@bnl.gov>
// Date:   June 2009
///////////////////////////////////////////////////////////////////

#include "ZdcEvent/ZdcRawChannel.h"
#include "ZdcEventTPCnv/ZdcRawChannelCnv_p1.h"


void
ZdcRawChannelCnv_p1::persToTrans(const ZdcRawChannel_p1* persObj, ZdcRawChannel* transObj, MsgStream &log)
{
  //log << MSG::INFO << "ZdcRawChannelCnv_p1::persToTrans called " << endmsg;
  log << MSG::VERBOSE << "ZdcRawChannelCnv_p1::persToTrans called " << endmsg;

  size_t sz = persObj->m_energy.size();

  if (sz != persObj->m_time.size()) std::abort();
  if (sz != persObj->m_chi.size()) std::abort();

  *static_cast<ZdcRawData*> (transObj) =
    ZdcRawData (Identifier(Identifier32(persObj->m_id)));

  transObj->setSize (sz);
  for (size_t i = 0; i < sz; i++) {
    transObj->setEnergy (i, persObj->m_energy[i]);
    transObj->setTime   (i, persObj->m_time[i]);
    transObj->setChi    (i, persObj->m_chi[i]);
  }
}


void
ZdcRawChannelCnv_p1::transToPers(const ZdcRawChannel* transObj, ZdcRawChannel_p1* persObj, MsgStream &log)
{
  log << MSG::VERBOSE << "ZdcRawChannelCnv_p1::transToPers called " << endmsg;

  persObj->m_id = transObj->identify().get_identifier32().get_compact();

  size_t sz = transObj->getSize();
  persObj->m_energy.resize(sz);
  persObj->m_time.resize(sz);
  persObj->m_chi.resize(sz);

  for (size_t i = 0; i < sz; i++) {
    persObj->m_energy[i] = transObj->getEnergy(i);
    persObj->m_time[i]   = transObj->getTime(i);
    persObj->m_chi[i]    = transObj->getChi(i);
  }
}
