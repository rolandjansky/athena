/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PixelMaterialMap_H
#define PixelMaterialMap_H

// Class to interpret and query table PixelMaterialMap

#include <string>
#include <map>
#include "RDBAccessSvc/IRDBAccessSvc.h"

class IGeometryDBSvc;

class PixelMaterialMap
{

public:
  PixelMaterialMap(const IGeometryDBSvc * db, const IRDBRecordset_ptr& mapTable);

  void addMaterial(int layerdisk, int typenum, const std::string & volumeName, const std::string & materialName);
  std::string getMaterial(int layerdisk, int typenum, const std::string & volumeName) const;

private:
  class Key 
  {
  public:
    Key(int layerdisk_in, int typenum_in, const std::string & volumeName_in);
    int layerdisk;
    int typenum;
    std::string volumeName;
    bool operator<(const Key &rhs) const;
  };

  typedef std::map<Key, std::string> mapType;
  mapType m_matmap;

};

#endif // PixelMaterialMap
