/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "GeoMaterial2G4/Geo2G4MaterialFactory.h"
#include "Geo2G4ElementFactory.h"
#include "GeoMaterial2G4/Geo2G4MatPropTableFactory.h"

#include "GeoModelKernel/GeoMaterial.h"
#include "GeoModelUtilities/GeoExtendedMaterial.h"
#include "GeoModelUtilities/GeoMaterialPropertiesTable.h"

#include "AthenaBaseComps/AthMsgStreamMacros.h"

#include "G4Material.hh"


matList  Geo2G4MaterialFactory::m_geoMaterialToG4Material;
matNames Geo2G4MaterialFactory::m_geoMaterialNameToObject;

Geo2G4MaterialFactory::Geo2G4MaterialFactory(): m_msg("Geo2G4MaterialFactory")
{
}

G4Material* Geo2G4MaterialFactory::Build(const GeoMaterial* geoMaterial)
{
  static Geo2G4ElementFactory eFactory;
  Geo2G4MatPropTableFactory* tFactory = Geo2G4MatPropTableFactory::instance();

  //
  // Check if this material has already been defined.
  //
  if(m_geoMaterialToG4Material.find(geoMaterial) != m_geoMaterialToG4Material.end()) {
    return m_geoMaterialToG4Material[geoMaterial];
  }

  const std::string& geoMaterialName = geoMaterial->getName();
  const int nelements = geoMaterial->getNumElements();

  // Different actions depending whether we are dealing with
  // standard or extended materials

  const GeoExtendedMaterial* extMat = dynamic_cast<const GeoExtendedMaterial*>(geoMaterial);
  G4Material* g4Material{};

  if(extMat) {
    G4State state = kStateUndefined;

    switch(extMat->getState())
      {
      case stateUndefined:
        state = kStateUndefined;
        break;
      case stateSolid:
        state = kStateSolid;
        break;
      case stateLiquid:
        state = kStateLiquid;
        break;
      case stateGas:
        state = kStateGas;
        break;
      default:
        break;
      }

    const double temperature = extMat->getTemperature();
    const double pressure = extMat->getPressure();

    g4Material= new G4Material(geoMaterialName,
                               extMat->getDensity(),
                               nelements,
                               state,
                               temperature,
                               pressure);

    // Build G4MaterialPropertiesTable if needed
    GeoMaterialPropertiesTable* geoPropTable = extMat->GetMaterialPropertiesTable();

    if(geoPropTable) {
      G4MaterialPropertiesTable* g4PropTable = tFactory->Build(geoPropTable);
      if(g4PropTable) {
        g4Material->SetMaterialPropertiesTable(g4PropTable);
      }
    }
  }
  else {
    g4Material= new G4Material(geoMaterialName,
                               geoMaterial->getDensity(),
                               nelements);
  }

  for (int ii = 0; ii< nelements; ii++)  {
    G4Element* g4Element = eFactory.Build(geoMaterial->getElement(ii));
    g4Material->AddElement(g4Element, geoMaterial->getFraction(ii));
  }

  const G4MaterialTable & theMaterialTable = *(g4Material->GetMaterialTable());
  const G4String& g4MaterialName = g4Material->GetName();
  std::vector<size_t> copyIndex{};
  for(size_t i=0; i< theMaterialTable.size(); ++i) {
    if(theMaterialTable[i]->GetName() == g4MaterialName) {
      copyIndex.push_back(i);
    }
  }
  if ( copyIndex.size() > 1 ) {
    ATH_MSG_INFO ( "Details of all G4Materials named " << g4MaterialName << " in the G4MaterialTable.");
    for (const auto& index : copyIndex) {
      ATH_MSG_INFO ( "G4Material at position "<< index<<" in the G4MaterialTable: \n" << *(theMaterialTable[index]));
    }
  }


  m_geoMaterialToG4Material[geoMaterial]=g4Material;

  // Check if we have the situation when on GeoModel side two different
  // materials share the same name.
  // Print an INFO message if so.
  if(m_geoMaterialNameToObject.find(geoMaterialName)==m_geoMaterialNameToObject.end())
    m_geoMaterialNameToObject[geoMaterialName] = geoMaterial;
  else if(m_geoMaterialNameToObject[geoMaterialName] != geoMaterial) {
    ATH_MSG_INFO ( "!!! On GeoModel side two different materials share the name: " << geoMaterialName );
  }

  return g4Material;
}
