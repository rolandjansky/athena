/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AGDDBox_H
#define AGDDBox_H

#include "AGDDKernel/AGDDVolume.h"
#include <string>
#include <vector>
#include <iostream>

class AGDDBox: public AGDDVolume {
public:
	AGDDBox(const std::string& s,
                 AGDDVolumeStore& vs,
                 AGDDSectionStore& ss)
          : AGDDVolume(s,vs,ss),m_x(0),m_y(0),m_z(0) {}
	~AGDDBox() {}
	void SetXYZ(const std::vector<double>& v) 
	{
		m_x=v[0];
		m_y=v[1];
		m_z=v[2];
	}
	double x() const {return m_x;}
	double y() const {return m_y;}
	double z() const {return m_z;}
        virtual void CreateVolume (AGDDBuilder& builder) override;
	virtual void CreateSolid (const AGDDBuilder& builder) override;
	double m_x;
	double m_y;
	double m_z;
};

#endif
