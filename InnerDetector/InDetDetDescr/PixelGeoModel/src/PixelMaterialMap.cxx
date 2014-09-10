/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "PixelGeoModel/PixelMaterialMap.h"
#include "GeometryDBSvc/IGeometryDBSvc.h"
#include "RDBAccessSvc/IRDBRecordset.h"

#include <iostream>

PixelMaterialMap::Key::Key(int layerdisk_in, int typenum_in, const std::string & volumeName_in):
  layerdisk(layerdisk_in),
  typenum(typenum_in),
  volumeName(volumeName_in)
{}      

bool
PixelMaterialMap::Key::operator<(const PixelMaterialMap::Key &rhs) const
{
  return 
    ((volumeName < rhs.volumeName) ||
     (volumeName == rhs.volumeName && layerdisk < rhs.layerdisk) ||
     (volumeName == rhs.volumeName && layerdisk == rhs.layerdisk && typenum < rhs.typenum));
}


PixelMaterialMap::PixelMaterialMap(const IGeometryDBSvc * db, IRDBRecordset_ptr mapTable)
{
  for (unsigned int i = 0; i < db->getTableSize(mapTable); i++) {
    int layerdisk = db->getInt(mapTable,"LAYERDISK",i);
    int typenum    = db->getInt(mapTable,"TYPENUM",i);
    std::string volumeName = db->getString(mapTable,"VOLUMENAME",i);
    std::string material   = db->getString(mapTable,"MATERIAL",i);
    addMaterial(layerdisk, typenum, volumeName, material);
  }
}

std::string
PixelMaterialMap::getMaterial(int layerdisk, int typenum, const std::string & volumeName) const
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
    std::cout << "ERROR: PixelMaterialMap::getMaterial Cannot find material for volumeName: " << volumeName << std::endl;
    return "";
  }
}


void
PixelMaterialMap::addMaterial(int layerdisk, int typenum, const std::string & volumeName, const std::string & materialName) 
{
  m_matmap[Key(layerdisk, typenum, volumeName)] = materialName;
}
