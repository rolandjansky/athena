/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
 * ZdcRecNoiseTool.cxx
 *
 *  Pedestal calculation and retrieval from file or DB.
 *  Also used to study noise performance of the electronics
 *  Created on: Feb 18, 2010
 *      Author: leite
 */



#include <iostream>
#include <fstream>

#include <numeric>
#include <algorithm>
#include <vector>
#include <map>

//#include <boost/foreach.hpp>

//#include "Identifier/HWIdentifier.h"

#include "GaudiKernel/IInterface.h"
#include "GaudiKernel/MsgStream.h"

//#include "ZdcIdentifier/ZdcID.h"
#include "ZdcEvent/ZdcDigits.h"
#include "ZdcEvent/ZdcDigitsCollection.h"
#include "ZdcRec/ZdcRecNoiseTool.h"



#define foreach         BOOST_FOREACH
#define reverse_foreach BOOST_REVERSE_FOREACH


//==================================================================================================
const InterfaceID& ZdcRecNoiseTool::interfaceID()
{
	return IID_IZdcRecNoiseTool;

}
//==================================================================================================

//==================================================================================================
ZdcRecNoiseTool::ZdcRecNoiseTool(const std::string& type,
								          const std::string& name,
								          const IInterface* parent) :

								          AthAlgTool(type, name, parent)

{
	//Declare properties here...

	declareInterface<ZdcRecNoiseTool>(this);

	declareProperty("PedestalsDirectory", m_pedestalDir = "/afs/cern.ch/user/l/leite/public/ZdcData/Pedestals",
							"Pedestal files directory");
	declareProperty("PedestalsFile", m_pedestalFile = "zdc_pedestal_0000.data",
							"Pedestal file name");

}
//==================================================================================================

//==================================================================================================
ZdcRecNoiseTool::~ZdcRecNoiseTool()
{
}
//==================================================================================================

//==================================================================================================
int ZdcRecNoiseTool::readPedestals()
{
	m_pedestalData = new ZdcDigitsCollection;

	int nsamples = 0;
	int i = 0;
	unsigned int id;

	//Read the file name as passed to the tool for the pedestals
	std::string     str = m_pedestalDir + m_pedestalFile;
	std::ifstream  infile;

	infile.open(str.data(), std::ifstream::in);
	if (infile.fail()) {
		msg(MSG::ERROR) << "ZDC ---> No pedestal file available - aborting ";
		return 0;
	}
	else {
		infile >> nsamples;
		std::vector<int> fadc00(nsamples);
		std::vector<int> fadc01(nsamples);
		std::vector<int> fadc10(nsamples);
		std::vector<int> fadc11(nsamples);
		int sz = sizeof(int)*nsamples;
		i = 0;
		while (infile.good()) {
			infile.read ((char *)&id,sizeof(id));
			infile.read ((char *)&fadc00[0],sz);
			infile.read ((char *)&fadc01[0],sz);
			infile.read ((char *)&fadc10[0],sz);
			infile.read ((char *)&fadc11[0],sz);

			ZdcDigits* digits_p = new ZdcDigits(Identifier(id));
			digits_p->set_digits_gain0_delay0(fadc00);
			digits_p->set_digits_gain0_delay1(fadc01);
			digits_p->set_digits_gain1_delay0(fadc10);
			digits_p->set_digits_gain1_delay0(fadc11);
			m_pedestalData->push_back(digits_p);
			i++;
		}
		msg(MSG::INFO) << "ZDC ---> Read " << i <<  " pedestal channels from file" ;
	}
return i;
}
//==================================================================================================

//==================================================================================================
int ZdcRecNoiseTool::writePedestals()
{
	//remove this
	m_pedestalData = new ZdcDigitsCollection;

	int nsamples = 0;
	int i = 0;
	unsigned int id;

	//Read the file name as passed to the tool for the pedestals
	std::string     str = m_pedestalDir + m_pedestalFile;
	std::ofstream  outfile;

	//TODO: First check if it exists, never overwrites
	//TODO: Check if size of pedestal collection is ok

	outfile.open(str.data(), std::ofstream::out);
		if (outfile.fail()) {
			msg(MSG::ERROR) << "ZDC ---> Cannot create Pedestal File - aborting ";
			return 0;
		}
		else {
			std::vector<int> fadc00(nsamples);
			std::vector<int> fadc01(nsamples);
			std::vector<int> fadc10(nsamples);
			std::vector<int> fadc11(nsamples);
			outfile << nsamples;
			int sz = sizeof(int)*nsamples;

			for (const ZdcDigits* p : *m_pedestalData) {
				id = p->identify().get_identifier32().get_compact();
				outfile.write((char *)&id, sizeof(id));

				fadc00 = p->get_digits_gain0_delay0();
				fadc01 = p->get_digits_gain0_delay1();
				fadc10 = p->get_digits_gain1_delay0();
				fadc11 = p->get_digits_gain1_delay1();
				outfile.write ((char *)&fadc00, sz);
				outfile.write ((char *)&fadc01, sz);
				outfile.write ((char *)&fadc10, sz);
				outfile.write ((char *)&fadc11, sz);
				i++;
			}
		}
	return i;
}



//==================================================================================================
//==================================================================================================
StatusCode ZdcRecNoiseTool::initialize()
{
	msg(MSG::INFO) << "Initializing " << name() << endmsg;

	//Get the pedestal information for the channels.
	//For now, this is a file; later on it will be moved to a database


	return StatusCode::SUCCESS;
}
//==================================================================================================

//==================================================================================================
StatusCode ZdcRecNoiseTool::finalize()
{
	msg(MSG::INFO) << "Finalizing " << name() << endmsg;
	return StatusCode::SUCCESS;
}
//==================================================================================================
