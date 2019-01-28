/*
   Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
 */

#include "InDetGeoModelUtils/InDetMaterialManager.h"
#include "InDetGeoModelUtils/InDetDDAthenaComps.h"
#include "GeoModelInterfaces/AbsMaterialManager.h"
#include "GeoModelInterfaces/StoredMaterialManager.h"
#include "GeoModelKernel/GeoMaterial.h"
#include "GeoModelKernel/GeoElement.h"
#include "GeoModelKernel/Units.h"
#include "GaudiKernel/SystemOfUnits.h"
#include "RDBAccessSvc/IRDBRecordset.h"
#include "RDBAccessSvc/IRDBRecord.h"
#include "GeometryDBSvc/IGeometryDBSvc.h"
#include "StoreGate/StoreGateSvc.h"

#include <iostream>
#include <iomanip>
#include <cmath>
#include <stdexcept>

// Constructor
InDetMaterialManager::InDetMaterialManager(const std::string& managerName,
                                           StoreGateSvc* detStore)
  : m_managerName(managerName),
  m_msg(managerName),
  m_extraFunctionality(false),
  m_athenaComps(0) {
  m_materialManager = retrieveManager(detStore);
}

// Constructor
InDetMaterialManager::InDetMaterialManager(const std::string& managerName,
                                           StoreGateSvc* detStore,
                                           IRDBRecordset_ptr weightTable,
                                           const std::string& space,
                                           bool extraFunctionality)
  : m_managerName(managerName),
  m_msg(managerName),
  m_extraFunctionality(extraFunctionality),
  m_athenaComps(0) {
  m_materialManager = retrieveManager(detStore);

  if (weightTable) addWeightTable(weightTable, space);

  // For testing we add a few materials.
  //m_weightMap["sct::CoolingBlock"] = MaterialByWeight(2.418*CLHEP::gram);
  //m_weightMap["sct::CoolingBlock"] = MaterialByWeight(2*CLHEP::gram);
  //m_weightMap["sct::BrlHybrid"] = MaterialByWeight("sct::Hybrid", 8*CLHEP::gram);
  //m_weightMap["sct::FwdHybrid"] = MaterialByWeight("std::Carbon", 7.662*CLHEP::gram);
}

InDetMaterialManager::InDetMaterialManager(const std::string& managerName, StoreGateSvc* detStore,
                                           IRDBRecordset_ptr weightTable,
                                           IRDBRecordset_ptr compositionTable,
                                           const std::string& space)
  : m_managerName(managerName),
  m_msg(managerName),
  m_extraFunctionality(true),
  m_athenaComps(0) {
  m_materialManager = retrieveManager(detStore);

  if (weightTable) addWeightTable(weightTable, space);
  if (compositionTable) addCompositionTable(compositionTable, space);
}

InDetMaterialManager::InDetMaterialManager(const std::string& managerName,
                                           const InDetDD::AthenaComps* athenaComps)
  : m_managerName(managerName),
  m_msg(managerName),
  m_extraFunctionality(true),
  m_athenaComps(athenaComps) {
  m_materialManager = retrieveManager(athenaComps->detStore());
  addTextFileMaterials();
}

InDetMaterialManager::~InDetMaterialManager() {
  // Dereference the materials.
  MaterialStore::const_iterator iter;
  for (iter = m_store.begin(); iter != m_store.end(); ++iter) {
    iter->second->unref();
  }
}

const AbsMaterialManager*
InDetMaterialManager::retrieveManager(StoreGateSvc* detStore) {
  const StoredMaterialManager* theGeoMaterialManager = nullptr;

  if (StatusCode::SUCCESS != detStore->retrieve(theGeoMaterialManager, "MATERIALS")) {
    msg(MSG::FATAL) << "Cannot locate Materials";
  }

  return theGeoMaterialManager;
}

const GeoElement*
InDetMaterialManager::getElement(const std::string& elementName) const {
  return m_materialManager->getElement(elementName);
}

const GeoMaterial*
InDetMaterialManager::getMaterial(const std::string& materialName) {
  return extraScaledMaterial(materialName, getMaterialInternal(materialName));
}

bool
InDetMaterialManager::hasMaterial(const std::string& materialName) const {
  return m_store.find(materialName) != m_store.end();
}

const GeoMaterial*
InDetMaterialManager::getMaterialInternal(const std::string& materialName) const {
  // First check local store of materials. If not found then get it from the GeoModel
  // manager.
  const GeoMaterial* material = getAdditionalMaterial(materialName);

  if (!material) {
    // This prints error message if not found.
    material = m_materialManager->getMaterial(materialName);
  }
  return material;
}

const GeoMaterial*
InDetMaterialManager::getAdditionalMaterial(const std::string& materialName) const {
  MaterialStore::const_iterator iter;
  if ((iter = m_store.find(materialName)) != m_store.end()) {
    return iter->second;
  } else {
    return 0;
  }
}

