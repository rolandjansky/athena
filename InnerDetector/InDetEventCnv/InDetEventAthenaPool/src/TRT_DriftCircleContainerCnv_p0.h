/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRT_DRIFTCIRCLECONTAINERCNV_P0_H
#define TRT_DRIFTCIRCLECONTAINERCNV_P0_H

#include "StoreGate/StoreGateSvc.h"
#include "AthContainers/DataVector.h"
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
#include "InDetPrepRawData/TRT_DriftCircleContainer.h"
typedef DataVector<Trk::PrepRawDataCollection<InDet::TRT_DriftCircle> > TRT_DriftCircleContainer_p0;


class TRT_ID;
namespace InDetDD{
class TRT_DetectorManager;
}

class MsgStream;
class TRT_DriftCircleContainerCnv_p0  : public T_AthenaPoolTPCnvBase<InDet::TRT_DriftCircleContainer, TRT_DriftCircleContainer_p0> {
 private:
   const TRT_ID*  m_trtId;
   const InDetDD::TRT_DetectorManager* m_trtMgr;
 public:
  virtual void   persToTrans(const TRT_DriftCircleContainer_p0*, InDet::TRT_DriftCircleContainer*, MsgStream&) override {
    // everything is done in createTransient()
  }

  virtual void   transToPers(const InDet::TRT_DriftCircleContainer*, TRT_DriftCircleContainer_p0*, MsgStream&) override {
    throw std::runtime_error("Writing TRT PRDs in the old format is not supported");
  }

  virtual InDet::TRT_DriftCircleContainer* createTransient(const TRT_DriftCircleContainer_p0* /*persObj*/, MsgStream& /*log*/) override
  { std::abort(); }
  InDet::TRT_DriftCircleContainer* createTransient(TRT_DriftCircleContainer_p0* persObj, MsgStream& log);

  // ID helper can't be used in the constructor, need initialize()
  StatusCode initialize( MsgStream &log );
};

#endif/*TRT_RDO_CONTAINERCNV_P0_H*/
