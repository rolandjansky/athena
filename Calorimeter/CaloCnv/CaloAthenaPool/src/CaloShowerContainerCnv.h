/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//Dear emacs, this is -*-c++-*- 
#ifndef CALOATHENAPOOL_CALOSHOWERCONTAINERCNV_H
#define CALOATHENAPOOL_CALOSHOWERCONTAINERCNV_H

#include "CaloTPCnv/CaloShowerContainerCnv_p1.h"
#include "CaloTPCnv/CaloShowerContainerCnv_p2.h"
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"

class CaloShowerContainer;

typedef CaloShowerContainer_p2 CaloShowerContainerPERS;

typedef T_AthenaPoolCustomCnv<CaloShowerContainer,CaloShowerContainerPERS> CaloShowerContainerCnvBase;

/**
 @class CaloShowerContainerCnv
 @brief POOL Converter for CaloShowerContainer 

**/
class CaloShowerContainerCnv : public CaloShowerContainerCnvBase 
{
  friend class CnvFactory<CaloShowerContainerCnv>;
 protected:
  CaloShowerContainerCnv(ISvcLocator*);
  virtual CaloShowerContainer* createTransient();
  virtual CaloShowerContainerPERS* createPersistent(CaloShowerContainer*);
 private:
  CaloShowerContainerCnv_p1 m_converter1;
  CaloShowerContainerCnv_p2 m_converter2;
  pool::Guid   m_p0_guid;
  pool::Guid   m_p1_guid;
  pool::Guid   m_p2_guid;

};

#endif
