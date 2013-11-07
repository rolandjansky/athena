/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SCT_SLHC_MATERIALMANAGER_H
#define SCT_SLHC_MATERIALMANAGER_H

// SCT_SLHC_MaterialManager. This provides an interface to the InDetMaterialManager which in turn
// is an interface to GeoModel Material Manager with some additional functionality.

class GeoMaterial;
class GeoElement;
#include <string>

#include "InDetGeoModelUtils/InDetMaterialManager.h"

namespace InDetDDSLHC {

class SCT_DataBase;
class SCT_GeoModelAthenaComps;

class SCT_MaterialManager : public InDetMaterialManager
{

public:

  SCT_MaterialManager(const SCT_DataBase * sctdb, const SCT_GeoModelAthenaComps * athenaComps);

  // Default gas material
  const GeoMaterial* gasMaterial() const;

  // Allow changing gas material. (Will be air if never called)
  void setGasMaterial(const GeoMaterial* material);
  void setGasMaterial(const std::string & materialName);

private:
  void loadMaterials();
  const GeoMaterial* m_gasMaterial;

};

}
#endif // SCT_SLHC_MATERIALMANAGER_H
