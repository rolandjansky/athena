/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// class header
#include "G4AtlasTools/CylindricalEnvelope.h"

// Geant4 includes used in functions
#include "G4NistManager.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4SystemOfUnits.hh"
#include "G4Tubs.hh"

CylindricalEnvelope::CylindricalEnvelope(const std::string& type, const std::string& name, const IInterface* parent)
  : DetectorGeometryBase(type,name,parent)
{
  ATH_MSG_DEBUG( "CylindricalEnvelope constructor for " << name );
}

void CylindricalEnvelope::BuildGeometry()
{
  ATH_MSG_DEBUG( "CylindricalEnvelope::BuildGeometry(): (Inner Radius, Outer Radius, dZ) = ("<<m_innerRadius<<", "<<m_outerRadius<<", "<<m_dZ <<")");

  // Get nist material manager
  G4NistManager* nist = G4NistManager::Instance();

  G4Material* mat = nist->FindOrBuildMaterial(m_materialName.value());

  G4Tubs* envSolid=new G4Tubs(m_detectorName.value(),m_innerRadius,m_outerRadius,m_dZ,m_startphi,m_deltaphi);

  G4LogicalVolume* logicWorld = new G4LogicalVolume(envSolid,mat,m_detectorName.value());

  m_envelope.theEnvelope=logicWorld;
}
