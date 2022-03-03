/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AGDDPcon_H
#define AGDDPcon_H

#include "AGDDKernel/AGDDVolume.h"
#include <string>
#include <vector>
#include <iostream>

class AGDDPcon: public AGDDVolume {
public:
	AGDDPcon(const std::string& s,
                 AGDDVolumeStore& vs,
                 AGDDSectionStore& ss)
          : AGDDVolume(s,vs,ss),m_phi0(0),m_dphi(0) {}
	void SetPlane(double ri,double ro,double z) 
	{
		m_rin.push_back(ri);
		m_rou.push_back(ro);
		m_z.push_back(z);
	}
	void SetProfile(double phi0,double dphi) {m_phi0=phi0;m_dphi=dphi;}
	int NrOfPlanes() const {return m_rin.size();}
	double Rin(int i) const {return m_rin[i];}
	double Rout(int i) const {return m_rou[i];}
	double Z(int i) const {return m_z[i];}
	double Phi0() const {return m_phi0;}
	double Dphi() const {return m_dphi;}
        virtual void CreateVolume (AGDDBuilder& builder) override;
	virtual void CreateSolid (const AGDDBuilder& builder) override;
	std::vector<double> m_rin;
	std::vector<double> m_rou;
	std::vector<double> m_z;
	
	double m_phi0;
	double m_dphi;
};

#endif
