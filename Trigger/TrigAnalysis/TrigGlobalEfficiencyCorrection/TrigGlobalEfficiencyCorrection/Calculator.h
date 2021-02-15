/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

// contact: jmaurer@cern.ch

#ifndef TRIGGLOBALEFFICIENCYCORRECTION_CALCULATOR_H
#define TRIGGLOBALEFFICIENCYCORRECTION_CALCULATOR_H 1

#include "TrigGlobalEfficiencyCorrection/TrigGlobalEfficiencyCorrectionTool.h"
#include "TrigGlobalEfficiencyCorrection/ImportData.h"
#include "TrigGlobalEfficiencyCorrection/Efficiencies.h"

#include <map>
#include <algorithm>
#include <functional>
#include <boost/container/flat_set.hpp>
template<typename Key> using flat_set = boost::container::flat_set<Key>;

namespace TrigGlobEffCorr
{

class Lepton;

class Calculator : public asg::AsgMessaging
{
	using LeptonList = TrigGlobalEfficiencyCorrectionTool::LeptonList;
	using TrigDef = TrigGlobEffCorr::ImportData::TrigDef;
	using GlobEffFunc = std::function<bool(Calculator*,const LeptonList&,unsigned,Efficiencies&)>;

public:
	Calculator(TrigGlobalEfficiencyCorrectionTool& parent, unsigned nPeriodsToReserve);
	bool addPeriod(ImportData& data, const std::pair<unsigned,unsigned>& boundaries, const std::string& combination, 
			bool useToys, std::size_t& uniqueElectronLeg, std::size_t& uniquePhotonLeg);
	bool compute(TrigGlobalEfficiencyCorrectionTool& parent, const LeptonList& leptons, unsigned runNumber, Efficiencies& efficiencies);
	bool checkTriggerMatching(TrigGlobalEfficiencyCorrectionTool& parent, bool& matched, const LeptonList& leptons, unsigned runNumber);
	bool getRelevantTriggersForUser(TrigGlobalEfficiencyCorrectionTool& parent, std::vector<std::string>& triggers, unsigned runNumber);
	
	struct Period
	{
		const std::pair<unsigned,unsigned> m_boundaries;
		GlobEffFunc m_formula;
		std::vector<TrigDef> m_triggers; /// only used for trigger matching; not filled otherwise. Also, single-lepton _OR_ triggers are split!
		Period(const decltype(m_boundaries)& b, decltype(m_formula)&& f, decltype(m_triggers)&& t = {}) : m_boundaries(b), m_formula(f), m_triggers(t) {}
	};
	
private:
	TrigGlobalEfficiencyCorrectionTool* m_parent; /// pointer updated at each call to compute() because the parent tool might have been moved in-between
	
	std::vector<Period> m_periods;
	std::map<std::pair<const Lepton*, std::size_t>, Efficiencies> m_cachedEfficiencies;
	
	bool aboveThreshold(const Lepton& p,std::size_t leg) const { return m_parent->aboveThreshold(p, leg); }
	template<typename Trig1L> auto getLoosestLegAboveThreshold(const Lepton& lepton, const flat_set<Trig1L>& trigs, bool& success)
			-> std::enable_if_t<Trig1L::is1L(), std::size_t>
		{ return m_parent->getLoosestLegAboveThreshold(lepton, Trig1L::anonymize(trigs), success); }
	Efficiencies getCachedTriggerLegEfficiencies(const Lepton& lepton, unsigned runNumber, std::size_t leg, bool& success);
	bool fillListOfLegsFor(const Lepton& lepton, const std::vector<TrigDef>& triggers, flat_set<std::size_t>& validLegs) const;
	bool canTriggerBeFired(const TrigDef& trig, const std::vector<flat_set<std::size_t> >& firedLegs) const;
	const Period* getPeriod(unsigned runNumber) const;
	bool findUniqueLeg(xAOD::Type::ObjectType obj, std::size_t& uniqueLeg, const std::vector<TrigDef>& defs);
	
