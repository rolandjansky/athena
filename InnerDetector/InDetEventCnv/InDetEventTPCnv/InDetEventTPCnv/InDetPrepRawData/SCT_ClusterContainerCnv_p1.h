/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SCT_CLUSTERCONTAINERCNV_P1_H
#define SCT_CLUSTERCONTAINERCNV_P1_H

//-----------------------------------------------------------------------------
//
// file:   SCT_ClusterContainerCnv_p1.h
//
//-----------------------------------------------------------------------------
 
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
 
#include "InDetPrepRawData/SCT_ClusterContainer.h"
#include "InDetEventTPCnv/InDetPrepRawData/InDetPRD_Container_p1.h"
class SCT_ID;
namespace InDetDD{ class SCT_DetectorManager;}

class StoreGateSvc;

class MsgStream;
namespace InDet{
class SCT_ClusterContainerCnv_p1 : public T_AthenaPoolTPCnvBase< InDet::SCT_ClusterContainer, InDet::InDetPRD_Container_p1 >
 
{
 public:
   typedef InDet::InDetPRD_Container_p1 PERS; 
   typedef InDet::SCT_ClusterContainer TRANS;
   SCT_ClusterContainerCnv_p1(): m_sctId{nullptr}, m_storeGate{nullptr}, m_sctMgr{nullptr}, m_isInitialized(0) {}
   virtual void persToTrans(const PERS* persCont, TRANS* transCont, MsgStream &log); 
   virtual void transToPers(const TRANS* transCont, PERS* persCont, MsgStream &log);
   virtual InDet::SCT_ClusterContainer* createTransient(const InDet::InDetPRD_Container_p1* persObj, MsgStream& log);
 private:
   const SCT_ID *m_sctId;
   StoreGateSvc *m_storeGate;
   const InDetDD::SCT_DetectorManager* m_sctMgr;
   bool m_isInitialized;
   StatusCode initialize(MsgStream &log);
};
 
}
#endif 
 
