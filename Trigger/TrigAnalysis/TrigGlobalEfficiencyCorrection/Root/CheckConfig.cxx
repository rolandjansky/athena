/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

// contact: jmaurer@cern.ch

#include "TrigGlobalEfficiencyCorrection/CheckConfig.h"
#include "TrigGlobalEfficiencyCorrection/ImportData.h"
#include "TrigGlobalEfficiencyCorrection/Calculator.h"
#include <boost/container/flat_set.hpp>
template<typename Key> using flat_set = boost::container::flat_set<Key>;

#include <cctype>

using CheckConfig = TrigGlobEffCorr::CheckConfig;
using ImportData = TrigGlobEffCorr::ImportData;


CheckConfig::CheckConfig(TrigGlobalEfficiencyCorrectionTool& parent) :
	asg::AsgMessaging(&parent), m_parent(parent)
{
	msg().setLevel(parent.msg().level());
}


template<class CPTool>
ToolHandle<CPTool>* CheckConfig::findToolByName(ToolHandleArray<CPTool>& suppliedTools, const std::string& name)
{
	for(auto& tool : suppliedTools)
	{
		if(tool.name()==name || tool->name()==name) // athena: not always the same
		{
			return &tool;
		}
	}
	return nullptr;
}

bool CheckConfig::basicConfigChecks()
{
	bool success = true;
	
	if(m_parent.m_suppliedElectronEfficiencyTools.size() != m_parent.m_suppliedElectronScaleFactorTools.size())
	{
		ATH_MSG_ERROR("The numbers of electron tools supplied via the 'ElectronEfficiencyTools' and 'ElectronScaleFactorTools' properties should be identical");
		return false;
	}
	
	if(m_parent.m_suppliedPhotonEfficiencyTools.size() != m_parent.m_suppliedPhotonScaleFactorTools.size())
	{
		ATH_MSG_ERROR("The numbers of photon tools supplied via the 'PhotonEfficiencyTools' and 'PhotonScaleFactorTools' properties should be identical");
		return false;
	}
	
	/// All tools mentioned in 'ListOfLegsPerTool' must be in 'ElectronEfficiencyTools' or 'ElectronScaleFactorTools' (or equivalent photon tools)
	for(auto& kv : m_parent.m_legsPerTool)
	{
		auto& name = kv.first;
		if(findToolByName(m_parent.m_suppliedElectronEfficiencyTools, name)
			|| findToolByName(m_parent.m_suppliedElectronScaleFactorTools, name)
			|| findToolByName(m_parent.m_suppliedPhotonEfficiencyTools, name)
			|| findToolByName(m_parent.m_suppliedPhotonScaleFactorTools, name)
			) continue;
		success = false;
		if(findToolByName(m_parent.m_suppliedMuonTools, name))
		{
			ATH_MSG_ERROR("Muon tool " << name << " mentioned in property 'ListOfLegsPerTool', which is only aimed at electron and photon tools");
		}
		else
		{
			std::string known_tools = "; the known tools are";
			for(auto& tool : m_parent.m_suppliedElectronEfficiencyTools) known_tools += " " + tool.name();
			for(auto& tool : m_parent.m_suppliedElectronScaleFactorTools) known_tools += " " + tool.name();
			for(auto& tool : m_parent.m_suppliedPhotonEfficiencyTools) known_tools += " " + tool.name();
			for(auto& tool : m_parent.m_suppliedPhotonScaleFactorTools) known_tools += " " + tool.name();
			ATH_MSG_ERROR("Unknown tool " << name << " mentioned in property 'ListOfLegsPerTool'" << known_tools);
		}
	}
	if(!success) return false;
	
	/// All electron/photon tools must be associated to trigger legs (except when there's only one tool)
	auto toolsHaveLegInfo = [this](auto& effTools, auto& sfTools, const char* type)
	{
		if(effTools.size() < 2) return true;
		bool success = true;
		for(int i=0;i<2;++i)
		{
			for(auto& tool : (i? effTools: sfTools))
			{
				const std::string& name = tool.name();
				if(m_parent.m_legsPerTool.find(name) == m_parent.m_legsPerTool.end())
				{
					ATH_MSG_ERROR(type << " tool " << name << " associated trigger legs are not indicated in 'ListOfLegsPerTool', "
						"doing so is mandatory when several tools are used");
					success = false;
				}
			}
		}
		return success;
	};
	success &= toolsHaveLegInfo(m_parent.m_suppliedElectronEfficiencyTools, m_parent.m_suppliedElectronScaleFactorTools, "Electron");
	success &= toolsHaveLegInfo(m_parent.m_suppliedPhotonEfficiencyTools, m_parent.m_suppliedPhotonScaleFactorTools, "Photon");
	if(!success) return false;
	
	/// 
	/// Additional checks when tags are used
	///
	
	if(m_parent.m_leptonTagDecorations == "")
	{
		if(m_parent.m_muonLegsPerTag.size() || m_parent.m_electronLegsPerTag.size()
			|| m_parent.m_legsPerTag.size() || m_parent.m_tagsPerTool.size())
		{
			ATH_MSG_ERROR("the property 'LeptonTagDecorations' must be filled when any of 'ListOfTagsPerTool'"
				" / 'ListOfLegsPerTag' / 'MuonLegsPerTag' / 'ElectronLegsPerTag' is.");
			return false;
		}
		return true;
	}
	
	/// All tools mentioned in 'ListOfTagsPerTool' must be known
	unsigned nElectronToolsWithTags = 0, nMuonToolsWithTags = 0, nPhotonToolsWithTags = 0;
	for(auto& kv : m_parent.m_tagsPerTool)
	{
		auto& name = kv.first;
		if(findToolByName(m_parent.m_suppliedElectronEfficiencyTools, name)
			|| findToolByName(m_parent.m_suppliedElectronScaleFactorTools, name)) ++nElectronToolsWithTags;
		else if(findToolByName(m_parent.m_suppliedMuonTools, name)) ++nMuonToolsWithTags;
		else if(findToolByName(m_parent.m_suppliedPhotonEfficiencyTools, name)
			|| findToolByName(m_parent.m_suppliedPhotonScaleFactorTools, name)) ++nPhotonToolsWithTags;
		else
		{
			success = false;
			std::string all_tools = "; the known tools are";
			for(auto& tool : m_parent.m_suppliedElectronEfficiencyTools) all_tools += " " + tool.name();
			for(auto& tool : m_parent.m_suppliedElectronScaleFactorTools) all_tools += " " + tool.name();
			for(auto& tool : m_parent.m_suppliedPhotonEfficiencyTools) all_tools += " " + tool.name();
			for(auto& tool : m_parent.m_suppliedPhotonScaleFactorTools) all_tools += " " + tool.name();
			ATH_MSG_ERROR("Unknown tool " << name << " mentioned in property 'ListOfTagsPerTool'");
		}
	}
	/// Either all muon tools are associated to tags, either none
	if(nMuonToolsWithTags && (nMuonToolsWithTags != m_parent.m_suppliedMuonTools.size()))
	{
		ATH_MSG_ERROR("Not all muon tools have been associated with tags in the 'ListOfTagsPerTool' property");
		success = false;
	}
	/// Either all electron tools are associated to tags, either none
	unsigned nSupplied = m_parent.m_suppliedElectronEfficiencyTools.size() + m_parent.m_suppliedElectronScaleFactorTools.size();
	if(nElectronToolsWithTags && (nElectronToolsWithTags!=nSupplied))
	{
		ATH_MSG_ERROR("Not all electron tools have been associated with tags in the 'ListOfTagsPerTool' property");
		success = false;
	}
	if(!success) return false;
	/// Either all photon tools are associated to tags, either none
	nSupplied = m_parent.m_suppliedPhotonEfficiencyTools.size() + m_parent.m_suppliedPhotonScaleFactorTools.size();
	if(nPhotonToolsWithTags && (nPhotonToolsWithTags!=nSupplied))
	{
		ATH_MSG_ERROR("Not all photon tools have been associated with tags in the 'ListOfTagsPerTool' property");
		success = false;
	}
	if(!success) return false;

	/*
	 *  More checks that are done in other places (or still need to be implemented!):
	 *
	 * - [advancedConfigChecks()] for each entry in ListOfLegsPerTag there must be a suitable tool for that tag and leg(s)
	 * - [enumerateTools()] no two electron/photon tools share the same {leg,tag} combination
	 * - [enumerateTools()] no two muon tools share the same tag
	 * - [advancedConfigChecks()] electron efficiency and scale factors have identical configurations: for each eff. tool leg/tag we must find a SF tool, and the other leg/tag pairs associated to those tools must be identical.
	 * - [UNCHECKED] if tags are used with electron (resp. muon, photon) tools, then all electron (muon, photon) tools must have an entry in ListOfTagsPerTool. Done partially in this function, but the case where no tools are tagged (yet tags are used, according to ListOfLegsPerTag or LeptonTagDecorations) escapes detection. 
	 * - [loadTagsConfiguration()] each entry of ListOfLegsPerTag can be matched to a suitable tool
	 * - [UNCHECKED] suffixed tag read from a lepton must correspond to a know tag
	 * - [enumerateTools()] list of legs associated to each tool contains only known legs
	 * - [UNCHECKED TrigGlobEffCorrImportData import* functions] various consistency checks of the configuration files
	 * - [advancedConfigChecks()] user-specified periods are orthogonal
	 * - [ImportData::parseTriggerString()] no duplicated triggers in the combination
	 * - [UNCHECKED] for each configured electron/photon tool there is at least one associated tag^leg pair in 'ListOfLegsPerTag' (unless no electron/photon tags used)
	 * - [UNCHECKED] for each configured muon tool there is at least one associated tag in 'MuonLegsPerTag' (unless empty)
	 * - [enumerateTools()] electron tools can't be associated to photon legs; and reciprocally
	 */
	
	return success;
}

