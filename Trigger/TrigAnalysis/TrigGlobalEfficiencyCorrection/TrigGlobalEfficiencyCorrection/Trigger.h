/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

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

class Trig1E;
class Trig1MU;
class Trig1PH;

template<TriggerType tt, typename CastType1 = UnusedArg, typename CastType2 = UnusedArg>
class Trigger
{
private:
	template<typename Key> using flat_set = boost::container::flat_set<Key>;
	static constexpr bool extraCheck(xAOD::Type::ObjectType obj) { return (object()==obj || obj==xAOD::Type::Other); }
public:
	static constexpr TriggerType type() { return tt; }
	
	static constexpr bool mixed()
	{
		constexpr auto x = (tt&TT_MASK_FLAVOUR);
		static_assert(!(x&~(TT_ELECTRON_FLAG|TT_MUON_FLAG|TT_PHOTON_FLAG)), "implementation needs to account for new flavours");
		return (x!=TT_ELECTRON_FLAG) && (x!=TT_MUON_FLAG) && (x!=TT_PHOTON_FLAG);
	}
	
	static constexpr unsigned nDistinctLegs()
	{
		auto x = tt&TT_MASK_SYMMETRY;
		if(tt&TT_SINGLELEPTON_FLAG) return 1;
		else if(tt&TT_DILEPTON_FLAG) return 2 - 1*(x==TT_SYM);
		else if(tt&TT_TRILEPTON_FLAG) return (x==TT_ASYM)? 3 : 1 + 1*(mixed()||(x!=TT_SYM));
		return 0;
	}
	
	static constexpr unsigned nDistinctLegs(xAOD::Type::ObjectType obj)
	{
		switch(obj)
		{
			case xAOD::Type::Electron:
				return !!(tt&TT_ELECTRON_FLAG) * (mixed()? ((tt&TT_X2Y_FLAG)? 1 : nDistinctLegs()-1) : nDistinctLegs());
			case xAOD::Type::Muon:
				return !!(tt&TT_MUON_FLAG) * (mixed()? ((tt&TT_X2Y_FLAG)? nDistinctLegs()-1 : 1) : nDistinctLegs());
			case xAOD::Type::Photon:
				return (tt&TT_PHOTON_FLAG)? nDistinctLegs() : 0;
			default: return 0;
		}
	}
	
	static constexpr xAOD::Type::ObjectType object()
	{
		if(mixed()) return xAOD::Type::Other;
		if(tt&TT_ELECTRON_FLAG) return xAOD::Type::Electron;
		if(tt&TT_MUON_FLAG) return xAOD::Type::Muon;
		if(tt&TT_PHOTON_FLAG) return xAOD::Type::Photon;
		return xAOD::Type::Other;
	}

	static constexpr xAOD::Type::ObjectType major_object()
	{
		if(tt==TT_2E_MU_SYM || tt==TT_2E_MU_ASYM) return xAOD::Type::Electron;
		if(tt==TT_E_2MU_SYM || tt==TT_E_2MU_ASYM) return xAOD::Type::Muon;
		return object();
	}
	
	static constexpr xAOD::Type::ObjectType minor_object()
	{
		if(tt==TT_2E_MU_SYM || tt==TT_2E_MU_ASYM) return xAOD::Type::Muon;
		if(tt==TT_E_2MU_SYM || tt==TT_E_2MU_ASYM) return xAOD::Type::Electron;
		return xAOD::Type::Other;
	}
	
	static bool relevantFor(const Lepton& lepton) { return lepton.type()==object(); }
	static bool irrelevantFor(const Lepton& lepton) { return !relevantFor(lepton); }
	
