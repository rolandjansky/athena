/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

// contact: jmaurer@cern.ch

#include "TrigGlobalEfficiencyCorrection/Calculator.h"
#include "TrigGlobalEfficiencyCorrection/ImportData.h"
#include "TrigGlobalEfficiencyCorrection/Lepton.h"
#include "TrigGlobalEfficiencyCorrection/Trigger.h"
#include "AsgTools/AsgMessaging.h"

#include <random>
#include <iterator>

using namespace TrigGlobEffCorr;
using std::placeholders::_1;
using std::placeholders::_2;
using std::placeholders::_3;
using std::placeholders::_4;

Calculator::Calculator(TrigGlobalEfficiencyCorrectionTool& parent, unsigned nPeriodsToReserve) :
	asg::AsgMessaging(&parent),
	m_parent(&parent)
{
	msg().setLevel(parent.msg().level());
	m_periods.reserve(nPeriodsToReserve);
}
		
bool Calculator::addPeriod(ImportData& data, const std::pair<unsigned,unsigned>& boundaries, const std::string& combination, 
	bool useToys, std::size_t& uniqueElectronLeg, std::size_t& uniquePhotonLeg)
{
	bool success = true;
	m_parent = data.getParent();
	
	auto triggers = data.parseTriggerString(combination, success);
	if(!success) return false;
	if(!triggers.size())
	{
		ATH_MSG_ERROR("The trigger combination is empty!");
		return false;
	}
	
	if(!findUniqueLeg(xAOD::Type::Electron, uniqueElectronLeg, triggers)) return false;
	if(!findUniqueLeg(xAOD::Type::Photon, uniquePhotonLeg, triggers)) return false;
	
	/// Choose the appropriate function to compute efficiencies for this particular trigger combination
	Helper helper(triggers);
	if(helper.duplicates())
	{
		ATH_MSG_ERROR("The following combination of triggers contains duplicates: " << combination);
		return false;
	}
	if(!useToys)
	{	
		success = helper.findAndBindFunction();
		if(!helper.m_formula)
		{
			ATH_MSG_ERROR("This trigger combination is currently not supported with an explicit formula (you may use toys instead, slower): " << combination);
			return false;
		}
	}
	else
	{
		helper.m_formula = std::bind(&Calculator::globalEfficiency_Toys, ::_1, ::_2, ::_3, triggers, ::_4);
	}
	if(success)
	{
		if(data.adaptTriggerListForTriggerMatching(triggers))
		{
			m_periods.emplace_back(boundaries, std::move(helper.m_formula), std::move(triggers));
		}
		else
		{
			if(m_parent->m_validTrigMatchTool) return false;
			m_periods.emplace_back(boundaries, std::move(helper.m_formula));
		}
	}
	else
	{
		ATH_MSG_ERROR("Unspecified error occurred while trying to find the formula for the trigger combination " << combination);
	}
	return success;
}

bool Calculator::findUniqueLeg(xAOD::Type::ObjectType obj, std::size_t& uniqueLeg, const std::vector<TrigDef>& defs)
{
	if(uniqueLeg) return true; /// initial non-zero value means that ListOfLegsPerTool is filled
	for(auto& def : defs)
	{
		TriggerProperties tp(def);
		for(auto itr=tp.cbegin(obj);itr!=tp.cend(obj);++itr)
		{
			if(uniqueLeg && uniqueLeg!=*itr)
			{
				ATH_MSG_ERROR("The property 'ListOfLegsPerTool' needs to be filled as the specified trigger combination involves several electron (or photon) trigger legs");
				return false;
			}
			uniqueLeg = *itr;
		}
	}
	return true;
}

const Calculator::Period* Calculator::getPeriod(unsigned runNumber) const
{
	auto period = std::find_if(m_periods.cbegin(), m_periods.cend(),
		[=](const Period& p) { return runNumber>=p.m_boundaries.first && runNumber<=p.m_boundaries.second; });
	if(period == m_periods.end())
	{
		ATH_MSG_ERROR("No trigger combination has been specified for run " << runNumber);
		return nullptr;
	}
	return &*period;
}

bool Calculator::compute(TrigGlobalEfficiencyCorrectionTool& parent, const LeptonList& leptons, unsigned runNumber, Efficiencies& efficiencies)
{
	m_parent = &parent;
	auto period = getPeriod(runNumber);
	if(!period) return false;
	m_cachedEfficiencies.clear();
	return period->m_formula && period->m_formula(this, leptons, runNumber, efficiencies);
}

bool Calculator::checkTriggerMatching(TrigGlobalEfficiencyCorrectionTool& parent, bool& matched, const LeptonList& leptons, unsigned runNumber)
{
	matched = false;
	m_parent = &parent;
	auto period = getPeriod(runNumber);
	if(!period) return false;
	if(!period->m_triggers.size())
	{
		ATH_MSG_ERROR("Empty list of triggers for run number " << runNumber);
		return false;
	}
	auto& trigMatchTool = m_parent->m_trigMatchTool;
	
	/// First, for each lepton, list the trigger leg(s) it is allowed to fire (depends on pT and selection tags)
	const unsigned nLep = leptons.size();
	std::vector<flat_set<std::size_t> > validLegs(leptons.size());
	for(unsigned i=0;i<nLep;++i)
	{
		if(!fillListOfLegsFor(leptons[i], period->m_triggers, validLegs[i])) return false;
	}
	
	/// Then for each trigger, call trigger matching tool for all possible (valid) lepton combinations
	std::vector<flat_set<std::size_t> > firedLegs;
	std::vector<const xAOD::IParticle*> trigLeptons;
	const std::size_t magicWordHLT = 0xf7b8b87ef2917d66;

	for(auto& trig : period->m_triggers)
	{
		/// Get trigger chain name with a "HLT_" prefix
		auto itr = m_parent->m_dictionary.find(trig.name ^ magicWordHLT);
		if(itr == m_parent->m_dictionary.end())
		{
			itr = m_parent->m_dictionary.emplace(trig.name ^ magicWordHLT, "HLT_" + m_parent->m_dictionary.at(trig.name)).first;
		}
		const std::string& chain = itr->second;
		
		unsigned nLegs = 0;
		if(trig.type & TT_SINGLELEPTON_FLAG) nLegs = 1;
		else if(trig.type & TT_DILEPTON_FLAG) nLegs = 2;
		else if(trig.type & TT_TRILEPTON_FLAG) nLegs = 3;
		else
		{
			ATH_MSG_ERROR("Unsupported trigger (type = " << std::hex << trig.type << std::dec << ") " << chain );
			return false;
		}
		firedLegs.resize(nLegs);
		trigLeptons.resize(nLegs);
		for(unsigned i0=0;i0<nLep;++i0)
		{
			firedLegs[0].swap(validLegs[i0]); /// borrow the set of legs that can be fired by that lepton
			trigLeptons[0] = leptons[i0].particle();
			if(nLegs == 1)
			{
				if(canTriggerBeFired(trig, firedLegs) /// enough lepton(s) on trigger plateau?
					&& trigMatchTool->match(trigLeptons, chain)) return (matched = true);
			}
			else for(unsigned i1=i0+1;i1<nLep;++i1)
			{
				firedLegs[1].swap(validLegs[i1]);
				trigLeptons[1] = leptons[i1].particle();
				if(nLegs == 2)
				{
					if(canTriggerBeFired(trig, firedLegs)
						&& trigMatchTool->match(trigLeptons, chain)) return (matched = true);
				}
				else for(unsigned i2=i1+1;i2<nLep;++i2)
				{
					firedLegs[2].swap(validLegs[i2]);
					trigLeptons[2] = leptons[i2].particle();
					if(canTriggerBeFired(trig, firedLegs)
						&& trigMatchTool->match(trigLeptons, chain)) return (matched = true);
					firedLegs[2].swap(validLegs[i2]);
				}
				firedLegs[1].swap(validLegs[i1]);
			}
			firedLegs[0].swap(validLegs[i0]); /// return the set of legs back to the main container
		}
	}
	return true;
}

bool Calculator::getRelevantTriggersForUser(TrigGlobalEfficiencyCorrectionTool& parent, std::vector<std::string>& triggers, unsigned runNumber)
{
	triggers.clear();
	m_parent = &parent;
	auto period = getPeriod(runNumber);
	if(!period) return false;
	if(!period->m_triggers.size())
	{
		ATH_MSG_ERROR("Empty list of triggers for run number " << runNumber << " (was there a configuration issue? please check for warnings during initialization)");
		return false;
	}
	bool success = true;
	auto notfound = parent.m_dictionary.end();
	for(auto& trig : period->m_triggers)
	{
		auto itr = parent.m_dictionary.find(trig.name);
		if(itr == notfound)
		{
			ATH_MSG_ERROR("can't retrieve name of trigger with hash " << trig.name << " (shouldn't happen; contact tool developers!)");
			success = false;
		}
		else triggers.push_back(itr->second);
	}
	if(!success) triggers.clear();
	return success;
}

Efficiencies Calculator::getCachedTriggerLegEfficiencies(const Lepton& lepton, unsigned runNumber, std::size_t leg, bool& success)
{
	auto insertion = m_cachedEfficiencies.emplace(std::make_pair(&lepton, leg), Efficiencies());
	Efficiencies& efficiencies = insertion.first->second;
	if(insertion.second)
	{
		bool cpSuccess = false;
		switch(lepton.type())
		{
		case xAOD::Type::Electron:
			cpSuccess = m_parent->getTriggerLegEfficiencies(lepton.electron(), runNumber, leg, lepton.tag(), efficiencies);
			break;
		case xAOD::Type::Muon:
			cpSuccess = m_parent->getTriggerLegEfficiencies(lepton.muon(), leg, lepton.tag(), efficiencies);
			break;
		case xAOD::Type::Photon:
			cpSuccess = m_parent->getTriggerLegEfficiencies(lepton.photon(), runNumber, leg, lepton.tag(), efficiencies);
			break;
		default: ATH_MSG_ERROR("Unsupported particle type");
		}
		if(!cpSuccess)
		{
			efficiencies.data() = -777.;
			efficiencies.mc() = -777.;
			success = false;
		}
	}
	if(efficiencies.mc()==-777.) success = false;
	return efficiencies;
}

