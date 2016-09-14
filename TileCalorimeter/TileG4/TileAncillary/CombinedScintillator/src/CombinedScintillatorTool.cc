/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//class header
#include "CombinedScintillatorTool.hh"

//Athena headers
#include "GeoModelInterfaces/StoredMaterialManager.h"
#include "GeoModelKernel/GeoMaterial.h"
#include "GeoMaterial2G4/Geo2G4MaterialFactory.h"
#include "StoreGate/DataHandle.h"

// Geant4 headers
#include "G4LogicalVolume.hh"
#include "G4Material.hh"
#include "G4PVPlacement.hh"
#include "G4Tubs.hh"

// CLHEP headers
#include "CLHEP/Units/SystemOfUnits.h"

CombinedScintillatorTool::CombinedScintillatorTool(const std::string& type, const std::string& name,
                                                   const IInterface* parent)
    : DetectorGeometryBase(type, name, parent),
      m_rMin(0),
      m_rMax(0),
      m_dzSci(0),
      m_phiPos(0),
      m_phiNeg(0)
{
  ATH_MSG_DEBUG("CombinedScintillatorTool constructor for " << name);
  declareProperty("RMin", m_rMin, "");
  declareProperty("RMax", m_rMax, "");
  declareProperty("DZSci", m_dzSci, "");
  declareProperty("PhiPos", m_phiPos, "");
  declareProperty("PhiNeg", m_phiNeg, "");

}

CombinedScintillatorTool::~CombinedScintillatorTool() {

}

void CombinedScintillatorTool::BuildGeometry() {
  ATH_MSG_VERBOSE(name() << " CombinedScintillatorTool::BuildGeometry(): Starting");
  // Scintillator between LAr & Tile described as a sector of a cylinder

  // Define starting point for phi angle
  const double phi1 = m_phiNeg;
  // Define range in phi to be covered
  const double dphi = m_phiPos - m_phiNeg;

  G4Tubs* sci_mother = new G4Tubs("CScintillatorLayer", m_rMin, m_rMax, m_dzSci, phi1 * CLHEP::deg, dphi * CLHEP::deg);
  // Get the materials
  const DataHandle<StoredMaterialManager> materialManager;
  if (StatusCode::SUCCESS != detStore()->retrieve(materialManager, std::string("MATERIALS"))) {
    //FIXME should probably at least print an ERROR here...
    return;
  }
  const GeoMaterial *geoScintillator = materialManager->getMaterial("tile::Scintillator");
  Geo2G4MaterialFactory theMaterialFactory;
  G4Material *scintillatorMaterial = theMaterialFactory.Build(geoScintillator);

  G4LogicalVolume* sciV = new G4LogicalVolume(sci_mother, scintillatorMaterial, "CScintillatorLayer");

  m_envelope.theEnvelope = sciV;

  ATH_MSG_VERBOSE(name() << " CombinedScintillatorTool::BuildGeometry(): Finished");
}
