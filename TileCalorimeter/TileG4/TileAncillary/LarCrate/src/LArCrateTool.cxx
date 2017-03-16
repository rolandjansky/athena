/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//class header
#include "LArCrateTool.h"

//Athena headers
#include "GeoModelInterfaces/StoredMaterialManager.h"
#include "GeoModelKernel/GeoMaterial.h"
#include "GeoMaterial2G4/Geo2G4MaterialFactory.h"
#include "StoreGate/DataHandle.h"

// Geant4 headers
#include "G4Box.hh"
#include "G4LogicalVolume.hh"
#include "G4Material.hh"
#include "G4PVPlacement.hh"

// CLHEP headers
#include "CLHEP/Units/SystemOfUnits.h"

LArCrateTool::LArCrateTool(const std::string& type, const std::string& name, const IInterface* parent)
  : DetectorGeometryBase(type,name,parent),
    m_zLength(0),
    m_yLength(0),
    m_xLength(0)
{
  ATH_MSG_DEBUG( "LArCrateTool constructor for " << name );
  declareProperty("ZLength", m_zLength, "");
  declareProperty("YLength", m_yLength, "");
  declareProperty("XLength", m_xLength, "");
}

void LArCrateTool::BuildGeometry()
{
  ATH_MSG_VERBOSE( name() << " LArCrateTool::BuildGeometry(): Starting" );

  const double dzCrate = (m_zLength)/2.*CLHEP::mm ;
  const double dyCrate = (m_yLength)/2.*CLHEP::mm ;
  const double dxCrate = (m_xLength)/2.*CLHEP::mm ;

  G4Box *emptybox = new G4Box("Crate",dxCrate,dyCrate,dzCrate);

  // Get the materials
  const DataHandle<StoredMaterialManager> materialManager;
  if (StatusCode::SUCCESS != detStore()->retrieve(materialManager, std::string("MATERIALS")))
    {
      //FIXME should probably at least print an ERROR here...
      return;
    }
  const GeoMaterial *geoAir          = materialManager->getMaterial("tile::Air");
  const GeoMaterial *geoAluminium    = materialManager->getMaterial("std::Aluinium");
  const GeoMaterial *geoSiO2CondEpox = materialManager->getMaterial("tile::SiO2CondEpox");
  Geo2G4MaterialFactory theMaterialFactory;
  G4Material *airMaterial=theMaterialFactory.Build(geoAir);
  G4Material *aluminiumMaterial=theMaterialFactory.Build(geoAluminium);
  G4Material *SiO2CondEpoxMaterial=theMaterialFactory.Build(geoSiO2CondEpox);

  G4Material *crateMaterial = new G4Material("LArCrateMaterial", 0.2876*CLHEP::g/CLHEP::cm3,3);
  crateMaterial->AddMaterial(aluminiumMaterial, 0.7744);
  crateMaterial->AddMaterial(SiO2CondEpoxMaterial, 0.2216);
  crateMaterial->AddMaterial(airMaterial, 0.0040);

  G4LogicalVolume *emptyboxV = new G4LogicalVolume(emptybox,crateMaterial,"LarCrate");
  // creates wall of 5mm aluminum thickness
  const double thicknessAlu = 5.*CLHEP::mm ;

  for(int j=0; j<2;j++)
    {
      double xLayers=0.0, yLayers=0.0, zLayers=0.0;

      // top and Bottom plates
      G4Box *AluLayerTB = new G4Box("AluLayerTB",dxCrate,thicknessAlu/2.,dzCrate);
      G4LogicalVolume *AluLayerVTB = new G4LogicalVolume(AluLayerTB,aluminiumMaterial,"AluLayerTB");
      if(j==0) yLayers = dyCrate - thicknessAlu/2.;
      if(j==1) yLayers = -dyCrate + thicknessAlu/2.;
      G4PVPlacement *AluLayerVTBPhys __attribute__((unused)) =
        new G4PVPlacement(0,
                          G4ThreeVector(0.,yLayers,0.),
                          AluLayerVTB,
                          "AluLayerTB" ,
                          emptyboxV,
                          false,
                          j+1);

      // Right and Left sides
      G4Box *AluLayerRL = new G4Box("AluLayerRL",dxCrate,dyCrate-thicknessAlu,thicknessAlu/2.);
      G4LogicalVolume *AluLayerVRL = new G4LogicalVolume(AluLayerRL,aluminiumMaterial,"AluLayerRL");
      if(j==0) zLayers = dzCrate - thicknessAlu/2.;
      if(j==1) zLayers = -dzCrate + thicknessAlu/2.;
      G4PVPlacement *AluLayerVRLPhys __attribute__((unused)) =
        new G4PVPlacement(0,
                          G4ThreeVector(0.,0.,zLayers),
                          AluLayerVRL,
                          "AluLayerRL" ,
                          emptyboxV,
                          false,
                          j+1);

      // Front and back sides
      G4Box *AluLayerFB = new G4Box("AluLayerFB",thicknessAlu/2.,dyCrate-thicknessAlu,dzCrate-thicknessAlu);
      G4LogicalVolume *AluLayerVFB = new G4LogicalVolume(AluLayerFB,aluminiumMaterial,"AluLayerFB");
      if(j==0) xLayers = dxCrate - thicknessAlu/2.;
      if(j==1) xLayers = -dxCrate + thicknessAlu/2.;
      G4PVPlacement *AluLayerVFBPhys __attribute__((unused)) =
        new G4PVPlacement(0,
                          G4ThreeVector(xLayers,0.,0.),
                          AluLayerVFB,
                          "AluLayerFB" ,
                          emptyboxV,
                          false,
                          j+1);
    }
  m_envelope.theEnvelope=emptyboxV;
  ATH_MSG_VERBOSE( name() << " LArCrateTool::BuildGeometry(): Finished" );
}
