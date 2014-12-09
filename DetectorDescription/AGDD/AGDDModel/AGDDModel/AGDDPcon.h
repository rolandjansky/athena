/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AGDDPcon_H
#define AGDDPcon_H

#include "AGDDKernel/AGDDVolume.h"
#include <string>
#include <vector>
#include <iostream>

class AGDDPcon: public AGDDVolume {
public:
	AGDDPcon(std::string s):AGDDVolume(s),_phi0(0),_dphi(0) {}
	void SetPlane(double ri,double ro,double z) 
	{
		_rin.push_back(ri);
		_rou.push_back(ro);
		_z.push_back(z);
	}
	void SetProfile(double phi0,double dphi) {_phi0=phi0;_dphi=dphi;}
	int NrOfPlanes() {return _rin.size();}
	double Rin(int i) {return _rin[i];}
	double Rout(int i) {return _rou[i];}
	double Z(int i) {return _z[i];}
	double Phi0() {return _phi0;}
	double Dphi() {return _dphi;}
	void CreateVolume();
	void CreateSolid();
	std::vector<double> _rin;
	std::vector<double> _rou;
	std::vector<double> _z;
	
	double _phi0;
	double _dphi;
};

#endif