const GeoMaterial*
InDetMaterialManager::getCompositeMaterialForVolume(const std::string& newMatName,
                                                    const double volumeTot,
                                                    const double volume1, const std::string& matName1,
                                                    const double volume2, const std::string& matName2
                                                    ) {
  std::vector<std::string> baseMaterials;
  std::vector<double> fracWeight;
  baseMaterials.reserve(2);
  fracWeight.reserve(2);

  msg(MSG::DEBUG) << "Composite material : " << volumeTot / Gaudi::Units::cm3 << " = " << volume1 / Gaudi::Units::cm3 << " + " <<
    volume2 / Gaudi::Units::cm3 << endmsg;
  msg(MSG::DEBUG) << "Composite material : " << matName1 << " " << matName2 << endmsg;

  double density1, density2;

  MaterialWeightMap::const_iterator iter;
  if ((iter = m_weightMap.find(matName1)) != m_weightMap.end()) {
    const GeoMaterial* mat1 = getMaterialForVolume(matName1, volume1);
    density1 = mat1->getDensity();
    msg(MSG::DEBUG) << "Composite material 1 - weight : " << density1 / (GeoModelKernelUnits::gram / Gaudi::Units::cm3) << endmsg;
  } else {
    const GeoMaterial* mat1 = getMaterial(matName1);
    density1 = mat1->getDensity();
    msg(MSG::DEBUG) << "Composite material 1 - standard : " << density1 / (GeoModelKernelUnits::gram / Gaudi::Units::cm3) << endmsg;
  }

  if ((iter = m_weightMap.find(matName2)) != m_weightMap.end()) {
    const GeoMaterial* mat2 = getMaterialForVolume(matName2, volume2);
    density2 = mat2->getDensity();
    msg(MSG::DEBUG) << "Composite material 2 - weight : " << density2 / (GeoModelKernelUnits::gram / Gaudi::Units::cm3) << endmsg;
  } else {
    const GeoMaterial* mat2 = getMaterial(matName2);
    density2 = mat2->getDensity();
    msg(MSG::DEBUG) << "Composite material 2 - standard : " << density2 / (GeoModelKernelUnits::gram / Gaudi::Units::cm3) << endmsg;
  }

  double weight1 = density1 * volume1;
  double weight2 = density2 * volume2;
  double invWeightTot = 1.0 / (weight1 + weight2);

  double density = (weight1 + weight2) / volumeTot;

  double frac1 = weight1 / (weight1 + weight2);
  double frac2 = weight2 / (weight1 + weight2);
  double density_2 = 1.0 / (frac1 / density1 + frac2 / density2);
  double density_3 = (weight1 + weight2) / (volume1 + volume2);
  msg(MSG::DEBUG) << "-> weights : " << weight1 / (GeoModelKernelUnits::gram) << " " << weight2 / (GeoModelKernelUnits::gram) << endmsg;
  msg(MSG::DEBUG) << "-> density : " << density / (GeoModelKernelUnits::gram / Gaudi::Units::cm3) << "  " << density_2 /
  (GeoModelKernelUnits::gram / Gaudi::Units::cm3) << " " << density_3 / (GeoModelKernelUnits::gram / Gaudi::Units::cm3) << endmsg;


  baseMaterials.push_back(matName1);
  baseMaterials.push_back(matName2);
  fracWeight.push_back(weight1 * invWeightTot);
  fracWeight.push_back(weight2 * invWeightTot);

  return getMaterial(newMatName, baseMaterials, fracWeight, density);
}

// This creates a new material with specified density.

// If a newName is supplied it creates the new material even if the orginal material
// has the same density. It however first checks if the material with NewName exists.

// If no new name is supplied then it checks the density of
// the existing material. If it is consistent it returns the material.
// If it is different it creates a material with the string "Modified" added to the
// name.


const GeoMaterial*
InDetMaterialManager::getMaterial(const std::string& origMaterialName,
                                  double density,
                                  const std::string& newName) {
  return extraScaledMaterial(origMaterialName, newName,
                             getMaterialInternal(origMaterialName, density, newName));
}

const GeoMaterial*
InDetMaterialManager::getMaterialInternal(const std::string& origMaterialName,
                                          double density,
                                          const std::string& newName) {
  std::string newName2 = newName;
  bool newNameProvided = !newName2.empty();
  if (!newNameProvided) {
    newName2 = origMaterialName + "Modified";
  }

  const GeoMaterial* newMaterial = 0;

  // First see if we already have the modified material
  const GeoMaterial* material = getAdditionalMaterial(newName2);
  if (material) {
    if (!compareDensity(material->getDensity(), density)) {
      msg(MSG::WARNING) << "Density is not consistent for material " << newName2
                        << "  " << material->getDensity() / (GeoModelKernelUnits::gram / Gaudi::Units::cm3)
                        << " / " << density / (GeoModelKernelUnits::gram / Gaudi::Units::cm3) << endmsg;
    }
    newMaterial = material;
  } else {
    const GeoMaterial* origMaterial = getMaterialInternal(origMaterialName);
    newMaterial = origMaterial;
    if (origMaterial) {
      // If no new name was provided we check if the density is compatible
      // and if so we return the original material.
      if (newNameProvided || !compareDensity(origMaterial->getDensity(), density)) {
        // create new material
        GeoMaterial* newMaterialTmp = new GeoMaterial(newName2, density);
        newMaterialTmp->add(const_cast<GeoMaterial*>(origMaterial), 1.);
        addMaterial(newMaterialTmp);
        newMaterial = newMaterialTmp;
      }
    }
  }
  return newMaterial;
}

