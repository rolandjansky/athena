/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// class header
#include "G4AtlasTools/CylindricalEnvelope.h"

// Geant4 includes used in functions
#include "G4NistManager.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4SystemOfUnits.hh"
#include "G4Tubs.hh"

// CLHEP includes
#include "CLHEP/Units/SystemOfUnits.h"

CylindricalEnvelope::CylindricalEnvelope(const std::string& type, const std::string& name, const IInterface* parent)
  : DetectorGeometryBase(type,name,parent)
  , m_materialName("Air")
  , m_innerRadius(0.)
  , m_outerRadius(0.)
  , m_dZ(0.)
  , m_startphi(0.)
  , m_deltaphi(360.*CLHEP::deg)

{
  ATH_MSG_DEBUG( "CylindricalEnvelope constructor for " << name );
  declareProperty("Material",m_materialName,"Envelope Material");
  declareProperty("InnerRadius",m_innerRadius,"Envelope inner radius");
  declareProperty("OuterRadius",m_outerRadius,"Envelope outer radius");
  declareProperty("dZ",m_dZ,"Envelope dZ");
  declareProperty("StartPhi", m_startphi, "");
  declareProperty("DeltaPhi", m_deltaphi, "");
}

void CylindricalEnvelope::BuildGeometry()
{
  ATH_MSG_DEBUG( "CylindricalEnvelope::BuildGeometry(): (Inner Radius, Outer Radius, dZ) = ("<<m_innerRadius<<", "<<m_outerRadius<<", "<<m_dZ <<")");

  // Get nist material manager
  G4NistManager* nist = G4NistManager::Instance();

  G4Material* mat = nist->FindOrBuildMaterial(m_materialName);

  G4Tubs* envSolid=new G4Tubs(m_detectorName,m_innerRadius,m_outerRadius,m_dZ,m_startphi,m_deltaphi);

  G4LogicalVolume* logicWorld = new G4LogicalVolume(envSolid,mat,m_detectorName);

  m_envelope.theEnvelope=logicWorld;
}
