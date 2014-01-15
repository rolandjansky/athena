/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//-----------------------------------------------------------------------------
//
// file:   PixelClusterCnv_p2.cxx
//
//-----------------------------------------------------------------------------

#define private public
#define protected public
#include "InDetPrepRawData/PixelCluster.h"
#undef private
#undef protected
#include "EventPrimitives/EventPrimitives.h"

#include "InDetEventTPCnv/InDetPrepRawData/PixelClusterCnv_p2.h"


void PixelClusterCnv_p2::
persToTrans( const InDet::PixelCluster_p2 *persObj, InDet::PixelCluster *transObj, MsgStream &log) 
{
//   log << MSG::VERBOSE << "In PixelCluster::persToTrans" << endreq;
  m_swCnv.persToTrans(&persObj->m_width, &transObj->m_width, log);

  transObj->m_omegax = persObj->m_omX;
  transObj->m_omegay = persObj->m_omY;
  
  // Base class elements:
  //
  
  // Local Position
  transObj->m_localPos[Trk::locX] = persObj->m_localPosX; 
  transObj->m_localPos[Trk::locY] = persObj->m_localPosY;
  
  // Error matrix
  Amg::MatrixX* cmat = new  Amg::MatrixX(2,2);
  (*cmat)(0,0) = static_cast<double>(persObj->m_mat00);
  (*cmat)(1,0) = static_cast<double>(persObj->m_mat01);
  (*cmat)(0,1) = static_cast<double>(persObj->m_mat01);
  (*cmat)(1,1) = static_cast<double>(persObj->m_mat11);
  transObj->m_localCovariance     = cmat;

// List of Id of the cluster
  transObj->m_rdoList.resize( persObj->m_rdoList.size() );
  transObj->m_clusId = persObj->m_clusId;
 // unsigned int id32 = (unsigned int) transObj->identify();
  Identifier::value_type id32 = transObj->identify().get_compact();
  std::vector<Identifier>::iterator tit = transObj->m_rdoList.begin();
//  for (std::vector<short>::const_iterator it=persObj->m_rdoList.begin(); it != persObj->m_rdoList.end(); it++) {
//    *tit = Identifier((unsigned int) *it+id32);

  for(std::vector<InDet::PixelCluster_p2::rdo_diff_type>::const_iterator it = persObj->m_rdoList.begin(); 
                                                            it != persObj->m_rdoList.end();it++)
  {
    *tit = Identifier((Identifier::value_type) *it + id32);
    tit++;
  }
  
}


void PixelClusterCnv_p2::transToPers( const InDet::PixelCluster *transObj, InDet::PixelCluster_p2 *persObj, MsgStream &log )
{
//   log << MSG::VERBOSE << "In PixelCluster::transToPers" << endreq;
  m_swCnv.transToPers(&transObj->m_width, &persObj->m_width, log);

  persObj->m_omX = transObj->m_omegax;
  persObj->m_omY = transObj->m_omegay;

  // base class:
  //

  // Local Position
  persObj->m_localPosX = transObj->m_localPos[Trk::locX];
  persObj->m_localPosY = transObj->m_localPos[Trk::locY];

  // Error Matrix
  persObj->m_mat00 = (*transObj->m_localCovariance)(0,0);
  persObj->m_mat01 = (*transObj->m_localCovariance)(0,1);
  persObj->m_mat11 = (*transObj->m_localCovariance)(1,1);

  // List of Id of the cluster
  persObj->m_clusId = transObj->m_clusId.get_compact();
  persObj->m_rdoList.resize( transObj->m_rdoList.size() );
  // convert the list of ID saved for the cluster
  persObj->m_rdoList.resize(transObj->m_rdoList.size() );
  
  
  std::vector<InDet::PixelCluster_p2::rdo_diff_type>::iterator pit = persObj->m_rdoList.begin();
  for (std::vector<Identifier>::const_iterator it=transObj->m_rdoList.begin(); it != transObj->m_rdoList.end(); it++) {
  
   *pit = (InDet::PixelCluster_p2::rdo_diff_type)  (it->get_compact()) - persObj->m_clusId  ;
    pit++;
  }
  
}


