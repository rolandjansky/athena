/*
   Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
 */

#include "AFPProtonTransportTool.h"
#include "AFPProtonTransportPolynomial.h"
#include <string>
#include <fstream>
#include <math.h>

AFPProtonTransportTool::AFPProtonTransportTool(const std::string & type, const std::string & name, const IInterface* parent) : AthAlgTool(type, name, parent) { 
 
  declareInterface<IAFPProtonTransportTool>(this);

}

StatusCode AFPProtonTransportTool::initialize(){

  ATH_MSG_INFO("AFPProtonTransportTool::initialize Initializing transport tool");

  return StatusCode::SUCCESS;
}

AFPProtonTransportTool::~AFPProtonTransportTool () { }

StatusCode AFPProtonTransportTool::load() {
  std::ifstream file;
  file.open(m_filename);
  ATH_MSG_INFO("AFPProtonTransportTool::load "<<m_filename);
  if (!file.is_open()) return StatusCode::FAILURE;

  AFPProtonTransportPolynomial* pol[4][8];

  while (file.peek() == '#') {
    char ch;
    std::string name;
    file >> ch >> name;

	//Fetching variable values from the txt file
	if (name == "m_xPositionInitIP") {
		double value;
		file >> value;
		m_xPositionInitIP = value;
	}
	if (name == "m_yPositionInitIP") {
		double value;
		file >> value;
		m_yPositionInitIP = value;
	}
	if (name == "m_zPositionInitIP") {
		double value;
		file >> value;
		m_zPositionInitIP = value;
	}
	if (name == "m_xSlopeInitIP") {
		double value;
		file >> value;
		m_xSlopeInitIP = value;
	}
	if (name == "m_ySlopeInitIP") {
		double value;
		file >> value;
		m_ySlopeInitIP = value;
	}
	if (name == "m_detectorPosition") {
		double value;
		file >> value;
		m_parametrisationPosition = value;
	}
	if (name == "m_nominalEnergy") {
		double value;
		file >> value;
		m_energy = value;
	}

	file.ignore(1000, '\n');
  }

  //Fetching the polynomial coeficients
  for (int iEqu = 0; iEqu < 4; iEqu++) {
	  file.ignore(1000, '\n');
	  for (int iPoly = 0; iPoly < 8; iPoly++) {
		  int N = 0;
		  file >> N;
		  double ch[10];
		  for (int i = 0; i < 10; i++) {
			  if (i <= N) file >> ch[i];
			  else ch[i] = 0;
		  }
		  file.ignore(1000, '\n');

		  //Create polynomial object with the coeficients loaded from the file
		  pol[iEqu][iPoly] =
			  new AFPProtonTransportPolynomial(N, ch[0], ch[1], ch[2], ch[3], ch[4], ch[5], ch[6], ch[7], ch[8], ch[9]);
	  }
  }
  //Creating the Parameterization objects with the polynomials created
  m_x_position = new AFPProtonTransportParam(m_energy, pol[0]);
  m_y_position = new AFPProtonTransportParam(m_energy, pol[1]);
  m_x_slope = new AFPProtonTransportParam(m_energy, pol[2]);
  m_y_slope = new AFPProtonTransportParam(m_energy, pol[3]);

  return StatusCode::SUCCESS;
}
