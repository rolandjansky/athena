/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AGDDMMSpacer_H
#define AGDDMMSpacer_H

#include "AGDDKernel/AGDDVolume.h"
#include <string>
#include <vector>
#include <iostream>

class GeoMaterial;
class AGDDVolumeStore;
class AGDDSectionStore;

class AGDDMMSpacer: public AGDDVolume {
public:
	AGDDMMSpacer(const std::string& s,
                     AGDDVolumeStore& vs,
                     AGDDSectionStore& ss)
          : AGDDVolume(s,vs,ss),m_small_x(0),m_large_x(0),m_y(0),m_z(0) {}
	void SetXYZ(const std::vector<double>& v) 
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
	double small_x() const {return m_small_x;}
	double large_x() const {return m_large_x;}
	double y() const {return m_y;}
	double z() const {return m_z;}
	std::string tech;
        virtual void CreateVolume (AGDDBuilder& builder) override;
	virtual void CreateSolid (const AGDDBuilder& builder) override;
private:
	double m_small_x;
	double m_large_x;
	double m_y;
	double m_z;
};

#endif
