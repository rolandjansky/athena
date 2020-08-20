/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AGDDSnake_H
#define AGDDSnake_H

#include "AGDDKernel/AGDDVolume.h"

#include <string>
#include <vector>
#include <iostream>

class AGDDSnake: public AGDDVolume {
public:
	AGDDSnake(std::string s):AGDDVolume(s),m_radius(0) {}
	void SetPoint(GeoTrf::Vector3D p)
	{
		m_points.push_back(p);
	}
	int NrOfPoints() {return m_points.size();}
	double Radius() {return m_radius;}
	void Radius(double d) {m_radius=d;}
	GeoTrf::Vector3D GetPoint(int i) {return m_points[i];}
	void CreateVolume();
	void CreateSolid();
private:
	double m_radius;
	std::vector<GeoTrf::Vector3D> m_points;
};

#endif
