/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "VxVertex/RecVertex.h"
#include "TrkEventTPCnv/VxVertex/RecVertexCnv_p1.h"
#include "TrkEventTPCnv/helpers/EigenHelpers.h"

void  RecVertexCnv_p1::persToTrans(const Trk::RecVertex_p1 * persObj, Trk::RecVertex * transObj, MsgStream &log) 
{
  fillTransFromPStore( &m_vertexConverter, persObj->vtx, transObj, log);
	
  // fillTransFromPStore( &m_errorMatrixConverter, persObj->m_positionError, &transObj->m_positionCovariance, log ); 
  Trk::ErrorMatrix dummy;
  fillTransFromPStore( &m_errorMatrixConverter, persObj->m_positionError, &dummy, log );
  EigenHelpers::vectorToEigenMatrix(dummy.values, transObj->m_positionCovariance, "RecVertexCnv_p1");
  
  fillTransFromPStore(&m_fitQualityConverter, persObj->m_fitQuality, &transObj->m_fitQuality, log);
	
//   std::cout<<"ILIJA read RecVertex p1  --- > "<<(*transObj)<<std::endl;
}
  
void  RecVertexCnv_p1::transToPers(const Trk::RecVertex * transObj, Trk::RecVertex_p1 * persObj, MsgStream &log) 
{
  persObj->vtx = baseToPersistent(&m_vertexConverter,  transObj, log );
  // persObj->m_positionError = toPersistent( &m_errorMatrixConverter, &(transObj->m_positionCovariance), log );
  
  Trk::ErrorMatrix pMat;
  EigenHelpers::eigenMatrixToVector(pMat.values, transObj->m_positionCovariance, "RecVertexCnv_p1");
  persObj->m_positionError = toPersistent( &m_errorMatrixConverter, &pMat, log );
  
  
  persObj->m_fitQuality = toPersistent( &m_fitQualityConverter, &(transObj->m_fitQuality), log ); 

//   std::cout<<"ILIJA write RecVertex p1  --- > "<<(*transObj)<<std::endl;
}
