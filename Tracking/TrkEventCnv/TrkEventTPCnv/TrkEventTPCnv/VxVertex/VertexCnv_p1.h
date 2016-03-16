/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef VERTEX_CNV_P1_H
#define VERTEX_CNV_P1_H

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

#include "VxVertex/Vertex.h"
#include "TrkEventTPCnv/VxVertex/Vertex_p1.h"

/**
 * Converter for T/P separation of the VxVertex/Vertex class.
 * Kirill Prokofiev <Kirill.Prokofiev@cern.ch>
 *
 * July 2006
 * January 2007: bring up to date with new TP split version
 */

 class MsgStream;
 
 class VertexCnv_p1 : public T_AthenaPoolTPPolyCnvBase<Trk::Vertex, Trk::Vertex, Trk::Vertex_p1>
 {

  public:
  
   VertexCnv_p1(){}
  
   void  persToTrans(const Trk::Vertex_p1 * persObj, Trk::Vertex * transObj, MsgStream &log);
  
   void  transToPers(const Trk::Vertex * transObj, Trk::Vertex_p1 * persObj, MsgStream &log);
   
 };//end of class definitions

#endif //VERTEX_CNV_P1_H
