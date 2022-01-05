/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AGDDElcyl_H
#define AGDDElcyl_H

#include "AGDDKernel/AGDDVolume.h"
#include <string>
#include <vector>
#include <iostream>

class AGDDElcyl: public AGDDVolume {
public:
	AGDDElcyl(const std::string& s,
                  AGDDVolumeStore& vs,
                  AGDDSectionStore& ss)
          : AGDDVolume(s,vs,ss),m_dx(0),m_dy(0),m_z(0) {}
	void SetRXio_RYio_Z(const std::vector<double>& v) 
	{
		m_dx=v[0];
		m_dy=v[1];
		m_z=v[2];
	}
        virtual void CreateVolume (AGDDBuilder& builder) override;
	virtual void CreateSolid (const AGDDBuilder& builder) override;
	double dx() const {return m_dx;}
	double dy() const {return m_dy;}
	double z() const {return m_z;}
	double m_dx;
	double m_dy;
	double m_z;
};

#endif
