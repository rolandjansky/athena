/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SCT_CLUSTERCONTAINERCNV_P0_H
#define SCT_CLUSTERCONTAINERCNV_P0_H

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
#include "InDetPrepRawData/SCT_ClusterContainer.h"
#include "AthContainers/DataVector.h"
#include "InDetReadoutGeometry/SiDetectorElementCollection.h"
#include "StoreGate/ReadCondHandleKey.h"

typedef DataVector<Trk::PrepRawDataCollection<InDet::SCT_Cluster> > SCT_ClusterContainer_p0;

class SCT_ID;
class MsgStream;

class SCT_ClusterContainerCnv_p0  : public T_AthenaPoolTPCnvBase<InDet::SCT_ClusterContainer, SCT_ClusterContainer_p0> {
 public:
  SCT_ClusterContainerCnv_p0();

  // ID helper can't be used in the constructor, need initialize()
  StatusCode initialize( MsgStream& log );

  virtual void   persToTrans(const SCT_ClusterContainer_p0*, InDet::SCT_ClusterContainer*, MsgStream&) override {
    // everything is done in createTransient()
  }

  virtual void   transToPers(const InDet::SCT_ClusterContainer*, SCT_ClusterContainer_p0*, MsgStream&) override {
    throw std::runtime_error("Writing SCT PRDs in the old format is not supported");
  }

  virtual InDet::SCT_ClusterContainer* createTransient(const SCT_ClusterContainer_p0* /*persObj*/, MsgStream& /*log*/) override {
    std::abort(); 
  }

  InDet::SCT_ClusterContainer* createTransient(SCT_ClusterContainer_p0* persObj, MsgStream& log);

 private:
  const SCT_ID*  m_sctId;
  SG::ReadCondHandleKey<InDetDD::SiDetectorElementCollection> m_SCTDetEleCollKey;
};

#endif
