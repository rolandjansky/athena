/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef _STOREDMATERIALMANAGER_H_
#define _STOREDMATERIALMANAGER_H_
//---------------------------------------------------------//
//                                                         //
// class StoredMaterialManager  This class hold one or more//
// material managers and makes them storeable, under       //
// storegate:                                              //
//                                                         //
// Joe Boudreau March 2003                                 //
//                                                         //
//---------------------------------------------------------//
#include "GeoModelInterfaces/AbsMaterialManager.h"
#include "CLIDSvc/CLASS_DEF.h"
#include <map>
class StoredMaterialManager:public AbsMaterialManager 
{
 public:
  typedef std::map<std::string, GeoMaterial* > MaterialMap;
  typedef MaterialMap::const_iterator MaterialMapIterator;

  // Constructor:
  StoredMaterialManager() {};
  
  // Destructor:
  virtual ~StoredMaterialManager() {};

  // Query the material:
  virtual       GeoMaterial* getMaterial(const std::string& name)       = 0;
  virtual const GeoMaterial* getMaterial(const std::string& name) const = 0;

  // Query the elements:
  virtual       GeoElement* getElement(const std::string& name)       = 0;
  virtual const GeoElement* getElement(const std::string& name) const = 0;

  // Query the elements (by atomic number):
  virtual       GeoElement* getElement(unsigned int atomicNumber)       = 0;
  virtual const GeoElement* getElement(unsigned int atomicNumber) const = 0;

  // Add new material
  virtual void addMaterial(const std::string& space, GeoMaterial* material)       = 0;
  virtual void addMaterial(const std::string& space, GeoMaterial* material) const = 0;

  virtual MaterialMapIterator begin() const = 0;
  virtual MaterialMapIterator end() const = 0;

  // Number of materials in the manager
  virtual size_t size() = 0;

  virtual std::ostream& printAll(std::ostream & o=std::cout) const = 0;
};

CLASS_DEF(StoredMaterialManager, 9896,1)

#endif
