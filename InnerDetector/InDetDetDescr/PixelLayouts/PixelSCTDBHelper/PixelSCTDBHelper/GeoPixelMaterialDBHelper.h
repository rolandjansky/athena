/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GEOPIXELMATERIALDBHELPER_H
#define GEOPIXELMATERIALDBHELPER_H

#include "CLHEP/Geometry/Point3D.h"
#include <sstream>

#include "PixelGeoModel/PixelGeoBuilder.h"

#include "AthenaKernel/MsgStreamMember.h"
#include "InDetGeoModelUtils/OraclePixGeoAccessor.h"
#include "InDetGeoModelUtils/InDetMaterialManager.h" 

  class GeoPixelHelperMaterialMap;

  class GeoPixelMaterialDBHelper :  public PixelGeoBuilder {
  public:
    GeoPixelMaterialDBHelper( PixelGeoBuilderBasics* basics);
    ~GeoPixelMaterialDBHelper();
    
    std::string getMaterialName(const std::string & volumeName, int layerdisk=0, int typenum=0) const;

  private:
    GeoPixelHelperMaterialMap* m_materialMap;

  };


#endif
