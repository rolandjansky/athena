/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef SHOWERLIBLIST_H_
#define SHOWERLIBLIST_H_

#include "LArG4ShowerLib/IShowerLib.h"
#include "LArG4ShowerLib/ShowerLibStatistics.h"
#include <string>

class TFile;

namespace ShowerLib {

	IShowerLib* iterateTTree (TFile* fname);
	IShowerLib* iterateStruct(const std::string& fname);

}

#endif /* SHOWERLIBLIST_H_ */
