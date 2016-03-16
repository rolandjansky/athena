/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRT_DRIFTCIRCLECONTAINERCNV_P1_H
#define TRT_DRIFTCIRCLECONTAINERCNV_P1_H

//-----------------------------------------------------------------------------
//
// file:   TRT_DriftCircleContainerCnv_p1.h
//
//-----------------------------------------------------------------------------
 
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
 
#include "InDetPrepRawData/TRT_DriftCircleContainer.h"
#include "InDetEventTPCnv/InDetPrepRawData/InDetPRD_Container_p1.h"
class TRT_ID;
namespace InDetDD{ class TRT_DetectorManager;}

class StoreGateSvc;

class MsgStream;
namespace InDet{
class TRT_DriftCircleContainerCnv_p1 : public T_AthenaPoolTPCnvBase< InDet::TRT_DriftCircleContainer, InDet::InDetPRD_Container_p1 >
 
{
 public:
   typedef InDet::InDetPRD_Container_p1 PERS; 
   typedef InDet::TRT_DriftCircleContainer TRANS;
   TRT_DriftCircleContainerCnv_p1(): m_isInitialized(0) {}
   virtual void persToTrans(const PERS* persCont, TRANS* transCont, MsgStream &log); 
   virtual void transToPers(const TRANS* transCont, PERS* persCont, MsgStream &log);
   virtual InDet::TRT_DriftCircleContainer* createTransient(const InDet::InDetPRD_Container_p1* persObj, MsgStream& log);
 private:
   const TRT_ID *m_trtId;
   const InDetDD::TRT_DetectorManager* m_trtMgr;
   bool m_isInitialized;
   StatusCode initialize(MsgStream &log);
};
 
}
#endif 
 
