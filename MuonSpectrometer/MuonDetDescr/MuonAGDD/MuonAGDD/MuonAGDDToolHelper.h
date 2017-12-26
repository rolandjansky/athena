/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MuonAGDDToolHelper_H
#define MuonAGDDToolHelper_H

#include "GaudiKernel/StatusCode.h"

#include <vector>

class ITagInfoMgr;

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
	
	StatusCode SetTagInfo(std::string,std::string) const;
	
	std::vector<std::string>& ReadAGDDFlags();
	
	void SetNSWComponents();
	
	std::string GetAGDD(bool dumpIt);
	
	bool BuildMScomponents() const;
	bool BuildReadoutGeometry(MuonGM::MuonDetectorManager* mgr) const;

	std::string TagInfoKey() {return m_tagInfoKey;}

private:

	IRDBAccessSvc* p_RDBAccessSvc;
	IGeoModelSvc* p_GeoModelSvc;
	
	
	ITagInfoMgr* m_tagInfoMgr;
	std::string m_tagInfoKey;
	
};

#endif
