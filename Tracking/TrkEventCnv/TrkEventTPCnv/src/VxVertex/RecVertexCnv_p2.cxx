/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "VxVertex/RecVertex.h"
#include "TrkEventTPCnv/VxVertex/RecVertexCnv_p2.h"

#include "TrkEventTPCnv/TrkEventPrimitives/FitQuality_p1.h"
#include "TrkEventTPCnv/TrkEventPrimitives/FitQualityCnv_p1.h"
#include "TrkEventTPCnv/helpers/EigenHelpers.h"

static const FitQualityCnv_p1 fitQualityCnv;

void  RecVertexCnv_p2::persToTrans(const Trk::RecVertex_p2 * persObj, Trk::RecVertex * transObj, MsgStream &log) {

    // fillTransFromPStore( &m_vertexConverter, persObj->vtx, transObj, log);
  Amg::Vector3D trans_position(persObj->m_position[0], persObj->m_position[1], persObj->m_position[2]);

    // fillTransFromPStore( &m_errorMatrixConverter, persObj->m_positionError, &transObj->m_positionCovariance, log ); 

  Trk::ErrorMatrix dummy;
  fillTransFromPStore( &m_errorMatrixConverter, persObj->m_positionError, &dummy, log );
  AmgSymMatrix(3) cov;
  if (0!=dummy.values.size()){
    if (dummy.values.size() == 0) dummy.values.resize(6);
    EigenHelpers::vectorToEigenMatrix(dummy.values, cov, "RecVertexCnv_p2");
  } else {
    // Seems we didn't write out an error matrix - set it to an identity
    cov.setIdentity();
  }


  *transObj = Trk::RecVertex (trans_position, cov,
                              persObj->m_fitQuality.m_numberDoF,
                              persObj->m_fitQuality.m_chiSquared);
}

void  RecVertexCnv_p2::transToPers(const Trk::RecVertex * transObj, Trk::RecVertex_p2 * persObj, MsgStream &log) {

    // persObj->vtx = baseToPersistent(&m_vertexConverter,  transObj, log );
  const Amg::Vector3D& trans_position = transObj->position();
  persObj->m_position[0] = trans_position(0);
  persObj->m_position[1] = trans_position(1);
  persObj->m_position[2] = trans_position(2);

    // persObj->m_positionError = toPersistent( &m_errorMatrixConverter, &(transObj->m_positionCovariance), log );
  Trk::ErrorMatrix pMat;
  EigenHelpers::eigenMatrixToVector(pMat.values, transObj->covariancePosition(), "RecVertexCnv_p2");
  persObj->m_positionError = toPersistent( &m_errorMatrixConverter, &pMat, log );

    // persObj->m_fitQuality = toPersistent( &m_fitQualityConverter, &(transObj->m_fitQuality), log ); 
  fitQualityCnv.transToPers(&transObj->fitQuality(),  &persObj->m_fitQuality, log);

  // std::cout<<"ILIJA written RecVertex_p2 "<<(*transObj)<<std::endl;
}
