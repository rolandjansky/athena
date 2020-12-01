/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

// contact: jmaurer@cern.ch

#ifndef TRIGGLOBALEFFICIENCYCORRECTION_CHECKCONFIG_H
#define TRIGGLOBALEFFICIENCYCORRECTION_CHECKCONFIG_H 1

#include "TrigGlobalEfficiencyCorrection/TrigGlobalEfficiencyCorrectionTool.h"
#include "AsgMessaging/AsgMessaging.h"

#include <functional>
#include <vector>

namespace TrigGlobEffCorr
{
	
class CheckConfig : public asg::AsgMessaging
{	
public:
	CheckConfig(TrigGlobalEfficiencyCorrectionTool& parent);
	
	bool basicConfigChecks();
	bool advancedConfigChecks();
	
private:
	TrigGlobalEfficiencyCorrectionTool& m_parent;

	template<class CPTool> static ToolHandle<CPTool>* findToolByName(ToolHandleArray<CPTool>& suppliedTools, const std::string& name);
};

}

#endif
