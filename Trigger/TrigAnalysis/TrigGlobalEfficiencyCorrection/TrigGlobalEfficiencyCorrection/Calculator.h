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
#include <boost/container/container_fwd.hpp>

namespace TrigGlobEffCorr
{

class Lepton;
class Trig1E;
class Trig1MU;
class Trig2Esym;
class Trig2MUsym;
class TrigEMU;

class Calculator : public asg::AsgMessaging
{
	using LeptonList = TrigGlobalEfficiencyCorrectionTool::LeptonList;
	using TrigDef = TrigGlobEffCorr::ImportData::TrigDef;
	template<typename Key> using flat_set = boost::container::flat_set<Key>;

public:
	Calculator(TrigGlobalEfficiencyCorrectionTool& parent, unsigned nPeriodsToReserve);
	bool addPeriod(ImportData& data, const std::pair<unsigned,unsigned>& boundaries, const std::string& combination, 
			bool useToys, bool useDefaultTools, std::size_t& uniqueElectronLeg);
	bool compute(TrigGlobalEfficiencyCorrectionTool& parent, const LeptonList& leptons, unsigned runNumber, Efficiencies& efficiencies);
	
	struct Period
	{
		const std::pair<unsigned,unsigned> m_boundaries;
		std::function<bool(Calculator*,const LeptonList&,unsigned,Efficiencies&)> m_formula;
		Period(const decltype(m_boundaries)& b, decltype(m_formula)&& f) : m_boundaries(b), m_formula(f) {}
	};
	
private:
	TrigGlobalEfficiencyCorrectionTool* m_parent; /// pointer updated at each call to compute() because the parent tool might have been moved in-between
	
	std::vector<Period> m_periods;
	std::map<std::pair<const Lepton*,std::size_t>, Efficiencies> m_cachedEfficiencies;
	
	bool aboveThreshold(const Lepton& p,std::size_t leg) const { return m_parent->aboveThreshold(p, leg); }
	template<typename Trig1L> auto getLoosestLegAboveThreshold(const Lepton& lepton, const flat_set<Trig1L>& trigs, bool& success)
			-> typename std::enable_if<Trig1L::is1L(), std::size_t>::type
		{ return m_parent->getLoosestLegAboveThreshold(lepton, Trig1L::anonymize(trigs), success); }
	Efficiencies getCachedTriggerLegEfficiencies(const Lepton& lepton, unsigned runNumber, std::size_t leg, bool& success);

	
	template<typename Trig1L>
	auto globalEfficiency_One1L(const LeptonList& leptons, unsigned runNumber, const Trig1L trig, Efficiencies& globalEfficiencies)
		-> typename std::enable_if<Trig1L::is1L(), bool>::type;
	bool globalEfficiency_Two1L(const LeptonList& leptons, unsigned runNumber, const Trig1E trig1E, const Trig1MU trig1MU, Efficiencies& globalEfficiencies);
	template<typename Trig1L>
	auto globalEfficiency_Several1L(const LeptonList& leptons, unsigned runNumber, const flat_set<Trig1L>& trigs, Efficiencies& globalEfficiencies)
		-> typename std::enable_if<Trig1L::is1L(), bool>::type;
	bool globalEfficiency_Several1L(const LeptonList& leptons, unsigned runNumber, 
		const flat_set<Trig1E>& trigs1E, const flat_set<Trig1MU>& trigs1MU, Efficiencies& globalEfficiencies);
	//

