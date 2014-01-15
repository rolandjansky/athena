/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#define private public
#define protected public
#include "InDetCompetingRIOsOnTrack/CompetingSCT_ClustersOnTrack.h"
#undef private
#undef protected
#include "InDetEventTPCnv/InDetCompetingRIOsOnTrack/CompetingSCT_ClustersOnTrackCnv_p1.h"
#include "InDetRIO_OnTrack/SCT_ClusterOnTrack.h"
#include "TrkMeasurementBase/MeasurementBase.h"
#include "TrkRIO_OnTrack/RIO_OnTrack.h"

void 
CompetingSCT_ClustersOnTrackCnv_p1::persToTrans( const InDet::CompetingSCT_ClustersOnTrack_p1 *persObj,
                                                       InDet::CompetingSCT_ClustersOnTrack *transObj, 
                                                       MsgStream &log )
{
   fillTransFromPStore( &m_cRotCnv, persObj->m_competingROT, transObj, log );
   
   std::vector< TPObjRef >::const_iterator  it = persObj->m_containedChildRots.begin(), 
                                            itE = persObj->m_containedChildRots.end();
                                            
   transObj->m_containedChildRots = new std::vector< const InDet::SCT_ClusterOnTrack * >;
   
   for (; it!=itE;it++) {
       ITPConverterFor<Trk::MeasurementBase>  *rotCnv = 0;
       const InDet::SCT_ClusterOnTrack* mcot = dynamic_cast<const InDet::SCT_ClusterOnTrack*>(createTransFromPStore(&rotCnv, *it, log));
       transObj->m_containedChildRots->push_back( mcot );
   }
}

void 
CompetingSCT_ClustersOnTrackCnv_p1::transToPers( const InDet::CompetingSCT_ClustersOnTrack *transObj,
                                                        InDet::CompetingSCT_ClustersOnTrack_p1 *persObj, 
                                                        MsgStream &log ) 
{
    persObj->m_competingROT = baseToPersistent( &m_cRotCnv,  transObj, log );

    std::vector< const InDet::SCT_ClusterOnTrack * >::const_iterator   it  = transObj->m_containedChildRots->begin(), 
                                                                        itE = transObj->m_containedChildRots->end();    
    for (; it!=itE;it++) {
        ITPConverterFor<Trk::MeasurementBase>  *rotCnv = 0;
        persObj->m_containedChildRots.push_back( toPersistent(&rotCnv, *it, log) );
    }
}


