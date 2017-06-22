/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GeoMaterial2G4/Geo2G4MaterialFactory.h"
#include "Geo2G4ElementFactory.h"
#include "GeoMaterial2G4/Geo2G4MatPropTableFactory.h"

#include "GeoModelKernel/GeoMaterial.h"
#include "GeoModelUtilities/GeoExtendedMaterial.h"
#include "GeoModelUtilities/GeoMaterialPropertiesTable.h"

#include "AthenaBaseComps/AthMsgStreamMacros.h"

#include "G4Material.hh"

Geo2G4MaterialFactory::Geo2G4MaterialFactory(): m_msg("Geo2G4MaterialFactory")
{
}

G4Material* Geo2G4MaterialFactory::Build(const GeoMaterial* theMat)
{
  static Geo2G4ElementFactory eFactory;
  Geo2G4MatPropTableFactory* tFactory = Geo2G4MatPropTableFactory::instance();

  //
  // Check if this material has already been defined.
  //
  std::string nam = theMat->getName();

  if(definedMaterials.find(theMat) != definedMaterials.end())
    return definedMaterials[theMat];

  int nelements = theMat->getNumElements();

  // Different actions depending whether we are dealing with
  // standard or extended materials

  const GeoExtendedMaterial* extMat = dynamic_cast<const GeoExtendedMaterial*>(theMat);
  G4Material* newmaterial = 0;

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

    double temperature = extMat->getTemperature();
    double pressure = extMat->getPressure();

    newmaterial= new G4Material(nam,
                                extMat->getDensity(),
                                nelements,
                                state,
                                temperature,
                                pressure);

    // Build G4MaterialPropertiesTable if needed
    GeoMaterialPropertiesTable* geoPropTable = extMat->GetMaterialPropertiesTable();

    if(geoPropTable) {
      G4MaterialPropertiesTable* g4PropTable = tFactory->Build(geoPropTable);
      if(g4PropTable)
        newmaterial->SetMaterialPropertiesTable(g4PropTable);
    }
  }
  else
    newmaterial= new G4Material(nam,
                                theMat->getDensity(),
                                nelements);

  for (int ii = 0; ii< nelements; ii++)  {
    G4Element* theG4Ele = eFactory.Build(theMat->getElement(ii));
    newmaterial->AddElement(theG4Ele, theMat->getFraction(ii));
  }

  definedMaterials[theMat]=newmaterial;

  // Check if we have the situation when on GeoModel side two different
  // materials share the same name.
  // Print an INFO message if so.
  if(definedMatNames.find(nam)==definedMatNames.end())
    definedMatNames[nam] = theMat;
  else if(definedMatNames[nam] != theMat)
    ATH_MSG_INFO ( "!!! On GeoModel side two different materials share the name: " << nam );
  return newmaterial;
}
