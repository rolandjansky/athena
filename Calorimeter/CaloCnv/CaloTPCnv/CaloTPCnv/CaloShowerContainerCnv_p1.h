//Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CALOATHENAPOOL_CALOSHOWERCONTAINERCNV_P1_H
#define CALOATHENAPOOL_CALOSHOWERCONTAINERCNV_P1_H


//#include "CaloEvent/CaloShowerContainer.h"
//#include "CaloTPCnv/CaloShowerContainer_p1.h"
#include "CaloTPCnv/CaloClusterMomentStoreCnv_p1.h"
#include "CaloTPCnv/CaloSamplingDataCnv_p1.h"
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"


class CaloShowerContainer;
class CaloShowerContainer_p1;



//typedef T_AthenaPoolTPCnvVector<CaloShowerContainer, CaloShowerContainer_p1, CaloShowerCnv_p1> CaloShowerContainerCnvBasep1_t;

class CaloShowerContainerCnv_p1
  : public T_AthenaPoolTPCnvBase<CaloShowerContainer, CaloShowerContainer_p1>
{
public:
  CaloShowerContainerCnv_p1();

  virtual void persToTrans(const CaloShowerContainer_p1*, CaloShowerContainer*, MsgStream &log) override final;
  virtual void transToPers(const CaloShowerContainer*, CaloShowerContainer_p1*, MsgStream &log) override final;

 private:
  CaloClusterMomentStoreCnv_p1 m_momentStoreCnv;
  CaloSamplingDataCnv_p1 m_samplingDataCnv;

};


#endif
