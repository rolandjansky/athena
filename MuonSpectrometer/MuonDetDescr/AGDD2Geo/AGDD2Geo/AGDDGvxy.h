/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AGDDGvxy_H
#define AGDDGvxy_H

#include "AGDD2Geo/AGDDVolume.h"
#include <string>
#include <vector>
#include <iostream>

#include "AGDD2Geo/TwoPoint.h"

class AGDDGvxy: public AGDDVolume {
public:
	AGDDGvxy(std::string s):AGDDVolume(s) {}
	void SetDz(double v) 
	{
		_dz=v;
	}
	void SetPoint(TwoPoint p)
	{
//		std::cout<<" GVXY setting point "<<p.x()<<" "<<p.y()<<std::endl;
		_points.push_back(p);
	}
	int NrOfPoints() {return _points.size();}
	TwoPoint GetPoint(int i) {return _points[i];}
	double GetDz() {return _dz;}
	void CreateVolume();
	void CreateSolid();
private:
	double _dz;
	std::vector<TwoPoint> _points;
};

#endif
