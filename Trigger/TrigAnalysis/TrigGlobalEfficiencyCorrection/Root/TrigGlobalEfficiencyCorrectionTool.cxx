/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// contact: jmaurer@cern.ch

#include "TrigGlobalEfficiencyCorrection/TrigGlobalEfficiencyCorrectionTool.h"
#include "TrigGlobalEfficiencyCorrection/ImportData.h"
#include "TrigGlobalEfficiencyCorrection/CheckConfig.h"
#include "TrigGlobalEfficiencyCorrection/Calculator.h"
#include "TrigGlobalEfficiencyCorrection/Efficiencies.h"
#include "TrigGlobalEfficiencyCorrection/Lepton.h"
#include "xAODEventInfo/EventInfo.h"
#include "xAODEgamma/Electron.h"

#include <array>
#include <boost/container/flat_set.hpp>
#include <sstream>
#include <algorithm>
#include <cctype>
#include <iterator>
#include <type_traits>
#include <limits>

using CheckConfig = TrigGlobEffCorr::CheckConfig;
using Calculator = TrigGlobEffCorr::Calculator;
using ImportData = TrigGlobEffCorr::ImportData;
using Efficiencies = TrigGlobEffCorr::Efficiencies;
using Lepton = TrigGlobEffCorr::Lepton;


TrigGlobalEfficiencyCorrectionTool::TrigGlobalEfficiencyCorrectionTool(const std::string& name)
	: asg::AsgTool(name), m_checkElectronLegTag(false), m_checkMuonLegTag(false), m_seed(1), 
	m_runNumberDecorator("RandomRunNumber"), m_eventNumberDecorator("EventNumber"), m_calculator()
{
	declareProperty("ElectronEfficiencyTools", m_suppliedElectronEfficiencyTools, "electron MC efficiency tools (one for each kind of electron trigger leg)");
	declareProperty("ElectronScaleFactorTools", m_suppliedElectronScaleFactorTools, "electron scale factor tools (one for each kind of electron trigger leg)");
	declareProperty("MuonTools", m_suppliedMuonTools, "muon efficiency/scale factor tool (one per year)");
	declareProperty("ListOfLegsPerTool", m_legsPerTool, "comma-separated list of trigger legs supported by each electron tool");
	declareProperty("TriggerCombination", m_triggerCb, "map of trigger combination per period and/or range of runs");
	declareProperty("TriggerCombination2015", m_triggerCbPerYear["2015"] = "", "trigger combination \"trigger1 || trigger2 || ...\"");
	declareProperty("TriggerCombination2016", m_triggerCbPerYear["2016"] = "", "trigger combination \"trigger1 || trigger2 || ...\"");
	declareProperty("TriggerCombination2017", m_triggerCbPerYear["2017"] = "", "trigger combination \"trigger1 || trigger2 || ...\"");
	declareProperty("LeptonTagDecorations", m_leptonTagDecorations = "", 
		"comma-separated list of decorations for the lepton selection tags, ordered by increasing tightness. "
		"If a name ends with =, the tag is the decorated value, otherwise it is the decoration name");
	declareProperty("ListOfTagsPerTool", m_tagsPerTool, "comma-separated list of lepton selection tags associated to each tool");
	declareProperty("ElectronLegsPerTag", m_electronLegsPerTag, "DEPRECATED, use ListOfLegsPerTag instead");
	declareProperty("MuonLegsPerTag", m_muonLegsPerTag, "DEPRECATED, use ListOfLegsPerTag instead");
	declareProperty("ListOfLegsPerTag", m_legsPerTag, "map of allowed trigger legs for each tag");
	declareProperty("NumberOfToys", m_numberOfToys = 0, "if different from 0, use toy experiments instead of explicit formulas");
	declareProperty("OverrideThresholds", m_overrideThresholds, "new thresholds (in MeV) for the plateaux of the indicated trigger legs -- use at your own risk!");
	declareProperty("UseInternalSeed", m_useInternalSeed = false, "do not use event number as random number generation seed");
	
	m_cpCode.ignore();
}

TrigGlobalEfficiencyCorrectionTool::~TrigGlobalEfficiencyCorrectionTool()
{
	m_cpCode.ignore();
}

