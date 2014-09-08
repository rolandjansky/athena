/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#define private public
#define protected public
#include "VxVertex/RecVertex.h"
#undef private
#undef protected

#include "TrkEventTPCnv/VxVertex/RecVertexCnv_p2.h"

#include "TrkEventTPCnv/TrkEventPrimitives/FitQuality_p1.h"
#include "TrkEventTPCnv/TrkEventPrimitives/FitQualityCnv_p1.h"
#include "TrkEventTPCnv/helpers/EigenHelpers.h"

static FitQualityCnv_p1 m_fitQualityCnv;

void  RecVertexCnv_p2::persToTrans(const Trk::RecVertex_p2 * persObj, Trk::RecVertex * transObj, MsgStream &log) {

    // fillTransFromPStore( &m_vertexConverter, persObj->vtx, transObj, log);
  Amg::Vector3D trans_position(persObj->m_position[0], persObj->m_position[1], persObj->m_position[2]);
  transObj->m_position = trans_position;

    // fillTransFromPStore( &m_errorMatrixConverter, persObj->m_positionError, &transObj->m_positionCovariance, log ); 

  Trk::ErrorMatrix dummy;
  fillTransFromPStore( &m_errorMatrixConverter, persObj->m_positionError, &dummy, log );
  if (0!=dummy.values.size()){
    if (dummy.values.size() == 0) dummy.values.resize(6);
    EigenHelpers::vectorToEigenMatrix(dummy.values, transObj->m_positionCovariance, "RecVertexCnv_p2");
  } else {
    // Seems we didn't write out an error matrix - set it to an identity
    transObj->m_positionCovariance.setIdentity();
  }

  // fillTransFromPStore(&m_fitQualityConverter, persObj->m_fitQuality, &transObj->m_fitQuality, log);
  m_fitQualityCnv.persToTrans(&persObj->m_fitQuality,  &transObj->m_fitQuality, log);

//  std::cout<<"ILIJA Read RecVertex_p2 "<<(*transObj)<<std::endl;

}

void  RecVertexCnv_p2::transToPers(const Trk::RecVertex * transObj, Trk::RecVertex_p2 * persObj, MsgStream &log) {

    // persObj->vtx = baseToPersistent(&m_vertexConverter,  transObj, log );
  Amg::Vector3D trans_position = transObj->m_position;
  persObj->m_position[0] = trans_position(0);
  persObj->m_position[1] = trans_position(1);
  persObj->m_position[2] = trans_position(2);

    // persObj->m_positionError = toPersistent( &m_errorMatrixConverter, &(transObj->m_positionCovariance), log );
  Trk::ErrorMatrix pMat;
  EigenHelpers::eigenMatrixToVector(pMat.values, transObj->m_positionCovariance, "RecVertexCnv_p2");
  persObj->m_positionError = toPersistent( &m_errorMatrixConverter, &pMat, log );

    // persObj->m_fitQuality = toPersistent( &m_fitQualityConverter, &(transObj->m_fitQuality), log ); 
  m_fitQualityCnv.transToPers(&transObj->m_fitQuality,  &persObj->m_fitQuality, log);

  // std::cout<<"ILIJA written RecVertex_p2 "<<(*transObj)<<std::endl;
}
