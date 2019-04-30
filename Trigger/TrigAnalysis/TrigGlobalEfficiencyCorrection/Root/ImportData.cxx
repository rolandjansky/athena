/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

// contact: jmaurer@cern.ch

#include "TrigGlobalEfficiencyCorrection/ImportData.h"
#include "PathResolver/PathResolver.h"

#include <fstream>
#include <functional>
#include <algorithm>

using ImportData = TrigGlobEffCorr::ImportData;

ImportData::ImportData() :
	/// the following two variables are owned by this tool and released in the destructor
	asg::AsgMessaging("TrigGlobalEfficiencyCorrectionTool"),
	m_parent(nullptr),
	m_dictionary(*new std::map<std::size_t,std::string>),
	m_hasher(*new std::hash<std::string>)
{
}

ImportData::ImportData(TrigGlobalEfficiencyCorrectionTool& parent) :
	asg::AsgMessaging(&parent),
	m_parent(&parent),
	m_dictionary(parent.m_dictionary),
	m_hasher(parent.m_hasher)
{
	msg().setLevel(parent.msg().level());
}

ImportData::~ImportData()
{
	if(!m_parent)
	{
		delete &m_dictionary;
		delete &m_hasher;
	}
}

bool ImportData::importAll(const std::map<std::string, std::string>& overridenThresholds)
{
	return importPeriods()
		&& importThresholds(overridenThresholds)
		&& importTriggers()
		&& importHierarchies();
}

bool ImportData::readDataFile(const char* filename, std::vector<std::string>& contents)
{
	contents.clear();
	bool success = true;
	auto name = PathResolverFindCalibFile(filename);
	if(name.length())
	{
		std::ifstream f(name.c_str(),std::ios_base::in);
		if(f.is_open())
		{
			std::string line;
			while(f.good())
			{
				if(std::getline(f,line))
				{
					auto i = line.find('#');
					if(i != std::string::npos) line = line.substr(0,i);
					if(line.length() >= 3) contents.emplace_back(std::move(line));
				}
			}
			if(!f.eof())
			{
				ATH_MSG_ERROR("Issue encountered while reading configuration file " << filename);
				success = false;
			}
			f.close();
			return true;
		}
		else
		{
		ATH_MSG_ERROR("Unable to open configuration file " << filename);
			success = false;
		}
	}
	else
	{
		ATH_MSG_ERROR("Unable to find configuration file " << filename);
		success = false;
	}
	return success;
}

void ImportData::setNonMixed3LType(TrigDef& def, TriggerType flavourFlag)
{
	if(def.leg[0]==def.leg[1] && def.leg[1]==def.leg[2]) def.type = static_cast<TriggerType>(TT_TRILEPTON_SYM | flavourFlag);
	else if(def.leg[0]!=def.leg[1] && def.leg[1]!=def.leg[2] && def.leg[0]!=def.leg[2]) def.type = static_cast<TriggerType>(TT_TRILEPTON_ASYM | flavourFlag);
	else /// swap legs so that the last two are identical, for later convenience
	{
		if(def.leg[1]!=def.leg[0] && def.leg[1]!=def.leg[2]) std::swap(def.leg[0],def.leg[1]);
		else if(def.leg[2]!=def.leg[0] && def.leg[2]!=def.leg[1]) std::swap(def.leg[0],def.leg[2]);
		def.type = static_cast<TriggerType>(TT_TRILEPTON_HALFSYM | flavourFlag);
	}
}

