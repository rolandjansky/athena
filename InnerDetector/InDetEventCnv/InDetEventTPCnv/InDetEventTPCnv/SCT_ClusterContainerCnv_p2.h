/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SCT_CLUSTERCONTAINERCNV_P2_H
#define SCT_CLUSTERCONTAINERCNV_P2_H

// SCT_DriftCircleContainerCnv_p2, T/P separation of SCT PRD
// author D.Costanzo <davide.costanzo@cern.ch>

#include "InDetPrepRawData/SCT_ClusterContainer.h"
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
#include "InDetEventTPCnv/SCT_ClusterContainer_p2.h"

class SCT_ID;
class StoreGateSvc;
namespace InDetDD{ class SCT_DetectorManager;}

class SCT_ClusterContainerCnv_p2 : public T_AthenaPoolTPCnvBase<InDet::SCT_ClusterContainer, InDet::SCT_ClusterContainer_p2>
{
 public:
  SCT_ClusterContainerCnv_p2() : m_sctId{nullptr}, m_storeGate{nullptr}, m_sctMgr{nullptr}, m_isInitialized{false} {};
  
  virtual void	persToTrans(const InDet::SCT_ClusterContainer_p2* persCont,
			    InDet::SCT_ClusterContainer* transCont,
			    MsgStream &log) ;
  virtual void	transToPers(const InDet::SCT_ClusterContainer* transCont,
			    InDet::SCT_ClusterContainer_p2* persCont,
			    MsgStream &log) ;

  virtual InDet::SCT_ClusterContainer* createTransient(const InDet::SCT_ClusterContainer_p2* persObj, MsgStream& log);
  

 private:
   const SCT_ID *m_sctId;
   StoreGateSvc *m_storeGate;
   const InDetDD::SCT_DetectorManager* m_sctMgr;
   bool m_isInitialized;
   StatusCode initialize(MsgStream &log);


};

#endif


