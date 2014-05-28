/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AGDD2Geo/AGDDPositioner.h"
#include "AGDD2Geo/AGDDPositionerStore.h"
#include "AGDD2Geo/AGDDVolume.h"
#include "AGDD2Geo/AGDDVolumeStore.h"

#include "CLHEP/Geometry/Transform3D.h"
#include "CLHEP/Vector/ThreeVector.h"
#include "CLHEP/Vector/Rotation.h"

AGDDPositioner::AGDDPositioner(std::string n,HepGeom::Transform3D t,int i):volume(n),transform(t),copyNo(i)
{
	AGDDPositionerStore::GetPositionerStore()->RegisterPositioner(this);
	theVolume=AGDDVolumeStore::GetVolumeStore()->GetVolume(Volume());
}
AGDDPositioner::AGDDPositioner(std::string n,CLHEP::HepRotation r,CLHEP::Hep3Vector v,int i):volume(n),copyNo(i)
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

int AGDDPositioner::GetCopyNumber() const
{
	return copyNo;
}

void AGDDPositioner::SetCopyNumber(int i) 
{
	copyNo=i;
}