StatusCode TrigGlobalEfficiencyCorrectionTool::initialize()
{
	if(m_initialized)
	{
		ATH_MSG_ERROR("Tool has already been initialized");
		return StatusCode::FAILURE;
	}
	ATH_MSG_INFO ("Initializing " << name() << "...");
	
	m_dictionary.emplace(0, "");
	
	if(!processDeprecatedProperties()) return StatusCode::FAILURE;
	
	ATH_MSG_DEBUG("Importing data from configuration files");
	ImportData data(*this);
	if(!data.importAll(m_overrideThresholds)) return StatusCode::FAILURE;
	/// Copy the following variables from data as the latter is short-lived
	m_hierarchyMeta = data.getHierarchyMeta();
	m_hierarchyData = data.getHierarchyData();
	m_thresholds = data.getTriggerThresholds();
	
	ATH_MSG_DEBUG("Retrieving tools"); 
	if(m_suppliedElectronEfficiencyTools.retrieve() != StatusCode::SUCCESS
		|| m_suppliedElectronScaleFactorTools.retrieve() != StatusCode::SUCCESS
		|| m_suppliedMuonTools.retrieve() != StatusCode::SUCCESS)
	{
		ATH_MSG_ERROR("Unable to retrieve CP tools");
		return StatusCode::FAILURE;
	}
	
	ATH_MSG_DEBUG("Basic checks");
	CheckConfig checks(*this);
	if(!checks.basicConfigChecks()) return StatusCode::FAILURE;

	ATH_MSG_DEBUG("Enumerating tools");
	flat_set<std::size_t> collectedElectronTags, collectedMuonTags;
	if(!enumerateTools(m_suppliedElectronEfficiencyTools, m_electronEffTools, collectedElectronTags)
		|| !enumerateTools(m_suppliedElectronScaleFactorTools, m_electronSfTools, collectedElectronTags)
		|| !enumerateTools(m_suppliedMuonTools, m_muonTools, collectedMuonTags))
	{
		return StatusCode::FAILURE;
	}
	
	ATH_MSG_DEBUG("Loading user-defined trigger combination"); 
	bool useDefaultTools = (m_suppliedElectronEfficiencyTools.size()==1) && (m_suppliedElectronScaleFactorTools.size()==1) && (m_legsPerTool.size()==0);
	if(!loadTriggerCombination(data,useDefaultTools)) return StatusCode::FAILURE;
	
	ATH_MSG_DEBUG("Loading lepton selection tags decorators"); 
	if(!loadTagDecorators(collectedElectronTags,collectedMuonTags)) return StatusCode::FAILURE;

	ATH_MSG_DEBUG("Loading list of legs allowed for each tag"); 
	if(!loadListOfLegsPerTag(collectedElectronTags,collectedMuonTags)) return StatusCode::FAILURE;
	
	ATH_MSG_DEBUG("Advanced checks");
	if(!checks.advancedConfigChecks()) return StatusCode::FAILURE;
	
	ATH_MSG_INFO("Initialization successful"); 
	m_initialized = true;
	return StatusCode::SUCCESS;
}

bool TrigGlobalEfficiencyCorrectionTool::processDeprecatedProperties()
{
	if(m_electronLegsPerTag.size())
	{
		ATH_MSG_WARNING("The property 'ElectronLegsPerTag' is deprecated, please eventually use 'ListOfLegsPerTag' instead");
		for(auto& kv : m_electronLegsPerTag)
		{
			auto insert = m_legsPerTag.insert(kv);
			if(!insert.second) insert.first->second += "," + kv.second;
		}
	}
	if(m_muonLegsPerTag.size())
	{
		ATH_MSG_WARNING("The property 'MuonLegsPerTag' is deprecated, please eventually use 'ListOfLegsPerTag' instead");
		for(auto& kv : m_muonLegsPerTag)
		{
			auto insert = m_legsPerTag.insert(kv);
			if(!insert.second) insert.first->second += "," + kv.second;
		}
	}
	return true;
}

bool TrigGlobalEfficiencyCorrectionTool::parseTagString(const std::string& tagstring, flat_set<std::size_t>& allTags)
{
	bool success = true;
	const std::size_t star = m_hasher("*");
	for(std::size_t tag : listNonOrderedCSValues(tagstring,success))
	{
		allTags.insert((tag!=star)? tag : 0);
	}
	if(!success)
	{
		ATH_MSG_ERROR("List of tags \"" << tagstring << "\" is not provided in a valid format");
	}
	return success;
}

template<class CPTool>
bool TrigGlobalEfficiencyCorrectionTool::enumerateTools(ToolHandleArray<CPTool>& suppliedTools, 
	std::map<std::size_t, ToolHandle<CPTool>* >& indexedTools, flat_set<std::size_t>& collectedTags)
{
	bool success = true;
	for(auto& handle : suppliedTools)
	{
		const std::string& name = handle.name(), altname = handle->name(); // athena: not always the same
		flat_set<std::size_t> listOfLegs;
		/// Find the legs associated to this tool ("ListOfLegsPerTool" property)
		if(suppliedTools.size()!=1 || m_legsPerTool.size()!=0)
		{
			auto itrLegs = m_legsPerTool.find(name);
			if(itrLegs == m_legsPerTool.end())  itrLegs = m_legsPerTool.find(altname);
			if(itrLegs != m_legsPerTool.end())
			{
				listOfLegs = listNonOrderedCSValues(itrLegs->second,success);
				if(!success)
				{
					ATH_MSG_ERROR("The 'ListOfLegsPerTool' property has an invalid entry for the tool'" << name << "'");
					continue;
				}
			}
			else if(std::is_same<CPTool,IAsgElectronEfficiencyCorrectionTool>::value)
			{
				ATH_MSG_ERROR("Property 'ListOfLegsPerTool' empty for tool '" << name << "'");
				success = false;
				continue;
			}
			else
			{
				listOfLegs.insert(0);
			}
		}
		else listOfLegs.insert(0);
		/// Find the tags associated to this tool ("ListOfTagsPerTool" property)
		flat_set<std::size_t> tags;
		auto itrTags = m_tagsPerTool.find(name);
		if(itrTags == m_tagsPerTool.end()) itrTags = m_tagsPerTool.find(altname);
		if(itrTags != m_tagsPerTool.end())
		{
			success = success && parseTagString(itrTags->second,tags);
			collectedTags.insert(tags.begin(),tags.end());
		}
		else tags.emplace(0);

		/// Store
		unsigned short nUncheckedLegs = 0;
		for(std::size_t leg : listOfLegs)
		{
			if(leg && (m_thresholds.find(leg) == m_thresholds.end()))
			{
				ATH_MSG_ERROR("Unknown trigger leg '" << m_dictionary[leg] << "' found in 'ListOfLegsPerTool' for tool " << name);
				success = false;
				continue;
			}
			for(std::size_t tag : tags)
			{
				if(!indexedTools.emplace(leg ^ tag, &handle).second)
				{
					if(leg && tag) ATH_MSG_ERROR("Multiple tools associated to the selection tag '" <<  m_dictionary[tag] << "' for the trigger leg '" << m_dictionary[leg] << "'");
					else if(leg) ATH_MSG_ERROR("Multiple tools associated to the trigger leg '" <<  m_dictionary[leg] << "'");
					else if(tag) ATH_MSG_ERROR("Multiple tools associated to the selection tag '" <<  m_dictionary[tag] << "'");
					else ATH_MSG_ERROR("Multiple tools not associated to any trigger leg / selection tag");
					success  = false;
				}
			}
			++nUncheckedLegs;
		}
		if(!nUncheckedLegs)
		{
			ATH_MSG_ERROR("Tool " << name << " hasn't been associated to any trigger leg");
			success = false;
		}
	}
	return success;
}

