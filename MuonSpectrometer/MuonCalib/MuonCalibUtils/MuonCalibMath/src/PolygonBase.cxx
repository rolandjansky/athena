/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include <iostream>
#include <cstdlib>
#include "MuonCalibMath/PolygonBase.h"
#include <TString.h> // for Form

using namespace MuonCalib;

void PolygonBase::init(const std::vector<double> & x) {

///////////////////////////////
// CHECK FOR CORRECT FILLING //
///////////////////////////////

	for (unsigned int k=0; k<x.size()-1; k++) {
		if (x[k]>=x[k+1]) {
			throw std::runtime_error(Form("File: %s, Line: %d\nPolygonBase::init - Abscissae not in increasing order!", __FILE__, __LINE__));
		}
	}

/////////////////////////////
// TRANSFER THE INPUT DATA //
/////////////////////////////

	m_x = x;

	return;

}

//*****************************************************************************

//::::::::::::::::::
//:: METHOD value ::
//::::::::::::::::::

double PolygonBase::value(const int & k, const double & x) const {

///////////////////////////////////
// CHECK THE BASE FUNCTION INDEX //
///////////////////////////////////

	if (k<0 || k>static_cast<int>(m_x.size()-1)) {
		throw std::runtime_error(Form("File: %s, Line: %d\nPolygonBase::value - Illegal function index %i! Should be >0 and <%lu", __FILE__, __LINE__, k, m_x.size()));
	}

//////////////////////////////////////////////
// CALCULATE THE VALUE OF THE BASE FUNCTION //
//////////////////////////////////////////////

// outside the domain of the polygon //
	if (x<m_x[0] || x>m_x[m_x.size()-1]) {
		return 0.0;
	}

// inside the domain of the polygon //
	if (k==0 && x<m_x[0]) {
		return 0.0;
	}
	if (k==0 && x<=m_x[1]) {
		return (m_x[1]-x)/(m_x[1]-m_x[0]);
	}
	if (k==0 && x>m_x[1]) {
		return 0.0;
	}

	if (static_cast<unsigned int>(k)==m_x.size()-1 &&
							x<=m_x[m_x.size()-2]) {
		return 0.0;
	}
	if (static_cast<unsigned int>(k)==m_x.size()-1 &&
							x<=m_x[m_x.size()-1]) {
		return (x-m_x[k-1])/(m_x[k]-m_x[k-1]);
	}
	if (static_cast<unsigned int>(k)==m_x.size()-1 &&
							x>m_x[m_x.size()-1]) {
		return 0.0;
	}

	if (x<m_x[k-1] || x>m_x[k+1]) {
		return 0.0;
	}

	if (x<m_x[k]) {
		return (x-m_x[k-1])/(m_x[k]-m_x[k-1]);
	}

	return (m_x[k+1]-x)/(m_x[k+1]-m_x[k]);

}
