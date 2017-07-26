/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PIXELRDO_CONTAINERCNV_H
#define PIXELRDO_CONTAINERCNV_H

#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"

#include "InDetRawData/PixelRDO_Container.h"

#include "InDetRawData/PixelRDO_Collection.h"

#include "StoreGate/StoreGateSvc.h"

#include "PixelRDO_ContainerCnv_p0.h"
#include "Pixel1RawDataContainerCnv_p1.h"
#include "Pixel1RawDataContainerCnv_p2.h"
#include "CreateTransientTemplate.h"

// the latest persistent representation type of DataCollection:
typedef  InDetRawDataContainer_p2      PixelRDO_Container_PERS;
typedef  Pixel1RawDataContainerCnv_p2  PixelRDO_ContainerCnv_PERS;

typedef  T_AthenaPoolCustomCnv<PixelRDO_Container, PixelRDO_Container_PERS >   PixelRDO_ContainerCnvBase;

class PixelRDO_ContainerCnv : public PixelRDO_ContainerCnvBase {
  friend class CnvFactory<PixelRDO_ContainerCnv >;

  // Converters need to be initialized (use ID helpers)
  // Thus they can't be local
  PixelRDO_ContainerCnv_p0       m_converter_p0;
  Pixel1RawDataContainerCnv_p1   m_converter_TP1;
  Pixel1RawDataContainerCnv_p2   m_converter_TP2;
  PixelRDO_ContainerCnv_PERS     m_converter_PERS;

  // Should not be needed at some point.
  StoreGateSvc*  m_storeGate;

protected:
  PixelRDO_ContainerCnv (ISvcLocator* svcloc)
    : PixelRDO_ContainerCnvBase(svcloc, "PixelRDO_ContainerCnv"),
      m_converter_p0(),
      m_storeGate(nullptr)
  {}
  virtual PixelRDO_Container_PERS*   createPersistent (PixelRDO_Container* transCont);
  virtual PixelRDO_Container* createTransient ();

  // Must initialize ID helpers
  virtual StatusCode initialize();
};

#endif