bool TrigGlobalEfficiencyCorrectionTool::loadTriggerCombination(ImportData& data, bool useDefaultTools)
{
	bool success = true, mustBeEmpty = m_triggerCb.size();
	for(auto& kv : m_triggerCbPerYear)
	{
		if(!kv.second.size()) continue;
		if(mustBeEmpty)
		{
			ATH_MSG_ERROR ("You're not allowed to use simultaneously the 'TriggerCombination' and 'TriggerCombination" << kv.first << "' properties.");
			return false;
		}
		m_triggerCb.insert(kv);
	}
	
	m_calculator = std::unique_ptr<Calculator>(new Calculator(*this, m_triggerCb.size()));
	std::set<std::size_t> uniqueElectronLegs;
	for(auto& kv : m_triggerCb)
	{
		std::pair<unsigned, unsigned> boundaries;
		if(!data.getPeriodBoundaries(kv.first, boundaries))
		{
			success = false;
			continue;
		}
		std::size_t uniqueLeg = 0;
		if(!m_calculator->addPeriod(data, boundaries, kv.second, m_numberOfToys, useDefaultTools, uniqueLeg))
		{
			success = false;
			continue;
		}
		else if(uniqueLeg) uniqueElectronLegs.insert(uniqueLeg);
	}
	if(!success) return false;
	
	if(useDefaultTools && uniqueElectronLegs.size())
	{
		decltype(m_electronSfTools) remappedEffTools, remappedSfTools;
		for(std::size_t leg : uniqueElectronLegs)
		{
			if(!leg) continue;
			for(auto& kv : m_electronSfTools) remappedSfTools.emplace(kv.first^leg, kv.second);
			for(auto& kv : m_electronEffTools) remappedEffTools.emplace(kv.first^leg, kv.second);
		}
		m_electronSfTools.swap(remappedSfTools);
		m_electronEffTools.swap(remappedEffTools);
	}
	return success;
}

bool TrigGlobalEfficiencyCorrectionTool::loadTagDecorators(const flat_set<std::size_t>& collectedElectronTags, const flat_set<std::size_t>& collectedMuonTags)
{
	bool success = true;
	if(!m_muonTools.size() && !m_electronEffTools.size())
	{
		ATH_MSG_ERROR("Internal implementation error: enumerateTools() must be called first");
		return false;
	}
	
	flat_set<std::size_t> allTags(collectedElectronTags);
	allTags.insert(collectedMuonTags.begin(), collectedMuonTags.end());
	
	/// Initialize decorators
	/// Note: can't use listNonOrderedCSValues() as the order in which decorations are listed must be preserved
	std::stringstream ss(TrigGlobEffCorr::removeWhitespaces(m_leptonTagDecorations));
	std::string decoration;
	flat_set<std::size_t> allDecorations;
	while(std::getline(ss,decoration,','))
	{
		if(!decoration.length() || decoration=="?")
		{
			ATH_MSG_ERROR("Found an empty string in the 'LeptonTagDecorations' property");
			success = false;
			break;
		}
		
		bool found = false, suffixed = false;
		std::size_t h;
		if(decoration.back() != '?')
		{
			/// tag = decorator name (considered tagged if decorated value != 0)
			h = m_hasher(decoration);
			found = (allTags.find(h) != allTags.end());
		}
		else
		{
			/// tag = decorator name (without '?') + decorated value
			decoration.pop_back();
			suffixed = true;
			for(std::size_t tag : allTags) /// find a tag of the form <name><integer suffix>
			{
				auto& s = m_dictionary[tag];
				if(s.find(decoration) != 0) continue;
				if(std::all_of(s.begin() + decoration.length(),s.end(),[](char c){ return std::isdigit(c); }))
				{
					found = true;
					break;
				}
			}
			h = m_hasher(decoration);
		}
		if(!allDecorations.insert(h).second)	
		{
			ATH_MSG_ERROR("The selection tag '" << decoration << "' is listed twice in the property 'LeptonTagDecorations'");
			success = false;
			continue;
		}
		if(!found)
		{
			ATH_MSG_ERROR("the selection tag '" << decoration << "' is only referred to in the property 'LeptonTagDecorations'");
			success = false;
			continue;
		}
		m_leptonTagDecorators.emplace_back(decoration,h,suffixed);
		m_dictionary.emplace(h,decoration);
	}
	std::reverse(m_leptonTagDecorators.begin(),m_leptonTagDecorators.end());
	if(!success) return false;
	
	/// Check there's a suitable decorator for all tags associated to CP tools via ListOfTagsPerTool
	flat_set<std::size_t> collectedTags(collectedElectronTags);
	collectedTags.insert(collectedMuonTags.begin(), collectedMuonTags.end());
	for(std::size_t tag : collectedTags)
	{
		if(!tag) continue;
		auto itr = std::find_if(m_leptonTagDecorators.begin(),m_leptonTagDecorators.end(),
			[tag](const TagDecorator& ltd){ return (!ltd.suffixed) && (ltd.hash==tag); });
		if(itr != m_leptonTagDecorators.end()) continue; /// ok, there's a direct match
		bool found = false;
		auto& s= m_dictionary[tag];
		for(auto& ltd : m_leptonTagDecorators)
		{
			auto& name = m_dictionary[ltd.hash];
			if(s.find(name)!=0) continue;
			if(std::all_of(s.begin()+name.length(),s.end(),[](char c){ return std::isdigit(c); }))
			{
				found = true;
				break;
			}
		}
		if(!found)
		{
			ATH_MSG_ERROR("the selection tag '" << m_dictionary[tag] << "' hasn't been found in in 'LeptonTagDecorations'");
			success = false;
			continue;
		}
	}
	if(!success) return false;

	return success;
}

