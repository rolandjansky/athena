/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
 * ShowerLibList.cpp
 *
 *  Created on: Mar 24, 2011
 *      Author: sedov
 */

#include "LArG4ShowerLib/ShowerLibList.h"
#include <vector>


// list of all libraries
#include "LArG4ShowerLib/EtaEnergyShowerLib.h"
#include "LArG4ShowerLib/TestShowerLib.h"
#include "LArG4ShowerLib/FCALDistEnergyShowerLib.h"
#include "LArG4ShowerLib/FCALDistEtaEnergyShowerLib.h"

namespace ShowerLib {

	typedef IShowerLib* (*readTTree)(TFile*);
	typedef std::vector<readTTree> vectorTTree;
	typedef IShowerLib* (*readStruct)(const std::string&);
	typedef std::vector<readStruct> vectorStruct;

	vectorTTree vectorTTreeFull()
	{
    	vectorTTree ttreeVector;

		// List all TTree reader functions
		ttreeVector.push_back(&(EtaEnergyShowerLib::readFromROOTFile));
		ttreeVector.push_back(&(FCALDistEnergyShowerLib::readFromROOTFile));
		ttreeVector.push_back(&(FCALDistEtaEnergyShowerLib::readFromROOTFile));
		ttreeVector.push_back(&(TestShowerLib::readFromROOTFile));

		return ttreeVector;
	}

    vectorStruct vectorStructFull()
    {
    	vectorStruct structVector;

		// List all Struct reader functions
		structVector.push_back(&(EtaEnergyShowerLib::createEmptyLib));
		structVector.push_back(&(FCALDistEnergyShowerLib::createEmptyLib));
		structVector.push_back(&(FCALDistEtaEnergyShowerLib::createEmptyLib));
		structVector.push_back(&(TestShowerLib::createEmptyLib));

		return structVector;
	}

    IShowerLib * iterateTTree(TFile *fname)
    {
    	IShowerLib* library = NULL;
    	vectorTTree ttreeVector = vectorTTreeFull();
		vectorTTree::const_iterator libiter;
		for (libiter = ttreeVector.begin(); libiter != ttreeVector.end(); libiter++) {
			library = (**libiter)(fname);
			if (library != NULL) break;
		}
		return library;
    }

    IShowerLib * iterateStruct(const std::string & fname)
    {
    	IShowerLib* library = NULL;
    	vectorStruct structVector = vectorStructFull();
		vectorStruct::const_iterator libiter;
		for (libiter = structVector.begin(); libiter != structVector.end(); libiter++) {
			library = (**libiter)(fname);
			if (library != NULL) break;
		}
		return library;
    }
}