const GeoMaterial*
InDetMaterialManager::getMaterialScaled(const std::string& origMaterialName,
                                        double scaleFactor,
                                        const std::string& newName) {
  return extraScaledMaterial(origMaterialName, newName,
                             getMaterialScaledInternal(origMaterialName, scaleFactor, newName));
}

const GeoMaterial*
InDetMaterialManager::getMaterialScaledInternal(const std::string& origMaterialName,
                                                double scaleFactor,
                                                const std::string& newName) {
  // Don't allow large scale factors
  if (scaleFactor > 1000 || scaleFactor < 0.001) {
    msg(MSG::ERROR) << "Scale factor must be between 0.001 and 1000." << endmsg;
    return 0;
  }

  const GeoMaterial* origMaterial = getMaterialInternal(origMaterialName);

  // If scalefactor is 1 and no new name is requested
  // then just return the orginal material
  if (newName.empty() && scaleFactor == 1.) return origMaterial;

  const GeoMaterial* newMaterial = 0;

  if (origMaterial) {
    double density = origMaterial->getDensity() * scaleFactor;
    std::string newName2 = newName;
    if (newName2.empty()) {
      // Create name using the scale factor.
      int scaleInt = static_cast<int>(scaleFactor * 10000);
      int scale1 = scaleInt / 10000;
      int scale2 = scaleInt % 10000;

      std::ostringstream os;
      os << origMaterialName << scale1 << "_" << std::setw(4) << std::setfill('0') << scale2;
      newName2 = os.str();
    }

    newMaterial = getMaterialInternal(origMaterialName, density, newName2);
  }

  return newMaterial;
}

void
InDetMaterialManager::addMaterial(GeoMaterial* material) {
  std::string name(material->getName());
  if (m_store.find(name) != m_store.end()) {
    msg(MSG::WARNING) << "Ignoring attempt to redefine an existing material: " << name << endmsg;
    // Delete the material if it is not already ref counted.
    material->ref();
    material->unref();
    //std::cout << m_store[name] << std::endl;
  } else {
    material->lock();
    material->ref();
    m_store[name] = material;

    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Created new material: " << name << ", " << material->getDensity() /
      (Gaudi::Units::g / Gaudi::Units::cm3) << " g/cm3" << endmsg;
  }
}

bool
InDetMaterialManager::compareDensity(double d1, double d2) const {
  return(std::abs(d1 / d2 - 1.) < 1e-5);
}

void
InDetMaterialManager::addWeightTable(IRDBRecordset_ptr weightTable, const std::string& space) {
  if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Reading in weight table: " << weightTable->nodeName() << endmsg;
  // If not using geometryDBSvc revert to old version
  if (!db()) {
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "GeometryDBSvc not available. Using old version." << endmsg;
    addWeightTableOld(weightTable, space);
    return;
  }
  for (unsigned int i = 0; i < db()->getTableSize(weightTable); i++) {
    std::string materialName = db()->getString(weightTable, "MATERIAL", i);
    if (!space.empty()) {
      materialName = space + "::" + materialName;
    }
    std::string materialBase;
    if (db()->testField(weightTable, "BASEMATERIAL", i)) {
      materialBase = db()->getString(weightTable, "BASEMATERIAL", i);
    }
    double weight = db()->getDouble(weightTable, "WEIGHT", i) * GeoModelKernelUnits::gram;
    //std::cout << materialName << " " << materialBase << " " << weight/CLHEP::g <<  std::endl;

    bool linearWeightFlag = false;
    if (m_extraFunctionality && db()->testField(weightTable, "LINWEIGHTFLAG", i)) {
      linearWeightFlag = db()->getInt(weightTable, "LINWEIGHTFLAG", i);
    }

    if (m_weightMap.find(materialName) != m_weightMap.end()) {
      msg(MSG::WARNING) << "Material: " << materialName << " already exists in weight table" << endmsg;
    } else {
      msg(MSG::DEBUG) << "Adding " << materialName
                      << " weight " << weight
                      << " linearWeightFlag " << linearWeightFlag
                      << " raw weight " << db()->getDouble(weightTable, "WEIGHT", i)
                      << " m_extraFunctionality " << m_extraFunctionality
                      << " to weight table" << endmsg;
      m_weightMap[materialName] = MaterialByWeight(materialBase, weight, linearWeightFlag);
    }
  }
}

void
InDetMaterialManager::addWeightMaterial(std::string materialName, std::string materialBase, double weight,
                                        int linearWeightFlag) {
  // Weight in gr
  weight = weight * GeoModelKernelUnits::gram;

  if (m_weightMap.find(materialName) != m_weightMap.end()) {
    msg(MSG::WARNING) << "Material: " << materialName << " already exists in weight table" << endmsg;
  } else {
    msg(MSG::DEBUG) << "Adding " << materialName
                    << " weight " << weight
                    << " linearWeightFlag " << linearWeightFlag
                    << " to weight table" << endmsg;
    m_weightMap[materialName] = MaterialByWeight(materialBase, weight, linearWeightFlag);
  }
}

