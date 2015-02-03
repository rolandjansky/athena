/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AGDDMuonStation_H
#define AGDDMuonStation_H

#include "AGDDKernel/AGDDVolume.h"
#include <string>
#include <vector>
#include <iostream>

class GeoMaterial;

class AGDDMuonStation: public AGDDVolume {
public:
	AGDDMuonStation(std::string s):AGDDVolume(s),_small_x(0),_large_x(0),_y(0),_z(0) {}
	void SetXYZ(std::vector<double> v) 
	{
		_small_x=v[0];
		_large_x=v[1];
		_y=v[2];
		_z=v[3];
	}
	void small_x(double x) {_small_x=x;}
	void large_x(double x) {_large_x=x;}
	void y(double yval) {_y=yval;}
	void z(double zval) {_z=zval;}
	double small_x() {return _small_x;}
	double large_x() {return _large_x;}
	double y() {return _y;}
	double z() {return _z;}
	void CreateVolume();
	void CreateSolid();
private:
	double _small_x;
	double _large_x;
	double _y;
	double _z;
	
	GeoMaterial* GetMMMaterial(std::string);
};

#endif
