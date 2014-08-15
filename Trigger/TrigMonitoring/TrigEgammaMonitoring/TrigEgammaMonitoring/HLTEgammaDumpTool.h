/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef HLTEGAMMADUMPTOOL_H
#define HLTEGAMMADUMPTOOL_H

// Simple description to come

#include "TrigHLTMonitoring/IHLTMonTool.h"

class HLTEgammaDumpTool : public IHLTMonTool {

public:
	// Constructor
	HLTEgammaDumpTool (const std::string & type, const std::string & name, const IInterface* parent);
	virtual ~HLTEgammaDumpTool();
	StatusCode init(){return StatusCode::SUCCESS;} // Empty for the moment.
	StatusCode book(); // called by bookHistograms
	StatusCode fill(); // called by fillHistograms
        StatusCode proc() { return StatusCode::SUCCESS; } // called by procHistograms

};

#endif