bool TrigGlobalEfficiencyCorrectionTool::loadListOfLegsPerTag(const flat_set<std::size_t>& /*collectedElectronTags*/, const flat_set<std::size_t>& /*collectedMuonTags*/)
{
	bool success = true;
	
	/// For safety...
	/*
	if(!m_muonLegsPerTag.size() && m_leptonTagDecorators.size())
	{
		ATH_MSG_INFO("Lepton selection tag decorators have been configured, but the property 'MuonLegsPerTag' is empty. "
			"Muons will be considered suitable for any trigger leg regardless of their tag.");
	}
	if(!m_electronLegsPerTag.size() && m_leptonTagDecorators.size())
	{
		ATH_MSG_INFO("Lepton selection tag decorators have been configured, but the property 'ElectronLegsPerTag' is empty. "
			"Electrons will be considered suitable for any trigger leg that has an associated SF tool for that tag.");
	}
	*/
	
	/// Create the hash list of legs allowed for each tag; check that a CP tool is available for each combination
	for(auto& kv : m_legsPerTag)
	{
		std::size_t tag;
		if(!kv.first.size() || kv.first=="*") tag = 0;
		else tag = m_hasher(kv.first);
		auto listOfLegs = listNonOrderedCSValues(kv.second,success);
		if(!success)
		{
			ATH_MSG_ERROR("The property 'ListOfLegsPerTag' has an invalid entry for the tag '" << kv.first << "'");
			break;
		}
		for(std::size_t leg : listOfLegs)
		{
			auto type = ImportData::associatedLeptonFlavour(m_dictionary[leg], success);
			if(type==xAOD::Type::Electron && m_electronSfTools.find(leg^tag)==m_electronSfTools.end())
			{
				ATH_MSG_ERROR("No electron tool has been provided for trigger leg '" << m_dictionary[leg] << "' and selection tag " << kv.first
					<< " mentioned in the property 'ListOfLegsPerTag'");
				success = false;
			}
			else if(type==xAOD::Type::Muon && m_muonTools.find(tag)==m_muonTools.end())
			{
				ATH_MSG_ERROR("No muon tool has been provided for trigger leg '" << m_dictionary[leg] << "' and selection tag " << kv.first
					<< " mentioned in the property 'ListOfLegsPerTag'");
				success = false;
			}
			if(m_validLegTagPairs.insert(leg ^ tag).second)
			{
				if(type==xAOD::Type::Electron) m_checkElectronLegTag = true;
				if(type==xAOD::Type::Muon) m_checkMuonLegTag = true;
			}
			else
			{
				ATH_MSG_ERROR("The combination of trigger leg '" << m_dictionary[leg] << "' and selection tag " << kv.first
					<< " is mentioned more than once in the property 'ListOfLegsPerTag'");
				success = false;
			}
		}
	}
	if(!success) return false;
	
	return success;
}

inline bool TrigGlobalEfficiencyCorrectionTool::checkAndRecord(CP::CorrectionCode&& cc)
{
   cc.setChecked();
   if(cc > m_cpCode) m_cpCode = cc;
   return cc==CP::CorrectionCode::Ok;
}

