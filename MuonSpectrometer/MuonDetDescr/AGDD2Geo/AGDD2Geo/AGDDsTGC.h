/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AGDDsTGC_H
#define AGDDsTGC_H

#include "AGDD2Geo/AGDDVolume.h"
#include <string>
#include <vector>
#include <iostream>

class GeoMaterial;

class AGDDsTGC: public AGDDVolume {
public:
	AGDDsTGC(std::string s):AGDDVolume(s) {}
	void SetXYZ(std::vector<double> v) 
	{
		_small_x=v[0];
		_large_x=v[1];
		_y=v[2];
		_z=v[3];
		_yCutout=v[4];
	}
	void small_x(double x) {_small_x=x;}
	void large_x(double x) {_large_x=x;}
	void y(double yval) {_y=yval;}
	void z(double zval) {_z=zval;}
	void yCutout(double y) {_yCutout=y;}
	void subType(std::string s) {sType=s;}
	double small_x() {return _small_x;}
	double large_x() {return _large_x;}
	double y() {return _y;}
	double z() {return _z;}
	double yCutout() {return _yCutout;}

	std::string subType() {return sType;}
	std::string tech;
	void CreateVolume();
	void CreateSolid();
private:
	double _small_x;
	double _large_x;
	double _y;
	double _z;
	double _yCutout;

	std::string sType;
	
	GeoMaterial* GetMMMaterial(std::string);
};

#endif
