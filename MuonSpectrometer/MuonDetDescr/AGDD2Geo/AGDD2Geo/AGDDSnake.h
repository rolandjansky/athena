/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AGDDSnake_H
#define AGDDSnake_H

#include "AGDD2Geo/AGDDVolume.h"
#include <string>
#include <vector>
#include <iostream>

#include "CLHEP/Vector/ThreeVector.h"

class AGDDSnake: public AGDDVolume {
public:
	AGDDSnake(std::string s):AGDDVolume(s) {}
	void SetPoint(CLHEP::Hep3Vector p)
	{
		_points.push_back(p);
	}
	int NrOfPoints() {return _points.size();}
	double Radius() {return _radius;}
	void Radius(double d) {_radius=d;}
	CLHEP::Hep3Vector GetPoint(int i) {return _points[i];}
	void CreateVolume();
	void CreateSolid();
private:
	double _radius;
	std::vector<CLHEP::Hep3Vector> _points;
};

#endif
