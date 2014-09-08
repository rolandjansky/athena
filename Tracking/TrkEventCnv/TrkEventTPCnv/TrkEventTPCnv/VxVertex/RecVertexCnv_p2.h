/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef REC_VERTEX_CNV_P2_H
#define REC_VERTEX_CNV_P2_H

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

#include "VxVertex/RecVertex.h"
#include "TrkEventTPCnv/VxVertex/RecVertex_p2.h"
#include "VxVertex/Vertex.h"
#include "TrkEventTPCnv/TrkEventPrimitives/ErrorMatrixCnv_p1.h"

 class MsgStream;
 
 class RecVertexCnv_p2 : public T_AthenaPoolTPPolyCnvBase< Trk::Vertex, Trk::RecVertex, Trk::RecVertex_p2>
 {                            
  public:

   RecVertexCnv_p2():m_errorMatrixConverter(0){} //,m_vertexConverter(0),m_fitQualityConverter(0){} 

    void  persToTrans(const Trk::RecVertex_p2 * persObj, Trk::RecVertex * transObj, MsgStream &log);
  
    void  transToPers(const Trk::RecVertex * transObj, Trk::RecVertex_p2 * persObj, MsgStream &log);  
  
   private:

    ITPConverterFor<Trk::ErrorMatrix> *m_errorMatrixConverter;
    // ITPConverterFor<Trk::Vertex>      *m_vertexConverter;
    // ITPConverterFor<Trk::FitQuality>  *m_fitQualityConverter;
     
 }; 

#endif 
