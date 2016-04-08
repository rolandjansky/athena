/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Header file for class VP1RawDataHandle_PixelRDO           //
//                                                            //
//  Description: Handle for Pixel RDO's                       //
//                                                            //
//  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//  Initial version: April 2008 (rewritten January 2009)      //
//                                                            //
////////////////////////////////////////////////////////////////

#ifndef VP1RAWDATAHANDLE_PIXELRDO_H
#define VP1RAWDATAHANDLE_PIXELRDO_H

#include "VP1RawDataSystems/VP1RawDataHandleBase.h"
class PixelRDORawData;

namespace InDetDD { class SiDetectorElement; }

class VP1RawDataHandle_PixelRDO : public VP1RawDataHandleBase {
public:

  VP1RawDataHandle_PixelRDO(VP1RawDataCollBase*,const PixelRDORawData*);
  virtual ~VP1RawDataHandle_PixelRDO();
  QStringList clicked(bool verbose) const;

  VP1RawDataFlags::InDetPartsFlags inInDetParts() const;

protected:
  SoNode * buildShape();
  SoTransform * buildTransform();
  const PixelRDORawData* m_data;
  const InDetDD::SiDetectorElement * element() const;//null in case of errors
};

#endif
