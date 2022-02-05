/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AGDDTrd_H
#define AGDDTrd_H

#include "AGDDKernel/AGDDVolume.h"
#include <string>
#include <vector>
#include <iostream>

class AGDDTrd: public AGDDVolume {
public:
	AGDDTrd(const std::string& s,
                AGDDVolumeStore& vs,
                AGDDSectionStore& ss)
          : AGDDVolume(s,vs,ss),m_x1(0),m_x2(0),m_y1(0),m_y2(0),m_z(0) {}
	void SetXYZ(const std::vector<double>& v) 
	{
		m_x1=v[0];
		m_x2=v[1];
		m_y1=v[2];
		m_y2=v[3];
		m_z=v[4];
	}
	double x1() const {return m_x1;}
	double x2() const {return m_x2;}
	double y1() const {return m_y1;}
	double y2() const {return m_y2;}
	double z() const {return m_z;}
        virtual void CreateVolume (AGDDBuilder& builder) override;
	virtual void CreateSolid (const AGDDBuilder& builder) override;
	double m_x1;
	double m_x2;
	double m_y1;
	double m_y2;
	double m_z;
};

#endif
