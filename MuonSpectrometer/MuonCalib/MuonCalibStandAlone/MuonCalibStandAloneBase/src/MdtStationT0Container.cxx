/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// 01.02.2007, AUTHOR: OLIVER KORTNER
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
//:: IMPLEMENTATION OF METHODS DEFINED IN THE CLASS MdtStationT0Container ::
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

//::::::::::::::::::
//:: HEADER FILES ::
//::::::::::::::::::

#include <cstdlib>
#include <iostream>
#include <fstream>
#include "MuonCalibStandAloneBase/MdtStationT0Container.h"

//::::::::::::::::::::::::
//:: NAMESPACE SETTINGS ::
//::::::::::::::::::::::::

using namespace std;
using namespace MuonCalib;


//*****************************************************************************

//:::::::::::::::::
//:: METHOD init ::
//:::::::::::::::::

/*void MdtStationT0Container::init(void) {

	m_t0 = vector< vector< vector<double> > >(2); // two multilayers
	m_adc = vector< vector< vector<double> > >(2); // two multilayers
	for (unsigned int k=0; k<m_t0.size(); k++) {
		m_t0[k] = vector< vector<double> >(4); // up to four layers in 
						       // a multilayer
		m_adc[k] = vector< vector<double> >(4); // up to four layers in 
						       // a multilayer
		for (unsigned l=0; l<m_t0[k].size(); l++) {
			m_t0[k][l] = vector<double>(72, 9e9);
			m_adc[k][l] = vector<double>(72, 9e9);
						// up to 72 tubes per layer
		}
	}
	m_t0_loaded=false;
	return;

}*/

//*****************************************************************************

//:::::::::::::::
//:: METHOD t0 ::
//:::::::::::::::

double MdtStationT0Container::t0(const unsigned int & ml,
					const unsigned int & ly,
					const unsigned int & tb) const {

	if (!m_t0_loaded) return 9e9;
	return m_t0[ml-1][ly-1][tb-1];

}

//*****************************************************************************

//:::::::::::::::
//:: METHOD adc ::
//:::::::::::::::

double MdtStationT0Container::adc(const unsigned int & ml,
					const unsigned int & ly,
					const unsigned int & tb) const {

	if (!m_t0_loaded) return 9e9;
	return m_adc[ml-1][ly-1][tb-1];

}

//*****************************************************************************

//::::::::::::::::::
//:: METHOD Sett0 ::
//::::::::::::::::::

bool MdtStationT0Container::Sett0(const unsigned int & ml,
                                        const unsigned int & ly,
                                        const unsigned int & tb, double t0, double adc) {

	m_t0[ml-1][ly-1][tb-1]=t0;
	m_adc[ml-1][ly-1][tb-1]=adc;
	m_t0_loaded=true;

        return m_t0_loaded;

}

//*****************************************************************************

//:::::::::::::::::::::::
//:: METHOD readT0File ::
//:::::::::::::::::::::::

void MdtStationT0Container::readT0File(const std::string & file_name) {

///////////////
// VARIABLES //
///////////////

	string sdummy; // auxiliary string for file reading
	int idummy; // auxiliary integer for file reading
	double dummy; // auxiliary double for file reading
	int ml, ly, tb; // multilayer, layer, tube
	ifstream infile; // t0 file

//////////////////
// OPEN t0 file //
//////////////////

	infile.open(file_name.c_str());
	if (infile.fail()) {
		cerr << endl
			<< "Class MdtStationT0Container, method readToFile: "
			<< "ERROR!\n"
			<< "Could not open file " << file_name << "!\n";
		exit(1);
	}

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
		infile >> m_adc[ml-1][ly-1][tb-1];
		
		if(infile.fail()) return;

   // the rest of the line //
		for (unsigned k=0; k<10; k++) {
			infile >> dummy;
		}

	}
	m_t0_loaded=true;
	return;

}
