/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef sTGC_Technology_H
#define sTGC_Technology_H

#include "MuonGeoModel/Technology.h"
#include <vector>
namespace MuonGM {

// Description class to build sTGC chambers

class sTGC_Technology: public Technology {
public:
    double thickness;
	int nlayers;
	double gasThickness;
	double pcbThickness;
	double f4Thickness;
        double f5Thickness;
        double f6Thickness;

	

	int geoLevel;

	// inner structure parameters (to be defined)
	
	// constructor
	inline sTGC_Technology(std::string s);
	inline double Thickness() ;
};

sTGC_Technology::sTGC_Technology(std::string s): Technology(s),geoLevel(0)
{
}

double sTGC_Technology::Thickness() 
{
	//thickness=nlayers*(gasThickness+pcbThickness) + pcbThickness;
	return thickness;
}

} // namespace MuonGM

#endif
