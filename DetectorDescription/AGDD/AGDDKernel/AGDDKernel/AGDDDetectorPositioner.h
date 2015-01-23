/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AGDDDetectorPositioner_H
#define AGDDDetectorPositioner_H

#include "GeoModelKernel/GeoFullPhysVol.h"
#include "CLHEP/Geometry/Transform3D.h"
#include "CLHEP/Vector/ThreeVector.h"
#include "CLHEP/Vector/Rotation.h"

#include "AGDDKernel/AGDDPositioner.h"

#include <string>

class AGDDVolume;

class AGDDDetector;

struct Identifiers {
	std::string detectorType;
	std::string detectorAddress;
	int phiIndex;
	int etaIndex;
	int radIndex;
	int sideIndex;
	int mlayerIndex;
	int spare1;
	int spare2;
};

class AGDDDetectorPositioner:public AGDDPositioner {
public:
	AGDDDetectorPositioner(std::string n,HepGeom::Transform3D t);
	AGDDDetectorPositioner(std::string n,CLHEP::HepRotation r,CLHEP::Hep3Vector v);

	Identifiers ID;
	
	AGDDDetector* theDetector;
	
	GeoFullPhysVol* theVolume;
};

#endif
