/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SCT_MATERIALMANAGER_H
#define SCT_MATERIALMANAGER_H

// SCT_MaterialManager. This provides an interface to the InDetMaterialManager which in turn
// is an interface to GeoModel Material Manager with some additional functionality.

class GeoMaterial;
class GeoElement;
class InDetMaterialManager;

#include <string>

class SCT_MaterialManager
{

public:

  SCT_MaterialManager();
  
  static void reinit();

  const GeoMaterial* getMaterial(const std::string & materialName) const;
  const GeoElement* getElement(const std::string & elementName) const;

  const GeoMaterial* getMaterial(const std::string & originalMaterial, 
				 double density,  
				 const std::string & newName = "");
  const GeoMaterial *getMaterialForVolume(const std::string & materialName, double volume);


  // Default gas material
  const GeoMaterial* gasMaterial() const;

  // Allow changing gas material. (Will be air if never called)
  // Will affect all instances. 
  static void setGasMaterial(const GeoMaterial* material);
  static void setGasMaterial(const std::string & materialName);


private:
  void loadMaterials();
  void addMaterial(GeoMaterial* material);

  static InDetMaterialManager *s_materialManager;
  static const GeoMaterial* s_gasMaterial;

};


#endif // SCT_MATERIALMANAGER_H