	bool globalEfficiency_One2L(const LeptonList& leptons, unsigned runNumber, const TrigEMU trig, Efficiencies& globalEfficiencies);
	template<typename Trig2Lsym>
	auto globalEfficiency_One2L(const LeptonList& leptons, unsigned runNumber, const Trig2Lsym trig , Efficiencies& globalEfficiencies)
		-> typename std::enable_if<Trig2Lsym::is2Lsym(), bool>::type;
	template<typename Trig2Lasym>
	auto globalEfficiency_One2L(const LeptonList& leptons, unsigned runNumber, const Trig2Lasym trig, Efficiencies& globalEfficiencies)
		-> typename std::enable_if<Trig2Lasym::is2Lasym(), bool>::type;
	
	
	template<typename Trig2L, typename Trig1L>
	inline auto globalEfficiency_One2LSeveral1L(const LeptonList& leptons, unsigned runNumber, 
			const Trig2L trig2L, const Trig1L trig1L, Efficiencies& globalEfficiencies)
		-> typename std::enable_if<Trig2L::is2L(Trig1L::object()) && Trig1L::is1L(), bool>::type;
	bool globalEfficiency_One2LSeveral1L(const LeptonList& leptons, unsigned runNumber,
		const TrigEMU trigEMU, const flat_set<Trig1E>& trigs1E, const flat_set<Trig1MU>& trigs1MU, Efficiencies& globalEfficiencies);
	template<typename Trig2Lsym, typename Trig1L>
	auto globalEfficiency_One2LSeveral1L(const LeptonList& leptons, unsigned runNumber, 
			const Trig2Lsym trig2L, const flat_set<Trig1L>& trigs1L, Efficiencies& globalEfficiencies)
		-> typename std::enable_if<Trig2Lsym::is2Lsym(Trig1L::object()) && Trig1L::is1L(), bool>::type;
	template<typename Trig2Lasym, typename Trig1L>
	auto globalEfficiency_One2LSeveral1L(const LeptonList& leptons, unsigned runNumber, 
			const Trig2Lasym trig2L, const flat_set<Trig1L>& trigs1L, Efficiencies& globalEfficiencies)
		-> typename std::enable_if<Trig2Lasym::is2Lasym(Trig1L::object()) && Trig1L::is1L(), bool>::type;
	template<typename Trig2E>
	auto globalEfficiency_One2LSeveral1L(const LeptonList& leptons, unsigned runNumber, 
			const Trig2E trig2E, const flat_set<Trig1E>& trigs1E, const flat_set<Trig1MU>& trigs1MU, Efficiencies& globalEfficiencies)
		-> typename std::enable_if<Trig2E::is2L(xAOD::Type::Electron), bool>::type;
	template<typename Trig2MU>
	auto globalEfficiency_One2LSeveral1L(const LeptonList& leptons, unsigned runNumber, 
			const Trig2MU trig2MU, const flat_set<Trig1E>& trigs1E, const flat_set<Trig1MU>& trigs1MU, Efficiencies& globalEfficiencies)
		-> typename std::enable_if<Trig2MU::is2L(xAOD::Type::Muon), bool>::type;
		
	template<typename Trig2L> // not sure this one's actually called by the code...
	auto globalEfficiency_Two2L(const LeptonList& leptons, unsigned runNumber, const Trig2L trig2L, const TrigEMU trigEMU, Efficiencies& globalEfficiencies)
		-> typename std::enable_if<Trig2L::is2L(), bool>::type; 
	template<typename Trig2Lsym, typename Trig1L> 
	auto globalEfficiency_Two2LSeveral1L(const LeptonList& leptons, unsigned runNumber, 
				const Trig2Lsym trig2L1, const Trig2Lsym trig2L2, const flat_set<Trig1L>& trigs1L, Efficiencies& globalEfficiencies)
		-> typename std::enable_if<Trig2Lsym::is2Lsym(Trig1L::object()) && Trig1L::is1L(), bool>::type;
	template<typename Trig2Lasym, typename Trig2Lsym, typename Trig1L>
	auto globalEfficiency_Two2LSeveral1L(const LeptonList& leptons, unsigned runNumber, 
				const Trig2Lasym trig2Lasym, const Trig2Lsym trig2Lsym, const flat_set<Trig1L>& trigs1L, Efficiencies& globalEfficiencies)
		-> typename std::enable_if<Trig2Lasym::is2Lasym(Trig1L::object()) && Trig2Lsym::is2Lsym(Trig1L::object()) && Trig1L::is1L(), bool>::type;
	