bool ImportData::importTriggers()
{
	if(!m_triggerThresholds.size() && !importThresholds()) return false;
	m_triggerDefs.clear();
	std::vector<std::string> config;
	if(!readDataFile("TrigGlobalEfficiencyCorrection/Triggers.cfg", config)) return false;
	std::stringstream ss;
	std::string triggerName, token;
	bool success = true;
	for(auto& line : config)
	{
                ATH_MSG_DEBUG(line);
		ss.clear();
		ss.str(line);
		ss >> triggerName;
		std::size_t h = m_hasher(triggerName);
		m_dictionary[h] = triggerName;
		ATH_MSG_DEBUG(std::to_string(h) << " " << triggerName );
		auto& def = m_triggerDefs[h];
		def.name = h;
		for(std::size_t& leg : def.leg)
		{
			if(!(ss >> token)) break;
			h = m_hasher(token);
			m_dictionary.emplace(h,token);
			leg = h;
			if(m_triggerThresholds.find(h) == m_triggerThresholds.end())
			{
				ATH_MSG_ERROR("Unknown trigger leg '" << token << "' found in Triggers.cfg");
				success = false;
			}
		}
		if(!def.leg[0])
		{
			def.leg[0] = h; /// default: assume the leg's name is the same as the full trigger name
			if(m_triggerThresholds.find(h) == m_triggerThresholds.end())
			{
				ATH_MSG_ERROR("Unknown trigger leg '" << triggerName << "' (inferred from trigger name) found in Triggers.cfg");
				success = false;
			}
		}
		if(!success) continue;
		
		/// Classify trigger and re-arrange legs (if needed) so that all electron legs come before muon legs, and muon legs before photon legs
		def.type = TT_UNKNOWN;		
		auto flavour0 = associatedLeptonFlavour(def.leg[0], success);
		int ne = (flavour0 == xAOD::Type::Electron)*1;
		int nm = (flavour0 == xAOD::Type::Muon)*1;
		if(def.leg[1])
		{
			auto flavour1 = associatedLeptonFlavour(def.leg[1], success);
			if(flavour1 == xAOD::Type::Electron)
			{
				if(!ne) std::swap(def.leg[0],def.leg[1]);
				++ne;
			}
			else if(flavour1 == xAOD::Type::Muon)
			{
				if(!(ne+nm)) std::swap(def.leg[0],def.leg[1]);
				++nm;
			}
			else if(flavour1 != xAOD::Type::Photon) success = false;
			if(def.leg[2])
			{
				auto flavour2 = associatedLeptonFlavour(def.leg[2], success);
				if(flavour2 == xAOD::Type::Electron)
				{
					if(!ne) std::swap(def.leg[0], def.leg[2]);
					else if(ne==1) std::swap(def.leg[1], def.leg[2]);
					++ne;
				}
				else if(flavour2 == xAOD::Type::Muon)
				{
					if(!(ne+nm)) std::swap(def.leg[0], def.leg[2]);
					else if((ne+nm)==1) std::swap(def.leg[1], def.leg[2]);
					++nm;
				}
				else if(flavour2 != xAOD::Type::Photon) success = false;
				if(ne+nm==0 || ne==3 || nm==3) /// single-flavour trilepton triggers
				{
					setNonMixed3LType(def, (ne? TT_ELECTRON_FLAG : nm? TT_MUON_FLAG : TT_PHOTON_FLAG));
				}
				else /// mixed-flavour trilepton triggers
				{
					bool sym = (def.leg[0]==def.leg[1] || def.leg[1]==def.leg[2]);
					if(ne==2) def.type = nm? (sym? TT_2E_MU_SYM : TT_2E_MU_ASYM) : (sym? TT_2E_G_SYM : TT_2E_G_ASYM);
					else if(nm==2) def.type = ne? (sym? TT_E_2MU_SYM : TT_E_2MU_ASYM) : (sym? TT_2MU_G_SYM : TT_2MU_G_ASYM);
					else if(ne+nm==1) def.type = ne? (sym? TT_E_2G_SYM : TT_E_2G_ASYM) : (sym? TT_MU_2G_SYM : TT_MU_2G_ASYM);
					else success = false;
				}
			}
			else /// dilepton triggers
			{
				if(ne==2) def.type = (def.leg[0]==def.leg[1])? TT_2E_SYM : TT_2E_ASYM;
				else if(nm==2) def.type = (def.leg[0]==def.leg[1])? TT_2MU_SYM : TT_2MU_ASYM;
				else if(ne+nm==0) def.type = (def.leg[0]==def.leg[1])? TT_2G_SYM : TT_2G_ASYM;
				else if(ne==1 && nm==1) def.type = TT_E_MU;
				else if(ne==1) def.type = TT_E_G;
				else if(nm==1) def.type = TT_MU_G;
			}
		}
		else /// single lepton triggers
		{
			def.type = ne? TT_SINGLE_E : nm? TT_SINGLE_MU : TT_SINGLE_G;
		}
		if(!success || def.type==TT_UNKNOWN)
		{
			success = false;
			ATH_MSG_ERROR("Configuration issue for trigger " << triggerName);
		}
	}
	return success;
}