/// Parses the input string (comma-separated list) and returns a set of hashes
/// Watch out: since a flat_set is returned, the order of the entries in the input list is not preserved, as hinted by the name of the function
auto TrigGlobalEfficiencyCorrectionTool::listNonOrderedCSValues(const std::string& s, bool& success) -> flat_set<std::size_t>
{
	std::stringstream ss(TrigGlobEffCorr::removeWhitespaces(s));
	flat_set<std::size_t> hashes;
	std::string token;
	while(std::getline(ss, token, ','))
	{
		if(token.length())
		{
			std::size_t h = m_hasher(token);
			if(hashes.insert(h).second)
			{
				m_dictionary.emplace(h, std::move(token));
			}
			else
			{
				success = false;
				ATH_MSG_ERROR("Found duplicate entry while parsing comma-separated list '" << s << "'");
			}
		}
		else
		{
			success = false;
			ATH_MSG_ERROR("Found null-length entry while parsing comma-separated list '" << s << "'");
		}
	}
	if(!success) hashes.clear();
	return hashes;
}

bool TrigGlobalEfficiencyCorrectionTool::getTriggerLegEfficiencies(const xAOD::Electron* p, std::size_t leg, std::size_t tag, Efficiencies& efficiencies)
{
	auto pp = reinterpret_cast<const xAOD::IParticle*>(p);
	ATH_MSG_DEBUG("Retrieving efficiencies for electron " <<p << " (pt=" << pp->pt() << ",eta=" << pp->eta() 
		<< ",tag='" << m_dictionary[tag] << "') for trigger leg " << m_dictionary[leg]);
	auto itrSf = m_electronSfTools.find(leg^tag);
	auto itrEff = m_electronEffTools.find(leg^tag);
	if(itrSf==m_electronSfTools.end() || itrEff==m_electronEffTools.end())
	{
		if(!tag) ATH_MSG_ERROR("Unable to find electron tools needed for trigger leg " << m_dictionary[leg]);
		else ATH_MSG_ERROR("Unable to find electron tools needed for trigger leg " << m_dictionary[leg] << " and selection tag "<<m_dictionary[tag]);
		m_cpCode = CP::CorrectionCode::Error;
		return false;
	}
	auto& sfTool = *(itrSf->second);
	auto& mcTool = *(itrEff->second);
	double sf;
	bool success = checkAndRecord(sfTool->getEfficiencyScaleFactor(*p, sf));
	success = success && checkAndRecord(mcTool->getEfficiencyScaleFactor(*p, efficiencies.mc()));
	efficiencies.data() = sf * efficiencies.mc();
	ATH_MSG_DEBUG("found for that electron eff(data) = " << efficiencies.data()<<" and eff(MC) = "<<efficiencies.mc());
	return success;
}

bool TrigGlobalEfficiencyCorrectionTool::getTriggerLegEfficiencies(const xAOD::Muon* p, std::size_t leg, std::size_t tag, Efficiencies& efficiencies)
{
	ATH_MSG_DEBUG("Retrieving efficiencies for muon " <<p << " (pt=" << p->pt() << ",eta=" << p->eta() 
		<< ",tag='" << m_dictionary[tag] << "') for trigger leg " << m_dictionary[leg]);
	auto itr = m_muonTools.find(tag);
	if(itr==m_muonTools.end())
	{
		if(!tag) ATH_MSG_ERROR("Unable to find muon tool");
		else ATH_MSG_ERROR("Unable to find muon tool needed for selection tag " << m_dictionary[tag]);
		m_cpCode = CP::CorrectionCode::Error;
		return false;
	}
	auto& tool = *(itr->second);
	bool success = true;
	auto& hltTrig = m_dictionary[leg ^ 0xB0DDD56fF8E3250D];
	if(!hltTrig.size())
	{
		hltTrig = "HLT_" + m_dictionary[leg];
		while(true)
		{
			std::size_t i = hltTrig.find("_OR_m");
			if(i == std::string::npos) break;
			hltTrig.insert(i+4, "HLT_", 4);
		}
	}
	success = success && checkAndRecord(tool->getTriggerEfficiency(*p, efficiencies.mc(), hltTrig, kFALSE));
	success = success && checkAndRecord(tool->getTriggerEfficiency(*p, efficiencies.data(), hltTrig, kTRUE));
	ATH_MSG_DEBUG("found for that muon eff(data) = " << efficiencies.data()<<" and eff(MC) = "<<efficiencies.mc());
	return success;
}

bool TrigGlobalEfficiencyCorrectionTool::retrieveRunNumber(unsigned& runNumber)
{
	auto eventInfo = evtStore()->retrieve<const xAOD::EventInfo>("EventInfo");
	if(!eventInfo || !m_runNumberDecorator.isAvailable(*eventInfo))
	{
		ATH_MSG_ERROR("Can't retrieve run number from evtStore()");
		runNumber = 0;
		return false;
	}
	runNumber = m_runNumberDecorator(*eventInfo);
	return true;
}

bool TrigGlobalEfficiencyCorrectionTool::retrieveEventNumber(unsigned long& eventNumber)
{
	auto eventInfo = evtStore()->retrieve<const xAOD::EventInfo>("EventInfo");
	if(!eventInfo || !m_eventNumberDecorator.isAvailable(*eventInfo))
	{
		ATH_MSG_WARNING("Can't retrieve event number from evtStore()");
		eventNumber = 0;
		return false;
	}
	eventNumber = m_eventNumberDecorator(*eventInfo);
	return true;
}

