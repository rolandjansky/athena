/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//-----------------------------------------------------------------------------
//
// file:   TRT_DriftCircleCnv_p2.cxx
//
//-----------------------------------------------------------------------------

#define private public
#define protected public
#include "InDetPrepRawData/TRT_DriftCircle.h"
#undef private
#undef protected

#include "InDetEventTPCnv/InDetPrepRawData/TRT_DriftCircleCnv_p2.h"
#include "EventPrimitives/EventPrimitives.h"
#include "TrkEventPrimitives/ParamDefs.h"


void TRT_DriftCircleCnv_p2::persToTrans( const InDet::TRT_DriftCircle_p2 *persObj,
					 InDet::TRT_DriftCircle    *transObj,
					 MsgStream                   &/*log*/)
{
// Copy over the TRT word
   transObj->m_word   = persObj->m_TRTdata;

// Rebuild the base class information
//        Identifier
//   transObj->m_clusId=Identifier(persObj->m_id);
//        Local Position
   transObj->m_localPos[Trk::locX] = persObj->m_locPos; 
   transObj->m_localPos[Trk::locY] = 0.0;
   
   
//        Error matrix   
   Amg::MatrixX* cmat = new  Amg::MatrixX(1,1);
   (*cmat)(0,0) = static_cast<double>(persObj->m_errMat);
   transObj->m_localCovariance     = cmat;
//        List of ID of RDOs
//   transObj->m_rdoList.resize(1);
//   transObj->m_rdoList[0]=transObj->m_clusId;

}

void TRT_DriftCircleCnv_p2::transToPers( const InDet::TRT_DriftCircle    *transObj,
					 InDet::TRT_DriftCircle_p2 *persObj,
					 MsgStream                   &/*log*/)
{
  //Save the TRT word
     persObj->m_TRTdata  = transObj->m_word;

// Save bare minimum info to recreate the base class
//     persObj->m_id = (unsigned int) transObj->m_clusId;
     persObj->m_locPos = transObj->m_localPos[Trk::locX];
     persObj->m_errMat = (*transObj->m_localCovariance)(0,0);
     
}

