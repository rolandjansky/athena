/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "PixelSCTDBHelper/GeoPixelHelperMaterialMap.h"
#include "RDBAccessSvc/IRDBRecordset.h"

#include <iostream>

GeoPixelHelperMaterialMap::Key::Key(int layerdisk_in, int typenum_in, const std::string & volumeName_in):
  layerdisk(layerdisk_in),
  typenum(typenum_in),
  volumeName(volumeName_in)
{}      

bool GeoPixelHelperMaterialMap::Key::operator<(const GeoPixelHelperMaterialMap::Key &rhs) const
{
  return 
    ((volumeName < rhs.volumeName) ||
     (volumeName == rhs.volumeName && layerdisk < rhs.layerdisk) ||
     (volumeName == rhs.volumeName && layerdisk == rhs.layerdisk && typenum < rhs.typenum));
}

GeoPixelHelperMaterialMap::GeoPixelHelperMaterialMap(const OraclePixGeoAccessor * geoAccessor, IRDBRecordset_ptr mapTable)
{

  for (unsigned int i = 0; i < geoAccessor->getTableSize(mapTable); i++) {
    int layerdisk = geoAccessor->getInt(mapTable,"LAYERDISK",i);
    int typenum    = geoAccessor->getInt(mapTable,"TYPENUM",i);
    std::string volumeName = geoAccessor->getString(mapTable,"VOLUMENAME",i);
    std::string material   = geoAccessor->getString(mapTable,"MATERIAL",i);
    addMaterial(layerdisk, typenum, volumeName, material);
  }
}

std::string GeoPixelHelperMaterialMap::getMaterial(int layerdisk, int typenum, const std::string & volumeName) const
{
  // If not found try (layerdisk, 0) then (0, typenum), then (0,0)
  mapType::const_iterator iter;
  iter = m_matmap.find(Key(layerdisk, typenum, volumeName));
  if (iter == m_matmap.end() && typenum) {
    iter = m_matmap.find(Key(layerdisk, 0, volumeName));
  }
  if (iter == m_matmap.end() && layerdisk) {
    iter = m_matmap.find(Key(0, typenum, volumeName));
  }
  if (iter == m_matmap.end() && typenum && layerdisk) {
    iter = m_matmap.find(Key(0, 0, volumeName));
  }
  if (iter != m_matmap.end()) {
    return iter->second;
  } else {
    std::cout << "ERROR: GeoPixelHelperMaterialMap::getMaterial Cannot find material for volumeName: " << volumeName << std::endl;
    return "";
  }
}


void GeoPixelHelperMaterialMap::addMaterial(int layerdisk, int typenum, const std::string & volumeName, const std::string & materialName) 
{
  m_matmap[Key(layerdisk, typenum, volumeName)] = materialName;
}
