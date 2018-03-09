/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GeoMaterial2G4/Geo2G4MatPropTableFactory.h"
#include "G4MaterialPropertiesTable.hh"
#include "G4MaterialPropertyVector.hh"

#include "GeoModelUtilities/GeoMaterialPropertiesTable.h"
#include "GeoModelUtilities/GeoMaterialPropertyVector.h"

Geo2G4MatPropTableFactory* Geo2G4MatPropTableFactory::m_instance = 0;

Geo2G4MatPropTableFactory* Geo2G4MatPropTableFactory::instance()
{
  if(!m_instance)
    m_instance = new Geo2G4MatPropTableFactory();
  return m_instance;
}

Geo2G4MatPropTableFactory::Geo2G4MatPropTableFactory()
{
}

G4MaterialPropertiesTable* Geo2G4MatPropTableFactory::Build(const GeoMaterialPropertiesTable* thePropTable)
{
  //
  // Check if this material has already been defined.
  //
  if(definedTables.find(thePropTable) != definedTables.end())
    return definedTables[thePropTable];

  G4MaterialPropertiesTable* newTable = new G4MaterialPropertiesTable();

  // Add properties to the table ...

  // 1. Const properties
  GeoMaterialPropertiesTable::GeoMatPMap_ConstIt it1_first = thePropTable->beginPMap();
  GeoMaterialPropertiesTable::GeoMatPMap_ConstIt it1_last  = thePropTable->endPMap();

  for(;it1_first!=it1_last;it1_first++)
    newTable->AddConstProperty((it1_first->first).c_str(),it1_first->second);

  // 2. Vector properties
  GeoMaterialPropertiesTable::GeoMatPVMap_ConstIt it2_first = thePropTable->beginPVMap();
  GeoMaterialPropertiesTable::GeoMatPVMap_ConstIt it2_last  = thePropTable->endPVMap();

  for(;it2_first!=it2_last;it2_first++)
    {
      GeoMaterialPropertyVector* geoMPV = it2_first->second;
      //from G4 9.6 G4MaterialPropertyVector is now a typedef of G4PhysicsOrderedFreeVector
      G4MaterialPropertyVector* g4MPV = new G4MaterialPropertyVector();

      geoMPV->ResetIterator();

      while((*geoMPV).operator++())
        {
          //g4MPV->AddElement(geoMPV->GetPhotonMomentum(),geoMPV->GetProperty()); // G4 9.4 syntax
          //assume G4PhysicsOrderedFreeVector::InsertValues is equivalent to G4MaterialPropertyVector::AddElement
          g4MPV->InsertValues(geoMPV->GetPhotonMomentum(),geoMPV->GetProperty()); // G4 9.6 syntax
        }

      newTable->AddProperty((it2_first->first).c_str(),g4MPV);
    }

  // Save new table to the map
  definedTables[thePropTable]=newTable;

  return newTable;
}
