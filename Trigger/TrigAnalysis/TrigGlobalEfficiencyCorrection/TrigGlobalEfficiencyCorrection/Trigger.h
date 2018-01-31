// contact: jmaurer@cern.ch

#ifndef TRIGGLOBALEFFICIENCYCORRECTION_TRIGGER_H
#define TRIGGLOBALEFFICIENCYCORRECTION_TRIGGER_H 1

#include "TrigGlobalEfficiencyCorrection/ImportData.h"
#include "xAODBase/ObjectType.h"

#include <array>
#include <algorithm>
#include <type_traits>
#include <boost/container/flat_set.hpp>

namespace TrigGlobEffCorr
{

class UnusedArg { public: static constexpr xAOD::Type::ObjectType object() { return xAOD::Type::Other; } };

template<TriggerType tt, unsigned nE, unsigned nMu, typename Cast1 = UnusedArg, typename Cast2 = UnusedArg>
class Trigger
{
private:
	template<typename Key> using flat_set = boost::container::flat_set<Key>;
	static constexpr bool extraCheck(xAOD::Type::ObjectType obj) { return (object()==obj || obj==xAOD::Type::Other); }
public:
	std::array<std::size_t, nE+nMu> legs;
	static constexpr TriggerType type() { return tt; }
	static constexpr unsigned nLegs(xAOD::Type::ObjectType obj) { return obj==xAOD::Type::Electron? nE : nMu; }
	static constexpr unsigned mixed() { return nE&&nMu; }
	
	static constexpr xAOD::Type::ObjectType object()
	{
		if(nE&&!nMu) return xAOD::Type::Electron;
		if(!nE&&nMu) return xAOD::Type::Muon;
		return xAOD::Type::Other;
	}
	static constexpr xAOD::Type::ObjectType antiobject()
	{
		if(nE&&!nMu) return xAOD::Type::Muon;
		if(!nE&&nMu) return xAOD::Type::Electron;
		return xAOD::Type::Other;
	}
	static constexpr xAOD::Type::ObjectType major_object()
	{
		if(tt==TT_2E_MU_SYM || tt==TT_2E_MU_ASYM) return xAOD::Type::Electron;
		if(tt==TT_2MU_E_SYM || tt==TT_2MU_E_ASYM) return xAOD::Type::Muon;
		return xAOD::Type::Other;
	}
	static constexpr xAOD::Type::ObjectType minor_object()
	{
		if(tt==TT_2E_MU_SYM || tt==TT_2E_MU_ASYM) return xAOD::Type::Muon;
		if(tt==TT_2MU_E_SYM || tt==TT_2MU_E_ASYM) return xAOD::Type::Electron;
		return xAOD::Type::Other;
	}
	static bool relevantFor(const Lepton& lepton) { return lepton.type()==object(); }
	static bool irrelevantFor(const Lepton& lepton) { return !relevantFor(lepton); }
	
	static constexpr bool is1L(xAOD::Type::ObjectType obj = xAOD::Type::Other) { return (tt&TT_GENERIC_SINGLELEPTON) && extraCheck(obj); }
	static constexpr bool is2Lasym(xAOD::Type::ObjectType obj = xAOD::Type::Other) { return (tt==TT_2E_ASYM || tt==TT_2MU_ASYM) && extraCheck(obj); }
	static constexpr bool is2Lsym(xAOD::Type::ObjectType obj = xAOD::Type::Other) { return (tt==TT_2E_SYM || tt==TT_2MU_SYM) && extraCheck(obj); }
	static constexpr bool is2L(xAOD::Type::ObjectType obj = xAOD::Type::Other) { return (is2Lsym() || is2Lasym()) && extraCheck(obj); }
	static constexpr bool is3Lsym(xAOD::Type::ObjectType obj = xAOD::Type::Other) { return (tt==TT_3E_SYM || tt==TT_3MU_SYM) && extraCheck(obj); }
	static constexpr bool is3Lhalfsym(xAOD::Type::ObjectType obj = xAOD::Type::Other) { return (tt==TT_3E_HALFSYM || tt==TT_3MU_HALFSYM) && extraCheck(obj); }
	static constexpr bool isEMU() { return tt==TT_EMU; }
	static constexpr bool is3Lmix() { return tt==TT_2E_MU_SYM || tt==TT_2E_MU_ASYM || tt==TT_2MU_E_SYM || tt==TT_2MU_E_ASYM; }
	
	
	explicit Trigger() { std::fill(legs.begin(), legs.end(), 0); }
	bool set_definition(const ImportData::TrigDef& def)
	{
		if(def.type != tt) return false;
		if(tt == TT_2E_MU_SYM) // special case needed to skip the duplicated electron leg
		{
			legs[0] = def.leg[0];
			legs[1] = def.leg[2];
		}
		else // Works as well for trielectron/trimuon triggers since the asymmetric leg is always stored first
		{
			std::copy_n(def.leg.cbegin(), legs.size(), legs.begin());
		}
		return true;
	}
	template<bool dummy=true> typename std::enable_if<(nE+nMu==1)&&dummy, std::size_t>::type operator()(void) const { return legs[0]; }
	std::size_t operator()(unsigned index) const { return legs[index]; }
	template<bool dummy = true> typename std::enable_if<(nE+nMu==1)&&dummy, std::size_t>::type operator<(const Trigger& rhs) const
	{
		return legs[0] < rhs.legs[0];
	}
	explicit operator bool() const { return std::all_of(legs.cbegin(), legs.cend(), [](const std::size_t& x)->bool{ return x; }); }
	bool operator==(const Trigger& rhs) const { return legs == rhs.legs; }
	