bool ImportData::importThresholds(const std::map<std::string, std::string>& overridenThresholds)
{
	m_triggerThresholds.clear();
	std::vector<std::string> config;
	if(!readDataFile("TrigGlobalEfficiencyCorrection/Thresholds.cfg", config)) return false;
	std::stringstream ss;
	std::string leg, unit;
	float pt;
	bool success = true;
	for(auto& line : config)
	{
		ss.clear();
		ss.str(line);
		if(ss >> leg >> pt >> unit)
		{
			std::size_t h = m_hasher(leg);
			m_dictionary.emplace(h,leg);
			if(unit == "GeV") pt *= 1e3f;
			else if(unit != "MeV")
			{
				ATH_MSG_ERROR("Unable to import pT threshold for leg \"" << leg << "\" (missing unit)");
				success = false;
			}
			m_triggerThresholds[h] = pt;
		}
		else
		{
			ATH_MSG_ERROR("Unable to import pT threshold for leg \"" << leg << '"');
			success = false;
		}
	}
	if(!success)
	{
		m_triggerThresholds.clear();
		return false;
	}

	/// Override thresholds if requested
	bool belowRecommended = false;
	for(auto& kv : overridenThresholds)
	{
		auto itr = m_triggerThresholds.find(m_hasher(kv.first));
		if(itr != m_triggerThresholds.end())
		{
			float pt = 0.f;
			try { pt = std::stof(kv.second); }
			catch(...)
			{
				ATH_MSG_ERROR("Unable to convert threshold argument \""<<kv.second<<"\" to floating-point value");
				success = false;
				continue;
			}
			if(pt<1e3f)
			{
				ATH_MSG_WARNING("Suspiciously low threshold (" << pt << " MeV) set for trigger leg " << kv.first
					<< ", please make sure you provided the threshold in MeV and not in GeV!");
			}
			if(pt < itr->second) belowRecommended = true;
			itr->second = pt;
		}
		else
		{
			ATH_MSG_ERROR("Can't override threshold for unknown trigger leg " << kv.first);
			success = false;
		}
	}
	if(belowRecommended)
	{
		ATH_MSG_WARNING("Tool configured to use trigger thresholds below those recommended!");
	}
	return success;
}

bool ImportData::importPeriods()
{
	m_dataPeriods.clear();
	std::vector<std::string> config;
	if(!readDataFile("TrigGlobalEfficiencyCorrection/DataPeriods.cfg", config)) return false;
	std::stringstream ss;
	std::string key;
	std::pair<unsigned,unsigned> runs;
	for(auto& line : config)
	{
		ss.clear();
		ss.str(line);
		ss >> key >> runs.first >> runs.second;
		if(ss.fail())
		{
			m_dataPeriods.clear();
			return false;
		}
		m_dataPeriods.emplace(key,runs);
	}
	return true;
}

