/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
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
#include "xAODEgamma/Photon.h"

#include <array>
#include <boost/container/flat_set.hpp>
#include <sstream>
#include <algorithm>
#include <cctype>
#include <iterator>
#include <type_traits>
#include <limits>
#include <regex>

using CheckConfig = TrigGlobEffCorr::CheckConfig;
using Calculator = TrigGlobEffCorr::Calculator;
using ImportData = TrigGlobEffCorr::ImportData;
using Efficiencies = TrigGlobEffCorr::Efficiencies;
using Lepton = TrigGlobEffCorr::Lepton;


TrigGlobalEfficiencyCorrectionTool::TrigGlobalEfficiencyCorrectionTool(const std::string& name)
	: asg::AsgTool(name), m_trigMatchTool("", nullptr), m_checkElectronLegTag(false), m_checkMuonLegTag(false), m_seed(1), m_validTrigMatchTool(false),
	m_runNumberDecorator("RandomRunNumber"), m_calculator()
{
	declareProperty("ElectronEfficiencyTools", m_suppliedElectronEfficiencyTools, "electron MC efficiency tools (one for each kind of electron trigger leg)");
	declareProperty("ElectronScaleFactorTools", m_suppliedElectronScaleFactorTools, "electron scale factor tools (one for each kind of electron trigger leg)");
	declareProperty("PhotonEfficiencyTools", m_suppliedPhotonEfficiencyTools, "photon MC efficiency tools (one for each kind of photon trigger leg)");
	declareProperty("PhotonScaleFactorTools", m_suppliedPhotonScaleFactorTools, "photon scale factor tools (one for each kind of photon trigger leg)");	
	declareProperty("MuonTools", m_suppliedMuonTools, "muon efficiency/scale factor tool (one per year)");
	declareProperty("ListOfLegsPerTool", m_legsPerTool, "comma-separated list of trigger legs supported by each electron or photon tool");
	declareProperty("TriggerCombination", m_triggerCb, "map of trigger combination per period and/or range of runs");
	declareProperty("TriggerCombination2015", m_triggerCbPerYear["2015"] = "", "trigger combination \"trigger1 || trigger2 || ...\"");
	declareProperty("TriggerCombination2016", m_triggerCbPerYear["2016"] = "", "trigger combination \"trigger1 || trigger2 || ...\"");
	declareProperty("TriggerCombination2017", m_triggerCbPerYear["2017"] = "", "trigger combination \"trigger1 || trigger2 || ...\"");
	declareProperty("TriggerCombination2018", m_triggerCbPerYear["2018"] = "", "trigger combination \"trigger1 || trigger2 || ...\"");
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
	declareProperty("TriggerMatchingTool", m_trigMatchTool, "handle to an IMatchingTool instance");
	
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
		|| m_suppliedMuonTools.retrieve() != StatusCode::SUCCESS
		|| m_suppliedPhotonEfficiencyTools.retrieve() != StatusCode::SUCCESS
		|| m_suppliedPhotonScaleFactorTools.retrieve() != StatusCode::SUCCESS)
	{
		ATH_MSG_ERROR("Unable to retrieve CP tools");
		return StatusCode::FAILURE;
	}
	
	ATH_MSG_DEBUG("Retrieving trigger matching tool (if provided)");
	m_validTrigMatchTool = false;
	if(m_trigMatchTool.name() != "")
	{
		if(m_trigMatchTool.retrieve() != StatusCode::SUCCESS)
		{
			ATH_MSG_ERROR("Unable to retrieve trigger matching tool");
			return StatusCode::FAILURE;
		}
		ATH_MSG_DEBUG("Trigger matching support enabled");
		m_validTrigMatchTool = true;
	}
	
	ATH_MSG_DEBUG("Basic checks");
	CheckConfig checks(*this);
	if(!checks.basicConfigChecks()) return StatusCode::FAILURE;

	ATH_MSG_DEBUG("Enumerating tools");
	flat_set<std::size_t> collectedElectronTags, collectedMuonTags, collectedPhotonTags;
	if(!enumerateTools(data, m_suppliedElectronEfficiencyTools, m_electronEffToolIndex, collectedElectronTags)
		|| !enumerateTools(data, m_suppliedElectronScaleFactorTools, m_electronSfToolIndex, collectedElectronTags)
		|| !enumerateTools(data, m_suppliedMuonTools, m_muonToolIndex, collectedMuonTags)
		|| !enumerateTools(data, m_suppliedPhotonEfficiencyTools, m_photonEffToolIndex, collectedPhotonTags)
		|| !enumerateTools(data, m_suppliedPhotonScaleFactorTools, m_photonSfToolIndex, collectedPhotonTags))
	{
		return StatusCode::FAILURE;
	}
	
	ATH_MSG_DEBUG("Loading user-defined trigger combination"); 
	bool useDefaultElectronTools = (m_suppliedElectronEfficiencyTools.size()==1) && (m_suppliedElectronScaleFactorTools.size()==1) && (m_legsPerTool.size()==0);
	bool useDefaultPhotonTools = (m_suppliedPhotonEfficiencyTools.size()==1) && (m_suppliedPhotonScaleFactorTools.size()==1) && (m_legsPerTool.size()==0);
	if(!loadTriggerCombination(data, useDefaultElectronTools, useDefaultPhotonTools)) return StatusCode::FAILURE;
	
	ATH_MSG_DEBUG("Loading lepton selection tags decorators"); 
	if(!loadTagDecorators(collectedElectronTags, collectedMuonTags, collectedPhotonTags)) return StatusCode::FAILURE;

	ATH_MSG_DEBUG("Loading list of legs allowed for each tag"); 
	if(!loadListOfLegsPerTag(collectedElectronTags, collectedMuonTags, collectedPhotonTags)) return StatusCode::FAILURE;
	
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
bool TrigGlobalEfficiencyCorrectionTool::enumerateTools(ImportData& data, ToolHandleArray<CPTool>& suppliedTools, 
	std::map<ToolKey, std::size_t>& toolIndex, flat_set<std::size_t>& collectedTags)
{
	bool success = true;
	for(unsigned index=0;index<suppliedTools.size();++index)
	{
		auto& handle = suppliedTools[index];
		const std::string& name = handle.name(), altname = handle->name(); // athena: not always the same
		flat_set<ToolKey> listOfLegs;
		/// Find the legs associated to this tool ("ListOfLegsPerTool" property)
		if(suppliedTools.size()!=1 || m_legsPerTool.size()!=0)
		{
			auto itrLegs = m_legsPerTool.find(name);
			if(itrLegs == m_legsPerTool.end()) itrLegs = m_legsPerTool.find(altname);
			if(itrLegs != m_legsPerTool.end())
			{
				listOfLegs = parseListOfLegs(data, itrLegs->second, success);
				if(!success)
				{
					ATH_MSG_ERROR("The 'ListOfLegsPerTool' property has an invalid entry for the tool'" << name << "'");
					continue;
				}
			}
			else if(std::is_same<CPTool,IAsgElectronEfficiencyCorrectionTool>::value 
				|| std::is_same<CPTool,IAsgPhotonEfficiencyCorrectionTool>::value)
			{
				ATH_MSG_ERROR("Property 'ListOfLegsPerTool' empty for tool '" << name << "'");
				success = false;
				continue;
			}
			else
			{
				listOfLegs.emplace();
			}
		}
		else listOfLegs.emplace();
		/// Find the tags associated to this tool ("ListOfTagsPerTool" property)
		flat_set<std::size_t> tags;
		auto itrTags = m_tagsPerTool.find(name);
		if(itrTags == m_tagsPerTool.end()) itrTags = m_tagsPerTool.find(altname);
		if(itrTags != m_tagsPerTool.end())
		{
			success = success && parseTagString(itrTags->second, tags);
			collectedTags.insert(tags.begin(), tags.end());
		}
		else tags.emplace(0);

		/// Store
		unsigned short nUncheckedLegs = 0;
		for(auto& key : listOfLegs)
		{
			std::size_t leg = key.hash;
			if(leg)
			{
				auto flavour = data.associatedLeptonFlavour(leg, success);
				if(!(flavour==xAOD::Type::Electron && std::is_same<CPTool,IAsgElectronEfficiencyCorrectionTool>::value)
					&& !(flavour==xAOD::Type::Photon && std::is_same<CPTool,IAsgPhotonEfficiencyCorrectionTool>::value))
				{
					ATH_MSG_ERROR("Unexpected association of trigger leg '" << m_dictionary[leg] << "' to tool '" << name << "'");
					success = false;
				}
			}
			for(std::size_t tag : tags)
			{
				if(!toolIndex.emplace(ToolKey(leg, tag, key.boundaries), index).second)
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

auto TrigGlobalEfficiencyCorrectionTool::parseListOfLegs(ImportData& data, const std::string& inputList, bool& success) -> flat_set<ToolKey> 
{
	if(!inputList.length()) return {};
	std::regex rx("\\s*([[:alnum:]_]+)\\s*(?:\\[\\s*([^,\\[\\]]+)\\s*\\]\\s*)?(?:,|$)");
	flat_set<ToolKey> keys;
	std::smatch sm;
	auto itr=inputList.cbegin();
	do
	{
		if(sm.ready()) itr += sm.length();
		if(!std::regex_search(itr, inputList.cend(), sm ,rx) || sm.prefix().length())
		{
			ATH_MSG_ERROR("Invalid format for the property \"ListOfLegsPerTool\"");
			success = false;
			break;
		}
		std::size_t leg = m_hasher(sm[1].str());
		if(m_thresholds.find(leg) == m_thresholds.end())
		{
			ATH_MSG_ERROR("Unknown trigger leg '" << sm[1].str() << "' found in 'ListOfLegsPerTool'");
			success = false;
			continue;
		}
		ToolKey key(leg, 0);
		if(sm.length(2))
		{
			if(!data.getPeriodBoundaries(sm[2].str(), key.boundaries))
			{
				ATH_MSG_ERROR("Invalid period \"" << sm[2].str() << "\"found in the property \"ListOfLegsPerTool\"");
				success = false;
				continue;
			}
		}
		if(!keys.emplace(key).second)
		{
			ATH_MSG_ERROR("Trigger leg '" << sm[1].str() << "' mentioned several times with overlapping time periods in the property 'ListOfLegsPerTool'");
			success = false;
			continue;
		}
	} while(sm.suffix().length());
	return keys;
}

bool TrigGlobalEfficiencyCorrectionTool::loadTriggerCombination(ImportData& data, bool useDefaultElectronTools, bool useDefaultPhotonTools)
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
	std::set<std::size_t> allUniqueElectronLegs, allUniquePhotonLegs;
	for(auto& kv : m_triggerCb)
	{
		std::pair<unsigned, unsigned> boundaries;
		if(!data.getPeriodBoundaries(kv.first, boundaries))
		{
			success = false;
			continue;
		}
		std::size_t uniqueElectronLeg = !useDefaultElectronTools, uniquePhotonLeg = !useDefaultPhotonTools;
		if(!m_calculator->addPeriod(data, boundaries, kv.second, m_numberOfToys, uniqueElectronLeg, uniquePhotonLeg))
		{
			success = false;
			continue;
		}
		if(uniqueElectronLeg && useDefaultElectronTools) allUniqueElectronLegs.insert(uniqueElectronLeg);
		if(uniquePhotonLeg && useDefaultPhotonTools) allUniquePhotonLegs.insert(uniquePhotonLeg);
	}
	if(!success) return false;
	
	auto remapTools = [](auto& toolIndex, auto& allUniqueLegs) {
		typename std::remove_reference<decltype(toolIndex)>::type remappedToolIndex;
		for(std::size_t leg : allUniqueLegs)
		{
			if(!leg) continue;
			for(auto& kv : toolIndex)
			{
				/// tools have not been associated with legs so far so the key hash is only the tag
				const ToolKey& key = kv.first;
				remappedToolIndex.emplace(ToolKey(leg, key.hash, key.boundaries), kv.second);
			}
		}
		toolIndex.swap(remappedToolIndex);
	};
	
	if(useDefaultElectronTools && allUniqueElectronLegs.size())
	{
		remapTools(m_electronSfToolIndex, allUniqueElectronLegs);
		remapTools(m_electronEffToolIndex, allUniqueElectronLegs);
	}
	if(useDefaultPhotonTools && allUniquePhotonLegs.size())
	{
		remapTools(m_photonSfToolIndex, allUniquePhotonLegs);
		remapTools(m_photonEffToolIndex, allUniquePhotonLegs);
	}
	return success;
}

bool TrigGlobalEfficiencyCorrectionTool::loadTagDecorators(const flat_set<std::size_t>& collectedElectronTags,
	const flat_set<std::size_t>& collectedMuonTags, const flat_set<std::size_t>& collectedPhotonTags)
{
	bool success = true;
	
	flat_set<std::size_t> collectedTags(collectedElectronTags);
	collectedTags.insert(collectedMuonTags.begin(), collectedMuonTags.end());
	collectedTags.insert(collectedPhotonTags.begin(), collectedPhotonTags.end());
	flat_set<std::size_t> allTags = collectedTags;
	
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

bool TrigGlobalEfficiencyCorrectionTool::loadListOfLegsPerTag(const flat_set<std::size_t>& /*collectedElectronTags*/, const flat_set<std::size_t>& /*collectedMuonTags*/, const flat_set<std::size_t>& /*collectedPhotonTags*/)
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
			if(type==xAOD::Type::Electron && m_electronSfToolIndex.find(ToolKey(leg, tag))==m_electronSfToolIndex.end())
			{
				ATH_MSG_ERROR("No electron tool has been provided for trigger leg '" << m_dictionary[leg] << "' and selection tag " << kv.first
					<< " mentioned in the property 'ListOfLegsPerTag'");
				success = false;
			}
			else if(type==xAOD::Type::Muon && m_muonToolIndex.find(ToolKey(0, tag))==m_muonToolIndex.end())
			{
				ATH_MSG_ERROR("No muon tool has been provided for selection tag " << kv.first
					<< " mentioned in the property 'ListOfLegsPerTag'");
				success = false;
			}
			else if(type==xAOD::Type::Photon && m_photonSfToolIndex.find(ToolKey(leg, tag))==m_photonSfToolIndex.end())
			{
				ATH_MSG_ERROR("No photon tool has been provided for trigger leg '" << m_dictionary[leg] << "' and selection tag " << kv.first
					<< " mentioned in the property 'ListOfLegsPerTag'");
				success = false;
			}
			if(m_validLegTagPairs.insert(leg ^ tag).second)
			{
				if(type==xAOD::Type::Electron) m_checkElectronLegTag = true;
				if(type==xAOD::Type::Muon) m_checkMuonLegTag = true;
				if(type==xAOD::Type::Photon) m_checkPhotonLegTag = true;
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

template<class ParticleType>
bool TrigGlobalEfficiencyCorrectionTool::getEgammaTriggerLegEfficiencies(const ParticleType* p, unsigned runNumber, std::size_t leg, std::size_t tag, Efficiencies& efficiencies)
{
	/// Common implementation for electrons and photons
	auto ptype = []() { return std::is_same<ParticleType, xAOD::Electron>::value? "electron" : std::is_same<ParticleType, xAOD::Photon>::value? "photon" : "<unknown type>"; };
	auto pp = static_cast<const xAOD::IParticle*>(p);
	ATH_MSG_DEBUG("Retrieving efficiencies for " << ptype() << ' ' << p << " (pt=" << pp->pt() << ", eta=" << pp->eta() 
		<< ", tag='" << m_dictionary[tag] << "') for trigger leg " << m_dictionary[leg]);
	auto itrSf = GetScaleFactorToolIndex(p).find(ToolKey(leg, tag, runNumber));
	auto itrEff = GetEfficiencyToolIndex(p).find(ToolKey(leg, tag, runNumber));
	if(itrSf==GetScaleFactorToolIndex(p).end() || itrEff==GetEfficiencyToolIndex(p).end())
	{
		if(!tag) ATH_MSG_ERROR("Unable to find " << ptype() << " tools needed for trigger leg " << m_dictionary[leg] << " (run number = " << runNumber << ")");
		else ATH_MSG_ERROR("Unable to find " << ptype() << " tools needed for trigger leg " << m_dictionary[leg] << " and selection tag " << m_dictionary[tag] 
			<< " (run number = " << runNumber << ")");
		return false;
	}
	double sf;
	bool success = checkAndRecord(GetScaleFactorTool(p, itrSf->second).getEfficiencyScaleFactor(*p, sf))
		&& checkAndRecord(GetEfficiencyTool(p, itrEff->second).getEfficiencyScaleFactor(*p, efficiencies.mc()));
	efficiencies.data() = sf * efficiencies.mc();
	ATH_MSG_DEBUG("found for that " << ptype() << " eff(data) = " << efficiencies.data()<<" and eff(MC) = "<<efficiencies.mc());
	return success;
}

bool TrigGlobalEfficiencyCorrectionTool::getTriggerLegEfficiencies(const xAOD::Electron* p, unsigned runNumber, std::size_t leg, std::size_t tag, Efficiencies& efficiencies)
{
	return getEgammaTriggerLegEfficiencies(p, runNumber, leg, tag, efficiencies);
}

bool TrigGlobalEfficiencyCorrectionTool::getTriggerLegEfficiencies(const xAOD::Photon* p, unsigned runNumber, std::size_t leg, std::size_t tag, Efficiencies& efficiencies)
{
	return getEgammaTriggerLegEfficiencies(p, runNumber, leg, tag, efficiencies);
}

bool TrigGlobalEfficiencyCorrectionTool::getTriggerLegEfficiencies(const xAOD::Muon* p, std::size_t leg, std::size_t tag, Efficiencies& efficiencies)
{
	ATH_MSG_DEBUG("Retrieving efficiencies for muon " <<p << " (pt=" << p->pt() << ", eta=" << p->eta() 
		<< ", tag='" << m_dictionary[tag] << "') for trigger leg " << m_dictionary[leg]);
	auto itr = m_muonToolIndex.find(ToolKey(0, tag, 0));
	if(itr==m_muonToolIndex.end())
	{
		if(!tag) ATH_MSG_ERROR("Unable to find muon tool");
		else ATH_MSG_ERROR("Unable to find muon tool needed for selection tag " << m_dictionary[tag]);
		m_cpCode = CP::CorrectionCode::Error;
		return false;
	}
	auto& tool = *m_suppliedMuonTools[itr->second];
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
	bool success = checkAndRecord(tool.getTriggerEfficiency(*p, efficiencies.mc(), hltTrig, kFALSE))
		&& checkAndRecord(tool.getTriggerEfficiency(*p, efficiencies.data(), hltTrig, kTRUE));
	ATH_MSG_DEBUG("found for that muon eff(data) = " << efficiencies.data()<<" and eff(MC) = "<<efficiencies.mc());
	return success;
}

bool TrigGlobalEfficiencyCorrectionTool::retrieveRunNumber(unsigned& runNumber)
{
	runNumber = 0;
	auto eventInfo = evtStore()->retrieve<const xAOD::EventInfo>("EventInfo");
	if(!eventInfo)
	{
		ATH_MSG_ERROR("Can't retrieve 'EventInfo' from evtStore()");
		return false;
	}
	if(eventInfo->eventType(xAOD::EventInfo::IS_SIMULATION))
	{
		if(!m_runNumberDecorator.isAvailable(*eventInfo))
		{
			ATH_MSG_ERROR("Can't retrieve 'RandomRunNumber' from EventInfo");
			return false;
		}
		runNumber = m_runNumberDecorator(*eventInfo);
	}
	else runNumber = eventInfo->runNumber();
	return true;
}

bool TrigGlobalEfficiencyCorrectionTool::retrieveEventNumber(unsigned long& eventNumber)
{
	auto eventInfo = evtStore()->retrieve<const xAOD::EventInfo>("EventInfo");
	if(!eventInfo)
	{
		ATH_MSG_WARNING("Can't retrieve event number from evtStore()");
		eventNumber = 0;
		return false;
	}
	eventNumber = eventInfo->eventNumber();
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
		leptons.emplace_back(lep, tag);
	}
	return true;
}

CP::CorrectionCode TrigGlobalEfficiencyCorrectionTool::getEfficiencyScaleFactor(const std::vector<const xAOD::IParticle*>& leptons, double& efficiencyScaleFactor)
{
	unsigned runNumber;
	if(!retrieveRunNumber(runNumber)) return CP::CorrectionCode::Error;
	return getEfficiencyScaleFactor(runNumber, leptons, efficiencyScaleFactor);
}

CP::CorrectionCode TrigGlobalEfficiencyCorrectionTool::getEfficiency(const std::vector<const xAOD::IParticle*>& leptons, double& efficiencyData, double& efficiencyMc)
{
	unsigned runNumber;
	if(!retrieveRunNumber(runNumber)) return CP::CorrectionCode::Error;
	return getEfficiency(runNumber, leptons, efficiencyData, efficiencyMc);
}

CP::CorrectionCode TrigGlobalEfficiencyCorrectionTool::getEfficiencyScaleFactor(unsigned runNumber, const std::vector<const xAOD::IParticle*>& particles, double& efficiencyScaleFactor)
{
	m_cpCode = CP::CorrectionCode::Ok; /// ignores silently previous state + clears "checked" flag
	efficiencyScaleFactor = 1.;
	
	Efficiencies efficiencies;
	auto cc = getEfficiency(runNumber, particles, efficiencies.data(), efficiencies.mc());
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

CP::CorrectionCode TrigGlobalEfficiencyCorrectionTool::getEfficiency(unsigned runNumber, const std::vector<const xAOD::IParticle*>& particles, double& efficiencyData, double& efficiencyMc)
{
	m_cpCode = CP::CorrectionCode::Ok; /// ignores silently previous state + clears "checked" flag
	efficiencyData = 0.;
	efficiencyMc = 0.;
	
	ATH_MSG_DEBUG("Computing global trigger efficiency for this event with  " << particles.size() << " lepton(s) as input; run number = " << runNumber);
	
	if(runNumber<266904 || runNumber>364292)
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
	
	LeptonList leptons;
	updateLeptonList(leptons, particles);
	
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

CP::CorrectionCode TrigGlobalEfficiencyCorrectionTool::checkTriggerMatching(bool& matched, const std::vector<const xAOD::IParticle*>& particles)
{
	unsigned runNumber;
	if(!retrieveRunNumber(runNumber))
	{
		ATH_MSG_ERROR("Unable to retrieve run number, aborting checkTriggerMatching()");
		return CP::CorrectionCode::Error;
	}
	if(!m_validTrigMatchTool)
	{
		ATH_MSG_ERROR("A valid IMatchingTool instance should be provided via the property 'TriggerMatchingTool'");
		return CP::CorrectionCode::Error;
	}
	LeptonList leptons;
	updateLeptonList(leptons, particles);
	return m_calculator->checkTriggerMatching(*this, matched, leptons, runNumber)? CP::CorrectionCode::Ok : CP::CorrectionCode::Error;
}

CP::CorrectionCode TrigGlobalEfficiencyCorrectionTool::getRelevantTriggers(std::vector<std::string>& triggers)
{
	unsigned runNumber;
	if(!retrieveRunNumber(runNumber))
	{
		ATH_MSG_ERROR("Unable to retrieve run number, aborting getRelevantTriggers()");
		return CP::CorrectionCode::Error;
	}
	return m_calculator->getRelevantTriggersForUser(*this, triggers, runNumber)? CP::CorrectionCode::Ok : CP::CorrectionCode::Error;
}

CP::CorrectionCode TrigGlobalEfficiencyCorrectionTool::countTriggerLegs(const std::string& trigger, std::size_t& numberOfLegs)
{
	numberOfLegs = 0;
	/// this abuses a little the role of the dictionary to act as a cache: 
	/// for the key chosen as the trigger name hash XORed with the magic number,
	/// the mapped value is not the name of the trigger, but a meaningless string whose size equals the number of legs
	constexpr std::size_t magic = 0xa3bad03e613527c9;
	const std::size_t name = m_hasher(trigger), key = name^magic;
	std::string& value = m_dictionary[key];
	if(!value.length())
	{
		ImportData data;
		if(!data.importTriggers()) return CP::CorrectionCode::Error;
		auto itr = data.getTriggerDefs().find(name);
		if(itr == data.getTriggerDefs().end())
		{
			m_dictionary.erase(key);
			ATH_MSG_ERROR("The trigger " << trigger << " is not recognized");
			return CP::CorrectionCode::Error;
		}
		auto type = itr->second.type;
		using TrigGlobEffCorr::TriggerType;
		if(type & TriggerType::TT_SINGLELEPTON_FLAG) value = "~";
		else if(type & TriggerType::TT_DILEPTON_FLAG) value = "~~";
		else if(type & TriggerType::TT_TRILEPTON_FLAG) value = "~~~";
		else
		{
			m_dictionary.erase(key);
			ATH_MSG_ERROR("Unrecognized trigger type, implementation must be fixed!");
			return CP::CorrectionCode::Error;
		}
	}
	numberOfLegs = value.length();
	return CP::CorrectionCode::Ok;
}

bool TrigGlobalEfficiencyCorrectionTool::aboveThreshold(const Lepton& lepton, std::size_t leg) const
{
	bool decision = (lepton.pt() >= m_thresholds.at(leg));
	if(m_validLegTagPairs.size())
	{
		if((lepton.type()==xAOD::Type::Electron && m_checkElectronLegTag)
			|| (lepton.type()==xAOD::Type::Muon && m_checkMuonLegTag)
			|| (lepton.type()==xAOD::Type::Photon && m_checkPhotonLegTag))
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
	return leg1 ^ leg2; /// returns 0 if leg1 == leg2, which is the correct behaviour
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
	
	/// need not only to sort, but also to verify consistency for all pairs of legs (in case of configuration issue)
	/// for that, use a O(n^2) algorithm and count for each leg the number of legs tighter than itself
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
