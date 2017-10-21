/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SCT_CLUSTERCONTAINERCNV_P0_H
#define SCT_CLUSTERCONTAINERCNV_P0_H

#include "StoreGate/StoreGateSvc.h"
#include "AthContainers/DataVector.h"
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
#include "InDetPrepRawData/SCT_ClusterContainer.h"
typedef DataVector<Trk::PrepRawDataCollection<InDet::SCT_Cluster> > SCT_ClusterContainer_p0;


class SCT_ID;
namespace InDetDD{
class SCT_DetectorManager;
}

class MsgStream;
class SCT_ClusterContainerCnv_p0  : public T_AthenaPoolTPCnvBase<InDet::SCT_ClusterContainer, SCT_ClusterContainer_p0> {
 private:
  const SCT_ID*  m_sctId{nullptr};
  const InDetDD::SCT_DetectorManager* m_sctMgr{nullptr};
 public:
  virtual void   persToTrans(const SCT_ClusterContainer_p0*, InDet::SCT_ClusterContainer*, MsgStream&) override {
    // everything is done in createTransient()
  }

  virtual void   transToPers(const InDet::SCT_ClusterContainer*, SCT_ClusterContainer_p0*, MsgStream&) override {
   throw std::runtime_error("Writing SCT PRDs in the old format is not supported");
  }

  virtual InDet::SCT_ClusterContainer* createTransient(const SCT_ClusterContainer_p0* /*persObj*/, MsgStream& /*log*/) override
  { std::abort(); }
  InDet::SCT_ClusterContainer* createTransient(SCT_ClusterContainer_p0* persObj, MsgStream& log);

  // ID helper can't be used in the constructor, need initialize()
  StatusCode initialize( MsgStream &log );
};

#endif

