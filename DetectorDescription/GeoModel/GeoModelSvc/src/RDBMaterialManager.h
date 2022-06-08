/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GEOMODELSVC_RDBMATERIALMANAGER_H
#define GEOMODELSVC_RDBMATERIALMANAGER_H

//---------------------------------------------------------//
//                                                         //
// class RDBMaterialManager  This is a material manager   //
// which gets its material from RDB.                       //
//                                                         //
// Joe Boudreau March 2003                                 //
//                                                         //
//---------------------------------------------------------//
#include "GeoModelInterfaces/StoredMaterialManager.h"
#include "RDBAccessSvc/IRDBAccessSvc.h"

#include <string>
#include <map>
#include <vector>
#include <iosfwd>

class GeoMaterial;
class ISvcLocator;

class RDBMaterialManager final : public StoredMaterialManager {

 public:

  // Constructor:
  RDBMaterialManager(ISvcLocator* pSvcLocator);
  
  // Destructor:
  virtual ~RDBMaterialManager();

  // Query the material:
  virtual const GeoMaterial *getMaterial(const std::string &name) override;
 
  // Query the elements:
  virtual const GeoElement *getElement(const std::string & name) override;

  // Query the elements (by atomic number):
  virtual const GeoElement *getElement(unsigned int atomicNumber) override;

  // Add new material
  virtual void addMaterial(const std::string& space, GeoMaterial* material) override;

  virtual StoredMaterialManager::MaterialMapIterator begin() const override;
  virtual StoredMaterialManager::MaterialMapIterator end() const override;

  // Number of materials in the manager
  virtual size_t size() override;

  virtual std::ostream & printAll(std::ostream & o=std::cout) const override;

 private:

  StatusCode readMaterialsFromDB(ISvcLocator* pSvcLocator);

  void buildSpecialMaterials();

  GeoElement *searchElementVector (const std::string & name) const;
  GeoElement *searchElementVector (const unsigned int atomicNumber) const;
  GeoMaterial *searchMaterialMap (const std::string & name) const;
  
  // For DataBase connection and Query
  IRDBRecordset_ptr m_elements;
  
  IRDBRecordset_ptr m_trtmaterials;
  IRDBRecordset_ptr m_trtmatcomponents;
  IRDBRecordset_ptr m_stdmaterials;
  IRDBRecordset_ptr m_stdmatcomponents;
  IRDBRecordset_ptr m_larmaterials;
  IRDBRecordset_ptr m_larmatcomponents;
  IRDBRecordset_ptr m_muomaterials;
  IRDBRecordset_ptr m_muomatcomponents;
  IRDBRecordset_ptr m_pixmaterials;
  IRDBRecordset_ptr m_pixmatcomponents;
  IRDBRecordset_ptr m_pixtbmaterials;
  IRDBRecordset_ptr m_pixtbmatcomponents;
  IRDBRecordset_ptr m_sctmaterials;
  IRDBRecordset_ptr m_sctmatcomponents;
  IRDBRecordset_ptr m_indetmaterials;
  IRDBRecordset_ptr m_indetmatcomponents;
  IRDBRecordset_ptr m_shieldmaterials;
  IRDBRecordset_ptr m_shieldmatcomponents;
  IRDBRecordset_ptr m_tilematerials;
  IRDBRecordset_ptr m_tilematcomponents;
  IRDBRecordset_ptr m_toromaterials;
  IRDBRecordset_ptr m_toromatcomponents;

  std::vector < GeoElement *>        m_elementVector;
  StoredMaterialManager::MaterialMap m_materialMap;
};


#endif
