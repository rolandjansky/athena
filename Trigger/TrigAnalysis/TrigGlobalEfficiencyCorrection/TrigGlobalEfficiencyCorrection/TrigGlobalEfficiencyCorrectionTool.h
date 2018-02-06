/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// contact: jmaurer@cern.ch

#ifndef TRIGGLOBALEFFICIENCYCORRECTION_TRIGGLOBALEFFICIENCYCORRECTIONTOOL_H
#define TRIGGLOBALEFFICIENCYCORRECTION_TRIGGLOBALEFFICIENCYCORRECTIONTOOL_H 1

#include "TriggerAnalysisInterfaces/ITrigGlobalEfficiencyCorrectionTool.h"

#include "AsgTools/AsgTool.h"
#include "AsgTools/ToolHandleArray.h"
#include "EgammaAnalysisInterfaces/IAsgElectronEfficiencyCorrectionTool.h"
#include "MuonAnalysisInterfaces/IMuonTriggerScaleFactors.h"
#include "AthContainers/AuxElement.h"

#include <string>
#include <vector>
#include <map>
#include <set>
#include <boost/container/container_fwd.hpp>

namespace TrigGlobEffCorr
{
	class ImportData;
	class Calculator;
	class CheckConfig;
	class Efficiencies;
	class Lepton;
}

class TrigGlobalEfficiencyCorrectionTool: public asg::AsgTool, public virtual ITrigGlobalEfficiencyCorrectionTool
{
public: 
	ASG_TOOL_CLASS(TrigGlobalEfficiencyCorrectionTool,ITrigGlobalEfficiencyCorrectionTool)

	TrigGlobalEfficiencyCorrectionTool(const std::string& name);
	virtual ~TrigGlobalEfficiencyCorrectionTool();

	virtual StatusCode  initialize() override;

	virtual CP::CorrectionCode getEfficiencyScaleFactor(const std::vector<const xAOD::Electron*>& electrons, const std::vector<const xAOD::Muon*>& muons, double& efficiencyScaleFactor) override;
	virtual CP::CorrectionCode getEfficiencyScaleFactor(unsigned runNumber, const std::vector<const xAOD::Electron*>& electrons, const std::vector<const xAOD::Muon*>& muons, double& efficiencyScaleFactor) override;
	virtual CP::CorrectionCode getEfficiency(const std::vector<const xAOD::Electron*>& electrons, const std::vector<const xAOD::Muon*>& muons, double& efficiencyData, double& efficiencyMc) override;
	virtual CP::CorrectionCode getEfficiency(unsigned runNumber, const std::vector<const xAOD::Electron*>& electrons, const std::vector<const xAOD::Muon*>& muons, double& efficiencyData, double& efficiencyMc) override;
	
	virtual CP::CorrectionCode getEfficiencyScaleFactor(const std::vector<const xAOD::IParticle*>& particles, double& efficiencyScaleFactor) override;
	virtual CP::CorrectionCode getEfficiencyScaleFactor(unsigned runNumber, const std::vector<const xAOD::IParticle*>& particles, double& efficiencyScaleFactor) override;
	virtual CP::CorrectionCode getEfficiency(const std::vector<const xAOD::IParticle*>& particles, double& efficiencyData, double& efficiencyMc) override;
	virtual CP::CorrectionCode getEfficiency(unsigned runNumber, const std::vector<const xAOD::IParticle*>& particles, double& efficiencyData, double& efficiencyMc) override;	
	
	virtual bool isAffectedBySystematic(const CP::SystematicVariation& systematic) const override;
	virtual CP::SystematicSet affectingSystematics() const override;
	virtual CP::SystematicSet recommendedSystematics() const override;
	virtual CP::SystematicCode applySystematicVariation(const CP::SystematicSet& systConfig) override;

	static CP::CorrectionCode suggestElectronMapKeys(const std::map<std::string,std::string>& triggerCombination, const std::string& version, 
		std::map<std::string,std::string>& legsPerKey, const asg::AsgToolBase* caller = nullptr);
	
private: 

	struct Hierarchy
	{
		short offset, nLegs;
		float minPt, maxPt;
	};
	struct TagDecorator
	{
		SG::AuxElement::Decorator<char> decorator;
		std::size_t hash;
		bool suffixed;
		TagDecorator(const std::string& name, std::size_t hash, bool suffixed) : decorator(name), hash(hash), suffixed(suffixed) {}
	};
	struct CachedRanking
	{
		float minPt, maxPt;
		unsigned long ranking;
		operator bool() const { return ranking+1; }
		static bool invalid(unsigned long ranking) { return !(ranking+1); }
	};
	using LeptonList = std::vector<TrigGlobEffCorr::Lepton>;
	
	/// Properties:
	ToolHandleArray<IAsgElectronEfficiencyCorrectionTool> m_suppliedElectronEfficiencyTools;
	ToolHandleArray<IAsgElectronEfficiencyCorrectionTool> m_suppliedElectronScaleFactorTools;
	ToolHandleArray<CP::IMuonTriggerScaleFactors> m_suppliedMuonTools;
	std::map<std::string, std::string> m_legsPerTool;
	std::map<std::string, std::string> m_triggerCb;
	std::map<std::string, std::string> m_triggerCbPerYear;
	std::string m_leptonTagDecorations;
	std::map<std::string, std::string> m_tagsPerTool;
	std::map<std::string, std::string> m_electronLegsPerTag; /// deprecated
	std::map<std::string, std::string> m_muonLegsPerTag; /// deprecated
	std::map<std::string, std::string> m_legsPerTag;
	std::map<std::string, std::string> m_overrideThresholds;
	unsigned long m_numberOfToys;
	bool m_useInternalSeed;