void
InDetMaterialManager::addWeightTableOld(IRDBRecordset_ptr weightTable, const std::string& space) {
  for (unsigned int i = 0; i < weightTable->size(); i++) {
    const IRDBRecord* record = (*weightTable)[i];
    std::string materialName = record->getString("MATERIAL");
    if (!space.empty()) {
      materialName = space + "::" + materialName;
    }
    std::string materialBase;
    if (!record->isFieldNull("BASEMATERIAL")) {
      materialBase = record->getString("BASEMATERIAL");
    }
    double weight = record->getDouble("WEIGHT") * GeoModelKernelUnits::gram;
    //std::cout << materialName << " " << materialBase << " " << weight/CLHEP::g <<  std::endl;

    bool linearWeightFlag = false;
    if (m_extraFunctionality) {
      linearWeightFlag = record->getInt("LINWEIGHTFLAG");
    }

    if (m_weightMap.find(materialName) != m_weightMap.end()) {
      msg(MSG::WARNING) << "Material: " << materialName << " already exists in weight table" << endmsg;
    } else {
      m_weightMap[materialName] = MaterialByWeight(materialBase, weight, linearWeightFlag);
    }
  }
}

void
InDetMaterialManager::addCompositionTable(IRDBRecordset_ptr compositionTable, const std::string& space) {
  if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Reading in composition table: " << compositionTable->nodeName() << endmsg;

  if (!db()) {
    msg(MSG::ERROR) << "GeometryDBSvc not available. Unable to read in composition table." << endmsg;
  }
  for (unsigned int i = 0; i < db()->getTableSize(compositionTable); i++) {
    std::string materialName = db()->getString(compositionTable, "MATERIAL", i);
    if (!space.empty()) {
      materialName = space + "::" + materialName;
    }

    std::string componentName = db()->getString(compositionTable, "COMPONENT", i);
    int count = db()->getInt(compositionTable, "COUNT", i);
    double factor = db()->getDouble(compositionTable, "FACTOR", i);
    double actualLength = db()->getDouble(compositionTable, "ACTUALLENGTH", i);

    m_matCompositionMap.insert(std::pair<std::string, MaterialComponent>(materialName,
                                                                         MaterialComponent(componentName,
                                                                                           count * factor,
                                                                                           actualLength)));
  }
}

void
InDetMaterialManager::addScalingTable(IRDBRecordset_ptr scalingTable) {
  if (!scalingTable) return;

  if (db()->getTableSize(scalingTable) == 0) return;

  if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Reading in extra material scaling table: " << scalingTable->nodeName() <<
    endmsg;
  if (!db()) {
    msg(MSG::ERROR) << "GeometryDBSvc not available. Unable to read in scaling table." << endmsg;
  }
  for (unsigned int i = 0; i < db()->getTableSize(scalingTable); i++) {
    std::string materialName = db()->getString(scalingTable, "MATERIAL", i);
    double scalingFactor = db()->getDouble(scalingTable, "FACTOR", i);

    if (msgLvl(MSG::DEBUG)) {
      if (scalingFactor >= 0 || scalingFactor == 1) {
        msg(MSG::DEBUG) << "Material " << materialName << " will be scaled by: " << scalingFactor << endmsg;
      } else {
        // -ve or scalefactor = 1 means will not be scaled.
        msg(MSG::DEBUG) << "Material " << materialName << " will be NOT be scaled." << endmsg;
      }
    }
    if (m_scalingMap.find(materialName) != m_scalingMap.end()) {
      msg(MSG::WARNING) << "Overriding material: " << materialName << " which already exists in scaling table" <<
      endmsg;
    }
    m_scalingMap[materialName] = scalingFactor;
  }
}

const GeoMaterial*
InDetMaterialManager::getMaterialForVolume(const std::string& materialName, double volume, const std::string& newName) {
  // Make sure we have a valid volume size.
  if (volume <= 0) {
    msg(MSG::ERROR) << "Invalid volume : " << volume << endmsg;
    return 0;
  }

  // Find if material is in the weight table.
  // If so we use the information to create a material with the
  // density calculated from the volume and weight. If a base material
  // is specified in the weight table, then a new material is made
  // which is the same as the base material but with the new
  // density. If no base material is specified then there should be a
  // material already existing with that name. If the existing material already has the
  // correct density it is used, otherwise a new material is created
  // with the string "Modified" added to the material name.

  MaterialWeightMap::const_iterator iter;
  if ((iter = m_weightMap.find(materialName)) != m_weightMap.end()) {
    const std::string& materialBase = iter->second.name;
    double weight = iter->second.weight;
    double density = weight / volume;
    if (iter->second.linearWeightFlag) {
      msg(MSG::ERROR) << "Material defined by linear weight cannot be created with getMaterialForVolume method: " <<
      materialName << endmsg;
    }

    if (msgLvl(MSG::VERBOSE)) {
      msg(MSG::VERBOSE)
        <<
      "Found material in weight table - name, base, weight(g), volume(cm3), density(g/cm3): "
        << materialName << ", "
        << materialBase << ", "
        << weight / GeoModelKernelUnits::gram << ", "
        << volume / Gaudi::Units::cm3 << ", "
        << density / (Gaudi::Units::g / Gaudi::Units::cm3) << endmsg;
    }

    if (materialBase.empty()) {
      return getMaterial(materialName, density, newName);
    } else {
      if (newName.empty()) {
        return getMaterial(materialBase, density, materialName);
      } else {
        return getMaterial(materialBase, density, newName);
      }
    }
  } else {
    // If not in the weight table we just return the material.
    // This is not an error.
    if (msgLvl(MSG::VERBOSE))
      msg(MSG::VERBOSE)
        << "Material not in weight table, using standard material: "
        << materialName
        << ", volume(cm3) = " << volume / Gaudi::Units::cm3
        << endmsg;
    return getMaterial(materialName);
  }
}

