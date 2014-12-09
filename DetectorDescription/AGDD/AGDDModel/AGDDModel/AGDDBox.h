/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AGDDBox_H
#define AGDDBox_H

#include "AGDDKernel/AGDDVolume.h"
#include <string>
#include <vector>
#include <iostream>

class AGDDBox: public AGDDVolume {
public:
	AGDDBox(std::string s):AGDDVolume(s),_x(0),_y(0),_z(0) {}
	~AGDDBox() {}
	void SetXYZ(std::vector<double> v) 
	{
		_x=v[0];
		_y=v[1];
		_z=v[2];
	}
	double x() {return _x;}
	double y() {return _y;}
	double z() {return _z;}
	void CreateVolume();
	void CreateSolid();
	double _x;
	double _y;
	double _z;
};

#endif
