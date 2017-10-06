/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// 29.06.2006, AUTHOR: OLIVER KORTNER
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//::::::::::::::::::
//:: HEADER FILES ::
//::::::::::::::::::

#include <iostream>
#include <cstdlib>
#include "MuonCalibMath/PolygonBase.h"

//::::::::::::::::::::::::
//:: NAMESPACE SETTINGS ::
//::::::::::::::::::::::::

using namespace MuonCalib;
using namespace std;

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
//:: IMPLEMENTATION OF METHODS DEFINED IN THE CLASS PolygonBase ::
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

//*****************************************************************************

//:::::::::::::::::::
//:: METHOD init ::
//:::::::::::::::::::

void PolygonBase::init(const std::vector<double> & x) {

///////////////////////////////
// CHECK FOR CORRECT FILLING //
///////////////////////////////

	for (unsigned int k=0; k<x.size()-1; k++) {
		if (x[k]>=x[k+1]) {
			cerr << "\n"
				<< "Class PolygonBase, method init: ERROR!\n"
				<< "Abscissae not in increasing order!\n";
			exit(1);
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
		cerr << "\n"
			<< "Class PolygonBase, method value: ERROR!\n"
			<< "Illegal function index " << k << "!\n"
			<< "Should be >0 and <" << m_x.size() << ".\n";
		exit(1);
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
