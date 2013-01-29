/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// 01.02.2007, AUTHOR: OLIVER KORTNER
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
//:: IMPLEMENTATION OF METHODS DEFINED IN THE CLASS StationT0FromFile ::
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

//::::::::::::::::::
//:: HEADER FILES ::
//::::::::::::::::::

#include <iostream>
#include <fstream>
#include "MdtCalibDbAsciiTool/StationT0FromFile.h"

//::::::::::::::::::::::::
//:: NAMESPACE SETTINGS ::
//::::::::::::::::::::::::

using namespace std;
using namespace MuonCalib;


//*****************************************************************************

//:::::::::::::::::
//:: METHOD init ::
//:::::::::::::::::

void StationT0FromFile::init(void) {

	m_t0 = vector< vector< vector<double> > >(2); // two multilayers
	for (unsigned int k=0; k<m_t0.size(); k++) {
		m_t0[k] = vector< vector<double> >(4); // up to four layers in 
						       // a multilayer
		for (unsigned l=0; l<m_t0[k].size(); l++) {
			m_t0[k][l] = vector<double>(72, 0);
						// up to 72 tubes per layer
		}
	}

	return;

}

//*****************************************************************************

//:::::::::::::::
//:: METHOD t0 ::
//:::::::::::::::

double StationT0FromFile::t0(const unsigned int & ml,
					const unsigned int & ly,
					const unsigned int & tb) const {

	return m_t0[ml-1][ly-1][tb-1];

}

//*****************************************************************************

//:::::::::::::::::::::::
//:: METHOD readT0File ::
//:::::::::::::::::::::::

void StationT0FromFile::readT0File(std::istream &infile) {

///////////////
// VARIABLES //
///////////////

	string sdummy; // auxiliary string for file reading
	int idummy; // auxiliary integer for file reading
	double dummy; // auxiliary double for file reading
	int ml, ly, tb; // multilayer, layer, tube

//////////////////////
// READ THE t0 FILE //
//////////////////////

// ignore the first line //
	getline(infile, sdummy);

// read the t0 look-up table //
	while (!infile.eof()) {

   // station name //
		infile >> sdummy;
		if (infile.eof()) {
			break;
		}

   // eta, phi, technology index //
		infile >> idummy >> idummy >> idummy;

   // multilayer, layer, tube //
		infile >> ml >> ly >> tb;

   // t0 //
		infile >> m_t0[ml-1][ly-1][tb-1];

   // the rest of the line //
		for (unsigned k=0; k<11; k++) {
			infile >> dummy;
		}

	}

	return;

}
