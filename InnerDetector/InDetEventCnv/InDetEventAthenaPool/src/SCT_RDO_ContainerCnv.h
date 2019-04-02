/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SCT_RDO_CONTAINERCNV_H
#define SCT_RDO_CONTAINERCNV_H

#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"

#include "InDetRawData/SCT_RDO_Container.h"

#include "InDetRawData/SCT_RDO_Collection.h"

#include "StoreGate/StoreGateSvc.h"

#include "SCT_RDO_ContainerCnv_p0.h"
#include "SCT1_RawDataContainerCnv_p1.h"
#include "SCT_RawDataContainerCnv_p1.h"
#include "SCT_RawDataContainerCnv_p2.h"
#include "SCT_RawDataContainerCnv_p3.h"
#include "SCT_RawDataContainerCnv_p4.h"

// the latest persistent representation type of DataCollection:
typedef  SCT_RawDataContainer_p4     SCT_RDO_Container_PERS;
typedef  SCT_RawDataContainerCnv_p4  SCT_RDO_ContainerCnv_PERS;

typedef  T_AthenaPoolCustomCnv<SCT_RDO_Container, SCT_RDO_Container_PERS >   SCT_RDO_ContainerCnvBase;

class SCT_RDO_ContainerCnv : public SCT_RDO_ContainerCnvBase {
  friend class CnvFactory<SCT_RDO_ContainerCnv >;

  // Converters need to be initialized (use ID helpers)
  // Thus they can't be local
  SCT_RDO_ContainerCnv_p0      m_converter_p0;
  SCT1_RawDataContainerCnv_p1  m_converter_TP1;
  SCT_RawDataContainerCnv_p1   m_converter_SCT_TP1;
  SCT_RawDataContainerCnv_p2   m_converter_SCT_TP2;
  SCT_RawDataContainerCnv_p3   m_converter_SCT_TP3;
  SCT_RawDataContainerCnv_p4   m_converter_SCT_TP4;
  SCT_RDO_ContainerCnv_PERS    m_converter_PERS;

  // Should not be needed at some point.
  StoreGateSvc*  m_storeGate;

protected:
  SCT_RDO_ContainerCnv (ISvcLocator* svcloc);
  virtual SCT_RDO_Container_PERS*   createPersistent (SCT_RDO_Container* transCont);
  virtual SCT_RDO_Container* createTransient ();

  // Must initialize ID helpers
  virtual StatusCode initialize();
};

#endif
