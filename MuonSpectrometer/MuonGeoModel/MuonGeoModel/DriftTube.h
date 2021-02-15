/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DriftTube_H
#define DriftTube_H

#include "MuonGeoModel/DetectorElement.h"

#include <string>

class GeoVPhysVol;

namespace MuonGM {

class DriftTube: public DetectorElement {
public:
	std::string gasMaterial;
	std::string tubeMaterial;
	std::string plugMaterial;
	std::string wireMaterial;
	double length;
	double outerRadius;
	double gasRadius;
	double plugLength;

	GeoVPhysVol *build();
	void print();
	DriftTube(std::string s);
};
} // namespace MuonGM


#endif
