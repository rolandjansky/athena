/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GeoMaterialPropertiesTable_h
#define GeoMaterialPropertiesTable_h 1

#include <map>
#include <string>

/// Ensure that the extensions for the Vector3D are properly loaded
#include "GeoPrimitives/GeoPrimitives.h"

#include "GeoModelKernel/RCBase.h"
#include "GeoModelUtilities/GeoMaterialPropertyVector.h"

class GeoMaterialPropertyVector;

class GeoMaterialPropertiesTable : public RCBase
{
 public:
  GeoMaterialPropertiesTable(); 
  ~GeoMaterialPropertiesTable();
  
 public: 
  // ******** Typedefs
  // Maps
  typedef std::map<std::string, GeoMaterialPropertyVector*, std::less<std::string> > GeoMatPVMap;
  typedef std::map< std::string, double, std::less<std::string> > GeoMatPMap;

  // Iterators
  typedef GeoMatPVMap::const_iterator GeoMatPVMap_ConstIt;
  typedef GeoMatPMap::const_iterator GeoMatPMap_ConstIt;

 public:
  // Add a new property to the table by giving a key-name and value 
  void AddConstProperty(const char     *key,
			double PropertyValue);

  // Add a new property to the table by giving a key-name and the
  // arrays x and y of size NumEntries.  
  void AddProperty(const char     *key,
		   double *PhotonMomenta,
		   double *PropertyValues,
		   int     NumEntries);

  // Add a new property to the table by giving a key-name and an
  // already constructed GeoMaterialPropertyVector.  
  void AddProperty(const char *key, GeoMaterialPropertyVector *opv);

  // Remove a constant property from the table.  
  void RemoveConstProperty(const char *key);

  // Remove a property from the table.  
  void RemoveProperty(const char *key);

  // Add a new entry (pair of numbers) to the table for a given key.  
  void AddEntry(const char *key, double aPhotonMomentum,
		double  aPropertyValue);
  
  // Access to the contents
  GeoMatPVMap_ConstIt beginPVMap() const;
  GeoMatPVMap_ConstIt endPVMap() const;

  GeoMatPMap_ConstIt beginPMap() const;
  GeoMatPMap_ConstIt endPMap() const;

  // Dump contents
  void DumpTable() const;

 private:
  GeoMatPVMap m_MPT;
  GeoMatPMap m_MPTC;

  typedef GeoMatPVMap::iterator MPTiterator;
  typedef GeoMatPMap::iterator MPTCiterator;
};

#endif 
