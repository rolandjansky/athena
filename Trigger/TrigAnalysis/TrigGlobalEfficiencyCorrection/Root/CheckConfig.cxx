// contact: jmaurer@cern.ch

#include "TrigGlobalEfficiencyCorrection/CheckConfig.h"
#include "TrigGlobalEfficiencyCorrection/ImportData.h"
#include "TrigGlobalEfficiencyCorrection/Calculator.h"
#include <boost/container/flat_set.hpp>

using CheckConfig = TrigGlobEffCorr::CheckConfig;
using ImportData = TrigGlobEffCorr::ImportData;


template<class CPTool>
ToolHandle<CPTool>* CheckConfig::findToolByName(ToolHandleArray<CPTool>& suppliedTools, const std::string& name)
{
	for(auto& tool : suppliedTools)
	{
		if(tool->name()==name)
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
	
	/// All tools mentioned in 'ListOfLegsPerTool' must be in 'ElectronEfficiencyTools' or 'ElectronScaleFactorTools'
	for(auto& kv : m_parent.m_legsPerTool)
	{
		auto& name = kv.first;
		if(findToolByName(m_parent.m_suppliedElectronEfficiencyTools, name)
			|| findToolByName(m_parent.m_suppliedElectronScaleFactorTools, name)) continue;
		success = false;
		if(findToolByName(m_parent.m_suppliedMuonTools, name))
		{
			ATH_MSG_ERROR("Muon tool" << name << " mentioned in property 'ListOfLegsPerTool', which is only aimed at electron tools");
		}
		else
		{
			ATH_MSG_ERROR("Unknown tool" << name << " mentioned in property 'ListOfLegsPerTool'");
		}
	}
	if(!success) return false;
	
	/// All electron tools must be associated to trigger legs (except when there's only one tool)
	if(m_parent.m_suppliedElectronEfficiencyTools.size() > 1)
	{
		for(int i=0;i<2;++i)
		{
			for(auto& tool : (i? m_parent.m_suppliedElectronEfficiencyTools : m_parent.m_suppliedElectronScaleFactorTools))
			{
				const std::string& name = tool->name();
				if(m_parent.m_legsPerTool.find(name) == m_parent.m_legsPerTool.end())
				{
					success = false;
					ATH_MSG_ERROR("Electron tool " << name << " associated trigger legs are not indicated in 'ListOfLegsPerTool', "
						"doing so is mandatory when several tools are used");
				}
			}
		}
	}
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
	
	/// All tools mentioned in 'ListOfTagsPerTool' must be in 'ElectronEfficiencyTools', 'ElectronScaleFactorTools' or 'MuonTools'
	unsigned nElectronToolsWithTags = 0, nMuonToolsWithTags = 0;
	for(auto& kv : m_parent.m_tagsPerTool)
	{
		auto& name = kv.first;
		if(findToolByName(m_parent.m_suppliedElectronEfficiencyTools,name)
			|| findToolByName(m_parent.m_suppliedElectronScaleFactorTools,name)) ++nElectronToolsWithTags;
		else if(findToolByName(m_parent.m_suppliedMuonTools,name)) ++nMuonToolsWithTags;
		else
		{
			success = false;
			ATH_MSG_ERROR("Unknown tool" << name << " mentioned in property 'ListOfTagsPerTool'");
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

	/*
	 *  More checks that are done in other places:
	 *
	 * - [advancedConfigChecks()] for each entry in ListOfLegsPerTag there must be a suitable tool for that tag and leg(s)
	 * - [enumerateTools()] no two electron tools share the same {leg,tag} combination
	 * - [enumerateTools()] no two muon tools share the same tag
	 * - [matchingEfficiencyAndScaleFactorTools()] electron efficiency and scale factors have identical configurations: for each eff. tool leg/tag we must find a SF tool, and the other leg/tag pairs associated to those tools must be identical.
	 * - [UNCHECKED] if tags are used with electron (resp. muon) tools, then all electron (muon) tools must have an entry in ListOfTagsPerTool. Done partially in this function, but the case where no tools are tagged (yet tags are used, according to ListOfLegsPerTag or LeptonTagDecorations) escapes detection. 
	 * - [loadTagsConfiguration()] each entry of ListOfLegsPerTag can be matched to a suitable tool
	 * - [UNCHECKED] suffixed tag read from a lepton must correspond to a know tag
	 * - [enumerateTools()] list of legs associated to each tool contains only known legs
	 * - [UNCHECKED TrigGlobEffCorrImportData import* functions] various consistency checks of the configuration files
	 * - [UNCHECKED] some unfinished functions in advancedConfigChecks
	 * - [advancedConfigChecks()] user-specified periods are orthogonal
	 * - [ImportData::parseTriggerString()] no duplicated triggers in the combination
	 */
	
	return success;
}

bool CheckConfig::advancedConfigChecks()
{
	/// This method requires all (most) of TrigGlobalEfficiencyCorrectionTool internal variables to have been properly initialized already
	/// -> to be called as the last step of TrigGlobalEfficiencyCorrectionTool::initialize()
	bool success = true;
	
	const auto& electronEffTools = m_parent.m_electronEffTools;
	const auto& electronSfTools = m_parent.m_electronSfTools;
	const auto& muonTools = m_parent.m_muonTools;
	
	/// Check that for each electron efficiency tool there is an associated scale factor tool
	/// with the same associated legs and tags. And vice versa. 
	bool mismatch = (electronEffTools.size() != electronSfTools.size());
	if(!mismatch)
	{
		for(auto& kv : electronSfTools)
		{
			auto itr = electronEffTools.find(kv.first);
			if(itr != electronEffTools.end())
			{
				void *ptr1 = kv.second, *ptr2 = itr->second;
				flat_set<std::size_t> pairs1, pairs2;
				for(auto& kv : electronSfTools)
				{
					if(kv.second==ptr1) pairs1.insert(kv.first);
				}
				for(auto& kv : electronEffTools)
				{
					if(kv.second==ptr2) pairs2.insert(kv.first);
				}
				if(pairs1 != pairs2)
				{
					mismatch = true;
					break;
				}
			}
			else
			{
				mismatch = true;
				break;
			}
		}
	}
	if(mismatch)
	{
		ATH_MSG_ERROR("There must be a one-to-one correspondence between the electron efficiency and scale factor tools "
			"(including their associated trigger legs and selection tags)");
		return false;
	}
	
	/// A suitable CP tool must be available for each entry of 'ListOfLegsPerTag'
	for(auto& kv : m_parent.m_legsPerTag)
	{
		std::size_t tag = (kv.first!="*")? m_parent.m_hasher(kv.first) : 0;
		for(std::size_t leg : m_parent.listNonOrderedCSValues(kv.second,success))
		{
			auto type = ImportData::associatedLeptonFlavour(m_parent.m_dictionary[leg],success);
			if(type == xAOD::Type::Electron)
			{
				if(electronEffTools.find(tag ^ leg) == electronEffTools.end())
				{
					ATH_MSG_ERROR("No electron tool provided for the combination of trigger leg '" << m_parent.m_dictionary[leg]
						<< "' and selection tag '" << kv.first << "' mentioned in the property 'ListOfLegsPerTag'");
					success = false;
				}
			}
			else if(type == xAOD::Type::Muon)
			{
				if(muonTools.find(tag) == muonTools.end())
				{
					ATH_MSG_ERROR("No muon tool provided for the combination of trigger leg '" << m_parent.m_dictionary[leg]
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
	const auto calcs_end = m_parent.m_calculators.end();
	for(auto itr1=m_parent.m_calculators.begin(); itr1!=calcs_end; ++itr1)
	{
		auto& x = itr1->getBoundaries();
		if(x.second < x.first)
		{
			ATH_MSG_ERROR("One of the periods specified in TriggerCombination has runMin (" << x.first << ") > runMax ("  << x.second << ")");
			success = false;
		}
		for(auto itr2=itr1+1; itr2!=calcs_end; ++itr2)
		{
			auto& y = itr2->getBoundaries();
			if((x.first>=y.first && x.first<=y.second) || (x.second>=y.first && x.second<=y.second))
			{
				ATH_MSG_ERROR("The periods specified in TriggerCombination overlap");
				success = false;
			}
		}
	}
	if(!success) return false;
	
	/*
	/// Check that for each configured electron tool there is at least one associated tag^leg pair in 'ListOfLegsPerTag' (unless no electron tags used)
	if(m_electronLegsPerTag.size())
	{
		std::map<ToolHandle<IAsgElectronEfficiencyCorrectionTool>*,bool> safeTools;
		for(auto& kv : m_electronEffTools)
		{
			bool& safe = safeTools.emplace(kv.second,false).first->second;
			safe = safe || m_validLegTagPairs.count(kv.first);
		}
		for(auto& kv : safeTools)
		{
			if(kv.second) continue;
			for(auto& xy : m_electronLegsPerTag)
			{
				ATH_MSG_INFO(xy.first<<" "<<xy.second);
			}
			std::string name = kv.first->name();
			auto itrLegs = m_legsPerTool.find(name);
			std::string legs = (itrLegs!=m_legsPerTool.end())? "'"+itrLegs->second+"'" : "<not filled>";
			auto itrTags = m_tagsPerTool.find(name);
			std::string tags =  (itrTags!=m_tagsPerTool.end())? "'"+itrTags->second+"'" : "<not filled>";
			ATH_MSG_ERROR("the electron tool " << name <<" has been associated to leg(s) " << legs
				<< " and tag(s) " << tags <<", but none of the possible (tag,leg) combination has been found in the property 'ElectronLegsPerTag. "
				<< "Please double-check the properties 'ListOfLegsPerTool', 'ListOfTagsPerTool' and 'ElectronLegsPerTag'");
			success = false;
		}
	}
	
	/// Check that for each configured muon tool there is at least one associated tag in 'MuonLegsPerTag' (unless empty)
	/// Or should we let the default behaviour
	if(m_muonLegsPerTag.size())
	{
		for(auto& tool : m_suppliedMuonTools)
		{
			m_tagsPerTool[tool->name()] 
			need to check that all muon tools are mentioned in m_tagsPerTool
			bool& safe = safeTools.emplace(kv.second,false).first->second;
			safe = safe || m_validLegTagPairs.count(kv.first);
		}
	}
	*/
	
	return success;
}