const GeoMaterial*
InDetMaterialManager::getMaterialForVolumeLength(const std::string& materialName, double volume, double length,
                                                 const std::string& newName) {
  // In the case there is no material composition table (MaterialCompositionMap) and no linear weights are used this
  // will
  // behave the same way as getMaterialForVolume.
  // If the material is in the MaterialCompositionMap it will build a material using the components
  // from that table. If any components are defined as a linear weight the length is used to calculate the
  // weight (ie linear weight * length).


  std::string name;
  if (newName.empty()) {
    name = materialName;
  } else {
    name = newName;
  }

  // Make sure we have a valid volume size.
  if (volume <= 0 || length <= 0) {
    msg(MSG::ERROR) << "Invalid volume or length : " << volume << ", " << length << endmsg;
    return 0;
  }

  // First look in the predefinded collections
  std::pair<MaterialCompositionMap::const_iterator, MaterialCompositionMap::const_iterator> iterRange;
  iterRange = m_matCompositionMap.equal_range(materialName);
  if (iterRange.first != m_matCompositionMap.end()) {
    if (msgLvl(MSG::VERBOSE)) {
      msg(MSG::VERBOSE)
        << "Found material in material composition table:" << materialName << endmsg;
    }

    std::vector<double> factors;
    std::vector<std::string> components;
    for (MaterialCompositionMap::const_iterator iter = iterRange.first; iter != iterRange.second; iter++) {
      double factorTmp = iter->second.factor;
      if (iter->second.actualLength > 0) factorTmp *= iter->second.actualLength / length;
      factors.push_back(factorTmp);
      components.push_back(iter->second.name);
    }
    return getMaterialForVolumeLength(name, components, factors, volume, length);
  }

  // Next look in weight table
  MaterialWeightMap::const_iterator iter;
  if ((iter = m_weightMap.find(materialName)) != m_weightMap.end()) {
    const std::string& materialBase = iter->second.name;
    double weight = iter->second.weight;
    double density = weight / volume;
    if (iter->second.linearWeightFlag) weight *= length;

    if (materialBase.empty()) {
      return getMaterial(materialName, density, newName);
    } else {
      return getMaterial(materialBase, density, name);
    }
  } else {
    // Otherwise we just return the material.
    // This is not an error.
    if (msgLvl(MSG::VERBOSE))
      msg(MSG::VERBOSE)
        << "Material not in weight table, using standard material: "
        << materialName
        << ", volume(cm3) = " << volume / Gaudi::Units::cm3
        << endmsg;
    return getMaterial(materialName);
  }
}

const GeoMaterial*
InDetMaterialManager::getMaterialForVolumeLength(const std::string& name,
                                                 const std::string& materialComponent,
                                                 double factor,
                                                 double volume,
                                                 double length) {
  std::vector<std::string> tmpMaterialComponents(1, materialComponent);
  std::vector<double> tmpFactors(1, factor);
  return getMaterialForVolumeLength(name, tmpMaterialComponents, tmpFactors, volume, length);
}

