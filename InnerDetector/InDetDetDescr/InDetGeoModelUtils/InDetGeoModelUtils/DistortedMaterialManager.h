/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef InDetGeoModelUtils_DistortedMaterialManager_h
#define InDetGeoModelUtils_DistortedMaterialManager_h

class AbsMaterialManager;
#include "RDBAccessSvc/IRDBAccessSvc.h"

namespace InDetDD {

class DistortedMaterialManager
{
public:
  DistortedMaterialManager();  
  IRDBRecordset_ptr  extraMaterialTable() const {return  m_xMatTable;}
  const AbsMaterialManager * materialManager() const {return  m_materialManager;}

private:
  const AbsMaterialManager * m_materialManager;
  IRDBRecordset_ptr  m_xMatTable;
};


} // endnamespace

#endif // InDetGeoModelUtils_DistortedMaterialManager_h
