/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

//-----------------------------------------------------------------------------
//
// file:   PixelClusterCnv_p2.cxx
//
//-----------------------------------------------------------------------------

#include "InDetPrepRawData/PixelCluster.h"
#include "EventPrimitives/EventPrimitives.h"
#include "InDetEventTPCnv/InDetPrepRawData/PixelClusterCnv_p2.h"

#include <memory>

InDet::PixelCluster
PixelClusterCnv_p2::createPixelCluster (const InDet::PixelCluster_p2* persObj,
                                        const InDetDD::SiDetectorElement* detEl,
                                        MsgStream &log)
{
  // Local Position
  Amg::Vector2D localPos;
  localPos[Trk::locX] = persObj->m_localPosX; 
  localPos[Trk::locY] = persObj->m_localPosY;
  
  std::vector<Identifier> rdoList;
  rdoList.resize( persObj->m_rdoList.size() );
  Identifier id (persObj->m_clusId);
  Identifier::value_type id32 = id.get_compact();
  std::vector<Identifier>::iterator tit = rdoList.begin();
  for (const InDet::PixelCluster_p2::rdo_diff_type& rdo : persObj->m_rdoList)
  {
    *tit = Identifier((Identifier::value_type) rdo + id32);
    ++tit;
  }

  InDet::SiWidth width;
  m_swCnv.persToTrans(&persObj->m_width, &width, log);

  // Error matrix
  auto cmat = std::make_unique<Amg::MatrixX>(2,2);
  (*cmat)(0,0) = static_cast<double>(persObj->m_mat00);
  (*cmat)(1,0) = static_cast<double>(persObj->m_mat01);
  (*cmat)(0,1) = static_cast<double>(persObj->m_mat01);
  (*cmat)(1,1) = static_cast<double>(persObj->m_mat11);

  InDet::PixelCluster clus (id,
                            localPos,
                            std::move(rdoList),
                            0, // lvl1a
                            0, // totalToT
                            std::vector<float>(), // chargeList
                            0, // totalCharge
                            width,
                            detEl,
                            std::move(cmat),
                            persObj->m_omX,
                            persObj->m_omY,
                            0 //splitInfo
                            );
  return clus;
}


void PixelClusterCnv_p2::
persToTrans( const InDet::PixelCluster_p2 *persObj, InDet::PixelCluster *transObj, MsgStream &log) 
{
  *transObj = createPixelCluster (persObj, nullptr, log);
}


void PixelClusterCnv_p2::transToPers( const InDet::PixelCluster *transObj, InDet::PixelCluster_p2 *persObj, MsgStream &log )
{
//   log << MSG::VERBOSE << "In PixelCluster::transToPers" << endmsg;
  m_swCnv.transToPers(&transObj->width(), &persObj->m_width, log);

  persObj->m_omX = transObj->omegax();
  persObj->m_omY = transObj->omegay();

  // base class:
  //

  // Local Position
  persObj->m_localPosX = transObj->localPosition()[Trk::locX];
  persObj->m_localPosY = transObj->localPosition()[Trk::locY];

  // Error Matrix
  persObj->m_mat00 = (transObj->localCovariance())(0,0);
  persObj->m_mat01 = (transObj->localCovariance())(0,1);
  persObj->m_mat11 = (transObj->localCovariance())(1,1);

  // List of Id of the cluster
  persObj->m_clusId = transObj->identify().get_compact();
  persObj->m_rdoList.resize( transObj->rdoList().size() );
  // convert the list of ID saved for the cluster
  persObj->m_rdoList.resize(transObj->rdoList().size() );
  
  
  std::vector<InDet::PixelCluster_p2::rdo_diff_type>::iterator pit = persObj->m_rdoList.begin();
  for (const Identifier& id : transObj->rdoList()) {
   *pit = (InDet::PixelCluster_p2::rdo_diff_type)  (id.get_compact()) - persObj->m_clusId  ;
   ++pit;
  }
  
}


