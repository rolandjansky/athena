/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

//-----------------------------------------------------------------------------
//
// file:   InDetCompetingRIOsOnTrackCnv_p1.cxx
//
//-----------------------------------------------------------------------------

#include "InDetCompetingRIOsOnTrack/CompetingTRT_DriftCirclesOnTrack.h"
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
   auto containedChildRots = new std::vector< const InDet::TRT_DriftCircleOnTrack * >;
   
   for (const TPObjRef& ref : persObj->m_containedChildRots) {
       ITPConverterFor<Trk::MeasurementBase>  *rotCnv = 0;
       const InDet::TRT_DriftCircleOnTrack* mcot = dynamic_cast<const InDet::TRT_DriftCircleOnTrack*>(createTransFromPStore(&rotCnv, ref, log));
       containedChildRots->push_back( mcot );
   }
   
   ITPConverterFor<Trk::Surface>  *surfaceCnv = 0;
   Trk::Surface* associatedSurface    = createTransFromPStore(&surfaceCnv, persObj->m_associatedSurface, log);

   *transObj = InDet::CompetingTRT_DriftCirclesOnTrack (associatedSurface,
                                                        containedChildRots,
                                                        nullptr,
                                                        nullptr,
                                                        nullptr,
                                                        0);

   fillTransFromPStore( &m_cRotCnv, persObj->m_competingROT, transObj, log );
}

void 
CompetingTRT_DriftCirclesOnTrackCnv_p1::transToPers( const InDet::CompetingTRT_DriftCirclesOnTrack *transObj,
                                                        InDet::CompetingTRT_DriftCirclesOnTrack_p1 *persObj, 
                                                        MsgStream &log ) 
{
    persObj->m_competingROT = baseToPersistent( &m_cRotCnv,  transObj, log );

    for (const InDet::TRT_DriftCircleOnTrack* cl : transObj->containedROTs()) {
        ITPConverterFor<Trk::MeasurementBase>  *rotCnv = 0;
        persObj->m_containedChildRots.push_back( toPersistent(&rotCnv, cl, log) );
    }
    
    ITPConverterFor<Trk::Surface>  *surfaceCnv = 0;
    persObj->m_associatedSurface     = toPersistent(&surfaceCnv, &transObj->associatedSurface(), log);
    
}