	/// One single-lepton trigger
	template<typename Trig1L>
	auto globalEfficiency(const LeptonList&, unsigned, const Trig1L, Efficiencies&)
		-> std::enable_if_t<Trig1L::is1L(), bool>;
	/// Two single-lepton triggers, two object types
	template<typename Trig1L_obj1, typename Trig1L_obj2>
	auto globalEfficiency(const LeptonList&, unsigned, const Trig1L_obj1 trig1, const Trig1L_obj2 trig2, Efficiencies&)
		-> std::enable_if_t<Trig1L_obj1::is1L()
							&& Trig1L_obj2::is1L()
							&& Trig1L_obj1::object() != Trig1L_obj2::object(),
			bool>;
	/// Several single-lepton triggers, one object type
	template<typename Trig1L>
	auto globalEfficiency(const LeptonList&, unsigned, const flat_set<Trig1L>&, Efficiencies&)
		-> std::enable_if_t<Trig1L::is1L(), bool>;
	/// Several single-lepton triggers, two object types
	template<typename Trig1L_obj1, typename Trig1L_obj2>
	auto globalEfficiency(const LeptonList&, unsigned, const flat_set<Trig1L_obj1>& trigs1, const flat_set<Trig1L_obj2>& trigs2, Efficiencies&)
		-> std::enable_if_t<Trig1L_obj1::is1L()
							&& Trig1L_obj2::is1L()
							&& Trig1L_obj1::object() != Trig1L_obj2::object(),
			bool>;
	/// One mixed-flavour dilepton trigger
	template<typename Trig2Lmix>
	auto globalEfficiency(const LeptonList&, unsigned, const Trig2Lmix, Efficiencies&)
		-> std::enable_if_t<Trig2Lmix::is2Lmix(), bool>;
	/// One symmetric dilepton trigger
	template<typename Trig2Lsym>
	auto globalEfficiency(const LeptonList&, unsigned, const Trig2Lsym , Efficiencies&)
		-> std::enable_if_t<Trig2Lsym::is2Lsym(), bool>;
	/// One asymmetric dilepton trigger
	template<typename Trig2Lasym>
	auto globalEfficiency(const LeptonList&, unsigned, const Trig2Lasym, Efficiencies&) 
		-> std::enable_if_t<Trig2Lasym::is2Lasym(), bool>;
	/// One mixed-flavour dilepton trigger + single-lepton triggers
	template<typename Trig2Lmix, typename Trig1L_obj1, typename Trig1L_obj2>
	auto globalEfficiency(const LeptonList&, unsigned, const Trig2Lmix, const flat_set<Trig1L_obj1>&, const flat_set<Trig1L_obj2>&, Efficiencies&)
		-> std::enable_if_t<Trig2Lmix::is2Lmix()
							&& Trig1L_obj1::is1L() && Trig2Lmix::object1()==Trig1L_obj1::object()
							&& Trig1L_obj2::is1L() && Trig2Lmix::object2()==Trig1L_obj2::object(),
			bool>;
	/// One dilepton trigger + one single-lepton trigger
	template<typename Trig2L, typename Trig1L>
	inline auto globalEfficiency(const LeptonList&, unsigned, const Trig2L, const Trig1L, Efficiencies&)
		-> std::enable_if_t<Trig2L::is2Lnomix() 
							&& Trig1L::is1L()
							&& Trig2L::object()==Trig1L::object(),
			bool>;
	/// One symmetric dilepton trigger + several single-lepton triggers
	template<typename Trig2Lsym, typename Trig1L>
	auto globalEfficiency(const LeptonList&, unsigned, const Trig2Lsym, const flat_set<Trig1L>&, Efficiencies&)
		-> std::enable_if_t<Trig2Lsym::is2Lsym() 
							&& Trig1L::is1L() 
							&& Trig1L::object() == Trig2Lsym::object(), 
			bool>;
	/// One asymmetric dilepton trigger + several single-lepton triggers
	template<typename Trig2Lasym, typename Trig1L>
	auto globalEfficiency(const LeptonList&, unsigned, const Trig2Lasym, const flat_set<Trig1L>&, Efficiencies&)
		-> std::enable_if_t<Trig2Lasym::is2Lasym() 
							&& Trig1L::is1L() 
							&& Trig1L::object() == Trig2Lasym::object(), 
			bool>;
	/// Two symmetric dilepton triggers + several single-lepton triggers
	template<typename Trig2Lsym, typename Trig1L> 
	auto globalEfficiency(const LeptonList&, unsigned, const Trig2Lsym, const Trig2Lsym, const flat_set<Trig1L>&, Efficiencies&)
		-> std::enable_if_t<Trig2Lsym::is2Lsym() 
							&& Trig1L::is1L()
							&& Trig1L::object() == Trig2Lsym::object(), 
			bool>;
	/// Two dilepton triggers (one asym., one sym.) + several single-lepton triggers
	template<typename Trig2Lasym, typename Trig2Lsym, typename Trig1L>
	auto globalEfficiency(const LeptonList&, unsigned, const Trig2Lasym, const Trig2Lsym, const flat_set<Trig1L>&, Efficiencies&)
		-> std::enable_if_t<Trig2Lasym::is2Lasym() 
							&& Trig2Lsym::is2Lsym() && Trig2Lsym::object()==Trig2Lasym::object()
							&& Trig1L::is1L() && Trig1L::object()==Trig2Lasym::object(), 
			bool>;
	/// One symmetric trilepton trigger
	template<typename Trig3Lsym>
	auto globalEfficiency(const LeptonList&, unsigned, const Trig3Lsym, Efficiencies&)
		-> std::enable_if_t<Trig3Lsym::is3Lsym(), bool>;
	/// One half-symmetric trilepton trigger
	template<typename Trig3Lhalfsym>
	auto globalEfficiency(const LeptonList&, unsigned, const Trig3Lhalfsym, Efficiencies&)
		-> std::enable_if_t<Trig3Lhalfsym::is3Lhalfsym(), bool>;
	/// One dilepton trigger + one mixed-flavour dilepton trigger
	template<typename Trig2L, typename Trig2Lmix>
	auto globalEfficiency(const LeptonList&, unsigned, const Trig2L, const Trig2Lmix, Efficiencies&)
		-> std::enable_if_t<Trig2L::is2Lnomix()
							&& Trig2Lmix::is2Lmix()
							&& (Trig2Lmix::object1()==Trig2L::object() || Trig2Lmix::object2()==Trig2L::object()),
			bool>;
	/// Combinaisons with 3 dilepton triggers including one mixed-flavour, and one sym./asym. dilepton for each flavour
	template<typename Trig2L_obj1, typename Trig2L_obj2, typename Trig2Lmix>
	auto globalEfficiency(const LeptonList&, unsigned, const Trig2L_obj1, const Trig2L_obj2, const Trig2Lmix, Efficiencies&)
		-> std::enable_if_t<Trig2Lmix::is2Lmix()
							&& Trig2L_obj1::is2Lnomix() && Trig2L_obj1::object() == Trig2Lmix::object1()
							&& Trig2L_obj2::is2Lnomix() && Trig2L_obj2::object() == Trig2Lmix::object2(),
							