///
/// One single-lepton trigger
///
template<typename Trig1L>
auto Calculator::globalEfficiency(const LeptonList& leptons, unsigned runNumber, const Trig1L trig, Efficiencies& globalEfficiencies)
	-> std::enable_if_t<Trig1L::is1L(), bool>
{
	ATH_MSG_DEBUG("Entered Calculator::globalEfficiency_One1L() at line " << __LINE__);
	if(!trig)
	{
		globalEfficiencies = {0.};
		return true;
	}
	globalEfficiencies = {1.};
	bool success = true;
	for(auto& lepton : leptons)
	{
		if(trig.irrelevantFor(lepton) || !aboveThreshold(lepton, trig())) continue;
		auto efficiencies = getCachedTriggerLegEfficiencies(lepton, runNumber, trig(), success);
		globalEfficiencies *= ~efficiencies;
	}
	globalEfficiencies = ~globalEfficiencies;
	return success;
}

///
/// Two single-lepton triggers, two object types
///
template<typename Trig1L_obj1, typename Trig1L_obj2>
auto Calculator::globalEfficiency(const LeptonList& leptons, unsigned runNumber, const Trig1L_obj1 trig1, const Trig1L_obj2 trig2, Efficiencies& globalEfficiencies)
	-> std::enable_if_t<Trig1L_obj1::is1L()
						&& Trig1L_obj2::is1L()
						&& Trig1L_obj1::object() != Trig1L_obj2::object(),
		bool>
{
	ATH_MSG_DEBUG("Entered Calculator::globalEfficiency_Two1L() at line " << __LINE__);
	if(!trig1) return globalEfficiency(leptons, runNumber, trig2, globalEfficiencies);
	if(!trig2) return globalEfficiency(leptons, runNumber, trig1, globalEfficiencies);
	globalEfficiencies = {1.};
	bool success = true;
	for(auto& lepton : leptons)
	{
		std::size_t leg;
		if(trig1.relevantFor(lepton)) leg = trig1();
		else if(trig2.relevantFor(lepton)) leg = trig2();
		else continue;
		if(!aboveThreshold(lepton, leg)) continue;
		auto efficiencies = getCachedTriggerLegEfficiencies(lepton, runNumber, leg, success);
		globalEfficiencies *= ~efficiencies;
	}
	globalEfficiencies = ~globalEfficiencies;
	return success;
}

///
/// Several single-lepton triggers, one object type
///
template<typename Trig1L>
auto Calculator::globalEfficiency(const LeptonList& leptons, unsigned runNumber, const flat_set<Trig1L>& trigs, Efficiencies& globalEfficiencies)
	-> std::enable_if_t<Trig1L::is1L(), bool>
{
	ATH_MSG_DEBUG("Entered Calculator::globalEfficiency_Several1L() at line " << __LINE__);
	if(trigs.size() == 1) return globalEfficiency(leptons, runNumber, *trigs.cbegin(), globalEfficiencies);
	if(!trigs.size())
	{
		globalEfficiencies = {0.};
		return true;
	}
	globalEfficiencies = {1.};
	bool success = true;
	for(auto& lepton : leptons)
	{
		if(Trig1L::irrelevantFor(lepton)) continue;
		std::size_t loosestLeg = getLoosestLegAboveThreshold(lepton, trigs, success);
		if(loosestLeg && success)
		{
			auto efficiencies = getCachedTriggerLegEfficiencies(lepton, runNumber, loosestLeg, success);
			globalEfficiencies *= ~efficiencies;
		}
	}
	globalEfficiencies = ~globalEfficiencies;
	return success;
}

///
/// Several single-lepton triggers, two object types
///
template<typename Trig1L_obj1, typename Trig1L_obj2>
auto Calculator::globalEfficiency(const LeptonList& leptons, unsigned runNumber,
			const flat_set<Trig1L_obj1>& trigs1, const flat_set<Trig1L_obj2>& trigs2, Efficiencies& globalEfficiencies)
	-> std::enable_if_t<Trig1L_obj1::is1L()
						&& Trig1L_obj2::is1L()
						&& Trig1L_obj1::object() != Trig1L_obj2::object(),
		bool>
{
	ATH_MSG_DEBUG("Entered Calculator::globalEfficiency_Several1L() at line " << __LINE__);
	if(trigs1.size()==1 && trigs2.size()==1)
	{
		return globalEfficiency(leptons, runNumber, *trigs1.cbegin(), *trigs2.cbegin(), globalEfficiencies);
	}
	if(!trigs1.size()) return globalEfficiency(leptons, runNumber, trigs2, globalEfficiencies);
	if(!trigs2.size()) return globalEfficiency(leptons, runNumber, trigs1, globalEfficiencies);
	globalEfficiencies = {1.};
	bool success = true;
	for(auto& lepton : leptons)
	{
		std::size_t loosestLeg;
		if(Trig1L_obj1::relevantFor(lepton)) loosestLeg = getLoosestLegAboveThreshold(lepton, trigs2, success);
		else if(Trig1L_obj2::relevantFor(lepton)) loosestLeg = getLoosestLegAboveThreshold(lepton, trigs2, success);
		else continue;
		if(loosestLeg && success)
		{
			auto efficiencies = getCachedTriggerLegEfficiencies(lepton, runNumber, loosestLeg, success);
			globalEfficiencies *= ~efficiencies;
		}
	}
	globalEfficiencies = ~globalEfficiencies;
	return success;
}

///
/// One mixed-flavour dilepton trigger
///
template<typename Trig2Lmix>
auto Calculator::globalEfficiency(const LeptonList& leptons, unsigned runNumber, const Trig2Lmix trig, Efficiencies& globalEfficiencies)
	-> std::enable_if_t<Trig2Lmix::is2Lmix(), bool>
{
	ATH_MSG_DEBUG("Entered Calculator::globalEfficiency_One2L() at line " << __LINE__);
	Efficiencies efficiencies[2];
	bool success = globalEfficiency(leptons, runNumber, trig.side1(), efficiencies[0])
		&& globalEfficiency(leptons, runNumber, trig.side2(), efficiencies[1]);
	if(success) globalEfficiencies = efficiencies[0] * efficiencies[1];
	else globalEfficiencies = {0.};
	return success;
}

///
/// One symmetric dilepton trigger
///
template<typename Trig2Lsym>
auto Calculator::globalEfficiency(const LeptonList& leptons, unsigned runNumber, const Trig2Lsym trig , Efficiencies& globalEfficiencies)
	-> std::enable_if_t<Trig2Lsym::is2Lsym(), bool>
{
	ATH_MSG_DEBUG("Entered Calculator::globalEfficiency_One2L() at line " << __LINE__);
	globalEfficiencies = {0.};
	if(!trig) return true;
	Efficiencies singleInefficiencies(1.);
	bool success = true;
	int nt = 0;
	for(auto& lepton : leptons)
	{
		if(trig.irrelevantFor(lepton) || !aboveThreshold(lepton, trig())) continue;
		++nt;
		auto efficiencies = getCachedTriggerLegEfficiencies(lepton, runNumber, trig(), success);
		globalEfficiencies = ~efficiencies*globalEfficiencies + efficiencies*~singleInefficiencies;
		singleInefficiencies *= ~efficiencies;
	}
	return success;
}

///
/// One asymmetric dilepton trigger
///
template<typename Trig2Lasym>
auto Calculator::globalEfficiency(const LeptonList& leptons, unsigned runNumber, const Trig2Lasym trig, Efficiencies& globalEfficiencies)
	-> std::enable_if_t<Trig2Lasym::is2Lasym(), bool>
{
	ATH_MSG_DEBUG("Entered Calculator::globalEfficiency_One2L() at line " << __LINE__);
	if(trig.symmetric()) return globalEfficiency(leptons, runNumber, trig.to_symmetric(), globalEfficiencies);
	globalEfficiencies = {0.};
	if(!trig) return true;
	Efficiencies singleInefficiencies[2] = {{1.},{1.}}, twoSingleInefficiencies = {1.};
	bool success = true;
	for(auto& lepton : leptons)
	{
		if(trig.irrelevantFor(lepton)) continue;
		Efficiencies efficiencies[2] = {{0.}, {0.}};
		int loosest = 0;
		if(aboveThreshold(lepton, trig(0)))
		{
			efficiencies[0] = getCachedTriggerLegEfficiencies(lepton, runNumber, trig(0), success);
			if(aboveThreshold(lepton, trig(1)))
			{
				efficiencies[1] = getCachedTriggerLegEfficiencies(lepton, runNumber, trig(1), success);
				loosest = m_parent->getLoosestLeg(lepton, trig(0), trig(1), success)==trig(1);
			}
		}
		else if(aboveThreshold(lepton, trig(1)))
		{
			efficiencies[1] = getCachedTriggerLegEfficiencies(lepton, runNumber, trig(1), success);
			loosest = 1;
		}
		else continue;
		const int tightest = 1 - loosest;
		globalEfficiencies = ~efficiencies[loosest]*globalEfficiencies 
			+ (efficiencies[loosest]-efficiencies[tightest])*~singleInefficiencies[tightest]
			+ efficiencies[tightest]*~twoSingleInefficiencies;
		twoSingleInefficiencies *= ~efficiencies[loosest];
		for(int i=0;i<2;++i) singleInefficiencies[i] *= ~efficiencies[i];
	}
	return success;
}

