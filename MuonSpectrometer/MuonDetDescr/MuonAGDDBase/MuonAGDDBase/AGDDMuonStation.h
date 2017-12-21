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
	AGDDMuonStation(std::string s):AGDDVolume(s),m_small_x(0),m_large_x(0),m_y(0),m_z(0) {}
	void SetXYZ(std::vector<double> v) 
	{
		m_small_x=v[0];
		m_large_x=v[1];
		m_y=v[2];
		m_z=v[3];
	}
	void small_x(double x) {m_small_x=x;}
	void large_x(double x) {m_large_x=x;}
	void y(double yval) {m_y=yval;}
	void z(double zval) {m_z=zval;}
	double small_x() {return m_small_x;}
	double large_x() {return m_large_x;}
	double y() {return m_y;}
	double z() {return m_z;}
	void CreateVolume();
	void CreateSolid();
private:
	double m_small_x;
	double m_large_x;
	double m_y;
	double m_z;
	
	GeoMaterial* GetMMMaterial(std::string);
};

#endif