	template<xAOD::Type::ObjectType obj = object()>
	decltype(legs.cbegin()) cbegin() const { return legs.cbegin() + nE*(obj==xAOD::Type::Muon); }
	template<xAOD::Type::ObjectType obj = object()>
	decltype(legs.cend()) cend() const { return legs.cend() - nMu*(obj==xAOD::Type::Electron); }

	template<typename Trig1L> auto hiddenBy(const Trig1L trig) const
		-> typename std::enable_if<Trig1L::is1L(), bool>::type
	{
		constexpr auto obj = Trig1L::object();
		return std::find(cbegin<obj>(), cend<obj>(), trig()) != cend<obj>();
	}
	template<typename Trig1L> auto hiddenBy(const flat_set<Trig1L>& trigs) const
		-> typename std::enable_if<Trig1L::is1L(), bool>::type
	{
		return std::any_of(trigs.cbegin(), trigs.cend(), [&](Trig1L t)->bool{ return hiddenBy(t); });
	}
	template<xAOD::Type::ObjectType obj> auto side() const
		-> typename std::enable_if<mixed(), typename std::conditional<Cast1::object()==obj, Cast1, Cast2>::type>::type
	{
		decltype(side<obj>()) trig;
		std::copy(this->cbegin<obj>(), this->cend<obj>(), trig.legs.begin());
		return trig;
	}
	template<typename TrigX> auto side() const -> decltype(side<TrigX::object()>()) { return side<TrigX::object()>(); }
	template<typename TrigX> auto antiside() const -> decltype(side<TrigX::antiobject()>()) { return side<TrigX::antiobject()>(); }

	template<typename Trig1L> auto merge(const flat_set<Trig1L>& trigs1L) const
		-> typename std::enable_if<mixed()&&Trig1L::is1L()&&nLegs(Trig1L::object())==1, flat_set<Trig1L>>::type 
	{
		flat_set<Trig1L> trigs(trigs1L);
		trigs.insert(side<Trig1L>());
		return trigs;
	}
	
	template<bool dummy=true> auto symmetric() const
		-> typename std::enable_if<!std::is_same<Cast1, UnusedArg>::value && dummy, bool>::type
	{
		return std::all_of(legs.cbegin()+1, legs.cend(), [&](std::size_t l)->bool{ return l==legs[0]; });
	}

	template<bool dummy=true> auto to_symmetric() const
		-> typename std::enable_if<!std::is_same<Cast1, UnusedArg>::value&&dummy, Cast1>::type
	{
		Cast1 trig;
		trig.legs[0] = this->legs[0];
		return trig;
	}
};

class Trig1E : public Trigger<TT_SINGLE_E, 1, 0>
{
public:
	static const flat_set<std::size_t>& anonymize(const flat_set<Trig1E>& triggers)
	{
		return reinterpret_cast<const flat_set<std::size_t>&>(triggers);
	}
};
class Trig1MU : public Trigger<TT_SINGLE_MU, 0, 1>
{
public:
	static const flat_set<std::size_t>& anonymize(const flat_set<Trig1MU>& triggers)
	{
		return reinterpret_cast<const flat_set<std::size_t>&>(triggers);
	}
};
class Trig2Esym : public Trigger<TT_2E_SYM, 1, 0> {};
class Trig2Easym : public Trigger<TT_2E_ASYM, 2, 0, Trig2Esym> {};
class Trig2MUsym : public Trigger<TT_2MU_SYM, 0, 1> {};
class Trig2MUasym : public Trigger<TT_2MU_ASYM, 0, 2, Trig2MUsym> {};
class TrigEMU : public Trigger<TT_EMU, 1, 1, Trig1E, Trig1MU> {};
class Trig3Esym : public Trigger<TT_3E_SYM, 1, 0> {};
class Trig3Ehalfsym : public Trigger<TT_3E_HALFSYM, 2, 0, Trig3Esym>
{
public:
	std::size_t asymLeg() const { return legs[0]; }
	std::size_t symLeg() const { return legs[1]; }
};
class Trig3MUsym : public Trigger<TT_3MU_SYM, 0, 1> {};
class Trig3MUhalfsym : public Trigger<TT_3MU_HALFSYM, 0, 2, Trig3MUsym>
{
public:
	std::size_t asymLeg() const { return legs[0]; }
	std::size_t symLeg() const { return legs[1]; }
};
class Trig2EMUsym : public Trigger<TT_2E_MU_SYM, 1, 1, Trig2Esym, Trig1MU> {};
class Trig2EMUasym : public Trigger<TT_2E_MU_ASYM, 2, 1, Trig2Easym, Trig1MU> {};
class TrigE2MUsym : public Trigger<TT_2MU_E_SYM, 1, 1, Trig1E, Trig2MUsym> {};
class TrigE2MUasym : public Trigger<TT_2MU_E_ASYM, 1, 2, Trig1E, Trig2MUasym> {};

}


#endif