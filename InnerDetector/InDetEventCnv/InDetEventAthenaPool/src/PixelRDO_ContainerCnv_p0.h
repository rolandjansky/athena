/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PIXELRDO_CONTAINERCNV_P0_H
#define PIXELRDO_CONTAINERCNV_P0_H

#include "AthContainers/DataVector.h"

#include "InDetRawData/PixelRDO_Container.h"
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
#include "InDetRawData/Pixel1RawData.h"

class PixelID;

typedef  DataVector<InDetRawDataCollection< Pixel1RawData > >  PixelRDO_Container_p0;

class MsgStream;
class PixelRDO_ContainerCnv_p0  : public T_AthenaPoolTPCnvBase<PixelRDO_Container, PixelRDO_Container_p0> {
   const PixelID*  m_pixId;
public:
  virtual void   persToTrans(const PixelRDO_Container_p0*, PixelRDO_Container*, MsgStream&) {
    // everything is done in createTransient()
  }

  virtual void   transToPers(const PixelRDO_Container*, PixelRDO_Container_p0*, MsgStream&)  {
    throw std::runtime_error("Writing Pixel RDOs in the old format is not supported");
  }

  virtual PixelRDO_Container* createTransient(const PixelRDO_Container_p0* persObj, MsgStream& log);

  // ID helper can't be used in the constructor, need initialize()
  void initialize(const PixelID *idhelper) { m_pixId = idhelper; }
};

#endif/*PIXELRDO_CONTAINERCNV_P0_H*/