///
/// One mixed-flavour dilepton trigger + single-lepton triggers
///
template<typename Trig2Lmix, typename Trig1L_obj1, typename Trig1L_obj2>
auto Calculator::globalEfficiency(const LeptonList& leptons, unsigned runNumber,
		const Trig2Lmix trig2Lmix, const flat_set<Trig1L_obj1>& trigs1L1, const flat_set<Trig1L_obj2>& trigs1L2, Efficiencies& globalEfficiencies)
	-> std::enable_if_t<Trig2Lmix::is2Lmix()
						&& Trig1L_obj1::is1L() && Trig2Lmix::object1()==Trig1L_obj1::object()
						&& Trig1L_obj2::is1L() && Trig2Lmix::object2()==Trig1L_obj2::object(),
		bool>
{
	ATH_MSG_DEBUG("Entered Calculator::globalEfficiency_One2LSeveral1L() at line " << __LINE__);
	if(!(trigs1L1.size() + trigs1L2.size()))
		return globalEfficiency(leptons, runNumber, trig2Lmix, globalEfficiencies);
	if(trig2Lmix.hiddenBy(trigs1L1) || trig2Lmix.hiddenBy(trigs1L2))
		return globalEfficiency(leptons, runNumber, trigs1L1, trigs1L2, globalEfficiencies);
	Efficiencies efficiencies[4];
	bool success = globalEfficiency(leptons, runNumber, trigs1L1, efficiencies[0])
		&& globalEfficiency(leptons, runNumber, trigs1L2, efficiencies[1])
		&& globalEfficiency(leptons, runNumber, trig2Lmix.addTo(trigs1L1), efficiencies[2])
		&& globalEfficiency(leptons, runNumber, trig2Lmix.addTo(trigs1L2), efficiencies[3]);
	if(success)
	{
		globalEfficiencies = Efficiencies(1.) - ~efficiencies[0]*~efficiencies[1]
			+ (efficiencies[2]-efficiencies[0])*(efficiencies[3]-efficiencies[1]);
	}
	else globalEfficiencies = {0.};
	return success;
}

///
/// One dilepton trigger + one single-lepton trigger
///
template<typename Trig2L, typename Trig1L>
inline auto Calculator::globalEfficiency(const LeptonList& leptons, unsigned runNumber,
			const Trig2L trig2L, const Trig1L trig1L, Efficiencies& globalEfficiencies)
	-> std::enable_if_t<Trig2L::is2Lnomix() 
						&& Trig1L::is1L()
						&& Trig2L::object()==Trig1L::object(),
		bool>
{
	ATH_MSG_DEBUG("Entered Calculator::globalEfficiency_One2LSeveral1L() at line " << __LINE__);
	return globalEfficiency(leptons, runNumber, trig2L, flat_set<Trig1L>{trig1L}, globalEfficiencies);
}

///
/// One symmetric dilepton trigger + several single-lepton triggers
///
template<typename Trig2Lsym, typename Trig1L>
auto Calculator::globalEfficiency(const LeptonList& leptons, unsigned runNumber,
			const Trig2Lsym trig2L, const flat_set<Trig1L>& trigs1L, Efficiencies& globalEfficiencies)
	-> std::enable_if_t<Trig2Lsym::is2Lsym() 
						&& Trig1L::is1L() 
						&& Trig1L::object() == Trig2Lsym::object(), 
		bool>
{
	ATH_MSG_DEBUG("Entered Calculator::globalEfficiency_One2LSeveral1L() at line " << __LINE__);
	if(!trigs1L.size()) return globalEfficiency(leptons, runNumber, trig2L, globalEfficiencies);
	if(!trig2L || trig2L.hiddenBy(trigs1L)) return globalEfficiency(leptons, runNumber, trigs1L, globalEfficiencies);
	globalEfficiencies = {0.};
	Efficiencies twoSingleInefficiencies = {1.};
	bool success = true;
	for(auto& lepton : leptons)
	{
		if(trig2L.irrelevantFor(lepton)) continue;
		Efficiencies efficiencies1L(0.), efficiencies2L(0.);
		const Efficiencies* loosestLegEfficiency;
		std::size_t loosest1lepLeg = getLoosestLegAboveThreshold(lepton, trigs1L, success);
		if(loosest1lepLeg)
		{
			efficiencies1L = getCachedTriggerLegEfficiencies(lepton, runNumber, loosest1lepLeg, success);
			if(aboveThreshold(lepton, trig2L()))
			{
				efficiencies2L = getCachedTriggerLegEfficiencies(lepton, runNumber, trig2L(), success);
				loosestLegEfficiency = (m_parent->getLoosestLeg(lepton, trig2L(), loosest1lepLeg , success)==trig2L())? &efficiencies2L : &efficiencies1L;
			}
			else loosestLegEfficiency = &efficiencies1L;
		}
		else if(aboveThreshold(lepton, trig2L()))
		{
			efficiencies2L = getCachedTriggerLegEfficiencies(lepton, runNumber, trig2L(), success);
			loosestLegEfficiency = &efficiencies2L;
		}
		else continue;
		globalEfficiencies = ~(*loosestLegEfficiency)*globalEfficiencies + efficiencies1L;
		if(loosestLegEfficiency==&efficiencies2L) globalEfficiencies += ~twoSingleInefficiencies*(efficiencies2L - efficiencies1L);			
		twoSingleInefficiencies *= ~(*loosestLegEfficiency);
	}
	return success;
}

///
/// One asymmetric dilepton trigger + several single-lepton triggers
///
template<typename Trig2Lasym, typename Trig1L>
auto Calculator::globalEfficiency(const LeptonList& leptons, unsigned runNumber,
			const Trig2Lasym trig2L, const flat_set<Trig1L>& trigs1L, Efficiencies& globalEfficiencies)
	-> std::enable_if_t<Trig2Lasym::is2Lasym() 
						&& Trig1L::is1L() 
						&& Trig1L::object() == Trig2Lasym::object(), 
		bool>
{
	ATH_MSG_DEBUG("Entered Calculator::globalEfficiency_One2LSeveral1L() at line " << __LINE__);
	if(trig2L.symmetric()) return globalEfficiency(leptons, runNumber, trig2L.to_symmetric(), trigs1L, globalEfficiencies);
	if(!trigs1L.size()) return globalEfficiency(leptons, runNumber, trig2L, globalEfficiencies);
	if(!trig2L || trig2L.hiddenBy(trigs1L)) return globalEfficiency(leptons, runNumber, trigs1L, globalEfficiencies);
	globalEfficiencies = {0.};
	Efficiencies twoSingleInefficiencies[2] = {{1.}, {1.}}, threeSingleInefficiencies = {1.};
	bool success = true;
	for(auto& lepton : leptons)
	{
		if(trig2L.irrelevantFor(lepton)) continue;
		flat_set<std::size_t> validLegs;
		for(std::size_t leg : trig2L.legs) if(aboveThreshold(lepton, leg)) validLegs.insert(leg);
		Efficiencies efficiencies1L = {0.};
		std::size_t loosest1lepLeg = getLoosestLegAboveThreshold(lepton, trigs1L, success);
		if(loosest1lepLeg)
		{
			efficiencies1L = getCachedTriggerLegEfficiencies(lepton, runNumber, loosest1lepLeg, success);
			validLegs.insert(loosest1lepLeg);
		}
		if(!validLegs.size()) continue;
		auto looseLegs = m_parent->getTwoLoosestLegs(lepton,validLegs,success);
		auto efficienciesLoose = (looseLegs.first==loosest1lepLeg)? efficiencies1L : getCachedTriggerLegEfficiencies(lepton, runNumber, looseLegs.first, success);
		Efficiencies efficienciesMedium = {0.};
		if(validLegs.size()>=2)
			efficienciesMedium = (looseLegs.second==loosest1lepLeg)? efficiencies1L : getCachedTriggerLegEfficiencies(lepton, runNumber, looseLegs.second, success);
		globalEfficiencies = ~efficienciesLoose*globalEfficiencies + efficiencies1L;
		if(loosest1lepLeg!=looseLegs.first)
		{
			globalEfficiencies += (efficienciesLoose-efficienciesMedium)*~twoSingleInefficiencies[looseLegs.first==trig2L.legs[0]];
			if(loosest1lepLeg!=looseLegs.second) globalEfficiencies += (efficienciesMedium-efficiencies1L)*~threeSingleInefficiencies;
		}
		threeSingleInefficiencies *= ~efficienciesLoose;
		twoSingleInefficiencies[0] *= (looseLegs.first!=trig2L.legs[1])?~efficienciesLoose:~efficienciesMedium; /// S1 v S3
		twoSingleInefficiencies[1] *= (looseLegs.first!=trig2L.legs[0])?~efficienciesLoose:~efficienciesMedium; /// S2 v S3
	}
	return success;
}

