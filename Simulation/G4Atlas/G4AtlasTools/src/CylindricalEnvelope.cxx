/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Base class
#include "G4AtlasTools/DetectorGeometryBase.h"
#include "G4AtlasTools/CylindricalEnvelope.h"

#include "G4NistManager.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4SystemOfUnits.hh"
#include "G4Tubs.hh"

// Geant4 includes used in functions

CylindricalEnvelope::CylindricalEnvelope(const std::string& type, const std::string& name, const IInterface* parent)
  : DetectorGeometryBase(type,name,parent)
{
	ATH_MSG_INFO( "CylindricalEnvelope being created: " << name );
	declareProperty("InnerRadius",m_innerRadius,"Envelope inner radius");
	declareProperty("OuterRadius",m_outerRadius,"Envelope outer radius");
	declareProperty("dZ",m_dZ,"Envelope dZ");
}

StatusCode CylindricalEnvelope::initialize()
{
	return StatusCode::SUCCESS;
}


void CylindricalEnvelope::BuildGeometry() 
{
	ATH_MSG_INFO( "this is CylindricalEnvelope::BuildGeometry: "<<m_innerRadius<<" "<<m_outerRadius<<" "<<m_dZ );
	
// Get nist material manager
  	G4NistManager* nist = G4NistManager::Instance();

	G4Material* mat = nist->FindOrBuildMaterial("G4_AIR");
	
	G4Tubs* envSolid=new G4Tubs(m_detectorName,m_innerRadius,m_outerRadius,m_dZ,0.,360.*deg);
	
	G4LogicalVolume* logicWorld = new G4LogicalVolume(envSolid,mat,m_detectorName);

	envelope.theEnvelope=logicWorld;
}