	std::hash<std::string> m_hasher; //!
	std::map<std::size_t,float > m_thresholds; //!
	std::multimap<std::size_t, CachedRanking> m_cachedLegRankings; //!
	std::map<std::size_t, ToolHandle<IAsgElectronEfficiencyCorrectionTool>* > m_electronSfTools; //!
	std::map<std::size_t, ToolHandle<IAsgElectronEfficiencyCorrectionTool>* > m_electronEffTools; //!
	std::map<std::size_t, ToolHandle<CP::IMuonTriggerScaleFactors>* > m_muonTools; //!
	std::set<std::size_t> m_validLegTagPairs; //!
	bool m_checkElectronLegTag; //!
	bool m_checkMuonLegTag; //!
	std::map<std::size_t, std::string> m_dictionary; //!
	
	std::vector<Hierarchy> m_hierarchyMeta; //!
	std::vector<std::size_t> m_hierarchyData; //!
	
	bool m_initialized = false; //!
	CP::CorrectionCode m_cpCode = CP::CorrectionCode::Ok; //!
	unsigned long m_seed;
	
	std::vector<TagDecorator> m_leptonTagDecorators; //!
	SG::AuxElement::Decorator<unsigned int> m_runNumberDecorator; //!
	SG::AuxElement::Decorator<unsigned long> m_eventNumberDecorator; //!
	std::vector<TrigGlobEffCorr::Calculator> m_calculators; //!
	
	template<typename Key> using flat_set = boost::container::flat_set<Key>;

	/// Internal methods (I) -- initialization of the tool
	bool loadHierarchies();
	template<class CPTool> bool enumerateTools(ToolHandleArray<CPTool>& suppliedTools,
		std::map<std::size_t, ToolHandle<CPTool>* >& indexedTools, flat_set<std::size_t>& collectedTags);
	bool parseTagString(const std::string& tagstring, flat_set<std::size_t>& tags);
	bool loadTriggerCombination(TrigGlobEffCorr::ImportData& data, bool useDefaultTools);
	bool loadTagDecorators(const flat_set<std::size_t>& collectedElectronTags, const flat_set<std::size_t>& collectedMuonTags);
	bool loadListOfLegsPerTag(const flat_set<std::size_t>& collectedElectronTags, const flat_set<std::size_t>& collectedMuonTags);
	bool processDeprecatedProperties();
	
	/// Internal methods (II) -- core task
	bool retrieveRunNumber(unsigned& runNumber);
	bool retrieveEventNumber(unsigned long& eventNumber);
	bool aboveThreshold(const TrigGlobEffCorr::Lepton& p,std::size_t leg) const;
	template<class... ListOfLegs> unsigned long getCachedTriggerLegsRanking(const TrigGlobEffCorr::Lepton& lepton, ListOfLegs... legs);
	std::size_t getLoosestLegAboveThreshold(const TrigGlobEffCorr::Lepton& lepton, const flat_set<std::size_t>& legs, bool& success);
	std::size_t getLoosestLeg(const TrigGlobEffCorr::Lepton& lepton, std::size_t leg1, std::size_t leg2, bool& success);
	std::pair<std::size_t,std::size_t> getTwoLoosestLegs(const TrigGlobEffCorr::Lepton& lepton, const flat_set<std::size_t>& legs, bool& success);
	std::vector<std::size_t> getSortedLegs(const TrigGlobEffCorr::Lepton& lepton, const flat_set<std::size_t>& legs, bool& success);
	template<class Container> CachedRanking rankTriggerLegs(float pt, const Container& legs);
	template<class Particle> bool updateLeptonList(LeptonList& leptons, const std::vector<const Particle*>& particles);
	void updateMuonTriggerNames(std::size_t leg, const std::string& name);
	bool getTriggerLegEfficiencies(const xAOD::Electron* p, std::size_t leg, std::size_t tag, TrigGlobEffCorr::Efficiencies& efficiencies);
	bool getTriggerLegEfficiencies(const xAOD::Muon* p, std::size_t leg, std::size_t tag, TrigGlobEffCorr::Efficiencies& efficiencies);
	CP::CorrectionCode getEfficiencyScaleFactor(unsigned runNumber, const LeptonList& leptons, double& efficiencyScaleFactor);
	CP::CorrectionCode getEfficiency(unsigned runNumber, const LeptonList& leptons, double& efficiencyData, double& efficiencyMc);
	std::size_t getCombinedHash(const flat_set<std::size_t>& legs);
	std::size_t getCombinedHash(std::size_t leg1, std::size_t leg2);
	static inline constexpr const flat_set<std::size_t>& forwardLegs(const flat_set<std::size_t>& legs);
	static inline constexpr std::array<std::size_t, 2> forwardLegs(std::size_t leg1, std::size_t leg2);
	
	/// Internal methods (III) -- misc. helpers
	inline bool checkAndRecord(CP::CorrectionCode&& cc);
	flat_set<std::size_t> listNonOrderedCSValues(const std::string& s, bool& success);

	friend class TrigGlobEffCorr::ImportData;
	friend class TrigGlobEffCorr::CheckConfig;
	friend class TrigGlobEffCorr::Calculator;
};

#endif
