/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GEOMODELINTERFACES_STOREDMATERIALMANAGER_H
#define GEOMODELINTERFACES_STOREDMATERIALMANAGER_H

/**
 * @class StoredMaterialManager
 *
 * @brief This class holds one or more material managers and makes 
 *        them storeable, under StoreGate
 *
 * @author Joe Boudreau, March 2003
 */

#include "AthenaKernel/CLASS_DEF.h"
#include <map>

class GeoMaterial;
class GeoElement;

class StoredMaterialManager
{
 public:
  typedef std::map<std::string, GeoMaterial* > MaterialMap;
  typedef MaterialMap::const_iterator MaterialMapIterator;

  // Constructor:
  StoredMaterialManager() {};
  
  // Destructor:
  virtual ~StoredMaterialManager() {};

  // Query the material:
  virtual const GeoMaterial* getMaterial(const std::string& name) const = 0;

  // Query the elements:
  virtual const GeoElement* getElement(const std::string& name) const = 0;

  // Query the elements (by atomic number):
  virtual const GeoElement* getElement(unsigned int atomicNumber) const = 0;

  // Add new material
  virtual void addMaterial(const std::string& space, GeoMaterial* material) const = 0;

  // Return iterators
  virtual MaterialMapIterator begin() const = 0;
  virtual MaterialMapIterator end() const = 0;

  // Number of materials in the manager
  virtual size_t size() = 0;

  // Dump the contents
  virtual std::ostream& printAll(std::ostream & o=std::cout) const = 0;
};

CLASS_DEF(StoredMaterialManager, 9896,1)

#endif
