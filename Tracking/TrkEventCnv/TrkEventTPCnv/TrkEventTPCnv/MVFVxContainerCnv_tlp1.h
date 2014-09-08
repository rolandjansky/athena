/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MVFVXCONTAINER_CNV_TLP1_TRK_H
#define MVFVXCONTAINER_CNV_TLP1_TRK_H


#include "AthenaPoolCnvSvc/AthenaPoolTopLevelTPConverter.h"

#include "MVFVxContainer_tlp1.h"

//-----------------------------------------
// VxMultiVertex
//-----------------------------------------
#include "TrkEventTPCnv/VxMultiVertex/MVFVxContainerCnv_p1.h"   
#include "TrkEventTPCnv/VxMultiVertex/MVFVxCandidateCnv_p1.h" 
#include "TrkEventTPCnv/VxMultiVertex/MVFVxTrackAtVertexCnv_p1.h"

class MVFVxContainerCnv_tlp1 : public AthenaPoolTopLevelTPConverter< MVFVxContainerCnv_p1, Trk::MVFVxContainer_tlp1 >
{

 public:
 
  MVFVxContainerCnv_tlp1();
  virtual ~MVFVxContainerCnv_tlp1() {}
  
//default methods  
  virtual void	setPStorage( Trk::MVFVxContainer_tlp1 *storage );

  virtual unsigned short       getConverterID() { return 1; }

// all TP converters for types used in the Vertex package   

  protected:

//VxMultiVertex
   MVFVxCandidateCnv_p1      m_mVFVxCandidateCnv;
   MVFVxTrackAtVertexCnv_p1  m_mVFVxTrackAtVertexCnv;
     
};//end of class definitions


template <>
class T_TPCnv<MVFVxContainer, Trk::MVFVxContainer_tlp1 >
  : public MVFVxContainerCnv_tlp1
{
public:
  // ??? Maybe this should be moved up to AthenaPoolTopLevelTPConverter...
  virtual void persToTrans (const Trk::MVFVxContainer_tlp1* pers,
                            MVFVxContainer* trans,
                            MsgStream& msg);
};


#endif //MVFVXCONTAINER_CNV_TLP1_TRK_H