	template<typename Trig2E, typename Trig2MU>
	auto globalEfficiency_Three2L(const LeptonList& leptons, unsigned runNumber, 
			const Trig2E trig2E, const Trig2MU trig2MU, const TrigEMU trigEMU, Efficiencies& globalEfficiencies)
		-> typename std::enable_if<Trig2E::is2L(xAOD::Type::Electron) && Trig2MU::is2L(xAOD::Type::Muon), bool>::type;
	template<typename Trig2E, typename Trig2MU>
	auto globalEfficiency_Three2LSeveral1L(const LeptonList& leptons, unsigned runNumber, const Trig2E trig2E, const Trig2MU trig2MU, 
			const TrigEMU trigEMU, const flat_set<Trig1E>& trigs1E, const flat_set<Trig1MU>& trigs1MU, Efficiencies& globalEfficiencies)
		-> typename std::enable_if<Trig2E::is2L(xAOD::Type::Electron) && Trig2MU::is2L(xAOD::Type::Muon), bool>::type;
	
	
	template<typename Trig2L, typename Trig2Lsym, typename Trig1L>
	auto globalEfficiency_Six2LSeveral1L_singleObjectFactor(const LeptonList& leptons, unsigned runNumber, 
			const Trig2L trig2L, const Trig2Lsym trig2Lsym, const TrigEMU trigEMU1, const TrigEMU trigEMU2, const flat_set<Trig1L>& trigs1L, Efficiencies (&efficiencies)[4])
		-> typename std::enable_if<Trig2L::is2L() && Trig2Lsym::is2Lsym(Trig2L::object()) && Trig1L::is1L(Trig2L::object()), bool>::type;
	template<typename Trig2E, typename Trig2MU>
	auto globalEfficiency_Six2LSeveral1L(const LeptonList& leptons, unsigned runNumber, 
			const Trig2E trig2E, const Trig2Esym trig2Esym, const Trig2MU trig2MU, const Trig2MUsym trig2MUsym,
			const TrigEMU trigEMU1, const TrigEMU trigEMU2, const flat_set<Trig1E>& trigs1E, const flat_set<Trig1MU>& trigs1MU, Efficiencies& globalEfficiencies)
		-> typename std::enable_if<Trig2E::is2L(xAOD::Type::Electron) && Trig2MU::is2L(xAOD::Type::Muon), bool>::type;
	
	template<typename Trig3Lsym>
	auto globalEfficiency_One3L(const LeptonList& leptons, unsigned runNumber, const Trig3Lsym trig, Efficiencies& globalEfficiencies)
		-> typename std::enable_if<Trig3Lsym::is3Lsym(), bool>::type;
	template<typename Trig3Lhalfsym>
	auto globalEfficiency_One3L(const LeptonList& leptons, unsigned runNumber, const Trig3Lhalfsym trig, Efficiencies& globalEfficiencies)
		-> typename std::enable_if<Trig3Lhalfsym::is3Lhalfsym(), bool>::type;		
	template<typename Trig3Lmix>
	auto globalEfficiency_One3L(const LeptonList& leptons, unsigned runNumber, const Trig3Lmix trig, Efficiencies& globalEfficiencies)
		-> typename std::enable_if<Trig3Lmix::is3Lmix(), bool>::type;		
	template<typename Trig3Lmix1, typename Trig3Lmix2>
	auto globalEfficiency_Two3L(const LeptonList& leptons, unsigned runNumber, const Trig3Lmix1 trig1, const Trig3Lmix2 trig2, Efficiencies& globalEfficiencies)
		-> typename std::enable_if<Trig3Lmix1::is3Lmix() && Trig3Lmix2::is3Lmix() && (Trig3Lmix1::major_object()!=Trig3Lmix2::major_object()), bool>::type;

	//	
	bool globalEfficiency_Toys(const LeptonList&, unsigned, const std::vector<TrigDef>& triggers, Efficiencies&);
	
private:
	class Helper
	{
	public:
		Helper(const std::vector<TrigDef>& defs);
		const unsigned int n1L, n2L, n3L;
		template<typename Trig1L = void> bool bind_1x1L();
		template<typename Trig2L> bool bind_1x2L();
		template<typename Trig2E, typename Trig2MU> bool bind_3x2L();	
		template<typename Trig2E, typename Trig2MU> bool bind_6x2L();
		template<typename Trig3L> bool bind_1x3L();
		template<typename Trig3L1, typename Trig3L2> bool bind_2x3L();
		bool duplicates() const;
		std::function<bool(Calculator*,const LeptonList&,unsigned,Efficiencies&)> m_func;
	protected:
		const std::vector<TrigDef>& m_defs;
		template<typename Trig> Trig get(unsigned index = 0) const;
		template<typename Trig1L> flat_set<Trig1L> get_all() const;
		template<typename Trig> std::size_t count() const
		{
			return std::count_if(m_defs.cbegin(), m_defs.cend(), 
				[](const TrigDef& td)->bool{return td.type==Trig::type();});
		}
	};
	
	friend class Helper;
	friend class CheckConfig;
};

}
#endif
