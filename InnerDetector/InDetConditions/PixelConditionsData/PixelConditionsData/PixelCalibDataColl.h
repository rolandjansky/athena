/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PIXELCALIBDATACOLL_H
#define PIXELCALIBDATACOLL_H
#include "DataModel/DataVector.h"
#include "PixelConditionsData/PixelCalibData.h"
#include "CLIDSvc/CLASS_DEF.h"

/** This typedef represents a collection of PixelCalibData objects. 
It is a DataVector. It can be saved
to storegate and persistified using POOL */
 
typedef DataVector<PixelCalib::PixelCalibData> PixelCalibDataColl;
 
CLASS_DEF(PixelCalibDataColl, 146316417, 1 )

#endif // PIXELCALIBDATACOLL_H
 
