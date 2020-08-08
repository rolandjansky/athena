/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AGDDPositioner_H
#define AGDDPositioner_H

#include "CLHEP/Geometry/Transform3D.h"
#include "CLHEP/Vector/ThreeVector.h"
#include "CLHEP/Vector/Rotation.h"
#include "GeoModelKernel/GeoDefinitions.h"

#include <string>

class AGDDVolume;

class AGDDPositioner {
public:
	AGDDPositioner(std::string n,HepGeom::Transform3D t);
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