bool CheckConfig::advancedConfigChecks()
{
	/// This method requires all (most) of TrigGlobalEfficiencyCorrectionTool internal variables to have been properly initialized already
	/// -> to be called as the last step of TrigGlobalEfficiencyCorrectionTool::initialize()
	bool success = true;

	using ToolKey = TrigGlobalEfficiencyCorrectionTool::ToolKey;
	
	/// Check that for each electron/photon efficiency tool there is an associated scale factor tool
	/// with the same associated legs and tags. And vice versa. 
	auto checkConsistency = [this](auto& effToolIndex, auto& sfToolIndex, const char* type)
	{
		bool mismatch = (effToolIndex.size() != sfToolIndex.size());
		if(!mismatch)
		{
			for(auto& kv : sfToolIndex)
			{
				auto itr = effToolIndex.find(kv.first);
				if(itr != effToolIndex.end())
				{
					std::size_t index1 = kv.second, index2 = itr->second;
					flat_set<ToolKey> pairs1, pairs2;
					for(auto& kv : sfToolIndex) if(kv.second==index1) pairs1.insert(kv.first);
					for(auto& kv : effToolIndex) if(kv.second==index2) pairs2.insert(kv.first);
					if(pairs1 != pairs2) mismatch = true;
				}
				else mismatch = true;
			}
		}
		if(mismatch)
		{
			ATH_MSG_ERROR("There must be a one-to-one correspondence between the " << type << " efficiency and scale factor tools "
				"(including their associated trigger legs and selection tags)");
			return false;
		}
		return true;
	};
	if(!checkConsistency(m_parent.m_electronEffToolIndex, m_parent.m_electronSfToolIndex, "electron")) return false;
	if(!checkConsistency(m_parent.m_photonEffToolIndex, m_parent.m_photonSfToolIndex, "photon")) return false;
	
	/// A suitable CP tool must be available for each entry of 'ListOfLegsPerTag'
	for(auto& kv : m_parent.m_legsPerTag)
	{
		std::size_t tag = (kv.first!="*")? m_parent.m_hasher(kv.first) : 0;
		for(std::size_t leg : m_parent.listNonOrderedCSValues(kv.second,success))
		{
			auto type = ImportData::associatedLeptonFlavour(m_parent.m_dictionary[leg],success);
			if(type == xAOD::Type::Electron)
			{
				if(m_parent.m_electronEffToolIndex.find(ToolKey(leg, tag)) == m_parent.m_electronEffToolIndex.end())
				{
					ATH_MSG_ERROR("No electron tool provided for the combination of trigger leg '" << m_parent.m_dictionary[leg]
						<< "' and selection tag '" << kv.first << "' mentioned in the property 'ListOfLegsPerTag'");
					success = false;
				}
			}
			else if(type == xAOD::Type::Muon)
			{
				if(m_parent.m_muonToolIndex.find(ToolKey(0, tag)) == m_parent.m_muonToolIndex.end())
				{
					ATH_MSG_ERROR("No muon tool provided for the combination of trigger leg '" << m_parent.m_dictionary[leg]
						<< "' and selection tag '" << kv.first << "' mentioned in the property 'ListOfLegsPerTag'");
					success = false;
				}
			}
			else if(type == xAOD::Type::Photon)
			{
				if(m_parent.m_photonEffToolIndex.find(ToolKey(leg, tag)) == m_parent.m_photonEffToolIndex.end())
				{
					ATH_MSG_ERROR("No photon tool provided for the combination of trigger leg '" << m_parent.m_dictionary[leg]
						<< "' and selection tag '" << kv.first << "' mentioned in the property 'ListOfLegsPerTag'");
					success = false;
				}
			}
			else
			{					
				ATH_MSG_ERROR("Unable to determine which lepton flavour is associated to the trigger leg '" << m_parent.m_dictionary[leg] << "' in the property 'ListOfLegsPerTag'");
				success = false;
			}
		}
	}
	if(!success) return false;
	
	/// Periods don't overlap
	auto periods = m_parent.m_calculator->m_periods;	
	const auto periods_end =  periods.end();
	for(auto itr1=periods.begin(); itr1!=periods_end; ++itr1)
	{
		auto& x = itr1->m_boundaries;
		if(x.second < x.first)
		{
			ATH_MSG_ERROR("One of the periods specified in TriggerCombination has runMin (" << x.first << ") > runMax ("  << x.second << ")");
			success = false;
		}
		for(auto itr2=itr1+1; itr2!=periods_end; ++itr2)
		{
			auto& y = itr2->m_boundaries;
			if((x.first>=y.first && x.first<=y.second) || (x.second>=y.first && x.second<=y.second))
			{
				ATH_MSG_ERROR("The periods specified in TriggerCombination overlap");
				success = false;
			}
		}
	}
	if(!success) return false;
	
	return success;
}
