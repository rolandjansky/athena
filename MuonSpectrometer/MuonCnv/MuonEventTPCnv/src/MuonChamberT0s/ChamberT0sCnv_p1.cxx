/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//-----------------------------------------------------------------------------
//
// file:   ChamberT0sCnv_p1.cxx
//
//-----------------------------------------------------------------------------

#include "MuonChamberT0s/ChamberT0s.h"
#include "MuonEventTPCnv/MuonChamberT0s/ChamberT0sCnv_p1.h"
#include <utility>

void ChamberT0sCnv_p1::
persToTrans( const Muon::ChamberT0s_p1 *persObj, Muon::ChamberT0s *transObj,MsgStream & log ) 
{
    if (log.level() <= MSG::DEBUG) log << MSG::DEBUG << "ChamberT0sCnv_p1::persToTrans" << endmsg;
    // better way to do this?

    std::vector<std::pair<Identifier, float> > t0s;
    t0s.reserve( persObj->m_t0s.size() );
    for (const std::pair<unsigned int, float>& p : persObj->m_t0s) {
      t0s.emplace_back( Identifier(p.first), p.second );
    }

    *transObj = Muon::ChamberT0s (std::move(t0s));
}

void ChamberT0sCnv_p1::
transToPers( const Muon::ChamberT0s *transObj, Muon::ChamberT0s_p1 *persObj, MsgStream & log )
{
    if (log.level() <= MSG::DEBUG) log << MSG::DEBUG << "ChamberT0sCnv_p1::transToPers" << endmsg;
    persObj->m_t0s.clear();
    persObj->m_t0s.reserve( transObj->getAllT0s().size() );
    for (const std::pair< Identifier, float>& p : transObj->getAllT0s() ) {
      persObj->m_t0s.emplace_back( p.first.get_identifier32().get_compact(), p.second );
    }
}


