/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AGDDGvxy_H
#define AGDDGvxy_H

#include "AGDDKernel/AGDDVolume.h"
#include "AGDDKernel/TwoPoint.h"
#include <string>
#include <vector>
#include <iostream>

class AGDDGvxy: public AGDDVolume {
public:
	AGDDGvxy(const std::string& s,
                 AGDDVolumeStore& vs,
                 AGDDSectionStore& ss)
          : AGDDVolume(s,vs,ss),m_dz(0) {}
	void SetDz(double v) 
	{
		m_dz=v;
	}
	void SetPoint(TwoPoint p)
	{
//		std::cout<<" GVXY setting point "<<p.x()<<" "<<p.y()<<std::endl;
		m_points.push_back(p);
	}
	int NrOfPoints() const {return m_points.size();}
	TwoPoint GetPoint(int i) const {return m_points[i];}
	double GetDz() const {return m_dz;}
        virtual void CreateVolume (AGDDBuilder& builder) override;
	virtual void CreateSolid (const AGDDBuilder& builder) override;
private:
	double m_dz;
	std::vector<TwoPoint> m_points;
};

#endif
