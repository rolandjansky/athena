/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AGDDKernel/AGDDPositioner.h"
#include "AGDDKernel/AGDDPositionerStore.h"
#include "AGDDKernel/AGDDVolume.h"
#include "AGDDKernel/AGDDVolumeStore.h"

#include "CLHEP/Geometry/Transform3D.h"
#include "CLHEP/Vector/ThreeVector.h"
#include "CLHEP/Vector/Rotation.h"

AGDDPositioner::AGDDPositioner(std::string n,HepGeom::Transform3D t):isSensitiveDetector(false),volume(n),transform(t)
{
	AGDDPositionerStore::GetPositionerStore()->RegisterPositioner(this);
	theVolume=AGDDVolumeStore::GetVolumeStore()->GetVolume(Volume());
}
AGDDPositioner::AGDDPositioner(std::string n,CLHEP::HepRotation r,CLHEP::Hep3Vector v):isSensitiveDetector(false),volume(n)
{
	AGDDPositionerStore::GetPositionerStore()->RegisterPositioner(this);
	transform=HepGeom::Transform3D(r,v);
	theVolume=AGDDVolumeStore::GetVolumeStore()->GetVolume(Volume());
}
std::string AGDDPositioner::Volume() 
{
	return volume;
}
const HepGeom::Transform3D& AGDDPositioner::Transform() 
{
	return transform;
} 

AGDDVolume* AGDDPositioner::GetVolume() 
{
	return theVolume;
} 