template<class Particle>
bool TrigGlobalEfficiencyCorrectionTool::updateLeptonList(LeptonList& leptons, const std::vector<const Particle*>& particles)
{
	for(auto lep : particles)
	{
		std::size_t tag = 0;
		for(auto& ltd: m_leptonTagDecorators)
		{
			if(ltd.decorator.isAvailable(*lep))
			{
				char v = ltd.decorator(*lep);
				if(v)
				{
					if(ltd.suffixed) /// tag = <name>+<value>
					{
						std::string s = m_dictionary.at(ltd.hash) + std::to_string(v); 
						tag = m_hasher(s);
						m_dictionary.emplace(tag, s);
					}
					else tag = ltd.hash;
				}
			}
		}
		leptons.emplace_back(lep,tag);
	}
	return true;
}

CP::CorrectionCode TrigGlobalEfficiencyCorrectionTool::getEfficiencyScaleFactor(const std::vector<const xAOD::Electron*>& electrons, const std::vector<const xAOD::Muon*>& muons, double& efficiencyScaleFactor)
{
	unsigned runNumber;
	if(!retrieveRunNumber(runNumber)) return CP::CorrectionCode::Error;
	return getEfficiencyScaleFactor(runNumber, electrons, muons, efficiencyScaleFactor);
}

CP::CorrectionCode TrigGlobalEfficiencyCorrectionTool::getEfficiencyScaleFactor(unsigned runNumber, const std::vector<const xAOD::Electron*>& electrons, const std::vector<const xAOD::Muon*>& muons, double& efficiencyScaleFactor)
{
	LeptonList leptons;
	updateLeptonList(leptons, electrons);
	updateLeptonList(leptons, muons);
	return getEfficiencyScaleFactor(runNumber, leptons, efficiencyScaleFactor);
}

CP::CorrectionCode TrigGlobalEfficiencyCorrectionTool::getEfficiency(const std::vector<const xAOD::Electron*>& electrons, const std::vector<const xAOD::Muon*>& muons, double& efficiencyData, double& efficiencyMc)
{
	unsigned runNumber;
	if(!retrieveRunNumber(runNumber)) return CP::CorrectionCode::Error;
	return getEfficiency(runNumber, electrons, muons, efficiencyData, efficiencyMc);
}

CP::CorrectionCode TrigGlobalEfficiencyCorrectionTool::getEfficiency(unsigned runNumber, const std::vector<const xAOD::Electron*>& electrons, const std::vector<const xAOD::Muon*>& muons, double& efficiencyData, double& efficiencyMc)
{
	LeptonList leptons;
	updateLeptonList(leptons, electrons);
	updateLeptonList(leptons, muons);
	return getEfficiency(runNumber, leptons, efficiencyData, efficiencyMc);
}

CP::CorrectionCode TrigGlobalEfficiencyCorrectionTool::getEfficiencyScaleFactor(const std::vector<const xAOD::IParticle*>& leptons, double& efficiencyScaleFactor)
{
	unsigned runNumber;
	if(!retrieveRunNumber(runNumber)) return CP::CorrectionCode::Error;
	return getEfficiencyScaleFactor(runNumber, leptons, efficiencyScaleFactor);
}

CP::CorrectionCode TrigGlobalEfficiencyCorrectionTool::getEfficiencyScaleFactor(unsigned runNumber, const std::vector<const xAOD::IParticle*>& particles, double& efficiencyScaleFactor)
{
	LeptonList leptons;
	updateLeptonList(leptons, particles);
	return getEfficiencyScaleFactor(runNumber, leptons, efficiencyScaleFactor);
}

CP::CorrectionCode TrigGlobalEfficiencyCorrectionTool::getEfficiency(const std::vector<const xAOD::IParticle*>& leptons, double& efficiencyData, double& efficiencyMc)
{
	unsigned runNumber;
	if(!retrieveRunNumber(runNumber)) return CP::CorrectionCode::Error;
	return getEfficiency(runNumber, leptons, efficiencyData, efficiencyMc);
}

CP::CorrectionCode TrigGlobalEfficiencyCorrectionTool::getEfficiency(unsigned runNumber, const std::vector<const xAOD::IParticle*>& particles, double& efficiencyData, double& efficiencyMc)
{
	LeptonList leptons;
	updateLeptonList(leptons, particles);
	return getEfficiency(runNumber, leptons, efficiencyData, efficiencyMc);
}

CP::CorrectionCode TrigGlobalEfficiencyCorrectionTool::getEfficiencyScaleFactor(unsigned runNumber, const LeptonList& leptons, double& efficiencyScaleFactor)
{
	m_cpCode = CP::CorrectionCode::Ok; // ignores silently previous state + clears "checked" flag
	efficiencyScaleFactor = 1.;
	
	Efficiencies efficiencies;
	auto cc = getEfficiency(runNumber, leptons, efficiencies.data(), efficiencies.mc());
	if(cc == CP::CorrectionCode::Ok)
	{
		if(efficiencies.data()>0. && efficiencies.mc()>0.)
		{
			efficiencyScaleFactor = efficiencies.data() / efficiencies.mc();
			return CP::CorrectionCode::Ok;
		}
		else
		{
			efficiencyScaleFactor = 1.;
			ATH_MSG_WARNING("Efficiencies do not seem valid, forcing the scale factor to 1.");
			return CP::CorrectionCode::OutOfValidityRange;
		}
	}
	efficiencyScaleFactor = 1.;
	return cc;
}

