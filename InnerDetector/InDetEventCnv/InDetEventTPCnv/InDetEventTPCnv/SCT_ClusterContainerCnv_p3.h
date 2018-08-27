/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SCT_CLUSTERCONTAINERCNV_P3_H
#define SCT_CLUSTERCONTAINERCNV_P3_H

// SCT_DriftCircleContainerCnv_p3, T/P separation of SCT PRD
// author D.Costanzo <davide.costanzo@cern.ch>

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

#include "InDetPrepRawData/SCT_ClusterContainer.h"
#include "InDetEventTPCnv/SCT_ClusterContainer_p3.h"

#include "InDetReadoutGeometry/SiDetectorElementCollection.h"
#include "StoreGate/ReadCondHandleKey.h"

class SCT_ID;
class StoreGateSvc;

class SCT_ClusterContainerCnv_p3 : public T_AthenaPoolTPCnvBase<InDet::SCT_ClusterContainer, InDet::SCT_ClusterContainer_p3>
{
 public:
  SCT_ClusterContainerCnv_p3() : m_sctId{nullptr}, m_storeGate{nullptr}, m_SCTDetEleCollKey{"SCT_DetectorElementCollection"}, m_useDetectorElement{true}, m_isInitialized{false} {};
  
  virtual void transToPers(const InDet::SCT_ClusterContainer* transCont,
                           InDet::SCT_ClusterContainer_p3* persCont,
                           MsgStream &log) ;
  virtual void persToTrans(const InDet::SCT_ClusterContainer_p3* persCont,
                           InDet::SCT_ClusterContainer* transCont,
                           MsgStream &log) ;

  virtual InDet::SCT_ClusterContainer* createTransient(const InDet::SCT_ClusterContainer_p3* persObj, MsgStream& log);

  // Methods for test/SCT_ClusterContainerCnv_p3_test.cxx
  void setIdHelper(const SCT_ID* sct_id);
  void setUseDetectorElement(const bool useDetectorElement);

 private:
  const SCT_ID *m_sctId;
  StoreGateSvc *m_storeGate;
  SG::ReadCondHandleKey<InDetDD::SiDetectorElementCollection> m_SCTDetEleCollKey;
  bool m_useDetectorElement;
  bool m_isInitialized;
  StatusCode initialize(MsgStream &log);

};

#endif
