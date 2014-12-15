/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#define private public
#define protected public
#include "MuonSegment/MuonSegment.h"
#undef private
#undef protected

#include "MuonEventTPCnv/MuonSegment/MuonSegmentCnv_p1.h"
#include "TrkSurfaces/Surface.h"
#include "TrkSurfaces/PlaneSurface.h"
#include "TrkEventCnvTools/DetElementSurface.h"
#include "TrkEventPrimitives/LocalDirection.h"


void MuonSegmentCnv_p1::persToTrans( const Muon::MuonSegment_p1 *persObj, Muon::MuonSegment *transObj, MsgStream &log )
{
    fillTransFromPStore( &m_segmentCnv, persObj->m_segment, transObj, log );
    ITPConverterFor<Trk::Surface> *associatedSurfaceCnv(0);
    const Trk::Surface* temp = createTransFromPStore( &associatedSurfaceCnv, persObj->m_associatedSurface, log ); 
    transObj->m_associatedSurface = dynamic_cast<const Trk::PlaneSurface*>(temp);
    if(!transObj->m_associatedSurface) throw std::runtime_error( "MuonSegmentCnv_p1::persToTrans : Cannot get associated surface!" );
    m_localDirCnv.persToTrans(&persObj->m_localDirection,&transObj->m_localDirection,log);

    transObj->m_time = persObj->m_t0;
    transObj->m_errorTime = persObj->m_t0Error;
    transObj->recalculateCache();
    //if (persObj->m_localDirection==0)
    //   log<<MSG::WARNING<<"MuonSegment is apparently missing a LocalDirection. Dumping:"<<*transObj
    //	   << "Persistent object has LD_p1 m_angleXZ,m_angleYZ:"<<persObj->m_localDirection.m_angleXZ<<","<<persObj->m_localDirection.m_angleYZ<<endreq;
 
}


void MuonSegmentCnv_p1::transToPers( const Muon::MuonSegment */*transObj*/, Muon::MuonSegment_p1 */*persObj*/, MsgStream &/*log*/ )
{
  throw std::runtime_error( "MuonSegmentCnv_p1::transToPers is deprecated!" );
}
