/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AGDDSnake_H
#define AGDDSnake_H

#include "AGDDKernel/AGDDVolume.h"
#include <string>
#include <vector>
#include <iostream>

#include "CLHEP/Vector/ThreeVector.h"

class AGDDSnake: public AGDDVolume {
public:
	AGDDSnake(std::string s):AGDDVolume(s),m_radius(0) {}
	void SetPoint(CLHEP::Hep3Vector p)
	{
		m_points.push_back(p);
	}
	int NrOfPoints() {return m_points.size();}
	double Radius() {return m_radius;}
	void Radius(double d) {m_radius=d;}
	CLHEP::Hep3Vector GetPoint(int i) {return m_points[i];}
	void CreateVolume();
	void CreateSolid();
private:
	double m_radius;
	std::vector<CLHEP::Hep3Vector> m_points;
};

#endif
