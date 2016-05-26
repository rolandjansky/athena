/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//Dear emacs, this is -*-c++-*-

#ifndef CALOATHENAPOOL_CALOCLUSTERCONTAINERCNV_H
#define CALOATHENAPOOL_CALOCLUSTERCONTAINERCNV_H

#include "CaloTPCnv/CaloClusterContainerCnv_p1.h"
#include "CaloTPCnv/CaloClusterContainerCnv_p2.h"
#include "CaloTPCnv/CaloClusterContainerCnv_p3.h"
#include "CaloTPCnv/CaloClusterContainerCnv_p4.h"
#include "CaloTPCnv/CaloClusterContainerCnv_p5.h"
#include "CaloTPCnv/CaloClusterContainerCnv_p6.h"
#include "CaloTPCnv/CaloClusterContainerCnv_p7.h"
#include "CaloEvent/CaloClusterContainer.h"
#include "CaloTPCnv/CaloClusterContainer_p1.h"
#include "CaloTPCnv/CaloClusterContainer_p2.h"
#include "CaloTPCnv/CaloClusterContainer_p3.h"
#include "CaloTPCnv/CaloClusterContainer_p4.h"
#include "CaloTPCnv/CaloClusterContainer_p5.h"
#include "CaloTPCnv/CaloClusterContainer_p6.h"
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"


typedef CaloClusterContainer_p7 CaloClusterContainerPERS;
typedef T_AthenaPoolCustomCnv<CaloClusterContainer,CaloClusterContainerPERS> CaloClusterContainerCnvBase;

/**
 @class CaloClusterContainerCnv
 @brief POOL Converter for CaloClusterContainer

**/

class CaloClusterContainerCnv : public CaloClusterContainerCnvBase 
{
  friend class CnvFactory<CaloClusterContainerCnv>;
 protected:
  CaloClusterContainerCnv(ISvcLocator*);
  virtual CaloClusterContainer* createTransient();
  virtual CaloClusterContainerPERS* createPersistent(CaloClusterContainer*);
 private:
  CaloClusterContainerCnv_p1 m_converter_p1;
  CaloClusterContainerCnv_p2 m_converter_p2;
  CaloClusterContainerCnv_p3 m_converter_p3;
  CaloClusterContainerCnv_p4 m_converter_p4;
  CaloClusterContainerCnv_p5 m_converter_p5;
  CaloClusterContainerCnv_p6 m_converter_p6;
  CaloClusterContainerCnv_p7 m_converter_p7;
  pool::Guid   m_p0_guid;
  pool::Guid   m_p1_guid;
  pool::Guid   m_p2_guid;
  pool::Guid   m_p3_guid;
  pool::Guid   m_p4_guid;
  pool::Guid   m_p5_guid;
  pool::Guid   m_p6_guid;
  pool::Guid   m_p7_guid;
};

#endif
