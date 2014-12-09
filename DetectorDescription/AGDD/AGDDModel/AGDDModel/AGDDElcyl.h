/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AGDDElcyl_H
#define AGDDElcyl_H

#include "AGDDKernel/AGDDVolume.h"
#include <string>
#include <vector>
#include <iostream>

class AGDDElcyl: public AGDDVolume {
public:
	AGDDElcyl(std::string s):AGDDVolume(s),_dx(0),_dy(0),_z(0) {}
	void SetRXio_RYio_Z(std::vector<double> v) 
	{
		_dx=v[0];
		_dy=v[1];
		_z=v[2];
	}
	void CreateVolume();
	void CreateSolid();
	double dx() {return _dx;}
	double dy() {return _dy;}
	double z() {return _z;}
	double _dx;
	double _dy;
	double _z;
};

#endif
