/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonSegment/MuonSegment.h"
#include "MuonEventTPCnv/MuonSegment/MuonSegmentCnv_p1.h"
#include "TrkSurfaces/Surface.h"
#include "TrkSurfaces/PlaneSurface.h"
#include "TrkEventCnvTools/DetElementSurface.h"
#include "TrkEventPrimitives/LocalDirection.h"


void MuonSegmentCnv_p1::persToTrans( const Muon::MuonSegment_p1 *persObj, Muon::MuonSegment *transObj, MsgStream &log )
{
    Amg::Vector2D locSegPos;
    locSegPos.setZero();

    Trk::LocalDirection localDirection;
    m_localDirCnv.persToTrans(&persObj->m_localDirection,&localDirection,log);

    ITPConverterFor<Trk::Surface> *associatedSurfaceCnv(0);
    Trk::Surface* temp = createTransFromPStore( &associatedSurfaceCnv, persObj->m_associatedSurface, log ); 
    Trk::PlaneSurface* associatedSurface = 
      dynamic_cast<Trk::PlaneSurface*>(temp);
    if(!associatedSurface) throw std::runtime_error( "MuonSegmentCnv_p1::persToTrans : Cannot get associated surface!" );


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


void MuonSegmentCnv_p1::transToPers( const Muon::MuonSegment */*transObj*/, Muon::MuonSegment_p1 */*persObj*/, MsgStream &/*log*/ )
{
  throw std::runtime_error( "MuonSegmentCnv_p1::transToPers is deprecated!" );
}
