/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef NSWAGDDTool_H
#define NSWAGDDTool_H

#include "AGDDControl/AGDDToolBase.h"
#include "GaudiKernel/StatusCode.h"

namespace MuonGM
{
	class MuonDetectorManager;
}


class NSWAGDDTool: public AGDDToolBase
{
public:
	NSWAGDDTool(const std::string& type, const std::string& name, 
				 const IInterface* parent);
	virtual StatusCode construct();
	
	virtual StatusCode initialize();

private:
	

};

#endif
