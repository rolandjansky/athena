/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//-----------------------------------------------------------------------------
//
// file:   SCT_ClusterCnv_p2.cxx
//
//-----------------------------------------------------------------------------

#define private public
#define protected public
#include "InDetPrepRawData/SCT_Cluster.h"
#undef private
#undef protected

#include "EventPrimitives/EventPrimitives.h"

#include "InDetEventTPCnv/InDetPrepRawData/SCT_ClusterCnv_p2.h"

SCT_ClusterCnv_p2::SCT_ClusterCnv_p2(const SCT_ID * sctid )
 :
  m_sctId2(sctid)
{}


void SCT_ClusterCnv_p2::
persToTrans( const InDet::SCT_Cluster_p2 *persObj, InDet::SCT_Cluster *transObj, MsgStream &log) 
{
//   if (log.level() <= MSG::VERBOSE) log << MSG::VERBOSE << "In SCT_Cluster::persToTrans" << endreq;
  InDet::SiWidth *sw = new InDet::SiWidth();
  m_swCnv.persToTrans(&persObj->m_width, sw, log);
  transObj->m_width=*sw;
  delete sw;
  // Base class elements:
  //

  // Local Position
  transObj->m_localPos[Trk::locX] = persObj->m_localPos; 
  transObj->m_localPos[Trk::locY] = 0.0;

  // Error matrix
  Amg::MatrixX* cmat = new  Amg::MatrixX(2,2);
  (*cmat)(0,0) = static_cast<double>(persObj->m_mat00);
  (*cmat)(1,0) = static_cast<double>(persObj->m_mat01);
  (*cmat)(0,1) = static_cast<double>(persObj->m_mat01);
  (*cmat)(1,1) = static_cast<double>(persObj->m_mat11);
  transObj->m_localCovariance     = cmat;

// List of Id of the cluster
  transObj->m_rdoList.resize( persObj->m_rdoList.size() );
  //Identifier::value_type id32 = transObj->identify().get_compact(); 
  Identifier id32 = transObj->identify(); 
  std::vector<Identifier>::iterator tit = transObj->m_rdoList.begin();
  for (std::vector<InDet::SCT_Cluster_p2::rdo_diff_type>::const_iterator it=persObj->m_rdoList.begin(); it != persObj->m_rdoList.end(); it++) {
 
    //*tit = Identifier( *it+id32);
    *tit = Identifier(m_sctId2->strip_id_offset(id32,*it) );
    tit++;
  }
  
}


void SCT_ClusterCnv_p2::transToPers( const InDet::SCT_Cluster *transObj, InDet::SCT_Cluster_p2 *persObj, MsgStream &log )
{
//   if (log.level() <= MSG::VERBOSE) log << MSG::VERBOSE << "In SCT_Cluster::transToPers" << endreq;
  const InDet::SiWidth *sw = &transObj->m_width;
  m_swCnv.transToPers(sw, &persObj->m_width, log);

  // base class:
  //

  // Local Position
  persObj->m_localPos = transObj->m_localPos[Trk::locX];

  // Error Matrix
  persObj->m_mat00 = (*transObj->m_localCovariance)(0,0);
  persObj->m_mat01 = (*transObj->m_localCovariance)(0,1);
  persObj->m_mat11 = (*transObj->m_localCovariance)(1,1);

  // List of Id of the cluster
  persObj->m_rdoList.resize( transObj->m_rdoList.size() );
  //Identifier::value_type id32 = transObj->identify().get_compact();
  Identifier id32 = transObj->identify();
  
  
  // convert the list of ID saved for the cluster
  persObj->m_rdoList.resize(transObj->m_rdoList.size() );
  std::vector<InDet::SCT_Cluster_p2::rdo_diff_type>::iterator pit = persObj->m_rdoList.begin(); 
  
  
  for (std::vector<Identifier>::const_iterator it=transObj->m_rdoList.begin(); it != transObj->m_rdoList.end(); it++) {
    //*pit = static_cast<InDet::SCT_Cluster_p2::rdo_diff_type>(it->get_compact() - id32);
    *pit = static_cast<InDet::SCT_Cluster_p2::rdo_diff_type>( m_sctId2->calc_offset(id32, *it) );
    pit++;
  }
  
}