	static constexpr bool is1L(xAOD::Type::ObjectType obj = xAOD::Type::Other) { return (tt&TT_SINGLELEPTON_FLAG) && extraCheck(obj); }
	static constexpr bool is2Lasym(xAOD::Type::ObjectType obj = xAOD::Type::Other) { return ((tt&TT_MASK_TYPE)==TT_DILEPTON_ASYM) && extraCheck(obj); }
	static constexpr bool is2Lsym(xAOD::Type::ObjectType obj = xAOD::Type::Other) { return ((tt&TT_MASK_TYPE)==TT_DILEPTON_SYM) && extraCheck(obj); }
	static constexpr bool is2L(xAOD::Type::ObjectType obj = xAOD::Type::Other) { return (tt&TT_DILEPTON_FLAG) && !mixed() && extraCheck(obj); }
	static constexpr bool is3Lsym(xAOD::Type::ObjectType obj = xAOD::Type::Other) { return ((tt&TT_MASK_TYPE)==TT_TRILEPTON_SYM) && !mixed() && extraCheck(obj); }
	static constexpr bool is3Lhalfsym(xAOD::Type::ObjectType obj = xAOD::Type::Other) { return ((tt&TT_MASK_TYPE)==TT_TRILEPTON_HALFSYM) && !mixed() && extraCheck(obj); }
	static constexpr bool isEMU() { return tt==TT_EMU; }
	static constexpr bool is3Lmix() { return (tt&TT_TRILEPTON_FLAG) && mixed(); }
	
	using companionTrig1LType = typename std::conditional<object()==xAOD::Type::Electron, Trig1E, 
			typename std::conditional<object()==xAOD::Type::Muon, Trig1MU, 
			typename std::conditional<object()==xAOD::Type::Photon, Trig1PH, UnusedArg>::type>::type>::type;
	
	std::array<std::size_t, nDistinctLegs()> legs;
	
	explicit Trigger()
	{
		static_assert(!((tt&TT_PHOTON_FLAG)&&mixed()), "mixed photon- electron/muon triggers not yet implemented");
		std::fill(legs.begin(), legs.end(), 0);
	}
	
	bool set_definition(const ImportData::TrigDef& def)
	{
		if(def.type != tt) return false;
		if(tt == TT_2E_MU_SYM) // special case needed to skip the duplicated electron leg
		{
			legs[0] = def.leg[0];
			legs[1] = def.leg[2];
		}
		else // Works as well for non-mixed trilepton triggers since the asymmetric leg is always stored first
		{
			std::copy_n(def.leg.cbegin(), legs.size(), legs.begin());
		}
		return true;
	}
	template<bool dummy=true> typename std::enable_if<(nDistinctLegs()==1)&&dummy, std::size_t>::type operator()(void) const { return legs[0]; }
	std::size_t operator()(unsigned index) const { return legs[index]; }
	template<bool dummy = true> typename std::enable_if<is1L()&&dummy, std::size_t>::type operator<(const Trigger& rhs) const
	{
		return legs[0] < rhs.legs[0];
	}
	explicit operator bool() const { return std::all_of(legs.cbegin(), legs.cend(), [](const std::size_t& x)->bool{ return x; }); }
	bool operator==(const Trigger& rhs) const { return legs == rhs.legs; }
	
	template<xAOD::Type::ObjectType obj = object()>
	decltype(legs.cbegin()) cbegin() const
	{
		return legs.cbegin() + (obj!=xAOD::Type::Electron) *(nDistinctLegs(xAOD::Type::Electron)  
			+ (obj!=xAOD::Type::Muon)*nDistinctLegs(xAOD::Type::Muon));
	}
	
	template<xAOD::Type::ObjectType obj = object()>
	decltype(legs.cend()) cend() const
	{
		return legs.cend() - (obj!=xAOD::Type::Photon) *(nDistinctLegs(xAOD::Type::Photon)  
			+ (obj!=xAOD::Type::Muon)*nDistinctLegs(xAOD::Type::Muon));
	}

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
	
	// Returns a pseudo trigger built only from the legs of flavour 'obj'
	// If anti==true, uses instead only legs with flavours other than 'obj'
	template<xAOD::Type::ObjectType obj, bool anti=false> auto side() const
		-> typename std::enable_if<mixed(), typename std::conditional<anti ^ (CastType1::object()==obj), CastType1, CastType2>::type>::type
	{
		static_assert(obj != xAOD::Type::Other, "");
		using CastType = decltype(side<obj, anti>());
		CastType trig;
		std::copy(this->cbegin<CastType::object()>(), this->cend<CastType::object()>(), trig.legs.begin());
		return trig;
	}
	// Returns a pseudo trigger built only from the legs of the same flavour as the trigger 'TrigX'
	template<typename TrigX> auto side() const -> decltype(side<TrigX::object()>()) { return side<TrigX::object()>(); }
	// Complement to the previous function
	template<typename TrigX> auto antiside() const -> decltype(side<TrigX::object(),true>()) { return side<TrigX::object(),true>(); }

