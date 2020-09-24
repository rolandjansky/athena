/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

//-----------------------------------------------------------------------------
//
// file:   PixelClusterCnv_p3.cxx
//
//-----------------------------------------------------------------------------

#include "InDetPrepRawData/PixelCluster.h"
#include "EventPrimitives/EventPrimitives.h"

#include "InDetEventTPCnv/InDetPrepRawData/PixelClusterCnv_p3.h"
#include "InDetIdentifier/PixelID.h"

#include <memory>
#include <algorithm>



PixelClusterCnv_p3::PixelClusterCnv_p3(const PixelID * pixid )
 :
  m_pixId2(pixid)
{}


InDet::PixelCluster
PixelClusterCnv_p3::createPixelCluster (const InDet::PixelCluster_p3* persObj,
                                        const Identifier& waferId,
                                        const InDetDD::SiDetectorElement* detEl,
                                        MsgStream &log)
{
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

  // Local Position
  Amg::Vector2D localPos;
  localPos[Trk::locX] = persObj->m_localPosX; 
  localPos[Trk::locY] = persObj->m_localPosY;

  std::vector<Identifier> rdoList;
  rdoList.resize(rdoListSize);
  std::vector<Identifier>::iterator idit = rdoList.begin();
  std::vector<InDet::PixelCluster_p3::rdo_diff_type>::const_iterator it = persObj->m_rdoList.begin(); 
  if (is64BitId) ++it; // we have used first element of persistent rdoList for cluster id
  for (; it != persObj->m_rdoList.end(); ++it)  {
      // When reading in data written for 32-bit ids, we need to use
      // the PixelID helper to convert it if filling a 64-bit id
      // (i.e. using pixel_id_from_shortened). This does nothing if
      // the identifiers are still 32-bit  RDS
    if (is64BitId) {
      *idit =  Identifier(clusId.get_compact() + ( (static_cast<Identifier::value_type>(*it)) << m_pixId2->base_bit() ) );
    } 
    else {
      (*idit) =  m_pixId2->pixel_id_from_shortened(persObj->m_clusId + (*it));
    }
      ++idit;
  }

  InDet::SiWidth width;
  m_swCnv.persToTrans(&persObj->m_width, &width, log);

  // Error matrix
  auto cmat = std::make_unique<Amg::MatrixX>(2,2);
  (*cmat)(0,0) = static_cast<double>(persObj->m_mat00);
  (*cmat)(1,0) = static_cast<double>(persObj->m_mat01);
  (*cmat)(0,1) = static_cast<double>(persObj->m_mat01);
  (*cmat)(1,1) = static_cast<double>(persObj->m_mat11);


  // Check if charge or ToT has been stored
  int totalToT = 0;
  float totalCharge = 0;
  if ( (persObj->m_properties&(1<<27))!=0 ) 
    totalCharge = (float)(persObj->m_properties & 0xFFFFFF); 
  else 
    totalToT    = (persObj->m_properties & 0xFFFFFF); 

  InDet::PixelCluster clus (clusId,
                            localPos,
                            std::move(rdoList),
                            ((persObj->m_properties)>>28) & 0xF, // lvl1a
                            totalToT,
                            persObj->m_chargeList,
                            totalCharge,
                            width,
                            detEl,
                            std::move(cmat),
                            persObj->m_omegax,
                            persObj->m_omegay,
                            persObj->m_splitInfo
                            );

  clus.setGangedPixel( (persObj->m_properties&(1<<24))!=0 );
  clus.setFake( (persObj->m_properties&(1<<25))!=0 );
  clus.setAmbiguous( (persObj->m_properties&(1<<26))!=0 );
  return clus;
}


void PixelClusterCnv_p3::
persToTrans( const InDet::PixelCluster_p3 *persObj, InDet::PixelCluster *transObj, MsgStream &log) 
{
  Identifier waferId = transObj->identify();
  *transObj = createPixelCluster (persObj, waferId, nullptr, log);
}//end of cnv_p3 method


void PixelClusterCnv_p3::transToPers( const InDet::PixelCluster *transObj, InDet::PixelCluster_p3 *persObj, MsgStream &log )
{
//   log << MSG::VERBOSE << "In PixelCluster::transToPers" << endmsg;
  m_swCnv.transToPers(&transObj->width(), &persObj->m_width, log);

  // base class:
  //

  // Local Position
  persObj->m_localPosX = transObj->localPosition()[Trk::locX];
  persObj->m_localPosY = transObj->localPosition()[Trk::locY];

  // Error Matrix
  
  persObj->m_mat00 = (transObj->localCovariance())(0,0);
  persObj->m_mat01 = (transObj->localCovariance())(0,1);
  persObj->m_mat11 = (transObj->localCovariance())(1,1);

  // Charge balance
  persObj->m_omegax = transObj->omegax();
  persObj->m_omegay = transObj->omegay();

  // List of Id of the cluster

  //   For 64-bit ids, we set m_clusId to 0 and instead save the
  //   diff in m_rdoList as the first element
  persObj->m_rdoList.resize(transObj->rdoList().size() + 1);

  std::vector<InDet::PixelCluster_p3::rdo_diff_type>::iterator pit = persObj->m_rdoList.begin();
  // Get base identifier to calculate diffs
  Identifier clusId = transObj->identify();
  persObj->m_clusId = 0;
  // Save offset to wafer id for the cluster id
  Identifier waferId =  m_pixId2->wafer_id(clusId); // remove the eta/phi of channel id
  *pit = static_cast<InDet::PixelCluster_p3::rdo_diff_type>( m_pixId2->calc_offset(waferId, clusId) );
  ++pit;
  
  for (const Identifier& id : transObj->rdoList()) {
    *pit = static_cast<InDet::PixelCluster_p3::rdo_diff_type>( m_pixId2->calc_offset(clusId, id) );
    ++pit;
  }

  // Setting of cluster properties:
  persObj->m_properties = 0;
  if ( transObj->gangedPixel() )  persObj->m_properties |= (1<<24);
  if ( transObj->isFake()      )  persObj->m_properties |= (1<<25);
  if ( transObj->isAmbiguous() )  persObj->m_properties |= (1<<26);
  persObj->m_properties |= ((transObj->LVL1A() & 0xF)<<28);

  if ( transObj->totalCharge()>0. ) {
    persObj->m_properties |= (1<<27);
    persObj->m_properties |= ( (int)(transObj->totalCharge()+0.5) & 0xFFFFFF );
  } else {
    persObj->m_properties |= ( transObj->totalToT() & 0xFFFFFF );
  }
  // needed for pixel cluster splitting
  persObj->m_splitInfo    = transObj->splitInfoRaw();
  persObj->m_chargeList   = transObj->chargeList();
}


