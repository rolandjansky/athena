// contact: jmaurer@cern.ch

#ifndef TRIGGLOBALEFFICIENCYCORRECTION_CHECKCONFIG_H
#define TRIGGLOBALEFFICIENCYCORRECTION_CHECKCONFIG_H 1

#include "TrigGlobalEfficiencyCorrection/TrigGlobalEfficiencyCorrectionTool.h"

#include <functional>
#include <vector>

namespace TrigGlobEffCorr
{
	
class CheckConfig
{
	template<typename Key> using flat_set = boost::container::flat_set<Key>;
	
public:
	CheckConfig(TrigGlobalEfficiencyCorrectionTool& parent) : m_parent(parent) {}
	
	bool basicConfigChecks();
	bool advancedConfigChecks();
	
private:
	TrigGlobalEfficiencyCorrectionTool& m_parent;

	template<class CPTool> static ToolHandle<CPTool>* findToolByName(ToolHandleArray<CPTool>& suppliedTools, const std::string& name);
	MsgStream& msg(MSG::Level lvl)
	{
		return m_parent.msg(lvl); 
	}
};

}

#endif