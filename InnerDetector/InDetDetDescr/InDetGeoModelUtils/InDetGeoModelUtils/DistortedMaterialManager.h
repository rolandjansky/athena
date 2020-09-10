/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INDETGEOMODELUTILS_DISTORTEDMATERIALMANAGER_H
#define INDETGEOMODELUTILS_DISTORTEDMATERIALMANAGER_H

class StoredMaterialManager;
#include "RDBAccessSvc/IRDBAccessSvc.h"

namespace InDetDD {

class DistortedMaterialManager
{
public:
  DistortedMaterialManager();  
  IRDBRecordset_ptr  extraMaterialTable() const {return  m_xMatTable;}
  const StoredMaterialManager * materialManager() const {return  m_materialManager;}

private:
  const StoredMaterialManager * m_materialManager;
  IRDBRecordset_ptr  m_xMatTable;
};


} // endnamespace

#endif // InDetGeoModelUtils_DistortedMaterialManager_h
