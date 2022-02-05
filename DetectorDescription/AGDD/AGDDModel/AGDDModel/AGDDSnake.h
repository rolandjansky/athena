/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AGDDSnake_H
#define AGDDSnake_H

#include "AGDDKernel/AGDDVolume.h"

#include <string>
#include <vector>
#include <iostream>

class AGDDSnake: public AGDDVolume {
public:
	AGDDSnake(const std::string& s,
                  AGDDVolumeStore& vs,
                  AGDDSectionStore& ss)
          : AGDDVolume(s,vs,ss),m_radius(0) {}
	void SetPoint(const GeoTrf::Vector3D& p)
	{
		m_points.push_back(p);
	}
	int NrOfPoints() const {return m_points.size();}
	double Radius() const {return m_radius;}
	void Radius(double d) {m_radius=d;}
	GeoTrf::Vector3D GetPoint(int i) const {return m_points[i];}
        virtual void CreateVolume (AGDDBuilder& builder) override;
	virtual void CreateSolid (const AGDDBuilder& builder) override;
private:
	double m_radius;
	std::vector<GeoTrf::Vector3D> m_points;
};

#endif
