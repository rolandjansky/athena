/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//-----------------------------------------------------------------------------
//
// file:   MuonSegmentQualityCnv_p1.cxx
//
//-----------------------------------------------------------------------------

#include "MuonSegment/MuonSegmentQuality.h"
#include "TrkEventPrimitives/FitQuality.h"
#include "MuonEventTPCnv/MuonSegment/MuonSegmentQualityCnv_p1.h"
#include <iterator>
#include <algorithm>

void MuonSegmentQualityCnv_p1::
persToTrans( const Muon::MuonSegmentQuality_p1 *persObj, Muon::MuonSegmentQuality *transObj,MsgStream &log ) 
{
  std::vector<Identifier> channelsWithoutHit;
  for (unsigned int x : persObj->m_channelsWithoutHit)
    channelsWithoutHit.push_back(Identifier(Identifier32(x)));

   *transObj = Muon::MuonSegmentQuality (0, // chi2
                                         0, // ndof
                                         std::move(channelsWithoutHit));
   fillTransFromPStore( &m_baseCnv, persObj->m_base,  transObj, log );
}

void MuonSegmentQualityCnv_p1::
transToPers( const Muon::MuonSegmentQuality *transObj, Muon::MuonSegmentQuality_p1 *persObj, MsgStream &log )
{
    persObj->m_base = baseToPersistent( &m_baseCnv,  transObj, log );
    persObj->m_channelsWithoutHit.clear();
    persObj->m_channelsWithoutHit.reserve(transObj->channelsWithoutHit().size());
    for (const Identifier& id : transObj->channelsWithoutHit())
      persObj->m_channelsWithoutHit.push_back(id.get_identifier32().get_compact());
}



