//Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CALOTPCNV_CALOSHOWERCONTAINERCNV_P2_H
#define CALOTPCNV_CALOSHOWERCONTAINERCNV_P2_H

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
#include "CaloTPCnv/CaloClusterMomentContainerCnv_p1.h"
#include "CaloTPCnv/CaloSamplingDataContainerCnv_p1.h"
#include "CaloTPCnv/CaloShowerContainer_p2.h"
#include "AthenaPoolCnvSvc/ITPConverter.h"
#include "GaudiKernel/MsgStream.h"

class CaloShowerContainer;

class CaloShowerContainerCnv_p2
  : public T_AthenaPoolTPCnvConstBase<CaloShowerContainer, CaloShowerContainer_p2>
{
public:
  using base_class::transToPers;
  using base_class::persToTrans;


  virtual
  void persToTrans (const CaloShowerContainer_p2* pers,
                    CaloShowerContainer* trans,
                    MsgStream &) const override;


  virtual
  void transToPers (const CaloShowerContainer* trans,
                    CaloShowerContainer_p2* pers,
                    MsgStream &) const override;


private:
  CaloClusterMomentContainerCnv_p1 m_momentContainerCnv;
  CaloSamplingDataContainerCnv_p1 m_samplingContainerCnv;
};


#endif
