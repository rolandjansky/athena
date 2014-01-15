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
#include "InDetCompetingRIOsOnTrack/CompetingPixelClustersOnTrack.h"
#undef private
#undef protected
#include "InDetEventTPCnv/InDetCompetingRIOsOnTrack/CompetingPixelClustersOnTrackCnv_p1.h"
#include "InDetRIO_OnTrack/PixelClusterOnTrack.h"
#include "TrkMeasurementBase/MeasurementBase.h"
#include "TrkRIO_OnTrack/RIO_OnTrack.h"

void 
CompetingPixelClustersOnTrackCnv_p1::persToTrans( const InDet::CompetingPixelClustersOnTrack_p1 *persObj,
                                                       InDet::CompetingPixelClustersOnTrack *transObj, 
                                                       MsgStream &log )
{
   fillTransFromPStore( &m_cRotCnv, persObj->m_competingROT, transObj, log );
   
   std::vector< TPObjRef >::const_iterator  it = persObj->m_containedChildRots.begin(), 
                                            itE = persObj->m_containedChildRots.end();
                                            
   transObj->m_containedChildRots = new std::vector< const InDet::PixelClusterOnTrack * >;
   
   for (; it!=itE;it++) {
       ITPConverterFor<Trk::MeasurementBase>  *rotCnv = 0;
       const InDet::PixelClusterOnTrack* mcot = dynamic_cast<const InDet::PixelClusterOnTrack*>(createTransFromPStore(&rotCnv, *it, log));
       transObj->m_containedChildRots->push_back( mcot );
   }
}

void 
CompetingPixelClustersOnTrackCnv_p1::transToPers( const InDet::CompetingPixelClustersOnTrack *transObj,
                                                        InDet::CompetingPixelClustersOnTrack_p1 *persObj, 
                                                        MsgStream &log ) 
{
    persObj->m_competingROT = baseToPersistent( &m_cRotCnv,  transObj, log );

    std::vector< const InDet::PixelClusterOnTrack * >::const_iterator it  = transObj->m_containedChildRots->begin(), 
                                                                    itE = transObj->m_containedChildRots->end();
    //transObj->m_containedChildRots = new std::vector< const PixelClusterOnTrack * >;
    
    for (; it!=itE;it++) {
        ITPConverterFor<Trk::MeasurementBase>  *rotCnv = 0;
        persObj->m_containedChildRots.push_back( toPersistent(&rotCnv, *it, log) );
    }
}


