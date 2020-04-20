// -*- c++ -*- 
/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRT_DRIFTCIRCLECONTAINERCNV_P0_H
#define TRT_DRIFTCIRCLECONTAINERCNV_P0_H

#include "StoreGate/StoreGateSvc.h"
#include "AthContainers/DataVector.h"
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
#include "InDetPrepRawData/TRT_DriftCircleContainer.h"
#include "TRT_ReadoutGeometry/TRT_DetElementContainer.h"
#include "StoreGate/ReadCondHandleKey.h"
typedef DataVector<Trk::PrepRawDataCollection<InDet::TRT_DriftCircle> > TRT_DriftCircleContainer_p0;


class TRT_ID;

class MsgStream;
class TRT_DriftCircleContainerCnv_p0  : public T_AthenaPoolTPCnvBase<InDet::TRT_DriftCircleContainer, TRT_DriftCircleContainer_p0> {
 private:
   const TRT_ID*  m_trtId;
   SG::ReadCondHandleKey<InDetDD::TRT_DetElementContainer> m_trtDetEleContKey;
 public:

	// CID 106239 fix
  TRT_DriftCircleContainerCnv_p0() : m_trtId(nullptr), m_trtDetEleContKey("TRT_DetElementContainer") {}

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
