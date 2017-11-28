/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MM_Technology_H
#define MM_Technology_H

#include "AGDDKernel/AGDDTechnology.h"
#include <vector>
namespace MuonGM {

// Description class to build MicroMegas chambers

class MM_Technology: public AGDDTechnology {
public:
	double thickness;
	int nlayers;
	double gasThickness;
	double pcbThickness;
	double roThickness;
	double f1Thickness;
	double f2Thickness;
	double f3Thickness;

	int geoLevel;


	// inner structure parameters (to be defined)

	// constructor
	inline MM_Technology(std::string s);
	inline double Thickness() const;
};

MM_Technology::MM_Technology(std::string s): AGDDTechnology(s),
  thickness(0.), nlayers(0), gasThickness(0.), pcbThickness(0.),
  roThickness(0), f1Thickness(0.), f2Thickness(0.), f3Thickness(0.),
  geoLevel(0)
{
}

double MM_Technology::Thickness() const
{
	//thickness=nlayers*(gasThickness+pcbThickness) + 2.*pcbThickness;
	return thickness;
}

} // namespace MuonGM

#endif
