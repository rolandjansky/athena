/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AGDDCons_H
#define AGDDCons_H

#include "AGDDKernel/AGDDVolume.h"
#include <string>
#include <vector>
#include <iostream>

class AGDDCons: public AGDDVolume {
public:
	AGDDCons(std::string s):AGDDVolume(s),_rin1(0),_rin2(0),_rou1(0),_rou2(0),
						   _z(0),_phi0(0),_dphi(0) {}
	~AGDDCons() {}
	void SetRio_Z(std::vector<double> v) 
	{
		_rin2=v[0];
		_rou2=v[1];
		_rin1=v[2];
		_rou1=v[3];
		_z=v[4];
	}
	void SetProfile(double p0,double dp) {_phi0=p0;_dphi=dp;}
	double rin1() {return _rin1;}
	double rin2() {return _rin2;}
	double rou1() {return _rou1;}
	double rou2() {return _rou2;}
	double phi0() {return _phi0;}
	double dphi() {return _dphi;}
	double z() {return _z;}
	void CreateVolume();
	void CreateSolid();
	double _rin1;
	double _rin2;
	double _rou1;
	double _rou2;
	double _z;
	double _phi0;
	double _dphi;
};

#endif
