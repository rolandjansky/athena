/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

//-----------------------------------------------------------------------------
//
// file:   InDetCompetingRIOsOnTrackCnv_p1.cxx
//
//-----------------------------------------------------------------------------

#include "InDetCompetingRIOsOnTrack/CompetingPixelClustersOnTrack.h"
#include "InDetEventTPCnv/InDetCompetingRIOsOnTrack/CompetingPixelClustersOnTrackCnv_p1.h"
#include "InDetRIO_OnTrack/PixelClusterOnTrack.h"
#include "TrkMeasurementBase/MeasurementBase.h"
#include "TrkRIO_OnTrack/RIO_OnTrack.h"

void 
CompetingPixelClustersOnTrackCnv_p1::persToTrans( const InDet::CompetingPixelClustersOnTrack_p1 *persObj,
                                                       InDet::CompetingPixelClustersOnTrack *transObj, 
                                                       MsgStream &log )
{
   auto containedChildRots = new std::vector< const InDet::PixelClusterOnTrack * >;
   
   for (const TPObjRef& ref : persObj->m_containedChildRots) {
       ITPConverterFor<Trk::MeasurementBase>  *rotCnv = 0;
       const InDet::PixelClusterOnTrack* mcot = dynamic_cast<const InDet::PixelClusterOnTrack*>(createTransFromPStore(&rotCnv, ref, log));
       containedChildRots->push_back( mcot );
   }

   *transObj = InDet::CompetingPixelClustersOnTrack (containedChildRots,
                                                     nullptr);
   fillTransFromPStore( &m_cRotCnv, persObj->m_competingROT, transObj, log );
}

void 
CompetingPixelClustersOnTrackCnv_p1::transToPers( const InDet::CompetingPixelClustersOnTrack *transObj,
                                                        InDet::CompetingPixelClustersOnTrack_p1 *persObj, 
                                                        MsgStream &log ) 
{
    persObj->m_competingROT = baseToPersistent( &m_cRotCnv,  transObj, log );

    for (const InDet::PixelClusterOnTrack* cl : transObj->containedROTs()) {
        ITPConverterFor<Trk::MeasurementBase>  *rotCnv = 0;
        persObj->m_containedChildRots.push_back( toPersistent(&rotCnv, cl, log) );
    }
}


