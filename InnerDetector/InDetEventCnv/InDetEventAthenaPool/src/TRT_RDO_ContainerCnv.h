/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRT_RDO_CONTAINERCNV_H
#define TRT_RDO_CONTAINERCNV_H

#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"

#include "InDetRawData/TRT_RDO_Container.h"

#include "InDetRawData/TRT_RDO_Collection.h"

#include "StoreGate/StoreGateSvc.h"

#include "TRT_RDO_ContainerCnv_p0.h"
#include "TRT_RDO_ContainerCnv_p1.h"
#include "TRT_LoLumRawDataContainerCnv_p1.h"
#include "TRT_LoLumRawDataContainerCnv_p2.h"
#include "TRT_LoLumRawDataContainerCnv_p3.h"


// the latest persistent representation type of DataCollection:
//typedef  InDetRawDataContainer_p2         TRT_RDO_Container_PERS;
//typedef  TRT_LoLumRawDataContainerCnv_p2  TRT_RDO_ContainerCnv_PERS;
typedef  InDetRawDataContainer_p3         TRT_RDO_Container_PERS;
typedef  TRT_LoLumRawDataContainerCnv_p3  TRT_RDO_ContainerCnv_PERS;

typedef  T_AthenaPoolCustomCnv<TRT_RDO_Container, TRT_RDO_Container_PERS >   TRT_RDO_ContainerCnvBase;

class TRT_RDO_ContainerCnv : public TRT_RDO_ContainerCnvBase {
  friend class CnvFactory<TRT_RDO_ContainerCnv >;

  // Converters need to be initialized (use ID helpers)
  // Thus they can't be local
  TRT_RDO_ContainerCnv_p1           m_converter_p1;
  TRT_RDO_ContainerCnv_p0           m_converter_p0;
  TRT_LoLumRawDataContainerCnv_p1   m_converter_TP1;
  TRT_LoLumRawDataContainerCnv_p2   m_converter_TP2;
  TRT_LoLumRawDataContainerCnv_p3   m_converter_TP3;
  TRT_RDO_ContainerCnv_PERS         m_converter_PERS;

  // Should not be needed at some point.
  StoreGateSvc*  m_storeGate;

public:
  TRT_RDO_ContainerCnv (ISvcLocator* svcloc)
    : TRT_RDO_ContainerCnvBase(svcloc, "TRT_RDO_ContainerCnv"),
      m_converter_p1(),
      m_converter_p0(),
      m_storeGate(nullptr)
  {}
protected:
  virtual TRT_RDO_Container_PERS*   createPersistent (TRT_RDO_Container* transCont);
  virtual TRT_RDO_Container* createTransient ();

  // Must initialize ID helpers
  virtual StatusCode initialize();

};

#endif
