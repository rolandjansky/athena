/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//-----------------------------------------------------------------------------
//
// file:   InDetCompetingRIOsOnTrackCnv_p1.cxx
//
//-----------------------------------------------------------------------------

#define private public
#define protected public
#include "InDetCompetingRIOsOnTrack/CompetingTRT_DriftCirclesOnTrack.h"
#undef private
#undef protected
#include "InDetEventTPCnv/InDetCompetingRIOsOnTrack/CompetingTRT_DriftCirclesOnTrackCnv_p1.h"
#include "InDetRIO_OnTrack/TRT_DriftCircleOnTrack.h"
#include "TrkMeasurementBase/MeasurementBase.h"
#include "TrkRIO_OnTrack/RIO_OnTrack.h"
#include "TrkSurfaces/Surface.h"


void 
CompetingTRT_DriftCirclesOnTrackCnv_p1::persToTrans( const InDet::CompetingTRT_DriftCirclesOnTrack_p1 *persObj,
                                                       InDet::CompetingTRT_DriftCirclesOnTrack *transObj, 
                                                       MsgStream &log )
{
   fillTransFromPStore( &m_cRotCnv, persObj->m_competingROT, transObj, log );
   
   std::vector< TPObjRef >::const_iterator  it = persObj->m_containedChildRots.begin(), 
                                            itE = persObj->m_containedChildRots.end();
                                            
   transObj->m_containedChildRots = new std::vector< const InDet::TRT_DriftCircleOnTrack * >;
   
   for (; it!=itE;it++) {
       ITPConverterFor<Trk::MeasurementBase>  *rotCnv = 0;
       const InDet::TRT_DriftCircleOnTrack* mcot = dynamic_cast<const InDet::TRT_DriftCircleOnTrack*>(createTransFromPStore(&rotCnv, *it, log));
       transObj->m_containedChildRots->push_back( mcot );
   }
   
   ITPConverterFor<Trk::Surface>  *surfaceCnv = 0;
   transObj->m_associatedSurface    = createTransFromPStore(&surfaceCnv, persObj->m_associatedSurface, log);
}

void 
CompetingTRT_DriftCirclesOnTrackCnv_p1::transToPers( const InDet::CompetingTRT_DriftCirclesOnTrack *transObj,
                                                        InDet::CompetingTRT_DriftCirclesOnTrack_p1 *persObj, 
                                                        MsgStream &log ) 
{
    persObj->m_competingROT = baseToPersistent( &m_cRotCnv,  transObj, log );

    std::vector< const InDet::TRT_DriftCircleOnTrack * >::const_iterator it  = transObj->m_containedChildRots->begin(), 
                                                                    itE = transObj->m_containedChildRots->end();
    //transObj->m_containedChildRots = new std::vector< const TRT_DriftCircleOnTrack * >;
    
    for (; it!=itE;it++) {
        ITPConverterFor<Trk::MeasurementBase>  *rotCnv = 0;
        persObj->m_containedChildRots.push_back( toPersistent(&rotCnv, *it, log) );
    }
    
    ITPConverterFor<Trk::Surface>  *surfaceCnv = 0;
    persObj->m_associatedSurface     = toPersistent(&surfaceCnv, transObj->m_associatedSurface, log);
    
}


