/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef REC_VERTEX_CNV_P1_H
#define REC_VERTEX_CNV_P1_H

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

#include "VxVertex/RecVertex.h"
#include "TrkEventTPCnv/VxVertex/RecVertex_p1.h"
#include "VxVertex/Vertex.h"
#include "TrkEventTPCnv/TrkEventPrimitives/ErrorMatrixCnv_p1.h"

/**
 * Converter for Transient/Persistent separation
 * of the VxVertex/RecVertex class.
 * Kirill Prokofiev <Kirill.Prokofiev@cern.ch>
 *
 * July 2006
 * January 2007 modified for the new TP split schema
 */

 class MsgStream;
 
 class RecVertexCnv_p1 : public T_AthenaPoolTPPolyCnvBase< Trk::Vertex, Trk::RecVertex, Trk::RecVertex_p1>
 {                            
  public:
  
/** 
 * default constructor   
 */
   RecVertexCnv_p1():m_errorMatrixConverter(0),m_vertexConverter(0),m_fitQualityConverter(0){} 

/**  
 * Conversion methods 
 */
    void  persToTrans(const Trk::RecVertex_p1 * persObj, Trk::RecVertex * transObj, MsgStream &log);
  
    void  transToPers(const Trk::RecVertex * transObj, Trk::RecVertex_p1 * persObj, MsgStream &log);  
  
   private:

    ITPConverterFor<Trk::ErrorMatrix> *m_errorMatrixConverter;
    ITPConverterFor<Trk::Vertex>      *m_vertexConverter;
    ITPConverterFor<Trk::FitQuality>  *m_fitQualityConverter;
     
 }; //end of class definitions

#endif //REC_VERTEX_CNV_P1_H