///
/// Two symmetric dilepton triggers + several single-lepton triggers
///
template<typename Trig2Lsym, typename Trig1L> 
auto Calculator::globalEfficiency(const LeptonList& leptons, unsigned runNumber,
			const Trig2Lsym trig2L1, const Trig2Lsym trig2L2, const flat_set<Trig1L>& trigs1L, Efficiencies& globalEfficiencies)
	-> std::enable_if_t<Trig2Lsym::is2Lsym() 
						&& Trig1L::is1L()
						&& Trig1L::object() == Trig2Lsym::object(), 
		bool>
{
	ATH_MSG_DEBUG("Entered Calculator::globalEfficiency_Two2LSeveral1L() at line " << __LINE__);
	if(!trig2L1 || trig2L1==trig2L2 || trig2L1.hiddenBy(trigs1L)) return globalEfficiency(leptons, runNumber, trig2L2, trigs1L, globalEfficiencies);
	if(!trig2L2 || trig2L2.hiddenBy(trigs1L)) return globalEfficiency(leptons, runNumber, trig2L1, trigs1L, globalEfficiencies);
	globalEfficiencies = {0.};
	Efficiencies singleInefficiencies{1.};
	Efficiencies efficiencies2Lsym[2] = {{0.},{0.}};
	
	bool success = true;
	for(auto& lepton : leptons)
	{
		if(trig2L1.irrelevantFor(lepton)) continue;
		flat_set<std::size_t> validLegs;
		std::map<std::size_t, Efficiencies> efficiencies{{0,0.}};
		std::size_t loosest1lepLeg = getLoosestLegAboveThreshold(lepton, trigs1L, success);
		for(std::size_t leg : {loosest1lepLeg, trig2L1(), trig2L2()})
		{
			if(leg && aboveThreshold(lepton, leg))
			{
				validLegs.insert(leg);
				efficiencies.emplace(leg, getCachedTriggerLegEfficiencies(lepton, runNumber, leg, success));
			}
			else efficiencies.emplace(leg, 0.);
		}
		if(!validLegs.size()) continue;
		auto looseLegs = m_parent->getTwoLoosestLegs(lepton, validLegs, success);
		std::size_t lambda13 = (looseLegs.first!=trig2L2())? looseLegs.first : looseLegs.second;
		std::size_t lambda23 = (looseLegs.first!=trig2L1())? looseLegs.first : looseLegs.second;
		globalEfficiencies = globalEfficiencies*~efficiencies[looseLegs.first] + efficiencies[loosest1lepLeg];
		if(looseLegs.first==trig2L1()) globalEfficiencies += efficiencies2Lsym[1]*(efficiencies[trig2L1()] - efficiencies[looseLegs.second]);
		if(looseLegs.first==trig2L2()) globalEfficiencies += efficiencies2Lsym[0]*(efficiencies[trig2L2()] - efficiencies[looseLegs.second]);
		if(looseLegs.first!=loosest1lepLeg) globalEfficiencies += ~singleInefficiencies*(efficiencies[looseLegs.second] - efficiencies[loosest1lepLeg]);
		efficiencies2Lsym[0] = ~efficiencies[looseLegs.first]*efficiencies2Lsym[0] + efficiencies[lambda23];
		efficiencies2Lsym[1] = ~efficiencies[looseLegs.first]*efficiencies2Lsym[1] + efficiencies[lambda13];
		if(looseLegs.first==trig2L1()) efficiencies2Lsym[0] += (efficiencies[trig2L1()]-efficiencies[lambda23])*~singleInefficiencies;
		if(looseLegs.first==trig2L2()) efficiencies2Lsym[1] += (efficiencies[trig2L2()]-efficiencies[lambda13])*~singleInefficiencies;
		singleInefficiencies *= ~efficiencies[looseLegs.first];
	}
	return success;
}

///
/// Two dilepton triggers (one asym., one sym.) + several single-lepton triggers
///
template<typename Trig2Lasym, typename Trig2Lsym, typename Trig1L>
auto Calculator::globalEfficiency(const LeptonList& leptons, unsigned runNumber,
			const Trig2Lasym trig2Lasym, const Trig2Lsym trig2Lsym, const flat_set<Trig1L>& trigs1L, Efficiencies& globalEfficiencies)
	-> std::enable_if_t<Trig2Lasym::is2Lasym() 
						&& Trig2Lsym::is2Lsym() && Trig2Lsym::object()==Trig2Lasym::object()
						&& Trig1L::is1L() && Trig1L::object()==Trig2Lasym::object(), 
		bool>
{
	ATH_MSG_DEBUG("Entered Calculator::globalEfficiency_Two2LSeveral1L() at line " << __LINE__);
	if(!trig2Lasym || trig2Lasym.hiddenBy(trigs1L)) return globalEfficiency(leptons, runNumber, trig2Lsym, trigs1L, globalEfficiencies);
	if(!trig2Lsym || trig2Lsym.hiddenBy(trigs1L)) return globalEfficiency(leptons, runNumber, trig2Lasym, trigs1L, globalEfficiencies);
	if(trig2Lasym(0)==trig2Lsym() || trig2Lasym(1)==trig2Lsym())
	{
		ATH_MSG_ERROR("implementation -- does this function work properly when the two 2L triggers have one leg in common? Must be checked");
		return false;
	}
	if(trig2Lasym.symmetric()) return globalEfficiency(leptons, runNumber, trig2Lasym.to_symmetric(), trig2Lsym, trigs1L, globalEfficiencies);
	globalEfficiencies = {0.};
	Efficiencies singleInefficiencies[3] = {{1.}, {1.}, {1.}};
	Efficiencies efficiencies2Lasym {0.}, efficiencies2Lsym[3] = {{0.},{0.},{0.}};
	
	bool success = true;
	for(auto& lepton : leptons)
	{
		if(trig2Lasym.irrelevantFor(lepton)) continue;
		flat_set<std::size_t> validLegs;
		std::map<std::size_t, Efficiencies> efficiencies{{0,0.}};
		std::size_t loosest1lepLeg = getLoosestLegAboveThreshold(lepton, trigs1L, success);
		for(std::size_t leg : {trig2Lasym(0), trig2Lasym(1), trig2Lsym(), loosest1lepLeg})
		{
			if(leg && aboveThreshold(lepton, leg))
			{
				validLegs.insert(leg);
				efficiencies.emplace(leg, getCachedTriggerLegEfficiencies(lepton, runNumber, leg, success));
			}
			else efficiencies.emplace(leg, 0.);
		}
		if(!validLegs.size()) continue;
		
		const auto sortedLegs = m_parent->getSortedLegs(lepton, validLegs, success);
		if(!success) return false;
		std::size_t loosestLeg = sortedLegs[0];
		std::size_t secondLoosestLeg = validLegs.size()>=2 ? sortedLegs[1] : 0;
		std::size_t secondTightestLeg = validLegs.size()>=3 ? sortedLegs[2] : 0;
		std::size_t tightestLeg = validLegs.size()>=4 ? sortedLegs[3]: 0;
		std::size_t lambda124 = (loosestLeg!=trig2Lasym(1))? loosestLeg : secondLoosestLeg;
		std::size_t lambda134 = (loosestLeg!=trig2Lasym(0))? loosestLeg : secondLoosestLeg;
		std::size_t lambda234 = (loosestLeg!=trig2Lsym())? loosestLeg : secondLoosestLeg;
		std::size_t lambda14 = (lambda124!=trig2Lasym(0))? lambda124 : (lambda134!=trig2Lasym(1))? lambda134 : secondTightestLeg;
		std::size_t lambda24 = (lambda124!=trig2Lsym())? lambda124 : (lambda234!=trig2Lasym(1))? lambda234 : secondTightestLeg;
		std::size_t lambda34 = (lambda134!=trig2Lsym())? lambda134 : (lambda234!=trig2Lasym(0))? lambda234 : secondTightestLeg;
		std::size_t tau13=0, tau12=0, tau23=0;
		if(loosestLeg==trig2Lsym() || loosestLeg==trig2Lasym(0)) tau12 = (loosestLeg==trig2Lsym())? trig2Lasym(0) : trig2Lsym();
		else if(secondLoosestLeg==trig2Lsym() || secondLoosestLeg==trig2Lasym(0)) tau12 = (secondLoosestLeg==trig2Lsym())? trig2Lasym(0) : trig2Lsym();
		else if(secondTightestLeg==trig2Lsym() || secondTightestLeg==trig2Lasym(0)) tau12 = (secondTightestLeg==trig2Lsym())? trig2Lasym(0) : trig2Lsym();
		else if(tightestLeg==trig2Lsym() || tightestLeg==trig2Lasym(0)) tau12 = (tightestLeg==trig2Lsym())? trig2Lasym(0) : trig2Lsym();
		if(loosestLeg==trig2Lsym() || loosestLeg==trig2Lasym(1)) tau13 = (loosestLeg==trig2Lsym())? trig2Lasym(1) : trig2Lsym();
		else if(secondLoosestLeg==trig2Lsym() || secondLoosestLeg==trig2Lasym(1)) tau13 = (secondLoosestLeg==trig2Lsym())? trig2Lasym(1) : trig2Lsym();
		else if(secondTightestLeg==trig2Lsym() || secondTightestLeg==trig2Lasym(1)) tau13 = (secondTightestLeg==trig2Lsym())? trig2Lasym(1) : trig2Lsym();
		else if(tightestLeg==trig2Lsym() || tightestLeg==trig2Lasym(1)) tau13 = (tightestLeg==trig2Lsym())? trig2Lasym(1) : trig2Lsym();
		if(loosestLeg==trig2Lasym(0) || loosestLeg==trig2Lasym(1)) tau23 = (loosestLeg==trig2Lasym(0))? trig2Lasym(1) : trig2Lasym(0);
		else if(secondLoosestLeg==trig2Lasym(0) || secondLoosestLeg==trig2Lasym(1)) tau23 = (secondLoosestLeg==trig2Lasym(0))? trig2Lasym(1) : trig2Lasym(0);
		else if(secondTightestLeg==trig2Lasym(0) || secondTightestLeg==trig2Lasym(1)) tau23 = (secondTightestLeg==trig2Lasym(0))? trig2Lasym(1) : trig2Lasym(0);
		else if(tightestLeg==trig2Lasym(0) || tightestLeg==trig2Lasym(1)) tau23 = (tightestLeg==trig2Lasym(0))? trig2Lasym(1) : trig2Lasym(0);

		/// can't use tightestLeg==trig2Lsym because it might also be 0
		globalEfficiencies = globalEfficiencies*~efficiencies[loosestLeg] + efficiencies[loosest1lepLeg]
			+ (efficiencies[tau13] - efficiencies[secondTightestLeg])*~singleInefficiencies[0]
			+ (efficiencies[tau12] - efficiencies[secondTightestLeg])*~singleInefficiencies[1]
			+ (efficiencies[tau23] - efficiencies[secondTightestLeg])*efficiencies2Lsym[2];
		if(loosestLeg==trig2Lsym()) globalEfficiencies += (efficiencies[trig2Lsym()]-efficiencies[secondLoosestLeg])*efficiencies2Lasym;
		else if(loosestLeg==trig2Lasym(1)) globalEfficiencies += (efficiencies[trig2Lasym(1)]-efficiencies[secondLoosestLeg])*efficiencies2Lsym[0];
		else if(loosestLeg==trig2Lasym(0)) globalEfficiencies += (efficiencies[trig2Lasym(0)]-efficiencies[secondLoosestLeg])*efficiencies2Lsym[1];
		if(secondTightestLeg && tightestLeg==loosest1lepLeg) /// this works because loosest1lepLeg is 0 if not on plateau...
			globalEfficiencies += (efficiencies[secondTightestLeg]-efficiencies[tightestLeg])*~singleInefficiencies[2];

		efficiencies2Lasym = ~efficiencies[loosestLeg]*efficiencies2Lasym + efficiencies[lambda14];
		if(loosestLeg==trig2Lasym(0) || loosestLeg==trig2Lasym(1))
		{
			/// note: secondLoosestLeg is valid because the loosest leg is either trig2Lasym(0) or trig2Lasym(1)
			efficiencies2Lasym += (efficiencies[loosestLeg]-efficiencies[secondLoosestLeg])*~singleInefficiencies[loosestLeg==trig2Lasym(0)]
				+ (efficiencies[secondLoosestLeg]-efficiencies[lambda14])*~singleInefficiencies[2];
		}
		efficiencies2Lsym[0] = ~efficiencies[lambda124]*efficiencies2Lsym[0] + efficiencies[lambda24];
		efficiencies2Lsym[1] = ~efficiencies[lambda134]*efficiencies2Lsym[1] + efficiencies[lambda34];
		efficiencies2Lsym[2] = ~efficiencies[loosestLeg]*efficiencies2Lsym[2] + efficiencies[lambda234];
		if(lambda124==trig2Lsym()) efficiencies2Lsym[0] += (efficiencies[trig2Lsym()]-efficiencies[lambda24])*~singleInefficiencies[0];
		if(lambda134==trig2Lsym()) efficiencies2Lsym[1] += (efficiencies[trig2Lsym()]-efficiencies[lambda34])*~singleInefficiencies[1];
		if(loosestLeg==trig2Lsym()) efficiencies2Lsym[2] += (efficiencies[trig2Lsym()]-efficiencies[lambda234])*~singleInefficiencies[2];
		singleInefficiencies[0] *= ~efficiencies[lambda124];
		singleInefficiencies[1] *= ~efficiencies[lambda134];
		singleInefficiencies[2] *= ~efficiencies[loosestLeg];
	}
	return success;
}