const GeoMaterial*
InDetMaterialManager::getMaterialForVolumeLength(const std::string& name,
                                                 const std::vector<std::string>& materialComponents,
                                                 const std::vector<double> factors,
                                                 double volume,
                                                 double length) {
  // Make sure we have a valid volume size.
  if (volume <= 0 || length <= 0) {
    msg(MSG::ERROR) << "Invalid volume or length : " << volume << ", " << length << endmsg;
    return 0;
  }

  if (!factors.empty() && factors.size() < materialComponents.size()) {
    msg(MSG::WARNING) << "getMaterialForVolumeLength: factor vector size too small. Setting remaining factors to 1." <<
    endmsg;
  }

  std::vector<std::string> baseMaterials;
  std::vector<double> fracWeight;
  baseMaterials.reserve(materialComponents.size());
  fracWeight.reserve(materialComponents.size());

  double totWeight = 0;
  for (unsigned int iComp = 0; iComp < materialComponents.size(); ++iComp) {
    const std::string& materialName = materialComponents[iComp];

    // First search in MaterialWeightMap
    MaterialWeightMap::const_iterator iter;
    if ((iter = m_weightMap.find(materialName)) != m_weightMap.end()) {
      const std::string& materialBase = iter->second.name;
      double weight = iter->second.weight;

      if (iComp < factors.size()) {
        weight *= factors[iComp];
      }
      msg(MSG::DEBUG) << "Material " << materialName
                      << " found in weight table, weight " << iter->second.weight / GeoModelKernelUnits::gram
                      << " factor " << factors[iComp]
                      << " w*fac*len " << weight * length / GeoModelKernelUnits::gram
                      << " basMat " << materialBase
                      << " linear? " << iter->second.linearWeightFlag << endmsg;

      if (iter->second.linearWeightFlag) weight *= length;
      if (materialBase.empty()) {
        // If no base material then name should refer to an already defined material
        baseMaterials.push_back(materialName);
      } else {
        baseMaterials.push_back(materialBase);
      }
      fracWeight.push_back(weight); // Will be normalized later.
      totWeight += weight;
    } else {
      // If not in the weight table we look for a regular material.
      // I don't think this would normally be intentional so we give a warning message.
      /*
         if (msgLvl(MSG::WARNING))
         msg(MSG::WARNING)
         << "Component material not in weight table, using standard material: "
         << materialName << " with weight= "
         << factors.at(iComp) * length
         << endmsg;
         const GeoMaterial * material = getMaterialInternal(materialName);
       */

      // In this case the factor should correspond to the linear weight
      double weight = factors.at(iComp) * length * GeoModelKernelUnits::gram;

      // If material not found, will get error message when attempting to make the material. So carry on here.
      baseMaterials.push_back(materialName);
      fracWeight.push_back(weight);
      totWeight += weight;
    }
  }

  if (msgLvl(MSG::VERBOSE)) {
    msg(MSG::VERBOSE) << "Creating material from multiple components: " << name << endmsg;
    for (unsigned int i = 0; i < materialComponents.size(); ++i) {
      msg(MSG::VERBOSE) << " Component " << i << ": Name = " << baseMaterials[i]
                        << " Weight(g) = " << fracWeight[i] / Gaudi::Units::g << endmsg;
    }
  }

  for (unsigned int i = 0; i < fracWeight.size(); ++i) {
    fracWeight[i] /= totWeight;
  }
  double density = totWeight / volume;

  return getMaterial(name, baseMaterials, fracWeight, density);
}

// Add materials assuming they simply occupy the same volume.
/*
   const GeoMaterial*
   InDetMaterialManager::getMaterial(const std::vector<const GeoMaterial *> & materialComponents,
                  const std::string & newName)
   {
   const GeoMaterial * newMaterial = 0;
   std::vector<double> fracWeight;
   fracWeight.reserve(materialComponents.size());

   for (unsigned int i = 0; i < materialComponents.size(); i++) {
    const GeoMaterial * origMaterial = materialComponents[i];
    double weight = origMaterial->getDensity();
    fracWeight.push_back(weight);
    totWeight += weight;
   }
   for (unsigned int i = 0; i < fracWeight.size(); ++i) {
    fracWeight[i] /= totWeight;
   }
   return getMaterial(materialComponents, fracWeight, totWeight, newName);
   }

   const GeoMaterial*
   InDetMaterialManager::getMaterial(const std::vector<std::string> & materialComponents,
                  const std::string & newName)
   {
   const GeoMaterial * newMaterial = 0;

   // First see if we already have the modified material
   const GeoMaterial* material = getAdditionalMaterial(newName);

   for (unsigned int i = 0; i < materialComponents.size(); i++) {
    const GeoMaterial * origMaterial = getMaterial(materialComponents[i]);
    components.push_back(origMaterial);
   }
   return getMaterial(components,  newName);
   }
 */


const GeoMaterial*
InDetMaterialManager::getMaterial(const std::string& name,
                                  const std::vector<std::string>& materialComponents,
                                  const std::vector<double>& fracWeight,
                                  double density) {
  return extraScaledMaterial(name, getMaterialInternal(name, materialComponents, fracWeight, density));
}

const GeoMaterial*
InDetMaterialManager::getMaterialInternal(const std::string& name,
                                          const std::vector<std::string>& materialComponents,
                                          const std::vector<double>& fracWeight,
                                          double density) {
  const GeoMaterial* newMaterial = 0;

  // First see if we already have the material
  const GeoMaterial* material = getAdditionalMaterial(name);

  if (material) {
    if (!compareDensity(material->getDensity(), density)) {
      msg(MSG::WARNING) << "Density is not consistent for material " << name << endmsg;
    }
    newMaterial = material;
  } else {
    GeoMaterial* newMaterialTmp = new GeoMaterial(name, density);
    for (unsigned int i = 0; i < materialComponents.size(); i++) {
      const GeoMaterial* origMaterial = getMaterialInternal(materialComponents[i]);
      if (origMaterial) {
        newMaterialTmp->add(const_cast<GeoMaterial*>(origMaterial), fracWeight[i]);
      } else {
        msg(MSG::ERROR) << "Material component missing " << materialComponents[i] << endmsg;
      }
    }
    addMaterial(newMaterialTmp);
    newMaterial = newMaterialTmp;
  }
  return newMaterial;
}

