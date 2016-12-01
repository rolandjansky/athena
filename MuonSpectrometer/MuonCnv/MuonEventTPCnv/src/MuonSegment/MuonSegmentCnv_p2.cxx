/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "MuonSegment/MuonSegment.h"
#include "MuonEventTPCnv/MuonSegment/MuonSegmentCnv_p2.h"
#include "TrkSurfaces/Surface.h"
#include "TrkSurfaces/PlaneSurface.h"
#include "TrkEventCnvTools/DetElementSurface.h"
#include "TrkEventPrimitives/LocalDirection.h"


void MuonSegmentCnv_p2::persToTrans( const Muon::MuonSegment_p1 *persObj, Muon::MuonSegment *transObj, MsgStream &log )
{
  Amg::Vector2D locSegPos;
  locSegPos.setZero();

  Trk::LocalDirection localDirection;
  m_localDirCnv.persToTrans(&persObj->m_localDirection,&localDirection,log);

  Trk::PlaneSurface* associatedSurface = 
    static_cast<Trk::PlaneSurface*>(this->createTransFromPStore( &m_surfCnv, persObj->m_associatedSurface, log ));

  *transObj = Muon::MuonSegment (locSegPos,
                                 localDirection,
                                 Amg::MatrixX(), // locErr,
                                 associatedSurface,
                                 nullptr, // cmeas,
                                 nullptr, // qual
                                 Trk::Segment::AuthorUnknown
                                 );
  fillTransFromPStore( &m_segmentCnv, persObj->m_segment, transObj, log );
  transObj->setT0Error (persObj->m_t0, persObj->m_t0Error);
  transObj->recalculateCache();
}


void MuonSegmentCnv_p2::transToPers( const Muon::MuonSegment *transObj, Muon::MuonSegment_p1 *persObj, MsgStream &log )
{
  persObj->m_segment = baseToPersistent( &m_segmentCnv, transObj, log );
  m_localDirCnv.transToPers(&transObj->localDirection(),&persObj->m_localDirection,log);

    //if (transObj->m_localDirection!=0) 
    //    m_localDirCnv.transToPers(&persObj->m_localDirection,const_cast<Trk::LocalDirection*>(transObj->m_localDirection),log);
    //else
    //    log<<MSG::WARNING<<"MuonSegment is apparently missing a LocalDirection. Dumping:"<<*transObj<<endmsg;
  m_surfCnv=0;
  if (transObj->associatedSurface().isFree() ) // if this is a free surface, write it out 'as is'
    persObj->m_associatedSurface = toPersistent(&m_surfCnv, &transObj->associatedSurface(), log);
  else { // else, make it into a DetElementSurface, to allow the correct convertor to be called
    Trk::DetElementSurface surf( transObj->associatedSurface());
    persObj->m_associatedSurface = toPersistent(&m_surfCnv, &surf, log);
  }

  persObj->m_t0 = transObj->time();
  persObj->m_t0Error = transObj->errorTime();

}