	template<typename Trig1L> auto addTo(const flat_set<Trig1L>& trigs1L) const
		-> typename std::enable_if<mixed()&&Trig1L::is1L()&&nDistinctLegs(Trig1L::object())==1, flat_set<Trig1L>>::type 
	{
		flat_set<Trig1L> trigs(trigs1L);
		trigs.insert(side<Trig1L>());
		return trigs;
	}
	
	template<typename Trig1L> static auto anonymize(const flat_set<Trig1L>& triggers)
		-> typename std::enable_if<is1L() && tt==Trig1L::type(), const flat_set<std::size_t>&>::type
	{
		static_assert(sizeof(Trig1L)==sizeof(std::size_t), "");
		return reinterpret_cast<const flat_set<std::size_t>&>(triggers);
	}
	
	template<bool dummy=true> auto symmetric() const
		-> typename std::enable_if<!std::is_same<CastType1, UnusedArg>::value && dummy, bool>::type
	{
		return std::all_of(legs.cbegin()+1, legs.cend(), [&](std::size_t l)->bool{ return l==legs[0]; });
	}

	template<bool dummy=true> auto to_symmetric() const
		-> typename std::enable_if<!std::is_same<CastType1, UnusedArg>::value&&dummy, CastType1>::type
	{
		CastType1 trig;
		trig.legs[0] = this->legs[0];
		return trig;
	}
	
	template<bool dummy=true> auto asymLeg() const
		-> typename std::enable_if<((tt&TT_MASK_SYMMETRY)==TT_HALFSYM)&&dummy, std::size_t>::type
	{
		return legs[0];
	}
	
	template<bool dummy=true> auto symLeg() const
		-> typename std::enable_if<((tt&TT_MASK_SYMMETRY)==TT_HALFSYM)&&dummy, std::size_t>::type
	{
		return legs[1];
	}
};

class Trig1E : public Trigger<TT_SINGLE_E> {};
class Trig1MU : public Trigger<TT_SINGLE_MU> {};
class Trig1PH : public Trigger<TT_SINGLE_PH> {};
class Trig2Esym : public Trigger<TT_2E_SYM> {};
class Trig2Easym : public Trigger<TT_2E_ASYM, Trig2Esym> {};
class Trig2MUsym : public Trigger<TT_2MU_SYM> {};
class Trig2MUasym : public Trigger<TT_2MU_ASYM, Trig2MUsym> {};
class TrigEMU : public Trigger<TT_EMU, Trig1E, Trig1MU> {};
class Trig2PHsym : public Trigger<TT_2PH_SYM> {};
class Trig2PHasym : public Trigger<TT_2PH_ASYM, Trig2PHsym> {};
class Trig3Esym : public Trigger<TT_3E_SYM> {};
class Trig3Ehalfsym : public Trigger<TT_3E_HALFSYM, Trig3Esym> {};
class Trig3MUsym : public Trigger<TT_3MU_SYM> {};
class Trig3MUhalfsym : public Trigger<TT_3MU_HALFSYM, Trig3MUsym> {};
class Trig2EMUsym : public Trigger<TT_2E_MU_SYM, Trig2Esym, Trig1MU> {};
class Trig2EMUasym : public Trigger<TT_2E_MU_ASYM, Trig2Easym, Trig1MU> {};
class TrigE2MUsym : public Trigger<TT_E_2MU_SYM, Trig1E, Trig2MUsym> {};
class TrigE2MUasym : public Trigger<TT_E_2MU_ASYM, Trig1E, Trig2MUasym> {};
class Trig3PHsym : public Trigger<TT_3PH_SYM> {};
class Trig3PHhalfsym : public Trigger<TT_3PH_HALFSYM, Trig3PHsym> {};

}


#endif