bool ImportData::importHierarchies()
{
	if(!m_triggerThresholds.size() && !importThresholds()) return false;
	m_hierarchyMeta.clear();
	m_hierarchyData.clear();
	std::vector<std::string> config;
	if(!readDataFile("TrigGlobalEfficiencyCorrection/Hierarchies.cfg", config)) return false;
	std::stringstream ss;
	std::string token, unit;
	std::map<std::size_t, std::vector<std::size_t> > aliases;
	for(auto& line : config)
	{
		bool success = true;
		ss.clear();
		ss.str(line);
		if(line[0]=='[')
		{
			auto& meta = *m_hierarchyMeta.emplace(m_hierarchyMeta.end(), Hierarchy{(short)m_hierarchyData.size(),0,0.f,1e12f});
			if(line[1]=='-' && line[2]==']') ss.ignore(3);
			else
			{
				char sep = '-';
				if(line[1]=='>') ss.ignore(2) >> meta.minPt >> unit;
				else if(line[1]=='<') ss.ignore(2) >> meta.maxPt >> unit;
				else ss.ignore(1) >> meta.minPt >> sep >> meta.maxPt >> unit;
				if(!ss || sep!='-' || (unit!="GeV]" && unit!="MeV]"))
				{
					ATH_MSG_ERROR("Unable to parse pT restrictions in Hierarchies.cfg");
					success = false;
				}
				if(unit == "GeV]")
				{
					meta.minPt *= 1e3f;
					if(meta.maxPt < 1e12f) meta.maxPt *= 1e3f;
				}
			}
			while(ss >> token)
			{
				std::size_t h = m_hasher(token);
				auto itr = aliases.find(h);
				if(itr == aliases.end())
				{
					if(m_triggerThresholds.find(h) == m_triggerThresholds.end())
					{
						ATH_MSG_ERROR("Unknown trigger leg '" << token << "' found in Hierarchies.cfg");
						success = false;
					}
					m_dictionary.emplace(h,token);
					m_hierarchyData.push_back(h);
				}
				else m_hierarchyData.insert(m_hierarchyData.end(),itr->second.begin(),itr->second.end());
				if(ss >> token && token!='>') success = false;
			}
			meta.nLegs = m_hierarchyData.size() - meta.offset;
			success = success && meta.nLegs;
		}
		else
		{
			ss >> token;
			auto& legs = aliases[m_hasher(token)];
			if(ss >> token && token==":=")
			{
				legs.clear();
				while(ss >> token)
				{
					std::size_t h = m_hasher(token);
					m_dictionary.emplace(h,token);
					legs.push_back(h);
					if(m_triggerThresholds.find(h) == m_triggerThresholds.end())
					{
						ATH_MSG_ERROR("Unknown trigger leg '" << token << "' found in Hierarchies.cfg");
						success = false;
					}
					if(ss >> token && token!='>') success = false;
				}
				success = success && legs.size();
			}
			else success = false;
		}
		if(!success)
		{
			ATH_MSG_ERROR("Failed parsing line from Hierarchies.cfg:\n" << line);
			m_hierarchyMeta.clear();
			m_hierarchyData.clear();
			return false;
		}
	}
	return true;
}

bool ImportData::importMapKeys(const std::string& version, std::map<std::size_t,std::map<std::size_t,int> >& keysPerLeg)
{
	keysPerLeg.clear();
	std::vector<std::string> config;
	if(!readDataFile("TrigGlobalEfficiencyCorrection/MapKeys.cfg", config)) return false;
	std::stringstream ss;
	std::string token;
	bool reading = false;
	for(auto& line : config)
	{
		std::size_t pos = line.find("[VERSION]");
		if(pos != std::string::npos)
		{
			reading = false;
			ss.clear();
			ss.str(TrigGlobEffCorr::removeWhitespaces(line.substr(pos+9)));
			while(std::getline(ss, token, ','))
			{
				if(token == version)
				{
					reading = true;
					break;
				}
			}
			continue;
		}
		if(!reading) continue;
		ss.clear();
		ss.str(line);
		int year;
		ss >> year >> token;
		year = 1<<(year-2015);
		std::size_t leg = m_hasher(token);
		auto& keys = keysPerLeg[leg];
		while(ss >> token)
		{
			std::size_t h = m_hasher(token);
			auto insertion = keys.emplace(h, year);
			if(insertion.second) m_dictionary.emplace(h, token);
			else insertion.first->second |= year;
		}
	}
	if(!keysPerLeg.size())
	{
		ATH_MSG_ERROR("Unable to import the available map keys for the version " << version);
		return false;
	}
	return true;
}

bool ImportData::getPeriodBoundaries(const std::string& period, std::pair<unsigned,unsigned>& boundaries)
{
	// First possibility: a defined period keyword
	auto itr = m_dataPeriods.find(period);
	if(itr!=m_dataPeriods.end())
	{
		boundaries = itr->second;
		return true;
	}
	// Otherwise it's a '-'-separated range
	auto sep = period.find_first_of('-');
	if(sep!=std::string::npos)
	{
		std::string kwMin = period.substr(0,sep);
		std::string kwMax = period.substr(sep+1);
		// Second possibility: a range of defined period keywords
		auto itrMin = m_dataPeriods.find(kwMin);
		auto itrMax = m_dataPeriods.find(kwMax);
		if(itrMin!=m_dataPeriods.end() && itrMax!=m_dataPeriods.end())
		{
			boundaries = std::minmax({itrMin->second.first, itrMax->second.first, itrMin->second.second, itrMax->second.second});
			return true;
		}
		// Third possibility: a range of run numbers
		try
		{
			boundaries = std::minmax(std::stoi(kwMin), std::stoi(kwMax));
			return true;
		}
		catch(...) {}
	}
	ATH_MSG_ERROR("Unable to understand the period/range " << period);
	return false;
}

