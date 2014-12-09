/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AGDDTrd_H
#define AGDDTrd_H

#include "AGDDKernel/AGDDVolume.h"
#include <string>
#include <vector>
#include <iostream>

class AGDDTrd: public AGDDVolume {
public:
	AGDDTrd(std::string s):AGDDVolume(s),_x1(0),_x2(0),_y1(0),_y2(0),_z(0) {}
	void SetXYZ(std::vector<double> v) 
	{
		_x1=v[0];
		_x2=v[1];
		_y1=v[2];
		_y2=v[3];
		_z=v[4];
	}
	double x1() {return _x1;}
	double x2() {return _x2;}
	double y1() {return _y1;}
	double y2() {return _y2;}
	double z() {return _z;}
	void CreateVolume();
	void CreateSolid();
	double _x1;
	double _x2;
	double _y1;
	double _y2;
	double _z;
};

#endif
