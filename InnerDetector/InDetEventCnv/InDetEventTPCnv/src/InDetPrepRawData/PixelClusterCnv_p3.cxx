/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//-----------------------------------------------------------------------------
//
// file:   PixelClusterCnv_p3.cxx
//
//-----------------------------------------------------------------------------

#define private public
#define protected public
#include "InDetPrepRawData/PixelCluster.h"
#undef private
#undef protected

#include "EventPrimitives/EventPrimitives.h"

#include "InDetEventTPCnv/InDetPrepRawData/PixelClusterCnv_p3.h"
#include "InDetIdentifier/PixelID.h"

#include <algorithm>

PixelClusterCnv_p3::PixelClusterCnv_p3(const PixelID * pixid )
 :
  m_pixId2(pixid)
{}

void PixelClusterCnv_p3::
persToTrans( const InDet::PixelCluster_p3 *persObj, InDet::PixelCluster *transObj, MsgStream &log) 
{

//  std::cout<<"PixelClusterCnv_p3 called"<<std::endl;
//   log << MSG::VERBOSE << "In PixelCluster::persToTrans" << endreq;
  m_swCnv.persToTrans(&persObj->m_width, &transObj->m_width, log);

  // Base class elements:
  //

  transObj->m_gangedPixel = ( (persObj->m_properties&(1<<24))!=0 );
  transObj->m_fake        = ( (persObj->m_properties&(1<<25))!=0 );
  transObj->m_ambiguous   = ( (persObj->m_properties&(1<<26))!=0 );
  transObj->m_lvl1        = ( ((persObj->m_properties)>>28) & 0xF );
  // Check if charge or ToT has been stored
  if ( (persObj->m_properties&(1<<27))!=0 ) 
    transObj->m_totalCharge = (float)(persObj->m_properties & 0xFFFFFF); 
  else 
    transObj->m_totalToT    = (persObj->m_properties & 0xFFFFFF); 

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

  // Charge unbalance
  transObj->m_omegax = persObj->m_omegax;
  transObj->m_omegay = persObj->m_omegay;

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
      Identifier waferId = transObj->m_clusId;
      clusId = Identifier(m_pixId2->pixel_id_offset(waferId, persObj->m_rdoList[0]) );
      is64BitId = true;
      rdoListSize--; 
  }
  else {
      // Must also fix cluster pixel id when reading in old 32-bit ids
      // and the code has 64-bit ids
      clusId =  m_pixId2->pixel_id_from_shortened(persObj->m_clusId);
  }
  transObj->m_clusId = clusId;
  transObj->m_rdoList.resize(rdoListSize);
  std::vector<Identifier>::iterator idit = transObj->m_rdoList.begin();
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

  // AS: changes needed for the Pixel cluster splitting
  transObj->m_splitInfo    = persObj->m_splitInfo;
  transObj->m_chargeList   = persObj->m_chargeList;
  
}//end of cnv_p3 method


void PixelClusterCnv_p3::transToPers( const InDet::PixelCluster *transObj, InDet::PixelCluster_p3 *persObj, MsgStream &log )
{
//   log << MSG::VERBOSE << "In PixelCluster::transToPers" << endreq;
  m_swCnv.transToPers(&transObj->m_width, &persObj->m_width, log);

  // base class:
  //

  // Local Position
  persObj->m_localPosX = transObj->m_localPos[Trk::locX];
  persObj->m_localPosY = transObj->m_localPos[Trk::locY];

  // Error Matrix
  
  persObj->m_mat00 = (*transObj->m_localCovariance)(0,0);
  persObj->m_mat01 = (*transObj->m_localCovariance)(0,1);
  persObj->m_mat11 = (*transObj->m_localCovariance)(1,1);

  // Charge balance
  persObj->m_omegax = transObj->m_omegax;
  persObj->m_omegay = transObj->m_omegay;

  // List of Id of the cluster

  //   Save cluster id explicitly in pers cluster m_clusId for 32-bit
  //   ids. For 64-bit ids, we set m_clusId to 0 and instead save the
  //   diff in m_rdoList as the first element
#ifndef __IDENTIFIER_64BIT__
  persObj->m_rdoList.resize( transObj->m_rdoList.size() );
#else /* __IDENTIFIER_64BIT__ */
  persObj->m_rdoList.resize(transObj->m_rdoList.size() + 1);
#endif /* __IDENTIFIER_64BIT__ */

  std::vector<InDet::PixelCluster_p3::rdo_diff_type>::iterator pit = persObj->m_rdoList.begin();
  // Get base identifier to calculate diffs
  Identifier clusId = transObj->identify();
#ifndef __IDENTIFIER_64BIT__
  persObj->m_clusId = clusId.get_compact();
#else /* __IDENTIFIER_64BIT__ */
  persObj->m_clusId = 0;
  // Save offset to wafer id for the cluster id
  Identifier waferId =  m_pixId2->wafer_id(clusId); // remove the eta/phi of channel id
  *pit = static_cast<InDet::PixelCluster_p3::rdo_diff_type>( m_pixId2->calc_offset(waferId, clusId) );
  pit++;
#endif /* __IDENTIFIER_64BIT__ */
  
  for (std::vector<Identifier>::const_iterator it=transObj->m_rdoList.begin(); 
       it != transObj->m_rdoList.end(); it++) {
    *pit = static_cast<InDet::PixelCluster_p3::rdo_diff_type>( m_pixId2->calc_offset(clusId, *it) );
    pit++;
  }

  // Setting of cluster properties:
  persObj->m_properties = 0;
  if ( transObj->m_gangedPixel )  persObj->m_properties |= (1<<24);
  if ( transObj->m_fake        )  persObj->m_properties |= (1<<25);
  if ( transObj->m_ambiguous   )  persObj->m_properties |= (1<<26);
  persObj->m_properties |= ((transObj->m_lvl1 & 0xF)<<28);

  if ( transObj->m_totalCharge>0. ) {
    persObj->m_properties |= (1<<27);
    persObj->m_properties |= ( (int)(transObj->m_totalCharge+0.5) & 0xFFFFFF );
  } else {
    persObj->m_properties |= ( transObj->m_totalToT & 0xFFFFFF );
  }
  // needed for pixel cluster splitting
  persObj->m_splitInfo    = transObj->m_splitInfo;
  persObj->m_chargeList   = transObj->m_chargeList;
}


