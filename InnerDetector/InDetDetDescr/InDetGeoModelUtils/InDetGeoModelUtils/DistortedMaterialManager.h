/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
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
  StoredMaterialManager * materialManager() {return  m_materialManager;}

private:
  StoredMaterialManager * m_materialManager;
  IRDBRecordset_ptr  m_xMatTable;
};


} // endnamespace

#endif // InDetGeoModelUtils_DistortedMaterialManager_h