xAOD::Type::ObjectType ImportData::associatedLeptonFlavour(const std::string& leg, bool& success)
{
	// note: 'success' is not set to 'true', only downgraded to 'false' if needed
	if(leg.length()>=2 && leg[0]=='e' && leg[1]>='1' && leg[1]<='9') return xAOD::Type::Electron;
	else if(leg.length()>=3 && leg[0]=='m' && leg[1]=='u' && leg[2]>='1' && leg[2]<='9') return xAOD::Type::Muon;
	else if(leg.length()>=3 && leg[0]=='g' && leg[1]>='1' && leg[1]<='9') return xAOD::Type::Photon;
	success = false;
	return xAOD::Type::Other;
}

xAOD::Type::ObjectType ImportData::associatedLeptonFlavour(std::size_t leg, bool& success)
{
	// note: 'success' is not set to 'true', only downgraded to 'false' if needed
	auto itr = m_dictionary.find(leg);
	if(itr != m_dictionary.end())
	{
		return associatedLeptonFlavour(itr->second,success);
	}
	success = false;
	return xAOD::Type::Other;
}

std::vector<ImportData::TrigDef> ImportData::parseTriggerString(const std::string& triggerString, bool& success)
{
	std::string s = TrigGlobEffCorr::removeWhitespaces(triggerString);
	if(s.find("|||") != std::string::npos)
	{
		ATH_MSG_ERROR("Invalid format for the trigger combination '" << triggerString <<"'");
		success = false;
		return {};
	}
	/// Replace all || by |
	while(true)
	{
		auto i = s.find("||");
		if(i == std::string::npos) break;
		s.replace(i, 1, "");
	}
	if(s=="" || s=="|")
	{
		ATH_MSG_ERROR("Invalid format for the trigger combination '" << triggerString <<"'");
		success = false;
		return {};
	}
	std::vector<TrigDef> triggers;
	std::set<std::size_t> hashes;
	std::stringstream ss(s);
	while(std::getline(ss,s,'|'))
	{
		std::size_t trig = m_hasher(s);
		ATH_MSG_DEBUG(std::to_string(trig) << " --> " << s );
		auto itr = m_triggerDefs.find(trig);
		if(itr == m_triggerDefs.end())
		{
			ATH_MSG_ERROR("Unknown trigger '" << s << "' found while parsing trigger combination");
			success = false;
			return {};	
		}
		if(!hashes.insert(trig).second)
		{
			ATH_MSG_ERROR("The trigger '" << s << "' is present more than once in the combination");
			success = false;
			return {};
		}
		triggers.push_back(itr->second);
	}
	success = success && triggers.size();
	return triggers;
}