///
/// One symmetric trilepton trigger
///
template<typename Trig3Lsym>
auto Calculator::globalEfficiency(const LeptonList& leptons, unsigned runNumber, const Trig3Lsym trig, Efficiencies& globalEfficiencies)
	-> std::enable_if_t<Trig3Lsym::is3Lsym(), bool>
{
	ATH_MSG_DEBUG("Entered Calculator::globalEfficiency_One3L() at line " << __LINE__);
	globalEfficiencies = {0.};
	Efficiencies singleInefficiencies{1.}, efficiencies2L{0.};
	bool success = true;
	for(auto& lepton : leptons)
	{
		if(trig.irrelevantFor(lepton) || !aboveThreshold(lepton, trig())) continue;
		auto efficiencies = getCachedTriggerLegEfficiencies(lepton, runNumber, trig(), success);
		globalEfficiencies = ~efficiencies*globalEfficiencies + efficiencies*efficiencies2L;
		efficiencies2L = ~efficiencies*efficiencies2L + efficiencies*~singleInefficiencies;
		singleInefficiencies *= ~efficiencies;
	}
	return success;
}

///
/// One half-symmetric trilepton trigger
///
template<typename Trig3Lhalfsym>
auto Calculator::globalEfficiency(const LeptonList& leptons, unsigned runNumber, const Trig3Lhalfsym trig, Efficiencies& globalEfficiencies)
	-> std::enable_if_t<Trig3Lhalfsym::is3Lhalfsym(), bool>
{
	ATH_MSG_DEBUG("Entered Calculator::globalEfficiency_One3L() at line " << __LINE__);
	if(trig.symmetric()) return globalEfficiency(leptons, runNumber, trig.to_symmetric(), globalEfficiencies);
	globalEfficiencies = {0.};
	Efficiencies singleInefficiencies[2] = {{1.}, {1.}}, twoSingleInefficiencies{1.};
	Efficiencies efficiencies2Lsym{0.}, efficiencies2Lasym{0.}, efficiencies2L2L{0.};
	bool success = true;
	for(auto& lepton : leptons)
	{
		if(trig.irrelevantFor(lepton)) continue;
		Efficiencies efficiencies[2] = {{0.}, {0.}};
		const int asym = 0, sym = 1;
		int loosestLeg;
		if(aboveThreshold(lepton, trig.asymLeg()))
		{
			efficiencies[asym] = getCachedTriggerLegEfficiencies(lepton, runNumber, trig.asymLeg(), success);
			if(aboveThreshold(lepton, trig.symLeg()))
			{
				efficiencies[sym] = getCachedTriggerLegEfficiencies(lepton, runNumber, trig.symLeg(), success);
				loosestLeg = m_parent->getLoosestLeg(lepton, trig.asymLeg(), trig.symLeg(), success)==trig.asymLeg()? asym : sym;
			}
			else loosestLeg = asym;
		}
		else if(aboveThreshold(lepton, trig.symLeg()))
		{
			efficiencies[sym] = getCachedTriggerLegEfficiencies(lepton, runNumber, trig.symLeg(), success);
			loosestLeg = sym;
		}
		else continue;
		Efficiencies delta = efficiencies[asym] - efficiencies[sym];
		if(loosestLeg == asym)
		{
			globalEfficiencies = ~efficiencies[asym]*globalEfficiencies + efficiencies[sym]*efficiencies2L2L + delta*efficiencies2Lsym;
			efficiencies2L2L = ~efficiencies[asym]*efficiencies2L2L + efficiencies[sym]*~twoSingleInefficiencies + delta*~singleInefficiencies[sym];
			efficiencies2Lasym = ~efficiencies[asym]*efficiencies2Lasym + efficiencies[sym]*~twoSingleInefficiencies + delta*~singleInefficiencies[sym];
		}
		else
		{
			globalEfficiencies = ~efficiencies[sym]*globalEfficiencies + efficiencies[asym]*efficiencies2L2L - delta*efficiencies2Lasym;
			efficiencies2L2L = ~efficiencies[sym]*efficiencies2L2L + efficiencies[sym]*~twoSingleInefficiencies;
			efficiencies2Lasym = ~efficiencies[sym]*efficiencies2Lasym + efficiencies[asym]*~twoSingleInefficiencies - delta*~singleInefficiencies[asym];
		}
		efficiencies2Lsym = ~efficiencies[sym]*efficiencies2Lsym + efficiencies[sym]*~singleInefficiencies[sym];
		twoSingleInefficiencies *= ~efficiencies[loosestLeg];
		singleInefficiencies[sym] *= ~efficiencies[sym];
		singleInefficiencies[asym] *= ~efficiencies[asym];
	}
	return success;
}

///
/// One dilepton trigger + one mixed-flavour dilepton trigger
///
template<typename Trig2L, typename Trig2Lmix>
auto Calculator::globalEfficiency(const LeptonList& leptons, unsigned runNumber, const Trig2L trig2L, const Trig2Lmix trig2Lmix, Efficiencies& globalEfficiencies)
	-> std::enable_if_t<Trig2L::is2Lnomix()
						&& Trig2Lmix::is2Lmix()
						&& (Trig2Lmix::object1()==Trig2L::object() || Trig2Lmix::object2()==Trig2L::object()),
		bool>
{
	ATH_MSG_DEBUG("Entered Calculator::globalEfficiency_Two2L() at line " << __LINE__);
	Efficiencies efficiencies1L, efficiencies2L, efficiencies2Lor1L;
	bool success = globalEfficiency(leptons, runNumber, trig2Lmix.template antiside<Trig2L>(), efficiencies1L);
	success = success && globalEfficiency(leptons, runNumber, trig2L, efficiencies2L);
	success = success && globalEfficiency(leptons, runNumber, trig2L, trig2Lmix.template side<Trig2L>(), efficiencies2Lor1L);
	globalEfficiencies = efficiencies2L*~efficiencies1L + efficiencies1L*efficiencies2Lor1L;
	return success;
}

///
/// Combinaisons with 3 dilepton triggers including one mixed-flavour, and one sym./asym. dilepton for each flavour
///
template<typename Trig2L_obj1, typename Trig2L_obj2, typename Trig2Lmix>
auto Calculator::globalEfficiency(const LeptonList& leptons, unsigned runNumber,
		const Trig2L_obj1 trig2L_obj1, const Trig2L_obj2 trig2L_obj2, const Trig2Lmix trig2Lmix, Efficiencies& globalEfficiencies)
	-> std::enable_if_t<Trig2Lmix::is2Lmix()
						&& Trig2L_obj1::is2Lnomix() && Trig2L_obj1::object() == Trig2Lmix::object1()
						&& Trig2L_obj2::is2Lnomix() && Trig2L_obj2::object() == Trig2Lmix::object2(),
						
		bool>
{
	ATH_MSG_DEBUG("Entered Calculator::globalEfficiency_Three2L() at line " << __LINE__);
	Efficiencies efficiencies2L[2] = {{0.}, {0.}}, efficiencies2Lor1L[2] = {{0.}, {0.}};
	bool success = true;
	if(trig2L_obj1)
	{
		success = success && globalEfficiency(leptons, runNumber, trig2L_obj1, efficiencies2L[0]);
		if(trig2Lmix) success = success && globalEfficiency(leptons, runNumber, trig2L_obj1, trig2Lmix.template side<Trig2L_obj1>(), efficiencies2Lor1L[0]);
		else efficiencies2Lor1L[0] = efficiencies2L[0];
	}
	else if(trig2Lmix) success = success && globalEfficiency(leptons, runNumber, trig2Lmix.template side<Trig2L_obj1>(), efficiencies2Lor1L[0]);
	if(trig2L_obj2)
	{
		success = success && globalEfficiency(leptons, runNumber, trig2L_obj2, efficiencies2L[1]);
		if(trig2Lmix) success = success && globalEfficiency(leptons, runNumber, trig2L_obj2, trig2Lmix.template side<Trig2L_obj2>(), efficiencies2Lor1L[1]);
		else efficiencies2Lor1L[1] = efficiencies2L[1];
	}
	else if(trig2Lmix) success = success && globalEfficiency(leptons, runNumber, trig2Lmix.template side<Trig2L_obj2>(), efficiencies2Lor1L[1]);
	globalEfficiencies = efficiencies2L[0]*~efficiencies2Lor1L[1] +  efficiencies2L[1]*~efficiencies2Lor1L[0] + efficiencies2Lor1L[0]*efficiencies2Lor1L[1];
	return success;
}

