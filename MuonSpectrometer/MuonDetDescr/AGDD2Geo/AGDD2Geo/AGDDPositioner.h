/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AGDDPositioner_H
#define AGDDPositioner_H

#include "CLHEP/Geometry/Transform3D.h"
#include "CLHEP/Vector/ThreeVector.h"
#include "CLHEP/Vector/Rotation.h"
#include <string>

class AGDDVolume;

class AGDDPositioner {
public:
	AGDDPositioner(std::string n,HepGeom::Transform3D t, int nC=0);
	AGDDPositioner(std::string n,CLHEP::HepRotation r,CLHEP::Hep3Vector v, int nC=0);
	std::string Volume();
	AGDDVolume *GetVolume();
	int GetCopyNumber() const;
	void SetCopyNumber(int);
	const HepGeom::Transform3D& Transform();
private:
	std::string volume;
	AGDDVolume *theVolume;
	HepGeom::Transform3D transform;
	int copyNo;
};

#endif
