/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef sTGC_Technology_H
#define sTGC_Technology_H

#include "AGDDKernel/AGDDTechnology.h"
#include <vector>
namespace MuonGM {

// Description class to build sTGC chambers

class sTGC_Technology: public AGDDTechnology {
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
	inline double Thickness() const;
};

sTGC_Technology::sTGC_Technology(std::string s): AGDDTechnology(s),
  thickness(0.), nlayers(0), gasThickness(0.), pcbThickness(0.),
  f4Thickness(0.), f5Thickness(0.), f6Thickness(0.),
  geoLevel(0)
{
}

double sTGC_Technology::Thickness() const
{
	//thickness=nlayers*(gasThickness+pcbThickness) + pcbThickness;
	return thickness;
}

} // namespace MuonGM

#endif
