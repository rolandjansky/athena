/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//-----------------------------------------------------------------------------
//
// file:   MuonSegmentQualityCnv_p1.cxx
//
//-----------------------------------------------------------------------------

#define private public
#define protected public
#include "MuonSegment/MuonSegmentQuality.h"
#undef private
#undef protected

#include "TrkEventPrimitives/FitQuality.h"
#include "MuonEventTPCnv/MuonSegment/MuonSegmentQualityCnv_p1.h"
#include <iterator>
#include <algorithm>

void MuonSegmentQualityCnv_p1::
persToTrans( const Muon::MuonSegmentQuality_p1 *persObj, Muon::MuonSegmentQuality *transObj,MsgStream &log ) 
{
   fillTransFromPStore( &m_baseCnv, persObj->m_base,  transObj, log );
   //transObj->Trk::FitQuality::m_chiSquared   = persObj->m_chiSquared;
   //transObj->Trk::FitQuality::m_numberDoF    = persObj->m_numberDoF;
   // std::copy(   persObj->m_channelsWithoutHit.begin(), 
   //              persObj->m_channelsWithoutHit.end(), 
   //              std::back_inserter(transObj->m_channelsWithoutHit));
   std::vector<unsigned int>::const_iterator it, itEnd = persObj->m_channelsWithoutHit.end();
   for (it=persObj->m_channelsWithoutHit.begin(); it!=itEnd ; it++) 
       transObj->m_channelsWithoutHit.push_back(Identifier(Identifier32(*it)));
}

void MuonSegmentQualityCnv_p1::
transToPers( const Muon::MuonSegmentQuality *transObj, Muon::MuonSegmentQuality_p1 *persObj, MsgStream &log )
{
    persObj->m_base = baseToPersistent( &m_baseCnv,  transObj, log );
    //persObj->m_chiSquared   = transObj->m_chiSquared;
    //persObj->m_numberDoF    = transObj->m_numberDoF;
    persObj->m_channelsWithoutHit.reserve(transObj->m_channelsWithoutHit.size());
    std::vector<Identifier>::const_iterator it    = transObj->m_channelsWithoutHit.begin();
    std::vector<Identifier>::const_iterator itEnd = transObj->m_channelsWithoutHit.end();
    for ( ; it != itEnd; ++it) 
        persObj->m_channelsWithoutHit.push_back((*it).get_identifier32().get_compact());
}