///
/// Same combinaisons with 3 dilepton triggers, + single-lepton triggers
///
template<typename Trig2L_obj1, typename Trig2L_obj2, typename Trig2Lmix, typename Trig1L_obj1, typename Trig1L_obj2>
auto Calculator::globalEfficiency(const LeptonList& leptons, unsigned runNumber, const Trig2L_obj1 trig2L_obj1, const Trig2L_obj2 trig2L_obj2,
		const Trig2Lmix trig2Lmix, const flat_set<Trig1L_obj1>& trigs1L_obj1, const flat_set<Trig1L_obj2>& trigs1L_obj2, Efficiencies& globalEfficiencies)
	-> std::enable_if_t<Trig2Lmix::is2Lmix()
						&& Trig2L_obj1::is2Lnomix() && Trig2L_obj1::object()==Trig2Lmix::object1()
						&& Trig2L_obj2::is2Lnomix() && Trig2L_obj2::object()==Trig2Lmix::object2()
						&& Trig1L_obj1::is1L() && Trig1L_obj1::object()==Trig2Lmix::object1()
						&& Trig1L_obj2::is1L() && Trig1L_obj2::object()==Trig2Lmix::object2(),
						
		bool>
{
	ATH_MSG_DEBUG("Entered Calculator::globalEfficiency_Three2LSeveral1L() at line " << __LINE__);
	Efficiencies efficiencies[4];
	bool success = true;
	if(trig2L_obj1) success = success && globalEfficiency(leptons, runNumber, trig2L_obj1, trigs1L_obj1, efficiencies[0]);
	else success = success && globalEfficiency(leptons, runNumber, trigs1L_obj1, efficiencies[0]);
	if(trig2L_obj2) success = success && globalEfficiency(leptons, runNumber, trig2L_obj2, trigs1L_obj2, efficiencies[1]);
	else success = success && globalEfficiency(leptons, runNumber, trigs1L_obj2, efficiencies[1]);
	if(trig2Lmix && !trig2Lmix.hiddenBy(trigs1L_obj1))
	{
		auto t = trig2Lmix.addTo(trigs1L_obj1);
		if(trig2L_obj1) success = success && globalEfficiency(leptons, runNumber, trig2L_obj1, t, efficiencies[2]);
		else success = success && globalEfficiency(leptons, runNumber, t, efficiencies[2]);
	}
	else efficiencies[2] = efficiencies[0];
	if(trig2Lmix && !trig2Lmix.hiddenBy(trigs1L_obj2))
	{
		auto t = trig2Lmix.addTo(trigs1L_obj2);
		if(trig2L_obj2) success = success && globalEfficiency(leptons, runNumber, trig2L_obj2, t, efficiencies[3]);
		else success = success && globalEfficiency(leptons, runNumber, t, efficiencies[3]);
	}
	else efficiencies[3] = efficiencies[1];
	globalEfficiencies = Efficiencies(1.) - ~efficiencies[0]*~efficiencies[1] + (efficiencies[2]-efficiencies[0])*(efficiencies[3]-efficiencies[1]);
	return success;
}

///
/// Six dilepton triggers (two mixed-flavour, two sym., two asym./sym.) for two object types
///
template<typename Trig2L_obj1, typename Trig2Lsym_obj1, typename Trig2L_obj2, typename Trig2Lsym_obj2,
	typename Trig2Lmix, typename Trig1L_obj1, typename Trig1L_obj2>
auto Calculator::globalEfficiency(const LeptonList& leptons, unsigned runNumber,
		const Trig2L_obj1 trig2L_obj1, const Trig2Lsym_obj1 trig2Lsym_obj1, const Trig2L_obj2 trig2L_obj2, const Trig2Lsym_obj2 trig2Lsym_obj2,
		const Trig2Lmix trig2Lmix1, const Trig2Lmix trig2Lmix2, 
		const flat_set<Trig1L_obj1>& trigs1L_obj1, const flat_set<Trig1L_obj2>& trigs1L_obj2, Efficiencies& globalEfficiencies)
	-> std::enable_if_t<Trig2Lmix::is2Lmix()
						&& Trig2L_obj1::is2Lnomix() && Trig2L_obj1::object()==Trig2Lmix::object1()
						&& Trig2L_obj2::is2Lnomix() && Trig2L_obj2::object()==Trig2Lmix::object2()
						&& Trig2Lsym_obj1::is2Lsym() && Trig2Lsym_obj1::object()==Trig2Lmix::object1()
						&& Trig2Lsym_obj2::is2Lsym() && Trig2Lsym_obj2::object()==Trig2Lmix::object2()
						&& Trig1L_obj1::is1L() && Trig1L_obj1::object()==Trig2Lmix::object1()
						&& Trig1L_obj2::is1L() && Trig1L_obj2::object()==Trig2Lmix::object2(),
		bool>
{
	ATH_MSG_DEBUG("Entered Calculator::globalEfficiency_Six2LSeveral1L() at line " << __LINE__);
	
	auto singleObjectFactor = [=](auto trig2L, auto trig2Lsym, auto& trigs1L, Efficiencies (&efficiencies)[4]) -> bool
	{
		auto eval_for = [=](const auto& trigs1L_extended, Efficiencies& eff) -> bool
		{
			if(trig2L && trig2Lsym) return this->globalEfficiency(leptons, runNumber, trig2L, trig2Lsym, trigs1L_extended, eff);
			else if(trig2L) return this->globalEfficiency(leptons, runNumber, trig2L, trigs1L_extended, eff);
			else if(trig2Lsym) return this->globalEfficiency(leptons, runNumber, trig2Lsym, trigs1L_extended, eff);
			else return this->globalEfficiency(leptons, runNumber, trigs1L_extended, eff);
		};
	
		bool success = eval_for(trigs1L, efficiencies[0]);
		if(trig2Lmix1) success = success && eval_for(trig2Lmix1.addTo(trigs1L), efficiencies[1]);
		else efficiencies[1] = efficiencies[0];
		if(trig2Lmix2)
		{
			auto t = trig2Lmix2.addTo(trigs1L);
			success = success && eval_for(t, efficiencies[2]);
			if(trig2Lmix1) success && eval_for(trig2Lmix1.addTo(t), efficiencies[3]);
			else efficiencies[3] = efficiencies[2];
		}
		else
		{
			efficiencies[2] = efficiencies[0];
			efficiencies[3] = efficiencies[1];
		}
		return success;
	};
	
	Efficiencies efficiencies1[4], efficiencies2[4];
	bool success = singleObjectFactor(trig2L_obj1, trig2Lsym_obj1, trigs1L_obj1, efficiencies1)
		&& singleObjectFactor(trig2L_obj2, trig2Lsym_obj2, trigs1L_obj2, efficiencies2);
	globalEfficiencies = Efficiencies(1.) - ~efficiencies1[0]*~efficiencies2[0] + (efficiencies1[1]-efficiencies1[0])*(efficiencies2[1]-efficiencies2[0])
		+ (efficiencies1[2]-efficiencies1[0])*(efficiencies2[2]-efficiencies2[0]) 
		- (efficiencies1[0]-efficiencies1[1]-efficiencies1[2]+efficiencies1[3])*(efficiencies2[0]-efficiencies2[1]-efficiencies2[2]+efficiencies2[3]);
	return success;
}

///
/// One mixed-flavour trilepton trigger
///
template<typename Trig3Lmix>
auto Calculator::globalEfficiency(const LeptonList& leptons, unsigned runNumber, const Trig3Lmix trig, Efficiencies& globalEfficiencies)
	-> std::enable_if_t<Trig3Lmix::is3Lmix(), bool>
{
	ATH_MSG_DEBUG("Entered Calculator::globalEfficiency_One3L() at line " << __LINE__);
	Efficiencies efficiencies[2] = {{0.}, {0.}};
	bool success = globalEfficiency(leptons, runNumber, trig.template side<Trig3Lmix::object1()>(), efficiencies[0])
		&& globalEfficiency(leptons, runNumber, trig.template side<Trig3Lmix::object2()>(), efficiencies[1]);
	globalEfficiencies = efficiencies[0]*efficiencies[1];
	return success;
}

