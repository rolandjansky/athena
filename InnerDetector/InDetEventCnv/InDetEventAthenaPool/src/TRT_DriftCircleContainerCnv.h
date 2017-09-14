/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRT_DRIFTCIRCLECONTAINERCNV_H
#define TRT_DRIFTCIRCLECONTAINERCNV_H
#include "InDetPrepRawData/TRT_DriftCircleContainer.h"
#include "InDetPrepRawData/TRT_DriftCircleCollection.h"
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"



#include "StoreGate/StoreGateSvc.h"

#include "TRT_DriftCircleContainerCnv_p0.h"
#include "InDetEventTPCnv/TRT_DriftCircleContainerCnv_tlp1.h"
#include "InDetEventTPCnv/TRT_DriftCircleContainerCnv_p2.h"

// the latest persistent representation type of DataCollection:
typedef  InDet::TRT_DriftCircleContainer_p2  TRT_DriftCircleContainer_PERS;
typedef  T_AthenaPoolCustomCnv<InDet::TRT_DriftCircleContainer, TRT_DriftCircleContainer_PERS >   TRT_DriftCircleContainerCnvBase;

class TRT_DriftCircleContainerCnv : public TRT_DriftCircleContainerCnvBase
{
  friend class CnvFactory<TRT_DriftCircleContainerCnv >;

  // Converters need to be initialized (use ID helpers)
  // Thus they can't be local
  TRT_DriftCircleContainerCnv_p0   m_converter_p0;
  TRT_DriftCircleContainerCnv_tlp1 m_TPConverter;
  TRT_DriftCircleContainerCnv_p2   m_TPConverter2;

  // Should not be needed at some point.
  StoreGateSvc*  m_storeGate;

protected:
  TRT_DriftCircleContainerCnv (ISvcLocator* svcloc);
  ~TRT_DriftCircleContainerCnv();
  virtual TRT_DriftCircleContainer_PERS*   createPersistent (InDet::TRT_DriftCircleContainer* transCont);
  virtual InDet::TRT_DriftCircleContainer* createTransient ();

  // Must initialize ID helpers
  virtual StatusCode initialize();
  virtual AthenaPoolTopLevelTPCnvBase*	getTopLevelTPCnv() { return &m_TPConverter; }
};

#endif














