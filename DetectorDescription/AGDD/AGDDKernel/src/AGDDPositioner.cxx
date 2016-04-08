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

AGDDPositioner::AGDDPositioner(std::string n,HepGeom::Transform3D t):m_isSensitiveDetector(false),m_volume(n),m_transform(t)
{
	AGDDPositionerStore::GetPositionerStore()->RegisterPositioner(this);
	m_theVolume=AGDDVolumeStore::GetVolumeStore()->GetVolume(Volume());
}
AGDDPositioner::AGDDPositioner(std::string n,CLHEP::HepRotation r,CLHEP::Hep3Vector v):m_isSensitiveDetector(false),m_volume(n)
{
	AGDDPositionerStore::GetPositionerStore()->RegisterPositioner(this);
	m_transform=HepGeom::Transform3D(r,v);
	m_theVolume=AGDDVolumeStore::GetVolumeStore()->GetVolume(Volume());
}
std::string AGDDPositioner::Volume() 
{
	return m_volume;
}
const HepGeom::Transform3D& AGDDPositioner::Transform() 
{
	return m_transform;
} 

AGDDVolume* AGDDPositioner::GetVolume() 
{
	return m_theVolume;
} 

