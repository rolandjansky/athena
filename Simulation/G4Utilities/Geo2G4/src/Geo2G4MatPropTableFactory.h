/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef Geo2G4MatPropTableFactory_h
#define Geo2G4MatPropTableFactory_h

#include <map>

class G4MaterialPropertiesTable;
class GeoMaterialPropertiesTable;

typedef std::map<const GeoMaterialPropertiesTable* , G4MaterialPropertiesTable*, std::less<const GeoMaterialPropertiesTable*> > TableMap;

class Geo2G4MatPropTableFactory
{
public:
  static Geo2G4MatPropTableFactory* instance();

  G4MaterialPropertiesTable* Build(const GeoMaterialPropertiesTable*);

private:
  Geo2G4MatPropTableFactory();

  static Geo2G4MatPropTableFactory* m_instance;
  TableMap definedTables;
};

#endif
