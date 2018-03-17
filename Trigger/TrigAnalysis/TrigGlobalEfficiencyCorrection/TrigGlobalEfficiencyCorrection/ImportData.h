/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

// contact: jmaurer@cern.ch

#ifndef TRIGGLOBALEFFICIENCYCORRECTION_IMPORTDATA_H
#define TRIGGLOBALEFFICIENCYCORRECTION_IMPORTDATA_H 1

#include "TrigGlobalEfficiencyCorrection/TrigGlobalEfficiencyCorrectionTool.h"
#include "AsgTools/AsgMessaging.h"

#include <functional>
#include <algorithm>
#include <cctype>
#include <vector>
#include <map>
#include <array>

namespace TrigGlobEffCorr
{

enum TriggerType
{
	TT_UNKNOWN = -1,
	TT_GENERIC_SINGLELEPTON = 0x100,
	TT_SINGLE_E, TT_SINGLE_MU,
	TT_GENERIC_DILEPTON = 0x200,
	TT_2E_SYM, TT_2E_ASYM, TT_EMU, TT_2MU_SYM, TT_2MU_ASYM,
	TT_GENERIC_TRILEPTON = 0x400,
	TT_3E_SYM, TT_3E_HALFSYM, TT_3E_ASYM, TT_2E_MU_SYM, TT_2E_MU_ASYM, 
	TT_2MU_E_SYM, TT_2MU_E_ASYM, TT_3MU_SYM, TT_3MU_HALFSYM, TT_3MU_ASYM
	// to add more generic types (4L?) use 0x800, 0x1000, 0x2000 base...
};

class ImportData : public asg::AsgMessaging
{
	using Hierarchy = TrigGlobalEfficiencyCorrectionTool::Hierarchy;
	template<typename Key> using flat_set = boost::container::flat_set<Key>;
	
public:
	struct TrigDef
	{ 
		TriggerType type;
		std::array<std::size_t,3> leg;
		TrigDef(TriggerType type=TT_UNKNOWN, std::size_t leg0=0, std::size_t leg1=0, std::size_t leg2=0) : type(type), leg{leg0,leg1,leg2} {}
	};	

	ImportData();
	ImportData(TrigGlobalEfficiencyCorrectionTool& parent);
	~ImportData();
	
	bool importHierarchies();
	bool importTriggers();
	bool importThresholds(const std::map<std::string, std::string>& overridenThresholds = {});
	bool importPeriods();
	bool importMapKeys(const std::string& tag, std::map<std::size_t,std::map<std::size_t,int> >& keysPerLeg);
	// NB: the following function calls all import*() functions except importMapKeys()
	bool importAll(const std::map<std::string, std::string>& overridenThresholds = {});
	
	bool getPeriodBoundaries(const std::string& period, std::pair<unsigned,unsigned>& boundaries);
	bool suggestElectronMapKeys(const std::map<std::string,std::string>& triggerCombination,
		const std::string& version, std::map<std::string,std::string>& legsPerKey);
		
	const std::map<std::size_t,TrigDef>& getTriggerDefs() const { return m_triggerDefs; }
	const std::map<std::size_t,float>& getTriggerThresholds() const { return m_triggerThresholds; }
	const std::map<std::string, std::pair<unsigned,unsigned>>& getDataPeriods() const { return m_dataPeriods; }
	const std::vector<Hierarchy> getHierarchyMeta() const { return m_hierarchyMeta; }
	const std::vector<std::size_t> getHierarchyData() const { return m_hierarchyData; }
	const std::map<std::size_t,std::string>& getDictionary() const { return m_dictionary; }
	xAOD::Type::ObjectType associatedLeptonFlavour(std::size_t leg, bool& success);
	static xAOD::Type::ObjectType associatedLeptonFlavour(const std::string& leg, bool& success);
	std::vector<TrigDef> parseTriggerString(const std::string& triggerString, bool& success);
	TrigGlobalEfficiencyCorrectionTool* getParent() { return m_parent; }
	
protected:
	bool readDataFile(const char* filename, std::vector<std::string>& contents);
	
	TrigGlobalEfficiencyCorrectionTool* m_parent;
	std::map<std::size_t,std::string>& m_dictionary;
	std::hash<std::string>& m_hasher;
	
	std::map<std::size_t,TrigDef> m_triggerDefs;
	std::map<std::size_t,float> m_triggerThresholds;
	std::map<std::string, std::pair<unsigned,unsigned>> m_dataPeriods;
	std::vector<Hierarchy> m_hierarchyMeta;
	std::vector<std::size_t> m_hierarchyData;
};

inline std::string removeWhitespaces(const std::string& s)
{
	std::string t(s);
	t.erase(std::remove_if(t.begin(), t.end(),
			[](char c){ return std::isspace(c); }), t.end());
	return t;
}

}

#endif
