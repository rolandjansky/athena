///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ZdcRawChannelCnv_p1.cxx
// Implementation file for class ZdcRawChannelCnv_p1
// Author: Peter Steinberg <peter.steinberg@bnl.gov>
// Date:   June 2009
///////////////////////////////////////////////////////////////////

#define private public
#define protected public
#include "ZdcEvent/ZdcRawChannel.h"
#undef private
#undef protected

#include "ZdcEventTPCnv/ZdcRawChannelCnv_p1.h"


void
ZdcRawChannelCnv_p1::persToTrans(const ZdcRawChannel_p1* persObj, ZdcRawChannel* transObj, MsgStream &log)
{
  //log << MSG::INFO << "ZdcRawChannelCnv_p1::persToTrans called " << endreq;
  log << MSG::VERBOSE << "ZdcRawChannelCnv_p1::persToTrans called " << endreq;

  transObj->m_id = Identifier(Identifier32(persObj->m_id));

  transObj->m_energy.reserve(persObj->m_energy.size());
  transObj->m_time.reserve(persObj->m_time.size());
  transObj->m_chi.reserve(persObj->m_chi.size());

    for (std::vector<float>::const_iterator it = persObj->m_energy.begin(); it != persObj->m_energy.end(); ++it) {
      float val = (*it);
      transObj->m_energy.push_back( val );
    }

    for (std::vector<float>::const_iterator it = persObj->m_time.begin(); it != persObj->m_time.end(); ++it) {
          float val = (*it);
          transObj->m_time.push_back( val );
        }

    for (std::vector<float>::const_iterator it = persObj->m_chi.begin(); it != persObj->m_chi.end(); ++it) {
          float val = (*it);
          transObj->m_chi.push_back( val );
        }
  //log << MSG::INFO << "ZdcRawChannelCnv_p1::reading in... " << endreq;




  //log << MSG::INFO << "ZdcRawChannelCnv_p1::finished reading in... " << endreq;

}


void
ZdcRawChannelCnv_p1::transToPers(const ZdcRawChannel* transObj, ZdcRawChannel_p1* persObj, MsgStream &log)
{
  log << MSG::VERBOSE << "ZdcRawChannelCnv_p1::transToPers called " << endreq;

  persObj->m_id = transObj->m_id.get_identifier32().get_compact();

    persObj->m_energy.reserve(transObj->m_energy.size());
    persObj->m_time.reserve(transObj->m_time.size());
    persObj->m_chi.reserve(transObj->m_chi.size());

    for (std::vector<float>::const_iterator it = transObj->m_energy.begin(); it != transObj->m_energy.end(); ++it) {
      float val = (*it);
      persObj->m_energy.push_back( val );
    }

    for (std::vector<float>::const_iterator it = transObj->m_time.begin(); it != transObj->m_time.end(); ++it) {
          float val = (*it);
          persObj->m_time.push_back( val );
        }

    for (std::vector<float>::const_iterator it = transObj->m_chi.begin(); it != transObj->m_chi.end(); ++it) {
          float val = (*it);
          persObj->m_chi.push_back( val );
        }
}
