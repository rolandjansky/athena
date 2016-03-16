/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRT_DRIFTCIRCLECONTAINERCNV_P2_H
#define TRT_DRIFTCIRCLECONTAINERCNV_P2_H

// TRT_DriftCircleContainerCnv_p2, T/P separation of TRT PRD
// author D.Costanzo <davide.costanzo@cern.ch>

#include "InDetPrepRawData/TRT_DriftCircleContainer.h"
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
#include "InDetEventTPCnv/TRT_DriftCircleContainer_p2.h"

class TRT_ID;
class StoreGateSvc;
namespace InDetDD{ class TRT_DetectorManager;}

class TRT_DriftCircleContainerCnv_p2 : public T_AthenaPoolTPCnvBase<InDet::TRT_DriftCircleContainer, InDet::TRT_DriftCircleContainer_p2>
{
 public:
  TRT_DriftCircleContainerCnv_p2() : m_isInitialized(0)  {};
  
  virtual void	persToTrans(const InDet::TRT_DriftCircleContainer_p2* persCont,
			    InDet::TRT_DriftCircleContainer* transCont,
			    MsgStream &log) ;
  virtual void	transToPers(const InDet::TRT_DriftCircleContainer* transCont,
			    InDet::TRT_DriftCircleContainer_p2* persCont,
			    MsgStream &log) ;

  virtual InDet::TRT_DriftCircleContainer* createTransient(const InDet::TRT_DriftCircleContainer_p2* persObj, MsgStream& log);
  

 private:
   const TRT_ID *m_trtId;
   StoreGateSvc *m_storeGate;
   const InDetDD::TRT_DetectorManager* m_trtMgr;
   bool m_isInitialized;
   StatusCode initialize(MsgStream &log);


};

#endif