///
/// Two complementary mixed-flavour trilepton triggers
///
template<typename Trig3Lmix1, typename Trig3Lmix2>
auto Calculator::globalEfficiency(const LeptonList& leptons, unsigned runNumber, const Trig3Lmix1 trig1, const Trig3Lmix2 trig2, Efficiencies& globalEfficiencies)
	-> std::enable_if_t<Trig3Lmix1::is3Lmix() 
					&& Trig3Lmix2::is3Lmix() 
					&& Trig3Lmix1::object1() == Trig3Lmix2::object2()
					&& Trig3Lmix1::object2() == Trig3Lmix2::object1(), 
		bool>
{
	ATH_MSG_DEBUG("Entered Calculator::globalEfficiency_Two3L() at line " << __LINE__);
	Efficiencies efficiencies[6] = {{0.}, {0.}, {0.}, {0.}, {0.}, {0.}};
	auto trig2La = trig1.template side<Trig3Lmix1::object1()>();
	auto trig1La = trig2.template side<Trig3Lmix2::object2()>();
	bool success = globalEfficiency(leptons, runNumber, trig1La, efficiencies[0])
		&& globalEfficiency(leptons, runNumber, trig2La, efficiencies[1]);
	if(!trig2La.hiddenBy(trig1La)) success = success && globalEfficiency(leptons, runNumber, trig2La, trig1La, efficiencies[2]);
	else efficiencies[2] = efficiencies[0];
	auto trig2Lb = trig2.template side<Trig3Lmix2::object1()>();
	auto trig1Lb = trig1.template side<Trig3Lmix1::object2()>();
	success = success && globalEfficiency(leptons, runNumber, trig1Lb, efficiencies[3])
		&& globalEfficiency(leptons, runNumber, trig2Lb, efficiencies[4]);
	if(!trig2Lb.hiddenBy(trig1Lb)) success = success && globalEfficiency(leptons, runNumber, trig2Lb, trig1Lb, efficiencies[5]);
	else efficiencies[5] = efficiencies[3];
	globalEfficiencies = efficiencies[0]*efficiencies[4] + efficiencies[3]*efficiencies[1] 
		+ (efficiencies[2]-efficiencies[0]-efficiencies[1])*(efficiencies[3]+efficiencies[4]-efficiencies[5]);
	return success;
}

bool Calculator::globalEfficiency_Factorized2(const LeptonList& leptons, unsigned runNumber, GlobEffFunc func1, GlobEffFunc func2, Efficiencies& globalEfficiencies)
{
	Efficiencies efficiencies[2];
	if(!func1(this, leptons, runNumber, efficiencies[0])) return false;
	if(!func2(this, leptons, runNumber, efficiencies[1])) return false;
	globalEfficiencies = ~(~efficiencies[0] * ~efficiencies[1]);
	return true;
}

bool Calculator::globalEfficiency_Factorized3(const LeptonList& leptons, unsigned runNumber, GlobEffFunc func1, GlobEffFunc func2, GlobEffFunc func3, Efficiencies& globalEfficiencies)
{
	Efficiencies efficiencies[3];
	if(!func1(this, leptons, runNumber, efficiencies[0])) return false;
	if(!func2(this, leptons, runNumber, efficiencies[1])) return false;
	if(!func3(this, leptons, runNumber, efficiencies[2])) return false;
	globalEfficiencies = ~(~efficiencies[0] * ~efficiencies[1]* ~efficiencies[2]);
	return true;
}

bool Calculator::fillListOfLegsFor(const Lepton& lepton, const std::vector<TrigDef>& triggers, flat_set<std::size_t>& validLegs) const
{
	validLegs.clear();
	for(auto& trig : triggers)
	{
		TriggerProperties tp(trig);
		if(!tp.valid())
		{
			ATH_MSG_ERROR("Unrecognized trigger type " << trig.type);
			return false;
		}
		auto end = tp.cend(lepton.type());
		for(auto itr=tp.cbegin(lepton.type()); itr!=end; ++itr)if(aboveThreshold(lepton, *itr)) validLegs.emplace(*itr);
	}
	return true;
}

bool Calculator::canTriggerBeFired(const TrigDef& trig, const std::vector<flat_set<std::size_t> >& firedLegs) const
{
	static_assert(std::tuple_size<decltype(trig.leg)>::value == 3, "extend Calculator::canTriggerBeFired() implementation to support triggers with >= 4 legs");
	int n0=0, n1=0, n0min = 1 + (trig.leg[1]!=0)*(trig.leg[0]!=trig.leg[1]?-5:1) + (trig.leg[2]!=0)*(trig.leg[0]!=trig.leg[2]?-9:1);
	if(n0min>0 || !trig.leg[2])
	{
		for(auto& legs : firedLegs)
		{
			bool fire0 = legs.count(trig.leg[0]);
			if(n0min <= 0) /// Asymmetric dilepton triggers
			{
				if(n1 && fire0) return true;
				if(legs.count(trig.leg[1]))
				{
					if(n0) return true;
					++n1;
				}
				if(fire0) ++n0;
			}
			else if(fire0 && ++n0>=n0min) return true; /// Single-lepton and symmetric di/trilepton triggers 
		}
	}
	else /// Trilepton triggers (except fully-symmetric ones that are addressed above)
	{
		auto end = firedLegs.end();
		for(auto legs0=firedLegs.begin();legs0!=end;++legs0)
		{
			for(int i=0;i<3;++i)
			{
				if(!legs0->count(trig.leg[i])) continue;
				for(auto legs1=legs0+1;legs1!=end;++legs1)
				{
					for(int j=1;j<3;++j)
					{
						if(!legs1->count(trig.leg[(i+j)%3])) continue;
						for(auto legs2=legs1+1;legs2!=end;++legs2)
						{
							if(legs2->count(trig.leg[(i+3-j)%3])) return true;
						}
					}
				}
			}
		}
	}
	return false;
}

bool Calculator::globalEfficiency_Toys(const LeptonList& leptons, unsigned runNumber,
	const std::vector<TrigDef>& triggers, Efficiencies& globalEfficiencies)
{
	ATH_MSG_DEBUG("Entered Calculator::globalEfficiency_Toys() at line " << __LINE__);
	globalEfficiencies = {0.};
	if(m_parent->m_numberOfToys <= 0)
	{
		ATH_MSG_ERROR("The specified number of toys is <= 0");
		return false;
	}
	std::map<const Lepton*, std::vector<std::pair<std::size_t, Efficiencies> > > leptonEfficiencies;
	for(auto& lepton : leptons)
	{
		flat_set<std::size_t> validLegs;
		if(!fillListOfLegsFor(lepton, triggers, validLegs)) return false;
		auto& efficiencies = leptonEfficiencies[&lepton];
		const int nLegs = validLegs.size();
		if(nLegs)
		{
			bool success = true;
			for(std::size_t leg : m_parent->getSortedLegs(lepton, validLegs, success))
			{
				efficiencies.emplace_back(leg, getCachedTriggerLegEfficiencies(lepton, runNumber, leg, success));
			}
			if(!success) return false;
		}
	}
	unsigned long seed;
	if(!m_parent->m_useInternalSeed)
	{
		if(!m_parent->retrieveEventNumber(seed))
		{
			ATH_MSG_WARNING("Will use internal seed instead of event number");
			seed = m_parent->m_seed++;
		}
	}
	else seed = m_parent->m_seed++;
	std::mt19937_64 randomEngine(seed);
	std::uniform_real_distribution<double> uniformPdf(0., 1.);
	std::vector<flat_set<std::size_t> > firedLegs(leptonEfficiencies.size());
	unsigned long nPassed[2] = {0, 0};
	for(unsigned long toy=0;toy<m_parent->m_numberOfToys;++toy)
	{
		for(int step=0;step<2;++step) /// 0 = data, 1 = MC
		{
			auto legs = firedLegs.begin();
			for(auto& kv : leptonEfficiencies)
			{
				legs->clear();
				double x = uniformPdf(randomEngine);
				for(auto& p : kv.second)
				{
					if(x < (step? p.second.mc(): p.second.data())) legs->emplace(p.first);
				}
				++legs;
			}
			for(auto& trig : triggers)
			{
				if(!canTriggerBeFired(trig, firedLegs)) continue;
				++nPassed[step];
				break;
			}
		}
	}
	globalEfficiencies.data() = double(nPassed[0]) / double(m_parent->m_numberOfToys);
	globalEfficiencies.mc() = double(nPassed[1]) / double(m_parent->m_numberOfToys);
	return true;
}

Calculator::Helper::Helper(const std::vector<TrigDef>& defs) :
	m_formula(nullptr), m_defs(defs)
{
}

bool Calculator::Helper::duplicates() const
{
	for(auto itr1=m_defs.cbegin(); itr1!=m_defs.cend(); ++itr1)
		for(auto itr2=itr1+1; itr2!=m_defs.cend(); ++itr2)
			if(itr1->type==itr2->type && itr1->leg==itr2->leg) return true;
	return false;
}

namespace TrigGlobEffCorr /// the template specializations below must be enclosed in this namespace
{
	template<typename T>
	struct TrigGlobEffCorr::Calculator::Helper::BindPackedParam
	{
		using TrigType = T;
		using ArgType = std::add_const_t<T>;
		static constexpr bool multiple() { return false; }
		static constexpr bool optional() { return false; }
		static void add(T& arg, ImportData::TrigDef& def) { arg.setDefinition(def); }
		static constexpr bool valid(const T& arg) { return bool(arg); }
	};

	template<typename T>
	struct TrigGlobEffCorr::Calculator::Helper::BindPackedParam<flat_set<T>>
	{
		using TrigType = T;
		using ArgType = const flat_set<T>&;
		static constexpr bool multiple() { return true; }
		static constexpr bool optional() { return false; }
		static void add(flat_set<T>& arg, ImportData::TrigDef& def) { arg.emplace().first->setDefinition(def); }
		static constexpr bool valid(const flat_set<T>& arg) { return arg.size(); }
	};

	template<typename T>
	struct TrigGlobEffCorr::Calculator::Helper::BindPackedParam<Calculator::Helper::Optional<T>>
	{
		using TrigType = typename BindPackedParam<T>::TrigType;
		using ArgType = typename BindPackedParam<T>::ArgType;
		static constexpr bool multiple() { return BindPackedParam<T>::multiple(); }
		static constexpr bool optional() { return true; }
		static void add(std::remove_cv_t<std::remove_reference_t<ArgType>>& arg, ImportData::TrigDef def) { BindPackedParam<T>::add(arg, def); }
		static constexpr bool valid(ArgType& arg) { return BindPackedParam<T>::valid(arg); }
	};
}

template<typename Param>
auto Calculator::Helper::extract() 
{
	std::remove_cv_t<std::remove_reference_t<typename Param::ArgType>> trigs;
	for(auto& def : m_defs)
	{
		if(def.used || def.type!=Param::TrigType::type()) continue;
		def.used = true;
		Param::add(trigs, def);
		if(!Param::multiple()) break;
	}
	if(!Param::optional() && !Param::valid(trigs)) throw NoSuchTrigger();
	return trigs;
}

