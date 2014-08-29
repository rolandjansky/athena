/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#define private public
#define protected public
#include "MuonSegment/MuonSegment.h"
#undef private
#undef protected

#include "MuonEventTPCnv/MuonSegment/MuonSegmentCnv_p2.h"
#include "TrkSurfaces/Surface.h"
#include "TrkSurfaces/PlaneSurface.h"
#include "TrkEventCnvTools/DetElementSurface.h"
#include "TrkEventPrimitives/LocalDirection.h"


void MuonSegmentCnv_p2::persToTrans( const Muon::MuonSegment_p1 *persObj, Muon::MuonSegment *transObj, MsgStream &log )
{
  fillTransFromPStore( &m_segmentCnv, persObj->m_segment, transObj, log );

  transObj->m_associatedSurface = static_cast<Trk::PlaneSurface*>(this->createTransFromPStore( &m_surfCnv, persObj->m_associatedSurface, log ));
  m_localDirCnv.persToTrans(&persObj->m_localDirection,&transObj->m_localDirection,log);

  transObj->m_time = persObj->m_t0;
  transObj->m_errorTime = persObj->m_t0Error;
  transObj->recalculateCache();
    //if (persObj->m_localDirection==0)
    //   log<<MSG::WARNING<<"MuonSegment is apparently missing a LocalDirection. Dumping:"<<*transObj
    //	   << "Persistent object has LD_p1 m_angleXZ,m_angleYZ:"<<persObj->m_localDirection.m_angleXZ<<","<<persObj->m_localDirection.m_angleYZ<<endreq;

}


void MuonSegmentCnv_p2::transToPers( const Muon::MuonSegment *transObj, Muon::MuonSegment_p1 *persObj, MsgStream &log )
{
  persObj->m_segment = baseToPersistent( &m_segmentCnv, transObj, log );
  m_localDirCnv.transToPers(&transObj->m_localDirection,&persObj->m_localDirection,log);

    //if (transObj->m_localDirection!=0) 
    //    m_localDirCnv.transToPers(&persObj->m_localDirection,const_cast<Trk::LocalDirection*>(transObj->m_localDirection),log);
    //else
    //    log<<MSG::WARNING<<"MuonSegment is apparently missing a LocalDirection. Dumping:"<<*transObj<<endreq;
  m_surfCnv=0;
  if (transObj->m_associatedSurface->isFree() ) // if this is a free surface, write it out 'as is'
    persObj->m_associatedSurface = toPersistent(&m_surfCnv, transObj->m_associatedSurface, log);
  else { // else, make it into a DetElementSurface, to allow the correct convertor to be called
    Trk::DetElementSurface surf( *(transObj->m_associatedSurface));
    persObj->m_associatedSurface = toPersistent(&m_surfCnv, &surf, log);
  }

  persObj->m_t0 = transObj->m_time;
  persObj->m_t0Error = transObj->m_errorTime;

}
