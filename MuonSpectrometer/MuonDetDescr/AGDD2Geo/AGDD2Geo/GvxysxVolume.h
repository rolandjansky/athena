/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GvxysxVolume_H
#define GvxysxVolume_H

#include "AGDD2Geo/AGDDVolume.h"
#include <string>
#include <vector>
#include <iostream>

#include "AGDD2Geo/TwoPoint.h"

class GvxysxVolume: public AGDDVolume {
public:
	GvxysxVolume(std::string s):AGDDVolume(s) {}
	void SetDz(double v) 
	{
		_dz=v;
	}
	void SetPoint(TwoPoint p)
	{
		std::cout<<" Gvxysx setting point "<<p.x()<<" "<<p.y()<<std::endl;
		_points.push_back(p);
	}
private:
	double _dz;
	std::vector<TwoPoint> _points;
};

#endif