const IGeometryDBSvc*
InDetMaterialManager::db() {
  if (m_athenaComps) return m_athenaComps->geomDB();

  return 0;
}

void
InDetMaterialManager::addTextFileMaterials() {
  const std::string materialTable = "ExtraMaterials";
  const std::string componentsTable = "ExtraMatComponents";

  // Look for tables ExtraMaterials and ExtraMatComponents.
  // These are text file only tables where extra materials are desired or
  // one wants to override some database ones.
  if (!db() || !db()->testField("", "TableSize:" + materialTable) || !db()->getTableSize(materialTable)
      || !db()->testField("", "TableSize:" + componentsTable) || !db()->getTableSize(componentsTable)) return;


  msg(MSG::INFO) << "Extra materials being read in from text file." << endmsg;

  typedef std::map<std::string, MaterialDef> MatMap;
  MatMap materials;

  // read in material table
  for (unsigned int iMat = 0; iMat < db()->getTableSize(materialTable); iMat++) {
    std::string materialName = db()->getString(materialTable, "NAME", iMat);
    double density = db()->getDouble(materialTable, "DENSITY", iMat) * Gaudi::Units::g / Gaudi::Units::cm3;
    materials[materialName] = MaterialDef(materialName, density);
  }

  // read in material component table
  for (unsigned int iComp = 0; iComp < db()->getTableSize(componentsTable); iComp++) {
    std::string materialName = db()->getString(componentsTable, "NAME", iComp);
    std::string compName = db()->getString(componentsTable, "COMPNAME", iComp);
    double fracWeight = db()->getDouble(componentsTable, "FRACTION", iComp);
    MatMap::iterator iter = materials.find(materialName);
    if (iter != materials.end()) {
      iter->second.addComponent(compName, fracWeight);
    } else {
      msg(MSG::ERROR) << "Attemp to add material component, " << compName << ", to non-existing material: " <<
      materialName << endmsg;
    }
  }

  //Now create the materials
  int matCount = 0;
  int matCountLast = -1;
  bool someUndefined = true;
  // While there are still undefined materials keep creating materials.
  // Check also that the matCount had change to avoid endless loop due to cyclicly
  // defined materials.
  while (someUndefined && matCount != matCountLast) {
    matCountLast = matCount;
    someUndefined = false;
    for (MatMap::iterator iter = materials.begin(); iter != materials.end(); ++iter) {
      MaterialDef& tmpMat = iter->second;
      if (!tmpMat.isCreated()) {
        // Check if any components are materials in this table and if they are defined.
        // If not flag that there are undefined materials and go to next material
        bool compsDefined = true;
        for (unsigned int iComp = 0; iComp < tmpMat.numComponents(); ++iComp) {
          std::string compName = tmpMat.compName(iComp);
          MatMap::iterator iter2 = materials.find(compName);
          if (iter2 != materials.end()) {
            if (!iter2->second.isCreated()) {
              compsDefined = false;
              break;
            }
          }
        }
        if (compsDefined) {
          createMaterial(tmpMat);
          tmpMat.setCreated();
          matCount++;
        } else {
          someUndefined = true;
        }
      }
    }
  }


  if (someUndefined) {
    msg(MSG::ERROR) << "Not all materials could be defined due to cyclic definitions" << endmsg;
  }
}

