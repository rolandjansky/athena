/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONAGDD_MUONAGDDTOOLHELPER_H
#define MUONAGDD_MUONAGDDTOOLHELPER_H

#include "GaudiKernel/StatusCode.h"

#include <vector>

namespace MuonGM 
{
	class MuonDetectorManager;
}
class IRDBAccessSvc;
class IGeoModelSvc;

class MuonAGDDToolHelper
{
public:
	MuonAGDDToolHelper();
	
	std::vector<std::string>& ReadAGDDFlags();
	
	void SetNSWComponents();
	
	std::string GetAGDD(bool dumpIt, std::string tableName);
	
	bool BuildMScomponents() const;
	bool BuildReadoutGeometry(MuonGM::MuonDetectorManager* mgr) const;

private:

	IRDBAccessSvc* p_RDBAccessSvc;
	IGeoModelSvc* p_GeoModelSvc;
	
};

#endif
