/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AGDDTubs_H
#define AGDDTubs_H

#include "AGDDKernel/AGDDVolume.h"
#include <string>
#include <vector>
#include <iostream>

class AGDDTubs: public AGDDVolume {
public:
	AGDDTubs(std::string s):AGDDVolume(s),_rin(0),_rou(0),_z(0),_phi0(0),_dphi(0) {}
	void SetRio_Z(std::vector<double> v) 
	{
		_rin=v[0];
		_rou=v[1];
		_z=v[2];
	}
	void SetProfile(double p0,double dp) {_phi0=p0;_dphi=dp;}
	void CreateVolume();
	void CreateSolid();
	double rin() {return _rin;}
	double rou() {return _rou;}
	double z() {return _z;}
	double phi0() {return _phi0;}
	double dphi() {return _dphi;}
	double _rin;
	double _rou;
	double _z;
	double _phi0;
	double _dphi;
};

#endif