bool ImportData::suggestElectronMapKeys(const std::map<std::string,std::string>& triggerCombination,
		const std::string& version, std::map<std::string,std::string>& legsPerKey)
{
	legsPerKey.clear();
	if(!importAll()) return false;
	
	bool success = true;
	std::map<std::size_t,int> legs;
	
	for(auto& kv : triggerCombination)
	{
		auto itrPeriod = m_dataPeriods.find(kv.first);
		if(itrPeriod == m_dataPeriods.end())
		{
			ATH_MSG_ERROR("Unknown period " << kv.first);
			success = false;
			continue;
		}
		int years = 0;
		for(int k=0;k<32;++k)
		{
			auto itr = m_dataPeriods.find(std::to_string(2015+k));
			if(itr != m_dataPeriods.end() && itrPeriod->second.first <= itr->second.second 
				&& itrPeriod->second.second >= itr->second.first)
			{
				years |= (1<<k);
			}
		}
		auto triggers = parseTriggerString(kv.second,success);
		if(!success) continue;
		for(auto& trig : triggers)
		{
			for(std::size_t leg : trig.leg)
			{
				if(leg && associatedLeptonFlavour(leg, success)==xAOD::Type::Electron)
				{
					auto insertion = legs.emplace(leg,years);
					if(!insertion.second) insertion.first->second |= years;
				}
			}
		}
	}
	if(!success) return false;

	if(version != "")
	{
		std::map<std::size_t,std::map<std::size_t,int> > allKeys;
		if(!importMapKeys(version, allKeys)) return false;
		std::map<std::size_t,std::vector<std::size_t> > allLegsPerKey;
		std::set<std::size_t> legsWithMultipleKeys;
		bool sameKeyForAllyears = true;
		while(legs.size())
		{
			allLegsPerKey.clear();
			for(auto& kvLegs : legs) // loop on remaining legs
			{
				std::size_t leg = kvLegs.first;
				int years = kvLegs.second;
				auto itrKeys = allKeys.find(leg); // list of keys for that leg
				if(itrKeys != allKeys.end())
				{
					for(auto& kvKeys : itrKeys->second) // loop on those keys
					{
						auto y = (kvKeys.second & years);
						if((y==years) || (!sameKeyForAllyears && y!=0)) // key must support all years needed for that leg -- until no longer possible
						{
							auto insertion = allLegsPerKey.emplace(kvKeys.first,std::vector<std::size_t>{leg});
							if(!insertion.second) insertion.first->second.push_back(leg); 
						}
					}
				}
				else
				{
					ATH_MSG_ERROR("Sorry, no idea what the map key should be for the trigger leg '" 
							<< m_dictionary.at(leg) << "', manual configuration is needed");
					success = false;
				}
			}
			if(!success) break;

			if(!allLegsPerKey.size())
			{
				if(sameKeyForAllyears)
				{
					sameKeyForAllyears = false;
					continue;
				}
				success = false;
				break;
			}
			
			using T = decltype(allLegsPerKey)::value_type;
			auto itrKey = std::max_element(allLegsPerKey.begin(), allLegsPerKey.end(),
				[](T& x,T& y){return x.second.size()<y.second.size();});
			std::string& strLegs = legsPerKey[m_dictionary.at(itrKey->first)];
			for(std::size_t leg : itrKey->second)
			{
				int& wantedYears = legs.at(leg);
				int supportedYears = (allKeys.at(leg).at(itrKey->first)) & wantedYears;
				if(supportedYears!=wantedYears || legsWithMultipleKeys.count(leg))
				{
					legsWithMultipleKeys.insert(leg);
					for(int i=0;i<32;++i)
					{
						if(supportedYears & (1<<i))
						{
							if(strLegs.length() && strLegs.back()!=',') strLegs += ',';
							strLegs += m_dictionary.at(leg) + "[" + std::to_string(2015 + i) + "]";
						}
					}
				}
				else
				{
					if(strLegs.length() && strLegs.back()!=',') strLegs += ',';
					strLegs += m_dictionary.at(leg);
				}
				if(supportedYears == wantedYears) legs.erase(leg);
				else wantedYears &= ~supportedYears;
			}
		}
	}
	else
	{
		/// If no version is specified, the list of trigger legs is returned
		for(auto& kv : legs)
		{
			legsPerKey.emplace(std::to_string(legsPerKey.size()), m_dictionary.at(kv.first));
		}
	}
	
	if(!success) legsPerKey.clear();
	return success;
}

bool ImportData::adaptTriggerListForTriggerMatching(std::vector<ImportData::TrigDef>& triggers)
{
	/// This essentially splits _OR_ single lepton triggers into independent items
	std::set<std::size_t> extraItems; /// to prevent duplicates
	std::vector<ImportData::TrigDef> updatedTriggers;
	for(auto& trig : triggers)
	{
		auto& name = m_dictionary.at(trig.name);
		std::size_t pos = 0, len = name.find("_OR_");
		if(len == std::string::npos)
		{
			updatedTriggers.emplace_back(trig);
			continue;
		}
		while(true)
		{
			std::string item = name.substr(pos, len);
			auto h = m_hasher(item);
			auto def = m_triggerDefs.find(h);
			if(def == m_triggerDefs.end())
			{
				ATH_MSG_ERROR("while listing triggers for trigger matching; trigger \"" << item << "\" extracted from \"" << name << "\" is not recognized");
				return false;
			}
			if(extraItems.emplace(h).second) updatedTriggers.emplace_back(def->second);
			if(len == std::string::npos) break;
			pos += len + 4;
			len = name.find("_OR_", pos);
			if(len != std::string::npos) len -= pos;
		}
	}
	triggers.swap(updatedTriggers);
	return true;
}
