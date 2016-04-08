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
	AGDDPositioner(std::string n,HepGeom::Transform3D t);
	AGDDPositioner(std::string n,CLHEP::HepRotation r,CLHEP::Hep3Vector v);
	virtual ~AGDDPositioner() {;}
	std::string Volume();
	AGDDVolume *GetVolume();
	const HepGeom::Transform3D& Transform();
    bool IsSensitiveDetector() {return m_isSensitiveDetector;}
	void SensitiveDetector(bool a) {m_isSensitiveDetector=a;}
	
private:

	bool m_isSensitiveDetector;
	std::string m_volume;
	AGDDVolume *m_theVolume;
	HepGeom::Transform3D m_transform;
};

#endif