			bool>;
	/// Same combinaisons with 3 dilepton triggers, + single-lepton triggers
	template<typename Trig2L_obj1, typename Trig2L_obj2, typename Trig2Lmix, typename Trig1L_obj1, typename Trig1L_obj2>
	auto globalEfficiency(const LeptonList&, unsigned, const Trig2L_obj1, const Trig2L_obj2, const Trig2Lmix, 
			const flat_set<Trig1L_obj1>&, const flat_set<Trig1L_obj2>&, Efficiencies&)
		-> std::enable_if_t<Trig2Lmix::is2Lmix()
							&& Trig2L_obj1::is2Lnomix() && Trig2L_obj1::object()==Trig2Lmix::object1()
							&& Trig2L_obj2::is2Lnomix() && Trig2L_obj2::object()==Trig2Lmix::object2()
							&& Trig1L_obj1::is1L() && Trig1L_obj1::object()==Trig2Lmix::object1()
							&& Trig1L_obj2::is1L() && Trig1L_obj2::object()==Trig2Lmix::object2(),
							
			bool>;
	/// Six dilepton triggers (two mixed-flavour, two sym., two asym./sym.) for two object types
	template<typename Trig2L_obj1, typename Trig2Lsym_obj1, typename Trig2L_obj2, typename Trig2Lsym_obj2,
		typename Trig2Lmix, typename Trig1L_obj1, typename Trig1L_obj2>
	auto globalEfficiency(const LeptonList&, unsigned, const Trig2L_obj1, const Trig2Lsym_obj1, const Trig2L_obj2, const Trig2Lsym_obj2,
			const Trig2Lmix, const Trig2Lmix, const flat_set<Trig1L_obj1>&, const flat_set<Trig1L_obj2>&, Efficiencies&)
		-> std::enable_if_t<Trig2Lmix::is2Lmix()
							&& Trig2L_obj1::is2Lnomix() && Trig2L_obj1::object()==Trig2Lmix::object1()
							&& Trig2L_obj2::is2Lnomix() && Trig2L_obj2::object()==Trig2Lmix::object2()
							&& Trig2Lsym_obj1::is2Lsym() && Trig2Lsym_obj1::object()==Trig2Lmix::object1()
							&& Trig2Lsym_obj2::is2Lsym() && Trig2Lsym_obj2::object()==Trig2Lmix::object2()
							&& Trig1L_obj1::is1L() && Trig1L_obj1::object()==Trig2Lmix::object1()
							&& Trig1L_obj2::is1L() && Trig1L_obj2::object()==Trig2Lmix::object2(),
			bool>;
	/// One mixed-flavour trilepton trigger
	template<typename Trig3Lmix>
	auto globalEfficiency(const LeptonList&, unsigned, const Trig3Lmix, Efficiencies&)
		-> std::enable_if_t<Trig3Lmix::is3Lmix(), bool>;
	/// Two complementary mixed-flavour trilepton triggers
	template<typename Trig3Lmix1, typename Trig3Lmix2>
	auto globalEfficiency(const LeptonList&, unsigned, const Trig3Lmix1, const Trig3Lmix2, Efficiencies&)
		-> std::enable_if_t<Trig3Lmix1::is3Lmix() 
						&& Trig3Lmix2::is3Lmix() 
						&& Trig3Lmix1::object1() == Trig3Lmix2::object2()
						&& Trig3Lmix1::object2() == Trig3Lmix2::object1(), 
			bool>;
	