CP::CorrectionCode TrigGlobalEfficiencyCorrectionTool::getEfficiency(unsigned runNumber, const LeptonList& leptons, double& efficiencyData, double& efficiencyMc)
{
	m_cpCode = CP::CorrectionCode::Ok; // ignores silently previous state + clears "checked" flag
	efficiencyData = 0.;
	efficiencyMc = 0.;
	
	ATH_MSG_DEBUG("Computing global trigger efficiency for this event with  " << leptons.size() << " lepton(s) as input");
	
	if(runNumber<266904 || runNumber>339205)
	{
		ATH_MSG_WARNING("Suspicious run number provided " << runNumber << ", continuing anyway");
	}
	
	#ifdef XAOD_STANDALONE
	if(!m_initialized)
	{
		ATH_MSG_WARNING("Tool hasn't been initialized, trying now");
		if(initialize() != StatusCode::SUCCESS)
		{
			ATH_MSG_ERROR("Giving up.");
			m_cpCode.ignore();
			return CP::CorrectionCode::Error;
		}
	}
	#endif
	
	Efficiencies efficiencies;
	if(m_calculator->compute(*this, leptons, runNumber, efficiencies))
	{
		efficiencyData = efficiencies.data();
		efficiencyMc = efficiencies.mc();
		if(efficiencies.data()<=0. || efficiencies.mc()<=0.)
		{
			ATH_MSG_WARNING("Efficiencies do not seem valid");
			m_cpCode.ignore();
			m_cpCode = CP::CorrectionCode::OutOfValidityRange;
		}
	}
	else
	{
		m_cpCode.ignore();
		m_cpCode = CP::CorrectionCode::Error;
	}
	return m_cpCode;
}

bool TrigGlobalEfficiencyCorrectionTool::aboveThreshold(const Lepton& lepton,std::size_t leg) const
{
	bool decision = (lepton.pt() >= m_thresholds.at(leg));
	if(m_validLegTagPairs.size())
	{
		if((lepton.type()==xAOD::Type::Electron && m_checkElectronLegTag)
			|| (lepton.type()==xAOD::Type::Muon && m_checkMuonLegTag))
		{
			decision = decision && (m_validLegTagPairs.find(leg ^ lepton.tag()) != m_validLegTagPairs.end());
		}
	}
	ATH_MSG_DEBUG("Lepton " << lepton.particle() << " (pt=" << lepton.pt() << ") is " 
		<< (decision?"":"not ") << "considered suitable for firing trigger leg " << m_dictionary.at(leg));
	return decision;
}

std::size_t TrigGlobalEfficiencyCorrectionTool::getCombinedHash(const flat_set<std::size_t>& legs)
{
	if(legs.size() < 2) return 0;
	std::size_t combinedHash = 0;
	for(auto& leg : legs) combinedHash ^= leg;
	return combinedHash;
}

std::size_t TrigGlobalEfficiencyCorrectionTool::getCombinedHash(std::size_t leg1, std::size_t leg2)
{
	return leg1 ^ leg2; // returns 0 if leg1 == leg2, which is the correct behaviour
}

inline constexpr auto TrigGlobalEfficiencyCorrectionTool::forwardLegs(const flat_set<std::size_t>& legs) -> const flat_set<std::size_t>&
{
	return legs;
}

inline constexpr std::array<std::size_t,2> TrigGlobalEfficiencyCorrectionTool::forwardLegs(std::size_t leg1, std::size_t leg2)
{
	if(leg1<leg2) return {leg1, leg2};
	else return {leg2, leg1};
}

template<class... ListOfLegs>
unsigned long TrigGlobalEfficiencyCorrectionTool::getCachedTriggerLegsRanking(const Lepton& lepton, ListOfLegs... legs)
{
	auto combinedHash = getCombinedHash(legs...);
	if(!combinedHash) return 0xFEDCBA9876543210; // only one (distinct) leg
	auto cachedRankings = m_cachedLegRankings.equal_range(combinedHash);
	auto rankingItr = cachedRankings.first;
	float pt = lepton.pt();
	while(rankingItr!=cachedRankings.second && (pt<rankingItr->second.minPt || pt>=rankingItr->second.maxPt)) ++rankingItr;
	if(rankingItr == cachedRankings.second)
	{
		CachedRanking r = rankTriggerLegs(pt, forwardLegs(legs...));
		if(r) m_cachedLegRankings.emplace(combinedHash, r);
		return r.ranking;
	}
	return rankingItr->second.ranking;
}