void
InDetMaterialManager::createMaterial(const MaterialDef& material) {
  if (material.numComponents() == 0) {
    msg(MSG::ERROR) << "Material has no components: " << material.name() << endmsg;
    return;
  }

  // If total of fractions is greater than 1.1 then assume material is define by ratio of atoms.
  double totWeight = material.totalFraction();
  bool byAtomicRatio = false;
  if (totWeight > 1.1) {
    byAtomicRatio = true;
    for (unsigned int i = 0; i < material.numComponents(); i++) {
      if (material.compName(i).find("::") != std::string::npos) {
        // If component name has "::" in it then its not an element.
        msg(MSG::ERROR) << "Material, " << material.name()
                        <<
        ", is assumed to be defined by atomic ratio (due to total fraction > 1) but component is not an element: "
                        << material.compName(i) << endmsg;
        return;
      }
      const GeoElement* element = getElement(material.compName(i));
      if (!element) {
        msg(MSG::ERROR) << "Error making material " << material.name() << ". Element not found: " <<
        material.compName(i) << endmsg;
        return;
      }
      totWeight += material.fraction(i) * element->getA();
    }
  } else {
    // Check if total fraction is close to 1.
    if (std::abs(totWeight - 1) > 0.01) {
      msg(MSG::WARNING) << "Total fractional weight does not sum to 1. Will renormalize. Total = " << totWeight <<
      endmsg;
    }
  }
  // Now build the material
  GeoMaterial* newMaterial = new GeoMaterial(material.name(), material.density());
  if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Creating material: " << material.name()
                                          << " with density: " << material.density() / (Gaudi::Units::g / Gaudi::Units::cm3) <<
    endmsg;
  for (unsigned int i = 0; i < material.numComponents(); i++) {
    double fracWeight = material.fraction(i) / totWeight;
    if (material.compName(i).find("::") == std::string::npos) {
      const GeoElement* element = getElement(material.compName(i));
      if (!element) {
        msg(MSG::ERROR) << "Error making material " << material.name() << ". Element not found: " <<
        material.compName(i) << endmsg;
        // delete the partially created material
        newMaterial->ref();
        newMaterial->unref();
        return;
      }
      if (byAtomicRatio) {
        fracWeight = material.fraction(i) * element->getA() / totWeight;
      }
      newMaterial->add(const_cast<GeoElement*>(element), fracWeight);
      if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << " Component: " << material.compName(i) << " " << fracWeight << endmsg;
    } else {
      const GeoMaterial* materialTmp = getMaterialInternal(material.compName(i));
      if (!materialTmp) {
        msg(MSG::ERROR) << "Error making material " << material.name() << ". Component not found: " <<
        material.compName(i) << endmsg;
        // delete the partially created material
        newMaterial->ref();
        newMaterial->unref();
        return;
      }
      if (byAtomicRatio) {
        // Should not happen as already checked that all components were elements.
        msg(MSG::ERROR) << "Unexpected Error" << endmsg;
      }
      newMaterial->add(const_cast<GeoMaterial*>(materialTmp), fracWeight);
      if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << " Component: " << material.compName(i) << " " << fracWeight << endmsg;
    }
  }
  newMaterial->lock();
  addMaterial(newMaterial);
}

InDetMaterialManager::MaterialDef::MaterialDef()
  : m_density(0),
  m_created(false)
{}

InDetMaterialManager::MaterialDef::MaterialDef(const std::string& name, double density)
  : m_name(name),
  m_density(density),
  m_created(false)
{}

void
InDetMaterialManager::MaterialDef::addComponent(const std::string& compName, double fraction) {
  m_components.push_back(compName);
  m_fractions.push_back(fraction);
}

double
InDetMaterialManager::MaterialDef::totalFraction() const {
  double sum = 0;

  for (unsigned int i = 0; i < m_fractions.size(); i++) {
    sum += m_fractions[i];
  }
  return sum;
}

// We need the original name as the GeoMaterial from the standard
// material manager has its namespace dropped.  We have two versions
// of extraScaledMaterial. One where two names are provided. In this
// version if newName is not empty that is used, otherwise
// materialName is used.  The other just has one name and that is the
// one that is used.

const GeoMaterial*
InDetMaterialManager::extraScaledMaterial(const std::string& materialName,
                                          const std::string& newName,
                                          const GeoMaterial* origMaterial) {
  if (newName.empty()) {
    return extraScaledMaterial(materialName, origMaterial);
  } else {
    return extraScaledMaterial(newName, origMaterial);
  }
}

const GeoMaterial*
InDetMaterialManager::extraScaledMaterial(const std::string& materialName, const GeoMaterial* origMaterial) {
  if (!origMaterial) throw std::runtime_error(std::string("Invalid material: ") + materialName);

  double scaleFactor = getExtraScaleFactor(materialName);
  // -1 (or any -ve number) indicates material is not scaled. And if the scale factor
  // is 1 then there is no need to create a new material.
  if (scaleFactor < 0 || scaleFactor == 1 || materialName.find("Ether") != std::string::npos) return origMaterial;

  if (scaleFactor == 0) return getMaterialInternal("std::Vacuum");

  std::string newName = materialName + "_ExtraScaling";

  // Check if it is already made.
  const GeoMaterial* newMaterial = getAdditionalMaterial(newName);

  // Already made so we return it.
  if (newMaterial) return newMaterial;

  // Otherwise we need to make it.
  double density = origMaterial->getDensity() * scaleFactor;

  // create new material
  GeoMaterial* newMaterialTmp = new GeoMaterial(newName, density);
  newMaterialTmp->add(const_cast<GeoMaterial*>(origMaterial), 1.);
  addMaterial(newMaterialTmp);
  newMaterial = newMaterialTmp;

  return newMaterial;
}

double
InDetMaterialManager::getExtraScaleFactor(const std::string& materialName) {
  // If name is found in map we return the corresponding scale factor.
  // The special name "ALL" indicates all materials are scaled.
  // Individual materials can be excluded from scaling by giving either
  // a -ve scaling factor or just specifying a scaling factor of 1.
  // A scaling factor of 0 means the material will be replaced by vacuum.

  ExtraScaleFactorMap::const_iterator iter = m_scalingMap.find(materialName);
  if (iter != m_scalingMap.end()) {
    return iter->second;
  } else {
    // Check for special names
    // ALL means everything scaled. Do not scale air or vacuum (unless explicity requested)
    iter = m_scalingMap.find("ALL");
    if (iter != m_scalingMap.end() && materialName != "std::Air" && materialName != "std::Vacuum") {
      return iter->second;
    }
  }

  // If not found then return -1 to indicate material is not to be scaled.
  return -1;
}
