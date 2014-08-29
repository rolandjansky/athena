/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//-----------------------------------------------------------------------------
//
// file:   ChamberT0sCnv_p1.cxx
//
//-----------------------------------------------------------------------------

#define private public
#define protected public
#include "MuonChamberT0s/ChamberT0s.h"
#undef private
#undef protected

#include "MuonEventTPCnv/MuonChamberT0s/ChamberT0sCnv_p1.h"
#include <utility>

void ChamberT0sCnv_p1::
persToTrans( const Muon::ChamberT0s_p1 *persObj, Muon::ChamberT0s *transObj,MsgStream & log ) 
{
    if (log.level() <= MSG::DEBUG) log << MSG::DEBUG << "ChamberT0sCnv_p1::persToTrans" << endreq;
    // better way to do this?
    using namespace std;
    transObj->m_t0s.clear();
    transObj->m_t0s.reserve( persObj->m_t0s.size() );
    vector< pair < unsigned int, float > >::const_iterator it=persObj->m_t0s.begin(), itEnd=persObj->m_t0s.end();
    for (;it!=itEnd;++it) {
        transObj->m_t0s.push_back( make_pair( Identifier(it->first), it->second) );
    }
}

void ChamberT0sCnv_p1::
transToPers( const Muon::ChamberT0s *transObj, Muon::ChamberT0s_p1 *persObj, MsgStream & log )
{
    if (log.level() <= MSG::DEBUG) log << MSG::DEBUG << "ChamberT0sCnv_p1::transToPers" << endreq;
    using namespace std;
    persObj->m_t0s.reserve( transObj->m_t0s.size() );
    vector< pair < Identifier, float > >::const_iterator it=transObj->m_t0s.begin(), itEnd=transObj->m_t0s.end();
    for (;it!=itEnd;++it) {
        persObj->m_t0s.push_back( make_pair( it->first.get_identifier32().get_compact(), it->second) );
    }
}


