/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

//-----------------------------------------------------------------------------
//
// file:   TRT_DriftCircleCnv_p2.cxx
//
//-----------------------------------------------------------------------------

#include "InDetPrepRawData/TRT_DriftCircle.h"
#include "InDetEventTPCnv/InDetPrepRawData/TRT_DriftCircleCnv_p2.h"
#include "EventPrimitives/EventPrimitives.h"
#include "TrkEventPrimitives/ParamDefs.h"

#include <memory>

InDet::TRT_DriftCircle
TRT_DriftCircleCnv_p2::createTRT_DriftCircle( const InDet::TRT_DriftCircle_p2 *persObj,
                                              Identifier clusId,
                                              std::vector<Identifier>&& rdoList,
                                              const InDetDD::TRT_BaseElement* detEl,
                                              MsgStream &/*log*/)
{
  Amg::Vector2D localPos;
  localPos[Trk::locX] = persObj->m_locPos;
  localPos[Trk::locY] = 0;

  auto cmat = Amg::MatrixX(1,1);
   (cmat)(0,0) = static_cast<double>(persObj->m_errMat);
  
  InDet::TRT_DriftCircle dc (clusId,
                             localPos,
                             std::move(rdoList),
                             std::move(cmat),
                             detEl,
                             persObj->m_TRTdata
                             );
                             
  return dc;
}


void TRT_DriftCircleCnv_p2::persToTrans( const InDet::TRT_DriftCircle_p2 *persObj,
					 InDet::TRT_DriftCircle    *transObj,
					 MsgStream                   &log)
{
  *transObj = createTRT_DriftCircle (persObj,
                                     Identifier(),
                                     std::vector<Identifier>(),
                                     nullptr,
                                     log);
}

void TRT_DriftCircleCnv_p2::transToPers( const InDet::TRT_DriftCircle    *transObj,
					 InDet::TRT_DriftCircle_p2 *persObj,
					 MsgStream                   &/*log*/)
{
  //Save the TRT word
  persObj->m_TRTdata  = transObj->getWord();

// Save bare minimum info to recreate the base class
//     persObj->m_id = (unsigned int) transObj->m_clusId;
     persObj->m_locPos = transObj->localPosition()[Trk::locX];
     persObj->m_errMat = (transObj->localCovariance())(0,0);
     
}