	bool globalEfficiency_Factorized2(const LeptonList& leptons, unsigned runNumber, GlobEffFunc func1, GlobEffFunc func2, Efficiencies& globalEfficiencies);
	bool globalEfficiency_Factorized3(const LeptonList& leptons, unsigned runNumber, GlobEffFunc func1, GlobEffFunc func2, GlobEffFunc func3, Efficiencies& globalEfficiencies);

	bool globalEfficiency_Toys(const LeptonList&, unsigned, const std::vector<TrigDef>& triggers, Efficiencies&);
	
private:
	class Helper
	{
	public:
		Helper(const std::vector<TrigDef>& defs);
		Helper(Helper&&) = default;

		bool duplicates() const;
		std::function<bool(Calculator*,const LeptonList&,unsigned,Efficiencies&)> m_formula;
		
		bool findAndBindFunction();

	protected:
		std::vector<TrigDef> m_defs;
		unsigned m_n1L = 0, m_n2L = 0, m_n3L = 0;
	
		template<TriggerType object_flag> bool findAndBindFunction();
		template<TriggerType object1_flag, TriggerType object2_flag> bool findAndBindFunction();
		template<typename... Trigs> bool bindFunction();
		
		template<typename Param> auto extract();
		struct NoSuchTrigger {};
		template<typename T> struct Optional{}; /// to decorate the parameters of the findAndBind() function(s)
		template<typename T> struct BindPackedParam;

	};

	friend class CheckConfig;
};

}
#endif
