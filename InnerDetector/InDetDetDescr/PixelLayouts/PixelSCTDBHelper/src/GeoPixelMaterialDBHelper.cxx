/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//
// This class builds the Ladder services (Omega + Al tube + glue + part of pigtail + connector)
//
//
#include "PixelSCTDBHelper/GeoPixelMaterialDBHelper.h"
#include "PixelSCTDBHelper/GeoPixelHelperMaterialMap.h"

#include "AthenaKernel/MsgStreamMember.h"
#include "InDetGeoModelUtils/OraclePixGeoAccessor.h"
#include "InDetGeoModelUtils/InDetMaterialManager.h" 

#include <cmath>
#include <sstream>
#include <vector>


GeoPixelMaterialDBHelper::GeoPixelMaterialDBHelper( PixelGeoBuilderBasics* basics):
  PixelGeoBuilder(basics)
{

  // Create material map
  IRDBRecordset_ptr materialMapTable_ptr = geoAccessor().getTable("PixelMaterialMap");
  m_materialMap = new GeoPixelHelperMaterialMap( &geoAccessor(), materialMapTable_ptr);

}

GeoPixelMaterialDBHelper::~GeoPixelMaterialDBHelper()
{
  delete m_materialMap;
}

/////////////////////////////////////////////////////
//
//  Materials
//
/////////////////////////////////////////////////////

std::string GeoPixelMaterialDBHelper::getMaterialName(const std::string & volumeName, int layerdisk, int typenum) const 
{
  return m_materialMap->getMaterial(layerdisk, typenum, volumeName);
}
