/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

//
// This is the base class for all the pieces of the Pixel detector.
//
//
#ifndef GEOVPIXELFACTORY_H
#define GEOVPIXELFACTORY_H

#include "GeoModelKernel/GeoFullPhysVol.h"
#include "GeoModelKernel/GeoLogVol.h"
#include "PixelGeometryManager.h"
#include "InDetGeoModelUtils/InDetMaterialManager.h"
#include "CxxUtils/checker_macros.h"

// fwd declaration
namespace InDetDD {class PixelDetectorManager;}

class GeoVPixelFactory {
 public:
  GeoVPixelFactory(InDetDD::PixelDetectorManager* ddmgr,
                   PixelGeometryManager* mgr);
  virtual ~GeoVPixelFactory();
  virtual GeoVPhysVol* Build()=0;
     
 protected:
  PixelGeometryManager* m_gmt_mgr;
  InDetMaterialManager* m_mat_mgr;
  InDetDD::PixelDetectorManager* m_DDmgr;
  const double m_epsilon;

 private:
};


#endif
