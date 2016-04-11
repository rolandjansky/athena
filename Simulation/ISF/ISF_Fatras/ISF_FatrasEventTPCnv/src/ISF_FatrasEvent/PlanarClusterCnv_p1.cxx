/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//-----------------------------------------------------------------------------
//
// file:   PlanarClusterCnv_p1.cxx
//
//-----------------------------------------------------------------------------

#include "ISF_FatrasEvent/PlanarCluster.h"
#include "EventPrimitives/EventPrimitives.h"

#include "ISF_FatrasEventTPCnv/ISF_FatrasEvent/PlanarClusterCnv_p1.h"
#include "InDetIdentifier/PixelID.h"

#include <algorithm>

PlanarClusterCnv_p1::PlanarClusterCnv_p1(const PixelID * pixid )
 :
  m_pixId2(pixid)
{}


iFatras::PlanarCluster
PlanarClusterCnv_p1::createPlanarCluster (const iFatras::PlanarCluster_p1 *persObj,
                                          const Identifier& waferId,
                                          const iFatras::PlanarDetElement* detEl,
                                          MsgStream &log)
{
  InDet::SiWidth width;
  m_swCnv.persToTrans(&persObj->m_width, &width, log);

  // Local Position
  Amg::Vector2D localPos;
  localPos[Trk::locX] = persObj->m_localPosX; 
  localPos[Trk::locY] = persObj->m_localPosY;

  // Error matrix
  Amg::MatrixX* cmat = new  Amg::MatrixX(2,2);
  (*cmat)(0,0) = static_cast<double>(persObj->m_mat00);
  (*cmat)(1,0) = static_cast<double>(persObj->m_mat01);
  (*cmat)(0,1) = static_cast<double>(persObj->m_mat01);
  (*cmat)(1,1) = static_cast<double>(persObj->m_mat11);

  // List of Id of the cluster

  //  if persistent m_clusId == 0, we are reading in 64 bit ids and
  //  need to extract the cluster id from the first id in the
  //  m_rdoList
  Identifier clusId;
  bool is64BitId = false;
  unsigned int rdoListSize = persObj->m_rdoList.size();
  if (persObj->m_clusId == 0) {
      // 64-bit ids, get cluster id - wafer id has been stored in the
      // transient object by the container cnv
      clusId = Identifier(m_pixId2->pixel_id_offset(waferId, persObj->m_rdoList[0]) );
      is64BitId = true;
      rdoListSize--; 
  }
  else {
      // Must also fix cluster pixel id when reading in old 32-bit ids
      // and the code has 64-bit ids
      clusId =  m_pixId2->pixel_id_from_shortened(persObj->m_clusId);
  }

  std::vector<Identifier> rdoList;
  rdoList.reserve (rdoListSize);
  std::vector<iFatras::PlanarCluster_p1::rdo_diff_type>::const_iterator it = persObj->m_rdoList.begin(); 
  if (is64BitId) ++it; // we have used first element of persistent rdoList for cluster id
  for (; it != persObj->m_rdoList.end(); ++it)  {
      // When reading in data written for 32-bit ids, we need to use
      // the PlanarID helper to convert it if filling a 64-bit id
      // (i.e. using pixel_id_from_shortened). This does nothing if
      // the identifiers are still 32-bit  RDS
    if (is64BitId) {
      rdoList.emplace_back (clusId.get_compact() + ( (static_cast<Identifier::value_type>(*it)) << m_pixId2->base_bit() ) );
    } 
    else {
      rdoList.push_back (m_pixId2->pixel_id_from_shortened(persObj->m_clusId + (*it)));
    }
  }

  return iFatras::PlanarCluster (clusId,
                                 localPos,
                                 rdoList,
                                 width,
                                 detEl,
                                 cmat);
}

void PlanarClusterCnv_p1::
persToTrans( const iFatras::PlanarCluster_p1 *persObj, iFatras::PlanarCluster *transObj, MsgStream &log) 
{
  *transObj = createPlanarCluster (persObj,
                                   transObj->identify(),
                                   transObj->detectorElement(),
                                   log);
}//end of cnv_p1 method


void PlanarClusterCnv_p1::transToPers( const iFatras::PlanarCluster *transObj, iFatras::PlanarCluster_p1 *persObj, MsgStream &log )
{
  m_swCnv.transToPers(&transObj->width(), &persObj->m_width, log);

  // base class:
  //

  // Local Position
  persObj->m_localPosX = transObj->localPosition()[Trk::locX];
  persObj->m_localPosY = transObj->localPosition()[Trk::locY];

  // Error Matrix
  
  persObj->m_mat00 = transObj->localCovariance()(0,0);
  persObj->m_mat01 = transObj->localCovariance()(0,1);
  persObj->m_mat11 = transObj->localCovariance()(1,1);

  // List of Id of the cluster

  //   Save cluster id explicitly in pers cluster m_clusId for 32-bit
  //   ids. For 64-bit ids, we set m_clusId to 0 and instead save the
  //   diff in m_rdoList as the first element
  persObj->m_rdoList.resize(transObj->rdoList().size() + 1);

  std::vector<iFatras::PlanarCluster_p1::rdo_diff_type>::iterator pit = persObj->m_rdoList.begin();
  // Get base identifier to calculate diffs
  Identifier clusId = transObj->identify();
  persObj->m_clusId = 0;
  // Save offset to wafer id for the cluster id
  Identifier waferId =  m_pixId2->wafer_id(clusId); // remove the eta/phi of channel id
  *pit = static_cast<iFatras::PlanarCluster_p1::rdo_diff_type>( m_pixId2->calc_offset(waferId, clusId) );
  pit++;

  for (const Identifier& id : transObj->rdoList()) {
    *pit = static_cast<iFatras::PlanarCluster_p1::rdo_diff_type>( m_pixId2->calc_offset(clusId, id) );
    pit++;
  }

}


