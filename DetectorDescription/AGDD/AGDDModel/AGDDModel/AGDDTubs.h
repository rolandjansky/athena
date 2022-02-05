/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AGDDTubs_H
#define AGDDTubs_H

#include "AGDDKernel/AGDDVolume.h"
#include <string>
#include <vector>
#include <iostream>

class AGDDTubs: public AGDDVolume {
public:
	AGDDTubs(const std::string& s,
                 AGDDVolumeStore& vs,
                 AGDDSectionStore& ss)
          : AGDDVolume(s,vs,ss),m_rin(0),m_rou(0),m_z(0),m_phi0(0),m_dphi(0) {}
	void SetRio_Z(const std::vector<double>& v) 
	{
		m_rin=v[0];
		m_rou=v[1];
		m_z=v[2];
	}
	void SetProfile(double p0,double dp) {m_phi0=p0;m_dphi=dp;}
        virtual void CreateVolume (AGDDBuilder& builder) override;
	virtual void CreateSolid (const AGDDBuilder& builder) override;
	double rin() const {return m_rin;}
	double rou() const {return m_rou;}
	double z() const {return m_z;}
	double phi0() const {return m_phi0;}
	double dphi() const {return m_dphi;}
	double m_rin;
	double m_rou;
	double m_z;
	double m_phi0;
	double m_dphi;
};

#endif