template<class Container>
auto TrigGlobalEfficiencyCorrectionTool::rankTriggerLegs(float pt, const Container& legs) -> CachedRanking
{
	const std::size_t nLegs = legs.size();
	CachedRanking r;
	r.ranking = std::numeric_limits<decltype(r.ranking)>::max();
	r.minPt = 0.f;
	r.maxPt = 1e12f;
	if(nLegs >= 2*sizeof(r.ranking))
	{
		ATH_MSG_ERROR("Implementation currently doesn't support ranking of more than " << 2*sizeof(r.ranking) << " trigger legs");
		return r;
	}
	std::vector<uint8_t> counts(nLegs);
	
	// need not only to sort, but also to verify consistency for all pairs of legs (in case of configuration issue)
	// for that, use a O(n^2) algorithm and count for each leg the number of legs tighter than itself
	auto legI = legs.begin();
	for(unsigned i=0;i<nLegs;++i)
	{
		auto legJ = legI;
		for(unsigned j=i+1;j<nLegs;++j)
		{
			++legJ;
			bool found = false;
			for(auto& meta : m_hierarchyMeta)
			{
				if(pt<meta.minPt || pt>=meta.maxPt) continue;
				auto data = m_hierarchyData.begin() + meta.offset;
				auto end = data + meta.nLegs;
				auto a = std::find(data,end,*legI);
				if(a==end) continue;
				auto b = std::find(data,end,*legJ);
				if(b==end) continue;
				r.minPt = std::max(r.minPt, meta.minPt);
				r.maxPt = std::min(r.maxPt, meta.maxPt);
				++(counts[(a>b)? i : j]);
				found = true;
				break;
			}
			if(!found)
			{
				/// future: might search recursively whether some order can be found via transitivity through an intermediate leg
				ATH_MSG_ERROR("Unable to rank trigger legs " << m_dictionary[*legI] << " and " << m_dictionary[*legJ]);
				return r;
			}
		}
		++legI;
	}
	decltype(r.ranking) ranking = 0;
	for(unsigned i=0;i<nLegs;++i)
	{
		unsigned char index = std::find(counts.begin(), counts.end(), i) - counts.begin();
		if(index >= nLegs)
		{
			ATH_MSG_ERROR("Inconsistency found while trying to rank " << nLegs << " trigger legs");
			return r;
		}
		ranking = (ranking<<4 | index);
	}
	r.ranking = ranking;
	return r;
}

std::size_t TrigGlobalEfficiencyCorrectionTool::getLoosestLeg(const Lepton& lepton, std::size_t leg1, std::size_t leg2, bool& success)
{
	auto ranking = getCachedTriggerLegsRanking(lepton, leg1, leg2);
	if(CachedRanking::invalid(ranking))
	{
		success = false;
		return -1;
	}
	return forwardLegs(leg1, leg2)[ranking&0xF];
}

std::pair<std::size_t,std::size_t> TrigGlobalEfficiencyCorrectionTool::getTwoLoosestLegs(const Lepton& lepton, const flat_set<std::size_t>& legs, bool& success)
{
	auto ranking = getCachedTriggerLegsRanking(lepton, legs);
	if(CachedRanking::invalid(ranking))
	{
		success = false;
		return {0, 0};
	}
	std::pair<std::size_t,std::size_t> looseLegs{0, 0};
	looseLegs.first = *legs.nth(ranking&0xF);
	if(legs.size() >= 2) looseLegs.second = *legs.nth((ranking>>4)&0xF);
	return looseLegs;
}

std::size_t TrigGlobalEfficiencyCorrectionTool::getLoosestLegAboveThreshold(const Lepton& lepton, const flat_set<std::size_t>& legs, bool& success)
{
	flat_set<std::size_t> validLegs;
	for(auto leg : legs)
		if(aboveThreshold(lepton,leg)) validLegs.insert(leg);
	if(validLegs.size()==0) return 0;
	if(validLegs.size()==1) return *validLegs.begin();
	auto ranking = getCachedTriggerLegsRanking(lepton, legs);
	if(CachedRanking::invalid(ranking))
	{
		success = false;
		return 0;
	}
	return *legs.nth(ranking&0xF);
}

std::vector<std::size_t> TrigGlobalEfficiencyCorrectionTool::getSortedLegs(const Lepton& lepton, const flat_set<std::size_t>& legs, bool& success)
{
	const int nLegs = legs.size();
	unsigned long ranking = getCachedTriggerLegsRanking(lepton, legs);
	if(CachedRanking::invalid(ranking))
	{
		success = false;
		ranking = 0xFEDCBA9876543210;
	}
	std::vector<std::size_t> sorted_legs(nLegs);
	for(int i=0; i<nLegs; ++i)
	{
		sorted_legs[i] = *legs.nth(ranking&0xF);
		ranking >>= 4;
	}
	return sorted_legs;
}

CP::CorrectionCode TrigGlobalEfficiencyCorrectionTool::suggestElectronMapKeys(const std::map<std::string,std::string>& triggerCombination, 
	const std::string& version, std::map<std::string,std::string>& legsPerKey)
{
	ImportData data;
	bool success = data.suggestElectronMapKeys(triggerCombination, version, legsPerKey);
	return success? CP::CorrectionCode::Ok : CP::CorrectionCode::Error;
}

bool TrigGlobalEfficiencyCorrectionTool::isAffectedBySystematic(const CP::SystematicVariation& systematic) const
{
	auto sys = affectingSystematics();
	return !systematic.empty() && sys.find(systematic)!=sys.end();
}

CP::SystematicSet TrigGlobalEfficiencyCorrectionTool::affectingSystematics() const
{
	return CP::SystematicSet();
}

CP::SystematicSet TrigGlobalEfficiencyCorrectionTool::recommendedSystematics() const
{
	return {};
}

CP::SystematicCode TrigGlobalEfficiencyCorrectionTool::applySystematicVariation(const CP::SystematicSet&)
{
	return CP::SystematicCode::Ok;
}