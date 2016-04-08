/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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
	AGDDGvxy(std::string s):AGDDVolume(s),m_dz(0) {}
	void SetDz(double v) 
	{
		m_dz=v;
	}
	void SetPoint(TwoPoint p)
	{
//		std::cout<<" GVXY setting point "<<p.x()<<" "<<p.y()<<std::endl;
		m_points.push_back(p);
	}
	int NrOfPoints() {return m_points.size();}
	TwoPoint GetPoint(int i) {return m_points[i];}
	double GetDz() {return m_dz;}
	void CreateVolume();
	void CreateSolid();
private:
	double m_dz;
	std::vector<TwoPoint> m_points;
};

#endif
