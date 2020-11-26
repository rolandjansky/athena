/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

// contact: jmaurer@cern.ch

#ifndef TRIGGLOBALEFFICIENCYCORRECTION_IMPORTDATA_H
#define TRIGGLOBALEFFICIENCYCORRECTION_IMPORTDATA_H 1

#include "TrigGlobalEfficiencyCorrection/TrigGlobalEfficiencyCorrectionTool.h"
#include "AsgMessaging/AsgMessaging.h"
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
	TT_UNKNOWN = 0x0,
	TT_X2Y_FLAG = 0x1, // used to distinguish X_2Y from 2X_Y triggers
	TT_ELECTRON_FLAG = 0x10,
	TT_MUON_FLAG = 0x20,
	TT_PHOTON_FLAG = 0x40,
	TT_MASK_FLAVOUR = TT_ELECTRON_FLAG | TT_MUON_FLAG | TT_PHOTON_FLAG,
	TT_SYM = 0x100,
	TT_HALFSYM = 0x200,
	TT_ASYM = 0x300,
	TT_MASK_SYMMETRY = TT_SYM | TT_HALFSYM | TT_ASYM,
	TT_MASK_TYPE = ~TT_MASK_FLAVOUR,
	// single lepton triggers
	TT_SINGLELEPTON_FLAG = 0x1000,
	TT_SINGLE_E = TT_SINGLELEPTON_FLAG| TT_ELECTRON_FLAG,
	TT_SINGLE_MU = TT_SINGLELEPTON_FLAG | TT_MUON_FLAG,
	TT_SINGLE_G = TT_SINGLELEPTON_FLAG | TT_PHOTON_FLAG,
	// dilepton triggers
	TT_DILEPTON_FLAG = 0x2000,
	TT_DILEPTON_SYM = TT_DILEPTON_FLAG | TT_SYM,
	TT_DILEPTON_ASYM = TT_DILEPTON_FLAG | TT_ASYM,
	TT_2E_SYM = TT_DILEPTON_SYM | TT_ELECTRON_FLAG,
	TT_2E_ASYM = TT_DILEPTON_ASYM | TT_ELECTRON_FLAG,
	TT_2MU_SYM = TT_DILEPTON_SYM | TT_MUON_FLAG,
	TT_2MU_ASYM = TT_DILEPTON_ASYM | TT_MUON_FLAG,
	TT_E_MU = TT_DILEPTON_FLAG | TT_ELECTRON_FLAG | TT_MUON_FLAG,
	TT_2G_SYM = TT_DILEPTON_SYM | TT_PHOTON_FLAG,
	TT_2G_ASYM = TT_DILEPTON_ASYM | TT_PHOTON_FLAG,
	TT_E_G = TT_DILEPTON_FLAG | TT_ELECTRON_FLAG | TT_PHOTON_FLAG,
	TT_MU_G = TT_DILEPTON_FLAG | TT_MUON_FLAG | TT_PHOTON_FLAG,
	// trilepton triggers
	TT_TRILEPTON_FLAG = 0x4000,
	TT_TRILEPTON_SYM = TT_TRILEPTON_FLAG | TT_SYM,
	TT_TRILEPTON_HALFSYM = TT_TRILEPTON_FLAG | TT_HALFSYM,
	TT_TRILEPTON_ASYM = TT_TRILEPTON_FLAG | TT_ASYM,
	TT_3E_SYM = TT_TRILEPTON_SYM | TT_ELECTRON_FLAG,
	TT_3E_HALFSYM = TT_TRILEPTON_HALFSYM | TT_ELECTRON_FLAG,
	TT_3E_ASYM = TT_TRILEPTON_ASYM | TT_ELECTRON_FLAG,
	TT_3MU_SYM = TT_TRILEPTON_SYM | TT_MUON_FLAG,
	TT_3MU_HALFSYM = TT_TRILEPTON_HALFSYM | TT_MUON_FLAG,
	TT_3MU_ASYM = TT_TRILEPTON_ASYM | TT_MUON_FLAG,
	TT_2E_MU_SYM = TT_TRILEPTON_SYM | TT_ELECTRON_FLAG | TT_MUON_FLAG,
	TT_E_2MU_SYM = TT_2E_MU_SYM | TT_X2Y_FLAG,
	TT_2E_MU_ASYM = TT_TRILEPTON_ASYM | TT_ELECTRON_FLAG | TT_MUON_FLAG,
	TT_E_2MU_ASYM = TT_2E_MU_ASYM | TT_X2Y_FLAG,
	TT_3G_SYM = TT_TRILEPTON_SYM | TT_PHOTON_FLAG,
	TT_3G_HALFSYM = TT_TRILEPTON_HALFSYM | TT_PHOTON_FLAG,
	TT_3G_ASYM = TT_TRILEPTON_ASYM | TT_PHOTON_FLAG,
	TT_2E_G_SYM = TT_TRILEPTON_SYM | TT_ELECTRON_FLAG | TT_PHOTON_FLAG,
	TT_E_2G_SYM = TT_2E_G_SYM | TT_X2Y_FLAG,
	TT_2E_G_ASYM = TT_TRILEPTON_ASYM | TT_ELECTRON_FLAG | TT_PHOTON_FLAG,
	TT_E_2G_ASYM = TT_2E_G_ASYM | TT_X2Y_FLAG,
	TT_2MU_G_SYM = TT_TRILEPTON_SYM | TT_MUON_FLAG | TT_PHOTON_FLAG,
	TT_MU_2G_SYM = TT_2MU_G_SYM | TT_X2Y_FLAG,
	TT_2MU_G_ASYM = TT_TRILEPTON_ASYM | TT_MUON_FLAG | TT_PHOTON_FLAG,
	TT_MU_2G_ASYM = TT_2MU_G_ASYM | TT_X2Y_FLAG,
};

class ImportData : public asg::AsgMessaging
{
	using Hierarchy = TrigGlobalEfficiencyCorrectionTool::Hierarchy;
	
public:
	struct TrigDef
	{ 
		TriggerType type;
		std::size_t name;
		std::array<std::size_t,3> leg;
		bool used = false; /// auxiliary variable used by Calculator::Helper::extra() and bindFunction()
		TrigDef(TriggerType type=TT_UNKNOWN, std::size_t name=0, std::size_t leg0=0, std::size_t leg1=0, std::size_t leg2=0) : type(type), name(name), leg{leg0,leg1,leg2} {}
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
	bool adaptTriggerListForTriggerMatching(std::vector<ImportData::TrigDef>& triggers);
	
protected:
	bool readDataFile(const char* filename, std::vector<std::string>& contents);
	void setNonMixed3LType(TrigDef& def, TriggerType flavourFlag);
	
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