template<typename... Trigs>
bool Calculator::Helper::bindFunction()
{
	for(auto& def : m_defs) def.used = false;
	using fnptr = bool(Calculator::*)(const LeptonList&, unsigned, typename BindPackedParam<Trigs>::ArgType..., Efficiencies&);
	try
	{
		m_formula = std::bind<fnptr>(&Calculator::globalEfficiency, ::_1, ::_2, ::_3, extract<BindPackedParam<Trigs>>()..., ::_4);
		if(std::all_of(m_defs.cbegin(), m_defs.cend(), [](auto& def){return def.used;})) return true;
	}
	catch(NoSuchTrigger){}
	m_formula = nullptr;
	return false;
}

template<TriggerType object_flag>
bool Calculator::Helper::findAndBindFunction() /// for combinations with a single flavour present
{
	using A = TriggerClass<object_flag>;
	using A1L = flat_set<typename A::T_1>;
	using A_2sym = typename A::T_2sym;
	using A_2asym = typename A::T_2asym;
	if(!m_n2L && !m_n3L)
	{
		return bindFunction<typename A::T_1>() || bindFunction<A1L>();
	}
	else if(m_n2L==1 && !m_n3L)
	{
		return bindFunction<A_2sym>() || bindFunction<A_2asym>()
			|| bindFunction<A_2sym, A1L>() || bindFunction<A_2asym, A1L>();
	}
	else if(m_n2L==2 && !m_n3L)
	{
		return bindFunction<A_2sym, A_2sym, Optional<A1L>>() || bindFunction<A_2asym, A_2sym, Optional<A1L>>();
	}
	else if(m_n3L==1 && !m_n1L && !m_n2L)
	{
		return bindFunction<typename A::T_3sym>() || bindFunction<typename A::T_3halfsym>();
	}
	return false;
}

template<TriggerType object_flag1, TriggerType object_flag2>
bool Calculator::Helper::findAndBindFunction() /// for combinations with two flavours present
{
	/// this only deals with the presence of mixed triggers
	using A = TriggerClass<object_flag1>;
	using B = TriggerClass<object_flag2>;
	using AB = TriggerClass<object_flag1, object_flag2>;
	using A_1 = typename A::T_1;
	using B_1 = typename B::T_1;		
	using OA1L = Optional<flat_set<A_1>>;
	using OB1L = Optional<flat_set<B_1>>;
	using A_2sym = typename A::T_2sym;
	using B_2sym = typename B::T_2sym;
	using A_2asym = typename A::T_2asym;
	using B_2asym = typename B::T_2asym;
	using AB_1_1 = typename AB::T_1_1;
	
	/// checked if triggers can be factorized = no mixed trigger in the combination.
	if(m_n1L>0 && !m_n2L && !m_n3L)
	{
		return bindFunction<A_1, B_1>() || bindFunction<flat_set<A_1>, flat_set<B_1>>();
	}
	else if(m_n2L==1 && !m_n3L) /// one dilepton trigger (+ single-lepton triggers)
	{
		return bindFunction<AB_1_1>() || bindFunction<AB_1_1, flat_set<A_1>, flat_set<B_1>>();
	}
	else if(m_n2L>=2 && m_n2L<=6 && !m_n3L) /// several dilepton triggers (+ single-lepton triggers)
	{
		return 
			/// 2 dilepton triggers
			   bindFunction<A_2sym, AB_1_1>() || bindFunction<A_2asym, AB_1_1>()
			|| bindFunction<B_2sym, AB_1_1>() || bindFunction<B_2asym, AB_1_1>()
			/// 3 dilepton triggers
			|| bindFunction<Optional<A_2sym>, Optional<B_2sym>, Optional<AB_1_1>, OA1L, OB1L>()
			|| bindFunction<Optional<A_2asym>, Optional<B_2sym>, Optional<AB_1_1>, OA1L, OB1L>()
			|| bindFunction<Optional<A_2sym>, Optional<B_2asym>, Optional<AB_1_1>, OA1L, OB1L>()
			|| bindFunction<Optional<A_2asym>, Optional<B_2asym>, Optional<AB_1_1>, OA1L, OB1L>()
			/// 6 dilepton triggers
			|| bindFunction<Optional<A_2sym>, Optional<A_2sym>, Optional<B_2sym>, Optional<B_2sym>, Optional<AB_1_1>, Optional<AB_1_1>, OA1L, OB1L>()
			|| bindFunction<Optional<A_2asym>, Optional<A_2sym>, Optional<B_2sym>, Optional<B_2sym>, Optional<AB_1_1>, Optional<AB_1_1>, OA1L, OB1L>()
			|| bindFunction<Optional<A_2sym>, Optional<A_2sym>, Optional<B_2asym>, Optional<B_2sym>, Optional<AB_1_1>, Optional<AB_1_1>, OA1L, OB1L>()
			|| bindFunction<Optional<A_2asym>, Optional<A_2sym>, Optional<B_2asym>, Optional<B_2sym>, Optional<AB_1_1>, Optional<AB_1_1>, OA1L, OB1L>();
	}
	else if(m_n3L==1 && !m_n2L && !m_n1L) /// one mixed trilepton trigger
	{
		return bindFunction<typename AB::T_2sym_1>() || bindFunction<typename AB::T_1_2sym>()
			||  bindFunction<typename AB::T_2asym_1>() || bindFunction<typename AB::T_1_2asym>();
	}
	else if(m_n3L==2 && !m_n2L && !m_n1L) /// two mixed trilepton triggers
	{
		return bindFunction<typename AB::T_2sym_1, typename AB::T_1_2sym>() 
			|| bindFunction<typename AB::T_2asym_1, typename AB::T_1_2sym>()
			|| bindFunction<typename AB::T_2sym_1, typename AB::T_1_2asym>() 
			|| bindFunction<typename AB::T_2asym_1, typename AB::T_1_2asym>();
	}
	return false;
}

bool Calculator::Helper::findAndBindFunction() /// top-level function
{
	auto countTriggers = [&](auto nlep_flag) { return std::count_if(m_defs.cbegin(), m_defs.cend(), [=](auto& def){return def.type&nlep_flag;}); };
	m_n1L = countTriggers(TT_SINGLELEPTON_FLAG);
	m_n2L = countTriggers(TT_DILEPTON_FLAG);
	m_n3L = countTriggers(TT_TRILEPTON_FLAG);
	auto exclusively = [&](auto obj_flags) { return std::none_of(m_defs.cbegin(), m_defs.cend(), [=](auto& def){return def.type&TT_MASK_FLAVOUR&~obj_flags;}); };

	/// First check if the trigger combination refers to a single object type
	if(exclusively(TT_ELECTRON_FLAG)) return findAndBindFunction<TT_ELECTRON_FLAG>();
	if(exclusively(TT_MUON_FLAG)) return findAndBindFunction<TT_MUON_FLAG>();
	if(exclusively(TT_PHOTON_FLAG)) return findAndBindFunction<TT_PHOTON_FLAG>();

	/// Then try to rely on available formulas for combinations with two object types
	bool success = false;
	if(exclusively(TT_ELECTRON_FLAG|TT_MUON_FLAG)) success = findAndBindFunction<TT_ELECTRON_FLAG,TT_MUON_FLAG>();
	else if(exclusively(TT_ELECTRON_FLAG|TT_PHOTON_FLAG)) success = findAndBindFunction<TT_ELECTRON_FLAG,TT_PHOTON_FLAG>();
	else if(exclusively(TT_MUON_FLAG|TT_PHOTON_FLAG)) success = findAndBindFunction<TT_MUON_FLAG,TT_PHOTON_FLAG>();
	if(success) return true;
	
	/// As a last resort, maybe factorizing helps
	std::vector<Helper> helpers;
	for(auto obj_flag : {TT_ELECTRON_FLAG, TT_MUON_FLAG, TT_PHOTON_FLAG})
	{
		if(std::any_of(m_defs.cbegin(), m_defs.cend(), /// check there's no mixed-flavour trigger involving 'obj_flag'
			[&](auto& def){ return (def.type&obj_flag) && TriggerProperties(def.type).mixed();})) continue;
		std::vector<ImportData::TrigDef> trigs1, trigs2;
		std::partition_copy(m_defs.begin(), m_defs.end(), std::back_inserter(trigs1), std::back_inserter(trigs2), [&](auto& def){ return (def.type&obj_flag); });
		m_defs.swap(trigs2);
		if(!trigs1.size()) continue;
		helpers.emplace_back(trigs1);
		if(!helpers.back().findAndBindFunction()) return false;
	}
	if(helpers.size())
	{
		if(m_defs.size())
		{
			if(!findAndBindFunction()) return false;
			if(helpers.size() == 1) m_formula = std::bind(&Calculator::globalEfficiency_Factorized2, ::_1, ::_2, ::_3, 
				std::move(m_formula), std::move(helpers[0].m_formula), ::_4);
			else if(helpers.size()==2) m_formula = std::bind(&Calculator::globalEfficiency_Factorized3, ::_1, ::_2, ::_3, 
				std::move(m_formula), std::move(helpers[0].m_formula), std::move(helpers[1].m_formula), ::_4);
			else
			{
				m_formula = nullptr;
				return false;
			}
		}
		else	
		{
			if(helpers.size() == 2) m_formula = std::bind(&Calculator::globalEfficiency_Factorized2, ::_1, ::_2, ::_3, 
				std::move(helpers[0].m_formula), std::move(helpers[1].m_formula), ::_4);
			else if(helpers.size() == 3) m_formula = std::bind(&Calculator::globalEfficiency_Factorized3, ::_1, ::_2, ::_3, 
				std::move(helpers[0].m_formula), std::move(helpers[1].m_formula), std::move(helpers[2].m_formula), ::_4);
			else return false;
		}
		return true;
	}

	return false;
}
