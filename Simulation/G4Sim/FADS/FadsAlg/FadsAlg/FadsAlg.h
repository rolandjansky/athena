/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FadsAlg_H
#define FadsAlg_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include <string>
class FadsRunManager;

/////////////////////////////////////////////////////////////////////////////

class FadsAlg:public AthAlgorithm {
public:
	FadsAlg (const std::string& name, ISvcLocator* pSvcLocator);
	StatusCode initialize();
	StatusCode execute();
	StatusCode finalize();
private:
	FadsRunManager* runManager;
	
	std::string m_detectorConstruction;
	std::string m_physicsList;
	std::string m_primaryGenerator;
	std::string m_runManager;
	std::string m_loadLibs;
	std::string m_steppingAction;
	std::string m_runAction;
	std::string m_eventAction;
	std::string m_trackingAction;
	std::string m_stackingAction;
};

#endif
