/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AGDDCons_H
#define AGDDCons_H

#include "AGDDKernel/AGDDVolume.h"
#include <string>
#include <vector>
#include <iostream>

class AGDDCons: public AGDDVolume {
public:
	AGDDCons(const std::string& s,
                 AGDDVolumeStore& vs,
                 AGDDSectionStore& ss)
          : AGDDVolume(s,vs,ss),
            m_rin1(0),m_rin2(0),m_rou1(0),m_rou2(0),
            m_z(0),m_phi0(0),m_dphi(0) {}
	~AGDDCons() {}
	void SetRio_Z(const std::vector<double>& v) 
	{
		m_rin2=v[0];
		m_rou2=v[1];
		m_rin1=v[2];
		m_rou1=v[3];
		m_z=v[4];
	}
	void SetProfile(double p0,double dp) {m_phi0=p0;m_dphi=dp;}
	double rin1() const {return m_rin1;}
	double rin2() const {return m_rin2;}
	double rou1() const {return m_rou1;}
	double rou2() const {return m_rou2;}
	double phi0() const {return m_phi0;}
	double dphi() const {return m_dphi;}
	double z() const {return m_z;}
        virtual void CreateVolume (AGDDBuilder& builder) override;
	virtual void CreateSolid (const AGDDBuilder& builder) override;
	double m_rin1;
	double m_rin2;
	double m_rou1;
	double m_rou2;
	double m_z;
	double m_phi0;
	double m_dphi;
};

#endif
