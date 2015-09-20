/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Base class
#include "G4AtlasTools/DetectorGeometryBase.h"
#include "G4AtlasTools/GeoDetectorTool.h"
#include "Geo2G4/Geo2G4SvcAccessor.h"
#include "Geo2G4/Geo2G4SvcBase.h"
#include "Geo2G4/Geo2G4Builder.h"
#include "Geo2G4/VolumeBuilder.h"

#include "G4NistManager.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"

// Geant4 includes used in functions

GeoDetectorTool::GeoDetectorTool(const std::string& type, const std::string& name, const IInterface* parent)
  : DetectorGeometryBase(type,name,parent)
{
	ATH_MSG_INFO( "GeoDetectorTool being created: " << name );

}

StatusCode GeoDetectorTool::initialize()
{
	ATH_MSG_INFO( "GeoDetectorTool being initialized!" );
	
	Geo2G4SvcAccessor accessor;
    Geo2G4SvcBase *g=accessor.GetGeo2G4Svc();
    builderName=g->GetDefaultBuilder()->GetKey();
    blGetTopTransform = g->UseTopTransforms();
	
	return StatusCode::SUCCESS;
}


void GeoDetectorTool::BuildGeometry() 
{
	ATH_MSG_INFO( "this is GeoDetectorTool::BuildGeometry: " );
	G4LogicalVolume* temp=Convert();
	envelope.theEnvelope=temp;
	
	if (IsTopTransform())
		SetInitialTransformation(GetTopTransform());
}

G4LogicalVolume* GeoDetectorTool::Convert()
{
	Geo2G4Builder Builder(m_detectorName);
  	VolumeBuilder *b=Builder.GetVolumeBuilder(builderName);
  	b->SetParam(blParamOn);
  	G4LogicalVolume *temp=Builder.BuildTree();
  	return temp;
}

G4Transform3D& GeoDetectorTool::GetTopTransform()
{
	Geo2G4Builder Builder(m_detectorName);
	return Builder.GetDetectorTransform();
}

bool GeoDetectorTool::IsTopTransform()
{
	return blGetTopTransform;
}

void GeoDetectorTool::SetInitialTransformation(G4Transform3D& transform)
{
	if (!envelope.theRotation) envelope.theRotation=new G4RotationMatrix;
	*(envelope.theRotation)=transform.getRotation().inverse();
	envelope.thePosition=transform.getTranslation();
} 
