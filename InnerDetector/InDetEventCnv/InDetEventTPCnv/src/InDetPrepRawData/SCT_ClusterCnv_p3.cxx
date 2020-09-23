/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

//-----------------------------------------------------------------------------
//
// file:   SCT_ClusterCnv_p3.cxx
//
//-----------------------------------------------------------------------------

#include "InDetPrepRawData/SCT_Cluster.h"

#include "EventPrimitives/EventPrimitives.h"

#include "InDetEventTPCnv/InDetPrepRawData/SCT_ClusterCnv_p3.h"

#include <memory>

SCT_ClusterCnv_p3::SCT_ClusterCnv_p3(const SCT_ID * sctid )
 :
  m_sctId2(sctid)
{}


InDet::SCT_Cluster
SCT_ClusterCnv_p3::createSCT_Cluster (const InDet::SCT_Cluster_p3* persObj,
                                      Identifier clusId,
                                      const InDetDD::SiDetectorElement* detEl,
                                      MsgStream& log)
{
  // Local Position
  Amg::Vector2D localPos;
  localPos[Trk::locX] = persObj->m_localPos; 
  localPos[Trk::locY] = 0;

  // List of Id of the cluster
  std::vector<Identifier> rdoList;
  rdoList.resize( persObj->m_rdoList.size() );
  //Identifier::value_type id32 = transObj->identify().get_compact(); 
  //Identifier id32 = transObj->identify(); 
  std::vector<Identifier>::iterator tit = rdoList.begin();
  for (const InDet::SCT_Cluster_p3::rdo_diff_type& rdo : persObj->m_rdoList) {
    *tit = Identifier(m_sctId2->strip_id_offset(clusId,rdo) );
    ++tit;
  }
  
  InDet::SiWidth sw;
  m_swCnv.persToTrans(&persObj->m_width, &sw, log);

  // Error matrix
  auto cmat = std::make_unique<Amg::MatrixX>(2,2);
  (*cmat)(0,0) = static_cast<double>(persObj->m_mat00);
  (*cmat)(1,0) = static_cast<double>(persObj->m_mat01);
  (*cmat)(0,1) = static_cast<double>(persObj->m_mat01);
  (*cmat)(1,1) = static_cast<double>(persObj->m_mat11);

  InDet::SCT_Cluster clus (clusId,
                           localPos,
                           std::move(rdoList),
                           sw,
                           detEl,
                           std::move(cmat));
  clus.setHitsInThirdTimeBin( persObj->m_hitsInThirdTimeBin ) ;
  return clus;
}


void SCT_ClusterCnv_p3::
persToTrans( const InDet::SCT_Cluster_p3 *persObj, InDet::SCT_Cluster *transObj, MsgStream &log) 
{
  Identifier clusId = transObj->identify();
  *transObj = createSCT_Cluster (persObj, clusId, nullptr, log);
}


void SCT_ClusterCnv_p3::transToPers( const InDet::SCT_Cluster *transObj, InDet::SCT_Cluster_p3 *persObj, MsgStream &log )
{
//   if (log.level() <= MSG::VERBOSE) log << MSG::VERBOSE << "In SCT_Cluster::transToPers" << endmsg;
  const InDet::SiWidth *sw = &transObj->width();
  m_swCnv.transToPers(sw, &persObj->m_width, log);

  // base class:
  //

  // Local Position
  persObj->m_localPos = transObj->localPosition()[Trk::locX];
  
  // cluster weight
  persObj->m_hitsInThirdTimeBin = transObj->hitsInThirdTimeBin();
  

  // Error Matrix
  persObj->m_mat00 = (transObj->localCovariance())(0,0);
  persObj->m_mat01 = (transObj->localCovariance())(0,1);
  persObj->m_mat11 = (transObj->localCovariance())(1,1);

  // List of Id of the cluster
  persObj->m_rdoList.resize( transObj->rdoList().size() );
  //Identifier::value_type id32 = transObj->identify().get_compact();
  Identifier id32 = transObj->identify();
  
  
  // convert the list of ID saved for the cluster
  persObj->m_rdoList.resize(transObj->rdoList().size() );
  std::vector<InDet::SCT_Cluster_p3::rdo_diff_type>::iterator pit = persObj->m_rdoList.begin(); 
  
  
  for (const Identifier& id : transObj->rdoList()) {
    *pit = static_cast<InDet::SCT_Cluster_p3::rdo_diff_type>( m_sctId2->calc_offset(id32, id) );
    ++pit;
  }
  
}


